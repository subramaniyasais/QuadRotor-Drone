/* Advanced Control Bluetooth integration with Full flight Control With Ultrasonic sensor & Gyroscope */
/* Control via PC(Teraterm) and Phone(Blueterm or ArduinoRC) */

/* ************ MANUAL ************ */
/* 		I - Ascend
 		K - Descend
 		J - Turn Left
 		L - Turn Right
 		W - Move Forward
 		S - Move Backward
 		A - Move Left
 		D - Move Right
 		5 - High Brake
 		T - Low Brake
 		O - Decrease limit by 1
 		P - Increase Limit by 1
 space - Emergency Light
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
int led = 13,on=0;

Servo Front_Left_arm;        //Pin 5
Servo Rear_Left_arm;         //Pin 9
Servo Rear_Right_arm;        //Pin 10
Servo Front_Right_arm;       //Pin 11

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
  iniX = ax/100;
  iniY = ay/100;


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
        val=HIGH_CUT_OFF;       //Set high cut-off ex.100
        FL=val;
        FR=val;
        RL=val;
        RR=val;
        break;
      case 't':
        val=LOW_CUT_OFF;        //Set low cut-off ex.20
        FL=val;
        FR=val;
        RL=val;
        RR=val;
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
      case 'w':
        RL=RL+limit;            //Move Forward
        RR=RR+limit;            //Increase Rear Pair by limit
        break;
      case 's':
        FL=FL+limit;            //Move Backward
        FR=FR+limit;            //Increase Front Pair by limit
        break;
      case 'a':
        FR=FR+limit;            //Move Left
        RR=RR+limit;            //Increase Right Pair by limit
        break;
      case 'd':
        FL=FL+limit;            //Move Right
        RL=RL+limit;            //Increae Left Pair by limit
        break;
      case ' ':
        if(on==1)
        {
          digitalWrite(13, LOW);
          on=0;
        }
        else
        {
          digitalWrite(13, HIGH);
          on=1;
        }
        break;
      default:
        if(FL>0 && FR>0 && RL>0 && RR>0)
        {
          Front_Left_arm.write(FL=FL-1);
          Rear_Left_arm.write(RL=RL-1);
          Rear_Right_arm.write(RR=RR-1);
          Front_Right_arm.write(FR=FR-1);
        }
      }
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
    else
    {
      //Start Ranging - Generating a trigger of 10us burst
      digitalWrite(TRIGPIN,LOW);
      delayMicroseconds(2);
      digitalWrite(TRIGPIN,HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIGPIN,LOW);

      //Distance Calculation
      float distance = pulseIn(ECHOPIN,HIGH);
      distance= distance/58;
      //blueToothSerial.print(distance);
      //blueToothSerial.println(" cm");
      delay(200);

      if(distance<50)		//Collision Avoidance
      {
        FL=FL+breakSpeed;	//Move Backward - Avoiding Collision
        FR=FR+breakSpeed;	//Increase Front Pair by breaking speed
        Front_Left_arm.write(FL);
        Front_Right_arm.write(FR);
        delay(2000);
        RL=RL+breakSpeed;	//Move Forward - Stop Back motion
        RR=RR+breakSpeed;	//Increase Rear Pair by breaking speed
        Rear_Left_arm.write(RL);
        Rear_Right_arm.write(RR);
        delay(1000);
        FL=FL-breakSpeed;
        FR=FR-breakSpeed;
        RL=RL-breakSpeed;	//Stabilizing the Altitude
        RR=RR-breakSpeed;	//Decrease Altitude by breaking speed

          //Sending Signals
        Front_Left_arm.write(FL);
        Rear_Left_arm.write(RL);
        Rear_Right_arm.write(RR);
        Front_Right_arm.write(FR);
        delay(1000);
      }

      accelgyro.getMotion6(&ay, &ax, &az, &gx, &gy, &gz);
      if(((ax/100-iniX)<(-20)) || ((ax/100-iniX)>(20)) || ((ay/100-iniY)<(-20)) || ((ay/100-iniY)>(20)))
        AutoGyroStabilize();	//Auto Stabilize if in No command reception
      //      {
      //        Serial.print("X: ");
      //        Serial.print(ax/100-iniX);
      //        Serial.print(" ");
      //        Serial.print("Y: ");
      //        Serial.println(ay/100-iniY);
      //      }
    }
  }
}


void AutoGyroStabilize()
{
  int iFL,iFR,iRL,iRR;
  accelgyro.getMotion6(&ay, &ax, &az, &gx, &gy, &gz);
  while(((ax/100-iniX)<(-20)) || ((ax/100-iniX)>(20)) || ((ay/100-iniY)<(-20)) || ((ay/100-iniY)>(20)))
  {

    // GYRO STABILIZATION TO FORWARD MOVEMENT - NEGATIVE PITCH
    accelgyro.getMotion6(&ay, &ax, &az, &gx, &gy, &gz);
    if(((ax/100-iniX)>(-150)) && ((ax/100-iniX)<(-20)))
    {
      iFL=FL-(ax/100-iniX+15);		// Increases Front speed according to the gyro value -X
      iFR=FR-(ax/100-iniX+15);		// ex. motor speed is 60, gyro reads -22, 60-(-22+15)=60-(-7)=67.
      iRL=RL+(ax/100-iniX+15);		// Decreases Rear speed according to the gyro value -X
      iRR=RR+(ax/100-iniX+15);		// ex. motor speed is 60, gyro reads -22, 60+(-22+15)=60+(-7)=53.
      if(iRL>LOW_CUT_OFF && iRR>LOW_CUT_OFF && iFL>LOW_CUT_OFF && iFR>LOW_CUT_OFF)
      {
        Front_Left_arm.write(iFL);
        Front_Right_arm.write(iFR);
        Rear_Left_arm.write(iRL);
        Rear_Right_arm.write(iRR);

        Serial.print("iFL: ");
        Serial.print(iFL);
        Serial.print(" ");
        Serial.print("iFR: ");
        Serial.print(iFR);
        Serial.print(" ");
        Serial.print("iRL: ");
        Serial.print(iRL);
        Serial.print(" ");
        Serial.print("iRR: ");
        Serial.println(iRR);
        delay(500);
        Front_Left_arm.write(FL);
        Front_Right_arm.write(FR);
        Rear_Left_arm.write(RL);
        Rear_Right_arm.write(RR);
        delay(500);

      }
    }

    // GYRO STABILIZATION TO LEFTWARD MOVEMENT - POSITIVE YAWN
    accelgyro.getMotion6(&ay, &ax, &az, &gx, &gy, &gz);
    if(((ay/100-iniY)>(20)) && ((ay/100-iniY)<(150)))
    {
      iFL=FL+(ay/100-iniY)-15;		// Increases Left speed according to the gyro value +Y
      iRL=RL+(ay/100-iniY)-15;		// ex. motor speed is 60, gyro reads +22, 60+(+22-15)=60+(+7)=67.
      iFR=FR-(ay/100-iniY)-15+30;	// Decreases Right speed according to the gyro value +Y
      iRR=RR-(ay/100-iniY)-15+30;	// ex. motor speed is 60, gyro reads +22, 60-(+22-15)=60-(+7)=53.
      if(iRL>LOW_CUT_OFF && iRR>LOW_CUT_OFF && iFL>LOW_CUT_OFF && iFR>LOW_CUT_OFF)
      {
        Front_Left_arm.write(iFL);
        Front_Right_arm.write(iFR);
        Rear_Left_arm.write(iRL);
        Rear_Right_arm.write(iRR);
        Serial.print("iFL: ");
        Serial.print(iFL);
        Serial.print(" ");
        Serial.print("iFR: ");
        Serial.print(iFR);
        Serial.print(" ");
        Serial.print("iRL: ");
        Serial.print(iRL);
        Serial.print(" ");
        Serial.print("iRR: ");
        Serial.print(iRR);

        delay(500);
        Front_Left_arm.write(FL);
        Front_Right_arm.write(FR);
        Rear_Left_arm.write(RL);
        Rear_Right_arm.write(RR);
        delay(500);
      }
    }


    // GYRO STABILIZATION TO BACKWARD MOVEMENT - POSITIVE PITCH
    accelgyro.getMotion6(&ay, &ax, &az, &gx, &gy, &gz);
    if(((ax/100-iniX)>(20)) && ((ax/100-iniX)<(150)))
    {
      iRL=RL+(ax/100-iniX-15);		// Increases Rear speed according to the gyro value +X
      iRR=RR+(ax/100-iniX-15);		// ex. motor speed is 60, gyro reads +22, 60+(+22-15)=60+(+7)=67.
      iFL=FL-(ax/100-iniX-15);		// Decreases Front speed according to the gyro value +X
      iFR=FR-(ax/100-iniX-15);		// ex. motor speed is 60, gyro reads +22, 60-(+22-15)=60-(+7)=53.
      if(iRL>LOW_CUT_OFF && iRR>LOW_CUT_OFF && iFL>LOW_CUT_OFF && iFR>LOW_CUT_OFF)
      {
        Front_Left_arm.write(iFL);
        Front_Right_arm.write(iFR);
        Rear_Left_arm.write(iRL);
        Rear_Right_arm.write(iRR);
        Serial.print("iFL: ");
        Serial.print(iFL);
        Serial.print(" ");
        Serial.print("iFR: ");
        Serial.print(iFR);
        Serial.print(" ");
        Serial.print("iRL: ");
        Serial.print(iRL);
        Serial.print(" ");
        Serial.print("iRR: ");
        Serial.println(iRR);

        delay(500);
        Front_Left_arm.write(FL);
        Front_Right_arm.write(FR);
        Rear_Left_arm.write(RL);
        Rear_Right_arm.write(RR);
        delay(500);
      }

    }

    // GYRO STABILIZATION TO RIGHTWARD MOVEMENT - NEGATIVE YAWN
    accelgyro.getMotion6(&ay, &ax, &az, &gx, &gy, &gz);
    if(((ay/100-iniY)>(-150)) && ((ay/100-iniY)<(-20)))
    {
      iFR=FR-(ay/100-iniY)+15-30;	// Increases Right speed according to the gyro value -Y
      iRR=RR-(ay/100-iniY)+15-30;	// ex. motor speed is 60, gyro reads -22, 60-(-22+15)=60-(-7)=67.
      iFL=FL+(ay/100-iniY)+15;		// Decreases Left speed according to the gyro value -Y
      iRL=RL+(ay/100-iniY)+15;		// ex. motor speed is 60, gyro reads -22, 60+(-22+15)=60+(-7)=53.
      if(iRL>LOW_CUT_OFF && iRR>LOW_CUT_OFF && iFL>LOW_CUT_OFF && iFR>LOW_CUT_OFF)
      {
        Front_Left_arm.write(iFL);
        Front_Right_arm.write(iFR);
        Rear_Left_arm.write(iRL);
        Rear_Right_arm.write(iRR);
        Serial.print("iFL: ");
        Serial.print(iFL);
        Serial.print(" ");
        Serial.print("iFR: ");
        Serial.print(iFR);
        Serial.print(" ");
        Serial.print("iRL: ");
        Serial.print(iRL);
        Serial.print(" ");
        Serial.print("iRR: ");
        Serial.println(iRR);

        delay(500);
        Front_Left_arm.write(FL);
        Front_Right_arm.write(FR);
        Rear_Left_arm.write(RL);
        Rear_Right_arm.write(RR);
        delay(500);
      }

    }

    //    accelgyro.getMotion6(&ay, &ax, &az, &gx, &gy, &gz);
  }
}

void setupBlueToothConnection()
{
  blueToothSerial.begin(38400);                        //Bluetooth Baud Rate
  blueToothSerial.print("\r\n+STWMOD=0\r\n");          //Slave Mode for Copter
  //blueToothSerial.print("\r\n+STNA=SeeedBTSlave\r\n"); //Device Name for Bluetooth: "Quadcopter"
  blueToothSerial.print("\r\n+STNA=Quadcopter\r\n");
  blueToothSerial.print("\r\n+STOAUT=1\r\n");          //Allow Connection
  blueToothSerial.print("\r\n+STAUTO=0\r\n");          //No Auto Connect
  delay(2000);                                         //Some delay...Just to be safe..
  blueToothSerial.print("\r\n+INQ=1\r\n");             //Allow inquiry to device
  Serial.println("The slave bluetooth is inquirable!");
  delay(2000);                                         //Another delay...Again just to be safe
  blueToothSerial.flush();                             //Kick out all unwanted values
}





