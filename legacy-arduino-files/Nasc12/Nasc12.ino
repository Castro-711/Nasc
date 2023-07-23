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
#define DATA_PIN_2  13
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    300
CRGB leds1[NUM_LEDS]; // led array
CRGB leds2[NUM_LEDS];

#define BRIGHTNESS         255
#define FRAMES_PER_SECOND  60

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

void setup() {

  Serial.begin(9600);
  
  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds1, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  FastLED.addLeds<LED_TYPE,DATA_PIN_2,COLOR_ORDER>(leds2, NUM_LEDS).setCorrection(TypicalLEDStrip);

  

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { irelandFlagSinelon }; // taking Glitter & juggle out

//SimplePatternList gPatterns = { myRainbow, myRainbow, myRainbow, bpmLava, bpmHeat, bpmCloud, bpmForrest, bpmOcean, confettiGreenAndWhite, pinkAndWhite, dualSolid, rainbow, rainbowWithGlitter, confetti, sinelon, sinelon2, bpm, confettiGreenAndWhite, dualSolid, pinkAndWhite }; // taking Glitter & juggle out

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
uint8_t flagCounter = 0;
  
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { 
    gHue++; 
  } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
  
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void dualSolid()
{
  // setting the random colours for the dual_solid()
  uint8_t red = random(0, 256);
  uint8_t blue = random(0, 256);
  uint8_t green = random(0, 256);

  uint8_t red2 = blue;
  uint8_t blue2 = green;
  uint8_t green2 = red;

  CRGB one = CRGB(red, green, blue);
  CRGB two = CRGB(red2, green2, blue2);
  
  fill_solid( leds1, NUM_LEDS, 255 );
  delay(1000);

  fadeToBlackBy( leds1, NUM_LEDS, 255);
  delay(1000);
};

void pinkAndWhite()
{
  fill_solid( leds1, NUM_LEDS, CRGB::White );
  delay(1000);
  fadeToBlackBy(leds1, NUM_LEDS, 10);
  delay(1000);
}

void ChangePalettePeriodically()
{
    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;
    
    if( lastSecond != secondHand) {
        lastSecond = secondHand;
        if( secondHand ==  0)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if( secondHand == 10)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
        if( secondHand == 15)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
        if( secondHand == 40)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 45)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
  }
}  

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds1, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
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
  int posD = random16(NUM_LEDS);
  leds1[posD] += CHSV( gHue + random8(64), 200, 255);
}

void myConfetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds1, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds1[pos] += CHSV( gHue + random8(32), 255, 255);
}

void reggaeConfetti()
{
  fadeToBlackBy( leds1, NUM_LEDS, 10);
  uint8_t red = random16(NUM_LEDS);
  uint8_t yellow = random16(NUM_LEDS);
  uint8_t green = random16(NUM_LEDS);
  leds1[red] += CHSV( random8(0, 16), 255, 255);
  leds1[yellow] += CHSV( random8(64, 80), 255, 255);
  leds1[green] += CHSV( random8(96, 112), 255, 255);
}
//
//void reggaeSingleSinelon()
//{
//  fadeToBlackBy( leds1, NUM_LEDS, 20);
//  uint8_t red = beatsin16( 7, 0, 100);
//  uint8_t yellow = beatsin16( 7, 101, 200);
//  uint8_t green = beatsin16( 7, 201, 300);
//
//   leds1[red] += CHSV( 0, 255, 255);
//   leds1[yellow] += CHSV( 96, 255, 255);
//   leds1[green] += CHSV( 64, 255, 255);
//}


void irelandFlagSinelon()
{
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  fadeToBlackBy( leds2, NUM_LEDS, 20);
  uint8_t red = beatsin16( 7, 0, 100);
  uint8_t yellow = beatsin16( 7, 101, 200);
  uint8_t green = beatsin16( 7, 201, 300);

  uint8_t ry = beatsin16(7, 0, 200);
  uint8_t rg = beatsin16(7, 0, 300);
  uint8_t gy = beatsin16(7, 100, 200);

  CRGB greenC = CHSV( 96, 255, 255);
  CRGB whiteC = CRGB(255, 255, 255);
  CRGB orangeC = CHSV( 32, 255, 255);

  CRGB aquaC = CHSV( random8(128, 138), 255, 255);
  CRGB purpleC = CHSV( random8(192, 202), 255, 255);
  CRGB pinkC = CHSV( random8(224, 234), 255, 255);

   leds1[red] += greenC;
   leds1[rg] += whiteC;
   leds1[green] += orangeC;

   leds2[red] += aquaC;
   leds2[rg] += purpleC;
   leds2[green] += pinkC;
   

//   if(flagCounter % 5 == 0) {
//    leds1[ry] += CHSV(108, 255, 255);
//    leds1[rg] += CRGB(255, 255, 255);
//    leds1[gy] += CHSV(40, 255, 255);
//   }

   
    flagCounter++;
}

uint8_t sineColour = 0;
uint8_t red = 0;
uint8_t yellow = 64;
uint8_t green = 96;
// messing with bpm can give a confetti effect
void reggaeFullSinelone()
{
  fadeToBlackBy( leds1, NUM_LEDS, 20);
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

    
//    if(sineColour % 1 == 0){
//      master = CHSV(red, 255, 255);
//    }
//  }

  sineColour++;
  leds1[pos] += CHSV(64, 255, 255);
  leds1[pos_] += CHSV(96, 255, 255);
  leds1[pos__] += CHSV(0, 255, 255);
  
}

void funknebulaFullSinelone()
{
  fadeToBlackBy( leds1, NUM_LEDS, 50);
  int pos = beatsin16(13, 0, NUM_LEDS);
  int pos_ = beatsin16(7, 0, NUM_LEDS);
  int pos__ = beatsin16(9, 0, NUM_LEDS);

  CHSV master = CHSV(red, 255, 255);

  Serial.println("pos: " + pos);
  leds1[pos] += CHSV(random8(92, 100), 255, random16(128, 255));
  leds1[pos_] += CHSV(random8(60, 68), 255, random16(128, 255));
  leds1[pos__] += CHSV(random8(0, 8), 255, random16(128, 255));
  
}

void realSin()
{
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  
  int pos = sin8(random8(255));

  CHSV master = CHSV(random(0, 255), 255, 255);

  leds1[pos] += master;
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  int posE = beatsin16( 1, 0, NUM_LEDS-1 );
  int posD = beatsin16( 3, 0, NUM_LEDS-1 );
  leds1[posD] += CHSV( gHue, 255, 192);
}

void sinelon2()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  int posE = beatsin16( 30, 0, NUM_LEDS-1 );
  int posD = beatsin16( 13, 0, NUM_LEDS-1 );
  leds1[posE] += CHSV( gHue, 255, 127 );
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds1[i % 60] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void bpm2()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds1[i % 90] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void bpmCloud()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = CloudColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds1[i % 90] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void bpmForrest()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = ForestColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds1[i % 90] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
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
  }
}

void bpmLava()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = LavaColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds1[i % 90] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void myRainbow()
{
  for( int i = 0; i <= NUM_LEDS; i = i + 6) {

    fill_rainbow(leds1, i, 0, 10);
    leds1[i - 5].setRGB(0,0,0);
    FastLED.show();
    delay(50);
  }

  FastLED.clear();
  
}

void bpmHeat()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = HeatColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds1[i % 90] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds1, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds1[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 0, 127); // CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
