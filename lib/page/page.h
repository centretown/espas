#pragma once
#include <Arduino.h>

String createHTML();
String wrapHeader(String title);
String wrapGrid(String content);
String wrapSensors();
String wrapWiFi();
String wrapRSSI();
String wrapRSSIValue();
String wrapWifiScan();
String wrapLEDValues(int id, String color);

#ifdef USE_ROTARY
String wrapRotaryDirectionValue(const char *direction);
String wrapRotaryPositionValue(int position);
String wrapRotaryButtonValue(int state);
#endif

extern const char *cardHtml;
extern const char *labelHtml;
extern const char *endLabel;
extern const char *valueHtml;
extern const char *endSpan;
extern const char *endDiv;
extern const char *valueHtmlID;

template <typename T> String wrapCard(String title, T value) {
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

template <typename T> String wrapValue(String id, T value) {
  char valueDiv[128];
  snprintf(valueDiv, sizeof(valueDiv), valueHtmlID, id);
  String str = valueDiv;
  str += value;
  str += endDiv;
  return str;
}

// template <typename T> String wrapValueID(String id, String title, T value) {
//   char valueDiv[128];
//   snprintf(valueDiv, sizeof(valueDiv), valueHtmlID, id);

//   String str = cardHtml;

//   str += labelHtml;
//   str += title;
//   str += endLabel;

//   str += valueDiv;
//   str += value;
//   str += endDiv;

//   str += endDiv;

//   return str;
// }
