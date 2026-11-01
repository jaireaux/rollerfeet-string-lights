// FALL PREVIEW ONLY — Haunted Tide: slow blended olive, amber and cranberry waves.
// Time is seconds. Colors are RGB 0–255; edit these values to tune the palette.
function renderFrame({time, count, positions}, pixels) {
  const palette = [[100,120,42], [190,115,35], [150,42,35], [205,160,45]];
  const mix = (a,b,t)=>a.map((v,c)=>v+(b[c]-v)*t);
  for (let i=0; i<count; i++) {
    const x = i / Math.max(1,count-1);
    const waveA = (Math.sin(x*Math.PI*4-time)+1)/2;
    const waveB = (Math.sin(x*Math.PI*2.8+time*0.6)+1)/2;
    const color = mix(mix(palette[0],palette[1],waveA),mix(palette[2],palette[3],waveB),0.45);
    const brightness = 0.20 + 0.80 * (0.6*waveA+0.4*waveB);
    pixels[i] = color.map(v=>v*brightness);
  }
}
