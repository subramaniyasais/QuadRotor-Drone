/* This is a re-written program of Quadcopter_FlightControl_v1.0 file */
/* version 1.0.1 */
/* Author : Subramaniya Sai S */
/* ******** TESTED - WORKING CODE ******** */

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
}


void loop()
{
   if(Serial.available() > 0)
   {
      char ch = Serial.read();
  
      if (ch != 10)
      {
        Serial.print("I have received: ");
        Serial.print(ch, DEC);
        Serial.print('\n');
        incomingString += ch;
      }
      else
      {
        int val = incomingString.toInt();
        Serial.println("Printing the value: ");
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
        incomingString = "";
      }
   }
}
