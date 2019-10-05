#include <FastLED.h>

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    6
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    300
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          255 //0-255, use > 50 when driven from Arduino
#define FRAMES_PER_SECOND  240

void setup() {
  pinMode(3, INPUT); //PIR sensor
  delay(100); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { confetti, sinelon, juggle };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns


//#####  Color Palette by Paletton.com
//#####  Palette URL: http://paletton.com/#uid=30s0u0kpOGxckTcjPMbA4BXACnV
//
//
//*** Primary color:
//
//(255,139, 49)
//(255,199,157)
//(255,166, 97)
//(255,111,  0)
//(191, 83,  0)
//
//*** Secondary color (1):
//
//( 61,225, 44)
//(159,244,150)
//(103,234, 89)
//( 21,219,  0)
//( 15,156,  0)
//
//*** Secondary color (2):
//
//( 55,104,201)
//(153,180,234)
//( 95,136,216)
//( 16, 74,188)
//( 10, 50,127)
//
//
//#####  Generated by Paletton.com (c) 2002-2014

  
void loop()
{
  boolean pir = digitalRead(3); //Is there a person nearby?
  
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 10 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 5 ) { nextPattern(); } // change patterns periodically

//  if(pir == HIGH){
//    nextPattern();
//  }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void testPallet()
{
  // test pattern for wood tones
  fadeToBlackBy( leds, NUM_LEDS, 20);

  leds[pos] += CRGB( gHue, 255, 192);

leds[] += CRGB(255,139, 49);
leds[] += CRGB(255,199,157);
leds[] += CRGB(255,166, 97);
leds[] += CRGB(255,111,  0);
leds[] += CRGB(191, 83,  0);
leds[] += CRGB( 61,225, 44);
leds[] += CRGB(159,244,150);
leds[] += CRGB(103,234, 89);
leds[] += CRGB( 21,219,  0);
leds[] += CRGB( 15,156,  0);
leds[] += CRGB( 55,104,201);
leds[] += CRGB(153,180,234);
leds[] += CRGB( 95,136,216);
leds[] += CRGB( 16, 74,188);
leds[] += CRGB( 10, 50,127);
  
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
