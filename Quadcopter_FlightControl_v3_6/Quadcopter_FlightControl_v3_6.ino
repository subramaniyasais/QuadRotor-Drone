/* Bluetooth integration with Full flight Control With Ultrasonic sensor */
/* Control via PC(Teraterm) and Phone(Blueterm or ArduinoRC) */
/* Author : Subramaniya Sai S */
/* ******** UN TESTED - NOT WORKING CODE ******** */

/* MANUAL */
/* W - Ascend
   S - Descend
   M - Move Forward
   N - Move Backward
   A - Move Left
   D - Move Right
   H - High Brake
   L - Low Brake
   O - Decrease limit by 1
   P - Increase Limit by 1
*/

#include <SoftwareSerial.h>   //Software Serial Port
#include <Servo.h>

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
  Serial.begin(9600);
  pinMode(ECHOPIN,INPUT);
  pinMode(TRIGPIN,OUTPUT);
  setupBlueToothConnection();
} 

void loop() 
{ 
  int FL=0,FR=0,RL=0,RR=0;
  int val=0, limit=1, breakSpeed=5;
  char ch;
  while(1)
  {
  	
	
    if(blueToothSerial.available())
    {
      ch = blueToothSerial.read();
      blueToothSerial.print(ch);
      switch(ch)
      {
        case 'h':
          val=HIGH_CUT_OFF;         //Set high cut-off ex.100
          FL=val;
          FR=val;
          RL=val;
          RR=val;
          break;
        case 'l':
          val=LOW_CUT_OFF;          //Set low cut-off ex.20
          FL=val;
          FR=val;
          RL=val;
          RR=val;
          break;
        case 'w':
          FL=FL+limit;            //Increase Altitude by limit
          FR=FR+limit;
          RL=RL+limit;
          RR=RR+limit;
          break;
        case 's':
          FL=FL-limit;            //Decrease Altitude by limit
          FR=FR-limit;
          RL=RL-limit;
          RR=RR-limit;
          break;
        case 'p':
          limit=limit+1;          //Increase limit value by 1
          break;
        case 'o':
          limit=limit-1;          //Decrease limit value by 1
          break;
        case 'm':
          RL=RL+limit;            //Move Forward
          RR=RR+limit;            //Increase Rear Pair by limit
          break;
        case 'n':
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
        default:
          Serial.println("ERROR: Invalid Entry!");  
      }
    }
    else
    {
      //Sending Signals
      Front_Left_arm.write(FL);
      Rear_Left_arm.write(RL);
      Rear_Right_arm.write(RR);
      Front_Right_arm.write(FR);
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
    
    
	
	if(distance<150)			//Collision Avoidance
	{
                blueToothSerial.print(distance);
		FL=FL+breakSpeed;    	        //Move Backward - Avoiding Collision
                FR=FR+breakSpeed;        	    //Increase Front Pair by breaking speed
		Front_Left_arm.write(FL);
		Front_Right_arm.write(FR);
		delay(2000);
		RL=RL+breakSpeed;            	//Move Forward - Stop Back motion
                RR=RR+breakSpeed;            	//Increase Rear Pair by breaking speed
		Rear_Left_arm.write(RL);
		Rear_Right_arm.write(RR);
		delay(1000);
		FL=FL-breakSpeed;
		FR=FR-breakSpeed;
		RL=RL-breakSpeed;		//Stabilizing the Altitude
		RR=RR-breakSpeed;		//Decrease Altitude by breaking speed
		//Sending Signals
		Front_Left_arm.write(FL);
		Rear_Left_arm.write(RL);
		Rear_Right_arm.write(RR);
		Front_Right_arm.write(FR);
	}
    }
  }
} 

void setupBlueToothConnection()
{
  blueToothSerial.begin(38400);                        //Bluetooth Baud Rate
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

