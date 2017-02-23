/* Advanced Control Bluetooth integration with Full flight Control With Ultrasonic sensor & Gyroscope */
/* Control via PC(Teraterm) and Phone(Blueterm or ArduinoRC) */
/* ***** Demonstration Stability Control ***** */
/* *** Algorithm I *** */
/* Author : Subramaniya Sai S */
/* ******** TESTED - WORKING CODE ******** */

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
int altitudeControl;

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
  delay(1000);
  //manual();
/*  blueToothSerial.println("I - Ascend");
  delay(200);
  blueToothSerial.println("K - Descend");
  delay(200);
  blueToothSerial.println("J - Turn Left");
  delay(200);
  blueToothSerial.println("L - Turn Right");
  delay(200);
  blueToothSerial.println("W - Move Forward");
  delay(200);
  blueToothSerial.println("S - Move Backward");
  delay(200);
  blueToothSerial.println("A - Move Left");
  delay(200);
  blueToothSerial.println("D - Move Right");
  delay(200);
  blueToothSerial.println("5 - High Brake");
  delay(200);
  blueToothSerial.println("T - Low Brake");
  delay(200);
  blueToothSerial.println("O - Decrease limit by 1");
  delay(200);
  blueToothSerial.println("P - Increase Limit by 1");
  delay(200);
 */
  accelgyro.getMotion6(&ay, &ax, &az, &gx, &gy, &gz);
  iniX=ax;
  iniY=ay;
  
  
} 

void loop() 
{ 
  FL=0;FR=0;RL=0;RR=0;
  int val=0, limit=1, breakSpeed=5;
  altitudeControl=0;
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
	  FR=FR+limit;		  //Increase Right diagonal rotors by limit
          RL=RL+limit;
	  break;
	case 'l':
	  FL=FL+limit;		  //Increase Left diagonal rotors by limit
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
      }

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
	
      if(distance<50)					//Collision Avoidance
      {
		FL=FL+breakSpeed;				//Move Backward - Avoiding Collision
		FR=FR+breakSpeed;				//Increase Front Pair by breaking speed
		Front_Left_arm.write(FL);
		Front_Right_arm.write(FR);
		delay(2000);
		RL=RL+breakSpeed;				//Move Forward - Stop Back motion
		RR=RR+breakSpeed;				//Increase Rear Pair by breaking speed
		Rear_Left_arm.write(RL);
		Rear_Right_arm.write(RR);
		delay(1000);
		FL=FL-breakSpeed;
		FR=FR-breakSpeed;
		RL=RL-breakSpeed;				//Stabilizing the Altitude
		RR=RR-breakSpeed;				//Decrease Altitude by breaking speed
		//Sending Signals
		Front_Left_arm.write(FL);
		Rear_Left_arm.write(RL);
		Rear_Right_arm.write(RR);
		Front_Right_arm.write(FR);
      }
      delay(1000);
      if(((ax-iniX)<(-20)) || ((ax-iniX)>(20)) || ((ay-iniY)<(-20)) || ((ay-iniY)>(20)))
		AutoGyroStabilize();		//Auto Stabilize if in No command reception
    }
  }
} 


