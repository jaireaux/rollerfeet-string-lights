import {execFileSync} from 'node:child_process';
import {readFileSync,mkdtempSync,rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import vm from 'node:vm';
import assert from 'node:assert/strict';
const root=new URL('../',import.meta.url).pathname;
const dir=mkdtempSync(join(tmpdir(),'brew-parity-'));
try {
  const binary=join(dir,'probe');
  execFileSync(process.env.CXX||'c++',['-std=c++11',root+'tests/witches_brew_probe.cpp','-o',binary]);
  const c=vm.createContext({});vm.runInContext(readFileSync(root+'docs/simulator-sources/witches-brew-3.4.2.js','utf8'),c,{timeout:100});
  const points=[];
  for(let row=0;row<=16;row++)for(let col=0;col<=20;col++)points.push({x:col/20,y:row/16});
  const cases=[];const expected=[];
  for(const ms of [0,999,1500,5889,5891,6199,6200,7090,9000,15000,30000,123456,4294967295]) {
    c.context={time:ms/1000,count:points.length,positions:points};c.pixels=points.map(()=>[0,0,0]);
    vm.runInContext('renderFrame(context,pixels)',c,{timeout:100});
    points.forEach((p,i)=>{cases.push(`${ms} ${p.x} ${p.y}`);expected.push(Array.from(c.pixels[i],v=>Math.round(Math.min(255,Math.max(0,v)))));});
  }
  const output=execFileSync(binary,{input:cases.join('\n')+'\n',encoding:'utf8'}).trim().split('\n').map(s=>s.split(' ').map(Number));
  assert.equal(output.length,expected.length);
  let maxError=0;
  for(let i=0;i<output.length;i++)for(let channel=0;channel<3;channel++){
    const error=Math.abs(output[i][channel]-expected[i][channel]);maxError=Math.max(maxError,error);
    assert.ok(error<=1,`${cases[i]} channel ${channel}: C++ ${output[i][channel]}, JS ${expected[i][channel]}`);
  }
  console.log(`${output.length} frame/point samples match submitted JavaScript within ${maxError} RGB level(s).`);
} finally {rmSync(dir,{recursive:true,force:true});}
