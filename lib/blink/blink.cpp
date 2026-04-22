#include "blink.h"
#include "handlers.h"
#include <Arduino.h>
#include <FastLED.h>

UISlider hueSlider("Hue", 128, 0, 255, 1);
UISlider saturationSlider("Saturation", 255, 0, 255, 1);
UISlider valueSlider("Value", 255, 0, 255, 1);
UICheckbox autoHue("Auto Hue", true);

CRGB rgbLed[1];
#define ledPin LED_PIN
#define numLeds 1

int ledState = LED_RED;

String currentColor(CRGB rgb) {
  char buffer[20];
  snprintf(buffer, sizeof(buffer), "%d,%d,%d", rgb.red, rgb.green, rgb.blue);
  String str = buffer;
  return str;
}

void setup_blink() {
  FastLED.addLeds<SK6812, ledPin, GRB>(rgbLed, numLeds);
  FastLED.setBrightness(25);
  rgbLed[0] = CRGB::Blue;
  FastLED.show();
  ledState = LED_RED;
}

unsigned long lastTimeLed = 0;
unsigned long delayLed = 1000;

void blink() {
  if (!timerExpired(lastTimeLed, delayLed))
    return;
  lastTimeLed = millis();
  switch (ledState) {
  case LED_RED:
    rgbLed[0] = CRGB::Red;
    break;
  case LED_GREEN:
    rgbLed[0] = CRGB::Green;
    break;
  case LED_BLUE:
    rgbLed[0] = CRGB::Blue;
    break;
  }

  FastLED.show();

  wsCheckLED(0, currentColor(rgbLed[0]));

  if (++ledState >= LED_LAST) {
    ledState = LED_RED;
  }
}
