void metaballs() {
  unsigned long metaballsAnimationCycleLength = 500; // how many milliseconds to switch colors
  unsigned long metaballsAnimationCycle = millis() - animationsCycleStartTime;
  unsigned long metaballsAnimationDuration = millis() - currentAnimationDuration;

  float speed = 1;

  // get some 2 random moving points
  uint8_t x2 = inoise8(millis() * speed, 25355, 685 ) / 16;
  uint8_t y2 = inoise8(millis() * speed, 355, 11685 ) / 16;

  uint8_t x3 = inoise8(millis() * speed, 55355, 6685 ) / 16;
  uint8_t y3 = inoise8(millis() * speed, 25355, 22685 ) / 16;

  // and one Lissajou function
  uint8_t x1 = beatsin8(23 * speed, 0, 15);
  uint8_t y1 = beatsin8(28 * speed, 0, 15);

  for (uint8_t y = 0; y < Width; y++) {
    for (uint8_t x = 0; x < Height; x++) {

      // calculate distances of the 3 points from actual pixel
      // and add them together with weightening
      uint8_t  dx =  abs(x - x1);
      uint8_t  dy =  abs(y - y1);
      uint8_t dist = 2 * sqrt((dx * dx) + (dy * dy));

      dx =  abs(x - x2);
      dy =  abs(y - y2);
      dist += sqrt((dx * dx) + (dy * dy));

      dx =  abs(x - x3);
      dy =  abs(y - y3);
      dist += sqrt((dx * dx) + (dy * dy));

      // inverse result
      byte color = 1000 / dist;

      // map color between thresholds
      if (color > 0 and color < 60) {
        rawleds[XY(x, y)] = CHSV(color * 9, 255, 255);
      } else {
        rawleds[XY(x, y)] = CHSV(0, 255, 255);
      }
       // show the 3 points, too
        rawleds[XY(x1,y1)] = CRGB(255, 255,255);
        rawleds[XY(x2,y2)] = CRGB(255, 255,255);
        rawleds[XY(x3,y3)] = CRGB(255, 255,255);
    }
  }

  if( metaballsAnimationDuration < currentAnimationDurationMax ) {
    setSkip();
    FastLED.show();
    animationsCycleStartTime = millis();
    //CLS();
  } else {
    currentAnimationNumber = currentAnimationNumber + 1;
    currentAnimationDuration = millis();
    animationsCycleStartTime = millis();
  }

}

void CLS() {
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    rawleds[i] = 0x000000;
  }
}

// this finds the right index within a serpentine matrix
uint16_t XY( uint8_t x, uint8_t y) {
  uint16_t i;
  if ( y & 0x01) {
    uint8_t reverseX = (Width - 1) - x;
    i = (y * Width) + reverseX;
  } else {
    i = (y * Width) + x;
  }

  i = i%NUM_LEDS;

  return i;
}