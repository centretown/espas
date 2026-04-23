#ifdef USE_ROTARY

#include "rotary.h"
#include "ESPRotary.h"
#include "handlers.h"
// #include "page.h"
#include <Arduino.h>

#define ROTARY_PIN1 6
#define ROTARY_PIN2 5
#define BUTTON_PIN 7
#define CLICKS_PER_STEP 4

ESPRotary rotary;
hw_timer_t *timer = NULL;
int button_previous = 0;
int position_previous = -1;
rotary_direction direction_previous = (rotary_direction)2;

void checkPosition(ESPRotary &r) {
  int position = r.getPosition();
  if (position != position_previous) {
    position_previous = position;
    wsUpdateRotaryPosition(position);
  }
}

void checkDirection(ESPRotary &r) {
  rotary_direction direction = r.getDirection();
  if (direction != direction_previous) {
    direction_previous = direction;
    wsUpdateRotaryDirection(r.directionToString(direction).c_str());
  }
}

void checkButton() {
  int button = digitalRead(BUTTON_PIN);
  if (button != button_previous) {
    button_previous = button;
    wsUpdateRotaryButton(button);
  }
}

void rotary_setup() {
  Serial.println("\nROTARY SETUP");
  rotary.begin(ROTARY_PIN1, ROTARY_PIN2, CLICKS_PER_STEP);
  rotary.setChangedHandler(checkPosition);
  rotary.setLeftRotationHandler(checkDirection);
  rotary.setRightRotationHandler(checkDirection);
  // timer = timerBegin(0, 80, true);
  // timerAttachInterrupt(timer, &handleLoop, true);
  // timerAlarmWrite(timer, 10000, true); // every 0.1 seconds
  // timerAlarmEnable(timer);

  pinMode(BUTTON_PIN, INPUT); // sets the digital pin 7 as input

  Serial.println("\nROTARY SETUP done");
}

void rotary_loop() {
  rotary.loop();
  checkButton();
  // checkPosition();
  // checkDirection();
}

#endif
