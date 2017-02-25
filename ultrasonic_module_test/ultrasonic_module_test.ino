/* This is a progam to test the Ultrasonic module */
/* Author : Foreign  */
/* Edited : Subramaniya Sai S */
/* ******** TESTED - WORKING CODE ******** */


#define ECHOPIN 3
#define TRIGPIN 2


void setup()
{
  Serial.begin(9600);
  pinMode(ECHOPIN,INPUT);
  pinMode(TRIGPIN,OUTPUT);
}

void loop()
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
  Serial.print(distance);
  Serial.println(" cm");
  
  delay(200);
  
}
