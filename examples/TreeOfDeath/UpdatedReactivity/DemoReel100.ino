#include <FastLED.h>

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN 6
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 235 //strip length of actual tree prototype
CRGB leds[NUM_LEDS];

int BRIGHTNESS = 0; //0-255, use > 50 when driven from Arduino

const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;         //current sound level during sample window

int windowMin = 0;    //min value from ADC
int windowMax = 1023; //max value from ADC
int scaledMin = 0;    //no added brightness
int scaledMax = 150;  //approx. 70% of brightness

#define FRAMES_PER_SECOND 240

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
  Serial.begin(115200);
  //init proximity sensor, pin 2
  pinMode(2, INPUT);
  //init mic, analog a0
  analogReference(INTERNAL2V56); //Use lower reference to expand sound dynamic range

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
  //First try regular loop execution, check latency. use EVERY_N_MILLISECONDS otherwise

  unsigned long startMillis = millis(); // Start of sample window
  unsigned int peakToPeak = 0;          // peak-to-peak level

  unsigned int signalMax = 0;    //running max for current window
  unsigned int signalMin = 1024; //running min for current window

  // collect data for 50 mS
  while (millis() - startMillis < sampleWindow)
  {
    sample = analogRead(0);
    if (sample < 1024) // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample; // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample; // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin; // max - min = peak-peak amplitude

  peakToPeak = map(peakToPeak, windowMin, windowMax, scaledMin, scaledMax);

  Serial.println(peakToPeak);

  FastLED.setBrightness(BRIGHTNESS + peakToPeak); //not deterministic

  //Routine to adjust brightness based off PIR value
  EVERY_N_MILLISECONDS(100)
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
      //Serial.print("New brightness is: ");
      //Serial.println(BRIGHTNESS);
      // set master brightness control
      FastLED.setBrightness(BRIGHTNESS);
    }
    //else if brightness > 0 decrement brightness
  }

  //   EVERY_N_MILLISECONDS(100) //DEBUG OUTPUTS
  //   {
  //     Serial.print("PIR Sensor state: ");
  //     Serial.println(pir);
  //     Serial.print("Raw sound level: ");
  //     Serial.println(soundLevel);
  //     Serial.print("Max sound level: ");
  //     Serial.println(maxSoundLevel);
  //   }

  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  //do some periodic updates
  EVERY_N_MILLISECONDS(10) { gHue++; }  // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS(5) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
}

//The following animations are parametric, not pallete based. Using range of hues to compensate.

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), 200, 255); //plus random number up to 64
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