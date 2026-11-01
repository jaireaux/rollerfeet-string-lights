// FALL PREVIEW ONLY — Meteor Rain: autumn meteors; production launch interval, head size, speed and fade.
// Time is seconds. Colors are RGB 0–255; edit these values to tune the palette.
function renderFrame({time, count, positions}, pixels) {
  const palette = [[190,75,22], [205,160,45], [150,42,35], [100,120,42], [190,115,35]];
  const launchInterval = 1.25;
  const pixelsPerSecond = 60;
  const headPixels = 10;
  const fadeSeconds = 0.8;
  const lifetime = (count+headPixels)/pixelsPerSecond+fadeSeconds;
  const launch = Math.floor(time/launchInterval);
  for (let i=0; i<count; i++) pixels[i] = [0,0,0];
  for (let n=Math.max(0,launch-Math.ceil(lifetime/launchInterval)); n<=launch; n++) {
    const head = (time-n*launchInterval)*pixelsPerSecond;
    const color = palette[n%palette.length];
    for (let i=0; i<count; i++) {
      const distance = head-i;
      const glow = distance<0 ? 0 : distance<headPixels ? 1 : Math.max(0,1-(distance-headPixels)/(fadeSeconds*pixelsPerSecond));
      for (let c=0;c<3;c++) pixels[i][c]=Math.max(pixels[i][c],color[c]*glow);
    }
  }
}
