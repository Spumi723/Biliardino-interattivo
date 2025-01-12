# Biliardino interattivo

## Struttura del programma
Il codice dentro dentro il blocco
```C++
void setup(){
    ...
}
```
viene eseguito una sola volta all'accensione di arduino poi inizia ad eseguire all'infinito il codice dentro il blocco 
```C++
void loop(){
    ...
}
```

### Rilevazione di un possibile goal
Dentro il loop c'è il ciclo while di attesa
```C++
while(abs(analogRead(redSensor))<sensTh && abs(analogRead(blueSensor))<sensTh){
    ...
}

```
Tutto ciò che ci sta dentro viene ripetuto fin che riceve un segnale abbastanza forte da uno dei sensori che può essere interpretato come goal. Inoltre c'è una variabile ```counter``` che aumenta di 1 ad ogni iterazione.

**IMPORTANTE:** questo ciclo, in assenza di eventi (tipo cambio modalità o goal fatto) deve girare il più veloce possibile, arduino non ha una freqenza di clock molto alta quindi NON METTETECI DELAY o altre funzioni che lo rallentino in modo da ridurre al minimo il tempo morto dei sensori.

### Scelta della modalità
In qualche modo che definiremo si può cambiare modalità di animazione. Quando riceve un segnale entra dentro il blocco
```C++
if (irrecv.decode(&results)){
    ...
}
```
e imposta la variabile ``` mode ``` al valore del tasto premuto sul telecomando

### Animazione in background
Vogliamo che in alcune modalità ci sia anche un'illuminazione continua di sottofondo invocabile tramite ``` backAnimation(mode, watchTime) ```. Come detto prima se vogliamo ridurre al minimo il tempo morto non possiamo invocarla ogni volta, quindi si usa il ```watchTime```

#### *watchTime*
Il watchTime, definito a inizio codice, è il numero di cicli consecutivi da fare prima di aggiornare l'animazione in background
* ```watchtime alto``` -> poco tempo morto basso ma animazione a scatti
* ```watchtime``` -> tanto tempo morto ma animazione fluida
Si smanetta e si trova il compromesso migliore

#### backAnimation()
I valori che vengono passati alla funzione sono due
* ```mode``` -> per discriminare la modalità di animazione corrente
* ```counter/watchTime``` -> da vedere come un indice intero che ogni volta che la funzione viene chiamata aumenta di uno

Quando 