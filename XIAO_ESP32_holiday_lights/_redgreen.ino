void redgreen () {
  unsigned long redgreenAnimationCycleLength = 1000; // how many milliseconds to switch colors
  unsigned long redgreenAnimationCycle = millis() - animationsCycleStartTime;
  unsigned long redgreenAnimationDuration = millis() - currentAnimationDuration;

  // Serial.print("redgreenAnimationCycleLength=" + String(redgreenAnimationCycleLength));
  // Serial.println(";redgreenAnimationCycle=" + String(redgreenAnimationCycle));

  if( redgreenAnimationDuration < currentAnimationDurationMax ) {
    if( redgreenAnimationCycle > redgreenAnimationCycleLength) {
      // Serial.println("red=" + String(rawleds[0].r));
      if( rawleds[0].r == 255) leds=CRGB::DarkGreen;
      else leds=CRGB::Red;
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
