void solidglow () {
  unsigned long solidglowAnimationCycleLength = 1000; // how many milliseconds to switch colors
  unsigned long solidglowAnimationCycle = millis() - animationsCycleStartTime;
  unsigned long solidglowAnimationDuration = millis() - currentAnimationDuration;

  // Serial.print("solidglowAnimationCycleLength=" + String(solidglowAnimationCycleLength));
  // Serial.println(";solidglowAnimationCycle=" + String(solidglowAnimationCycle));

  if( solidglowAnimationDuration < currentAnimationDurationMax ) {
    if( solidglowAnimationCycle > solidglowAnimationCycleLength) {
      // Serial.println("red=" + String(rawleds[0].r));
      // if( rawleds[0].r == 255) leds=CRGB::DarkGreen;
      // else leds=CRGB::Red;
      leds=CRGB::White;
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
