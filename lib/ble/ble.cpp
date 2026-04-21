#ifdef USE_BLE

#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define bleServerName "ESP32-S3"
#define SERVICE_UUID "010d02e1-35f2-4428-a11f-e54ddbe77cc2"
#define CHARACTERISTIC_UUID "81e65a27-2d9d-4e0f-819f-b9904d85ec35"
#define A_UUID "8e45bd65-cbbc-475e-a5f9-b3b7760c5eef"
#define B_UUID "822255e6-d46b-4f00-aa67-04a53b15c4c2"
bool deviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
    Serial.println("ble connected");
  };
  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
    Serial.println("ble disconnected");
  }
};

BLEServer *pServer = NULL;
BLEService *pService = NULL;
BLECharacteristic *pCharacteristic = NULL;
BLEAdvertising *pAdvertising = NULL;

String ble_detail() {
  BLEServiceMap *map = new BLEServiceMap();
  if (map == NULL)
    return "NULL";
  BLEService *svc = map->getByUUID(SERVICE_UUID);
  String str = map->toString().c_str();
  return str;
}

String ble_server() {
  String str = "";
  if (pServer == NULL) {
    str = "NULL";
  } else {
    str = bleServerName;
  }
  return str;
}

void ble_setup() {
  // Create the BLE Device
  BLEDevice::init(bleServerName);
  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  // Create the BLE Service
  pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setValue("Don't do that Dave."); // Start the service
  pService->start();
  // Start advertising
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  pServer->getAdvertising()->start();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
  // Serial.println("Waiting a client connection to notify...");
}

void ble_loop() {}

#endif
