#pragma once
void setup_blink();
void blink();
inline bool timerExpired(unsigned long lastTime, unsigned long delay) {
  return (millis() - lastTime >= delay);
}
