/* This is a program to test the working of bluetooth shield */
/* pin 13 (LED) responds to the user input */
/* Author: Subramaniya Sai Sekar */
#include <SoftwareSerial.h>   //Software Serial Port
#define RxD 6
#define TxD 7

#define DEBUG_ENABLED  1

SoftwareSerial blueToothSerial(RxD,TxD);

void setup()
{
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  setupBlueToothConnection();
}

void loop()
{
  char recvChar;
  while(1)
  {
    if(blueToothSerial.available())
    {
      recvChar = blueToothSerial.read();
      Serial.print(recvChar);
      if(recvChar=='o')
        digitalWrite(13, HIGH);
      else if(recvChar=='f')
        digitalWrite(13, LOW);
    }
  }
}

void setupBlueToothConnection()
{
  blueToothSerial.begin(38400);
  blueToothSerial.print("\r\n+STWMOD=0\r\n");
  blueToothSerial.print("\r\n+STNA=SeeedBTSlave\r\n");
  blueToothSerial.print("\r\n+STOAUT=1\r\n");
  blueToothSerial.print("\r\n+STAUTO=0\r\n");
  delay(2000);
  blueToothSerial.print("\r\n+INQ=1\r\n");
  Serial.println("The slave bluetooth is inquirable!");
  delay(2000);
  blueToothSerial.flush();
}
