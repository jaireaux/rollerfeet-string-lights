// FALL PREVIEW ONLY — Orange / Purple: drifting muted orange and golden-yellow bands.
// Time is seconds. Colors are RGB 0–255; edit these values to tune the palette.
function renderFrame({time, count, positions}, pixels) {
  const orange = [190,75,22];
  const gold = [205,160,45];
  const bandWidth = 8;
  const pixelsPerSecond = 6;
  for (let i=0; i<count; i++) pixels[i] = [...(Math.floor((i+time*pixelsPerSecond)/bandWidth)%2 ? gold : orange)];
}
