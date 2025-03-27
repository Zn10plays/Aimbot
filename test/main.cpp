#include <Arduino.h>
#include <AccelStepper.h>
#include "BluetoothSerial.h"

#define hStepPin 22
#define hDirPin 23
#define vStepPin 18
#define vDirPin 19


int last_message_timestamp = 0;
BluetoothSerial SerialBT;
AccelStepper hStepper(AccelStepper::DRIVER, hStepPin, hDirPin);
AccelStepper vStepper(AccelStepper::DRIVER, vStepPin, vDirPin);

struct Message {
  String message;
  int x_error;
  int y_error;
  int timestamp;
};

Message checkForIncommingMessage(BluetoothSerial &Serial_BT) {
  Message m;
  m.message = "NULL";
  m.x_error = 0;
  m.y_error = 0;
  m.timestamp = 0;

  if (Serial_BT.available()) {
    char msg[50];  // Adjust length as needed
    Serial_BT.readStringUntil('\n').toCharArray(msg, 50);

    char message[20];
    int x, y, timestamp;

    if (sscanf(msg, "%s %d %d %d", message, &x, &y, &timestamp) == 4) {
      m.message = String(message);
      m.x_error = x;
      m.y_error = y;
      m.timestamp = timestamp;
    }
  }
  return m;
}

void setup() {
  Serial.begin(115200);
    
  if (!SerialBT.begin("TURC2C")) {
    Serial.println("An error occurred initializing Bluetooth");
  } else {
    Serial.println("Bluetooth initialized. Waiting for messages...");
  }

  hStepper.setMaxSpeed(1000); 
  hStepper.setAcceleration(500);

  vStepper.setMaxSpeed(1000);
  vStepper.setAcceleration(500);

  last_message_timestamp = millis();
}

void loop() {
  hStepper.run();
  vStepper.run(); 

  if (millis() - last_message_timestamp > 1000) {
    Message m = checkForIncommingMessage(SerialBT);

    if (m.message != "NULL") {
      Serial.println("Message received: ");
      Serial.println(m.message);
      Serial.println(m.x_error);
      Serial.println(m.y_error);
      Serial.println(m.timestamp);

      last_message_timestamp = millis();
    }
  }
}
