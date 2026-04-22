#include <Arduino.h>
#if defined(ESP32) || defined(LIBRETINY)
#include <AsyncTCP.h>
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#elif defined(TARGET_RP2040) || defined(TARGET_RP2350) ||                      \
    defined(PICO_RP2040) || defined(PICO_RP2350)
#include <RPAsyncTCP.h>
#include <WiFi.h>
#endif

#include "blink.h"
#include "handlers.h"

#ifdef USE_ROTARY
#include "rotary.h"
#endif

#ifdef USE_BLE
#include "ble.h"
#endif

// credentials.cpp (ignored by git)
// const char *ssid = "myssid";
// const char *password = "mypassword";
extern const char *ssid;
extern const char *password;

// flag to use from web update to reboot the ESP
bool shouldReboot = false;

void setup() {
  Serial.begin(115200);
  delay(500);
// configure encoder pins as inputs
#ifdef USE_ROTARY
  rotary_setup();
#endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
    return;
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  setup_blink();
  setup_handlers();
#ifdef USE_BLE
  ble_setup();
#endif
}

unsigned long lastTimeLog = 0;
unsigned long delayLog = 25000;

unsigned long lastTimeScan = 0;
unsigned long delayScan = 1000;

void loop() {
  if (shouldReboot) {
    Serial.println("Rebooting...");
    delay(100);
    ESP.restart();
  }

  blink();

#ifdef USE_ROTARY
  rotary_loop();
#endif

#ifdef USE_BLE
  ble_loop();
#endif

  if (timerExpired(lastTimeScan, delayScan)) {
    lastTimeScan = millis();
    wsCleanUp();
    wsCheckSensors();
    checkScan();
  }

  if (timerExpired(lastTimeLog, delayLog)) {
    lastTimeLog = millis();
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
    }
  }
}
