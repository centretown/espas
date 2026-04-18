#include <Arduino.h>
#include "blink.h"
#include <FastLED.h>

UISlider hueSlider("Hue", 128, 0, 255, 1);
UISlider saturationSlider("Saturation", 255, 0, 255, 1);
UISlider valueSlider("Value", 255, 0, 255, 1);
UICheckbox autoHue("Auto Hue", true);

typedef enum : int { RED, GREEN, BLUE } COLOUR;

CRGB rgbLed[1];
#define ledPin LED_PIN
#define numLeds 1

int ledState = RED;

void setup_blink() {
  FastLED.addLeds<SK6812, ledPin, RGB>(rgbLed, numLeds);
  FastLED.setBrightness(25);
  rgbLed[0] = CRGB::Blue;
  FastLED.show();
  ledState = RED;
}

unsigned long lastTimeLed = 0;
unsigned long delayLed = 1000;

void blink() {
  if (timerExpired(lastTimeLed, delayLed)) {
    lastTimeLed = millis();
    switch (ledState) {
    case RED:
      rgbLed[0] = CRGB::Red;
      break;
    case GREEN:
      rgbLed[0] = CRGB::Green;
      break;
    case BLUE:
      rgbLed[0] = CRGB::Blue;
      break;
    }
    FastLED.show();
    ledState++;
    if (ledState > BLUE) {
      ledState = RED;
    }
  }
}
