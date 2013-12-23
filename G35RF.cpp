#include "G35RF.h"
#include <Arduino.h>

// Number of bits/pulses in a radio frame
#define BITS (28)

// Debounce delay between valid commands
#define COMMAND_DEBOUNCE (400)

// Time of last successful command (for debounce)
unsigned long lastCommand;

// Time of the last edge
volatile unsigned long pulseStart = 0;

// Counts the pulses within a frame.
// -1 indicates no frame detected
volatile int currentPulse = -1;

// Store the widths of each pulse in the frame
volatile int data[BITS];

// Stores the last decoded frame
volatile int frame = 0;

// Interrupt Service Routine
void G35RF_isr() {
  if (frame) return;

  bool state = digitalRead(2);
  unsigned long pulseWidth = micros() - pulseStart;
  pulseStart = micros();

  // Check for the start of a frame
  if (pulseWidth > 2000 && state == HIGH) {
    currentPulse = 0;
    return;
  }

  // If we're already in a frame, log the pulse width
  else if (currentPulse >= 0 && currentPulse < BITS && state == LOW) {
    // Log the pulse widths
    data[currentPulse] = pulseWidth;
    currentPulse++;
  }

  // If frame is complete, validate it
  if (currentPulse == BITS) {

    // The first 10 pulses are preamble
    for (int i=0; i<10; i++) {
      if (data[i] < 80 || data[i] > 150) {
        currentPulse = -1;
        return;
      }
    }

    // The tenth is an extra long one signalling the end of the preamble
    if (data[10] < 380) {
      currentPulse = -1;
      return;
    }

    // The rest of the bits make up the frame
    for (int i=11; i<BITS; i++) {
      if (data[i] > 80) {
        frame |= 1 << (BITS - i - 1);
      }
    }
  }
}

void G35RF_enable() {
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(0, G35RF_isr, CHANGE);
}

int G35RF_get_command() {
  int currframe = frame;
  int command = 0;

  if (currframe) {
    command = frame >> 1 & 0xff;

    switch(command) {

      // Valid commands
      case G35RF_POWER_ON:
      case G35RF_POWER_OFF:
      case G35RF_FUNCTION:

        // Debounce
        if (millis() - lastCommand > COMMAND_DEBOUNCE) {
          lastCommand = millis();
        } else {
          command = 0;
        }
        break;

      // Invalid commands
      default:
        command = 0;
    }

    // Reset global state so the ISR will look for the next frame
    currentPulse = -1;
    frame = 0;
  }

  return command;
}
