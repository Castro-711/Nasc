eiejcckhtlrhtdkdrgulgvrtiiicgliehjljtbertcdn
#include <FastLED.h>

// Set up some constants
#define NUM_STRIPS 2
#define NUM_LEDS_PER_STRIP 300
#define NUM_LEDS NUM_LEDS_PER_STRIP * NUM_STRIPS

#define PIN_1 7
#define PIN_2 10

#define BRIGHTNESS         200
#define FRAMES_PER_SECOND  120

// set up the LED array
CRGB leds_1[NUM_LEDS_PER_STRIP];
CRGB leds_2[NUM_LEDS_PER_STRIP];

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void setup() 
{
  // put your setup code here, to run once:
  FastLED.addLeds<NEOPIXEL, PIN_1>(leds_1, NUM_LEDS_PER_STRIP);

  FastLED.addLeds<NEOPIXEL, PIN_2>(leds_2, NUM_LEDS_PER_STRIP);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() 
{
  for(uint32_t i = 0; i < NUM_LEDS_PER_STRIP; i++) {
    leds_1[i] = CRGB::White;
    leds_2[i] = CRGB::Blue;

    FastLED.show();

    leds_1[i - 3] = CRGB::Black;
    leds_2[i - 3] = CRGB::Black;
    delay(100);
  }
  
}

void rainbow_sinelon() 
{
  // beatsin16(bpm, lowest = 0, highest = 65535, timebase = 0, phase_offset = 0)
  int posE = beatsin16(7, 0, NUM_LEDS_PER_STRIP);
  int posD = beatsin16(13, NUM_LEDS_PER_STRIP, NUM_LEDS);
//  leds[posE] += CHSV( gHue, 255, 255);
//  leds[posD] += CHSV( gHue, 255, 255);
}
