#ifndef SYSTEM_CONTROLLER_H
#define SYSTEM_CONTROLLER_H

#include "config.h"
#include "ColorSensor.h"
#include "ServoController.h"
#include "LCDRenderer.h"

class SystemController
{
    ColorSensor &sensor;
    ServoController &servos;
    LCDRenderer &lcd;

    SystemState state;
    unsigned long stateTimer;
    uint8_t step;
    DetectedColor detectedColor;
    bool isUnknownColor;

public:
    SystemController(ColorSensor &s, ServoController &srv, LCDRenderer &l)
        : sensor(s), servos(srv), lcd(l), state(STATE_INTRO),
          stateTimer(0), step(0), detectedColor(COLOR_UNKNOWN), isUnknownColor(false) {}

    void begin()
    {
        sensor.begin();
        servos.begin();
        lcd.begin();
        state = STATE_INTRO;
        stateTimer = millis();
    }

    void update()
    {
        unsigned long now = millis();
        servos.update(now);

        switch (state)
        {
        case STATE_INTRO:
            handleIntro(now);
            break;
        case STATE_CALIBRATION:
            handleCalibration(now);
            break;
        case STATE_READY:
            handleReady(now);
            break;
        case STATE_READING:
            handleReading(now);
            break;
        case STATE_SHOW_COLOR:
            handleShowColor(now);
            break;
        case STATE_SORTING:
            handleSorting(now);
            break;
        case STATE_RESET:
            handleReset(now);
            break;
        case STATE_AMBIENT_CHECK:
            handleAmbientCheck(now);
            break;
        case STATE_SHOW_RECALIBRATION:
            handleShowRecalibration(now);
            break;
        }

        lcd.update(now, state, sensor.getCalibrationStep());
    }

private:
    void changeState(SystemState newState)
    {
        state = newState;
        stateTimer = millis();
        step = 0;
        lcd.forceRedraw();
    }

    bool elapsed(unsigned long now, uint16_t duration) { return now - stateTimer >= duration; }

    // STATE_INTRO - step 0 = show welcome, step 1 = show ambient message, step 2 = calibrate ambient
    void handleIntro(unsigned long now)
    {
        if (step == 0)
        {
            if (!elapsed(now, introDisplayTime))
                return;
            lcd.displayAmbient();
            stateTimer = now;
            step = 1;
        }
        else if (step == 1 && elapsed(now, displayDuration))
        {
            sensor.calibrateAmbient();
            sensor.startCalibration();
            changeState(STATE_CALIBRATION);
        }
    }

    // STATE_CALIBRATION - step 0 = wait ball, 1 = stabilize, 2 = show reading, 3 = read, 4 = wait removal
    void handleCalibration(unsigned long now)
    {
        switch (step)
        {
        case 0:
            if (sensor.isBallPresent())
            {
                stateTimer = now;
                step = 1;
            }
            break;
        case 1:
            if (elapsed(now, ballStabilizeTime))
            {
                lcd.displayReading();
                stateTimer = now;
                step = 2;
            }
            break;
        case 2:
            if (elapsed(now, displayDuration))
                step = 3;
            break;
        case 3:
            sensor.calibrateNextColor();
            lcd.displayRemove();
            stateTimer = now;
            step = 4;
            break;
        case 4:
            if (!sensor.isBallPresent())
            {
                if (sensor.getCalibrationStep() >= numColors)
                    changeState(STATE_READY);
                else
                {
                    stateTimer = now;
                    step = 0;
                    lcd.forceRedraw();
                }
            }
            break;
        }
    }

    void handleReady(unsigned long now)
    {
        if (sensor.isBallPresent())
            changeState(STATE_READING);
    }

    // STATE_READING - step 0 = stabilize, step 1 = detect
    void handleReading(unsigned long now)
    {
        if (step == 0 && elapsed(now, displayDuration))
            step = 1;
        else if (step == 1)
        {
            detectedColor = static_cast<DetectedColor>(sensor.detectColor());
            isUnknownColor = (detectedColor == COLOR_UNKNOWN);
            changeState(STATE_SHOW_COLOR);
        }
    }

    // STATE_SHOW_COLOR - known = display then sort, unknown = display then wait removal
    void handleShowColor(unsigned long now)
    {
        if (step == 0)
        {
            if (isUnknownColor)
                lcd.displayUnknownColor();
            else
                lcd.displayDetectedColor(detectedColor);
            step = 1;
        }
        else if (isUnknownColor)
        {
            if (!sensor.isBallPresent())
                changeState(STATE_READY);
        }
        else if (elapsed(now, displayColorDuration))
            changeState(STATE_SORTING);
    }

    // STATE_SORTING - step 0 = move ramp, 1 = wait ramp, 2 = open gate, 3 = wait drop, 4 = wait display
    void handleSorting(unsigned long now)
    {
        switch (step)
        {
        case 0:
            servos.moveRampToColor(detectedColor);
            step = 1;
            break;
        case 1:
            if (!servos.isBusy())
                step = 2;
            break;
        case 2:
            servos.openGate();
            stateTimer = now;
            step = 3;
            break;
        case 3:
            if (elapsed(now, ballDropTime))
            {
                stateTimer = now;
                step = 4;
            }
            break;
        case 4:
            if (elapsed(now, displayDuration))
                changeState(STATE_RESET);
            break;
        }
    }

    // STATE_RESET - step 0 = close gate, 1 = wait, 2 = ramp default, 3 = wait, 4 = delay then ambient check
    void handleReset(unsigned long now)
    {
        switch (step)
        {
        case 0:
            servos.closeGate();
            step = 1;
            break;
        case 1:
            if (!servos.isBusy())
                step = 2;
            break;
        case 2:
            servos.moveRampToDefault();
            step = 3;
            break;
        case 3:
            if (!servos.isBusy())
            {
                stateTimer = now;
                step = 4;
            }
            break;
        case 4:
            if (elapsed(now, displayDuration))
                changeState(STATE_AMBIENT_CHECK);
            break;
        }
    }

    // STATE_AMBIENT_CHECK - step 0 = show, 1 = stabilize, 2 = check
    void handleAmbientCheck(unsigned long now)
    {
        switch (step)
        {
        case 0:
            if (elapsed(now, displayDuration))
            {
                stateTimer = now;
                step = 1;
            }
            break;
        case 1:
            if (elapsed(now, ambientCheckTime))
                step = 2;
            break;
        case 2:
            changeState(sensor.hasAmbientChanged() ? STATE_SHOW_RECALIBRATION : STATE_READY);
            break;
        }
    }

    // STATE_SHOW_RECALIBRATION - step 0 = display recalibration message, step 1 = sensor reset, back to STATE_INTRO
    void handleShowRecalibration(unsigned long now)
    {
        if (step == 0)
        {
            lcd.displayRecalibration();
            step = 1;
        }
        else if (elapsed(now, displayRecalibration))
        {
            sensor.reset();
            changeState(STATE_INTRO);
        }
    }
};

#endif