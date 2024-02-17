#include <Arduino.h>

// LED matrix configuration
const int row[8] = {2, 7, 19, 5, 13, 18, 12, 16};
const int col[8] = {6, 11, 10, 3, 17, 4, 8, 9};
int pixels[8][8];
int x = 5;
int y = 5;

// Scenario test configuration
unsigned long startTime;
const unsigned long duration = 2 * 60 * 1000; // 2 minutes in milliseconds

// External 12V LED control
const int relayPin = 10; // Relay control pin connected to digital pin 10
const int externalLedInputPin1 = 2; // Digital pin for external LED input 1
const int externalLedInputPin2 = 3; // Digital pin for external LED input 2
unsigned long externalLedStartTime;
const unsigned long externalLedDuration = 5000; // 5 seconds for external LED

// Optical isolation using optocoupler
const int optocouplerPin = 11; // Optocoupler input pin connected to digital pin 11

void setup() {
    Serial.begin(9600);  // Initialize serial communication
    for (int thisPin = 0; thisPin < 8; thisPin++) {
        pinMode(col[thisPin], OUTPUT);
        pinMode(row[thisPin], OUTPUT);
        digitalWrite(col[thisPin], HIGH);
    }

    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, LOW); // Ensure the relay is initially off

    // Configure external LED input pins with protection
    pinMode(externalLedInputPin1, INPUT); // Set external LED input pin 1 as input
    pinMode(externalLedInputPin2, INPUT); // Set external LED input pin 2 as input

    // Optical isolation setup
    pinMode(optocouplerPin, OUTPUT);
    digitalWrite(optocouplerPin, LOW); // Ensure optocoupler input is initially low

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            pixels[i][j] = HIGH;
        }
    }

    startTime = millis(); // store the current time when the program starts
    externalLedStartTime = millis(); // store the current time for external LED
}

void loop() {
    // Check if the elapsed time is less than the duration
    if (millis() - startTime < duration) {
        // Continue running the LED matrix code
        readSensors();
        refreshScreen();
        controlExternalLed(); // Control the external 12V LED
    } else {
        // If 2 minutes have passed, stop the loop
        while (true) {
            // Do nothing, just wait here
        }
    }
}

void readSensors() {
    pixels[x][y] = HIGH;
    x = 7 - map(analogRead(A0), 0, 1023, 0, 7);
    y = map(analogRead(A1), 0, 1023, 0, 7);
    pixels[x][y] = LOW;
}

void refreshScreen() {
    for (int thisRow = 0; thisRow < 8; thisRow++) {
        digitalWrite(row[thisRow], HIGH);

        for (int thisCol = 0; thisCol < 8; thisCol++) {
            int thisPixel = pixels[thisRow][thisCol];
            digitalWrite(col[thisCol], thisPixel);

            if (thisPixel == LOW) {
                digitalWrite(col[thisCol], HIGH);
            }
        }

        digitalWrite(row[thisRow], LOW);
    }
}

void controlExternalLed() {
    // Check if 5 seconds have passed since the last external LED operation
    if (millis() - externalLedStartTime > externalLedDuration) {
        // Implement different lighting scenarios for the external LED
        int scenario = millis() / 10000 % 4; // Change scenario every 10 seconds

        switch (scenario) {
            case 0:
                // Scenario 0: Turn external LED on
                digitalWrite(relayPin, HIGH);
                break;
            case 1:
                // Scenario 1: Turn external LED off
                digitalWrite(relayPin, LOW);
                break;
            case 2:
                // Scenario 2: Blink external LED
                digitalWrite(relayPin, HIGH);
                delay(500);
                digitalWrite(relayPin, LOW);
                delay(500);
                break;
            case 3:
                                // Scenario 3: Toggle external LED based on the state of external LED input pins
                int inputState1 = digitalRead(externalLedInputPin1);
                int inputState2 = digitalRead(externalLedInputPin2);

                // If both input pins are HIGH, turn external LED on
                if (inputState1 == HIGH && inputState2 == HIGH) {
                    digitalWrite(relayPin, HIGH);
                } else {
                    // Otherwise, turn external LED off
                    digitalWrite(relayPin, LOW);
                }
                break;
        }

        externalLedStartTime = millis(); // Reset the timer for the next scenario
    }
}

