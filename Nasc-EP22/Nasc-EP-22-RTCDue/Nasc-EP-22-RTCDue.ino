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

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
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
#define SECTIONS    3
#define SEC_LEN     NUM_LEDS / SECTIONS
#define HUE_OFFSET  4

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

void setup() {
  Serial.begin(115200);
  Serial.print("HelloNasc!");
  
  delay(3000); // 3 second delay for recovery

  setupRTCTime();

  setupFastLEDs();
}

void setupFastLEDs() {
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds1, NUM_LEDS).setCorrection(TypicalLEDStrip);

  FastLED.addLeds<LED_TYPE,DATA_PIN_2,COLOR_ORDER>(leds2, NUM_LEDS).setCorrection(TypicalLEDStrip);

  FastLED.addLeds<LED_TYPE,DATA_PIN_3, COLOR_ORDER>(leds3, NUM_LEDS).setCorrection(TypicalLEDStrip);
  
  FastLED.addLeds<LED_TYPE,DATA_PIN_4,COLOR_ORDER>(leds4, NUM_LEDS).setCorrection(TypicalLEDStrip);

  FastLED.addLeds<LED_TYPE,DATA_PIN_5, COLOR_ORDER>(leds5, NUM_LEDS).setCorrection(TypicalLEDStrip);

  fadeToBlackBy(leds1, NUM_LEDS, 255);
  fadeToBlackBy(leds2, NUM_LEDS, 255);
  fadeToBlackBy(leds3, NUM_LEDS, 255);
  fadeToBlackBy(leds4, NUM_LEDS, 255);
  fadeToBlackBy(leds5, NUM_LEDS, 255);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
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


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList eveningPatterns = {  
  bluePurpSinelon
}; // irelandFlagSinelon, fillFadeIn, pulseSinelon

