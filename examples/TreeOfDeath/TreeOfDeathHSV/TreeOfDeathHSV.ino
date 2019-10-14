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

#define BRIGHTNESS          100 //0-255, use > 50 when driven from Arduino
#define FRAMES_PER_SECOND  240

void setup() {
  pinMode(2, INPUT); //Sound sensor
  pinMode(3, INPUT); //PIR sensor
  delay(100); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS); //.setCorrection(TypicalSMD5050);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { testPallet };

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
  boolean sound = digitalRead(2); //Audio?
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
  // 16 color test pallete
  leds[0] += CHSV(0,255,128); //26 81 100 hsv
  leds[1] += CHSV(2,255,128); //26 39 100 hsv
  leds[2] += CHSV(4,255,128); //26 62 100 hsv
  leds[3] += CHSV(6,255,128); //26 100 100 hsv
  leds[4] += CHSV(8,255,128); //26 100 75 hsv
  leds[5] += CHSV(10,255,128); //115 83 100 hsv
  leds[6] += CHSV(12,255,128); //95 59 96 hsv
  leds[7] += CHSV(14,255,128); //114 62 92 hsv
  leds[8] += CHSV(16,255,128); //114 100 86 hsv
  leds[9] += CHSV(18,255,128); //114 100 61 hsv
  leds[10] += CHSV(20,255,128); //220 73 79 hsv
  leds[11] += CHSV(22,255,128); //220 35 92 hsv
  leds[12] += CHSV(24,255,128); //220 56 85 hsv
  leds[13] += CHSV(26,255,128); //220 92 74 hsv
  leds[14] += CHSV(28,255,128); //219 92 50 hsv
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
