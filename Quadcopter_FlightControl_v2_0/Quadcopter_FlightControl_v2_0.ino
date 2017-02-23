/* Bluetooth integration */
/* Control via PC(Teraterm) and Phone(Blueterm) */
/* Author : Subramaniya Sai S */
/* ******** TESTED - WORKING CODE ******** */

#include <SoftwareSerial.h>   //Software Serial Port
#include <Servo.h>

Servo Front_Left_arm;        //5
Servo Rear_Left_arm;         //9
Servo Rear_Right_arm;        //10
Servo Front_Right_arm;       //11

#define RxD 6
#define TxD 7

#define HighCutOff 100
#define LowCutOff 20

#define DEBUG_ENABLED  1

SoftwareSerial blueToothSerial(RxD,TxD);

void setup() 
{ 
  Front_Left_arm.attach(5);
  Rear_Left_arm.attach(9);
  Rear_Right_arm.attach(10);
  Front_Right_arm.attach(11);
  Serial.begin(9600);
  setupBlueToothConnection();
} 

void loop() 
{ 
  int val=0;
  char ch;
  while(1){
    if(blueToothSerial.available()){
      ch = blueToothSerial.read();
      blueToothSerial.print(ch);
      if(ch=='h')
      {
        val=HighCutOff;    //Set high cut-off ex.100
      }
      else if(ch=='l')
      {
        val=LowCutOff;    //Set low cut-off ex.20
      }
      else if(ch=='w')
      {
        val=val+1;         //Increase Altitude by 1
      }
      else if(ch=='s')
      {
        val=val-1;         //Decrease Altitude by 1
      }
      else
      {
        Serial.println("ERROR: Invalid Entry!");
      }

      //Sending Signals
      Front_Left_arm.write(val);
      Rear_Left_arm.write(val);
      Rear_Right_arm.write(val);
      Front_Right_arm.write(val);
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

