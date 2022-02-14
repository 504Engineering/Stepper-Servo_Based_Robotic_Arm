// Servo Library
#include <Servo.h>

// Libraries for other stuff
#include <SparkFunLSM6DSO.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <AMIS30543.h>

// SD Card variables
File fd;

const uint_tBUFFER_SIZE = 292;
char filename[] = "recoFile.txt";
char buff[BUFFER_SIZE + 2] = "";
uint8_t index = 0;

const uint8_t chipSelect = 2;
const uint8_t cardDetect = 3;

enum states: uint8_t { NORMAL, E, EO};
uint8_t state = NORMAL;

bool alreadyBegan = false;

/*****************************************/

// Stepper Motors and Servos variables
// Stepper Motor 1
const uint8_t amisDirPin = 4;
const uint8_t amisStepPin = 5;
const uint8_t amisSlaveSelect = 6;

AMIS30543 stepper;
AMIS30543 stepper2;

// Stepper Motor 2
const uint8_t amisDirPin2 = 7;
const uint8_t amisStepPin2 = 8;
const uint8_t amisSlaveSelect2 = 9;

/*****************************************/

// Accelerometer variables
int16_t AcX, AcY, AcZ, AcX1, AcY1, AcZ1;
double x, y, z, x1, y1, z1;
double AcX_o, AcY_o, AcZ_o, AcX1_o, AcY1_o, AcZ1_o;

/*****************************************/

// Servo variables
Servo S1;
Servo S2;

/*****************************************/

// Serial data variables
int VA = 1500;
int VB = 1500;
int VC = 1500;
int VD = 1500;

/*****************************************/

void step( uint8_t pin )
{
  digitalWrite( pin, HIGH );
  delayMicroseconds( 3 );
  digitalWrite( pin, LOW );
  delayMicroseconds( 3 );
  delayMicroseconds( 2000 );
}

void setDirection( uint8_t pin, bool dir )
{
  delayMicroseconds( 1 );
  digitalWrite( pin, dir );
  delayMicroseconds( 1 );
}

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

void initializeCard( void )
{
  Serial.print( F("Initializing SD card...") );

  // Is there even a card?
  if ( !digitalRead(cardDetect) )
  {
    Serial.println(F( "No card detected. Waiting for card.") );
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

void flushBuffer( String s )
{
  fd = SD.open( fileName, FILE_WRITE );
  fd.println( s );
  fd.close( );
}

void setup( ) 
{
    Serial.begin( 250000 );
    delay( 500 ); 

    Wire.begin( );
    delay( 10 );

    if ( stepper.begin( 0x6A ) ) {
        Serial.println( "Ready." );
    } else {
        Serial.println( "Could not connect to IMU." );
        Serial.println( "Freezing" );
    }
    if ( stepper.initialize( BASIC_SETTINGS ) ) {
        Serial.println( "Loaded Settings." );
    }

    if ( stepper2.begin( 0x6B ) ) {
        Serial.println( "Ready." );
    } else {
        Serial.println( "Could not connect to IMU." );
        Serial.println( "Freezing" );
    }
    if ( stepper2.initialize( BASIC_SETTINGS ) ) {
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

void loop( ) 
{

}