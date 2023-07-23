#include <Arduino.h>
#include <FastLED.h>
#include <RTCDue.h>
#include <NascUtils.h>

FASTLED_USING_NAMESPACE

/**
 * Next Steps: 
 * 1. Test the new implementation of the platformio project by deploying it to the arduino
 * 2. create some unit tests & multiple code files 
 * 
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

//CRGB* leds[] = { leds1, leds2, leds3, leds4, leds5 };

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

// colour values
uint8_t red  = 0;
uint8_t orange = 32;
uint8_t yellow = 64;
uint8_t green = 96;
uint8_t aqua = 128;
uint8_t blue = 160;
uint8_t purple = 192;
uint8_t pink = 224;

uint8_t START_SIGNAL = 0;

// put function declarations here:
int myFunction(int, int);
void setupFastLEDs();
void setupRTCTime();
void fadeAllStripsToBlack();

void digitprint(int, int);
int numdigits(int);

void printCurrentPatternOnce(char*);
void printCurrentLoopOnce(char*);
void eveningLoop();
void dayLoop();
void nextEveningPattern();
void nextDayPattern();
void testStripLength();
void testStripAndDataPin();
void sinelonACDC();
void rainbowT2();
void lighteningStripCustom(CRGB, CRGB, CRGB, CRGB);
void lighteningStrip();
void blendStrips();
void lighteningBoltsDec();
void lighteningBoltsInc();
void slowFadeInAndOut();
void fadeAllStripsBy(uint8_t);
void rainbow();
void addGlitter(fract8);
void confetti(); 
void myConfetti();
void reggaeConfetti();
void reggaeSingleSinelon();
//void Fade
void reggaeSinelon();
void irelandFlagSinelon();
void fadeAllStripsToBlack();
void fillFadeInRandomHue();
void fillFadeInRandomHueAndSat();
void fillFadeInHtmlRandom();
void fillFadeInOriginal();
void sinelonEP22();
void stripUpStripDown();
void fadeSingleStripIn();
void stripSwitch();
void stripSwitchFade();
void edgeToCenterBoom(); // a nice overlapping effect whould be lovely nice & quick
void centerToEdgeBoom();
void centerToEdgeFill();
void pulseSinelon();
void reggaeFullSinelone();
void funknebulaFullSinelone();
void sinelon();
void whiteAquaPinkSinelon();
void greenPurpSinelon();
void eireSinelon();
void sinelon2();
void bpm();
void bpmCloud();
void bpmCloudForestAndOcean();
void bpmForest();
void bpmOcean();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("HelloNasc!");
  
  delay(3000); // 3 second delay for recovery

  setupRTCTime();

  setupFastLEDs();

  Serial.println();
  Serial.println("FASTLED.size: ");
  Serial.print(FastLED.size());
  Serial.println();
  Serial.println("FASTLED.brightness: ");
  Serial.print(FastLED.getBrightness());
}

void loop() {
  // put your main code here, to run repeatedly:
  /*  there seems to be a 24 second delay between minutes. Switching between day & night.
    13:16:24.006 -> Day Loop
    13:16:24.115 -> Evening Loop */
  if (START_SIGNAL == 0) {
    printCurrentPatternOnce("START_SIGNAL");
    fill_solid(leds1, NUM_LEDS, CHSV(aqua, 255, 255));
    FastLED.delay(3000);
    fadeAllStripsToBlack();
    START_SIGNAL = 1;
  }
  if (rtc.getMinutes() >= 0 && rtc.getMinutes() <= 30 ) {
    printCurrentLoopOnce("Day Loop");
    dayLoop();
  }
  else if ( rtc.getMinutes() >= 30 && rtc.getMinutes() <= 50) { // >= to try avoid the 24 second delay seen above
    printCurrentLoopOnce("Evening Loop");
    eveningLoop();
  }
  else {
    printCurrentLoopOnce("Fading to black");
    fill_solid(leds1, NUM_LEDS, CRGB::Black);
    fill_solid(leds2, NUM_LEDS, CRGB::Black);
    fill_solid(leds3, NUM_LEDS, CRGB::Black);
    fill_solid(leds4, NUM_LEDS, CRGB::Black);
    fill_solid(leds5, NUM_LEDS, CRGB::Black);
    FastLED.show();
  }
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
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

