#include "page.h"
#include <WiFi.h>
#include <cstdint>
#include <cstdio>

#ifdef USE_BLE
#include "ble.h"
#endif

#ifdef ESP32_S3_MINI
#include "esp32_s3_mini.html.h"
#elif ESP32_S3
#include "esp32_s3.html.h"
#endif

const char *cardHtml = "<div class=\"w3-card w3-padding w3-asphalt\">";
const char *labelHtml = "<label class=\"\">";
const char *endLabel = "</label>";
const char *valueHtml =
    "<div class=\"w3-text-amber\" style=\"margin-left:1rem;\">";
const char *endSpan = "</span>";
const char *endDiv = "</div>";
const char *valueHtmlID =
    "<div id=\"%s\" class=\"w3-text-amber\" style=\"margin-left:1rem;\">";

String wrapHTML(String begin, String content, String end) {
  String str = begin;
  str += content;
  str += end;
  return str;
}

String createHTML() { return pageHtml; }

String wrapCardContent(String content) { return cardHtml + content + endDiv; }

const char *spinner =
    "<span id=\"%s\" ><i class=\"w3-margin-left htmx-indicator fa fa-spinner "
    "fa-spin fa-2x fa-fw\"></i></span>";
String wrapSpinner(String id) {
  char buffer[160];
  snprintf(buffer, sizeof(buffer), spinner, id);
  String str = buffer;
  return str;
}

const char *gridHtml = "<div class=\"w3-grid w3-margin\" "
                       "style=\"gap:8px;grid-template-columns:repeat(auto-fit,"
                       "minmax(150px,1fr))\">";
String wrapGrid(String content) { return wrapHTML(gridHtml, content, endDiv); }

String wrapLabel(String title) { return wrapHTML(labelHtml, title, endLabel); }

const char *gridId = "<div id=\"%s\" class=\"w3-grid w3-margin\" "
                     "style=\"gap:8px;grid-template-columns:repeat(auto-fit,"
                     "minmax(150px,1fr))\">";
String wrapGridId(String id, String content) {
  char gridBuffer[512];
  snprintf(gridBuffer, sizeof(gridBuffer), gridId, id);
  return wrapHTML(gridBuffer, content, endDiv);
}

const char *titleHtml = "<div class=\"w3-padding w3-dark-grey w3-margin-top\">";
String wrapHeader(String title) { return wrapHTML(titleHtml, title, endDiv); }

String wrapRSSIValue() { return wrapValue("rssi-stream", WiFi.RSSI()); }

String wrapRSSI() {
  return wrapLabel("Signal Strength (RSSI)") + wrapRSSIValue();
}

String wrapCardItems(String title, int value) {
  String str = cardHtml;

  str += labelHtml;
  str += title;
  str += endLabel;

  str += valueHtml;
  str += value;
  str += endDiv;

  str += endDiv;

  return str;
}

// const char *cardUpdate = "<div id=\"%s\" class=\"w3-card w3-padding
// w3-asphalt "
//                          "w3-hover-cobalt w3-border w3-border-green\" >";
// String wrapUpdateCard(String id, String content) {
//   char cardDiv[160];
//   snprintf(cardDiv, sizeof(cardDiv), cardUpdate, id);
//   String str = cardDiv;
//   str += content;
//   str += endDiv;
//   return str;
// }

const char *cardAction = "<div class=\"w3-card w3-padding w3-asphalt "
                         "w3-hover-cobalt w3-border w3-border-blue\" "
                         "hx-get=\"/%s\" hx-swap=\"innerHTML\">";
String wrapActionCard(String id, String (*wrapper)()) {
  char cardDiv[256];
  snprintf(cardDiv, sizeof(cardDiv), cardAction, id);
  String str = cardDiv;
  str += wrapper();
  str += endDiv;
  return str;
}

const char *titleAction =
    "<div class=\"w3-padding w3-dark-grey w3-margin-top w3-hover-cobalt "
    "w3-border w3-border-blue\" hx-get=\"/%s\" "
    "hx-swap=\"none\" hx-indicator=\"#%s\" hx-select-oob=\"#%s\">";

