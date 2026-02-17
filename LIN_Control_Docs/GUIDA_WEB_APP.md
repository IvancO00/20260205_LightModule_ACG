# Guida Completa alla Web App Bluetooth (LIN Controller)

Questa guida spiega in dettaglio come funziona il file `index.html` che usi per controllare le luci tramite l'ESP32 e come implementare funzionalità avanzate come gli slider RGB "mixabili".

---

## 1. Come Funziona il File `index.html`

Il file web non è altro che un telecomando virtuale. È diviso in tre strati logici:

### A. La Struttura (`HTML`)
Definisce **cosa c'è** nella pagina. I pulsanti, i testi e gli input (come le caselle di testo o gli slider) vivono qui.
Ogni elemento importante ha un `id` (un nome unico, es. `customCmd`) o un attributo `onclick` (cosa succede quando clicchi).

### B. Lo Stile (`CSS`)
Definisce **come appare**. Colori, bordi e spaziature. Se vedi `.btn-red { background-color: #ff3b30; }`, significa "Tutti gli elementi che hanno la classe 'btn-red' devono essere rossi".

### C. La Logica (`JavaScript`)
Definisce **cosa succede**. È la parte nel blocco `<script>`.
Qui avviene la magia del Bluetooth:
1.  **Chiedi il permesso**: `navigator.bluetooth.requestDevice` apre il menu di sistema per scegliere il dispositivo.
2.  **Ti connetti**: Cerca il "Servizio UART" (una sorta di canale dedicato ai dati seriali).
3.  **Ottieni la Caratteristica**: Cerca la "Caratteristica RX" (la buca delle lettere dove imbucare i messaggi).
4.  **Invii i dati**: `rxCharacteristic.writeValue(data)` trasforma il testo in byte e lo spedisce via radio.

---

## 2. Il Protocollo di Comunicazione
Il codice JavaScript invia delle semplici stringhe di testo che il tuo ESP32 legge e interpreta.

-   **Comando:** `L, <LedIndex>, <Rosso>, <Verde>, <Blu>`
-   **Esempio:** `L,99,4095,0,0\n`
    *   `L`: Comando Luce
    *   `99`: Tutti i LED (oppure 0, 1, 2, 3 per i singoli)
    *   `4095`: Intensità Rosso Massima (12-bit PWM)
    *   `0`: Verde Spento
    *   `0`: Blu Spento
    *   `\n`: Carattere invisibile "a capo" (fondamentale perché l'ESP sappia che il messaggio è finito).

---

## 3. Tutorial: Creare 3 Slider RGB (Mixabili)

Come hai chiesto, ecco come implementare 3 slider (Rosso, Verde, Blu) che lavorano insieme. Se imposti il Rosso al 50% e poi muovi il Verde, il Rosso **deve rimanere** al 50% e non spegnersi.

### Il Problema dei Comandi Semplici
Se fai tre funzioni separate tipo `setRed(val)` che manda `L,99,val,0,0`, ogni volta che muovi uno slider spegni gli altri due colori!
**Soluzione:** Devi leggere TUTTI e tre i valori attuali ogni volta che ne muovi uno.

### Passo 1: Modifica l'HTML
Aggiungi tre slider nel `<body>` (magari sotto ai pulsanti rapidi). Ognuno ha un `id` diverso e chiama la stessa funzione `updateRGB()` quando cambia.

```html
<div class="control-group">
    <h3>Mixer RGB</h3>
    
    <label>Rosso (0-4095)</label>
    <!-- Range da 0 a 4095. onchange = aggiorna quando rilasci (più sicuro per il Bluetooth) -->
    <input type="range" id="sliderR" min="0" max="4095" value="0" onchange="updateRGB()">
    
    <label>Verde (0-4095)</label>
    <input type="range" id="sliderG" min="0" max="4095" value="0" onchange="updateRGB()">
    
    <label>Blu (0-4095)</label>
    <input type="range" id="sliderB" min="0" max="4095" value="0" onchange="updateRGB()">
</div>
```

### Passo 2: Aggiungi la Logica JavaScript
Nel blocco `<script>`, aggiungi questa nuova funzione. Non hai bisogno di variabili globali complesse, perché possiamo leggere il valore attuale direttamente dagli slider HTML ogni volta!

```javascript
/* 
   Questa funzione viene chiamata ogni volta che muovi UNO QUALSIASI degli slider.
   Legge TUTTI e tre i valori attuali e invia un unico comando combinato.
*/
function updateRGB() {
    // 1. Recupera i valori attuali dai tre slider HTML usando i loro ID
    let r = document.getElementById('sliderR').value;
    let g = document.getElementById('sliderG').value;
    let b = document.getElementById('sliderB').value;

    // 2. Costruisci il comando LIN composto
    // Esempio risultato: "L,99,4095,2000,0" (Rosso max, Verde medio, Blu spento)
    let comandoComposto = `L,99,${r},${g},${b}`;

    // 3. Invia tramite Bluetooth
    // window.log è la funzione che scrive nel riquadro bianco in basso
    if(window.log) window.log(`Mixer RGB: R=${r} G=${g} B=${b}`); 
    
    sendCommand(comandoComposto);
}
```

### Passo 3: Sincronizzazione Iniziale (Opzionale)
Quando ti connetti o premi "ALL OFF", gli slider rimangono nella posizione vecchia. Per fare le cose fatte bene, dovresti aggiornare la posizione visiva degli slider quando premi i pulsanti rapidi.

Esempio: modifica il pulsante OFF per resettare anche gli slider.
```html
<button class="btn-off" onclick="resetSlidersAndOff()">ALL OFF</button>
```

E aggiungi questa funzione nel Javascript:
```javascript
function resetSlidersAndOff() {
    // Porta gli slider a zero visivamente
    document.getElementById('sliderR').value = 0;
    document.getElementById('sliderG').value = 0;
    document.getElementById('sliderB').value = 0;
    
    // Manda il comando di spegnimento effettivo
    sendCommand('CLEAR');
}
```

### Consigli per lo Sviluppo
1.  **`onchange` vs `oninput`**:
    *   `oninput`: Lancia l'evento *mentre* trascini. È fluidissimo visivamente ma spara centinaia di comandi Bluetooth al secondo. L'ESP32 potrebbe non starci dietro.
    *   `onchange`: Lancia l'evento solo quando *rilasci* il dito. È molto più stabile. Ua questo all'inizio.

2.  **Debug**: Se non funziona, guarda il riquadro bianco "Log" nella tua app. Se vedi scritto `Sent: L,99,undefined,0,0` significa che hai sbagliato a scrivere l'ID dell'elemento HTML nel ccodice JavaScript.
