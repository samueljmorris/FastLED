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
int BRIGHTNESS = 0;  //0-255, use > 50 when driven from Arduino
int analogPin = 0;   //analog input to read amplitude from
int inputMin = 0;    //min value from ADC
int inputMax = 1023; //max value from ADC
int scaledMin = 0;   //no added brightness
int scaledMax = 70; //approx. 70% of brightness
#define FRAMES_PER_SECOND 500
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
  //Serial.begin(115200);
  //init proximity sensor, pin 2
  pinMode(2, INPUT);
  delay(100); // 3 second delay for recovery
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  // set master brightness control
  FastLED.setDither( 0 ); //son of a
  FastLED.setBrightness(BRIGHTNESS); //initial brightness is 0, off per Elmer
}

bool pir = false;

void loop()
{
  //Routine to adjust brightness based off PIR value
  EVERY_N_MILLISECONDS(50)
  {
    unsigned int peakToPeak = analogRead(analogPin);
    peakToPeak = map(peakToPeak, inputMin, inputMax, scaledMin, scaledMax);
    FastLED.setBrightness(BRIGHTNESS + peakToPeak);
    // Serial.println(BRIGHTNESS);
    // Serial.println(peakToPeak);
    pir = digitalRead(2);
    //check pir, check if max brightness
    if (pir && BRIGHTNESS < 81) //what about pir and greater than 81?
    {
      //PIR based brightness from 0 to 80 (0 to ~30%)
      BRIGHTNESS = BRIGHTNESS + 1;
      //Serial.print("New brightness is: ");
      //Serial.println(BRIGHTNESS);
      // set master brightness control
      FastLED.setBrightness(BRIGHTNESS + peakToPeak);
    }
    //increment brightness if triggered
    else if (BRIGHTNESS > 0)
    {
      BRIGHTNESS = BRIGHTNESS - 1;
      // Serial.print("New brightness is: ");
      // Serial.println(BRIGHTNESS);
      // set master brightness control
      FastLED.setBrightness(BRIGHTNESS + peakToPeak);
    }
    //else if brightness > 0 decrement brightness
  }

  fadeToBlackBy( leds, NUM_LEDS, 20); //fade by 20/256ths
  //manually animate
  for (int dot = 0; dot < NUM_LEDS; dot++)
  {
    leds[dot] = CRGB::Red;
  }
  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}