#include <Arduino.h>
#include "BluetoothSerial.h"

// Oggetto per gestire il Bluetooth Classico
BluetoothSerial SerialBT;

// PIN per la comunicazione con LIN Click (UART2 dell'ESP32)
// TX2 (GPIO 17) -> RX del LIN Click
// RX2 (GPIO 16) -> TX del LIN Click
#define RXD2 16
#define TXD2 17

// IMPORTANTE: Le board LIN (come MikroE LIN Click) hanno spesso un pin di "Enable" o "Sleep"
// che deve essere messo a HIGH per accendere il chip.
// Sul LIN Click standard è spesso il pin "CS". Collegalo a un pin digitale (es. 5) o a 3.3V.
#define LIN_ENABLE_PIN 5 // Imposta a -1 se colleghi Enable direttamente a 3.3V fisicamente

void setup() {
  // 0. Configura Enable Pin per LIN Transceiver
  if (LIN_ENABLE_PIN != -1) {
    pinMode(LIN_ENABLE_PIN, OUTPUT);
    digitalWrite(LIN_ENABLE_PIN, HIGH); // Accende il transceiver
  }

  // 1. Seriale di Debug (verso il PC via USB)
  Serial.begin(115200);

  // 2. Inizializza Bluetooth
  // Il nome "ESP32_Light_Control" apparirà sul telefono
  if(!SerialBT.begin("ESP32_Light_Control")) {
    Serial.println("An error occurred initializing Bluetooth");
  } else {
    Serial.println("Bluetooth Initialized! Pair with 'ESP32_Light_Control'");
  }

  // 3. Inizializza Seriale verso LIN Click (Hardware Serial 2)
  // Baudrate 19200 (deve corrispondere a quello del ATmega328PB)
  Serial2.begin(19200, SERIAL_8N1, RXD2, TXD2);
}

void loop() {
  // -- A. Da Telefono (Bluetooth) a Scheda Luci (LIN) --
  if (SerialBT.available()) {
    char cmd = SerialBT.read();
    
    // Debug su PC
    Serial.print("Received from BT: ");
    Serial.println(cmd);

    // Inoltra il comando sul bus LIN (Serial2)
    Serial2.write(cmd);
  }

  // -- B. Da Scheda Luci (LIN) a Telefono (Feedback opzionale) --
  if (Serial2.available()) {
    char feedback = Serial2.read();
    
    // Inoltra il feedback al telefono
    SerialBT.write(feedback);
    Serial.write(feedback); // Anche su debug
  }
  
  delay(10);
}
