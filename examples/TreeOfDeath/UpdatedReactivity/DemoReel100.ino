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
  delay(100); // 3 second delay for recovery

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS); //initial brightness off per Elmer
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {confetti, sinelon, juggle};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0;                  // rotating "base color" used by many of the patterns

void loop()
{
  boolean pir = digitalRead(2);    //Is there a person nearby?
  int soundLevel = analogRead(A0); //Current mic level?

  EVERY_N_MILLISECONDS(100)
  {
    //check pir, check if max brightness
    if (pir && BRIGHTNESS < 81)
    {
      //PIR based brightness from 0 to 80 (0 to ~30%)
      BRIGHTNESS = BRIGHTNESS + 1;
      Serial.print("New brightness is: ");
      Serial.println(BRIGHTNESS);
      // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
    }
    //increment brightness if triggered
    else if (BRIGHTNESS > 0)
    {
      BRIGHTNESS = BRIGHTNESS - 1;
      Serial.print("New brightness is: ");
      Serial.println(BRIGHTNESS);
      // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
    }
    //else if brightness > 0 decrement brightness
  }

  EVERY_N_MILLISECONDS(100)
  {
    Serial.print("PIR Sensor state: ");
    Serial.println(pir);
    Serial.print("Raw sound level: ");
    Serial.println(soundLevel);
  }

  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  //do some periodic updates
        EVERY_N_MILLISECONDS(10) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS(5) { nextPattern(); }      // change patterns periodically

  //  if(pir == HIGH){
  //    nextPattern();
  //  }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(gHue, 255, 192);
}

void juggle()
{
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  byte dothue = 0;
  for (int i = 0; i < 8; i++)
  {
    leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}