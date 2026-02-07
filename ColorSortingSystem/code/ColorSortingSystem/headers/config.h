#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// LCD I2C pins (hardware I2C)
const uint8_t lcdSdaPin = A4;
const uint8_t lcdSclPin = A5;

// Servo motor pins
const uint8_t servoGatePin = 9;
const uint8_t servoRampPin = 10;

// TCS230 color sensor pins
const uint8_t sensorS0Pin = 2;
const uint8_t sensorS1Pin = 3;
const uint8_t sensorS2Pin = 4;
const uint8_t sensorS3Pin = 5;
const uint8_t sensorOutPin = 6;

// LCD configuration
const uint8_t lcdI2CAddress = 0x27;
const uint8_t lcdCols = 16;
const uint8_t lcdRows = 2;

// Servo positions (degrees)
const uint8_t servoGateClosed = 167;
const uint8_t servoGateOpen = 120;
const uint8_t servoRampRed = 40;
const uint8_t servoRampGreen = 75;
const uint8_t servoRampBlue = 110;
const uint8_t servoRampDefault = 75;

// TCS230 frequency scaling (S0=HIGH, S1=LOW = 20% scaling)
const uint8_t sensorFreqScaleS0 = HIGH;
const uint8_t sensorFreqScaleS1 = LOW;

// Sampling parameters
const uint8_t noiseSamples = 20;
const uint8_t samples = 10;
const uint16_t sensorReadTimeout = 100000; // microseconds
const uint16_t sensorStabilizeDelay = 50;  // milliseconds
const uint8_t numColors = 3;

// Threshold calculation parameters
const float sigmaRuleMultiplier = 2.0; // 68–95–99.7 rule
const float thresholdPercentage = 0.5; // 50% of minimum distance

// Timing constants (milliseconds)
const uint16_t introDisplayTime = 5000;     // intro screen duration
const uint16_t servoMoveTime = 500;         // time for servo movement
const uint16_t ballDropTime = 1000;         // time for ball to drop into bin
const uint16_t ballStabilizeTime = 500;     // time for ball to stabilize on sensor
const uint16_t ambientCheckTime = 500;      // time for ambient check stabilization
const uint16_t displayDuration = 1000;      // standard message display duration
const uint16_t displayColorDuration = 2000; // time for color display
const uint16_t displayRecalibration = 2000; // time for recalibration message

// Detected color types
enum DetectedColor
{
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_UNKNOWN
};

// System state machine states
enum SystemState
{
    STATE_INTRO,
    STATE_CALIBRATION,
    STATE_READY,
    STATE_READING,
    STATE_SHOW_COLOR,
    STATE_SORTING,
    STATE_RESET,
    STATE_AMBIENT_CHECK,
    STATE_SHOW_RECALIBRATION
};

#endif