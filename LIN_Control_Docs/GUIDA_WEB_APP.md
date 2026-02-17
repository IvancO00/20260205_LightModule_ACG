# Guida alla Web App Bluetooth (LIN Controller)

Questa guida spiega come funziona il file `index.html` che usi per controllare le luci tramite l'ESP32. La pagina usa una tecnologia chiamata **Web Bluetooth API**, che permette a una pagina web (se visitata tramite browser compatibili come Chrome su Android o Bluefy su iOS) di connettersi direttamente ai dispositivi Bluetooth Low Energy (BLE).

## 1. Struttura del File
Il file è un classico documento HTML diviso in tre parti:
1.  **HTML (`<body>`)**: La struttura visiva (pulsanti, titoli, log).
2.  **CSS (`<style>`)**: L'aspetto grafico (colori, dimensioni, posizionamento).
3.  **JavaScript (`<script>`)**: Il "cervello" che gestisce la connessione Bluetooth e invia i dati.

---

## 2. Come Funziona la Logica (JavaScript)

### Gli UUID (Gli indirizzi del servizio)
All'inizio dello script, definiamo degli identificativi unici. Questi devono coincidere ESATTAMENTE con quelli nel codice dell'ESP32 (`main.cpp`).
```javascript
const serviceUuid = "6e400001-b5a3-f393-e0a9-e50e24dcca9e"; // Identifica il servizio UART
const characteristicUuidRx = "6e400002-b5a3-f393-e0a9-e50e24dcca9e"; // Dove scriviamo i dati (TX del telefono -> RX dell'ESP32)
```

### La Connessione (`connectBtn.addEventListener`)
Quando clicchi "Connect to ESP32":
1.  `navigator.bluetooth.requestDevice`: Il browser apre un popup cercando dispositivi che iniziano con il nome "ESP32".
2.  `device.gatt.connect()`: Si connette al dispositivo.
3.  `service.getCharacteristic()`: Cerca la "casella di posta" (`characteristicUuidRx`) dove imbucare i messaggi per l'ESP32.

### L'Invio dei Comandi (`sendCommand`)
Questa è la funzione principale. Prende una stringa di testo (es. `"L,99,4095,0,0"`), aggiunge un carattere di "a capo" (`\n`) se manca, la converte in byte e la spedisce via etere.
```javascript
async function sendCommand(cmd) {
    // ...controlli se connesso...
    
    // Aggiungi \n finale perché il codice Arduino usa readStringUntil('\n')
    let commandToSend = cmd.endsWith('\n') ? cmd : cmd + '\n';
    
    // Converti testo in numeri (byte)
    let encoder = new TextEncoder('utf-8');
    let data = encoder.encode(commandToSend);

    // Invia
    await rxCharacteristic.writeValue(data);
}
```

---

## 3. Come Aggiungere Nuovi Pulsanti

Per aggiungere un nuovo pulsante personalizzato, devi solo copiare una riga HTML esistente e cambiare il comando tra virgolette.

**Esempio:** Vuoi un pulsante che accenda tutto di **Viola** (Rosso + Blu)?
Cerca la sezione `<div class="control-group">` e aggiungi:

```html
<button class="btn-blue" onclick="sendCommand('L,99,4095,0,4095')">TUTTO VIOLA</button>
```
*(Nota: `L,99` significa tutti i LED, `4095` R, `0` G, `4095` B)*

**Esempio:** Vuoi controllare solo il **LED numero 1** (Indice 0)?
```html
<button style="background-color: orange;" onclick="sendCommand('L,0,4095,2000,0')">LED 1 ARANCIO</button>
```

---

## 4. Come Aggiungere uno Slider (Avanzato)

Se vuoi regolare l'intensità (es. del rosso) con una barra scorrevole invece che con i pulsanti, puoi aggiungere questo codice HTML:

```html
<!-- Slider per il Rosso -->
<div class="control-group">
    <label>Intensità Rosso Generala</label>
    <input type="range" min="0" max="4095" value="0" id="redRange" 
           onchange="updateColor()">
</div>
```

E aggiungere questa funzione nel blocco `<script>`:
```javascript
function updateColor() {
    var val = document.getElementById('redRange').value;
    // Invia comando per impostare Rosso al valore dello slider, Verde 0, Blu 0
    sendCommand(`L,99,${val},0,0`);
}
```

*(Nota: Usare `onchange` invia il comando solo quando lasci il dito. Usare `oninput` invierebbe troppi comandi intasando il Bluetooth, quindi per ora meglio `onchange`)*.
