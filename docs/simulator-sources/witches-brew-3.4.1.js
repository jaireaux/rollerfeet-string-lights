// Spatial adaptation of production Witch's Brew: pool/bubbles, then steam.
function renderFrame(context, pixels) {
  renderBackground(context, pixels);
  renderForeground(context, pixels);
}

function surfaceAt(x, simmer) {
  return 0.80 + 0.04 * Math.sin(Math.PI * 2 * x + simmer);
}

function soft(distance, width) {
  const a = Math.max(0, Math.min(1, 1 - distance / width));
  return a * a * (3 - 2 * a);
}

function renderBackground({time, count, positions}, pixels) {
  const tau = Math.PI * 2;
  const simmer = tau * (time % 15) / 15;
  for (let i = 0; i < count; i++) {
    const {x, y} = positions[i];
    const surface = surfaceAt(x, simmer);
    const pool = soft(y - surface, 0.07);
    let green = (35 + 25 * (0.5 + 0.5 * Math.sin(tau * (1.4 * x + y) - simmer))) * pool;
    let red = 0, blue = 0;
    for (let b = 0; b < 3; b++) {
      const period = 6.2 + 1.1 * b;
      const progress = ((time + b * 1.7) % period) / period;
      const cx = 0.18 + 0.30 * b + 0.07 * Math.sin(simmer + b);

const bubbleSurface = surfaceAt(cx, simmer);
const radius = 0.025 + 0.07 * progress;
const startY = 0.04;
const cy = startY + (bubbleSurface - radius - startY) * progress;

      // const cy = 0.04 + 0.40 * progress;
      // const radius = 0.025 + 0.07 * progress;
      const bubble = soft(Math.abs(Math.hypot(x - cx, y - cy) - radius), 0.055);
      red += 105 * bubble; blue += 140 * bubble;
      if (progress > 0.95) {
        const spark = soft(Math.abs(x - cx), 0.14) * soft(Math.abs(y - (surface + 0.08)), 0.10);
        red += 200 * spark; green += 65 * spark;
      }
    }
    pixels[i] = [Math.min(red, 255), Math.min(green, 255), Math.min(blue, 255)];
  }
}

function renderForeground({time, count, positions}, pixels) {
  const steamPhase = Math.PI * 2 * (time % 9) / 9;
  for (let i = 0; i < count; i++) {
    const {x, y} = positions[i];
    if (y <= 2 / 3) continue;
    const height = (y - 2 / 3) * 3;
    const center = 0.5 + 0.13 * Math.sin(steamPhase - 3 * height);
    const wisp = soft(Math.abs(x - center), 0.30) + 0.6 * soft(Math.abs(x - (center + 0.26)), 0.195);
    const white = 255 * (0.60 + 0.05 * Math.sin(steamPhase)) * Math.min(wisp, 1) * soft(1 - height, 1);
    pixels[i] = pixels[i].map(channel => Math.min(255, channel + white));
  }
}
