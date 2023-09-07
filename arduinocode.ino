/*Final code for the Arduino onboard board which controls the  main switching of the smart power diverter unit*/
/*version 4.7 final update, future updates will be uploaded on the website or git hub page.*/
/*All previous iterations are removed and fixes are done--Anindya Malik*/
/*Note that the main controller board should be designed to have higher redundancy, do not forget to put/connect the signal wires coming 
from the esp8266 board through a general-purpose diode as it prevents it from having any reverse polarity issues.*/

#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define OLED_SDA A4 //defining the oled data pin
#define OLED_SCL A5 // defining the oled clock pin
#define OLED_RST 4 // reset pin is set to pin 4 of the Arduino nano

Adafruit_SSD1306 display(OLED_RST);

const int r1= 5; //declaration and assigning of the relays.
const int r2= 2;
const int r3= 3;
const int r4= 4;

int v0=7; //IOT input of the different signal wires.
int v1=9;
int v2=10;
int v3=8;

bool stateSwitched = false; //flags for switching 
bool stateSwitched1 = false;
bool stateSwitched2 = false;

int sensor1, sensor2, sensor3, sensor4; //decleration of sensor input

void setup() {
  Serial.begin(9600); 

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.clearDisplay();

  pinMode(r1, OUTPUT);
  pinMode(r2, OUTPUT);
  pinMode(r3, OUTPUT);
  pinMode(r4, OUTPUT);

  pinMode(v0,INPUT);
  pinMode(v1,INPUT);
  pinMode(v2,INPUT);
  pinMode(v3,INPUT); 

  display.setCursor(0,0);
display.setTextSize(1);
String m3="INTIALIZING WIFI........."; //Testing the display with any output.
for(int k=0;k<m3.length();k++){ //producing the output serially, to check/ determine the speed of the Arduino. 
  display.write(m3[k]);
  display.display();
  delay(10);
}
  delay(600);

  display.clearDisplay(); //clearing the display.
  display.display();   

}

void loop() {
  sensor1= analogRead(A0); //taking the reading from the first sensor, which is the mains voltage input.
  float voltage1 = sensor1 * (24.0 / 1023.0);
  float mp1 = map(voltage1, 1.00, 24.0, 0, 100); //mapping the mains voltage to make it much more readable and make it easier to display on the oled screen.;

  sensor2= analogRead(A1); // taking the reading for the second sensor which is used to take inputs for the battery or external source inputs.
  float voltage2 = sensor2 * (24.0 / 1023.0);
  float mp2=map(voltage2, 0.00, 12.00, 0, 100);  //  mapping the given values to make it easier to read.

 

  int v_0 = digitalRead(v0); // reading the different values of the IOT inputs which are assigned in the blynk cloud.
  int v_1 = digitalRead(v1);
  int v_2 = digitalRead(v2);
  int v_3 = digitalRead(v3);

  Serial.print("voltage1= ");
  Serial.print(voltage1);

  Serial.print("voltage2= ");
  Serial.println(voltage2);


  if(v_0==HIGH){ // main check, to check whether the main power switch is turned on or not  in the app.


    if(v_3==HIGH){ // logic for forced battery mode the highest priority mode in the app as well as  in the hardware.
    digitalWrite(r4,LOW);
    digitalWrite(r1,LOW);
    digitalWrite(r2,LOW);

     if (!stateSwitched) {
      digitalWrite(r3, LOW);
      delay(4000);  // Delay for relay switching
      digitalWrite(r3, HIGH);
      stateSwitched = true;
      stateSwitched1 = false;
      stateSwitched2 = false;
    }
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Forced battery mode   turned ON");
    display.setCursor(0, 20);
    display.print("BATTERY POWER:");
    display.print(mp2);
    display.print("%");
    display.display();
    display.display();

  }

 else if((voltage1 < 20.00 && voltage2 < 12)||(v_2==HIGH)) { //external power sourcing mode logic 2nd priority mode.
    // Mains power is cut off, turn off relay 3 and turn on relay 4
    digitalWrite(r4, HIGH);
    digitalWrite(r2,LOW);
    digitalWrite(r1,LOW);

    if (!stateSwitched1) {
      digitalWrite(r3, LOW);
      delay(4000);
      digitalWrite(r3,HIGH);
      stateSwitched = false;
      stateSwitched1 = true;
      stateSwitched2 = false;
    }
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("External power       sourcing ENABLED");
    display.display();
  }

     else if ((voltage1 > 20.00)||(v_1==HIGH)) {  // main power sourcing logic which  has the least priority.
    // Mains power is present, turn on relay 1 and turn on relay 2
    digitalWrite(r1, HIGH);
    digitalWrite(r2, HIGH);
    digitalWrite(r4,LOW);
        display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Main sourcing Enabled");
    
    if (!stateSwitched2) {
      digitalWrite(r3, LOW);
      delay(4000);  // Delay for relay switching
      digitalWrite(r3, HIGH);
      stateSwitched = false;
      stateSwitched1 = false;
      stateSwitched2 = true;
    }

    display.setCursor(0,8);
    display.print("INPUT POWER:"); // displaying the main power in percentage.
    display.print(mp1);
    display.print("%");
    display.setCursor(0,20);
    display.print("BATTERY POWER:"); //diaplaying the  battery power, the mapped one. 
    display.print(mp2);
    display.print("%");
    display.display();
  }
 
  }
  else{
  digitalWrite(r1,LOW);
  digitalWrite(r2,LOW);
  digitalWrite(r3,LOW);
  digitalWrite(r4,LOW);
   stateSwitched = false;
   stateSwitched1 = false;
   stateSwitched2 = false;

   display.clearDisplay();
   display.setCursor(0,0);
   display.println("UNIT IS TURNED OFF  "); // the oled display will  display the following text if the main power switch is not turned on.
   display.print("Please turn on the    unit from the app,");
   display.display();
   
  }
}
