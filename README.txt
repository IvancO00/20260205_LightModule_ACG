SETUP INSTRUCTIONS (MKR WAN 1310 as ISP)

WARNING: VOLTAGE LEVELS (3.3V vs 5V)
---------------------------------------------
Your ATmega328PB target microcontroller runs at 5V (according to the schematic).
Your Arduino MKR WAN 1310 runs at 3.3V.

DIRECTLY CONNECTING MKR PINS (3.3V) TO THE 5V BOARD CAN DAMAGE THE MKR!
The SPI lines (MISO, MOSI, SCK) must be level-shifted.

SAFE SOLUTIONS:
1. Use a bidirectional Level Shifter between MKR and Target.
2. OR: Power the target board at 3.3V (instead of 5V) by disconnecting the external 5V source and using the MKR's VCC pin (3.3V) to power the Target's VCC pin. (Note: At 16MHz the 328PB is out of spec at 3.3V, but it often works for flashing).

---------------------------------------------
STEP 1: PREPARE THE MKR WAN 1310
---------------------------------------------
1. Open the classic Arduino IDE.
2. Open the example: File > Examples > 11.ArduinoISP > ArduinoISP.
3. SCROLL THE CODE until you find: `#define RESET 10`.
   ON MKR WAN 1310, pin 10 is often used for other things or can create conflicts (MISO).
   CHANGE that line to: `#define RESET 6` (or another free digital pin).
4. ALSO MODIFY LED PINS:
   Locate `#define LED_HB 9`. Pin 9 is SCK on MKR!
   Change to `#define LED_HB 2` (or any free pin).
   Same for other LEDs if they conflict with pins 8, 9, 10.
5. Upload this sketch to your MKR WAN 1310.

---------------------------------------------
STEP 2: WIRING
---------------------------------------------
MKR WAN 1310 Pin      ->      Target J2 (ISP Connector)
-------------------------------------------------------
VCC (3.3V)            ->      VCC (Pin 3) **ONLY IF POWERING AT 3.3V**
GND                   ->      GND (Pin 5)
Pin 6 (Digital)       ->      RESET (Pin 1)  (The one defined in the sketch)
MOSI (Pin 8)          ->      MOSI (Pin 4)
MISO (Pin 10)         ->      MISO (Pin 2)
SCK  (Pin 9)          ->      SCK  (Pin 6)

*Note on MKR SPI pins: Double check pinout or silkscreen. 
On many MKR boards: MOSI=8, SCK=9, MISO=10.

---------------------------------------------
STEP 3: FLASHING WITH VS CODE
---------------------------------------------
1. Open this project in VS Code.
2. Open the `platformio.ini` file.
3. Modify the line `upload_port = COM11` inserting the real COM port of your MKR.
4. Press the "PlatformIO: Upload" icon (the right arrow in the blue bottom bar, or in the side menu).
5. To set fuses (fix speed): Run "Burn Bootloader" task.
