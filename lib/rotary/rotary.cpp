#ifdef USE_ROTARY

#include <Arduino.h>
#include "rotary.h"
#include "ESPRotary.h"

#define ROTARY_PIN1 6
#define ROTARY_PIN2 5
#define BUTTON_PIN 7
#define CLICKS_PER_STEP 4

ESPRotary rotary;
hw_timer_t *timer = NULL;
int button_val = 0;

void rotate(ESPRotary &r) { Serial.println(r.getPosition()); }

// on left or right rotation
void showDirection(ESPRotary &r) {
  Serial.println(r.directionToString(r.getDirection()));
}

void IRAM_ATTR handleLoop() {
  int val = digitalRead(BUTTON_PIN);
  if (val != button_val) {
    button_val = val;
    if (val == HIGH) {
      Serial.println("Button HIGH");
    } else {
      Serial.println("Button LOW");
    }
  }
  rotary.loop();
}

void rotary_setup() {
  Serial.println("\nROTARY SETUP");
  rotary.begin(ROTARY_PIN1, ROTARY_PIN2, CLICKS_PER_STEP);
  rotary.setChangedHandler(rotate);
  rotary.setLeftRotationHandler(showDirection);
  rotary.setRightRotationHandler(showDirection);
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &handleLoop, true);
  timerAlarmWrite(timer, 10000, true); // every 0.1 seconds
  timerAlarmEnable(timer);

  pinMode(BUTTON_PIN, INPUT); // sets the digital pin 7 as input

  Serial.println("\nROTARY SETUP done");
}

void rotary_loop() {}

#endif
