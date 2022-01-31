// By: Michael Lees
// Date: 2022-01-30
// Use: Underwater Robotics Team at North Paulding High School (NPHS) manipulator project.

#include <SparkFunLSM6DSO.h>
#include <Servo.h>
#include <Wire.h>

LSM6DSO joint1;

int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

double x;
double y;
double z;

#define step1 5
#define step2 3
#define dir1 4
#define dir2 2

Servo S1;
Servo S2;

int VA = 1500;
int VB = 1500;
int VC = 1500;
int VD = 1500;

float AX_Last = 0;
float AY_Last = 0;
float AZ_Last = 0;

void SerialEvent()
{
  Serial.flush();
  char characterBuf[39];  //stores incoming
  int incomingLength = 0; //stores incoming length
  char *token;            //token for converting byte array to string array
  int counterNum = 1;
  //1503,1503,1500,1503,1523,1000,1500,0\n

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

void setup() 
{
  pinMode( 13, OUTPUT );
  Serial.begin( 115200 );
  delay( 500 );

  Wire.begin( );
  delay( 10 );

  if ( joint1.begin( ) ) {
    Serial.println( "Ready." );
  } else {
    Serial.println( "Could not connect to IMU." );
    Serial.println( "Freezing" );
  }
  if ( joint1.initialize( BASIC_SETTINGS ) ) {
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
  
}

void loop() 
{
  if ( Serial.available( ) ) {
    SerialEvent( );
    float AX = joint1.readFloatAccelX( );
    float AY = joint1.readFloatAccelY( );
    float AZ = joint1.readFloatAccelZ( );

    float M1_Val = map( VA, 1000, 2000, -1, 1 );
    float M2_Val = map( VB, 1000, 2000, -1, 1 );

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

    Serial.print( AX );
    Serial.print("    ");
    Serial.print( AY );
    Serial.print("    ");
    Serial.print( AZ );
    Serial.print("    ");
    Serial.print( AX_Last );
    Serial.print("    ");
    Serial.print( AY_Last );
    Serial.print("    ");
    Serial.print( AZ_Last );
    Serial.print("    ");
    Serial.print( M1_Val );
    Serial.print("    ");
    Serial.print( M2_Val );
    Serial.print("    ");
    Serial.print( VA );
    Serial.print("    ");
    Serial.println( VB );

    AX_Last = AX;
    AY_Last = AY;
    AZ_Last = AZ;
    
    delay(5); 
  }
}