SimplePatternList dayPatterns = {  
  lighteningBoltsInc, 
  lighteningBoltsDec, 
  lighteningStrip
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

// colours
CRGB greenC = CHSV( 96, 255, 255 );
CRGB whiteC = CRGB( 255, 255, 255 );
CRGB orangeC = CHSV( 32, 255, 255 );

CRGB aquaC = CHSV( random8(128, 138), 255, 255 );
CRGB purpleC = CHSV( random8(192, 202), 255, 255 );
CRGB pinkC = CHSV( random8(224, 234), 255, 255 );

// colour values
uint8_t redV  = 0;
uint8_t oranV = 32;
uint8_t yeloV = 64;
uint8_t grenV = 96;
uint8_t aquaV = 128;
uint8_t blueV = 160;
uint8_t purpV = 192;
uint8_t pinkV = 224;

  
void loop() {
  if ( rtc.getMinutes() <= 15 && rtc.getMinutes() >= 12 ) {
    dayLoop();
    Serial.println("Day Loop");
  }
  else if ( rtc.getMinutes() >= 16 && rtc.getMinutes() <= 18 ) {
    eveningLoop();
    Serial.println("Evening Loop");
  }
  else {
    Serial.print("Fading to black");
    fadeToBlackBy(leds1, NUM_LEDS, 255);
    fadeToBlackBy(leds2, NUM_LEDS, 255);
    fadeToBlackBy(leds3, NUM_LEDS, 255);
    fadeToBlackBy(leds4, NUM_LEDS, 255);
    fadeToBlackBy(leds5, NUM_LEDS, 255);
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
  EVERY_N_MILLISECONDS( 5 ) { 
    gHue++; 
  } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 2 ) // make this a variable & give it a range to increase the randomness
  { nextDayPattern(); } // change patterns periodically 
}
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextEveningPattern()
{
  // add one to the current pattern number, and wrap around at the end
  currentEveningPatternNumber = (currentEveningPatternNumber + 1) % ARRAY_SIZE( eveningPatterns );
}

void nextDayPattern() {
  currentDayPatternNumber = (currentDayPatternNumber + 1) % ARRAY_SIZE( dayPatterns );
}

void rainbowT2() {
  fadeToBlackBy( leds1, NUM_LEDS, 20 );
  fadeToBlackBy( leds2, NUM_LEDS, 20 );
  uint16_t pos = beatsin88( 1792, 0, NUM_LEDS );  // 7 * 256 = 1792
  uint16_t cub = cubicwave8( 1792 );
  leds1[pos] += CHSV( rHue += HUE_OFFSET, 255, 255 );
  leds2[cub] += CHSV( rHue += HUE_OFFSET, 255, 255);
  FastLED.show();
}

uint8_t lower = 192;
uint8_t upper = 255;

uint8_t one = aquaV;
uint8_t two = purpV;
uint8_t three = blueV;
uint8_t four = pinkV;
uint8_t five = grenV;

uint8_t colourArray[5];
//colourArray[0] = one;
//colourArray[1] = two;
//colourArray[2] = three;
//colourArray[3] = four;
//colourArray[4] = five;

uint8_t decCounter = 0;

void lighteningStrip()
{
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  // fade_video (CRGB *leds, uint16_t num_leds, uint8_t fadeBy)
  fill_solid(leds1, NUM_LEDS, CHSV(grenV, 255, 255));
  fill_solid(leds2, NUM_LEDS, CHSV(pinkV, 255, 255));
  fill_solid(leds3, NUM_LEDS, CHSV(blueV, 255, 255));
  fill_solid(leds4, NUM_LEDS, CHSV(purpV, 255, 255));
  fill_solid(leds5, NUM_LEDS, CHSV(aquaV, 255, 255));
}

uint8_t blendOffset = 100;

void blendStrips()
{
  fadeToBlackBy(leds1, NUM_LEDS, 20);

  // seems very static
//  CRGB   blend (const CRGB &p1, const CRGB &p2, fract8 amountOfP2)
  fill_solid(leds1, NUM_LEDS, blend(greenC, whiteC, 1 / blendOffset--));
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
      one = pinkV;
      two = blueV;
      three = aquaV;
      four = grenV;
      five = purpV;
    }
    else if(decCounter % 4 == 0) {
      one = blueV;
      two = purpV;
      three = pinkV;
      four = aquaV;
      five = grenV;
    }
    else if(decCounter % 3 == 0) {
      one = grenV;
      two = aquaV;
      three = purpV;
      four = blueV;
      five = purpV;
    }
    else if(decCounter % 2 == 0) {
      one = aquaV;
      two = grenV;
      three = purpV;
      four = pinkV;
      five = blueV;
    }
    else {
      one = grenV;
      two = pinkV;
      three = blueV;
      four = blueV;
      five = aquaV;
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
      one = pinkV;
      two = blueV;
      three = aquaV;
      four = grenV;
      five = purpV;
    }
    else if(decCounter % 4 == 0) {
      one = blueV;
      two = purpV;
      three = pinkV;
      four = aquaV;
      five = grenV;
    }
    else if(decCounter % 3 == 0) {
      one = grenV;
      two = aquaV;
      three = purpV;
      four = blueV;
      five = purpV;
    }
    else if(decCounter % 2 == 0) {
      one = aquaV;
      two = grenV;
      three = purpV;
      four = pinkV;
      five = blueV;
    }
    else {
      one = grenV;
      two = pinkV;
      three = blueV;
      four = blueV;
      five = aquaV;
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
  fadeToBlackBy( leds1, NUM_LEDS, 10);
  fadeToBlackBy( leds2, NUM_LEDS, 10);
  fadeToBlackBy( leds3, NUM_LEDS, 10);
  fadeToBlackBy( leds4, NUM_LEDS, 10);
  fadeToBlackBy( leds5, NUM_LEDS, 10);
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
  fadeToBlackBy( leds1, NUM_LEDS, 10);
  fadeToBlackBy( leds2, NUM_LEDS, 10);
  fadeToBlackBy( leds3, NUM_LEDS, 10);
  fadeToBlackBy( leds4, NUM_LEDS, 10);
  fadeToBlackBy( leds5, NUM_LEDS, 10);
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
  fadeToBlackBy( leds1, NUM_LEDS, 10);
  fadeToBlackBy( leds2, NUM_LEDS, 10);
  fadeToBlackBy( leds3, NUM_LEDS, 10);
  fadeToBlackBy( leds4, NUM_LEDS, 10);
  fadeToBlackBy( leds5, NUM_LEDS, 10);
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
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  fadeToBlackBy( leds2, NUM_LEDS, 20);
  fadeToBlackBy( leds3, NUM_LEDS, 20);
  fadeToBlackBy( leds4, NUM_LEDS, 20);
  fadeToBlackBy( leds5, NUM_LEDS, 20);


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
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  fadeToBlackBy( leds2, NUM_LEDS, 20);
  fadeToBlackBy( leds3, NUM_LEDS, 20);
  fadeToBlackBy( leds4, NUM_LEDS, 20);
  fadeToBlackBy( leds5, NUM_LEDS, 20);

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
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  fadeToBlackBy( leds2, NUM_LEDS, 20);
  fadeToBlackBy( leds3, NUM_LEDS, 20);
  fadeToBlackBy( leds4, NUM_LEDS, 20);
  fadeToBlackBy( leds5, NUM_LEDS, 20);

   leds1[top] += greenC;
   leds1[t2b] += whiteC;
   leds1[bot] += orangeC;

   leds2[top] += aquaC;
   leds2[t2b] += purpleC;
   leds2[bot] += pinkC;

   leds3[top] += greenC;
   leds3[t2b] += whiteC;
   leds3[bot] += orangeC;

   leds4[top] += aquaC;
   leds4[t2b] += purpleC;
   leds4[bot] += pinkC;

   leds5[top] += greenC;
   leds5[t2b] += whiteC;
   leds5[bot] += orangeC;
}

// fucking class - pulse session - love it...!!
// aqua + purple
void fillFadeIn() 
{
  fadeToBlackBy( leds1, NUM_LEDS, 20 );
  fadeToBlackBy( leds2, NUM_LEDS, 20 );
  fadeToBlackBy( leds3, NUM_LEDS, 20 );
  fadeToBlackBy( leds4, NUM_LEDS, 20 );
  fadeToBlackBy( leds5, NUM_LEDS, 20 );

  fill_solid(leds1, NUM_LEDS, CHSV(128, 255, random8(90, 255)));
  fill_solid(leds2, NUM_LEDS, CHSV(192, 255, random8(90, 255)));
  fill_solid(leds3, NUM_LEDS, CHSV(128, 255, random8(90, 255)));
  fill_solid(leds4, NUM_LEDS, CHSV(192, 255, random8(90, 255)));
  fill_solid(leds5, NUM_LEDS, CHSV(128, 255, random8(90, 255)));
}

// now a pulsating sinelon
void pulseSinelon()
{
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  fadeToBlackBy( leds2, NUM_LEDS, 20);
  fadeToBlackBy( leds3, NUM_LEDS, 20);
  fadeToBlackBy( leds4, NUM_LEDS, 20);
  fadeToBlackBy( leds5, NUM_LEDS, 20);
  
  uint16_t one_ = beatsin16(3, 0, NUM_LEDS);
  uint16_t two_ = beatsin16(3, 0, NUM_LEDS);
  uint16_t three_ = beatsin16(3, 0, NUM_LEDS);
  uint16_t four_ = beatsin16(3, 0, NUM_LEDS);
  uint16_t five_ = beatsin16(3, 0, NUM_LEDS);

  leds1[one_] += CHSV(192, 255, random8(90,255));
  leds2[two_] += CHSV(128, 255, random8(90,255));
  leds3[three_] += CHSV(192, 255, random8(90,255));
  leds4[four_] += CHSV(128, 255, random8(90,255));
  leds5[five_] += CHSV(192, 255, random8(90,255));
}

uint8_t sineColour = 0;
uint8_t red = 0;
uint8_t yellow = 64;
uint8_t green = 96;
// messing with bpm can give a confetti effect
void reggaeFullSinelone()
{
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  fadeToBlackBy( leds2, NUM_LEDS, 20);
  fadeToBlackBy( leds3, NUM_LEDS, 20);
  fadeToBlackBy( leds4, NUM_LEDS, 20);
  fadeToBlackBy( leds5, NUM_LEDS, 20);
  
  
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
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  fadeToBlackBy( leds2, NUM_LEDS, 20);
  fadeToBlackBy( leds3, NUM_LEDS, 20);
  fadeToBlackBy( leds4, NUM_LEDS, 20);
  fadeToBlackBy( leds5, NUM_LEDS, 20);
  
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
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  fadeToBlackBy( leds2, NUM_LEDS, 20);
  fadeToBlackBy( leds3, NUM_LEDS, 20);
  fadeToBlackBy( leds4, NUM_LEDS, 20);
  fadeToBlackBy( leds5, NUM_LEDS, 20);
  
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
    // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  fadeToBlackBy( leds2, NUM_LEDS, 20);
  fadeToBlackBy( leds3, NUM_LEDS, 20);
  fadeToBlackBy( leds4, NUM_LEDS, 20);
  fadeToBlackBy( leds5, NUM_LEDS, 20);
  
  int posE = beatsin16( 7, 0, NUM_LEDS-1 );
  int posD = beatsin16( 4, 0, NUM_LEDS-1 );
  int posR = beatsin16( 17, 0, NUM_LEDS-1 );
  
  leds1[posR] += CRGB( 255, 255, 255);
  leds2[posE] += CHSV( aquaC, 255, 255);
  leds3[posD] += CHSV( pinkC, 255, 255);
  leds4[posE] += CHSV( aquaC, 255, 255);
  leds5[posR] += CRGB( 255, 255, 255);
}

void bluePurpSinelon()
{
    // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  fadeToBlackBy( leds2, NUM_LEDS, 20);
  fadeToBlackBy( leds3, NUM_LEDS, 20);
  fadeToBlackBy( leds4, NUM_LEDS, 20);
  fadeToBlackBy( leds5, NUM_LEDS, 20);
  
  int posE = beatsin16( 7, 0, NUM_LEDS-1 );
  int posD = beatsin16( 4, 0, NUM_LEDS-1 );
  int posR = beatsin16( 17, 0, NUM_LEDS-1 );
  
  leds1[posR] += CRGB( 255, 255, 255);
  leds2[posE] += CHSV( blueV, 255, 255);
  leds3[posD] += CHSV( purpV, 255, 255);
  leds4[posE] += CHSV( aquaV, 255, 255);
  leds5[posR] += CRGB( 255, 255, 255);
}
void eireSinelon()
{
    // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  fadeToBlackBy( leds2, NUM_LEDS, 20);
  fadeToBlackBy( leds3, NUM_LEDS, 20);
  fadeToBlackBy( leds4, NUM_LEDS, 20);
  fadeToBlackBy( leds5, NUM_LEDS, 20);
  
  int posE = beatsin16( 7, 0, NUM_LEDS-1 );
  int posD = beatsin16( 4, 0, NUM_LEDS-1 );
  int posR = beatsin16( 17, 0, NUM_LEDS-1 );
  
  leds1[posR] += CHSV( grenV, 255, 255);
  leds2[posE] += CHSV( grenV, 255, 255);
  leds3[posD] += CRGB( 255, 255, 255);
  leds4[posE] += CHSV( oranV, 255, 255);
  leds5[posR] += CRGB( oranV, 255, 255);
}


void sinelon2()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  fadeToBlackBy( leds2, NUM_LEDS, 20);
  fadeToBlackBy( leds3, NUM_LEDS, 20);
  fadeToBlackBy( leds4, NUM_LEDS, 20);
  fadeToBlackBy( leds5, NUM_LEDS, 20);
  
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
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds1[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds2[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds3[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds4[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds5[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void bpm2()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds1[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds2[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds3[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds4[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds5[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void bpmCloud()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = CloudColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds1[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds2[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds3[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds4[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    leds5[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
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
