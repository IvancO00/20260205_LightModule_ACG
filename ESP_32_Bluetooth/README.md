# PROGETTO CONTROLLER ESP32 BLUETOOTH -> LIN

Questa cartella contiene il firmware per l'ESP32 che fa da ponte ("bridge") tra il tuo smartphone e la scheda luci LIN.

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

**Nota sul LIN Click:** 
1. Assicurati che il jumper `VCC SEL` sia su **3.3V**.
2. Assicurati che il jumper `JP2 MODE` sia su **MASTER** (o aggiungi resistenza pull-up 1k su LIN se usi scheda slave).
3. **Collegamento CS:** Il codice usa il **GPIO 5** per abilitare il chip (Pin 'CS' sulla Click Board). Se non lo colleghi, il LIN potrebbe rimanere spento!

## Istruzioni Software
Poiché questo è un progetto nidificato (una cartella dentro un'altra):

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