void fadeAllStripsToBlack() {
  fadeToBlackBy( leds1, NUM_LEDS, 255 );
  fadeToBlackBy( leds2, NUM_LEDS, 255 );
  fadeToBlackBy( leds3, NUM_LEDS, 255 );
  fadeToBlackBy( leds4, NUM_LEDS, 255 );
  fadeToBlackBy( leds5, NUM_LEDS, 255 );
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
void printCurrentPatternOnce(char* pattName) {
  if (pattName != currentPatternName) {
    Serial.println();
    Serial.print("Current pattern: ");
    Serial.println(pattName);
    currentPatternName = pattName;
  }
}

char* currentLoop = "SETUP";
void printCurrentLoopOnce(char* loopName) {
  if (loopName != currentLoop) {
    Serial.println();
    Serial.print("Current Loop: "); 
    Serial.println(loopName);
    currentLoop = loopName;
  }
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList eveningPatterns = {  
  stripSwitch,
  rainbow,
  stripUpStripDown,
  testStripAndDataPin,
  testStripLength,
  sinelonACDC,
  fillFadeInOriginal
};

SimplePatternList dayPatterns = { 
//  slowFadeInAndOut, 
//  stripSwitchFade, 
//  fadeSingleStripIn,
  edgeToCenterBoom,
  centerToEdgeBoom,
  centerToEdgeFill, 
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
  EVERY_N_SECONDS( 17 ) { nextEveningPattern(); } // change patterns periodically
}

// only difference here is that it is faster
void dayLoop() {
    dayPatterns[currentDayPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 50 ) { 
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
  printCurrentPatternOnce("testStripLength");
  Serial.println("testStripLength");

  fadeAllStripsToBlack();
  Serial.println("Num leds => ");
  Serial.println(NUM_LEDS);
  
  for ( uint16_t i = 0; i < NUM_LEDS; i++ ) {
    leds1[i] = CHSV(0, 0, 300);
    FastLED.delay(20);
    Serial.println(i);
  }

  FastLED.delay(10000);
}

void testStripAndDataPin() {
  printCurrentPatternOnce("testStringAndDataPin");
  fadeAllStripsToBlack();

  fill_solid(leds1, NUM_LEDS, CRGB::Blue);
  Serial.println("LED1 is on - Data Pin 3");
  delay(10000);

  fadeAllStripsToBlack();

  fill_solid(leds2, NUM_LEDS, CRGB::Green);
  Serial.println("LED2 is on - Data Pin 5");
  delay(10000);

  fadeAllStripsToBlack();

  fill_solid(leds3, NUM_LEDS, CRGB::Red);
  Serial.println("LED3 is on - Data Pin 7");
  delay(10000);

  fadeAllStripsToBlack();

  fill_solid(leds4, NUM_LEDS, CRGB::Green);
  Serial.println("LED4 is on - Data Pin 9");
  delay(10000);

  fadeAllStripsToBlack();

  fill_solid(leds5, NUM_LEDS, CRGB::Blue);
  Serial.println("LED5 is on - Data Pin 11");
  delay(10000);

  fadeAllStripsToBlack();
}

// first attempt without RGBSet will try that later
void sinelonACDC() {
  printCurrentPatternOnce("sinelonACDC");
  
  for ( uint16_t i = 0; i < NUM_LEDS; i++ ) {
    leds1[i] = CHSV(aqua, rand() % 255, 255 ); 
    leds1[i+1] = CHSV(aqua, rand() % 255, 255 ); 
    leds1[i+2] = CHSV(aqua, rand() % 255, 255 ); 

    leds2[i] = CHSV(blue, rand() % 255, 255 ); 
    leds2[i+1] = CHSV(blue, rand() % 255, 255 ); 
    leds2[i+2] = CHSV(blue, rand() % 255, 255 ); 

    leds3[i] = CHSV(pink, rand() % 100, 255 ); 
    leds3[i+1] = CHSV(aqua, rand() % 100, 255 ); 
    leds3[i+2] = CHSV(blue, rand() % 100, 255 ); 

    leds4[i] = CHSV(green, rand() % 100, 255 ); 
    leds4[i+1] = CHSV(yellow, rand() % 100, 255 ); 
    leds4[i+2] = CHSV(pink, rand() % 100, 255 ); 

    leds5[i] = CHSV(orange, rand() % 100, 255 ); 
    leds5[i+1] = CHSV(purple, rand() % 100, 255 ); 
    leds5[i+2] = CHSV(orange, rand() % 100, 255 ); 
    
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

  for ( uint16_t i = NUM_LEDS; i > 0; i-- ) {
    leds1[i] = CHSV(pink, rand() % 100, 255 );
    leds1[i-1] = CHSV(purple, rand() % 100, 255 );
    leds1[i-2] = CHSV(pink, rand() % 100, 255 );

    leds2[i] = CHSV(green, rand() % 100, 255 );
    leds2[i-1] = CHSV(0, 0, 255 );
    leds2[i-2] = CHSV(orange, rand() % 100, 255 );

    leds3[i] = CHSV(green, rand() % 100, 255 );
    leds3[i-1] = CHSV(0, 0, 255 );
    leds3[i-2] = CHSV(orange, rand() % 100, 255 );

    leds4[i] = CHSV(pink, rand() % 100, 255 );
    leds4[i-1] = CHSV(yellow, rand() % 100, 255 );
    leds4[i-2] = CHSV(purple, rand() % 100, 255 );

    leds5[i] = CHSV(purple, rand() % 100, 100 );
    leds5[i-1] = CHSV(pink, rand() % 100, 100 );
    leds5[i-2] = CHSV(aqua, rand() % 100, 100 );
    FastLED.delay(33);
    leds1[i] = CRGB::Black;
    leds1[i-1] = CRGB::Black;
    leds1[i-2] = CRGB::Black;

    leds2[i] = CRGB::Black;
    leds2[i-1] = CRGB::Black;
    leds2[i-2] = CRGB::Black;

    leds3[i] = CRGB::Black;
    leds3[i-1] = CRGB::Black;
    leds3[i-2] = CRGB::Black;

    leds4[i] = CRGB::Black;
    leds4[i-1] = CRGB::Black;
    leds4[i-2] = CRGB::Black;

    leds5[i] = CRGB::Black;
    leds5[i-1] = CRGB::Black;
    leds5[i-2] = CRGB::Black;
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

void slowFadeInAndOut() {
  printCurrentPatternOnce("slowFadeInAndOut");
  int colourOne = random8(0,255);
  int colourTwo = random8(0,255);
  for (uint8_t i = 0; i < 255; i++){
    fill_solid(leds1, NUM_LEDS, CHSV(colourOne, random8(175, 255), i));
    fill_solid(leds2, NUM_LEDS, CHSV(colourTwo, random8(175, 255), i));
    fill_solid(leds3, NUM_LEDS, CHSV(colourOne, random8(175, 255), i));
    fill_solid(leds4, NUM_LEDS, CHSV(colourTwo, random8(175, 255), i));
    fill_solid(leds5, NUM_LEDS, CHSV(colourTwo, random8(175, 255), i));
    FastLED.delay(random8(35, 90));
  }

  for (uint8_t i = 0; i< 255; i++) {
    fadeAllStripsBy(i);
    FastLED.delay(random8(35, 90));
  }
}

void fadeAllStripsBy(uint8_t fadeFactor) {
  fadeToBlackBy(leds1, NUM_LEDS, fadeFactor);
  fadeToBlackBy(leds2, NUM_LEDS, fadeFactor);
  fadeToBlackBy(leds3, NUM_LEDS, fadeFactor);
  fadeToBlackBy(leds4, NUM_LEDS, fadeFactor);
  fadeToBlackBy(leds5, NUM_LEDS, fadeFactor);
}



// use bseconds to get current time since boot
// binary seconds 1024/1000

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds1, NUM_LEDS, gHue, 100 );
  fill_rainbow( leds2, NUM_LEDS, gHue, 100 );
  fill_rainbow( leds3, NUM_LEDS, gHue, 100 );
  fill_rainbow( leds4, NUM_LEDS, gHue, 100 );
  fill_rainbow( leds5, NUM_LEDS, gHue, 100 );
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
  printCurrentPatternOnce("fillFadeInRandomHue");

  // rand() % 255 = x <= 255
  fill_solid(leds1, NUM_LEDS, CHSV(rand() % 255, 255, random8(90, 255)));
  fill_solid(leds2, NUM_LEDS, CHSV(rand() % 255, 255, random8(90, 255)));
  fill_solid(leds3, NUM_LEDS, CHSV(rand() % 255, 255, random8(90, 255)));
  fill_solid(leds4, NUM_LEDS, CHSV(rand() % 255, 255, random8(90, 255)));
  fill_solid(leds5, NUM_LEDS, CHSV(rand() % 255, 255, random8(90, 255)));
}

void fillFadeInRandomHueAndSat() {

  fadeAllStripsToBlack();
  printCurrentPatternOnce("fillFadeInRandomHueAndSat");

  // rand() % 100 = x <= 100 - saturation & value are apparently 0..100
  fill_solid(leds1, NUM_LEDS, CHSV(rand() % 255, rand() % 100, random8(90, 255)));
  fill_solid(leds2, NUM_LEDS, CHSV(rand() % 255, rand() % 100, random8(90, 255)));
  fill_solid(leds3, NUM_LEDS, CHSV(rand() % 255, rand() % 100, random8(90, 255)));
  fill_solid(leds4, NUM_LEDS, CHSV(rand() % 255, rand() % 100, random8(90, 255)));
  fill_solid(leds5, NUM_LEDS, CHSV(rand() % 255, rand() % 100, random8(90, 255)));
  
}

// not confident this will work well as we need the CHSV colours to change the brightness
void fillFadeInHtmlRandom() {
  fadeAllStripsToBlack();
  printCurrentPatternOnce("fillFadeInHtmlRandom");

  fill_solid(leds1, NUM_LEDS, htmlColours[random8(0, HTML_COLOUR_SIZE)] );
  fill_solid(leds2, NUM_LEDS, htmlColours[random8(0, HTML_COLOUR_SIZE)] );
  fill_solid(leds3, NUM_LEDS, htmlColours[random8(0, HTML_COLOUR_SIZE)] );
  fill_solid(leds4, NUM_LEDS, htmlColours[random8(0, HTML_COLOUR_SIZE)] );
  fill_solid(leds5, NUM_LEDS, htmlColours[random8(0, HTML_COLOUR_SIZE)] );
}

// aqua + purple
void fillFadeInOriginal() {
  fadeAllStripsToBlack();
  printCurrentPatternOnce("fillFadeInOriginal");

  fill_solid(leds1, NUM_LEDS, CHSV(128, 255, random8(90, 255)));
  fill_solid(leds2, NUM_LEDS, CHSV(192, 255, random8(90, 255)));
  fill_solid(leds3, NUM_LEDS, CHSV(128, 255, random8(90, 255)));
  fill_solid(leds4, NUM_LEDS, CHSV(192, 255, random8(90, 255)));
  fill_solid(leds5, NUM_LEDS, CHSV(128, 255, random8(90, 255)));
}


void sinelonEP22() {
  fadeAllStripsToBlack();
  printCurrentPatternOnce("sinelonEP22");
  
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

void stripUpStripDown() {
  fadeAllStripsToBlack();
  printCurrentPatternOnce("stripUpStripDown");
  fill_solid(leds3, NUM_LEDS, CHSV(green, 200, 255));
  // 16 bit int limit 65535
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    leds1[i] = CHSV(100, 0, 255); // white
    leds2[NUM_LEDS - i] = CHSV(orange, 0, 255);
    leds3[i] = CRGB::Black;
    FastLED.delay(10);
  }
}

void fadeSingleStripIn() {
  printCurrentPatternOnce("fadeSingleStripIn");
  for ( uint8_t i = 0; i < 6; i++ ) {
    
    for ( uint8_t y = 0; y < 255; y++ ) {
      if ( i == 0 ) {
        fill_solid(leds1, NUM_LEDS, CHSV(random8(255), random8(175, 255), y));
        FastLED.delay(random8(50, 150)); 
      } // i < 6
      else if (i == 1) {
        fill_solid(leds2, NUM_LEDS, CHSV(random8(255), random8(175, 255), y));
        fadeToBlackBy(leds1, NUM_LEDS, y);
        FastLED.delay(random8(50, 150)); 
      }
      else if (i == 2) {
        fill_solid(leds3, NUM_LEDS, CHSV(random8(255), random8(175, 255), y));
        fadeToBlackBy(leds2, NUM_LEDS, y);
        FastLED.delay(random8(50, 150)); 
      }
      else if (i == 3) {
        fill_solid(leds4, NUM_LEDS, CHSV(random8(255), random8(175, 255), y));
        fadeToBlackBy(leds3, NUM_LEDS, y);
        FastLED.delay(random8(50, 150)); 
      }
      else if (i == 4) {
        fill_solid(leds5, NUM_LEDS, CHSV(random8(255), random8(175, 255), y));
        fadeToBlackBy(leds4, NUM_LEDS, y);
        FastLED.delay(random8(50, 150));
      }
      else {
        fadeToBlackBy(leds5, NUM_LEDS, y);
        FastLED.delay(random8(50, 150));
      }
      
    }
  }
}

// a pattern that switches between full solids of colour & full solids of black
void stripSwitch() {
  fadeAllStripsToBlack();
  printCurrentPatternOnce("stripSwitch");
  
  for (uint8_t i = 0; i < 300; i++ ) {

    if ( i % 2 == 0 ) {
      fill_solid(leds1, NUM_LEDS, CHSV(random8(64, 255), random8(128, 255), random8(200, 255)));
      fill_solid(leds2, NUM_LEDS, CHSV(random8(64, 255), random8(128, 255), random8(200, 255)));
      fill_solid(leds3, NUM_LEDS, CRGB::Black);
      fill_solid(leds4, NUM_LEDS, CRGB::Black);
      fill_solid(leds5, NUM_LEDS, CHSV(random8(64, 255), random8(128, 255), random8(200, 255))); // strip at the banisters
      FastLED.delay(750);
    }
    else {
      fill_solid(leds1, NUM_LEDS, CRGB::Black);
      fill_solid(leds2, NUM_LEDS, CRGB::Black);
      fill_solid(leds3, NUM_LEDS, CHSV(random8(64, 255), random8(128, 255), random8(200, 255)));
      fill_solid(leds4, NUM_LEDS, CHSV(random8(64, 255), random8(128, 255), random8(200, 255)));
      fill_solid(leds5, NUM_LEDS, CRGB::Black);
      FastLED.delay(750);
    }

//    Serial.println(i);
  } // for
}

// a pattern that switches between full solids of colour & full solids of black
void stripSwitchFade() {
  fadeAllStripsToBlack();
  printCurrentPatternOnce("stripSwitchFade");
  
  for (uint8_t i = 0; i < 300; i++ ) {

    if ( i % 2 == 0 ) {
      for ( uint8_t y = 0; y < 255; y++) {
        fill_solid(leds1, NUM_LEDS, CHSV(random8(64, 255), random8(128, 255), y));
        fill_solid(leds2, NUM_LEDS, CHSV(random8(64, 255), random8(128, 255), y));
        fadeToBlackBy(leds3, NUM_LEDS, y);
        fadeToBlackBy(leds4, NUM_LEDS, y);
        fill_solid(leds5, NUM_LEDS, CHSV(random8(64, 255), random8(128, 255), y)); // strip at the banisters
        FastLED.delay(75);
      }      
    }
    else {
      for (uint8_t y = 0; y < 255; y++ ) {
        fadeToBlackBy(leds1, NUM_LEDS, y);        
        fadeToBlackBy(leds2, NUM_LEDS, y);
        fill_solid(leds3, NUM_LEDS, CHSV(random8(64, 255), random8(128, 255), y));
        fill_solid(leds4, NUM_LEDS, CHSV(random8(64, 255), random8(128, 255), y));
        fill_solid(leds5, NUM_LEDS, CRGB::Black);
        FastLED.delay(75);
      }
    }
  } // for
}

void edgeToCenterBoom() { // a nice overlapping effect whould be lovely nice & quick
  fadeAllStripsToBlack();
  printCurrentPatternOnce("edgeToCenterBoom");

  // loop to bring edges to center
  for ( uint16_t i = 0; i < NUM_LEDS; i++ ) {
//    if (i <= NUM_LEDS / 2) {
      leds1[i] = CHSV(aqua, 255, random8(200, 255));
      leds1[NUM_LEDS - i] = CHSV(pink, 255, random8(200, 255)); 

      leds2[i] = CHSV(pink, 255, random8(200, 255));
      leds2[NUM_LEDS - i] = CHSV(aqua, 255, random8(200, 255)); 

      leds3[i] = CHSV(pink, 255, random8(200, 255));
      leds3[NUM_LEDS - i] = CHSV(aqua, 255, random8(200, 255)); 

      leds4[i] = CHSV(pink, 255, random8(200, 255));
      leds4[NUM_LEDS - i] = CHSV(aqua, 255, random8(200, 255)); 

      leds5[i] = CHSV(aqua, 255, random8(200, 255));
      leds5[NUM_LEDS - i] = CHSV(pink, 255, random8(200, 255)); 

      if ( i > 1 ) {
        leds1[i-1] = CHSV(aqua, 255, random8(200, 255));
        leds1[(NUM_LEDS - i) + 1] = CHSV(pink, 255, random8(200, 255)); 

        leds2[i-1] = CHSV(aqua, 255, random8(200, 255));
        leds2[(NUM_LEDS - i) + 1] = CHSV(pink, 255, random8(200, 255)); 

        leds3[i-1] = CHSV(pink, 255, random8(200, 255));
        leds3[(NUM_LEDS - i) + 1] = CHSV(aqua, 255, random8(200, 255)); 

        leds4[i-1] = CHSV(pink, 255, random8(200, 255));
        leds4[(NUM_LEDS - i) + 1] = CHSV(aqua, 255, random8(200, 255)); 

        leds5[i-1] = CHSV(aqua, 255, random8(200, 255));
        leds5[(NUM_LEDS - i) + 1] = CHSV(pink, 255, random8(200, 255)); 
      }
      else {
        leds1[i - 2] = CHSV(aqua, 255, random8(200, 255)); 
        leds1[i - 3] = CRGB::Black;
        
        leds1[(NUM_LEDS - i) + 2] = CHSV(pink, 255, random8(200, 255));
        leds1[(NUM_LEDS - i) + 3] = CRGB::Black;

        leds2[i - 2] = CHSV(aqua, 255, random8(200, 255)); 
        leds2[i - 3] = CRGB::Black;
        
        leds2[(NUM_LEDS - i) + 2] = CHSV(pink, 255, random8(200, 255));
        leds2[(NUM_LEDS - i) + 3] = CRGB::Black;

        leds3[i - 2] = CHSV(aqua, 255, random8(200, 255)); 
        leds3[i - 3] = CRGB::Black;
        
        leds3[(NUM_LEDS - i) + 2] = CHSV(pink, 255, random8(200, 255));
        leds3[(NUM_LEDS - i) + 3] = CRGB::Black;

        leds4[i - 2] = CHSV(aqua, 255, random8(200, 255)); 
        leds4[i - 3] = CRGB::Black;
        
        leds4[(NUM_LEDS - i) + 2] = CHSV(pink, 255, random8(200, 255));
        leds4[(NUM_LEDS - i) + 3] = CRGB::Black;

        leds5[i - 2] = CHSV(aqua, 255, random8(200, 255)); 
        leds5[i - 3] = CRGB::Black;
        
        leds5[(NUM_LEDS - i) + 2] = CHSV(pink, 255, random8(200, 255));
        leds5[(NUM_LEDS - i) + 3] = CRGB::Black;
      }

      FastLED.delay(18);
      
//    } // midpoint boom - revisit this & fill the strip out from center again
//    break;
  }

  for(uint8_t i = 0; i < 100; i++) {
    fadeToBlackBy( leds1, NUM_LEDS, i*2);
    FastLED.delay(125);
  }
 
}

// not working as expected, only half the strip is lighting from center to edgt blue
void centerToEdgeBoom() {
  fadeAllStripsToBlack();
  printCurrentPatternOnce("centerToEdgeBoom");

  const uint8_t MID_STRIP = NUM_LEDS / 2;

  // loop to bring edges to center
  for ( uint16_t i = 0; i < MID_STRIP; i++ ) {
    if (i <= NUM_LEDS) {
      leds1[MID_STRIP + i] = CHSV(blue, 255, random8(200, 255));
      leds1[MID_STRIP - i] = CHSV(green, 255, random8(200, 255));

      leds2[MID_STRIP + i] = CHSV(blue, 255, random8(200, 255));
      leds2[MID_STRIP - i] = CHSV(green, 255, random8(200, 255));

      leds3[MID_STRIP + i] = CHSV(blue, 255, random8(200, 255));
      leds3[MID_STRIP - i] = CHSV(green, 255, random8(200, 255));

      leds4[MID_STRIP + i] = CHSV(blue, 255, random8(200, 255));
      leds4[MID_STRIP - i] = CHSV(green, 255, random8(200, 255));

      leds5[MID_STRIP + i] = CHSV(blue, 255, random8(200, 255));
      leds5[MID_STRIP - i] = CHSV(green, 255, random8(200, 255));

      if ( i > 2 ) {
        leds1[(MID_STRIP + i) -3] = CRGB::Black;
        leds1[(MID_STRIP - i) + 3] = CRGB::Black; 

        leds2[(MID_STRIP + i) -3] = CRGB::Black;
        leds2[(MID_STRIP - i) + 3] = CRGB::Black; 

        leds3[(MID_STRIP + i) -3] = CRGB::Black;
        leds3[(MID_STRIP - i) + 3] = CRGB::Black; 

        leds4[(MID_STRIP + i) -3] = CRGB::Black;
        leds4[(MID_STRIP - i) + 3] = CRGB::Black; 

        leds5[(MID_STRIP + i) -3] = CRGB::Black;
        leds5[(MID_STRIP - i) + 3] = CRGB::Black; 
      }

      FastLED.delay(45);
    } // midpoint boom - revisit this & fill the strip out from center again
  }

//  bpmForest();
}

void centerToEdgeFill() {
  
  fadeAllStripsToBlack();
  printCurrentPatternOnce("centerToEdgeFill");

  uint16_t MID_STRIP = NUM_LEDS / 2;
  Serial.println("MID_STRIP:");
  Serial.println(MID_STRIP);

  // loop to bring edges to center
  for ( uint16_t i = 0; i < MID_STRIP; i++ ) {
//    if (i <= NUM_LEDS) {
      leds1[MID_STRIP + i] = CHSV(green, 255, random8(200, 255));
      leds1[MID_STRIP - i] = CHSV(100, 0, random8(200, 255)); // white
 
      leds2[MID_STRIP + i] = CHSV(100, 0, random8(200, 255)); // white
      leds2[MID_STRIP - i] = CHSV(orange, 255, random8(200, 255)); 


      leds3[MID_STRIP + i] = CHSV(100, 0, random8(200, 255)); // white
      leds3[MID_STRIP - i] = CHSV(orange, 255, random8(200, 255)); 

      leds4[MID_STRIP + i] = CHSV(100, 0, random8(200, 255)); // white
      leds4[MID_STRIP - i] = CHSV(orange, 255, random8(200, 255)); 

      leds5[MID_STRIP + i] = CHSV(100, 0, random8(200, 255)); // white
      leds5[MID_STRIP - i] = CHSV(orange, 255, random8(200, 255)); 
      FastLED.delay(35);
//      break; // cause problems as I did not use it correctory. 
//    } // midpoint boom - revisit this & fill the strip out from center again
  }

  // follow with black to clean fade it out
  for ( uint16_t i = 0; i < MID_STRIP; i++ ) {
    if (i <= NUM_LEDS) {
      leds1[MID_STRIP + i] = CRGB::Black;
      leds1[MID_STRIP - i] = CRGB::Black;

      leds2[MID_STRIP + i] = CRGB::Black;
      leds2[MID_STRIP - i] = CRGB::Black;

      FastLED.delay(17);
      break;
    } 
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
  printCurrentPatternOnce("bpmCloud");
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 43;
  CRGBPalette16 cloud = CloudColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255 );
  uint8_t randomBeat = random8(10, 100);
  uint8_t randomHue = random8(2, 5);
  for( int i = 0; i < NUM_LEDS; i++ ) { //9948
    leds1[i] = ColorFromPalette(cloud, gHue+(i*randomHue), beat-gHue+(randomBeat));
    leds2[i] = ColorFromPalette(cloud, gHue+(i*randomHue), beat-gHue+(randomBeat));
    leds3[i] = ColorFromPalette(cloud, gHue+(i*randomHue), beat-gHue+(randomBeat));
    leds4[i] = ColorFromPalette(cloud, gHue+(i*randomHue), beat-gHue+(randomBeat));
    leds5[i] = ColorFromPalette(cloud, gHue+(i*randomHue), beat-gHue+(randomBeat));
  }
}

//needs work ends up just blue
void bpmCloudForestAndOcean() {
  printCurrentPatternOnce("bpmCloudForestAndOcean");
  uint8_t BeatsPerMinute = 43;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255 );
  uint8_t randomBeat = random8(10, 100);
  uint8_t randomHue = random8(2, 17);
  CRGBPalette16 palette = ForestColors_p;
  for( uint16_t i = 0; i < NUM_LEDS * 3; i++ ) { //9948
    if (i > NUM_LEDS && i < NUM_LEDS * 2) {
      palette = CloudColors_p;
    }
    else if ( i > NUM_LEDS * 2 ) {
      palette = ForestColors_p;
    }
    else {
      palette = OceanColors_p;
    }
    
    leds1[i] = ColorFromPalette(palette, gHue+(i*randomHue), beat-gHue+(randomBeat));
    leds2[i] = ColorFromPalette(palette, gHue+(i*randomHue), beat-gHue+(randomBeat));
    leds3[i] = ColorFromPalette(palette, gHue+(i*randomHue), beat-gHue+(randomBeat));
    leds4[i] = ColorFromPalette(palette, gHue+(i*randomHue), beat-gHue+(randomBeat));
    leds5[i] = ColorFromPalette(palette, gHue+(i*randomHue), beat-gHue+(randomBeat));
    FastLED.delay(10);
  }
}


void bpmForest() {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = ForestColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  uint8_t randomBeat = random8(10, 100);
  uint8_t randomHue = random8(2, 17);
  
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds1[i] = ColorFromPalette(palette, gHue+(i*randomHue), beat-gHue+(randomBeat));
    leds2[i] = ColorFromPalette(palette, gHue+(i*randomHue), beat-gHue+(randomBeat));
    leds3[i] = ColorFromPalette(palette, gHue+(i*randomHue), beat-gHue+(randomBeat));
    leds4[i] = ColorFromPalette(palette, gHue+(i*randomHue), beat-gHue+(randomBeat));
    leds5[i] = ColorFromPalette(palette, gHue+(i*randomHue), beat-gHue+(randomBeat));
  }
}

void bpmOcean()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint16_t BeatsPerMinute = 300;
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
