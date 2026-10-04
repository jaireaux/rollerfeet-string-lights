void throb () {
  unsigned long throbAnimationCycleLength = 50; // how many milliseconds to change brightness
  unsigned long throbAnimationCycle = millis() - animationsCycleStartTime;
  unsigned long throbAnimationDuration = millis() - currentAnimationDuration;

  // throb across a timewindow in milliseconds
  unsigned short throbAnimationPartialCycle = 3000;

  // unsigned long currentAnimationDurationMax = 10000;
  unsigned long currentAnimationDurationHalf = currentAnimationDurationMax / 2; // 5000 
  unsigned long throbAnimationMinBrightness = 10;
  unsigned long throbAnimationMaxBrightness = 200;
  unsigned long nextBrightness = 0;

  if( throbAnimationDuration < currentAnimationDurationMax ) {
    if( throbAnimationCycle > throbAnimationCycleLength) {
      // Serial.println("red=" + String(rawleds[0].r));
      // leds=CRGB::DarkOrange;
      unsigned short nextBrightness = throbAnimationMinBrightness;
      // throb across the entire cycle
      // unsigned short throbDelta = (
      //   ((float)throbAnimationDuration/(float)currentAnimationDurationMax) * 
      //   (throbAnimationMaxBrightness-throbAnimationMinBrightness)
      //   ) ;

      float modCalculation = (float)(throbAnimationDuration % throbAnimationPartialCycle);
      unsigned short throbDeltaX = ( 
        (modCalculation / throbAnimationPartialCycle ) *
        (throbAnimationMaxBrightness-throbAnimationMinBrightness)
      );

      // Serial.println("mC=" + String(modCalculation) + ";mc/2.0=" + String(modCalculation/2.0) + ";mc/2=" + String(int(modCalculation/2)));
      // if( modCalculation < throbAnimationPartialCycle ) leds=CRGB::DarkGreen;
      // else leds=CRGB::Red;

      // if(throbAnimationDuration < currentAnimationDurationHalf) {
      // if(throbAnimationDuration < (throbAnimationPartialCycle)) {
      if( modCalculation < throbAnimationPartialCycle/2) {
        nextBrightness = throbAnimationMinBrightness;
        nextBrightness = throbAnimationMinBrightness + throbDeltaX;
        leds=CRGB::DarkRed;
      } else {
        nextBrightness = throbAnimationMaxBrightness;
        nextBrightness = throbAnimationMaxBrightness - throbDeltaX;
        leds=CRGB::DarkGreen;
      }
      // nextBrightness = nextBrightness * 2; // I can't figure out why my math is off some I'm doubling this
      nextBrightness = nextBrightness > throbAnimationMaxBrightness ? throbAnimationMaxBrightness : nextBrightness;
      nextBrightness = nextBrightness < throbAnimationMinBrightness ? throbAnimationMinBrightness : nextBrightness;

      // if(throbAnimationDuration < currentAnimationDurationHalf) nextBrightness = throbAnimationMinBrightness + throbDelta;
      // else nextBrightness = throbAnimationMaxBrightness - (((float)throbAnimationDuration/(float)currentAnimationDurationMax) * (throbAnimationMaxBrightness-throbAnimationMinBrightness));

      // Serial.println("nextBrightness=" + String(nextBrightness) 
      //   + ";throbAnimationDuration=" + String(throbAnimationDuration) 
      //   + ";modCalculation=" + String((throbAnimationDuration % throbAnimationPartialCycle))
      //   + ";throbDeltaX=" + String( throbDeltaX) );

      FastLED.setBrightness( nextBrightness );
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


// unsigned long uptime = millis();
// int currentAnimationNumber = 1;
// unsigned long currentAnimationDuration = millis();
// unsigned long currentAnimationDurationMax = 10000;
// unsigned long animationsCycleStartTime = millis();
// int genericCounter=0;
