#include <Arduino.h>
#include "Adafruit_TLC5947.h"

// --- PIN CONFIGURATION (ATmega328PB) ---
// Based on MiniCore / Standard Arduino Uno mapping for PORTB
#define DATA_PIN  11  // PB3 (MOSI) -> SIN
#define CLOCK_PIN 13  // PB5 (SCK)  -> SCLK
#define LATCH_PIN 10  // PB2 (SS)   -> XLAT
#define BLANK_PIN 9   // PB1        -> BLANK

// Number of TLC5947 drivers connected in series (you have 1)
#define NUM_TLC5974 1

// Library instance
Adafruit_TLC5947 tlc = Adafruit_TLC5947(NUM_TLC5974, CLOCK_PIN, DATA_PIN, LATCH_PIN);

// LED Mapping (Channels 0-23)
// LED1: R0, G0, B0 -> Ch 0, 1, 2
// LED2: R1, G1, B1 -> Ch 3, 4, 5
// LED3: R2, G2, B2 -> Ch 6, 7, 8
// LED4: R3, G3, B3 -> Ch 9, 10, 11

void setLedColor(uint8_t ledIndex, uint16_t r, uint16_t g, uint16_t b) {
  // Each LED occupies 3 consecutive channels
  // ledIndex: 0 for LED1, 1 for LED2, etc.
  uint16_t baseChannel = ledIndex * 3;
  
  if (baseChannel > 21) return; // Protection (chip has 24 channels, but we only use up to 11)

  tlc.setPWM(baseChannel + 0, r); // Red
  tlc.setPWM(baseChannel + 1, g); // Green
  tlc.setPWM(baseChannel + 2, b); // Blue
}

void setup() {
  Serial.begin(9600);
  Serial.println("TLC5947 Test Start");

  // BLANK pin configuration: must be LOW to turn on LEDs
  pinMode(BLANK_PIN, OUTPUT);
  digitalWrite(BLANK_PIN, HIGH); // Start with LEDs off

  // Initialize library
  if (!tlc.begin()) {
    Serial.println("Reset failed");
  }

  // Enable outputs
  digitalWrite(BLANK_PIN, LOW);
  
  // Initial Test: All Red
  for(int i=0; i<4; i++) setLedColor(i, 4095, 0, 0);
  tlc.write();
  delay(1000);
  
  // Initial Test: All Green
  for(int i=0; i<4; i++) setLedColor(i, 0, 4095, 0);
  tlc.write();
  delay(1000);
  
  // Initial Test: All Blue
  for(int i=0; i<4; i++) setLedColor(i, 0, 0, 4095);
  tlc.write();
  delay(1000);

  // Initial Test: All White
  for(int i=0; i<4; i++) setLedColor(i, 4095, 4095, 4095);
  tlc.write();
  delay(3000);
  
  // Turn off
  for(int i=0; i<4; i++) setLedColor(i, 0, 0, 0);
  tlc.write();
}

// Variables for rainbow effect
uint16_t hue = 0;

void colorWheel(uint8_t ledNum, uint16_t wheelPos) {
  // Helper function to generate rainbow colors
  // wheelPos from 0 to 4095
  uint16_t r, g, b;
  
  // Divide the wheel into 3 parts
  if(wheelPos < 1365) {
    r = (1365 - wheelPos) * 3;
    g = wheelPos * 3;
    b = 0;
  } else if(wheelPos < 2730) {
    wheelPos -= 1365;
    r = 0;
    g = (1365 - wheelPos) * 3;
    b = wheelPos * 3;
  } else {
    wheelPos -= 2730;
    r = wheelPos * 3;
    g = 0;
    b = (1365 - wheelPos) * 3;
  }
  
  setLedColor(ledNum, r, g, b);
}

void loop() {
  // Rainbow Wave Effect
  for (int i=0; i<4; i++) {
    // Phase shift for each LED
    uint16_t localHue = (hue + (i * 1000)) % 4096; 
    colorWheel(i, localHue);
  }
  
  tlc.write();
  
  hue += 100; // Color change speed
  if (hue > 4095) hue = 0;
  
  delay(20);
}
