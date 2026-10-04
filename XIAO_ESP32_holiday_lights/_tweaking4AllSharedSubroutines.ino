void setPixel(int Pixel, byte red, byte green, byte blue) {
  // FastLED
  rawleds[Pixel].r = red;
  rawleds[Pixel].g = green;
  rawleds[Pixel].b = blue;
}

void setAll(byte red, byte green, byte blue) {
  // for (int i = 0; i < NUM_LEDS_PER_SECTION; i++ ) {
  //   setPixelMeta(i, red, green, blue);
  // }
  fill_solid( leds, NUM_LEDS, CRGB( red, green, blue) );
  FastLED.show();
}

// void fadeToBlack(int ledNo, byte fadeValue) {
//   // FastLED
//   rawleds[ledNo].fadeToBlackBy( fadeValue );
// }

// void fadeToBlackMeta(int ledNo, byte fadeValue) {
//   // FastLED
//   int actualPixel;
//   for (int j = 0; j < PIXELSECTIONS; j++) {
//     actualPixel = ledNo + j * NUM_LEDS_PER_SECTION;
//     rawleds[actualPixel].fadeToBlackBy( fadeValue );
//   }
// }

void setPixelMeta(int i, byte red, byte green, byte blue) {
  int actualPixel;
  for (int j = 0; j < PIXELSECTIONS; j++) {
    actualPixel = i + j * PIXELSPERSECTION;
    setPixel( actualPixel, red, green, blue);
  }
}

void setSkip() {
  for (int i = 210; i < NUM_LEDS-48; i++) {
    // setPixel(i, red, green, blue);
    setPixel(i, 0, 0, 0); // leds[i] = CRGB::Black;
  }
}