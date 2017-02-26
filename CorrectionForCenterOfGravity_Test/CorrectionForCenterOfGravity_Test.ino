/* Manual Center of Gravity Control using bluetooth without gyro and ultrasound */
/* Control via PC(Teraterm) and Phone(Blueterm or ArduinoRC) */

/* ************ MANUAL ************ */
/* 		
		Initial Controls:
		T - Low Brake (Initialize) or Arming the rotors
		V - TakeOff Boost (Speed +10 when operating at <=25 speed)
		
		Primary Controls:
		I - Ascend
 		K - Descend
 		J - Turn Left
 		L - Turn Right
		
		Additional Controls:
 		5 - High Brake  (Warning : Usage at wrong timing results in heavy boost)
		O - Decrease Acceleration limit by 1
 		P - Increase Acceleration limit by 1
		Space - Toggle Emergency Light
		
		Controls in action during Manual Stability State:
		Q - Front Left Rotor
		W - Front Right Rotor
		A - Rear Left Rotor
		S - Rear Right Rotor
		D - No Action
		
 */
/* ************ MANUAL *************/

#include <SoftwareSerial.h>   //Software Serial Port
#include <Servo.h>
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t iniX,iniY;
int FL,FR,RL,RR;
int led = 13;
boolean ledOnState=false;
boolean gyroOnState=true;
boolean ultrasoundOnState=true;
boolean manualStabilityOnState=false;

Servo Front_Left_arm;        //5
Servo Rear_Left_arm;         //9
Servo Rear_Right_arm;        //10
Servo Front_Right_arm;       //11

#define RxD 6
#define TxD 7

#define ECHOPIN 3
#define TRIGPIN 2

#define HIGH_CUT_OFF 100
#define LOW_CUT_OFF 20

#define DEBUG_ENABLED  1

SoftwareSerial blueToothSerial(RxD,TxD);

void setup()
{
  pinMode(led, OUTPUT);
  Front_Left_arm.attach(5);
  Rear_Left_arm.attach(9);
  Rear_Right_arm.attach(10);
  Front_Right_arm.attach(11);

  Wire.begin();
  Serial.begin(9600);
  pinMode(ECHOPIN,INPUT);
  pinMode(TRIGPIN,OUTPUT);
  accelgyro.initialize();
  setupBlueToothConnection();

  accelgyro.getMotion6(&ay, &ax, &az, &gx, &gy, &gz);
  iniX=ax/100;
  iniY=ay/100;
  blueToothSerial.print(iniX);
  blueToothSerial.print(" ");
  blueToothSerial.print(iniY);
  Serial.print(" iniX:");
  Serial.print(iniX);
  Serial.print(" iniY:");
  Serial.println(iniY);
}

void loop()
{
  FL=10,FR=10,RL=10,RR=10;
  int val=0, limit=1, breakSpeed=5;
  int altitudeControl=0;
  char ch;
  while(1)
  {

    if(blueToothSerial.available())
    {
      ch = blueToothSerial.read();
      blueToothSerial.print(ch);
      switch(ch)
      {
      case '5':
        val=HIGH_CUT_OFF;         //Set high cut-off ex.100
        FL=val;
        FR=val;
        RL=val;
        RR=val;
        break;
      case 't':
        val=LOW_CUT_OFF;          //Set low cut-off ex.20
        FL=val;
        FR=val;
        RL=val;
        RR=val;
        break;
	  case 'v':
        if(FL<=25)		          //Give Speed boost of +10 when speed<=25
		{
          FL=FL+10;
          FR=FR+10;
          RL=RL+10;
          RR=RR+10;
		}
        break;
      case 'i':
        FL=FL+limit;            //Increase Altitude by limit
        FR=FR+limit;
        RL=RL+limit;
        RR=RR+limit;
        break;
      case 'k':
        FL=FL-limit;            //Decrease Altitude by limit
        FR=FR-limit;
        RL=RL-limit;
        RR=RR-limit;
        break;
      case 'j':
        FR=FR+limit;		//Increase Right diagonal rotors by limit
        RL=RL+limit;
        break;
      case 'l':
        FL=FL+limit;		//Increase Left diagonal rotors by limit
        RR=RR+limit;
        break;
      case 'p':
        limit=limit+1;          //Increase limit value by 1
        break;
      case 'o':
        limit=limit-1;          //Decrease limit value by 1
        break;
	  case 'q':
		FL=FL+limit;				//Manual Front Left Rotor control
		break;
      case 'w':
		FR=FR+limit;				//Manual Front Right Rotor control
        break;
      case 's':
		RR=RR+limit;				//Manual Rear Right Rotor control
        break;
      case 'a':
		RL=RL+limit;				//Manual Rear Left Rotor control
        break;
      case ' ':								
        if(ledOnState)
          digitalWrite(13, LOW);			//Toggle LED
        else
          digitalWrite(13, HIGH);
		ledOnState=!ledOnState;
        break;
      default:								//Safe State during connection lost
        if(FL>0 && FR>0 && RL>0 && RR>0)
        {
          Front_Left_arm.write(FL=FL-1);
          Rear_Left_arm.write(RL=RL-1);
          Rear_Right_arm.write(RR=RR-1);
          Front_Right_arm.write(FR=FR-1);
		  delay(200);
        }
		else
		  break;
      }
	  
	  //Print the corresponding rotor speed on PC Serial Monitor
      Serial.print("FL: ");
      Serial.print(FL);
      Serial.print(" ");
      Serial.print("FR: ");
      Serial.print(FR);
      Serial.print(" ");
      Serial.print("RL: ");
      Serial.print(RL);
      Serial.print(" ");
      Serial.print("RR: ");
      Serial.println(RR);
	  
      //Sending Signals
      Front_Left_arm.write(FL);
      Rear_Left_arm.write(RL);
      Rear_Right_arm.write(RR);
      Front_Right_arm.write(FR);
    }
  }
}

void setupBlueToothConnection()
{
  blueToothSerial.begin(38400);                        //Bluetooth Baud Rate
  blueToothSerial.print("\r\n+STWMOD=0\r\n");          //Slave Mode for Copter
  //blueToothSerial.print("\r\n+STNA=SeeedBTSlave\r\n"); //Device Name for Bluetooth: "SeeedBTSlave"
  blueToothSerial.print("\r\n+STNA=Quadcopter\r\n");
  blueToothSerial.print("\r\n+STOAUT=1\r\n");          //Allow Connection
  blueToothSerial.print("\r\n+STAUTO=0\r\n");          //No Auto Connect
  delay(2000);                                         //Some delay...Just to be safe..
  blueToothSerial.print("\r\n+INQ=1\r\n");             //Allow inquiry to device
  Serial.println("The slave bluetooth is inquirable!");
  delay(2000);                                         //Another delay...Again just to be safe
  blueToothSerial.flush();                             //Kick out all unwanted values
}
