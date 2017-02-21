#include <Servo.h>

#define MOTOR_PIN1 5
#define MOTOR_PIN2 9
#define MOTOR_PIN3 10
#define MOTOR_PIN4 11

Servo motor1;
Servo motor2;
Servo motor3;
Servo motor4;

int i;

void setup()
{
  Serial.begin(9600);
  Serial.println("Program begin...");

  motor1.attach(MOTOR_PIN1);
  motor2.attach(MOTOR_PIN2);
  motor3.attach(MOTOR_PIN3);
  motor4.attach(MOTOR_PIN4);

  i=0;
}

void loop()
{
  for(i=15;i<=38;i++)
  {
    Serial.println(i);

    motor1.write(i);
    motor2.write(i);
    motor3.write(i);
    motor4.write(i);

    delay(200);
  }
}

