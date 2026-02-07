#ifndef LCD_RENDERER_H
#define LCD_RENDERER_H

#include <LiquidCrystal_I2C.h>
#include "config.h"

// LCD row indices
const uint8_t rowTop = 0;
const uint8_t rowBottom = 1;

class LCDRenderer
{
    // LCD hardware controller (I2C interface)
    LiquidCrystal_I2C lcd;

    // State tracking
    SystemState currentState;
    unsigned long stateStartTime;
    bool needsRedraw;

public:
    LCDRenderer() : lcd(lcdI2CAddress, lcdCols, lcdRows), currentState(STATE_INTRO), stateStartTime(0), needsRedraw(true) {}

    // Initialize LCD display with backlight
    void begin()
    {
        lcd.init();
        lcd.backlight();
        lcd.clear();
        stateStartTime = millis();
    }

    // Main update function - renders current system state
    void update(unsigned long currentTime, SystemState state, uint8_t calibStep = 0)
    {
        // Detect state change and trigger full redraw
        if (state != currentState)
        {
            currentState = state;
            stateStartTime = currentTime;
            needsRedraw = true;
        }

        // Render current state only when needed
        if (needsRedraw)
        {
            lcd.clear();
            render(calibStep);
            needsRedraw = false;
        }
    }

    // Display detected color result
    void displayDetectedColor(DetectedColor color)
    {
        lcd.clear();
        printCentered(F("Detected:"), rowTop);
        switch (color)
        {
        case COLOR_RED:
            printCentered(F("RED ball"), rowBottom);
            break;
        case COLOR_GREEN:
            printCentered(F("GREEN ball"), rowBottom);
            break;
        case COLOR_BLUE:
            printCentered(F("BLUE ball"), rowBottom);
            break;
        default:
            break;
        }
        needsRedraw = false;
    }

    // Display unknown color with removal instruction
    void displayUnknownColor()
    {
        lcd.clear();
        printCentered(F("UNKNOWN COLOR"), rowTop);
        printCentered(F("Remove the ball"), rowBottom);
        needsRedraw = false;
    }

    // Display light change / recalibration message
    void displayRecalibration()
    {
        lcd.clear();
        printCentered(F("Light changed!"), rowTop);
        printCentered(F("Recalibrating..."), rowBottom);
        needsRedraw = false;
    }

    // Display "Reading..." message
    void displayReading()
    {
        lcd.clear();
        printCentered(F("Reading..."), rowTop);
        needsRedraw = false;
    }

    // Display ambient calibration instruction
    void displayAmbient()
    {
        lcd.clear();
        printCentered(F("Calibrating"), rowTop);
        printCentered(F("ambient..."), rowBottom);
        needsRedraw = false;
    }

    // Display instruction to remove calibration ball
    void displayRemove()
    {
        lcd.clear();
        printCentered(F("Remove the ball"), rowTop);
        needsRedraw = false;
    }

    // Check if intro screen duration has elapsed
    bool isIntroDone(unsigned long currentTime) const
    {
        return (currentState == STATE_INTRO) && (currentTime - stateStartTime >= introDisplayTime);
    }

    // Force a full screen redraw on next update
    void forceRedraw() { needsRedraw = true; }

private:
    // Print text centered on specified row
    void printCentered(const __FlashStringHelper *text, uint8_t row)
    {
        PGM_P p = reinterpret_cast<PGM_P>(text);
        uint8_t len = strlen_P(p);
        uint8_t col = (len < lcdCols) ? (lcdCols - len) / 2 : 0;

        lcd.setCursor(col, row);
        lcd.print(text);
    }

    // Render appropriate content based on current system state
    void render(uint8_t calibStep)
    {
        switch (currentState)
        {
        case STATE_INTRO:
            printCentered(F("Sorting System"), rowTop);
            printCentered(F("by Gugiu Tudor"), rowBottom);
            break;

        case STATE_CALIBRATION:
            renderCalibration(calibStep);
            break;

        case STATE_READY:
            printCentered(F("System ready!"), rowTop);
            printCentered(F("Place ANY ball"), rowBottom);
            break;

        case STATE_READING:
            printCentered(F("Reading..."), rowTop);
            break;

        case STATE_SHOW_COLOR:
            break;

        case STATE_SORTING:
            printCentered(F("Sorting..."), rowTop);
            break;

        case STATE_RESET:
            printCentered(F("Resetting..."), rowTop);
            break;

        case STATE_AMBIENT_CHECK:
            printCentered(F("Checking"), rowTop);
            printCentered(F("ambient..."), rowBottom);
            break;

        case STATE_SHOW_RECALIBRATION:
            break;
        }
    }

    // Display calibration instructions for current step
    void renderCalibration(uint8_t step)
    {
        switch (step)
        {
        case 0:
            printCentered(F("Place RED ball"), rowTop);
            break;
        case 1:
            printCentered(F("Place GREEN ball"), rowTop);
            break;
        case 2:
            printCentered(F("Place BLUE ball"), rowTop);
            break;
        }
    }
};

#endif