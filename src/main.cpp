#include <Arduino.h>
#include "Adafruit_TLC5947.h"

//Limit max current at 30mA --> Max at 50%
#define TOTAL_MAX_POWER 4095*3/2

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
  if (r+g+b > TOTAL_MAX_POWER) {
    // Scale down to fit max power
    float scale = (float)TOTAL_MAX_POWER / (r + g + b);
    r = (uint16_t)(r * scale);
    g = (uint16_t)(g * scale);
    b = (uint16_t)(b * scale);
  }

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
  delay(500);
  
  // Initial Test: All Green
  for(int i=0; i<4; i++) setLedColor(i, 0, 4095, 0);
  tlc.write();
  delay(500);
  
  // Initial Test: All Blue
  for(int i=0; i<4; i++) setLedColor(i, 0, 0, 4095);
  tlc.write();
  delay(500);

  // Initial Test: All White
  for(int i=0; i<4; i++) setLedColor(i, 4095, 4095, 4095);
  tlc.write();
  delay(500);
  
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

void blink_white_function(int times, int delay_ms) {
  for (int i = 0; i < times; i++) {
    // Turn on all LEDs (white)
    for(int j=0; j<4; j++) setLedColor(j, 4095, 4095, 4095);
    tlc.write();
    delay(delay_ms);
    
    // Turn off all LEDs
    for(int j=0; j<4; j++) setLedColor(j, 0, 0, 0);
    tlc.write();
    delay(delay_ms);
  }
}

void blink_red_function(int times, int delay_ms) {
  for (int i = 0; i < times; i++) {
    // Turn on all LEDs (red)
    for(int j=0; j<4; j++) setLedColor(j, 4095, 0, 0);
    tlc.write();
    delay(delay_ms);
    
    // Turn off all LEDs
    for(int j=0; j<4; j++) setLedColor(j, 0, 0, 0);
    tlc.write();
    delay(delay_ms);
  }
}

void blink_green_function(int times, int delay_ms) {
  for (int i = 0; i < times; i++) {
    // Turn on all LEDs (green)
    for(int j=0; j<4; j++) setLedColor(j, 0, 4095, 0);
    tlc.write();
    delay(delay_ms);
    
    // Turn off all LEDs
    for(int j=0; j<4; j++) setLedColor(j, 0, 0, 0);
    tlc.write();
    delay(delay_ms);
  }
}

void blink_blue_function(int times, int delay_ms) {
  for (int i = 0; i < times; i++) {
    // Turn on all LEDs (blue)
    for(int j=0; j<4; j++) setLedColor(j, 0, 0, 4095);
    tlc.write();
    delay(delay_ms);
    
    // Turn off all LEDs
    for(int j=0; j<4; j++) setLedColor(j, 0, 0, 0);
    tlc.write();
    delay(delay_ms);
  }
}

void turn_off_all_leds() {
  for(int i=0; i<4; i++) setLedColor(i, 0, 0, 0);
  tlc.write();
}


void chargingEffect() {
  turn_off_all_leds(); // Turn off all LEDs after blinking

  blink_red_function(3, 200); // Blink all LEDs red 3 times at the start of charging

  // Simulate battery charging: dim each LED up to max, then move to next
  int led = 3;
  uint16_t brightness = 0;

  while (led >= 0) {
    brightness += 50;
    setLedColor(led, 0, brightness, 0); // Green color
    tlc.write();
    delay(10);
    if (brightness > 4095-50) {
      setLedColor(led, 0, 4095, 0); // Green color
      brightness = 0;
      led--;

      if (led == -1) {
        delay(500); // Short pause before blinking
        blink_white_function(5, 200); // Blink all LEDs 3 times at the end of charging
        
        turn_off_all_leds(); // Turn off all LEDs after blinking
        delay(500); // Short pause before next effect
      };
    };
  };
};

void loop() {

  chargingEffect();
  
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
