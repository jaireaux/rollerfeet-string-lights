// FALL PREVIEW ONLY — Red Slosh with a continuous autumn color cycle; original motion retained; shadow disabled but kept below.
function renderFrame(context, pixels) {
  const motionSpeed = 0.25; // Half the previous spiral/motion speed.
  const throbSpeed = 0.5; // Keep the current throb pace.
  context = { ...context, colorTime: context.time, throbTime: context.time * throbSpeed,
    time: context.time * motionSpeed, delta: context.delta * motionSpeed };
  renderBackground(context, pixels);
  // Optional 22-second shadow; uncomment to restore.
  // renderForeground(context, pixels);
}

function renderBackground({time, colorTime = time, throbTime = time, count, positions}, pixels) {
  // Color timing is independent of motion and throb.
  const colorCycleSeconds = 20;
  const palette = [
    [190, 42, 35], // cranberry red
    [190, 75, 22], // burnt orange
    [205, 160, 45], // golden yellow
    [110, 125, 42], // olive
    [145, 68, 30], // russet: a brighter brown-like bridge
    [170, 48, 30] // brick red, before returning to cranberry
  ];
  const secondsPerColor = colorCycleSeconds / palette.length;
  const colorPhase = colorTime / secondsPerColor;
  const colorIndex = Math.floor(colorPhase) % palette.length;
  const fraction = colorPhase % 1;
  const blend = fraction * fraction * (3 - 2 * fraction);
  const from = palette[colorIndex], to = palette[(colorIndex + 1) % palette.length];
  const mixed = from.map((channel, c) => channel + (to[c] - channel) * blend);
  // Keep RGB peak intensity constant; perceived brightness still depends on hue.
  const peakBrightness = 190;
  const peak = Math.max(...mixed);
  const color = mixed.map(channel => channel * peakBrightness / peak);
  const tau = Math.PI * 2;
  const angle = tau * (time % 1);
  const cosine = Math.cos(angle), sine = Math.sin(angle);
  const xShift = (time % 6) / 6;
  const yShift = 0.5 * (1 - Math.cos(tau * (time % 2) / 2));
  const wrap = value => ((value % 1) + 1) % 1;
  const levels = Array.from({length:4}, (_, q) => {
    const phase = wrap(throbTime / 3 - q / 4);
    return 0.30 + 0.70 * (1 - Math.abs(2 * phase - 1));
  });
  for (let i = 0; i < count; i++) {
    const point = positions[i];
    const dx = point.x - 0.5, dy = point.y - 0.5;
    const x = wrap(0.5 + dx * cosine - dy * sine + xShift);
    const y = wrap(0.5 + dx * sine + dy * cosine + yShift);
    const quadrant = y < 0.5 ? (x < 0.5 ? 0 : 3) : (x < 0.5 ? 1 : 2);
    const level = levels[quadrant];
    pixels[i] = color.map(channel => channel * level);
  }
}

function shadowCenter(time) {
  const t = time % 22;
  if (t < 7) return -0.1;
  if (t < 8) return -0.1 + 1.2 * (t - 7);
  if (t < 11) return 1.1;
  if (t < 12) return 1.1 - 1.2 * (t - 11);
  if (t < 15) return -0.1;
  if (t < 15.5) return -0.1 + 1.2 * (t - 15);
  if (t < 16.5) return 0.5;
  if (t < 17) return 0.5 - 1.2 * (t - 16.5);
  if (t < 20) return -0.1;
  if (t < 20.5) return -0.1 + 1.2 * (t - 20);
  if (t < 21.5) return 0.5;
  return 0.5 + 1.2 * (t - 21.5);
}

function renderForeground({time, count, positions}, pixels) {
  const center = shadowCenter(time);
  for (let i = 0; i < count; i++) {
    const distance = Math.abs(positions[i].x - center);
    if (distance >= 0.10) continue;
    const edge = Math.max(0, (distance - 0.07) / 0.03);
    const light = edge * edge * (3 - 2 * edge);
    pixels[i] = pixels[i].map(channel => channel * light);
  }
}
