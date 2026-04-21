#include "blink.h"
#include <Arduino.h>
#include <FastLED.h>
#include "handlers.h"

UISlider hueSlider("Hue", 128, 0, 255, 1);
UISlider saturationSlider("Saturation", 255, 0, 255, 1);
UISlider valueSlider("Value", 255, 0, 255, 1);
UICheckbox autoHue("Auto Hue", true);

CRGB rgbLed[1];
#define ledPin LED_PIN
#define numLeds 1

int ledState = LED_RED;

void setup_blink() {
  FastLED.addLeds<SK6812, ledPin, RGB>(rgbLed, numLeds);
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

  wsCheckLED(0, currentColor());

  if (++ledState >= LED_LAST) {
    ledState = LED_RED;
  }
}

String currentColor() {
  String str = "white";
  switch (ledState) {
  case LED_RED:
    str = "red";
    break;
  case LED_GREEN:
    str = "green";
    break;
  case LED_BLUE:
    str = "blue";
    break;
  }
  return str;
}
