void peppermint() {
  unsigned long peppermintAnimationCycleLength = 500; // how many milliseconds to switch colors
  unsigned long peppermintAnimationCycle = millis() - animationsCycleStartTime;
  unsigned long peppermintAnimationDuration = millis() - currentAnimationDuration;

  int startPosition = 0;
  int whichColor = 0;
  int placeOnString;
  int colorCycle;

  if( peppermintAnimationDuration < currentAnimationDurationMax ) {
    if( peppermintAnimationCycle > peppermintAnimationCycleLength) {
      for(int j=0; j<NUM_LEDS; j++) {
        placeOnString = (j%17);
        if( placeOnString < 5) {
          if( placeOnString%2==0) rawleds[j]=CRGB::White;
          else rawleds[j]=CRGB::Red;
        } else {
          colorCycle=(placeOnString+genericCounter)%4;
          if(colorCycle<2) rawleds[j]=CRGB::White;
          else rawleds[j]=CRGB::Red;
        }
        if(j==NUM_LEDS-1) {
  //        if(j<34) Serial.println("j=" + String(j) + ";pOS=" + String(placeOnString) + ";gC=" + String(genericCounter) + ";color=" + String(colorCycle));
          if(genericCounter<3) genericCounter=genericCounter+1;
          else genericCounter=0;
        }
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
