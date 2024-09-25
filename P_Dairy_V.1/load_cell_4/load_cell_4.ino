#include <Wire.h>
#include "DFRobot_ADXL345.h"

typedef unsigned char   u08;
typedef unsigned short  u16;
typedef unsigned long   u32;
uint8_t FX29_1 = 0x28;
uint8_t FX29_2 = 0x36;
uint8_t FX29_3 = 0x46;
uint8_t FX29_4 = 0x48;
DFRobot_ADXL345_I2C ADXL345(&Wire,0x53);

int accval[3];
int x, y, z;                        //three axis acceleration data
float roll = 0.00, pitch = 0.00;       //Roll & Pitch are the angles which rotate by the axis X and y

int _range = 25;
uint16_t _tare_1 = 0 ,_tare_2=0 ,_tare_3=0 ,_tare_4=0;

double getRawData(uint8_t device){
  u16 data;
  uint8_t a=device;
  uint8_t nB=2;
  switch (Wire.requestFrom(a, nB) )//returns nB Bytes if successful
      {   case 2://OK: Big-Endian data, Horner-algoritme, 14-bit
              data = ( Wire.read()<<8 | Wire.read() ) & 0x3fff;
              break;
          default://error, value -1 is the largest unsigned value
              data = (u16)-1;
              break;
      }
  double data_d = data;
  return data_d;
}
uint16_t tare(uint8_t device){
  uint32_t sum = 0;
  uint32_t _tare = 0;
  for (uint8_t i = 0; i < 10; i++) {
    sum += getRawData(device);
    delay(10);
  }
  _tare = sum / 10;
  return _tare;
}
float getGrams(uint8_t device,uint16_t _tare) {
  double bridgeData = getRawData(device);
  uint32_t net = 0;
  int8_t sign = 1;
  if (bridgeData >= _tare) {
    net = bridgeData - _tare;
    sign = 1;
  }
  else {
    net = _tare - bridgeData;
    sign = 0;
  }
  //Serial.print("raw data -> ");
  //Serial.println(bridgeData);
  
  return ((net * _range / 14000.0) * sign)* 453.592;
}

void setup() {
  Wire.begin();
  Serial.begin(115200);
  ADXL345.begin();
  ADXL345.powerOn();
  Serial.println("\nI2C Scanner");
  _tare_1=tare(FX29_1);
  _tare_2=tare(FX29_2);
  _tare_3=tare(FX29_3);
  _tare_4=tare(FX29_4);
  Serial.print("Grams1\tGrams\tGrams3\tGram4\tTotal\troll\tpitch\n");
}

void loop() {
  ADXL345.readAccel(accval);
  // Serial.print("Triaxial acceleration value is :");
  // Serial.print("x:");Serial.print(accval[0]);
  // Serial.print(" y:");Serial.print(accval[1]);
  // Serial.print(" z:");Serial.println(accval[2]);
  double x_Buff = float(accval[0]);
  double y_Buff = float(accval[1]);
  double z_Buff = float(accval[2]);
  roll = atan2(y_Buff , z_Buff) * 57.3;
  pitch = atan2((- x_Buff) , sqrt(y_Buff * y_Buff + z_Buff * z_Buff)) * 57.3;
  // Serial.print("Roll:"); Serial.println(roll);
  // Serial.print("Pitch:"); Serial.println(pitch);


  float g1= getGrams(FX29_1,_tare_1);
  float g2= getGrams(FX29_2,_tare_2);
  float g3= getGrams(FX29_3,_tare_3);
  float g4= getGrams(FX29_4,_tare_4);
  
  float total=g1+g2+g3+g4;
 // Serial.print("Grams1\tGrams\tGrams3\tGram4\tTotal\troll\tpitch\n");
  Serial.print(g1);  Serial.print("--");
  Serial.print(g2);  Serial.print("--");
  Serial.print(g3);  Serial.print("--");
  Serial.print(g4);  Serial.print("--");
  Serial.print(total);  Serial.print("--");
  Serial.print(total*cos(roll));  Serial.print("--");
  Serial.print(total*cos(pitch));  Serial.print("--");
  Serial.println(roll); Serial.print("--");
  Serial.println(pitch);  Serial.print("\n");
  delay(500);
}
