#include <FastLED.h>

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    7
#define DATA_PIN_2  10
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define THIRD_LEDS  100
#define NUM_LEDS    300
CRGB leds[NUM_LEDS]; // 0.. 100

#define BRIGHTNESS         255
#define FRAMES_PER_SECOND  120

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

void setup() {
  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS ).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
//SimplePatternList gPatterns = { bpmLava, bpmHeat, bpmCloud, bpmForrest, bpmOcean, confettiGreenAndWhite, pinkAndWhite, dualSolid, rainbow, rainbowWithGlitter, confetti, sinelon, sinelon2, bpm, confettiGreenAndWhite, dualSolid, pinkAndWhite }; // taking Glitter & juggle out

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
uint8_t iteration = 0;
  
void loop() {
  // put your main code here, to run repeatedly:
  // Call the current pattern function once, updating the 'leds' array
//  gPatterns[gCurrentPatternNumber]();


  rainbow(leds);


  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
//  EVERY_N_MILLISECONDS( 20 ) { 
//    iteration++;
//    gHue++; 
//    Serial.println("gHue value: " + gHue);
//  } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 20 ) { fadeToBlackBy(leds, NUM_LEDS, 255); } // change patterns periodically
}

void rainbow(CRGB* ledArray) 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( ledArray, NUM_LEDS, gHue, 7 );
}

//void confetti(CRGB ledArray) 
//{
//  // random colored speckles that blink in and fade smoothly
//  fadeToBlackBy( leds1, NUM_LEDS, 10);
//  int posD = random16(NUM_LEDS);
//  leds1[posD] += CHSV( gHue + random8(64), 200, 255);
//}

void sinelon(CRGB* ledArray)
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( ledArray, THIRD_LEDS, 20);
  int posD = beatsin16( 13, 0, NUM_LEDS-1 );
  ledArray[posD] += CHSV( gHue, 255, 192 );
}
