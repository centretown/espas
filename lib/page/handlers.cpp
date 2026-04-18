#include "handlers.h"
#include "page.h"

#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");           // access at ws://[esp ip]/ws
AsyncEventSource events("/events"); // event source (Server-Sent events)

void onRequest(AsyncWebServerRequest *request) {
  // Handle Unknown Request
  request->send(404);
}

void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len,
            size_t index, size_t total) {
  // Handle body
}

void onUpload(AsyncWebServerRequest *request, String filename, size_t index,
              uint8_t *data, size_t len, bool final) {
  // Handle upload
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
             AwsEventType type, void *arg, uint8_t *data, size_t len) {
  // Handle WebSocket event
}

void handleOnConnect(AsyncWebServerRequest *request) {
  request->send(200, "text/html", createHTML());
}
void handleNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}
void handleRSSI(AsyncWebServerRequest *request) {
  request->send(200, "text/html", wrapRSSI());
}

static AsyncWebServerRequestPtr requestPtr;
static String scanStr;
static bool scanning = false;

void handleWifiScan(AsyncWebServerRequest *request) {
  if (!scanning) {
    requestPtr = request->pause();
    scanning = true;
  }
}

void checkScan() {
  if (!scanning) {
    return;
  }
  if (requestPtr.expired()) {
    scanning = false;
    return;
  }

  String str = wrapWifiScan();

  if (!requestPtr.expired()) {
    if (auto request = requestPtr.lock()) {
      request->send(200, "text/html", str);
      scanning = false;
    }
  }
}

void handleSensors(AsyncWebServerRequest *request) {
  Serial.println("handleSensors");
  request->send(200, "text/html", wrapSensors());
}

void setup_handlers() {

  // attach AsyncWebSocket
  ws.onEvent(onEvent);
  server.addHandler(&ws);

  // attach AsyncEventSource
  server.addHandler(&events);

  // respond to GET requests on URL /heap
  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });

  // upload a file to /upload
  server.on(
      "/upload", HTTP_POST,
      [](AsyncWebServerRequest *request) { request->send(200); }, onUpload);

  // send a file when /index is requested (SPIFFS example)
  // server.on("/index", HTTP_ALL, [](AsyncWebServerRequest *request){
  //   request->send(SPIFFS, "/index.htm");
  // });

  // send a file when /index is requested (LittleFS example)
  // server.on("/index", HTTP_ALL, [](AsyncWebServerRequest *request){
  //   request->send(LittleFS, "/index.htm");
  // });

  // Simple Firmware Update Form
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html",
                  "<form method='POST' action='/update' "
                  "enctype='multipart/form-data'><input type='file' "
                  "name='update'><input type='submit' value='Update'></form>");
  });
  // server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request){
  //   shouldReboot = !Update.hasError();
  //   AsyncWebServerResponse *response = request->beginResponse(200,
  //   "text/plain", shouldReboot?"OK":"FAIL");
  //   response->addHeader("Connection", "close");
  //   request->send(response);
  // },[](AsyncWebServerRequest *request, String filename, size_t index, uint8_t
  // *data, size_t len, bool final){
  //   if(!index){
  //     Serial.printf("Update Start: %s\n", filename.c_str());
  //     Update.runAsync(true);
  //     if(!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)){
  //       Update.printError(Serial);
  //     }
  //   }
  //   if(!Update.hasError()){
  //     if(Update.write(data, len) != len){
  //       Update.printError(Serial);
  //     }
  //   }
  //   if(final){
  //     if(Update.end(true)){
  //       Serial.printf("Update Success: %uB\n", index+len);
  //     } else {
  //       Update.printError(Serial);
  //     }
  //   }
  // });

  // attach filesystem root at URL /fs (SPIFFS example)
  // server.serveStatic("/fs", SPIFFS, "/");

  // attach filesystem root at URL /fs (LittleFS example)
  // server.serveStatic("/fs", LittleFS, "/");

  // Catch-All Handlers
  // Any request that can not find a Handler that canHandle it
  // ends in the callbacks below.
  server.onNotFound(onRequest);
  server.onFileUpload(onUpload);
  server.onRequestBody(onBody);

  server.on("/", handleOnConnect);
  server.onNotFound(handleNotFound);
  server.on("/rssi", handleRSSI);
  server.on("/scan-wifi", handleWifiScan);
  server.on("/sensors", handleSensors);
  server.begin();
  Serial.println("HTTP server started.");
}

void send_time() {
  static char temp[128];
  sprintf(temp, "Seconds since boot: %u", millis() / 1000);
  events.send(temp, "time"); // send event "time"
}
