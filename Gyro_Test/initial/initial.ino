#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;

bool blinkState = false;

void setup()
{
  Wire.begin();
  Serial.begin(38400);

  accelgyro.initialize();
}

void loop()
{
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  Serial.print("X: ");
  Serial.print(ay/100);
  Serial.print("\tY: ");
  Serial.print(ax/100);
  Serial.println("\t");
  blinkState = !blinkState;

  delay(1000);
}


