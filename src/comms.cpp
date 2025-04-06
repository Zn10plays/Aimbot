#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "util.cpp"

#define SERVICE_UUID           "4fafc201-1fb5-459e-8fcc-c5c9c331914b" // Custom service UUID
#define CHARACTERISTIC_UUID_RX "beb5483e-36e1-4688-b7f5-ea07361b26a8" // Characteristic for receiving data (write)
#define CHARACTERISTIC_UUID_TX "beb5483f-36e1-4688-b7f5-ea07361b26a8" // Characteristic for sending data (notify)

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic_TX = NULL; // For sending data (notify)
BLECharacteristic* pCharacteristic_RX = NULL; // For receiving data (write)
bool deviceConnected = false;
bool oldDeviceConnected = false;
CommandData* receivedData = nullptr;  // Store received data
long last_message_timestamp = 0; // Store the last message timestamp

class ServerConnectionCallbacks: public BLEServerCallbacks {
public:
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        Serial.println("Device connected!");
    };

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        Serial.println("Device disconnected!");
    }
};

class ReceiveCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {
        std::string rxValue = pCharacteristic->getValue();

        if (rxValue.length() > 0) {
            Serial.print("Received Value: ");
            for (int i = 0; i < rxValue.length(); i++) {
                Serial.print(rxValue[i]);
            }
            Serial.println();
            
            // Process the received command
            CommandData command;
            
            parseMessage(rxValue, &command);
            
            if (receivedData == nullptr) {
                last_message_timestamp = millis();
                receivedData = new CommandData(command);
                return;
            }

            if (receivedData->time < command.time) {
                return;
            }

            last_message_timestamp = millis();
            delete receivedData;
            receivedData = new CommandData(command);

            // if debugging is enabled, print the received command
            #ifdef DEBUG
                Serial.print("Command: ");
                Serial.print(receivedData->command.c_str());
                Serial.print(", X: ");
                Serial.print(receivedData->x);
                Serial.print(", Y: ");
                Serial.print(receivedData->y);
                Serial.print(", X Error: ");
                Serial.print(receivedData->xerror);
                Serial.print(", Y Error: ");
                Serial.print(receivedData->yerror);
                Serial.print(", Time: ");
                Serial.println(receivedData->time);
            #endif;
        }
    }
};

// Function to initialize BLE server and characteristics
void initBLE(char* name) {
    BLEDevice::init(name); // Initialize BLE device with a name
    pServer = BLEDevice::createServer(); // Create BLE server
    pServer->setCallbacks(new ServerConnectionCallbacks()); // Set connection callbacks

    // Create service and characteristics
    BLEService* pService = pServer->createService(SERVICE_UUID);
    pCharacteristic_RX = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
    pCharacteristic_TX = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);

    // Add descriptor for notifications
    pCharacteristic_TX->addDescriptor(new BLE2902());

    // Set callbacks for receiving data
    pCharacteristic_RX->setCallbacks(new ReceiveCallbacks());

    // Start the service and advertising
    pService->start();

    BLEAdvertising* pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID); // Add service UUID to advertising data
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x12);
    pAdvertising->start();
}


// function to get last message 
void getLastMessage(CommandData* returnDest) {
    if (receivedData != nullptr) {
        pipeMessage(receivedData, returnDest);
    } else {
        returnDest->command = "none";
        returnDest->x = 0;
        returnDest->y = 0;
        returnDest->xerror = 0;
        returnDest->yerror = 0;
        returnDest->time = 0;
    }
}