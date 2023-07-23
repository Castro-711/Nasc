#include <Arduino.h>
#include <FastLED.h>
#include <RTCDue.h>

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

// put function declarations here:
int myFunction(int, int);
void setupFastLEDs();
void setupRTCTime();
void fadeAllStripsToBlack();

void digitprint(int, int);
int numdigits(int);

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

