#include <AccelStepper.h>
#include "util.cpp"

#define yStepPin 18
#define yDirPin  19
#define pStepPin 22
#define pDirPin  23

AccelStepper* yawMotor;
AccelStepper* pitchMotor;

void initMotors() {
    yawMotor = new AccelStepper(AccelStepper::DRIVER, yStepPin, yDirPin);
    pitchMotor = new AccelStepper(AccelStepper::DRIVER, pStepPin, pDirPin);

    yawMotor->setMaxSpeed(1000.0);
    pitchMotor->setAcceleration(2000.0);

    yawMotor->setMaxSpeed(1000.0);
    pitchMotor->setAcceleration(2000.0);
}

// target with respect to origin in degrees
// yaw: 0-360, pitch: -90 to 90
void setTarget(double yaw, double pitch) {

    // if more then range restrict it 
    yaw = max(-360, min(360, yaw));
    pitch = max(-90, min(90, pitch));
 
    double yawSteps = yaw * 3200 / 360;
    double pitchSteps = pitch * 3200 / 360;

    yawMotor->moveTo(yaw);
    pitchMotor->moveTo(pitch);
}

// get current position in degrees
// yaw: 0-360, pitch: -90 to 90
double getYaw() {
    return yawMotor->currentPosition() * 360 / 3200;
}
double getPitch() {
    return pitchMotor->currentPosition() * 360 / 3200;
}

void loopMotors() {
    yawMotor->run();
    pitchMotor->run();
}

