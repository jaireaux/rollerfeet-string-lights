import {execFileSync} from 'node:child_process';
import {readFileSync,mkdtempSync,rmSync} from 'node:fs';
import vm from 'node:vm';import assert from 'node:assert/strict';
const root=new URL('../',import.meta.url).pathname,dir=mkdtempSync('/tmp/fall-parity-');
const names=['harvest-glow','autumn-tide','orchard-bands','falling-embers','cranberry-drift','harvest-cider'];
try{
 execFileSync('c++',['-std=c++11',root+'tests/fall_probe.cpp','-o',dir+'/probe']);
 const map=[...readFileSync(root+'XIAO_ESP32_holiday_lights/spatial_map.h','utf8').matchAll(/\{(\d+),\s*(\d+)\}/g)].map(m=>({x:+m[1]/65535,y:+m[2]/65535}));
 const points=map.slice(0,300);assert.equal(points.length,300);
 const lines=[],expected=[]; const sources=[];
 for(let effect=0;effect<6;effect++){
  const source=readFileSync(root+'docs/simulator-sources/fall-3.5.1/'+names[effect]+'.js','utf8');sources.push(source); const scope=vm.createContext({});vm.runInContext(source,scope);
  for(const count of [1,50,600])for(const ms of [0,1,500,1249,1250,2999,3000,6200,7999,10000,15000,19999,20000,20001,60000,179999,4294967295]){
   const positions=Array.from({length:count},(_,i)=>points[i%points.length]);scope.context={time:ms/1000,delta:1/60,count,positions};scope.pixels=positions.map(()=>[0,0,0]);
   vm.runInContext('renderFrame(context,pixels)',scope);
   for(let i=0;i<count;i++){lines.push(`${effect} ${ms} ${i} ${count} ${positions[i].x} ${positions[i].y}`);expected.push(Array.from(scope.pixels[i],v=>Math.round(Math.max(0,Math.min(255,v)))));}
  }
 }
 const actual=execFileSync(dir+'/probe',{input:lines.join('\n')+'\n',encoding:'utf8',maxBuffer:20e6}).trim().split('\n').map(s=>s.split(' ').map(Number));let max=0,boundaries=0;
 for(let i=0;i<actual.length;i++){
  const errors=actual[i].map((v,c)=>Math.abs(v-expected[i][c]));
  if(Math.max(...errors)<=1){max=Math.max(max,...errors);continue;}
  // Exact quadrant boundaries can fall on opposite sides by a floating-point ulp.
  // Accept only if a microscopic coordinate perturbation reproduces the C++ color.
  const [effect,ms,index,count,x,y]=lines[i].split(' ').map(Number);let adjacent=false;
  if(effect===4)for(const dx of [-1e-12,0,1e-12])for(const dy of [-1e-12,0,1e-12]){
   const scope=vm.createContext({context:{time:ms/1000,count:1,positions:[{x:x+dx,y:y+dy}]},pixels:[[0,0,0]]});vm.runInContext(sources[4],scope);vm.runInContext('renderFrame(context,pixels)',scope);
   if(Array.from(scope.pixels[0]).every((v,c)=>Math.abs(Math.round(v)-actual[i][c])<=1))adjacent=true;
  }
  assert.ok(adjacent,`${lines[i]}: ${actual[i]} != ${expected[i]}`);boundaries++;
 }
 console.log(`PASS ${actual.length} JS/C++ fall samples; maximum RGB error ${max} away from ${boundaries} verified quadrant-boundary samples`);
}finally{rmSync(dir,{recursive:true,force:true});}
