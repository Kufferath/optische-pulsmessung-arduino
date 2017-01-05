
/* Pulsmessung_ die erste 2.te
R.Kufferath  Jan - 2017 f
mit AD-max find   */

#include <SoftwareSerial.h>
#include <Nextion.h>
#include <Average.h>

Average<int> ave(5);  // für Mittelwertbildung gleitend
Average<int> wandler(100);

SoftwareSerial nextion(2, 3);// Nextion TX to pin 2 and RX to pin 3 of Arduino

Nextion myNextion(nextion, 9600); //create a Nextion object serial port @ 9600bps

boolean button1State;
boolean button2State;

int old_sensor_value = 0;
  int sensor = 0;
  int value;
  int Puls;
  int Abstand;
  long timer=0;
int state=0;
long grenze=300;  //Schwelle des AD Wertes für Pulserkennung-  Startwert- wird später angepasst;
  
void setup() {
  Serial.begin(9600);
  myNextion.init();
    pinMode(A0,INPUT);
            }

void loop() {
  int maxat = 0;
  String message = myNextion.listen();

 if(message != ""){ // if a message is received...
Serial.println(message);} //...print it out

  if (message == "65 0 1 0 ffff ffff ffff") {
    myNextion.buttonToggle(button1State, "b0", 0, 2);
    Serial.println("B0 - ");
  }
  if (message == "65 0 2 0 ffff ffff ffff") {
    myNextion.buttonToggle(button2State, "b1", 0, 2);
    Serial.println("B1 - ");
  }
//**************** Messung Puls ***********************
value = analogRead(A0);
//Serial.println(value);
sensor = value;
wandler.push(value);  // Mittelwert des AD Wandlerwertes, MAx/MIN ermitteln...

grenze=wandler.maximum(&maxat)*8/10;  //Anpassen der Erkennungsschwelle auf 80%

 if (value>grenze&& grenze>200) {
    // falls aktiver Zustand 0
    if (state==0) {
      // zu Zustand 1 Übergehen
      state=1;
      // Zeit merken
      timer=millis();
    }
    
    // falls aktiver Zustand 2
    if (state==2) {
      
      // zu Startzustand zurÃ¼ckkehren
      state=0;
      // Zeitdifferenz berechnen
      long ms=millis()-timer;
      // Puls berechnen und ausgeben
      if (ms>400) {
        //Serial.print("Puls (1/min): ");
        Puls= 60000/ms; // Puls berechnet
        Serial.print(Puls);
         Serial.print("  ");
        ave.push(Puls); //Aktuellen Puls in die Mittelwertbildung...
        //Serial.print("Puls (mean): ");
        Puls= 60000/ms; // Puls berechnet
        Serial.println(ave.mean());
        Serial.print("MaxAD:"); Serial.println(wandler.maximum(&maxat));
        Serial.println(grenze);
      }
    }
  }
  if (value<300 && state==1) {
    // zu Zustand 2 Ã¼bergehen
    state=2;
  }
//*************************************************************

  if (abs(sensor - old_sensor_value) > 20) { //Nur Updaten wenn eine Differenz größerer 2 ist
    old_sensor_value = sensor;

    int scaled_value = map(sensor, 0, 1023, 0, 100); // always map value from 0 to 100

     myNextion.setComponentValue("j0", scaled_value);  //Progressbar setzen
     
      myNextion.setComponentText("t0", String(ave.mean(),0 ));  // Ausgabe Puls an t0
     
     if (ave.mean()>60 && ave.mean()<120)  {
       myNextion.setComponentText("t6", "98");  // Oxygen
      myNextion.setComponentText("t1", "Ruhe :)"); } // Ruhe
      else  myNextion.setComponentText("t1", " - - - ");
        if (ave.mean()>120)  {
      myNextion.setComponentText("t1", "Stress !");  // Stress
       myNextion.setComponentText("t6", "97");  // Oxyen
       }
   
    
  }
}

