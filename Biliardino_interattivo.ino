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
#define BUZZER_PIN 8
CRGB leds[NUM_LEDS];


const int ledPin = 13;      // LED collegato al pin 13 per indicazioni di stato
const int redSensor = A2;   // Sensori piezo collegati ai pin A1 (rossi) A2 (blu)
const int blueSensor = A1; 
const unsigned int pinClk = 12;
const unsigned int pinDt = 11;
const unsigned int pinSw = 10;

// Conserva stato ultima posizione encoder
int prevClk;
int prevDt;
int currClk;
int currDt;
int swState;


// Variabile per mantenere il valore del contatore di esempio
int contatore;
int tPress;

/*
 * Valori chiave per la rilevazione dei goal
 * forse un giorno faremo machine learning
 * ma non è questo il giorno
 */

const int sensTh = 200;   // Threshold per inizio rilevazione
const int redTh = 4700;   // Threshold segnale totale porta rossa
const int blueTh = 4900;  // Threshold segnale totale porta blu

const float redFactor = 2.5;    // Differenza minima di segnale durante un goal
const float blueFactor = 2.5;


float redReading = 0;     // Segnale porta rossa
float blueReading = 0;    // Segale porta blu

int rTemp = 0;    // Variabili di appoggio
int bTemp = 0;

int mode = 0;  // Modalità di animazione
int backCol = 0; // Color di fondo
int sMode = 1; // Modalità di suono
int brightness = 5;

const int watchTime = 100; // Cicli consecutivi senza cambiare la backAnimation

unsigned long int counter = 0; // Contatore per le animazioni

bool soundOn = false;

void setup() {
  pinMode(ledPin, OUTPUT);  // declare the ledPin as as OUTPUT
  pinMode(BUZZER_PIN,OUTPUT);
  Serial.begin(9600);       // use the serial port
  FastLED.addLeds<WS2811, LED_PIN, RGB>(leds, NUM_LEDS);
  counter = 0;
  mode = 1;
  tPress = 0;

  pinMode(pinClk, INPUT);
  pinMode(pinDt, INPUT);
  pinMode(pinSw, INPUT);

  prevClk = digitalRead(pinClk);
  prevDt = digitalRead(pinDt);

  contatore = 0;
}