String wrapActionTitle(String title, String id, String spinId) {
  char div[512];
  snprintf(div, sizeof(div), titleAction, id, spinId, id);
  String str = div;
  str += title;
  str += wrapSpinner(spinId);
  str += endDiv;
  str += "<div id=\"" + id + "\"></div>";
  return str;
}

String wrapWifiScan() {
  // int16_t n = WiFi.scanNetworks(bool async = false, bool show_hidden = false,
  //               bool passive = false, uint32_t max_ms_per_chan = 300,
  //               uint8_t channel = 0, const char *ssid = nullptr,
  //               const uint8_t *bssid = nullptr);
  int16_t network_count =
      WiFi.scanNetworks(false, false, false, 300, 0, nullptr, nullptr);
  String str = "";
  for (int16_t i = 0; i < network_count; i++) {
    str += wrapCard(WiFi.SSID(i), WiFi.RSSI(i));
  }
  return wrapGridId("scan-wifi", str);
}

String wrapWiFi() {
  String wifi_str = wrapCard("Local IP Address", WiFi.localIP().toString());
  wifi_str += wrapCard("Local MAC Address", WiFi.macAddress());
  wifi_str += wrapCard("Connection (SSID)", WiFi.SSID());
  wifi_str += wrapActionCard("rssi", wrapRSSI);
  wifi_str += wrapCard("Router MAC (BSSID)", WiFi.BSSIDstr());
  String str = wrapGrid(wifi_str);
  str += wrapActionTitle("Scan Networks", "scan-wifi", "spin-scan");
  return str;
}

#ifdef USE_BLE
String BLEHeader() {
  String ble_str = "Bluetooth LE: " + ble_server();
  return ble_str;
}
#endif

const char *ledFormat = "<i id=\"ws-rgb-%d\" class=\"fa fa-circle w3-xlarge\" "
                        "style=\"color:rgb(%s);\"></i>";
String wrapLEDValues(int id, String color) {
  char buffer[128];
  snprintf(buffer, sizeof(buffer), ledFormat, id, color);
  return String(buffer);
}

const char *spanId = "<span id=\"%s\">";
String wrapSpanId(String id, String content) {
  char buffer[60];
  snprintf(buffer, sizeof(buffer), spanId, id);
  return String(buffer) + content + String(endSpan);
}

#ifdef USE_ROTARY
String wrapRotaryDirectionValue(const char *direction) {
  return wrapSpanId("rdir", String(direction));
}

String wrapRotaryDirection(const char *direction) {
  return wrapLabel("Rotary Direction") + wrapRotaryDirectionValue(direction);
}

String wrapRotaryPositionValue(int position) {
  return wrapSpanId("rpos", String(position));
}

String wrapRotaryPosition(int position) {
  return wrapLabel("Rotary Position") + wrapRotaryPositionValue(position);
}

String wrapRotaryButtonValue(int state) {
  String button_state = (state == 1) ? "up" : "down";
  return wrapSpanId("rbutton", button_state);
}

String wrapRotaryButton(int state) {
  return wrapLabel("Rotary Button") + wrapRotaryButtonValue(state);
}
#endif // USE_ROTARY

String wrapSensors() {
  // String str = "<div id=\"sensors\">";
  String str = wrapHeader("Wifi");
  str += wrapWiFi();
#ifdef USE_BLE
  str += wrapHeader(BLEHeader());
  str += wrapGrid(wrapCard("Detail", ble_detail()));
#endif
#ifdef USE_ROTARY
  str += wrapHeader("Rotary");
  str += wrapGrid(wrapCard("Position", wrapRotaryPositionValue(0)) +
                  wrapCard("Button", wrapRotaryButtonValue(1)) +
                  wrapCard("Direction", wrapRotaryDirectionValue("")));
#endif
  str += wrapHeader("LED");
  str += wrapGrid(wrapCard("Status: ", wrapLEDValues(0, "0,0,0")));
  // str += wrapHeader("Temperature");
  return str;
}
