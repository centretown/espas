#include "handlers.h"
#include "page.h"

#include <mutex>
#include <vector>

#include <ESPAsyncWebServer.h>

static AsyncWebServer server(80);
static AsyncWebSocketMessageHandler wsHandler;
static AsyncWebSocket ws("/ws");           // access at ws://[esp ip]/ws
static AsyncEventSource events("/events"); // event source (Server-Sent events)

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
  Serial.printf("event received\n");
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

static String scanStr;
static bool scanning = false;
static AsyncWebServerRequestPtr requestPtr;

void handleWifiScan(AsyncWebServerRequest *request) {
  requestPtr = request->pause();
  scanning = true;
  Serial.println("WiFi scan requested.");
}

void checkScan() {
  if (!scanning) {
    return;
  }

  scanning = false;
  if (requestPtr.expired()) {
    Serial.printf("WiFi scan request expired.\n");
    return;
  }
  // this takes a while
  Serial.printf("WiFi scan in progress...\n");
  String str = wrapWifiScan();

  if (auto request = requestPtr.lock()) {
    request->send(200, "text/html", str);
    Serial.printf("WiFi scan complete.\n");
  }
}

void handleSensors(AsyncWebServerRequest *request) {
  Serial.println("handleSensors");
  request->send(200, "text/html", wrapSensors());
}

const char *statusTextFmt = "<span id=\"ws-status-text\">%s</span>";
// setup_handlers
//
void setup_handlers() {
  wsHandler.onConnect([](AsyncWebSocket *server, AsyncWebSocketClient *client) {
    Serial.printf("Client %" PRIu32 " connected\n", client->id());
    server->textAll("<span id=\"ws-status-text\">Client (#" +
                    String(client->id()) + ") connected</span>");
    // server->printfAll(statusTextFmt, "Connected: client #" +
    // String(client->id()));
  });

  wsHandler.onDisconnect([](AsyncWebSocket *server, uint32_t clientId) {
    Serial.printf("Client %" PRIu32 " disconnected\n", clientId);
    server->textAll("<span id=\"ws-status-text\">Client (#" + String(clientId) +
                    ") disconnected</span>");
    // server->textAll("Client " + String(clientId) + " disconnected");
    // server->printfAll(statusTextFmt, "Disconnected: client #" + clientId));
  });

  wsHandler.onError([](AsyncWebSocket *server, AsyncWebSocketClient *client,
                       uint16_t errorCode, const char *reason, size_t len) {
    Serial.printf("Client %" PRIu32 " error: %" PRIu16 ": %s\n", client->id(),
                  errorCode, reason);
  });

  wsHandler.onMessage([](AsyncWebSocket *server, AsyncWebSocketClient *client,
                         const uint8_t *data, size_t len) {
    Serial.printf("Client %" PRIu32 " data: %s\n", client->id(),
                  (const char *)data);
    server->textAll(data, len);
  });

  wsHandler.onFragment([](AsyncWebSocket *server, AsyncWebSocketClient *client,
                          const AwsFrameInfo *frameInfo, const uint8_t *data,
                          size_t len) {
    Serial.printf("Client %" PRIu32 " fragment %" PRIu32 ": %s\n", client->id(),
                  frameInfo->num, (const char *)data);
  });

  // attach AsyncWebSocket
  ws.onEvent(wsHandler.eventHandler());
  //
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
  char temp[128];
  sprintf(temp, "Seconds since boot: %u", millis() / 1000);
  events.send(temp, "time"); // send event "time"
}

void wsCleanUp() { ws.cleanupClients(); }

void wsCheckSensors() { ws.textAll(wrapRSSIValue()); }

void wsCheckLED(int id, String color) {
  // Serial.println(wrapLEDValues(id, color));
  ws.textAll(wrapLEDValues(id, color));
}