void loop() {
  // read the sensor and store it in the variable sensorReading:
  redReading = 0;
  blueReading = 0;

  while(abs(analogRead(redSensor))<sensTh && abs(analogRead(blueSensor))<sensTh){

    currClk = digitalRead(pinClk);
    currDt = digitalRead(pinDt);
    swState = digitalRead(pinSw);
    tPress = 0;

    // Encoder moved
    if (currClk != prevClk) {

      //Se il valore di dt è uguale al valore corrente di clk
      if (currDt == currClk) {
        brightness ++;
        if(brightness > 100){
          brightness = 100;
        }
      } else {
        brightness --;
        if(brightness < 0){
          brightness = 0;
        }
      }

      //Aggiorna valori
      delay(5);
      
      currClk = digitalRead(pinClk);
      currDt = digitalRead(pinDt);

      prevClk = currClk;
      prevDt = currDt;
    }

    // Entering menu
    while(swState == 0){
      if(tPress == 130){
        tone(8,523,100);
        delay(100);
        tone(8,659,100);
        delay(100);
        tone(8,783,100);
        delay(100);
        while(swState == 0){
          swState = digitalRead(pinSw);
          delay(10);
        }
        brightness = 80;
        backAnimation(backCol,counter/watchTime);
        while(swState == 1){
          currClk = digitalRead(pinClk);
          currDt = digitalRead(pinDt);
          swState = digitalRead(pinSw);

          // Encoder moved
          if (currClk != prevClk) {

            //Se il valore di dt è uguale al valore corrente di clk
            if (currDt == currClk) {
              backCol ++;
              if(backCol > 5){
                backCol = 0;
              }
            } else {
              backCol --;
              if(backCol < 0){
                backCol = 5;
              }
            }

            backAnimation(backCol,counter/watchTime);
            //Aggiorna valori
            delay(300);
            
            currClk = digitalRead(pinClk);
            currDt = digitalRead(pinDt);

            prevClk = currClk;
            prevDt = currDt;
          }
        }

        while(swState == 0){
          swState = digitalRead(pinSw);
          delay(10);
        }

        while(swState == 1){
          currClk = digitalRead(pinClk);
          currDt = digitalRead(pinDt);
          swState = digitalRead(pinSw);

          // Encoder moved
          if (currClk != prevClk) {

            //Se il valore di dt è uguale al valore corrente di clk
            if (currDt == currClk) {
              mode ++;
              if(mode > 1){
                mode = 0;
              }
            } else {
              mode --;
              if(mode < 0){
                mode = 1;
              }
            }

            goalAnimation(mode,0);
            //Aggiorna valori
            delay(300);
            
            currClk = digitalRead(pinClk);
            currDt = digitalRead(pinDt);

            prevClk = currClk;
            prevDt = currDt;
          }
        }
        tone(8,783,100);
        delay(100);
        tone(8,659,100);
        delay(100);
        tone(8,523,100);
        delay(100);
        while(swState == 0){
          swState = digitalRead(pinSw);
          delay(1);
        }
        tPress = 0;
      }

      tPress += 1;
      delay(10);
      swState = digitalRead(pinSw);
    }

    if(tPress > 8){
      if(!soundOn){
        soundOn = true;
        tone(8,523,100);
        delay(100);
        tone(8,783,100);
        delay(100);
      } else {
        soundOn = false;
        tone(8,783,100);
        delay(100);
        tone(8,523,100);
        delay(100);
      }
    }

    if(counter%watchTime == 0){
      backAnimation(backCol,counter/watchTime);
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
      if(soundOn) soundAnimation(sMode);
      
    } else  if( redReading/redFactor > blueReading){ // Goal dei Blu
      goalAnimation(mode, 0);
      if(soundOn) soundAnimation(sMode);
    }
  }
  
  
  Serial.print(blueReading);
  Serial.print(",");
  Serial.println(redReading);
  
  
  delay(1);  // delay to avoid overloading the serial port buffer
}

void goalAnimation(int m, int t){ // t=0 goal dei blu, t=1 goal dei rossi
  CRGB flameColor;
  switch(m){
    case 0: // Standard blink
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

    case 1:  // Dynamic blink
      if(t == 0){
        flameColor = CRGB(255,0,0);
        for(int i=0;i<5;i++){
          for(int j=0;j<25;j++){
            for (int i = 0; i < NUM_LEDS; i++) {
              if(i == 25 - j || i == 26 + j || i == 24 - j || i == 27 + j){
                leds[i] = flameColor;
              } else {
                leds[i] = CRGB(0,0,0);
              }
            }
            FastLED.show();
            delay(6);
          }
        }
      }
      else if(t == 1){
        flameColor = CRGB(0,255,0);
        for(int i=0;i<5;i++){
          for(int j=0;j<25;j++){
            for (int i = 0; i < NUM_LEDS; i++) {
              if(i == j || i == 51 - j || i == j + 1 || i == 50 - j){
                leds[i] = flameColor;
              } else {
                leds[i] = CRGB(0,0,0);
              }
            }
            FastLED.show();
            delay(6);
          }
        }
      }
      break;
  }
}

void backAnimation(int backCol, long int j){
  CRGB flameColor;
  switch(backCol){
    case 0:
      flameColor = CRGB(0*brightness/100,0*brightness/100,255*brightness/100);
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = flameColor;
      }
      FastLED.show();
      break;

    case 1:
      flameColor = CRGB(255*brightness/100,0*brightness/100,0*brightness/100);
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = flameColor;
      }
      FastLED.show();
      break;
    
    case 2:
      flameColor = CRGB(0*brightness/100,255*brightness/100,0*brightness/100);
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = flameColor;
      }
      FastLED.show();
      break;

    case 3:
      flameColor = CRGB(0*brightness/100,227*brightness/100,90*brightness/100);
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = flameColor;
      }
      FastLED.show();
      break;

    case 4:
      flameColor = CRGB(227*brightness/100,217*brightness/100,25*brightness/100);
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = flameColor;
      }
      FastLED.show();
      break;
    
    case 5:
      flameColor = CRGB(227*brightness/100,25*brightness/100,166*brightness/100);
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = flameColor;
      }
      FastLED.show();
      break;
  }
  
}

