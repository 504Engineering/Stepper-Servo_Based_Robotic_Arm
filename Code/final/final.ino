#include <AMIS30543.h>
#include <Servo.h>
#include <Wire.h> 
#include <SPI.h>

Servo Cam;
Servo T;
Servo C;

int encoder1 = A7;

// Joint 1 motor pins
const uint8_t j1_dirpin = 6;
const uint8_t j1_steppin = 5;
const uint8_t j1_cs = 7;

// Instantiate joint 1 motor controller
AMIS30543 j1;

// Serial Data variables
int VA = 1500;
int VB = 1500;
int VC = 1500;
int VD = 1500;

void SerialEvent()
{
  Serial.flush();
  char characterBuf[22];  //stores incoming
  int incomingLength = 0; //stores incoming length
  char *token;            //token for converting byte array to string array
  int counterNum = 1;
  //1500,1500,1500,1500\n

  if (Serial.available()) {
    //    Serial.println("1");
    incomingLength = Serial.readBytesUntil('\n', characterBuf, 50);    //calculate length of byte array
    token = strtok(characterBuf, ",");  //convert to string
    VA = atoi(token);
    //    Serial.println(token);
    while (token != NULL) {   //if token doesnt find another comma it goes back to beginning
      token = strtok(NULL, ",");  //changes token to a string def of NULL
      switch (counterNum) {
        case 1:
          VB = atoi(token);
          break;

        case 2:
          VC = atoi(token);
          break;

        case 3:
          VD = atoi(token);
          break;
      }
      counterNum++;
    }
  }
}

void setup() 
{
  Cam.attach(2);
  T.attach(3);
  C.attach(4);
  
  // Starting all the things we need later
  Serial.begin(115200);
  SPI.begin();
  Wire.begin();
  delayMicroseconds(500);

  // initialize the motor controllers
  j1.init(j1_cs);
  delayMicroseconds(500);

  // Write all output pins to low to stop it breaking.
  digitalWrite(j1_steppin, LOW);
  pinMode(j1_steppin, OUTPUT);
  digitalWrite(j1_dirpin, LOW);
  pinMode(j1_dirpin, OUTPUT);
  
  // Set the settings for the stepper motors as follows. 2.4 Amps per motor
  j1.resetSettings();
  j1.setCurrentMilliamps(2070);
  j1.setStepMode(32);
  j1.enableDriver();
}

void loop() 
{
  SerialEvent();
  
  int data = analogRead(encoder1);
  
  int pos1 = map(VA, 1000, 2000, 265, 730);
  
  if (data > pos1 + 2) {
      digitalWrite(j1_dirpin, HIGH);
      for (int i = 0; i < 200; i++) {
        digitalWrite(j1_steppin, HIGH);
        delayMicroseconds(4);
        digitalWrite(j1_steppin, LOW);
        delayMicroseconds(4);
      }
  } else if (data < pos1 - 2) {
      digitalWrite(j1_dirpin, LOW);
      for (int i = 0; i < 200; i++) {
        digitalWrite(j1_steppin, HIGH);
        delayMicroseconds(4);
        digitalWrite(j1_steppin, LOW);
        delayMicroseconds(4);
      }
  } else {
    digitalWrite(j1_dirpin, LOW);
    digitalWrite(j1_steppin, LOW);
  }

  T.writeMicroseconds(VB);
  C.writeMicroseconds(VC);
  Cam.writeMicroseconds(VD);

//  String datastring = String(data);
//  String VAs = String(VA);
//  String VBs = String(VB);
//  String VCs = String(VC);
//  String VDs = String(VD);
//  String pos1s = String(pos1);
//
//  Serial.println(datastring + "," + VAs + "," + VBs + "," + VCs + "," + VDs + "," + pos1s);
}
