// By: Michael Lees
// Date: 2022-01-30
// Use: Underwater Robotics Team at North Paulding High School (NPHS) manipulator project.

#include <SparkFunLSM6DSO.h>
#include <Servo.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

/****************** SD Card ******************/
File fd;
// AX: -0.000, AY: -0.000, AZ: -0.000, AX_Last: -0.000, AY_Last: -0.000, AZ_Last: -0.000, M1_Val: 1500, M2_Val: 1500, VA: 1500, VB: 1500EOF\n
// tilt 1: AX: -0.000, AY: -0.000, AZ: -0.000, AX_Last: -0.000, AY_Last: -0.000, AZ_Last: -0.000, M1_Val: 1500, M2_Val: 1500, VA: 1500, VB: 1500, tilt 2: AX: -0.000, AY: -0.000, AZ: -0.000, AX_Last: -0.000, AY_Last: -0.000, AZ_Last: -0.000, M1_Val: 1500, M2_Val: 1500, VA: 1500, VB: 1500EOF\n
const uint8_t BUFFER_SIZE = 292;
char fileName[] = "recoFile.txt";
char buff[BUFFER_SIZE + 2] = "";
uint8_t index = 0;

const uint8_t chipSelect = 8;
const uint8_t cardDetect = 9;

enum states: uint8_t { NORMAL, E, EO };
uint8_t state = NORMAL;

bool alreadyBegan = false;

/*********************************************/

/****************** Steppers *****************/
LSM6DSO joint1;
LSM6DSO joint2;

int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
int16_t AcX1, AcY1, AcZ1, Tmp1, GyX1, GyY1, GyZ1;

double x;
double y;
double z;
double x1;
double y1;
double z1;

#define step1 5
#define step2 3
#define dir1 4
#define dir2 2

float AX_Last = 0;
float AY_Last = 0;
float AZ_Last = 0;
float AX1_Last = 0;
float AY1_Last = 0;
float AZ1_Last = 0;
/********************************************/

/****************** Servos ******************/
Servo S1;
Servo S2;
/********************************************/

/****************** Serial ******************/
int VA = 1500;
int VB = 1500;
int VC = 1500;
int VD = 1500;
/********************************************/

