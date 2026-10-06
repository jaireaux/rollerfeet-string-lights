#!/usr/bin/env python3
"""Exercise the real PHP relay using a temporary private store and HTTP requests."""
import base64,json,os,shutil,socket,subprocess,tempfile,time,urllib.request,urllib.error,urllib.parse
from pathlib import Path
root=Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix='holiday-relay-test-') as directory:
 p=Path(directory);(p/'private').mkdir();shutil.copy(root/'remote/api.php',p/'api.php')
 hashed=subprocess.check_output(['php','-r','echo password_hash(stream_get_contents(STDIN), PASSWORD_DEFAULT);'],input=b'test-only-password').decode()
 (p/'private/config.php').write_text("<?php return ['password_hash'=>'"+hashed+"','bridge_token'=>'test-only-bridge'];")
 with socket.socket() as s:s.bind(('127.0.0.1',0));port=s.getsockname()[1]
 server=subprocess.Popen(['php','-S',f'127.0.0.1:{port}','-t',str(p)],stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
 base=f'http://127.0.0.1:{port}/api.php?action='
 auth='Basic '+base64.b64encode(b'admin:test-only-password').decode()
 def call(action,body=None,credential=auth,origin=None):
  headers={'Authorization':credential,'X-Holiday-Control':'1'}
  if origin:headers['Origin']=origin
  if body is None:data=None
  elif action=='bridge':data=json.dumps(body).encode();headers['Content-Type']='application/json'
  else:
   data=urllib.parse.urlencode(body).encode();headers['Content-Type']='application/x-www-form-urlencoded'
  try:
   with urllib.request.urlopen(urllib.request.Request(base+action,data=data,headers=headers)) as r:return r.status,json.loads(r.read())
  except urllib.error.HTTPError as e:return e.code,json.loads(e.read())
 try:
  for _ in range(50):
   try:call('state');break
   except OSError:time.sleep(.05)
  assert call('state',credential='bad')[0]==401
  assert call('bridge',{},credential=auth)[0]==401
  assert call('state')[1]['online'] is False
  assert call('control',{'action':'power','value':0})[0]==409
  state={'version':'3.0.0-web.1','pixels':600,'power':True,'brightness':100,'auto':True,'playlist':'preview','duration':30,'animation':2,'updating':False}
  def report(state=state,ack=None):return call('bridge',{'state':state,'ack':ack},'Bearer test-only-bridge')
  assert report()[0]==200 and call('state')[1]['online']
  for name,value in [('duration',9),('duration',601),('animation',5),('brightness',101),('power',2),('next',0),('unknown',0),('power','-1')]:assert call('control',{'action':name,'value':value})[0]==400
  assert call('control',{'action':'power','value':0},origin='https://evil.invalid')[0]==403
  code,command=call('control',{'action':'next','value':1});assert code==202
  assert call('control',{'action':'power','value':0})[0]==409
  claimed=report()[1]['command'];assert claimed['id']==command['id'] and claimed['action']=='next'
  assert report()[1]['command'] is None # Never executes a claimed command twice.
  assert report(ack={'id':'wrong-id','ok':True})[0]==200
  assert call('state')[1]['command']['status']=='applying'
  report(ack={'id':command['id'],'ok':True});assert call('state')[1]['command']['status']=='done'
  assert call('control',{'action':'brightness','value':25})[0]==202
  f=p/'private/state.json';d=json.loads(f.read_text());d['command']['created']-=20;f.write_text(json.dumps(d))
  assert report()[1]['command'] is None and call('state')[1]['command']['status']=='expired'
  assert call('control',{'action':'next','value':1})[0]==202
  report();d=json.loads(f.read_text());d['command']['created']-=20;f.write_text(json.dumps(d))
  assert call('state')[1]['command']['status']=='uncertain'
  report(None);assert not call('state')[1]['online']
  report(dict(state,updating=True));assert call('control',{'action':'power','value':0})[0]==409
  report(dict(state,private_secret='must-not-leak'));assert 'private_secret' not in call('state')[1]['state']
  print('PASS: relay auth, bounded settings, offline/update protection, single claim, expiry, acknowledgements, state filtering')
 finally:server.terminate();server.wait(timeout=5)
