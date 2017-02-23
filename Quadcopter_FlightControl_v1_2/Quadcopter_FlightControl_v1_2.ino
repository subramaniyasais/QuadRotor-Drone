/* This is a re-written program of ESC-TEST2.ino file */
/* version 1.2 */
/* Author : Subramaniya Sai S */
/* ******** NOT-TESTED - WORKING CODE ******** */

 #include <Servo.h>

Servo Front_Left_arm;        //6
Servo Rear_Left_arm;         //9
Servo Rear_Right_arm;        //10
Servo Front_Right_arm;       //11

String incomingString;

void setup()
{
  Front_Left_arm.attach(6);
  Rear_Left_arm.attach(9);
  Rear_Right_arm.attach(10);
  Front_Right_arm.attach(11);
  Serial.begin(9600);
  Serial.println("initializing");
  incomingString="";
}


void loop()
{
   if(Serial.available() > 0)
   {
      int val = Serial.read();
//      if(ch==10)
      {
//        Serial.println();
//        Serial.print("String input:");
//        Serial.println(incomingString);
        
//        int val=incomingString.toInt();
        Serial.print("Integer input:");
        Serial.println(val);
        
        if (val > -1 && val < 181)
        {
          Serial.println("Value is between 0 and 180");
          Front_Left_arm.write(val);
          Rear_Left_arm.write(val);
          Rear_Right_arm.write(val);
          Front_Right_arm.write(val);       
        }
        else
        {
          Serial.println("ERROR: Value is NOT between 0 and 180");
        }
        incomingString="";
      }
//      incomingString+=ch;
   }
}
