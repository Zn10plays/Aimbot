#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <AccelStepper.h>

#include <vector>

struct CommandData {
    std::string command;
    int x;
    int y;
    int xerror;
    int yerror;
    long long time; // Use long long for large timestamps
};

CommandData parseMessage(const std::string& message) {
    CommandData data;
    size_t start = 0, end;
    std::vector<std::string> tokens;

    // Tokenize the message using ',' as a delimiter
    while ((end = message.find(',', start)) != std::string::npos) {
        tokens.push_back(message.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(message.substr(start)); // Last token

    // Assign values to struct fields
    if (tokens.size() == 6) {
        data.command = tokens[0];
        data.x = std::stoi(tokens[1]);
        data.y = std::stoi(tokens[2]);
        data.xerror = std::stof(tokens[3]);
        data.yerror = std::stof(tokens[4]);
        data.time = std::stoll(tokens[5]);
    }

    return data;
}
#define SERVICE_UUID           "4fafc201-1fb5-459e-8fcc-c5c9c331914b" // Custom service UUID
#define CHARACTERISTIC_UUID_RX "beb5483e-36e1-4688-b7f5-ea07361b26a8" // Characteristic for receiving data (write)
#define CHARACTERISTIC_UUID_TX "beb5483f-36e1-4688-b7f5-ea07361b26a8" // Characteristic for sending data (notify)

#define hStepPin 22
#define hDirPin 23
#define vStepPin 18
#define vDirPin 19

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic_TX = NULL; // For sending data (notify)
BLECharacteristic* pCharacteristic_RX = NULL; // For receiving data (write)
bool deviceConnected = false;
bool oldDeviceConnected = false;
std::string receivedData = "";  // Store received data

AccelStepper hStepper(AccelStepper::DRIVER, hStepPin, hDirPin);
AccelStepper vStepper(AccelStepper::DRIVER, vStepPin, vDirPin);

bool target_detected = false;
int last_message_timestamp = 0;
CommandData* previousCommand = NULL;

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
            // Serial.print("Received Value: ");
            // for (int i = 0; i < rxValue.length(); i++) {
            //     Serial.print(rxValue[i]);
            // }
            // Serial.println();

            CommandData command = parseMessage(rxValue);

            if (previousCommand != NULL) {
                delete previousCommand;
            }
            previousCommand = new CommandData(command);
            last_message_timestamp = millis();

            // Example: Echo back the received data + " - Received!"
            std::string response = rxValue + " - Received!";
            pCharacteristic_TX->setValue(response);
            pCharacteristic_TX->notify();
            // Serial.println("Sent confirmation: " + String(response.c_str()));

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


    hStepper.setMaxSpeed(1000); 
    hStepper.setAcceleration(700);
  
    vStepper.setMaxSpeed(1000);
    vStepper.setAcceleration(700);

    last_message_timestamp = millis();
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

    // if (deviceConnected) {
    //     // Example: Send a message every 2 seconds
    //     static unsigned long lastSendTime = 0;
    //     if (millis() - lastSendTime > 2000) {
    //         String message = "Hello from ESP32! Time: " + String(millis());
    //         pCharacteristic_TX->setValue(message.c_str());
    //         pCharacteristic_TX->notify();
    //         Serial.println("Sent: " + message);
    //         lastSendTime = millis();
    //     }
    // }

    if (last_message_timestamp == 0) {
        last_message_timestamp = millis();
        Serial.println("Waiting for messages...");
    } else if (millis() - last_message_timestamp > 1000) {
        previousCommand = NULL;
        target_detected = false;
        Serial.println("No messages received for 1 second");
        delay(500);
    } else if (previousCommand == NULL) {
        Serial.println("No previous command");
        delay(500);
    } else if (vStepper.distanceToGo() == 0 && hStepper.distanceToGo() == 0) {
        Serial.println("setting target_detected to true");
        target_detected = true;

        // vStepper.move(-previousCommand->yerror * .3);
        // hStepper.move(previousCommand->xerror * .3);
        hStepper.moveTo(previousCommand->x * 3200/360);
    } else {
        // vStepper.setAcceleration(-previousCommand->yerror * 30);
        hStepper.setAcceleration(previousCommand->xerror * 35);
    }

     // Small delay to avoid busy-waiting
    hStepper.run();
    // vStepper.run();
}