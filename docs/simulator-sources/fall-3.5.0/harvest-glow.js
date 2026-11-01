// FALL PREVIEW ONLY — Throb: one complete rise and fall per autumn color.
// Time is seconds. Colors are RGB 0–255; edit these values to tune the palette.
function renderFrame({time, count, positions}, pixels) {
  const palette = [[190,75,8], [100,120,12], [205,160,18], [170,48,4], [190,115,12]];
  const secondsPerColor = 3;
  const phase = time / secondsPerColor;
  const color = palette[Math.floor(phase) % palette.length];
  const brightness = (1 - Math.cos((phase % 1) * Math.PI * 2)) / 2;
  for (let i=0; i<count; i++) pixels[i] = color.map(v=>v*brightness);
}
