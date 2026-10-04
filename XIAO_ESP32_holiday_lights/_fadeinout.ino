void fadeinout(){
  int fadeTime=500; // in ms the amount of time to fade in or out (not in and out)
  int fadeCycle; 
  fadeCycle = (millis() - currentAnimationDuration) % fadeTime;
  Serial.println("fadeCycle=" + String(fadeCycle) + ";timeSoFar=" + String((millis() - currentAnimationDuration)));

//  if(
//  for(int j = 0; j < 3; j++ ) {
//    // Fade IN
//    for(int k = 0; k < 256; k++) {
//      switch(j) {
//        case 0: setAll(k,0,0); break;
//        case 1: setAll(0,k,0); break;
//        case 2: setAll(k,k,k); break;
//      }
//      FastLED.show();
        delay(100);
//    }
//    // Fade OUT
//    for(int k = 255; k >= 0; k--) {
//      switch(j) {
//        case 0: setAll(k,0,0); break;
//        case 1: setAll(0,k,0); break;
//        case 2: setAll(k,k,k); break;
//      }
//      FastLED.show();
//      unblockingDelay(5);
//    }
//  }
}
