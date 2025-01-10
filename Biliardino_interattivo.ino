/*
  Codice arduino per il biliardino interattivo

  Questo codice deve fare 3 cose:

  - Rilevare i goal tramite sensori piezoelettrici
  - Gestire le animazioni luminose e sonore
  - Scegliere tramite sensore IR la modalità di animazione
*/

#include <IRremote.h>
#define RECV_PIN  11        //Infrared signal receiving pin
#define LED       13        //define LED pin
IRrecv irrecv(RECV_PIN);
decode_results results;

#include <FastLED.h>
#define LED_PIN 5         // Replace with the pin number you used for DATA connection
#define NUM_LEDS 52       // Replace with the number of LEDs in your strip
#define FLICKER_DELAY 20  // Delay between flicker changes (in milliseconds)
CRGB leds[NUM_LEDS];


const int ledPin = 13;      // LED collegato al pin 13 per indicazioni di stato
const int redSensor = A1;   // Sensori piezo collegati ai pin A1 (rossi) A2 (blu)
const int blueSensor = A2; 

/*
 * Valori chiave per la rilevazione dei goal
 * forse un giorno faremo machine learning
 * ma non è questo il giorno
 */

const int sensTh = 200;  // Threshold per inizio rilevazione
const int redTh = 2500; // Threshold per inizio rilevazione blu
const int blueTh = 2500; // Threshold per inizio rilevazione blu
const int redDiff = 1000;
const int blueDiff = 1000;


int redReading = 0;     // Segnale porta rossa
int blueReading = 0;    // Segale porta blu

int rTemp = 0;    // Variabili di appoggio
int bTemp = 0;

int mode = 0;  // Modalità di animazione   

int counter = 0;


void setup() {
  pinMode(ledPin, OUTPUT);  // declare the ledPin as as OUTPUT
  Serial.begin(9600);       // use the serial port
  FastLED.addLeds<WS2811, LED_PIN, RGB>(leds, NUM_LEDS);
  irrecv.enableIRIn(); // Start the receiver
}

void loop() {
  // read the sensor and store it in the variable sensorReading:
  redReading = 0;
  blueReading = 0;

  counter = 0;

  while(abs(analogRead(redSensor))<redTh && abs(analogRead(blueSensor))<blueTh){
    
    if (irrecv.decode(&results)){ // Rilevazione della modalità tramite sensore IR
      switch(results.value){
        case 16738455:
          mode = 0;
          break;
        case 16724175:
          mode = 1;
          break;
        case 16718055:
          mode = 2;
          break;
        case 16743045:
          mode = 3;
          break;
        case 16716015:
          mode = 4;
          break;
        case 16726215:
          mode = 5;
          break;
        case 16734885:
          mode = 6;
          break;
        case 16728765:
          mode = 7;
          break;
        case 16730805:
          mode = 8;
          break;
        case 16732845:
          mode = 9;
          break;        
      }
      Serial.println(mode);
      irrecv.resume();        // prepare to receive the next value
    }

    if(counter%100 == 0){
      defaultAnimation(mode,counter/100);
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
    if( blueReading > redReading + blueDiff){ // Goal dei Rossi
      goalAnimation(mode, 1);
    } else if(redReading > blueReading + redDiff){ // Goal dei BLu
      goalAnimation(mode, 0);
    }
  }

  
  Serial.print("Rosso: ");
  Serial.println(redReading);
  delay(5);
  Serial.print("Blue: ");
  Serial.println(blueReading);
  
  delay(1000);  // delay to avoid overloading the serial port buffer
}

void goalAnimation(int m, int t){ // t=0 goal dei blu, t=1 goal dei rossi
  switch(m){
    case 0:
      if(t == 0){
      CRGB flameColor = CRGB(255,0,0);
      for(int j=0;j<3;j++){
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = flameColor;
        }
        FastLED.show();
        delay(200);
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CRGB::Black;
        }
        FastLED.show();
        delay(200);
      }
      
    }
    else if(t == 1){
      CRGB flameColor = CRGB(0,255,0);
      for(int j=0;j<3;j++){
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = flameColor;
        }
        FastLED.show();
        delay(200);
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CRGB::Black;
        }
        FastLED.show();
        delay(200);
      }
    }
  }
}

void defaultAnimation(int m, int i){
  i = i%510;
  i = abs(i-255);
  CRGB flameColor = CRGB(i,i,0);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = flameColor;
  }
  FastLED.show();
}
