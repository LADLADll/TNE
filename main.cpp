#include <SPI.h>
#include <arduino.h>
#include <rgb_lcd.h>
#include <Wire.h>
#define LOG_PERIOD 500                                                          //Logging period in milliseconds, recommended value 15000-60000.
#define MAX_PERIOD 60000                                                        //Maximum logging period

//Defineer de variabelen
rgb_lcd lcd;
unsigned long counts;                                                           //variabele voor het aantal tellen
unsigned long cpm;                                                              //variabele voor het aantal tellen per minuut
unsigned int multiplier;                                                        //variable for calculation CPM in this sketch
unsigned long previousMillis;                                                   //variable for time measurement
unsigned long startKnop;
unsigned long currentMillis;
float aantalMinutenAanHetMeten;
boolean eerstUit;
boolean eerstAan;

//instellen kleur lcd-scherm
const int lcdRood = 100;
const int lcdGroen = 100;
const int lcdBlauw = 100;

// telkens een puls wordt gedetecteerd, moet het aantal counts met 1 verhogen
void tube_impulse(){
  counts++;
}

// Begininstelling
void setup(){
  lcd.begin(16,2);
  lcd.setRGB(lcdRood, lcdGroen, lcdBlauw);                                // beginkleur
  counts = 0;
  cpm = 0;
  multiplier = MAX_PERIOD / LOG_PERIOD;                                         //calculating multiplier, depend on your log period
  Serial.begin(9600);                                                           // start serial monitor
  pinMode(2, INPUT);
  pinMode(8, INPUT);                                                            // set pin INT0 input for capturing GM Tube events
  digitalWrite(2, LOW);                                                         // turn on internal pullup resistors, solder C-INT on the PCB WAS HIGH IPV LOW
  digitalWrite(8, LOW);
}

void loop(){
  startKnop = digitalRead(8);
  attachInterrupt(digitalPinToInterrupt(2), tube_impulse, FALLING);
  if (startKnop > 0) {
    currentMillis = millis();
    eerstUit = true;
    if(currentMillis - previousMillis > LOG_PERIOD){
        previousMillis = currentMillis;
        }
  }                                       //main cycle
  else {
    if (eerstUit == true){
        lcd.clear();
        eerstUit = false;
    }
    counts = 0;
    cpm = 0;
    currentMillis = 0;
  }
  aantalMinutenAanHetMeten = currentMillis/60000;
  cpm = counts/aantalMinutenAanHetMeten;
  lcd.setCursor(0, 1);
  lcd.print(cpm);
  lcd.print(" cpm; ");
  lcd.print(counts);
  lcd.setCursor(0, 0);
  lcd.print(currentMillis/1000);
  lcd.print(" seconden");
}
