void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, bool meteorRandomDecay, int SpeedDelay) {

  unsigned long redgreenAnimationDuration = millis() - currentAnimationDuration;
  unsigned long redgreenAnimationCycleLength = 1000; // how many milliseconds to switch colors
  unsigned long redgreenAnimationCycle = millis() - animationsCycleStartTime;

  // setAll(0, 0, 0);
  fill_solid( leds, NUM_LEDS, CRGB( 0, 0, 0) );

// void loop() {
//   meteorRain(0xff, 0x00, 0x00, 4, 22, true, 8);
// }

  for (int i = 0; i < NUM_LEDS + NUM_LEDS; i++) {

    // fade brightness all LEDs one step
    for (int j = 0; j < NUM_LEDS; j++) {
      if ((!meteorRandomDecay) || (random(10) > 5)) {
        // fadeToBlack(j, meteorTrailDecay);
        leds[j].fadeToBlackBy( meteorTrailDecay );
        // leds[ledNo].fadeToBlackBy( fadeValue );
      }
    }

    // draw meteor
    for (int j = 0; j < meteorSize; j++) {
      if ((i - j < NUM_LEDS) && (i - j >= 0)) {
        // setPixel(i - j, red, green, blue);
        leds[i-j] = CRGB(red, green, blue);
        // leds[Pixel] = CRGB(red, green, blue);
      }
    }

    // Serial.print("redgreenAnimationDuration=" + String(redgreenAnimationDuration));
    // Serial.print("; currentAnimationDurationMax=" + String(currentAnimationDurationMax));
    // Serial.println("; currentAnimationNumber=" + String(currentAnimationNumber));
    // Serial.println("redgreenAnimationCycle=" + String(redgreenAnimationCycle));
    setSkip();
    FastLED.show(); // display this frame
    delay(SpeedDelay);

  }

    if( redgreenAnimationDuration < currentAnimationDurationMax ) {
      setSkip();
      FastLED.show(); // display this frame
      animationsCycleStartTime = millis();
    } else {
      Serial.println("current animation=" + String(currentAnimationNumber));      
      currentAnimationNumber = currentAnimationNumber + 1;
      currentAnimationDuration = millis();
      animationsCycleStartTime = millis();
    }
}

// void fadeToBlack(int ledNo, byte fadeValue) {
//   #ifdef ADAFRUIT_NEOPIXEL_H
//   // NeoPixel
//   uint32_t oldColor;
//   uint8_t r, g, b;
//   int value;

//   oldColor = strip.getPixelColor(ledNo);
//   r = (oldColor & 0x00ff0000 UL) >> 16;
//   g = (oldColor & 0x0000ff00 UL) >> 8;
//   b = (oldColor & 0x000000ff UL);

//   r = (r <= 10) ? 0 : (int) r - (r * fadeValue / 250);
//   g = (g <= 10) ? 0 : (int) g - (g * fadeValue / 250);
//   b = (b <= 10) ? 0 : (int) b - (b * fadeValue / 250);

//   strip.setPixelColor(ledNo, r, g, b);
//   #endif
//   #ifndef ADAFRUIT_NEOPIXEL_H
//   // FastLED
//   leds[ledNo].fadeToBlackBy(fadeValue);
//   #endif
// }
// -----------------------------------------
// void showStrip() {
//   #ifdef ADAFRUIT_NEOPIXEL_H
//   // NeoPixel
//   strip.show();
//   #endif
//   #ifndef ADAFRUIT_NEOPIXEL_H
//   // FastLED
//   FastLED.show();
//   #endif
// }

// void setPixel(int Pixel, byte red, byte green, byte blue) {
//   #ifdef ADAFRUIT_NEOPIXEL_H
//   // NeoPixel
//   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
//   #endif
//   #ifndef ADAFRUIT_NEOPIXEL_H
//   // FastLED
//   leds[Pixel].r = red;
//   leds[Pixel].g = green;
//   leds[Pixel].b = blue;
//   #endif
// }

// void setAll(byte red, byte green, byte blue) {
//   for (int i = 0; i < NUM_LEDS; i++) {
//     // setPixel(i, red, green, blue);
//     leds[Pixel] = CRGB(red, green, blue);
//   }
//   // showStrip();
//   FastLED.show();
// }