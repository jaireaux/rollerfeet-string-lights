<?php
declare(strict_types=1);
// Private storage is denied by private/.htaccess; config.php emits no text.
umask(0077);
header('Content-Type: application/json');
header('Cache-Control: no-store');
header('X-Content-Type-Options: nosniff');
function fail(int $code, string $message): void { http_response_code($code); echo json_encode(['error'=>$message]); exit; }
set_exception_handler(function (Throwable $e): void { error_log('Holiday lights relay: '.get_class($e)); fail(503,'The lights service is temporarily unavailable.'); });
$configFile=__DIR__.'/private/config.php';
if (!is_file($configFile)) fail(503,'The lights service needs its private configuration.');
$config=require $configFile;
$action=$_GET['action']??'';
$authorization=$_SERVER['HTTP_AUTHORIZATION']??$_SERVER['REDIRECT_HTTP_AUTHORIZATION']??'';
$bridge=$action==='bridge';
$public=in_array($action,['public-state','public-control'],true);
if ($bridge) {
    if (!hash_equals('Bearer '.$config['bridge_token'], $authorization)) fail(401,'Authentication required.');
} elseif (!$public) {
    $credentials=str_starts_with($authorization,'Basic ')?base64_decode(substr($authorization,6),true):false;
    if (!$credentials && isset($_SERVER['PHP_AUTH_USER'])) $credentials=$_SERVER['PHP_AUTH_USER'].':'.($_SERVER['PHP_AUTH_PW']??'');
    if (!is_string($credentials) || !str_starts_with($credentials,'admin:') || !password_verify(substr($credentials,6),$config['password_hash'])) fail(401,'Enter your lights web password.');
}
if (!in_array($action,['state','public-state'],true)) {
    if ($_SERVER['REQUEST_METHOD']!=='POST') fail(405,'Use POST for this request.');
    if (!$bridge) {
        if (($_SERVER['HTTP_X_HOLIDAY_CONTROL']??'')!=='1') fail(403,'Use the lights control page.');
        $origin=$_SERVER['HTTP_ORIGIN']??'';
        if ($origin!=='' && $origin!=='https://'.($_SERVER['HTTP_HOST']??'')) fail(403,'This page is not allowed to control the lights.');
    }
}
$limits=['power'=>[0,1],'brightness'=>[0,100],'animation'=>[0,5],'auto'=>[0,1],'playlist'=>[0,1],'duration'=>[10,600],'next'=>[1,1]];
if (in_array($action,['control','public-control'],true)) {
    $name=$_POST['action']??''; $value=$_POST['value']??'';
    if (!is_string($name) || !isset($limits[$name]) || !is_string($value) || !preg_match('/^\d{1,3}$/D',$value)) fail(400,'Invalid light setting.');
    if ($public && !in_array($name,['animation','next'],true)) fail(403,'Admin access required for this setting.');
    $value=(int)$value;
    if ($value<$limits[$name][0] || $value>$limits[$name][1]) fail(400,'Invalid light setting.');
} elseif ($bridge) {
    if ((int)($_SERVER['CONTENT_LENGTH']??0)>4096) fail(413,'Request too large.');
    try { $input=json_decode(file_get_contents('php://input'),true,16,JSON_THROW_ON_ERROR); }
    catch (JsonException $e) { fail(400,'Invalid relay report.'); }
    if (!is_array($input)) fail(400,'Invalid relay report.');
} elseif (!in_array($action,['state','public-state'],true)) fail(404,'Unknown request.');

