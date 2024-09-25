#include "DFRobot_ADXL345.h"

//#define cs_pin 10

DFRobot_ADXL345_I2C ADXL345(&Wire,0x53);
/**
 * Note that the ICSP pin need to be connected when using leonardo as the main controller.
 */
// DFRobot_ADXL345_SPI ADXL345(&SPI, cs_pin);

int accval[3];

int x, y, z;                        //three axis acceleration data
float roll = 0.00, pitch = 0.00;       //Roll & Pitch are the angles which rotate by the axis X and y

void RP_calculate(){
  double x_Buff = float(x);
  double y_Buff = float(y);
  double z_Buff = float(z);
  roll = atan2(y_Buff , z_Buff) * 57.3;
  pitch = atan2((- x_Buff) , sqrt(y_Buff * y_Buff + z_Buff * z_Buff)) * 57.3;
}

void setup() 
{
  Serial.begin(9600);
  ADXL345.begin();
  ADXL345.powerOn();
}

void loop() 
{
  ADXL345.readAccel(accval);
  Serial.print("Triaxial acceleration value is :");
  Serial.print("x:");Serial.print(accval[0]);
  Serial.print(" y:");Serial.print(accval[1]);
  Serial.print(" z:");Serial.println(accval[2]);
  double x_Buff = float(accval[0]);
  double y_Buff = float(accval[1]);
  double z_Buff = float(accval[2]);
  roll = atan2(y_Buff , z_Buff) * 57.3;
  pitch = atan2((- x_Buff) , sqrt(y_Buff * y_Buff + z_Buff * z_Buff)) * 57.3;
  Serial.print("Roll:"); Serial.println(roll);
  Serial.print("Pitch:"); Serial.println(pitch);
  delay(500);
}
