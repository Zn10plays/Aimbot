#include "BluetoothSerial.h"
#include <cstdio>

struct Message {
    String message;
    int x_error;
    int y_error;
    int timestamp;
};

Message* checkForIncommingMessage(BluetoothSerial Serial_BT) {
    Message *m;
    // m->message = "NULL";
    // m->x_error = 0;
    // m->y_error = 0;
    // m->timestamp = 0;

    // if (Serial_BT.available()) {
    //     const char *msg = Serial_BT.readStringUntil('\n').c_str();

    //     String message;
    //     int* x;
    //     int* y;
    //     int* timestamp;

    //     sscanf(msg, "%s %d %d %d", message, x, y, timestamp);

    //     m->message = message;
    //     m->x_error = *x;
    //     m->y_error = *y;
    //     m->timestamp = *timestamp;
    // }

    return m;
}