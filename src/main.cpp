#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "4fafc201-1fb5-459e-8fcc-c5c9c331914b" // Custom service UUID
#define CHARACTERISTIC_UUID_RX "beb5483e-36e1-4688-b7f5-ea07361b26a8" // Characteristic for receiving data (write)
#define CHARACTERISTIC_UUID_TX "beb5483f-36e1-4688-b7f5-ea07361b26a8" // Characteristic for sending data (notify)

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic_TX = NULL; // For sending data (notify)
BLECharacteristic* pCharacteristic_RX = NULL; // For receiving data (write)
bool deviceConnected = false;
bool oldDeviceConnected = false;
std::string receivedData = "";  // Store received data

// Callback for connection/disconnection events
class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        Serial.println("Device connected!");
    };

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        Serial.println("Device disconnected!");
    }
};

// Callback for when the central device writes to the RX characteristic
class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        std::string rxValue = pCharacteristic->getValue();

        if (rxValue.length() > 0) {
            Serial.print("Received Value: ");
            for (int i = 0; i < rxValue.length(); i++) {
                Serial.print(rxValue[i]);
            }
            Serial.println();

            receivedData = rxValue; // Store the received data

            // Example: Echo back the received data + " - Received!"
            std::string response = receivedData + " - Received!";
            pCharacteristic_TX->setValue(response);
            pCharacteristic_TX->notify();
            Serial.println("Sent confirmation: " + String(response.c_str()));

        }
    }
};

void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE work!");

    BLEDevice::init("TUTC2C"); // Give it a name

    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService* pService = pServer->createService(SERVICE_UUID);

    // Create the TX Characteristic (for sending - notify)
    pCharacteristic_TX = pService->createCharacteristic(
        CHARACTERISTIC_UUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharacteristic_TX->addDescriptor(new BLE2902());  // Add the standard Client Characteristic Configuration Descriptor

    // Create the RX Characteristic (for receiving - write)
    pCharacteristic_RX = pService->createCharacteristic(
        CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_WRITE
    );
    pCharacteristic_RX->setCallbacks(new MyCallbacks());

    pService->start();

    // Start advertising
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); 
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("Start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }

    if (deviceConnected) {
        // Example: Send a message every 2 seconds
        static unsigned long lastSendTime = 0;
        if (millis() - lastSendTime > 2000) {
            String message = "Hello from ESP32! Time: " + String(millis());
            pCharacteristic_TX->setValue(message.c_str());
            pCharacteristic_TX->notify();
            Serial.println("Sent: " + message);
            lastSendTime = millis();
        }
    }

    delay(10); // Small delay to avoid busy-waiting
}