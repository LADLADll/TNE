#include <SPI.h>
#include <arduino.h>
#include <rgb_lcd.h>
#include <Wire.h>
#define LOG_PERIOD 5000                                                         //meting refreshen na ...µs
#define MAX_PERIOD 60000                                                        //maximale duur van een meting (µs)

//Defineer de variabelen
rgb_lcd lcd;
int counts;                                                                     //variabele voor het aantal tellen (unsigned omdat geen negatieve getallen moeten opgeslagen worden)
double cpm;                                                                     //variabele voor het aantal tellen per minuut (unsigned long omdat geen negatieve getallen moeten opgeslagen)                                              //variable for calculation CPM in this sketch (int???) (is deze nodig???)
unsigned long previousMillis = 0;                                               //variable for time measurement (unsigned long omdat geen negatieve getallen moeten opgeslagen)
unsigned long currentMillis;
unsigned long startMillis;
boolean eerstUit = 1;
boolean startKnop;
int a0;
int presetTijd_min;
int presetTijd_s;
int meetTijd_ms;
int meetTijd_s;
//long meetTijd_min;
int vorigeMeetTijd_s;


//Aansluiting digitale io
const byte ledPin = 6;                                                          //digitale pin 6 als uitgang voor led
const byte schuifSchakelaar = 8;                                                //digitale pin 8 als ingang van de schuifschakelaar
const byte GMTube = 2;                                                          //digitale pin 2 als ingang GM-buis

//Aansluiting analoge INPUT
const byte potentiometer = 0;                                                   //analoge pin 0 als ingang potentiometer

//Instellen kleur lcd-scherm
const int lcdRood = 100;
const int lcdGroen = 100;
const int lcdBlauw = 100;

//Telkens een puls wordt gedetecteerd, moet het aantal counts met 1 verhogen
void tube_impulse(){
  startKnop = digitalRead(8);
  if(startKnop ==1 ){
  counts++;
  }
}

//Begininstelling
void setup(){
  lcd.begin(16,2);                                                              //dimensie lcd-scherm, 16 kolommen, 2 rijen
  lcd.setRGB(lcdRood, lcdGroen, lcdBlauw);                                      //instellen kleur lcd-scherm
  counts = 0;
  cpm = 0;
  Serial.begin(9600);                                                           //start serial monitor
  pinMode(GMTube, INPUT);                                                       //digitale pin 2 als ingang GM-buis
  pinMode(schuifSchakelaar, INPUT);                                             //digitale pin 8 als ingang van de schuifschakelaar
  pinMode(ledPin, OUTPUT);                                                      //digitale pin 6 als uitgang voor led
  //digitalWrite(2, LOW);       //testen of dit wel nodig is
  //digitalWrite(8, LOW);
  //digitalWrite(7, LOW);
  //digitalWrite(6, LOW);
}

void loop(){
  //attachInterrupt(digitalPinToInterrupt(GMTube), tube_impulse, FALLING);
  a0 = analogRead(potentiometer);                                               //instellen meettijd adhv potentiometer, van 1 tot 10min of continu (0)
  presetTijd_min = (1024 - a0)/102,4;
  presetTijd_s = presetTijd_min * 60;

  startKnop = digitalRead(8);
  currentMillis = millis();

  if (startKnop == 1){

    if(eerstUit == true){                                                       //schakelaar uit --> aan: reset

      counts = 0;
      cpm = 0;
      meetTijd_s = 0;
      //meetTijd_min = 0;
      eerstUit = false;

      startMillis = millis();

      if(presetTijd_min == 0){
        attachInterrupt(digitalPinToInterrupt(GMTube), tube_impulse, FALLING);
        //meetTijd_s = (currentMillis - previousMillis)*1000;
        //meetTijd_min = meetTijd_s/60;
        //cpm = counts/(meetTijd_min);
        digitalWrite(ledPin, HIGH);
        vorigeMeetTijd_s = meetTijd_s;                                                  //led brandt als meting loopt
        }

      else{
        meetTijd_s = (currentMillis - startMillis)/1000;
        //meetTijd_min = meetTijd_s/60;

        if(meetTijd_s < presetTijd_s){
          vorigeMeetTijd_s = meetTijd_s;
          attachInterrupt(digitalPinToInterrupt(GMTube), tube_impulse, FALLING);
          cpm = counts/(vorigeMeetTijd_s)*60;
          digitalWrite(ledPin, HIGH);

          }
        else{
          digitalWrite(ledPin, LOW);                                             //led doofft als meting gedaan is
          detachInterrupt(digitalPinToInterrupt(2));
          }
      }
    }

    else{
      currentMillis = millis();
      meetTijd_s = (currentMillis-startMillis)/1000;
      //meetTijd_min = meetTijd_s/60;

      if(presetTijd_min == 0) {
        vorigeMeetTijd_s = meetTijd_s;
        attachInterrupt(digitalPinToInterrupt(GMTube), tube_impulse, FALLING);
        cpm = counts/(vorigeMeetTijd_s*60);
        digitalWrite(ledPin, HIGH);
        }

      else{
        if(meetTijd_s < presetTijd_s){
          vorigeMeetTijd_s = meetTijd_s;
          attachInterrupt(digitalPinToInterrupt(GMTube), tube_impulse, FALLING);
          cpm = counts/(vorigeMeetTijd_s*60);
          digitalWrite(ledPin, HIGH);
        }
        else{
          digitalWrite(ledPin, LOW);
          detachInterrupt(digitalPinToInterrupt(GMTube));
        }
      }
    }
  }

  if(startKnop == LOW){
    digitalWrite(6, LOW);
    detachInterrupt(digitalPinToInterrupt(GMTube));
    eerstUit = true;
    }



  //Projectie op het lcd-scherm
  lcd.setCursor(0, 1);
  lcd.print(cpm,2);
  lcd.print("cpm");
  lcd.setCursor(9, 1);
  lcd.print(counts);
  lcd.setCursor(0, 0);
  lcd.print(vorigeMeetTijd_s);
  lcd.print("s");

  if(presetTijd_min != 0){
  lcd.setCursor(9, 0);
  lcd.print(presetTijd_min);
  lcd.print("min     ");;
  }
    else{
      lcd.setCursor(9,0);
      lcd.print("continu");
      delay(1000);
      }
}
