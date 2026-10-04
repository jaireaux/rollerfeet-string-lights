void classicChristmas() {
  unsigned long classicChristmasAnimationCycleLength = 500; // how many milliseconds to switch colors
  unsigned long classicChristmasAnimationCycle = millis() - animationsCycleStartTime;
  unsigned long classicChristmasAnimationDuration = millis() - currentAnimationDuration;
 
  if( classicChristmasAnimationDuration < currentAnimationDurationMax ) {
    CRGB christmaslights [7] { CRGB::Red,
                              CRGB::Yellow,
                              CRGB::Blue,
                              CRGB::Magenta,
                              CRGB::Orange,
                              CRGB::Cyan,
                              CRGB::Green };
    int currentColor;
    currentColor=(genericCounter % 7);
                              
    if( classicChristmasAnimationCycle > classicChristmasAnimationCycleLength) {
      genericCounter=genericCounter+1;
      for(int j=0; j<NUM_LEDS; j++) {
  //      Serial.println("j=" + String(j) + ";gC=" + String(genericCounter) + ";cC=" + String(currentColor));
        rawleds[j] =  christmaslights [ currentColor ];
        currentColor = (currentColor < 6) ? currentColor+1 : 0;
      }
      setSkip();
      FastLED.show(); // display this frame
      animationsCycleStartTime = millis();
    }
  } else {
    currentAnimationNumber = currentAnimationNumber + 1;
    currentAnimationDuration = millis();
    animationsCycleStartTime = millis();
  }
}
