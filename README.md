# ColorSortingSystem

This repository contains the implementation of an automated color-based ball sorting system, developed in the 3rd year at the Faculty of Mathematics and Computer Science, University of Bucharest, as part of the Introduction to Robotics course.

## 1. General description

The Color Sorting System is an automated system that sorts colored balls into three compartments based on their detected color. The system uses a color sensor to identify red, green, and blue balls as they roll down an inclined ramp, then directs each ball to its corresponding compartment using a servo-controlled pivoting gate.

The project demonstrates a complete automated workflow: a ball is placed at the top of the ramp and rolls down due to gravity. As it passes the color sensor, the Arduino reads the RGB frequency values and determines the ball's color by comparing them against pre-calibrated thresholds. Based on the detected color, the servo motor rotates a small pivoting gate to one of three positions - 0° for red (left), 30° for green (center), or 60° for blue (right). The ball then continues rolling and is directed into the appropriate compartment. Throughout this process, a 16x2 LCD display provides real-time feedback showing the detected color and sorting status.

## 2. Bill of Materials (BOM)

**Arduino Uno** - microcontroller that serves as the brain of the system, processes sensor data and controls the servo motor;

**TCS3200 color sensor** - detects RGB color values of balls as they pass by;

**SG90 servo motor** - actuates the pivoting gate mechanism to direct balls into compartments;

**LCD 16x2 with I2C module** - displays system status, detected colors, and user feedback messages;

**9V battery** - provides portable power supply for the entire system;

**Breadboard (maybe?)** - allows easy connection of all electronic components;

**Jumper wires (M-M, M-F)**;

**Colored balls** - the objects being sorted, must include red, green, and blue balls;

**Cardboard** - construction material for the ramp structure and compartment dividers;

**Hot glue gun** - mechanical assembly of ramp, servo mounting, and gate attachment;

**Small containers (3 pieces)** - collection compartments for each color category.

## 3. Project analysis questions

**Q1: What is the system boundary?**

The system boundary includes the Arduino Uno, TCS3200 color sensor, servo motor, LCD display, ramp structure, pivoting gate, three collection compartments, and the 9V battery power supply. Everything needed for autonomous operation is contained within this boundary.

**Q2: Where does intelligence live?**

All intelligence resides exclusively on the Arduino board. The Arduino reads frequency values from the TCS3200 sensor, compares these RGB values against calibrated thresholds stored in its memory, determines which color matches the input, calculates the appropriate servo angle for that color, generates LCD display messages, and manages the complete workflow from detection through sorting to reset.

**Q3: What is the hardest technical problem?**

The hardest technical problem is achieving accurate color detection across varying lighting conditions. The TCS3200 returns relative frequency values rather than absolute color measurements, meaning the same ball can produce drastically different RGB readings under different lighting environments such as natural sunlight versus artificial lighting. Finding optimal threshold values that reliably distinguish between red, green, and blue while handling edge cases like dark blue versus dark green requires extensive calibration.

**Q4: What is the minimum demo?**

The minimum viable demo requires successfully sorting one ball of each color in sequence. First, place a red ball on the ramp - the system detects it, displays "Color: RED" on the LCD, rotates the servo to 0 degrees, and the ball lands in the red compartment. Then place a green ball - it's detected, LCD shows "Color: GREEN", servo moves to 30 degrees, and it lands in the green compartment. Finally, place a blue ball - it's detected, LCD shows "Color: BLUE", servo rotates to 60 degrees, and it lands in the blue compartment.

**Q5: Why is this not just a tutorial?**

This project requires original work beyond following tutorials. No tutorial provides the exact RGB threshold values needed for your specific colored balls, lighting environment, and particular TCS3200 sensor unit - these must be determined through empirical testing and iterative adjustment. While tutorials show how to read from a TCS3200 or control a servo independently, this project requires integrating multiple subsystems with timing synchronization between color detection and servo actuation. The mechanical design including ramp dimensions, gate geometry, and servo mounting position must be custom-developed through prototyping and testing.

**Q6: Do you need an ESP32?**

No, an ESP32 is not necessary for this project. An Arduino Uno is the appropriate platform choice because the system requires no networking capabilities - it operates standalone without WiFi, Bluetooth, IoT integration, or mobile app connectivity.