#pragma once
#include <Arduino.h>

typedef enum : int { LED_RED, LED_GREEN, LED_BLUE, LED_LAST } LEDState;

void setup_blink();
void blink();
String currentColor();
inline bool timerExpired(unsigned long lastTime, unsigned long delay) {
  return (millis() - lastTime >= delay);
}
