// Exercise browser-independent UI behavior; this does not verify visual layout.
const assert = require('node:assert/strict');
const vm = require('node:vm');
const fs = require('node:fs');
const path = require('node:path');
const html = fs.readFileSync(path.join(__dirname,'../web/index.html'),'utf8');
const script = html.match(/<script>([\s\S]*?)<\/script>/)[1];
function element() { return {value:'',hidden:false,disabled:false,checked:false,children:[],attributes:{},classList:{toggle(){},remove(){}},setAttribute(k,v){this.attributes[k]=v;},appendChild(b){this.children.push(b);},checkValidity(){return true;},reportValidity(){}}; }
async function scenario(remote) {
 const elements={};for(const match of html.matchAll(/id="([^"]+)"/g))elements[match[1]]=element();
 const state={version:'test',pixels:600,power:true,brightness:100,auto:true,playlist:'preview',duration:30,animation:2,updating:false};
 let online=true,command=null,fail=false,poll,requests=0;
 const document={hidden:false,activeElement:null,getElementById:id=>elements[id],createElement:element,addEventListener(){}};
 const sandbox={document,location:{pathname:remote?'/lights/':'/'},AbortController,URLSearchParams,console,setTimeout,clearTimeout,setInterval:fn=>{poll=fn;},btoa:s=>Buffer.from(s).toString('base64'),fetch:async(url,options)=>{
  requests++;if(fail)throw new Error('Failed to fetch');
  if(options.headers.Authorization!=='Basic '+Buffer.from('admin:test-password').toString('base64'))return {ok:false,status:401,json:async()=>({error:'Wrong password'})};
  if(options.body){const body=new URLSearchParams(options.body);if(remote){command={id:'cmd-1',status:'queued'};return {ok:true,status:202,json:async()=>({id:command.id})};}state[body.get('action')]=Number(body.get('value'));}
  return {ok:true,status:200,json:async()=>remote?{online,state:{...state},command}:({...state})};
 }};
 vm.createContext(sandbox);vm.runInContext(script,sandbox);
 elements.password.value='test-password';await elements.loginForm.onsubmit({preventDefault(){}});
 assert.equal(elements.login.hidden,true);assert.equal(elements.app.hidden,false);assert.equal(elements.controls.disabled,false);assert.equal(elements.effects.children.length,5);assert.equal(elements.current.textContent,'Meteor Rain');
 elements.brightness.value='25';await elements.brightness.onchange();
 if(remote){assert.equal(elements.controls.disabled,true);command.status='done';state.brightness=25;await poll();}
 assert.equal(elements.brightnessValue.textContent,'25%');assert.equal(elements.controls.disabled,false);
 if(remote){online=false;await poll();assert.equal(elements.power.disabled,true);assert.equal(elements.connection.textContent,'Controller offline');assert.equal(elements.playingLabel.textContent,'Last known animation');online=true;}
 fail=true;await poll();assert.equal(elements.controls.disabled,true);assert.equal(elements.connection.textContent,'Connection lost');fail=false;await poll();assert.equal(elements.controls.disabled,false);
 const previous=requests;document.hidden=true;await poll();assert.equal(requests,previous);document.hidden=false;
 elements.logout.onclick();assert.equal(elements.login.hidden,false);assert.equal(elements.app.hidden,true);
 elements.password.value='incorrect';await elements.loginForm.onsubmit({preventDefault(){}});assert.equal(elements.loginError.textContent,'Wrong password');assert.equal(elements.app.hidden,true);
}
(async()=>{await scenario(false);await scenario(true);console.log('PASS: web login, controls, remote acknowledgements, offline/reconnect, hidden-tab polling, logout, invalid login');})().catch(error=>{console.error(error);process.exit(1);});
