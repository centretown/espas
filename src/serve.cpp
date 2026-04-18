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

// credentials.cpp (ignored by git)
// const char *ssid = "myssid";
// const char *password = "mypassword";
extern const char *ssid;
extern const char *password;

// flag to use from web update to reboot the ESP
bool shouldReboot = false;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
    return;
  }
  setup_blink();
  setup_handlers();
}

unsigned long lastTimeLog = 0;
unsigned long delayLog = 5000;

void loop() {
  if (shouldReboot) {
    Serial.println("Rebooting...");
    delay(100);
    ESP.restart();
  }
  send_time();

  blink();
  checkScan();

  if (timerExpired(lastTimeLog, delayLog)) {
    lastTimeLog = millis();
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
    }
  }

}