void soundAnimation(int sMode){
  switch(sMode){
    case 0:
      tone(8,1046);
      delay(100);
      noTone(8);
      delay(50);
      tone(8,1328);
      delay(100);
      noTone(8);
      delay(50);
      tone(8,1567);
      delay(100);
      noTone(8);
      break;
    case 1:
      int bpm = 170;
      int beatDuration = 60000 / bpm;

      char *melody[] = {"A4", "B4", "C#5", "C#5", "C#5", "C#5", "C#5", "D5", "C#5", "B4", "A4", "C#5"};
      int durations[] = {4, 8, 8, 8, 8, 4, 8, 4, 8, 8, 8, 4};
      for (int i = 0; i < sizeof(durations) / sizeof(durations[0]); i++) {
        playNote(melody[i], durations[i], beatDuration);
      }
  }
}

void playNote(char *note, int duration, int beatDuration) {
    int frequency = getNoteFrequency(note);
    int noteDuration = (beatDuration * 4) / duration;  // Convert beats to milliseconds
    
    if (frequency > 0) {
        tone(BUZZER_PIN, frequency, noteDuration-70);
    }
    delay(noteDuration);  // Add slight delay for separation
}

int getNoteFrequency(const char *note) {
    if (strcmp(note, "P") == 0) return 0;  // Pause (silent)
    
    struct { const char *name; int frequency; } noteMap[] = {
        {"C3", 131}, {"C#3", 139}, {"Db3", 139}, {"D3", 147}, {"D#3", 156}, {"Eb3", 156},
        {"E3", 165}, {"F3", 175}, {"F#3", 185}, {"Gb3", 185}, {"G3", 196}, {"G#3", 208},
        {"Ab3", 208}, {"A3", 220}, {"A#3", 233}, {"Bb3", 233}, {"B3", 247}, {"C4", 262},
        {"C#4", 277}, {"Db4", 277}, {"D4", 294}, {"D#4", 311}, {"Eb4", 311}, {"E4", 330},
        {"F4", 349}, {"F#4", 370}, {"Gb4", 370}, {"G4", 392}, {"G#4", 415}, {"Ab4", 415},
        {"A4", 440}, {"A#4", 466}, {"Bb4", 466}, {"B4", 494}, {"C5", 523}, {"C#5", 554},
        {"Db5", 554}, {"D5", 587}, {"D#5", 622}, {"Eb5", 622}, {"E5", 659}, {"F5", 698},
        {"F#5", 740}, {"Gb5", 740}, {"G5", 784}, {"G#5", 831}, {"Ab5", 831}, {"A5", 880},
        {"A#5", 932}, {"Bb5", 932}, {"B5", 988}, {"C6", 1047}, {"C#6", 1109}, {"Db6", 1109},
        {"D6", 1175}, {"D#6", 1245}, {"Eb6", 1245}, {"E6", 1319}, {"F6", 1397}, {"F#6", 1480},
        {"Gb6", 1480}, {"G6", 1568}, {"G#6", 1661}, {"Ab6", 1661}, {"A6", 1760}, {"A#6", 1865},
        {"Bb6", 1865}, {"B6", 1976}
    };
    
    for (int i = 0; i < sizeof(noteMap) / sizeof(noteMap[0]); i++) {
        if (strcmp(note, noteMap[i].name) == 0) {
            return noteMap[i].frequency;
        }
    }
    return 0;  // Default to silence if note is unknown
}