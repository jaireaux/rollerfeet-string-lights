#include "version.h"
#include "arduino_secrets.h"

//  DEVELOPMENT LIGHTS
// #define ANIMATION_CYCLE 10000 // in milliseconds, how ofter the animations change

//     ****** FASTLED ANIMATIONS ******* below
#define FASTLED_INTERNAL // add this before including FastLED.h
#include <FastLED.h>

// --------------- BLYNK --------------
/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_NAME         "Quickstart Template"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = SECRET_WIFI_SSID;
char pass[] = SECRET_WIFI_PASSWORD;

BlynkTimer timer;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Blynk.virtualWrite(V1, value);
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
}

// ----------- END BLYNK --------------

// prod
#define ANIMATION_CYCLE 15000 // 600000 // in milliseconds, how ofter the animations change
#define ANIMATION_FADE_CYCLE 5000 // in milliseconds, how ofter the animations change

#define APP_DEBUG

//     ****** FASTLED ANIMATIONS ******* below
// #define FASTLED_INTERNAL // add this before including FastLED.h
// #include <FastLED.h>

#include "_animations.h"
#define ANIMATIONS 5 // how many animations are in play
                      // 1. redgreen, NOPE 2. pacifica, NOPE 3. metaballs, 4. classicChristmas
                      // 5. peppermint, 6 meteorRain, NOPE 7 fadeinot

#define LED_PIN     4
#define DATA_PIN LED_PIN
#define COLOR_ORDER RGB
#define CHIPSET     WS2811
#define PIXELSECTIONS 6
#define PIXELSPERSECTION 50
#define NUM_LEDS PIXELSPERSECTION * PIXELSECTIONS
#define Width ((int) sqrt( NUM_LEDS) )+1
#define Height ((int) sqrt( NUM_LEDS) )-1
#define BRIGHTNESS 50

CRGB rawleds[NUM_LEDS];
CRGBSet leds(rawleds, NUM_LEDS);

unsigned long uptime = millis();
int currentAnimationNumber = 1;
unsigned long currentAnimationDuration = millis();
unsigned long currentAnimationDurationMax = ANIMATION_CYCLE;
unsigned long animationsCycleStartTime = millis();
int genericCounter=0;
unsigned long loopcounter=0;

//     ****** FASTLED ANIMATIONS ******* above

void setup()
{

  // ---------------- BLYNK ----------------
  // Debug console
  Serial.begin(115200);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
  // ------------ END BLYNK ----------------

  delay(1000);
  Serial.println("let's do this!");
  Serial.println("NUM_LEDS are " + String(NUM_LEDS));
  
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(rawleds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  genericCounter=0;
}

void loop() {
  // ---------------- BLYNK ----------------
  Blynk.run();
  timer.run();
  // ------------ END BLYNK ----------------

  // if( loopcounter % 20000 == 0) Serial.println("in the loop!");
  if( loopcounter % 5000 == 0) Serial.println("current animation=" + String(currentAnimationNumber));
  // Serial.println("blynk ready!");
  // Serial.println("timer set!");
  
//  switch (2) {
 switch (currentAnimationNumber) {
 case 1:
  FastLED.setBrightness(200);
  pacifica();
  break;
 case 2:
  FastLED.setBrightness(200);
  metaballs();
  Serial.println("metaballs");
  break;
 case 3:
  //  if( loopcounter % 20000 == 0) Serial.println("just in case 1!");
   FastLED.setBrightness( 200 );
  // meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, bool meteorRandomDecay, int SpeedDelay)
   meteorRain(0xFF, 0x00, 0x00, 0x0A, 0x01, false, 0);
  // solidglow();
   break;
 case 4:
  //  if( loopcounter % 20000 == 0) Serial.println("just in case 1!");
   FastLED.setBrightness( 200 );
   whiteblue();
  // solidglow();
   break;
 case 5:
  //  if( loopcounter % 20000 == 0) Serial.println("just in case 2!");
   FastLED.setBrightness( 255 );
  //  classicChristmas();
  throb();
   break;
 case 6:
  //  if( loopcounter % 20000 == 0) Serial.println("just in case 3!");
   FastLED.setBrightness( 200 );
   peppermint();
   break;
 case 7:
  //  if( loopcounter % 20000 == 0) Serial.println("just in case 3!");
   FastLED.setBrightness( 200 );
   classicChristmas();
   break;
 default:
  //  pacifica();

    if( loopcounter % 20000 == 0) Serial.println("blergh default");
    // whiteblue();
    // FastLED.setBrightness( 25 );
    // leds=CRGB::White;
    // FastLED.show(); // display this frame
    currentAnimationNumber = 1;
    currentAnimationDuration = millis();
    animationsCycleStartTime = millis();

   break;
 }
 loopcounter++;
}

void printwhichanimation() {
     if( loopcounter % 1000 == 0) Serial.println("current animation=" + String(currentAnimationNumber));
}

// Valentine colors
//// Gradient palette "bhw4_098_gp", originally from
//// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw4/tn/bhw4_098.png.index.html
//// converted for FastLED with gammas (2.6, 2.2, 2.5)
//// Size: 32 bytes of program space.
//
//DEFINE_GRADIENT_PALETTE( bhw4_098_gp ) {
//    0, 128, 33, 52,
//   35, 255, 17, 47,
//   58, 222,  2, 51,
//   99, 144, 56, 78,
//  124, 188,115,137,
//  178, 255, 16, 52,
//  219, 199,  1,  4,
//  255, 106,  1,  2};

// Gradient palette "bhw1_hello_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_hello.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 32 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw1_hello_gp ) {
    0, 237,156,197,
   35, 244,189,230,
   56, 255,255,255,
   79, 244,189,230,
  109, 237,156,197,
  160, 121,255,255,
  196, 255,255,255,
  255, 121,255,255};
