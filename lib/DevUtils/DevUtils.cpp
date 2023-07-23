#include <Arduino.h>

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