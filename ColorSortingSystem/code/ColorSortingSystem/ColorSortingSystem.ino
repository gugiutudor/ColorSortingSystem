#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#include "headers/config.h"
#include "headers/ColorSensor.h"
#include "headers/ServoController.h"
#include "headers/LCDRenderer.h"
#include "headers/SystemController.h"

// Global component instances
ColorSensor colorSensor;
ServoController servoController;
LCDRenderer lcdRenderer;
SystemController systemController(colorSensor, servoController, lcdRenderer);

void setup() {
    // Initialize system
    systemController.begin();
}

void loop() {
    // Main system update
    systemController.update();
}