void SerialEvent()
{
  Serial.flush();
  char characterBuf[22];  //stores incoming
  int incomingLength = 0; //stores incoming length
  char *token;            //token for converting byte array to string array
  int counterNum = 1;

  if (Serial.available()) {
    //    Serial.println("1");
    incomingLength = Serial.readBytesUntil('\n', characterBuf, 50);    //calculate length of byte array
    token = strtok(characterBuf, ",");  //convert to string
    VA = atoi(token);
    //    Serial.println(token);
    while (token != NULL) {   //if token doesnt find another comma it goes back to beginning

      token = strtok(NULL, ",");  //changes token to a string def of NULL
      //      Serial.println(token);

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

void initializeCard(void)
{
  Serial.print(F("Initializing SD card..."));

  // Is there even a card?
  if (!digitalRead(cardDetect))
  {
    Serial.println(F("No card detected. Waiting for card."));
    while (!digitalRead(cardDetect));
    delay(250); // 'Debounce insertion'
  }

  // Card seems to exist.  begin() returns failure
  // even if it worked if it's not the first call.
  if (!SD.begin(chipSelect) && !alreadyBegan)  // begin uses half-speed...
  {
    Serial.println(F("Initialization failed!"));
    initializeCard(); // Possible infinite retry loop is as valid as anything
  }
  else
  {
    alreadyBegan = true;
  }
  Serial.println(F("Initialization done."));

  Serial.print(fileName);
  if (SD.exists(fileName))
  {
    Serial.println(F(" exists."));
  }
  else
  {
    Serial.println(F(" doesn't exist. Creating."));
  }

  Serial.print("Opening file: ");
  Serial.println(fileName);

  Serial.println(F("Good to go."));
}

void flushBuffer(String s)
{
  fd = SD.open(fileName, FILE_WRITE);
  fd.println(s);
  fd.close();
}

void setup() 
{
  pinMode( 13, OUTPUT );
  Serial.begin( 250000 );
  delay( 500 );

  Wire.begin( );
  delay( 10 );

  if ( joint1.begin( 0x6A ) ) {
    Serial.println( "Ready." );
  } else {
    Serial.println( "Could not connect to IMU." );
    Serial.println( "Freezing" );
  }
  if ( joint1.initialize( BASIC_SETTINGS ) ) {
    Serial.println( "Loaded Settings." );
  }

  if ( joint2.begin( 0x6B ) ) {
    Serial.println( "Ready." );
  } else {
    Serial.println( "Could not connect to IMU." );
    Serial.println( "Freezing" );
  }
  if ( joint2.initialize( BASIC_SETTINGS ) ) {
    Serial.println( "Loaded Settings." );
  }

  S1.attach( 7 );
  S2.attach( 6 );

  pinMode( step1, OUTPUT );
  pinMode( step2, OUTPUT );
  pinMode( dir1, OUTPUT );
  pinMode( dir2, OUTPUT );

  S1.writeMicroseconds( 1500 );
  S2.writeMicroseconds( 1500 );
 
  pinMode(cardDetect, INPUT);
  initializeCard();
  
}

void loop() 
{
  if (!digitalRead(cardDetect)) {
    initializeCard();
  }
  if ( Serial.available( ) ) {
    SerialEvent( );

    // M1 Control
    float AX = joint1.readFloatAccelX( );
    float AY = joint1.readFloatAccelY( );
    float AZ = joint1.readFloatAccelZ( );

    float M1_Val = map( VA, 1000, 2000, -1, 1 );

    while ( AX != M1_Val ) {   
      AX = joint1.readFloatAccelX( ); 
      if ( AX <= M1_Val && M1_Val <= 0 ) {
        digitalWrite( dir1, HIGH );
        for ( int x = 0; x < 100; x++ ) {
          digitalWrite( step1, HIGH );
          delayMicroseconds( 1 );
          digitalWrite( step1, LOW );
          delayMicroseconds( 1 );
        }
      } else if ( AX >= M1_Val && M1_Val >= 0 ) {
        digitalWrite( dir1, LOW );
        for ( int x = 0; x < 100; x++ ) {
          digitalWrite( step1, HIGH );
          delayMicroseconds( 1 );
          digitalWrite( step1, LOW );
          delayMicroseconds( 1 );
        }
      } else {
        digitalWrite( dir1, LOW );
        digitalWrite( step1, LOW );
      }
    }

    // M2 Control
    float AX1 = joint2.readFloatAccelX( );
    float AY1 = joint2.readFloatAccelY( );
    float AZ1 = joint2.readFloatAccelZ( );

    float M2_Val = map( VB, 1000, 2000, -1, 1 );

    while ( AX1 != M2_Val ) {   
      AX1 = joint2.readFloatAccelX( ); 
      if ( AX1 <= M2_Val && M2_Val <= 0 ) {
        digitalWrite( dir2, HIGH );
        for ( int x = 0; x < 100; x++ ) {
          digitalWrite( step2, HIGH );
          delayMicroseconds( 1 );
          digitalWrite( step2, LOW );
          delayMicroseconds( 1 );
        }
      } else if ( AX1 >= M2_Val && M2_Val >= 0 ) {
        digitalWrite( dir2, LOW );
        for ( int x = 0; x < 100; x++ ) {
          digitalWrite( step2, HIGH );
          delayMicroseconds( 1 );
          digitalWrite( step2, LOW );
          delayMicroseconds( 1 );
        }
      } else {
        digitalWrite( dir2, LOW );
        digitalWrite( step2, LOW );
      }
    }


    // debugging
    String s1 = "tilt 1: AX: " + String(AX) + ", AY: " + String(AY) + ", AZ: " + String(AZ) + ", AX_Last: " + String(AX_Last) + ", AY_Last: " + String(AY_Last) + ", AZ_Last: " + String(AZ_Last) + ", M1_Val: " + String(M1_Val) + ", ";
    String s2 = "AX: " + String(AX1) + ", AY: " + String(AY1) + ", AZ: " + String(AZ1) + ", AX_Last: " + String(AX1_Last) + ", AY_Last: " + String(AY1_Last) + ", AZ_Last: " + String(AZ1_Last) + ", M2_Val: " + String(M2_Val) + ", VA: " + String(VA) + ", VB: " + String(VB) + "EOF\n"; 

    String s = s1 + ", tilt 2: " + s2;

    Serial.println(s);
  
    flushBuffer(s);
    
  }
}
