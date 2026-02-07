#ifndef COLOR_SENSOR_H
#define COLOR_SENSOR_H

#include <Arduino.h>
#include "config.h"

// RGB color data structure for sensor readings
struct RGBColor
{
    uint16_t r, g, b;

    RGBColor() : r(0), g(0), b(0) {}
    RGBColor(uint16_t red, uint16_t green, uint16_t blue) : r(red), g(green), b(blue) {}
};

class ColorSensor
{
    // Calibration reference data
    RGBColor ambient;
    RGBColor refColors[numColors];

    // Dynamic thresholds
    float ballDetectionThreshold;
    float colorMatchThreshold;
    float ambientChangeThreshold;

    // Calibration state tracking
    bool ambientCalibrated;
    bool colorsCalibrated;
    bool thresholdsCalculated;
    uint8_t calibrationStep;

public:
    ColorSensor() : ambientCalibrated(false), colorsCalibrated(false), thresholdsCalculated(false),
                    calibrationStep(0), ballDetectionThreshold(0), colorMatchThreshold(0), ambientChangeThreshold(0) {}

    // Initialize TCS230 color sensor pins and frequency scaling
    void begin()
    {
        pinMode(sensorS0Pin, OUTPUT);
        pinMode(sensorS1Pin, OUTPUT);
        pinMode(sensorS2Pin, OUTPUT);
        pinMode(sensorS3Pin, OUTPUT);
        pinMode(sensorOutPin, INPUT);

        // Set frequency scaling to 20% (S0=HIGH, S1=LOW)
        digitalWrite(sensorS0Pin, sensorFreqScaleS0);
        digitalWrite(sensorS1Pin, sensorFreqScaleS1);
    }

    // Full reset for recalibration
    void reset()
    {
        ambientCalibrated = false;
        colorsCalibrated = false;
        thresholdsCalculated = false;
        calibrationStep = 0;
        ballDetectionThreshold = 0;
        colorMatchThreshold = 0;
        ambientChangeThreshold = 0;
        ambientChangeThreshold = 0;
    }

    // Capture ambient light baseline and calculate threshold for detecting significant
    // ambient changes using the 68–95–99.7 rule, making the system environment-independent
    void calibrateAmbient()
    {
        // Collect multiple samples to measure noise
        RGBColor readings[noiseSamples];
        for (uint8_t i = 0; i < noiseSamples; i++)
        {
            readings[i] = readSingle();
            delay(sensorStabilizeDelay);
        }

        // Calculate average as ambient reference
        uint32_t sumR = 0, sumG = 0, sumB = 0;
        for (uint8_t i = 0; i < noiseSamples; i++)
        {
            sumR += readings[i].r;
            sumG += readings[i].g;
            sumB += readings[i].b;
        }
        ambient = RGBColor(sumR / noiseSamples, sumG / noiseSamples, sumB / noiseSamples);

        // Calculate standard deviation to measure noise level
        float sumSqDist = 0;
        for (uint8_t i = 0; i < noiseSamples; i++)
        {
            float dist = colorDistance(readings[i], ambient);
            sumSqDist += dist * dist;
        }
        float stdDev = sqrt(sumSqDist / noiseSamples);

        // Set threshold
        ambientChangeThreshold = stdDev * sigmaRuleMultiplier;
        ambientCalibrated = true;
    }

    // Begin color calibration sequence
    void startCalibration()
    {
        calibrationStep = 0;
        colorsCalibrated = false;
        thresholdsCalculated = false;
    }

    // Calibrate next color in sequence - returns true when all colors are done
    bool calibrateNextColor()
    {
        refColors[calibrationStep] = readAverage(samples);
        calibrationStep++;

        if (calibrationStep >= numColors)
        {
            colorsCalibrated = true;
            calculateDynamicThresholds();
            return true;
        }
        return false;
    }

