#include <FastLED.h>

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

extern CRGB leds1[];
extern CRGB leds2[];
extern CRGB leds3[];
extern CRGB leds4[];
extern CRGB leds5[];

#define BRIGHTNESS         255
#define FRAMES_PER_SECOND  60

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