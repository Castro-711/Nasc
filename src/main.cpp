#include <FastLED.h>
#include <RTCDue.h>
#include <NascUtils.h>
#include <NascUtils.cpp>
#include <DevUtils.h>

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

// RTC variables
RTCDue rtc(XTAL);
const char* daynames[]={"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

uint8_t START_SIGNAL = 0;

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