void AutoGyroStabilize()
{

// GYRO STABILIZATION TO FORWARD MOVEMENT - NEGATIVE PITCH
	accelgyro.getMotion6(&ay, &ax, &az, &gx, &gy, &gz);
	if(((ax-iniX)>(-50)) && ((ax-iniX)<(-10)))
	{
		FL=FL+1;
		FR=FR+1;
		Front_Left_arm.write(FL);
		Front_Right_arm.write(FR);
		altitudeControl+=1;
		delay(1000);
	}
	else if(((ax-iniX)>(-51)) && ((ax-iniX)<(-160)))
	{
		FL=FL+10;
		FR=FR+10;
		Front_Left_arm.write(FL);
		Front_Right_arm.write(FR);
		altitudeControl+=1;
		delay(1000);
	}
	
// GYRO STABILIZATION TO BACKWARD MOVEMENT - POSITIVE PITCH
	accelgyro.getMotion6(&ay, &ax, &az, &gx, &gy, &gz);
	if(((ax-iniX)>(10)) && ((ax-iniX)<(50)))
	{
		RL=RL+1;
		RR=RR+1;
		Rear_Left_arm.write(RL);
		Rear_Right_arm.write(RR);
		altitudeControl+=1;
		delay(1000);
	}
	else if(((ax-iniX)>(51)) && ((ax-iniX)<(160)))
	{
		RL=RL+10;
		RR=RR+10;
		Rear_Left_arm.write(RL);
		Rear_Right_arm.write(RR);
		altitudeControl+=1;
		delay(1000);
	}

// GYRO STABILIZATION TO RIGHTWARD MOVEMENT - NEGATIVE YAWN
	accelgyro.getMotion6(&ay, &ax, &az, &gx, &gy, &gz);
	if(((ay-iniY)>(-50)) && ((ay-iniY)<(-10)))
	{
		FR=FR+1;
		RR=RR+1;
		Front_Right_arm.write(FR);
		Rear_Right_arm.write(RR);
		altitudeControl+=1;
		delay(1000);
	}
	else if(((ay-iniY)>(-51)) && ((ay-iniY)<(-160)))
	{
		FR=FR+10;
		RR=RR+10;
		Front_Right_arm.write(FR);
		Rear_Right_arm.write(RR);
		altitudeControl+=1;
		delay(1000);
	}
	
// GYRO STABILIZATION TO LEFTWARD MOVEMENT - POSITIVE YAWN
	accelgyro.getMotion6(&ay, &ax, &az, &gx, &gy, &gz);
	if(((ay-iniY)>(10)) && ((ay-iniY)<(50)))
	{
		FL=FL+1;
		RL=RL+1;
		Front_Left_arm.write(FL);
		Rear_Left_arm.write(RL);
		altitudeControl+=1;
		delay(1000);
	}
	else if(((ay-iniY)>(51)) && ((ay-iniY)<(160)))
	{
		FL=FL+10;
		RL=RL+10;
		Front_Left_arm.write(FL);
		Rear_Left_arm.write(RL);
		altitudeControl+=1;
		delay(1000);
	}

//Stabilize the altitude after changes
	if(altitudeControl>10)
	{
		int i=0;
		for(;i<7;i=i+1)
		{
			FL=FL-1;
			FR=FR-1;
			RL=RL-1;
			RR=RR-1;
			//Sending Signals
			Front_Left_arm.write(FL);
			Rear_Left_arm.write(RL);
			Rear_Right_arm.write(RR);
			Front_Right_arm.write(FR);
			delay(500);
		}
		altitudeControl=0;
	}
}

/*void manual()
{
	int i=0;
	char man[60] = "Manual:";
	Serial.println("Serial Manual Print");
	for(;i<7;i=i+1)
		{blueToothSerial.print(man[i]); delay(100);}
	delay(200);
	man="W-Move Forward    I-Ascend";
	for(i=0;i<26;i=i+1)
		{blueToothSerial.print(man[i]); delay(100);}
	delay(200);
	man="S-Move Backward   K-Descend";
	for(i=0;i<27;i=i+1)
		{blueToothSerial.print(man[i]); delay(100);}
	delay(200);
	man="A-Move Left     J-Turn Left";
	for(i=0;i<27;i=i+1)
		{blueToothSerial.print(man[i]); delay(100);}
	delay(200);
	man="D-Move Right    L-Turn Right";
	for(i=0;i<28;i=i+1)
		{blueToothSerial.print(man[i]); delay(100);}
	man="T-Low Brake    5-High Break";
	for(i=0;i<27;i=i+1)
		{blueToothSerial.print(man[i]); delay(100);}
	delay(200);
}
*/

void setupBlueToothConnection()
{
	blueToothSerial.begin(9600);                        //Bluetooth Baud Rate
	blueToothSerial.print("\r\n+STWMOD=0\r\n");          //Slave Mode for Copter
	blueToothSerial.print("\r\n+STNA=SeeedBTSlave\r\n"); //Device Name for Bluetooth: "SeeedBTSlave"
	blueToothSerial.print("\r\n+STOAUT=1\r\n");          //Allow Connection
	blueToothSerial.print("\r\n+STAUTO=0\r\n");          //No Auto Connect
	delay(2000);                                         //Some delay...Just to be safe..
	blueToothSerial.print("\r\n+INQ=1\r\n");             //Allow inquiry to device 
	Serial.println("The slave bluetooth is inquirable!");
	delay(2000);                                         //Another delay...Again just to be safe
	blueToothSerial.flush();                             //Kick out all unwanted values
}