    // Calculate all dynamic thresholds based on calibrated values
    void calculateDynamicThresholds()
    {
        if (!ambientCalibrated || !colorsCalibrated)
            return;

        // Calculate ball detect threshold based on ambient-to-color distance
        float distToColors[numColors];
        for (uint8_t i = 0; i < numColors; i++)
            distToColors[i] = colorDistance(ambient, refColors[i]);

        float minDistToColor = findMinimum(distToColors, numColors);
        ballDetectionThreshold = minDistToColor * thresholdPercentage;

        // Calculate color match threshold based on inter-color separation
        float interColorDist[3];
        interColorDist[0] = colorDistance(refColors[0], refColors[1]); // red-green
        interColorDist[1] = colorDistance(refColors[0], refColors[2]); // red-blue
        interColorDist[2] = colorDistance(refColors[1], refColors[2]); // green-blue

        float minInterColorDist = findMinimum(interColorDist, numColors);
        colorMatchThreshold = minInterColorDist * thresholdPercentage;

        // Set ambient change threshold based on measured noise
        ambientChangeThreshold = ambientChangeThreshold;
        thresholdsCalculated = true;
    }

    // Get current calibration step
    uint8_t getCalibrationStep() const { return calibrationStep; }

    // Detect if a ball is present by comparing to ambient light
    bool isBallPresent()
    {
        if (!ambientCalibrated)
            return false;

        RGBColor current = readAverage(samples);
        float dist = colorDistance(current, ambient);
        float threshold = thresholdsCalculated ? ballDetectionThreshold : ambientChangeThreshold;

        return dist > threshold;
    }

    // Check if ambient lighting conditions have changed significantly
    bool hasAmbientChanged()
    {
        if (!ambientCalibrated || !thresholdsCalculated)
            return false;

        RGBColor current = readAverage(samples);
        float dist = colorDistance(current, ambient);
        return dist > ambientChangeThreshold;
    }

    // Identify ball color by comparing to calibrated references
    uint8_t detectColor()
    {
        if (!colorsCalibrated || !thresholdsCalculated)
            return COLOR_UNKNOWN;

        RGBColor current = readAverage(samples);

        // Find closest matching color
        uint8_t minIndex = 0;
        float minDistance = colorDistance(current, refColors[0]);

        for (uint8_t i = 1; i < numColors; i++)
        {
            float dist = colorDistance(current, refColors[i]);
            if (dist < minDistance)
            {
                minDistance = dist;
                minIndex = i;
            }
        }

        // Return unknown if distance exceeds threshold
        if (minDistance > colorMatchThreshold)
            return COLOR_UNKNOWN;

        return minIndex;
    }

private:
    // Find minimum value in an array
    float findMinimum(const float arr[], uint8_t size)
    {
        float minVal = arr[0];
        for (uint8_t i = 1; i < size; i++)
            if (arr[i] < minVal)
                minVal = arr[i];
        return minVal;
    }

    // Calculate Euclidean distance between two RGB colors
    float colorDistance(const RGBColor &c1, const RGBColor &c2)
    {
        int32_t dr = (int32_t)c1.r - (int32_t)c2.r;
        int32_t dg = (int32_t)c1.g - (int32_t)c2.g;
        int32_t db = (int32_t)c1.b - (int32_t)c2.b;
        return sqrt((float)(dr * dr + dg * dg + db * db));
    }

    // Read single color channel from TCS230 sensor
    uint16_t readChannel(bool s2, bool s3)
    {
        digitalWrite(sensorS2Pin, s2);
        digitalWrite(sensorS3Pin, s3);
        delay(sensorStabilizeDelay);
        return pulseIn(sensorOutPin, LOW, sensorReadTimeout);
    }

    // Read all three color channels once
    RGBColor readSingle()
    {
        return RGBColor(
            readChannel(LOW, LOW),   // Red filter
            readChannel(HIGH, HIGH), // Green filter
            readChannel(LOW, HIGH)   // Blue filter
        );
    }

    // Read and average multiple samples for stable readings
    RGBColor readAverage(uint8_t samples)
    {
        uint32_t sumR = 0, sumG = 0, sumB = 0;

        for (uint8_t i = 0; i < samples; i++)
        {
            RGBColor reading = readSingle();
            sumR += reading.r;
            sumG += reading.g;
            sumB += reading.b;
        }

        return RGBColor(sumR / samples, sumG / samples, sumB / samples);
    }
};

#endif