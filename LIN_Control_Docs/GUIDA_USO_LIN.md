# GUIDA ALL'USO DEL CONNETTORE LIN

## 1. Che cos'è?
Il connettore J1 sulla tua scheda permette di controllare le luci da un dispositivo esterno (Computer, Centralina auto, o un altro Arduino) utilizzando il bus **LIN** (12V).

La scheda monta un chip **TJA1028** che converte i segnali LIN (12V) in segnali Seriali (5V) comprensibili dal microcontrollore.

## 2. Collegamenti Hardware (Modulo USB-LIN)
Quando avrai il tuo convertitore USB-LIN in ufficio, collegalo così al connettore **J1**:

| USB-LIN Module | Scheda J1 | Note |
| :--- | :--- | :--- |
| **LIN** (Data) | **Pin 2** (LIN) | Il filo dei dati |
| **GND** | **Pin 1** (GND) | Massa comune (Fondamentale!) |
| **VBAT / 12V** | **Pin 4** (+12V) | Se il modulo fornisce corrente, altrimenti usa alimentatore esterno |

*Attenzione: Controlla la piedinatura del tuo cavo J1 specifico, la tabella sopra è indicativa basata sui nomi standard.*

## 3. Protocollo di Comunicazione
Il codice di esempio fornito (`LIN_main_example.cpp`) utilizza una **Seriale Raw su livello fisico LIN**. 
Non è un protocollo LIN "completo" (con Master/Slave PID, Checksum), ma una semplice trasmissione seriale che sfrutta la robustezza del cavo LIN. È il modo più semplice per iniziare.

**Impostazioni Terminale PC:**
*   **Baudrate:** 19200
*   **Data Bits:** 8
*   **Parity:** None
*   **Stop Bits:** 1

**Comandi disponibili (Da digitare nel terminale):**
*   `R` -> Accende i LED Rossi
*   `G` -> Accende i LED Verdi
*   `B` -> Accende i LED Blu
*   `W` -> Accende i LED Bianchi
*   `A` -> Attiva la modalità Arcobaleno (Animazione)
*   `O` -> Spegne tutto (Off)

## 4. Come caricare il Firmware LIN
Il file principale del progetto è ancora quello originale (Rainbow automatico). Per attivare il controllo via LIN:
1.  Apri il file `src/main.cpp`.
2.  Cancella tutto il contenuto.
3.  Copia il contenuto del file `LIN_Control_Docs/LIN_main_example.cpp`.
4.  Incolla dentro `src/main.cpp`.
5.  Fai **Upload** con PlatformIO.

## 5. Note Tecniche Importanti
Per far funzionare il chip TJA1028, il firmware DEVE tenere alti (HIGH) due pin specifici dell'Arduino:
*   **Pin 2** (PD2) -> Collegato a RSTN del transceiver.
*   **Pin 3** (PD3) -> Collegato a EN del transceiver.

Se questi pin sono LOW, il chip LIN è in modalità "Sleep" e non riceverà nulla. Il codice di esempio gestisce già questa cosa nel `setup()`.
