#include <FastLED.h>

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    6
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    300
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          50 //0-255, use > 50 when driven from Arduino
#define FRAMES_PER_SECOND  240

volatile boolean sounds;
volatile boolean isNearby;

void setup() {
  pinMode(2, INPUT); //Sound sensor
  pinMode(3, INPUT); //PIR sensor
  attachInterrupt(digitalPinToInterrupt(2), sound, RISING);
  attachInterrupt(digitalPinToInterrupt(3), nearby, RISING);
  delay(100); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS); //.setCorrection(TypicalSMD5050);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

void loop()
{
  if(sounds){
    hueShift();
  }
  else{
    testPallet();
  }
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);
  sounds = false;
}

void testPallet()
{
  fadeToBlackBy( leds, NUM_LEDS, 20);
  for(int i=0; i<NUM_LEDS; i++){
    leds[i] += CHSV(i,255,128);
  }
}

void hueShift()
{
  for(int i=0; i<NUM_LEDS; i++){
    leds[i] = -leds[i];
  }
}

void sound(){
  sounds = true;
}

void nearby(){
  isNearby = true;
}
