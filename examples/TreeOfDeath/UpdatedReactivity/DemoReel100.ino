#include <FastLED.h>

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN 6
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS 235 //strip length of actual tree prototype
CRGB leds[NUM_LEDS];

int BRIGHTNESS = 0; //0-255, use > 50 when driven from Arduino

#define FRAMES_PER_SECOND 200

CRGBPalette16 currentPalette;
TBlendType currentBlending;

//HSL/HSV values:
//40 is upper bounds for amber (already kind of greenish)
//30 is better
//20 very warm, below is red
//0 solid red
//120 lower bound, turquoise
//180 purple
//220 magenta
//230 pink
//250 salmon
//255 red

void setup()
{
  //Serial.begin(9600);
  Serial1.begin(9600);
  //init proximity sensor, pin 2
  pinMode(2, INPUT);

  delay(100); // 3 second delay for recovery

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS); //initial brightness is 0, off per Elmer
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {confetti, sinelon, juggle};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0;                  // rotating "base color" used by many of the patterns
bool pir = false;

void loop()
{
  EVERY_N_MILLISECONDS(500)
  {
    unsigned int peakToPeak = Serial1.parseInt();
    FastLED.setBrightness(BRIGHTNESS + peakToPeak);
    //Serial.println(peakToPeak);
  }

  //Routine to adjust brightness based off PIR value
  EVERY_N_MILLISECONDS(1000)
  {
    pir = digitalRead(2);
    //check pir, check if max brightness
    if (pir && BRIGHTNESS < 81) //what about pir and greater than 81?
    {
      //PIR based brightness from 0 to 80 (0 to ~30%)
      BRIGHTNESS = BRIGHTNESS + 1;
      //Serial.print("New brightness is: ");
      //Serial.println(BRIGHTNESS);
      // set master brightness control
      FastLED.setBrightness(BRIGHTNESS);
    }
    //increment brightness if triggered
    else if (BRIGHTNESS > 0)
    {
      BRIGHTNESS = BRIGHTNESS - 1;
      // Serial.print("New brightness is: ");
      // Serial.println(BRIGHTNESS);
      // set master brightness control
      FastLED.setBrightness(BRIGHTNESS);
    }
    //else if brightness > 0 decrement brightness
  }

    // EVERY_N_MILLISECONDS(100) //DEBUG OUTPUTS
    // {
      // Serial.print("PIR Sensor state: ");
      // Serial.println(pir);
      // Serial.print("Raw sound level: ");
      // Serial.println(soundLevel);
      // Serial.print("Max sound level: ");
      // Serial.println(maxSoundLevel);
    // }

  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  //do some periodic updates
  //EVERY_N_MILLISECONDS(10) { gHue++; }  // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS(5) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
}

//The following animations are parametric, not pallete based. Using range of hues to compensate.

//This method adds up to 64 to hue value at random. how to ensure hue stays within range?
void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(random8(40), 200, 255); //plus random number hue up to 40 per hue table above
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1); //change first parameter (bpm of sin wave) to audio level?
  leds[pos] += CHSV(gHue, 255, 192);
}

void juggle()
{
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  byte dothue = 0;
  for (int i = 0; i < 8; i++)
  {
    leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255); //change first parameter to audio level like above?
    dothue += 32;
  }
}