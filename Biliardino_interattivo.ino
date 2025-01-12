/*
  Codice arduino per il biliardino interattivo

  Questo codice deve fare 3 cose:

  - Rilevare i goal tramite sensori piezoelettrici
  - Gestire le animazioni luminose e sonore
  - Scegliere tramite sensore IR la modalità di animazione
*/

#include <FastLED.h>
#define LED_PIN 5         // Replace with the pin number you used for DATA connection
#define NUM_LEDS 52       // Replace with the number of LEDs in your strip
#define FLICKER_DELAY 20  // Delay between flicker changes (in milliseconds)
CRGB leds[NUM_LEDS];


const int ledPin = 13;      // LED collegato al pin 13 per indicazioni di stato
const int redSensor = A2;   // Sensori piezo collegati ai pin A1 (rossi) A2 (blu)
const int blueSensor = A1; 

/*
 * Valori chiave per la rilevazione dei goal
 * forse un giorno faremo machine learning
 * ma non è questo il giorno
 */

const int sensTh = 200;   // Threshold per inizio rilevazione
const int redTh = 4500;   // Threshold segnale totale porta rossa
const int blueTh = 4800;  // Threshold segnale totale porta blu

const float redFactor = 3;    // Differenza minima di segnale durante un goal
const float blueFactor = 3;


float redReading = 0;     // Segnale porta rossa
float blueReading = 0;    // Segale porta blu

int rTemp = 0;    // Variabili di appoggio
int bTemp = 0;

int mode = 0;  // Modalità di animazione
int brightness = 5;

const int watchTime = 100; // Cicli consecutivi senza cambiare la backAnimation

unsigned long int counter = 0; // Contatore per le animazioni

void setup() {
  pinMode(ledPin, OUTPUT);  // declare the ledPin as as OUTPUT
  Serial.begin(9600);       // use the serial port
  FastLED.addLeds<WS2811, LED_PIN, RGB>(leds, NUM_LEDS);
  counter=0;
  mode = 0;
}

void loop() {
  // read the sensor and store it in the variable sensorReading:
  redReading = 0;
  blueReading = 0;

  while(abs(analogRead(redSensor))<sensTh && abs(analogRead(blueSensor))<sensTh){
      // wait

    if(counter%watchTime == 0){
      backAnimation(mode,counter/watchTime);
    }
    counter += 1;
  }

  for(int i=0;i<200;i++){ // Integrazione del segnale
    rTemp = analogRead(redSensor);
    bTemp = analogRead(blueSensor);
    redReading += abs(rTemp);
    blueReading += abs(bTemp);
    delay(1);
  }


  if( blueReading > blueTh || redReading > redTh ){ // Goal detected
    if( blueReading/blueFactor > redReading){ // Goal dei Rossi
      goalAnimation(mode, 1);
    } else  if( redReading/redFactor > blueReading){ // Goal dei Blu
      goalAnimation(mode, 0);
    }
  }
  
  /*
  Serial.print(blueReading);
  Serial.print(",");
  Serial.println(redReading);
  */
  
  
  delay(1);  // delay to avoid overloading the serial port buffer
}

void goalAnimation(int m, int t){ // t=0 goal dei blu, t=1 goal dei rossi
  CRGB flameColor;
  switch(m){
    case 0:
      if(t == 0){
      flameColor = CRGB(255,0,0);
      for(int j=0;j<4;j++){
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = flameColor;
        }
        FastLED.show();
        delay(150);
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CRGB::Black;
        }
        FastLED.show();
        delay(50);
      }
      
    }
    else if(t == 1){
      flameColor = CRGB(0,255,0);
      for(int j=0;j<4;j++){
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = flameColor;
        }
        FastLED.show();
        delay(150);
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CRGB::Black;
        }
        FastLED.show();
        delay(50);
      }
    }
    break;
  }
}

void backAnimation(int m, long int j){
  CRGB flameColor;
  switch(m){
    case 0:
      flameColor = CRGB(15*brightness/100,6*brightness/100,198*brightness/100);
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = flameColor;
      }
      FastLED.show();
      break;

    case 1:
      flameColor = CRGB(255*brightness/100,255*brightness/100,255*brightness/100);
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = flameColor;
      }
      FastLED.show();
      break;

    case 2:
      j = j%410;
      j = j - 205;
      j = abs(j);
      flameColor = CRGB(0,0,(j+50)*brightness/10);
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = flameColor;
      }
      FastLED.show();
      break;
  }
  
}
