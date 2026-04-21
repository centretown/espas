#pragma once
#include <Arduino.h>

void setup_handlers();
void send_time();
void checkScan();
void wsCheckSensors();
void wsCleanUp();
void wsCheckLED(int id, String color);
