#include <Arduino.h>
#include "Adafruit_TLC5947.h"

/**
 * FIRMWARE ESEMPIO PER CONTROLLO CAMBIO LUCI VIA LIN (SERIALE 12V)
 * 
 * NOTE SUL CHIP TJA1028:
 * Per far funzionare la comunicazione, dobbiamo abilitare il transceiver.
 * U2 Pin 2 (EN)   -> ATmega PD3 (Pin 3) -> Deve essere HIGH
 * U2 Pin 7 (RSTN) -> ATmega PD2 (Pin 2) -> Deve essere HIGH
 */

// --- CONFIGURAZIONE PIN ---
#define DATA_PIN  11 
#define CLOCK_PIN 13 
#define LATCH_PIN 10 
#define BLANK_PIN 9  

// PIN ABILITAZIONE LIN TRANSCEIVER
#define LIN_EN_PIN   3
#define LIN_RST_PIN  2

#define NUM_TLC5974 1
Adafruit_TLC5947 tlc = Adafruit_TLC5947(NUM_TLC5974, CLOCK_PIN, DATA_PIN, LATCH_PIN);

// STATI DEL SISTEMA
enum LightMode {
  MODE_OFF,
  MODE_RAINBOW,
  MODE_FIXED_RGB
};

LightMode currentMode = MODE_RAINBOW; // Si parte col Rainbow
uint16_t fixedR = 0, fixedG = 0, fixedB = 0;

void setup() {
  // 1. INIZIALIZZA SERIALE (LIN)
  // Il LIN standard spesso usa 19200 o 9600 baud.
  Serial.begin(19200);

  // 2. ABILITA IL CHIP TJA1028 (Fondamentale!)
  pinMode(LIN_EN_PIN, OUTPUT);
  pinMode(LIN_RST_PIN, OUTPUT);
  digitalWrite(LIN_EN_PIN, HIGH);  // Attiva modalità Normal
  digitalWrite(LIN_RST_PIN, HIGH); // Togli Reset (Attiva il chip)

  // 3. INIZIALIZZA DRIVER LED
  pinMode(BLANK_PIN, OUTPUT);
  digitalWrite(BLANK_PIN, HIGH); 
  tlc.begin();
  digitalWrite(BLANK_PIN, LOW); // LED ON
  
  Serial.println("LIN READY. Send commands:");
  Serial.println("'R' -> Red, 'G' -> Green, 'A' -> Rainbow, 'O' -> Off");
}

// Funzione helper per settare tutti i LED insieme
void setAllLeds(uint16_t r, uint16_t g, uint16_t b) {
  for(int i=0; i<4; i++) {
    int base = i * 3;
    tlc.setPWM(base, r);
    tlc.setPWM(base+1, g);
    tlc.setPWM(base+2, b);
  }
  tlc.write();
}

// Variabili per l'animazione
uint16_t hue = 0;
unsigned long lastUpdate = 0;

void loop() {
  // --- 1. LETTURA SERIALE (LIN) ---
  if (Serial.available() > 0) {
    char cmd = Serial.read();

    // Protocollo Semplice a singolo carattere
    switch(cmd) {
      case 'R': // RED
        currentMode = MODE_FIXED_RGB;
        fixedR = 4095; fixedG = 0; fixedB = 0;
        Serial.println("CMD: Red");
        break;
      
      case 'G': // GREEN
        currentMode = MODE_FIXED_RGB;
        fixedR = 0; fixedG = 4095; fixedB = 0;
        Serial.println("CMD: Green");
        break;

      case 'B': // BLUE
        currentMode = MODE_FIXED_RGB;
        fixedR = 0; fixedG = 0; fixedB = 4095;
        Serial.println("CMD: Blue");
        break;

      case 'A': // ANIMATION (Rainbow)
        currentMode = MODE_RAINBOW;
        Serial.println("CMD: Rainbow Mode");
        break;

      case 'O': // OFF
        currentMode = MODE_OFF;
        Serial.println("CMD: Off");
        break;
        
      // Esempio comando avanzato: 'W' -> White
      case 'W': 
        currentMode = MODE_FIXED_RGB;
        fixedR = 4095; fixedG = 4095; fixedB = 4095;
        Serial.println("CMD: White");
        break;
    }
  }

  // --- 2. GESTIONE LED IN BASE ALLO STATO ---
  switch (currentMode) {
    case MODE_OFF:
      setAllLeds(0, 0, 0); // Risparmia energia ma continua ad ascoltare LIN
      break;

    case MODE_FIXED_RGB:
      // Scrive il colore salvato (in loop inutile ma sicuro)
      setAllLeds(fixedR, fixedG, fixedB);
      break;

    case MODE_RAINBOW:
      // Aggiorna l'animazione ogni 20ms senza bloccare la seriale (no delay!)
      if (millis() - lastUpdate > 20) {
        lastUpdate = millis();
        // Logica arcobaleno semplificata per brevità
        // (Copiare la funzione colorWheel completa dal vecchio main)
        hue += 50;
        if(hue > 4095) hue = 0;
        // Solo un esempio rapido: qui andrebbe la logica RGB completa
        // Per ora facciamo lampeggiare un colore rotante come placeholder
        setAllLeds(hue, 4095 - hue, (hue/2)); 
      }
      break;
  }
}