$handle=fopen(__DIR__.'/private/state.lock','c');
if (!$handle || !flock($handle,LOCK_EX)) fail(503,'The lights service is busy.');
$stateFile=__DIR__.'/private/state.json';
$raw=is_file($stateFile)?file_get_contents($stateFile):'';
$data=$raw?json_decode($raw,true,16,JSON_THROW_ON_ERROR):['state'=>null,'seen'=>0,'command'=>null];
$now=time();
$cmd=$data['command'];
if ($cmd && in_array($cmd['status'],['queued','applying'],true) && $now-$cmd['created']>15) {
    $data['command']['status']=$cmd['status']==='queued'?'expired':'uncertain';
    $data['command']['message']=$cmd['status']==='queued'?'Controller did not receive the setting.':'Could not confirm the setting. Check the display before trying again.';
}
$status=200;
if ($bridge) {
    // Only the controller's bounded, non-secret state is allowed onto the website.
    $s=$input['state']??null;
    $valid=is_array($s) && isset($s['version'],$s['pixels'],$s['power'],$s['brightness'],$s['auto'],$s['playlist'],$s['duration'],$s['animation'],$s['updating'])
        && is_string($s['version']) && preg_match('/^[a-zA-Z0-9.\-]{1,40}$/D',$s['version'])
        && is_int($s['pixels']) && $s['pixels']>0 && $s['pixels']<=5000
        && is_bool($s['power']) && is_int($s['brightness']) && $s['brightness']>=0 && $s['brightness']<=100
        && is_bool($s['auto']) && in_array($s['playlist'],['all','preview'],true)
        && is_int($s['duration']) && $s['duration']>=10 && $s['duration']<=600
        && is_int($s['animation']) && $s['animation']>=0 && $s['animation']<=5 && is_bool($s['updating']);
    if ($valid) {
        $data['state']=array_intersect_key($s,array_flip(['version','pixels','power','brightness','auto','playlist','duration','animation','updating','remaining_ms']));
        if (isset($s['remaining_ms']) && (!is_int($s['remaining_ms']) || $s['remaining_ms']<0 || $s['remaining_ms']>600000)) unset($data['state']['remaining_ms']);
        $data['seen']=$now;
    } else $data['seen']=0;
    $ack=$input['ack']??null;
    if (is_array($ack) && $data['command'] && ($ack['id']??'')===$data['command']['id']
        && in_array($data['command']['status'],['applying','uncertain'],true)) {
        $done=($ack['ok']??false)===true;
        $data['command']['status']=$done?'done':'failed';
        $data['command']['message']=$done?'Setting applied.':'Controller could not confirm the setting. Check the display before trying again.';
    }
    $out=['command'=>null];
    if ($valid && !$s['updating'] && $data['command'] && $data['command']['status']==='queued') {
        $data['command']['status']='applying';
        $out['command']=$data['command']; // Claim once; never replay a next-animation command.
    }
} elseif (in_array($action,['control','public-control'],true)) {
    if (!$data['state'] || $now-$data['seen']>15) { $status=409; $out=['error'=>'Controller is offline. No setting was queued.']; }
    elseif ($data['state']['updating']) { $status=409; $out=['error'=>'A firmware update is in progress.']; }
    elseif ($data['command'] && in_array($data['command']['status'],['queued','applying'],true)) { $status=409; $out=['error'=>'Wait for the previous setting to finish.']; }
    else {
        $data['command']=['id'=>bin2hex(random_bytes(12)),'action'=>$name,'value'=>$value,'created'=>$now,'status'=>'queued'];
        $status=202; $out=['id'=>$data['command']['id'],'status'=>'queued'];
    }
} else $out=['online'=>$data['state']!==null && $now-$data['seen']<=15,'state'=>$data['state'],'command'=>$data['command']];
$encoded=json_encode($data,JSON_THROW_ON_ERROR);
$temporary=tempnam(__DIR__.'/private','state-');
if (!$temporary || file_put_contents($temporary,$encoded)!==strlen($encoded) || !rename($temporary,$stateFile)) throw new RuntimeException('Storage write failed');
flock($handle,LOCK_UN);fclose($handle);
if ($public && isset($out['state']) && is_array($out['state'])) $out['state']=array_intersect_key($out['state'],array_flip(['auto','playlist','duration','animation','updating','remaining_ms']));
if ($public && isset($out['command']) && is_array($out['command'])) $out['command']=array_intersect_key($out['command'],array_flip(['id','status','message']));
if (isset($out['state']['remaining_ms'])) $out['state']['remaining_ms']=max(0,$out['state']['remaining_ms']-max(0,$now-$data['seen'])*1000);
http_response_code($status);echo json_encode($out,JSON_THROW_ON_ERROR);
