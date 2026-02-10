#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// PIN Definitions
// TX2 (GPIO 17) -> RX of LIN Click
// RX2 (GPIO 16) -> TX of LIN Click
#define RXD2 16
#define TXD2 17
// Pin to enable LIN Click (CS/EN)
#define LIN_ENABLE_PIN 5 

// UUIDs for Nordic UART Service (Widely supported by iOS Apps like Bluefruit Connect)
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" 
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// Callbacks for Connect/Disconnect
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Device Connected");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Device Disconnected");
    }
};

// Callbacks for receiving data from Phone
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.print("Received from Phone: ");
        for (int i = 0; i < rxValue.length(); i++) {
          Serial.print(rxValue[i]);
          // Forward character to LIN Bus
          Serial2.write(rxValue[i]);
        }
        Serial.println();
      }
    }
};

void setup() {
  Serial.begin(115200);

  // 1. Setup LIN Enable Pin
  if (LIN_ENABLE_PIN != -1) {
    pinMode(LIN_ENABLE_PIN, OUTPUT);
    digitalWrite(LIN_ENABLE_PIN, HIGH);
  }

  // 2. Setup LIN Serial (UART2)
  Serial2.begin(19200, SERIAL_8N1, RXD2, TXD2);

  // 3. Setup BLE
  BLEDevice::init("ESP32_LIN_Control_Ivan"); // Device Name

  // Create Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create TX Characteristic (ESP -> Phone)
  pTxCharacteristic = pService->createCharacteristic(
                    CHARACTERISTIC_UUID_TX,
                    BLECharacteristic::PROPERTY_NOTIFY
                  );
  pTxCharacteristic->addDescriptor(new BLE2902());

  // Create RX Characteristic (Phone -> ESP)
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
                       CHARACTERISTIC_UUID_RX,
                       BLECharacteristic::PROPERTY_WRITE
                     );
  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start Service
  pService->start();

  // Start Advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // funtions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Waiting for iPhone connection...");
}

void loop() {
  // If we receive data from LIN -> Send to Phone
  if (Serial2.available()) {
     int byte = Serial2.read();
     if (deviceConnected) {
        uint8_t val = (uint8_t)byte;
        pTxCharacteristic->setValue(&val, 1);
        pTxCharacteristic->notify();
     }
  }

  // Disconnection handling (Restart advertising)
  if (!deviceConnected && oldDeviceConnected) {
      delay(500); 
      pServer->startAdvertising(); 
      Serial.println("Restart advertising...");
      oldDeviceConnected = deviceConnected;
  }
  // Connection handling
  if (deviceConnected && !oldDeviceConnected) {
      oldDeviceConnected = deviceConnected;
  }
}
