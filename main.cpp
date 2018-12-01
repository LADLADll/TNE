#include <SPI.h>
#include <arduino.h>
#include <rgb_lcd.h>
#include <Wire.h>
#define LOG_PERIOD 500                                                //meting refreshen na ...µs (Logging period in milliseconds, recommended value 15000-60000.)
#define MAX_PERIOD 60000                                              //maximale duur van een meting (µs) (Maximum logging period)

//Defineer de variabelen
rgb_lcd lcd;
unsigned long counts;                                                 //variabele voor het aantal tellen (unsigned long omdat geen negatieve getallen moeten opgeslagen)
unsigned long cpm;                                                    //variabele voor het aantal tellen per minuut (unsigned long omdat geen negatieve getallen moeten opgeslagen)
unsigned int multiplier;                                              //variable for calculation CPM in this sketch (int???) (is deze nodig???)
unsigned long previousMillis;                                         //variable for time measurement (unsigned long omdat geen negatieve getallen moeten opgeslagen)
unsigned long startKnop;                                              
unsigned long currentMillis;
float aantMinMeten;                                                   //het aantal minuten dat er gemeten is (float omdat dit een kommagetal kan zijn)
boolean eerstUit;
//??? boolean eerstAan;

//Instellen kleur lcd-scherm
const int lcdRood = 100;
const int lcdGroen = 100;
const int lcdBlauw = 100;

//Telkens een puls wordt gedetecteerd, moet het aantal counts met 1 verhogen
void tube_impulse(){
  counts++;
}

//Begininstelling
void setup(){
  lcd.begin(16,2);                                                      //dimensie lcd-scherm 
  lcd.setRGB(lcdRood, lcdGroen, lcdBlauw);                              //instellen kleur lcd-scherm
  counts = 0;
  cpm = 0;
 //??? multiplier = MAX_PERIOD / LOG_PERIOD;                                 //calculating multiplier, depend on your log period
  Serial.begin(9600);                                                   // start serial monitor
  pinMode(2, INPUT);                                                    //digitale pin 2 als ingang van de schuifschakelaar
  pinMode(8, INPUT);                                                    //digitale pin 8 als ingang GM-buis (set pin INT0 input for capturing GM Tube events)
  digitalWrite(2, LOW);                                                 // (turn on internal pullup resistors, solder C-INT on the PCB WAS HIGH IPV LOW)
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
  //Bereken het aantal cpm
  aantMinMeten = currentMillis/60000;
  cpm = counts/aantMinMeten;
  //Projectie op het lcd-scherm
  lcd.setCursor(0, 1);
  lcd.print(cpm);
  lcd.print(" cpm; ");
  lcd.print(counts);
  lcd.setCursor(0, 0);
  lcd.print(currentMillis/1000);
  lcd.print(" seconden");
}
