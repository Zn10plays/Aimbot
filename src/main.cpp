#include <Arduino.h>
#include <AccelStepper.h>

#define hStepPin 22
#define hDirPin 23

#define vStepPin 18
#define vDirPin 19

// 3200

AccelStepper hStepper(AccelStepper::DRIVER, hStepPin, hDirPin);
AccelStepper vStepper(AccelStepper::DRIVER, vStepPin, vDirPin);

void setup() {
  Serial.begin(115200);

  hStepper.setMaxSpeed(1000); 
  hStepper.setAcceleration(500);

  vStepper.setMaxSpeed(1000);
  vStepper.setAcceleration(500);

  vStepper.moveTo(800);
}

void loop() {
  hStepper.run();
  vStepper.run(); 
}
