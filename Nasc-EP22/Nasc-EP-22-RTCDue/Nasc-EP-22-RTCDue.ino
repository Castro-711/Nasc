#include <FastLED.h>
#include <RTCDue.h>

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

/**
 * Todo:
 * 1. Expand on fillFadeIn - create some concrete versions with fave colours & keep the random one
 *    1a. Still to add more specific colours, but covered the random ones for now - don't think html will work as expected
 * 2. Create pattern better than sinelon using a for loop & maybe CRGBSet -> https://github.com/FastLED/FastLED/wiki/RGBSet-Reference
 *    2a. Use the fadeToBlackBy over the next few pixels, each one getting closer to off to help create the trail
 * 3. Nail down the day pattern with trails - concrete colours & random colours
 * 4. Expand on lightening patterns - more concrete colours & some random colours
 * 5. Figure out how to fade out lights completly
 */

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#if __STDC_VERSION__ < 199901L
# if __GNUC__ >= 2
#  define __func__ __FUNCTION__
# else
#  define __func__ "<unknown>"
# endif
#endif

#define DATA_PIN    3
#define DATA_PIN_2  5
#define DATA_PIN_3  7
#define DATA_PIN_4  9
#define DATA_PIN_5  11
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    300
#define MINI_LEDS   60 // mini strip
#define HUE_OFFSET  4
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

CRGB leds1[NUM_LEDS]; // led array
CRGB leds2[NUM_LEDS];
CRGB leds3[NUM_LEDS];
CRGB leds4[NUM_LEDS];
CRGB leds5[NUM_LEDS];

#define BRIGHTNESS         255
#define FRAMES_PER_SECOND  60

