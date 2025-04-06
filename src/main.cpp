#include "Arduino.h"
#include "motors.cpp"
#include "comms.cpp"

void main() {
    Serial.begin(115200);
    initMotors();
    initBLE("hercules");
}

void loop() {
    CommandData message;
    getLastMessage(&message);

    

    loopMotors();
}