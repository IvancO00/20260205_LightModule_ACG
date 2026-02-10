# PROGETTO CONTROLLER ESP32 BLE -> LIN (iOS & Android)

Questa cartella contiene il firmware per l'ESP32 configurato come **BLE UART Bridge**.
Supporta **iPhone (iOS)** e Android grazie all'uso dello standard Bluetooth Low Energy (Nordic UART Service).

## App Consigliata (iPhone/iOS)
Poiché iOS non supporta il Bluetooth Classico seriale, devi usare un'app BLE UART.
L'app migliore e più semplice è:

**Adafruit Bluefruit LE Connect**
1. Scarica l'app dall'App Store.
2. Apri l'app e cerca il dispositivo **"ESP32_LIN_Control"**.
3. Clicca su **Connect**.
4. Seleziona **UART**.
5. Scrivi i comandi (`R`, `G`, `B`) nel campo di testo e premi Send.

(Alternativa tecnica: **nRF Connect for Mobile**)

## Hardware Richiesto
1.  **ESP32 Dev Board** (es. ESP32-WROOM-32)
2.  **MikroE LIN Click**
3.  **Smartphone** Android (per Bluetooth Classico)

## Collegamenti (ESP32 <-> LIN Click)
| ESP32 Pin | LIN Click Pin | Funzione |
| :--- | :--- | :--- |
| **3.3V** | **3.3V** | Alimentazione logica |
| **5V / VIN** | **5V** | Alimentazione Click (se necessario) |
| **GND** | **GND** | Massa comune |
| **GPIO 17 (TX2)** | **RX** | ESP invia dati al LIN |
| **GPIO 16 (RX2)** | **TX** | ESP riceve dati dal LIN |
| **-** | **RST** | (Lasciare scollegato o HIGH) |
| **GPIO 5** | **CS / EN** | **IMPORTANTE:** Abilita il chip (Chip Select) |

**Nota sul su iPhone
1.  Apri **Bluefruit Connect**.
2.  Connetti a `ESP32_LIN_Control`.
3.  Vai su **UART**.
4.  Digita `R` e invia -> il LED LIN dovrebbe diventare ROSSO.
:

1.  In VS Code, vai su **File > Open Folder...**
2.  Seleziona la cartella `c:\Embedded_Scripts\20260205_LightModule_ACG\ESP_32_Bluetooth`.
3.  Ora vedrai solo i task di PlatformIO relativi all'ESP32.
4.  Collega l'ESP32 via USB.
5.  Fai **Upload**.

## Testing
1.  Accendi tutto.
2.  Sul telefono, attiva il Bluetooth e cerca dispositivi.
3.  Accoppia con **"ESP32_Light_Control"**.
4.  Apri un'app come **"Serial Bluetooth Terminal"**.
5.  Connettiti al dispositivo.
6.  Invia i caratteri `R`, `G`, `B`, `A` per vedere le luci cambiare!