// RTC variables
RTCDue rtc(XTAL);
const char* daynames[]={"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

// an array of html colours
const CRGB htmlColours[]={
                   CRGB::Aquamarine, CRGB::Fuchsia, CRGB::PaleGreen, CRGB::SeaGreen, CRGB::Turquoise,
                   CRGB::Tomato, CRGB::WhiteSmoke, CRGB::YellowGreen, CRGB::Plum, CRGB::Silver, 
                   CRGB::LightCoral, CRGB::LightPink, CRGB::HotPink, CRGB::Blue, CRGB::Cyan, 
                   CRGB::DeepSkyBlue, CRGB::SpringGreen, CRGB::MediumSpringGreen, CRGB::Lime,
                   CRGB::MediumSlateBlue, CRGB::Purple, CRGB::DarkOrchid, CRGB::DarkViolet,
                   CRGB::Magenta, CRGB::Yellow, CRGB::Khaki, CRGB::Orange, CRGB::DeepPink,
                   CRGB::Red, CRGB::FireBrick, CRGB::Gold
};

const int HTML_COLOUR_SIZE = ARRAY_SIZE(htmlColours);

void setup() {
  Serial.begin(115200);
  Serial.print("HelloNasc!");
  
  delay(3000); // 3 second delay for recovery

  setupRTCTime();

  setupFastLEDs();

  Serial.println("FASTLED.size: ");
  Serial.print(FastLED.size());
  Serial.println("FASTLED.brightness: ");
  Serial.print(FastLED.getBrightness());
}

void setupFastLEDs() {
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN, COLOR_ORDER>(leds1, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN_2, COLOR_ORDER>(leds2, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN_3, COLOR_ORDER>(leds3, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN_4,COLOR_ORDER>(leds4, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN_5, COLOR_ORDER>(leds5, NUM_LEDS).setCorrection(TypicalLEDStrip);

  fadeAllStripsToBlack();

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setTemperature(CoolWhiteFluorescent);
}

void setupRTCTime() {
  rtc.begin(); // init RTC

  rtc.setTime(__TIME__);
  rtc.setDate(__DATE__);
  
  Serial.print("Unixtime: ");
  Serial.println(rtc.unixtime());

  // Print time...
  Serial.println("And in plain for everyone");
  Serial.print("Time: ");
  digitprint(rtc.getHours(), 2);
  Serial.print(":");
  digitprint(rtc.getMinutes(), 2);
  Serial.print(":");
  digitprint(rtc.getSeconds(), 2);
  Serial.println("");

  // Print date...
  Serial.print("Date: ");
  Serial.print(daynames[rtc.getDayofWeek()]);
  Serial.print(" ");
  digitprint(rtc.getDay(), 2);
  Serial.print(".");
  digitprint(rtc.getMonth(), 2);
  Serial.print(".");
  Serial.println(rtc.getYear());
  Serial.println("");
}

/********* RTC helper methods from https://github.com/MarkusLange/RTCDue/blob/master/examples/RTCDue_Unixtime_Compilertime/RTCDue_Unixtime_Compilertime.ino **********/
void digitprint(int value, int lenght){
  for (int i = 0; i < (lenght - numdigits(value)); i++){
    Serial.print("0");
  }
  Serial.print(value);
}

int numdigits(int i){
  int digits;
  if (i < 10)
    digits = 1;
  else
    digits = (int)(log10((double)i)) + 1;
  return digits;
}

/******** My helper methods ********/
char* currentPatternName = "SETUP";
void printCurrentPatternOnce(const char* pattName) {
  if (pattName != currentPatternName) {
    Serial.print("Current pattern: ");
    Serial.println(currentPatternName);
    Serial.println();
    Serial.print("pattName: ");
    Serial.println(pattName);
    strcpy(currentPatternName, pattName);
//    currentPatternName = pattName; must use strcpy instead to work with __FUNCTION__ which is const
  }
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList eveningPatterns = {  
  testStripAndDataPin,
  testStripLength,
  sinelonACDC,
  fillFadeInOriginal
}; // irelandFlagSinelon, fillFadeIn, pulseSinelon

SimplePatternList dayPatterns = {  
  fillFadeInRandomHue,
  bpmCloud,
  fillFadeInRandomHueAndSat
}; // irelandFlagSinelon, fillFadeIn, pulseSinelon

//SimplePatternList eveningPatterns = { myRainbow, myRainbow, myRainbow, bpmLava, bpmHeat, bpmCloud, bpmForrest, bpmOcean, confettiGreenAndWhite, pinkAndWhite, dualSolid, rainbow, rainbowWithGlitter, confetti, sinelon, sinelon2, bpm, confettiGreenAndWhite, dualSolid, pinkAndWhite }; // taking Glitter & juggle out

uint8_t currentEveningPatternNumber = 0; // Index number of which pattern is current
uint8_t currentDayPatternNumber = 0;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
uint8_t rHue = 0;

// strip sections
uint16_t top = beatsin16( 7, 0, 100 );
uint16_t mid = beatsin16( 7, 101, 200 );
uint16_t bot = beatsin16( 7, 201, 300 );

uint16_t t2m = beatsin16(7, 0, 200 );      // top2middle
uint16_t t2b = beatsin16(7, 0, 300 );      // top2bottom
uint16_t m2b = beatsin16(7, 100, 300 );    // middle2bottom

// colours param doc
// CHSV(hue, saturation, brightness)
// CRGB(red, green, green)
CRGB whiteC = CRGB( 255, 255, 255 );

// colour values
uint8_t red  = 0;
uint8_t orange = 32;
uint8_t yellow = 64;
uint8_t green = 96;
uint8_t aqua = 128;
uint8_t blue = 160;
uint8_t purple = 192;
uint8_t pink = 224;

  
void loop() { 
  /*  there seems to be a 24 second delay between minutes. Switching between day & night.
      13:16:24.006 -> Day Loop
      13:16:24.115 -> Evening Loop
  */
  if ( rtc.getMinutes() >= 0 && rtc.getMinutes() <= 7 ) {
    dayLoop();
    Serial.println("Day Loop");
  }
  else if ( rtc.getMinutes() > 55 && rtc.getMinutes() <= 59 ) {
    eveningLoop();
    Serial.println("Evening Loop");
  }
  else {
    Serial.println("Fading to black");
    fadeAllStripsToBlack();
  }
}

void eveningLoop() {
   // Call the current pattern function once, updating the 'leds' array
  eveningPatterns[currentEveningPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { 
    gHue++; 
  } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 24 ) { nextEveningPattern(); } // change patterns periodically
}

// only difference here is that it is faster
void dayLoop() {
    dayPatterns[currentDayPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { 
    gHue++; 
  } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 17 ) // make this a variable & give it a range to increase the randomness
  { nextDayPattern(); } // change patterns periodically 
}


void nextEveningPattern()
{
  // add one to the current pattern number, and wrap around at the end
  currentEveningPatternNumber = (currentEveningPatternNumber + 1) % ARRAY_SIZE( eveningPatterns );
}

void nextDayPattern() {
  currentDayPatternNumber = (currentDayPatternNumber + 1) % ARRAY_SIZE( dayPatterns );
}

void testStripLength() {
  printCurrentPatternOnce(__FUNCTION__);
  Serial.println("testStripLength");

  fadeAllStripsToBlack();
  Serial.println("Num leds => ");
  Serial.println(NUM_LEDS);
  
  for ( uint8_t i = 0; i < NUM_LEDS; i++ ) {
    leds1[i] = CHSV(0, 0, 300);
    FastLED.delay(20);
    Serial.println(i);
  }

  FastLED.delay(10000);
}

void testStripAndDataPin() {
  printCurrentPatternOnce(__FUNCTION__);
  fadeAllStripsToBlack();

  fill_solid(leds1, NUM_LEDS, CRGB::Snow);
  Serial.println("LED1 is on - Data Pin 3");
  delay(10000);

  fadeAllStripsToBlack();

  fill_solid(leds2, NUM_LEDS, CRGB::Snow);
  Serial.println("LED2 is on - Data Pin 5");
  delay(10000);

  fadeAllStripsToBlack();

  fill_solid(leds3, NUM_LEDS, CRGB::Snow);
  Serial.println("LED3 is on - Data Pin 7");
  delay(10000);

  fadeAllStripsToBlack();

  fill_solid(leds4, NUM_LEDS, CRGB::Snow);
  Serial.println("LED4 is on - Data Pin 9");
  delay(10000);

  fadeAllStripsToBlack();

  fill_solid(leds5, NUM_LEDS, CRGB::Snow);
  Serial.println("LED5 is on - Data Pin 11");
  delay(10000);

  fadeAllStripsToBlack();
}

// first attempt without RGBSet will try that later
void sinelonACDC() {
  printCurrentPatternOnce(__FUNCTION__);
  
  for ( uint8_t i = 0; i < NUM_LEDS; i++ ) {
    leds1[i] = CHSV(aqua, rand() % 100, 100 ); 
    leds1[i+1] = CHSV(aqua, rand() % 100, 100 ); 
    leds1[i+2] = CHSV(aqua, rand() % 100, 100 ); 

    leds2[i] = CHSV(blue, rand() % 100, 100 ); 
    leds2[i+1] = CHSV(blue, rand() % 100, 100 ); 
    leds2[i+2] = CHSV(blue, rand() % 100, 100 ); 

    leds3[i] = CHSV(pink, rand() % 100, 100 ); 
    leds3[i+1] = CHSV(aqua, rand() % 100, 100 ); 
    leds3[i+2] = CHSV(blue, rand() % 100, 100 ); 

    leds4[i] = CHSV(green, rand() % 100, 100 ); 
    leds4[i+1] = CHSV(yellow, rand() % 100, 100 ); 
    leds4[i+2] = CHSV(pink, rand() % 100, 100 ); 

    leds5[i] = CHSV(orange, rand() % 100, 100 ); 
    leds5[i+1] = CHSV(purple, rand() % 100, 100 ); 
    leds5[i+2] = CHSV(orange, rand() % 100, 100 ); 
    
    FastLED.delay(33);
    leds1[i] = CRGB::Black;
    leds1[i+1] = CRGB::Black;
    leds1[i+2] = CRGB::Black;

    leds2[i] = CRGB::Black;
    leds2[i+1] = CRGB::Black;
    leds2[i+2] = CRGB::Black;

    leds3[i] = CRGB::Black;
    leds3[i+1] = CRGB::Black;
    leds3[i+2] = CRGB::Black;

    leds4[i] = CRGB::Black;
    leds4[i+1] = CRGB::Black;
    leds4[i+2] = CRGB::Black;

    leds5[i] = CRGB::Black;
    leds5[i+1] = CRGB::Black;
    leds5[i+2] = CRGB::Black;
  }

  for ( uint8_t i = NUM_LEDS; i > 0; i-- ) {
    leds1[i] = CHSV(pink, rand() % 100, 100 );
    leds1[i-1] = CHSV(purple, rand() % 100, 100 );
    leds1[i-2] = CHSV(pink, rand() % 100, 100 );

    leds2[i] = CHSV(green, rand() % 100, 100 );
    leds2[i-1] = CHSV(0, 0, 100 );
    leds2[i-2] = CHSV(orange, rand() % 100, 100 );

    leds3[i] = CHSV(green, rand() % 100, 100 );
    leds3[i-1] = CHSV(0, 0, 100 );
    leds3[i-2] = CHSV(orange, rand() % 100, 100 );

    leds4[i] = CHSV(pink, rand() % 100, 100 );
    leds4[i-1] = CHSV(yellow, rand() % 100, 100 );
    leds4[i-2] = CHSV(purple, rand() % 100, 100 );

    leds5[i] = CHSV(purple, rand() % 100, 100 );
    leds5[i-1] = CHSV(pink, rand() % 100, 100 );
    leds5[i-2] = CHSV(aqua, rand() % 100, 100 );
    FastLED.delay(33);
    leds1[i] = CRGB::Black;
    leds1[i-1] = CRGB::Black;
    leds1[i-2] = CRGB::Black;
  }
}

void rainbowT2() {
  fadeToBlackBy( leds1, NUM_LEDS, 20 );
  fadeToBlackBy( leds2, NUM_LEDS, 20 );
  uint16_t pos = beatsin88( 1792, 0, NUM_LEDS );  // 7 * 256 = 1792
  uint16_t cub = cubicwave8( 1792 );
  leds1[pos] += CHSV( rHue += HUE_OFFSET, 255, 255 );
  leds2[cub] += CHSV( rHue += HUE_OFFSET, 255, 255 );
  FastLED.show();
}

uint8_t lower = 192;
uint8_t upper = 255;

uint8_t one = aqua;
uint8_t two = purple;
uint8_t three = blue;
uint8_t four = pink;
uint8_t five = green;

uint8_t colourArray[5];
//colourArray[0] = one;
//colourArray[1] = two;
//colourArray[2] = three;
//colourArray[3] = four;
//colourArray[4] = five;

uint8_t decCounter = 0;

void lighteningStripCustom(CRGB one, CRGB two, CRGB three, CRGB four)
{
  Serial.println("lighteningStripCustom");
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  // fade_video (CRGB *leds, uint16_t num_leds, uint8_t fadeBy)
  fill_solid(leds1, NUM_LEDS, one);
  fill_solid(leds2, NUM_LEDS, two);
  fill_solid(leds3, NUM_LEDS, three);
  fill_solid(leds4, NUM_LEDS, four);
  uint8_t randomColour = rand() % HTML_COLOUR_SIZE;
  fill_solid(leds5, NUM_LEDS, htmlColours[randomColour]);
}

void lighteningStrip()
{
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  // fade_video (CRGB *leds, uint16_t num_leds, uint8_t fadeBy)
  fill_solid(leds1, NUM_LEDS, CHSV(green, 255, 255));
  fill_solid(leds2, NUM_LEDS, CHSV(pink, 255, 255));
  fill_solid(leds3, NUM_LEDS, CHSV(blue, 255, 255));
  fill_solid(leds4, NUM_LEDS, CHSV(purple, 255, 255));
  fill_solid(leds5, NUM_LEDS, CHSV(aqua, 255, 255));
}

uint8_t blendOffset = 17;

void blendStrips()
{
  fadeToBlackBy(leds1, NUM_LEDS, 20);

  // seems very static
//  CRGB   blend (const CRGB &p1, const CRGB &p2, fract8 amountOfP2)
  fill_solid(leds1, NUM_LEDS, green);
  blend(green, whiteC, 1 / blendOffset--);
  fill_solid(leds2, NUM_LEDS, whiteC);
  blend(whiteC, orange, 1 / blendOffset--);
  fill_solid(leds3, NUM_LEDS, orange);
  blend(orange, green, 1 / blendOffset--);
}

void lighteningBoltsDec() {
  // fill first
//  fadeToBlackBy( leds1, NUM_LEDS, 20);
  fill_solid(leds1, NUM_LEDS, CHSV(one, 255, random8(lower -= 4, upper -= 4)));
  fill_solid(leds2, NUM_LEDS, CHSV(two, 255, random8(lower -= 4, upper -= 4)));
  fill_solid(leds3, NUM_LEDS, CHSV(three, 255, random8(lower -= 4, upper -= 4)));
  fill_solid(leds4, NUM_LEDS, CHSV(four, 255, random8(lower -= 4, upper -= 4)));
  fill_solid(leds5, NUM_LEDS, CHSV(five, 255, random8(lower -= 4, upper -= 4)));
  
  delay(random8(0, 20));

  if(lower <= 4) {
    if(decCounter % 5 == 0) {
      one = pink;
      two = blue;
      three = aqua;
      four = green;
      five = purple;
    }
    else if(decCounter % 4 == 0) {
      one = blue;
      two = purple;
      three = pink;
      four = aqua;
      five = green;
    }
    else if(decCounter % 3 == 0) {
      one = green;
      two = aqua;
      three = purple;
      four = blue;
      five = purple;
    }
    else if(decCounter % 2 == 0) {
      one = aqua;
      two = green;
      three = purple;
      four = pink;
      five = blue;
    }
    else {
      one = green;
      two = pink;
      three = blue;
      four = blue;
      five = aqua;
  }
   decCounter++; // when lower = 0 we increment the counter
  }
}

uint8_t low = 0;
uint8_t up = 0;
uint8_t incCounter = 0;
void lighteningBoltsInc() {
  // fill first
//  fadeToBlackBy( leds1, NUM_LEDS, 20);
  fill_solid(leds1, NUM_LEDS, CHSV(one, 255, random8(low += 4, up += 4)));
  fill_solid(leds2, NUM_LEDS, CHSV(two, 255, random8(low += 4, up += 4)));
  fill_solid(leds3, NUM_LEDS, CHSV(three, 255, random8(low += 4, up += 4)));
  fill_solid(leds4, NUM_LEDS, CHSV(four, 255, random8(low += 4, up += 4)));
  fill_solid(leds5, NUM_LEDS, CHSV(five, 255, random8(low += 4, up += 4)));  
  
  delay( random8(0, 50)  );

  if(up >= 250) {
   if(decCounter % 5 == 0) {
      one = pink;
      two = blue;
      three = aqua;
      four = green;
      five = purple;
    }
    else if(decCounter % 4 == 0) {
      one = blue;
      two = purple;
      three = pink;
      four = aqua;
      five = green;
    }
    else if(decCounter % 3 == 0) {
      one = green;
      two = aqua;
      three = purple;
      four = blue;
      five = purple;
    }
    else if(decCounter % 2 == 0) {
      one = aqua;
      two = green;
      three = purple;
      four = pink;
      five = blue;
    }
    else {
      one = green;
      two = pink;
      three = blue;
      four = blue;
      five = aqua;
  }
   incCounter++; // when lower = 0 we increment the counter
  }
}



// use bseconds to get current time since boot
// binary seconds 1024/1000

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds1, NUM_LEDS, gHue, 7 );
  fill_rainbow( leds2, NUM_LEDS, gHue, 7 );
  fill_rainbow( leds3, NUM_LEDS, gHue, 7 );
  fill_rainbow( leds4, NUM_LEDS, gHue, 7 );
  fill_rainbow( leds5, NUM_LEDS, gHue, 7 );
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds1[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeAllStripsToBlack();
  
  int pos1 = random16(NUM_LEDS);
  int pos2 = random16(NUM_LEDS);
  int pos3 = random16(NUM_LEDS);
  int pos4 = random16(NUM_LEDS);
  int pos5 = random16(NUM_LEDS);
  leds1[pos1] += CHSV( gHue + random8(64), 200, 255);
  leds2[pos2] += CHSV( gHue + random8(64), 200, 255);
  leds3[pos3] += CHSV( gHue + random8(64), 200, 255);
  leds4[pos4] += CHSV( gHue + random8(64), 200, 255);
  leds5[pos5] += CHSV( gHue + random8(64), 200, 255);
}

void myConfetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeAllStripsToBlack();
  
  int pos1 = random16(NUM_LEDS);
  int pos2 = random16(NUM_LEDS);
  int pos3 = random16(NUM_LEDS);
  int pos4 = random16(NUM_LEDS);
  int pos5 = random16(NUM_LEDS);
  leds1[pos1] += CHSV( gHue + random8(64), 200, 255);
  leds2[pos2] += CHSV( gHue + random8(64), 200, 255);
  leds3[pos3] += CHSV( gHue + random8(64), 200, 255);
  leds4[pos4] += CHSV( gHue + random8(64), 200, 255);
  leds5[pos5] += CHSV( gHue + random8(64), 200, 255);
}


void reggaeConfetti()
{
  fadeAllStripsToBlack();
  
  uint8_t red = random16(NUM_LEDS);
  uint8_t yellow = random16(NUM_LEDS);
  uint8_t green = random16(NUM_LEDS);
  leds1[red] += CHSV( random8(0, 16), 255, 255);
  leds1[yellow] += CHSV( random8(64, 80), 255, 255);
  leds1[green] += CHSV( random8(96, 112), 255, 255);

  leds2[red] += CHSV( random8(0, 16), 255, 255);
  leds2[yellow] += CHSV( random8(64, 80), 255, 255);
  leds2[green] += CHSV( random8(96, 112), 255, 255);

  leds3[red] += CHSV( random8(0, 16), 255, 255);
  leds3[yellow] += CHSV( random8(64, 80), 255, 255);
  leds3[green] += CHSV( random8(96, 112), 255, 255);

  leds4[red] += CHSV( random8(0, 16), 255, 255);
  leds4[yellow] += CHSV( random8(64, 80), 255, 255);
  leds4[green] += CHSV( random8(96, 112), 255, 255);

  leds5[red] += CHSV( random8(0, 16), 255, 255);
  leds5[yellow] += CHSV( random8(64, 80), 255, 255);
  leds5[green] += CHSV( random8(96, 112), 255, 255);
}

void reggaeSingleSinelon()
{
   fadeAllStripsToBlack();

   leds1[top] += CHSV( 0, 255, 255);
   leds1[mid] += CHSV( 96, 255, 255);
   leds1[bot] += CHSV( 64, 255, 255);

   leds2[top] += CHSV( 0, 255, 255);
   leds2[mid] += CHSV( 96, 255, 255);
   leds2[bot] += CHSV( 64, 255, 255);

   leds3[top] += CHSV( 0, 255, 255);
   leds3[mid] += CHSV( 96, 255, 255);
   leds3[bot] += CHSV( 64, 255, 255);

   leds4[top] += CHSV( 0, 255, 255);
   leds4[mid] += CHSV( 96, 255, 255);
   leds4[bot] += CHSV( 64, 255, 255);

   leds5[top] += CHSV( 0, 255, 255);
   leds5[mid] += CHSV( 96, 255, 255);
   leds5[bot] += CHSV( 64, 255, 255);
}
//void Fade

void reggaeSinelon()
{
  fadeAllStripsToBlack();

  uint8_t reggaeRed = random8(0, 16);
  uint8_t reggaeYellow = random8(64, 80);
  uint8_t reggaeGreen = random8(96, 112);

   leds1[top] += reggaeRed;
   leds1[t2b] += reggaeYellow;
   leds1[bot] += reggaeGreen;

   leds2[top] += reggaeYellow;
   leds2[t2b] += reggaeGreen;
   leds2[bot] += reggaeRed;

   leds3[top] += reggaeGreen;
   leds3[t2b] += reggaeRed;
   leds3[bot] += reggaeYellow;

   leds4[top] += reggaeRed;
   leds4[t2b] += reggaeGreen;
   leds4[bot] += reggaeYellow;

   leds5[top] += reggaeGreen;
   leds5[t2b] += reggaeYellow;
   leds5[bot] += reggaeRed;
}

void irelandFlagSinelon()
{
   fadeAllStripsToBlack();

   leds1[top] += green;
   leds1[t2b] += whiteC;
   leds1[bot] += orange;

   leds2[top] += aqua;
   leds2[t2b] += purple;
   leds2[bot] += pink;

   leds3[top] += green;
   leds3[t2b] += whiteC;
   leds3[bot] += orange;

   leds4[top] += aqua;
   leds4[t2b] += purple;
   leds4[bot] += pink;

   leds5[top] += green;
   leds5[t2b] += whiteC;
   leds5[bot] += orange;
}

/***** fillFadeIn patterns ******/

// fucking class - pulse session - love it...!!
void fillFadeInRandomHue() 
{
  fadeAllStripsToBlack();
  printCurrentPatternOnce(__FUNCTION__);
  printCurrentPatternOnce("This uses 255 for sat & val limit");

  // rand() % 255 = x <= 255
  fill_solid(leds1, NUM_LEDS, CHSV(rand() % 255, 255, random8(90, 255)));
  fill_solid(leds2, NUM_LEDS, CHSV(rand() % 255, 255, random8(90, 255)));
  fill_solid(leds3, NUM_LEDS, CHSV(rand() % 255, 255, random8(90, 255)));
  fill_solid(leds4, NUM_LEDS, CHSV(rand() % 255, 255, random8(90, 255)));
  fill_solid(leds5, NUM_LEDS, CHSV(rand() % 255, 255, random8(90, 255)));
}

void fillFadeInRandomHueAndSat() {

  fadeAllStripsToBlack();
  printCurrentPatternOnce(__FUNCTION__);
  printCurrentPatternOnce("This uses 100 for sat & val limit");

  // rand() % 100 = x <= 100 - saturation & value are apparently 0..100
  fill_solid(leds1, NUM_LEDS, CHSV(rand() % 255, rand() % 100, random8(90, 100)));
  fill_solid(leds2, NUM_LEDS, CHSV(rand() % 255, rand() % 100, random8(90, 100)));
  fill_solid(leds3, NUM_LEDS, CHSV(rand() % 255, rand() % 100, random8(90, 100)));
  fill_solid(leds4, NUM_LEDS, CHSV(rand() % 255, rand() % 100, random8(90, 100)));
  fill_solid(leds5, NUM_LEDS, CHSV(rand() % 255, rand() % 100, random8(90, 100)));
  
}

// not confident this will work well as we need the CHSV colours to change the brightness
void fillFadeInHtmlRandom() {
  fadeAllStripsToBlack();
  printCurrentPatternOnce(__FUNCTION__);

  fill_solid(leds1, NUM_LEDS, htmlColours[random8(0, HTML_COLOUR_SIZE)] );
  fill_solid(leds2, NUM_LEDS, htmlColours[random8(0, HTML_COLOUR_SIZE)] );
  fill_solid(leds3, NUM_LEDS, htmlColours[random8(0, HTML_COLOUR_SIZE)] );
  fill_solid(leds4, NUM_LEDS, htmlColours[random8(0, HTML_COLOUR_SIZE)] );
  fill_solid(leds5, NUM_LEDS, htmlColours[random8(0, HTML_COLOUR_SIZE)] );
}

// aqua + purple
void fillFadeInOriginal() {
  fadeAllStripsToBlack();
  printCurrentPatternOnce(__FUNCTION__);

  fill_solid(leds1, NUM_LEDS, CHSV(128, 255, random8(90, 255)));
  fill_solid(leds2, NUM_LEDS, CHSV(192, 255, random8(90, 255)));
  fill_solid(leds3, NUM_LEDS, CHSV(128, 255, random8(90, 255)));
  fill_solid(leds4, NUM_LEDS, CHSV(192, 255, random8(90, 255)));
  fill_solid(leds5, NUM_LEDS, CHSV(128, 255, random8(90, 255)));
}


void sinelonEP22() {
  fadeAllStripsToBlack();
  printCurrentPatternOnce(__FUNCTION__);
  
  for(uint8_t i = 0; i < HTML_COLOUR_SIZE / 3; i++) {
    uint8_t randomColour = rand() % HTML_COLOUR_SIZE;
    uint8_t sPos = beatsin16( 3, 0, NUM_LEDS );
    uint8_t qPos = quadwave8( 17 );
    leds1[sPos] += htmlColours[randomColour];
    leds2[qPos] += htmlColours[randomColour];
    leds3[sPos] += htmlColours[randomColour];
    leds4[qPos] += htmlColours[randomColour];
    leds5[sPos] += htmlColours[randomColour];
  }
}

// now a pulsating sinelon
void pulseSinelon()
{
  fadeAllStripsToBlack();
  
  uint16_t one_ = beatsin16( 3, 0, NUM_LEDS );
  uint16_t two_ = beatsin16( 3, 0, NUM_LEDS );
  uint16_t three_ = beatsin16( 3, 0, NUM_LEDS );
  uint16_t four_ = beatsin16( 3, 0, NUM_LEDS );
  uint16_t five_ = beatsin16( 3, 0, NUM_LEDS );

  leds1[one_] += CHSV(192, 255, random8(90,255));
  leds2[two_] += CHSV(128, 255, random8(90,255));
  leds3[three_] += CHSV(192, 255, random8(90,255));
  leds4[four_] += CHSV(128, 255, random8(90,255));
  leds5[five_] += CHSV(192, 255, random8(90,255));
}

uint8_t sineColour = 0;
// messing with bpm can give a confetti effect
void reggaeFullSinelone()
{
  fadeAllStripsToBlack();
  
  int pos = beatsin16(13, 0, NUM_LEDS);
  int pos_ = beatsin16(7, 0, NUM_LEDS);
  int pos__ = beatsin16(9, 0, NUM_LEDS);

  CHSV master = CHSV(red, 255, 255);

//  if(pos % 13 == 0) {
    if(sineColour % 3 == 0){
      master = CHSV(green, 255, 255);
    }
    
    else if(sineColour % 2 == 0){
      master = CHSV(yellow, 255, 255);
    }

  sineColour++;
  leds1[pos] += CHSV(64, 255, 255);
  leds1[pos_] += CHSV(96, 255, 255);
  leds1[pos__] += CHSV(0, 255, 255);

  
  leds2[pos] += CHSV(64, 255, 255);
  leds2[pos_] += CHSV(96, 255, 255);
  leds2[pos__] += CHSV(0, 255, 255);

  
  leds3[pos] += CHSV(64, 255, 255);
  leds3[pos_] += CHSV(96, 255, 255);
  leds3[pos__] += CHSV(0, 255, 255);

  
  leds4[pos] += CHSV(64, 255, 255);
  leds4[pos_] += CHSV(96, 255, 255);
  leds4[pos__] += CHSV(0, 255, 255);

  
  leds5[pos] += CHSV(64, 255, 255);
  leds5[pos_] += CHSV(96, 255, 255);
  leds5[pos__] += CHSV(0, 255, 255);
  
}

void funknebulaFullSinelone()
{
  fadeAllStripsToBlack();
  
  int pos = beatsin16(13, 0, NUM_LEDS);
  int pos_ = beatsin16(7, 0, NUM_LEDS);
  int pos__ = beatsin16(9, 0, NUM_LEDS);

  CHSV master = CHSV(red, 255, 255);

  Serial.println("pos: " + pos);
  leds1[pos] += CHSV(random8(92, 100), 255, random16(128, 255));
  leds1[pos_] += CHSV(random8(60, 68), 255, random16(128, 255));
  leds1[pos__] += CHSV(random8(0, 8), 255, random16(128, 255));

  leds2[pos] += CHSV(random8(92, 100), 255, random16(128, 255));
  leds2[pos_] += CHSV(random8(60, 68), 255, random16(128, 255));
  leds2[pos__] += CHSV(random8(0, 8), 255, random16(128, 255));

  leds3[pos] += CHSV(random8(92, 100), 255, random16(128, 255));
  leds3[pos_] += CHSV(random8(60, 68), 255, random16(128, 255));
  leds3[pos__] += CHSV(random8(0, 8), 255, random16(128, 255));

  leds4[pos] += CHSV(random8(92, 100), 255, random16(128, 255));
  leds4[pos_] += CHSV(random8(60, 68), 255, random16(128, 255));
  leds4[pos__] += CHSV(random8(0, 8), 255, random16(128, 255));

  leds5[pos] += CHSV(random8(92, 100), 255, random16(128, 255));
  leds5[pos_] += CHSV(random8(60, 68), 255, random16(128, 255));
  leds5[pos__] += CHSV(random8(0, 8), 255, random16(128, 255));
  
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeAllStripsToBlack();
  
  int posE = beatsin16( 7, 0, NUM_LEDS-1 );
  int posD = beatsin16( 4, 0, NUM_LEDS-1 );
  
  leds1[posD] += CHSV( gHue, 255, 255);
  leds2[posE] += CHSV( gHue, 255, 255);
  leds3[posD] += CHSV( gHue, 255, 255);
  leds4[posE] += CHSV( gHue, 255, 255);
  leds5[posD] += CHSV( gHue, 255, 255);
}

void whiteAquaPinkSinelon()
{
  fadeAllStripsToBlack();
  
  int posE = beatsin16( 7, 0, NUM_LEDS-1 );
  int posD = beatsin16( 4, 0, NUM_LEDS-1 );
  int posR = beatsin16( 17, 0, NUM_LEDS-1 );
  
  leds1[posR] += CRGB( 255, 255, 255);
  leds2[posE] += CHSV( aqua, 255, 255);
  leds3[posD] += CHSV( pink, 255, 255);
  leds4[posE] += CHSV( aqua, 255, 255);
  leds5[posR] += CRGB( 255, 255, 255);
}

void fadeAllStripsToBlack() {
  fadeToBlackBy( leds1, NUM_LEDS, 255 );
  fadeToBlackBy( leds2, NUM_LEDS, 255 );
  fadeToBlackBy( leds3, NUM_LEDS, 255 );
  fadeToBlackBy( leds4, NUM_LEDS, 255 );
  fadeToBlackBy( leds5, NUM_LEDS, 255 );
}

void greenPurpSinelon()
{
  Serial.println("greenPurpSinelon");
    // a colored dot sweeping back and forth, with fading trails
  fadeAllStripsToBlack();
  
  int posE = beatsin16( 3, 0, NUM_LEDS );
  int posD = beatsin16( 4, 0, NUM_LEDS );
  int posR = beatsin16( 5, 0, NUM_LEDS );
  
  leds1[posR] += CRGB::Snow;
  leds2[posE] += CHSV( blue, 255, 255);
  leds3[posD] += CHSV( purple, 255, 255);
  leds4[posE] += CHSV( aqua, 255, 255);
  leds5[posR] += CRGB::Snow;
}
void eireSinelon()
{
    // a colored dot sweeping back and forth, with fading trails
  fadeAllStripsToBlack();
  
  int posE = beatsin16( 7, 0, NUM_LEDS-1 );
  int posD = beatsin16( 4, 0, NUM_LEDS-1 );
  int posR = beatsin16( 17, 0, NUM_LEDS-1 );
  
  leds1[posR] += CHSV( green, 255, 255);
  leds2[posE] += CHSV( green, 255, 255);
  leds3[posD] += CRGB( 255, 255, 255);
  leds4[posE] += CHSV( orange, 255, 255);
  leds5[posR] += CRGB( orange, 255, 255);
}


void sinelon2()
{
  fadeAllStripsToBlack();
  
  int posE = beatsin16( 20, 0, NUM_LEDS-1 );
  int posD = beatsin16( 7, 0, NUM_LEDS-1 );
  
  leds1[posE] += CHSV( gHue, 255, 255);
  leds2[posD] += CHSV( gHue, 255, 255);
  leds3[posE] += CHSV( gHue, 255, 255);
  leds4[posD] += CHSV( gHue, 255, 255);
  leds5[posE] += CHSV( gHue, 255, 255);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( uint8_t i = 0; i < NUM_LEDS; i++) { //9948
    leds1[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds2[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds3[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds4[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds5[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

// modified this to include some randomness, original looked like above bpm2
// will monitor the changes soon & see what I think!
void bpmCloud()
{
  printCurrentPatternOnce(__func__);
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = CloudColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255 );
  uint8_t randomBeat = random8(10, 100);
  uint8_t randomHue = random8(2, 5);
  for( int i = 0; i < NUM_LEDS; i++ ) { //9948
    leds1[i] = ColorFromPalette(palette, gHue+(i*randomHue), beat-gHue+(randomBeat));
    leds2[i] = ColorFromPalette(palette, gHue+(i*randomHue), beat-gHue+(randomBeat));
    leds3[i] = ColorFromPalette(palette, gHue+(i*randomHue), beat-gHue+(randomBeat));
    leds4[i] = ColorFromPalette(palette, gHue+(i*randomHue), beat-gHue+(randomBeat));
    leds5[i] = ColorFromPalette(palette, gHue+(i*randomHue), beat-gHue+(randomBeat));
  }
}


void bpmForrest()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = ForestColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds1[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds2[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds3[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds4[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds5[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void bpmOcean()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 300;
  CRGBPalette16 palette = OceanColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds1[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds2[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds3[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds4[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds5[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}
