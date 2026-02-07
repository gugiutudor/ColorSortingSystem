#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include <Servo.h>
#include "config.h"

class ServoController
{
    // Servo motor instances
    Servo gateServo;
    Servo rampServo;

    // Movement timing tracking
    unsigned long lastMoveTime;
    bool moving;

public:
    ServoController() : lastMoveTime(0), moving(false) {}

    // Initialize servo motors and set default positions
    void begin()
    {
        gateServo.attach(servoGatePin);
        rampServo.attach(servoRampPin);

        // Initialize to default positions
        gateServo.write(servoGateClosed);
        rampServo.write(servoRampDefault);
        delay(servoMoveTime);
        moving = false;
    }

    // Update servo state - check if movement duration has elapsed
    void update(unsigned long currentTime)
    {
        if (moving && (currentTime - lastMoveTime >= servoMoveTime))
            moving = false;
    }

    // Check if servos are currently moving
    bool isBusy() const { return moving; }

    // Close gate to hold ball in place
    void closeGate()
    {
        gateServo.write(servoGateClosed);
        lastMoveTime = millis();
        moving = true;
    }

    // Open gate to release ball
    void openGate()
    {
        gateServo.write(servoGateOpen);
        lastMoveTime = millis();
        moving = true;
    }

    // Position ramp to direct ball to correct sorting bin
    void moveRampToColor(DetectedColor color)
    {
        uint8_t position;

        switch (color)
        {
        case COLOR_RED:
            position = servoRampRed;
            break;
        case COLOR_GREEN:
            position = servoRampGreen;
            break;
        case COLOR_BLUE:
            position = servoRampBlue;
            break;
        default:
            position = servoRampDefault;
            break;
        }

        rampServo.write(position);
        lastMoveTime = millis();
        moving = true;
    }

    // Return ramp to neutral position for next ball
    void moveRampToDefault()
    {
        rampServo.write(servoRampDefault);
        lastMoveTime = millis();
        moving = true;
    }
};

#endif