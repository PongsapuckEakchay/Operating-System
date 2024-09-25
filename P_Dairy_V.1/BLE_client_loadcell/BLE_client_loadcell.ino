#include "BLEDevice.h"
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


//BLE Server name (the other ESP32 name running the server sketch)
#define bleServerName "BME280_ESP32"

/* UUID's of the service, characteristic that we want to read*/
// BLE Service
static BLEUUID bmeServiceUUID("91bad492-b950-4226-aa2b-4ede9fa42f59");

// Humidity Characteristic
static BLEUUID humidityCharacteristicUUID("ca73b3ba-39f6-4ab3-91ae-186dc9577d99");

//Flags stating if should begin connecting and if the connection is up
static boolean doConnect = false;
static boolean connected = false;

//Address of the peripheral device. Address will be found during scanning...
static BLEAddress *pServerAddress;
 
//Characteristicd that we want to read
//static BLERemoteCharacteristic* temperatureCharacteristic;
static BLERemoteCharacteristic* humidityCharacteristic;

//Activate notify
const uint8_t notificationOn[] = {0x1, 0x0};
const uint8_t notificationOff[] = {0x0, 0x0};

//Variables to store temperature and humidity
char* temperatureChar;
char* humidityChar;

//Flags to check whether new temperature and humidity readings are available
boolean newTemperature = false;
boolean newHumidity = false;

//Connect to the BLE Server that has the name, Service, and Characteristics
bool connectToServer(BLEAddress pAddress) {
   BLEClient* pClient = BLEDevice::createClient();
 
  // Connect to the remove BLE Server.
  pClient->connect(pAddress);
  Serial.println(" - Connected to server");
 
  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(bmeServiceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(bmeServiceUUID.toString().c_str());
    return (false);
  }
 
  // Obtain a reference to the characteristics in the service of the remote BLE server.
  humidityCharacteristic = pRemoteService->getCharacteristic(humidityCharacteristicUUID);

  if (humidityCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID");
    return false;
  }
  Serial.println(" - Found our characteristics");
 
  //Assign callback functions for the Characteristics
  humidityCharacteristic->registerForNotify(humidityNotifyCallback);
  return true;
}

//Callback function that gets called, when another device's advertisement has been received
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.getName() == bleServerName) { //Check if the name of the advertiser matches
      advertisedDevice.getScan()->stop(); //Scan can be stopped, we found what we are looking for
      pServerAddress = new BLEAddress(advertisedDevice.getAddress()); //Address of advertiser is the one we need
      doConnect = true; //Set indicator, stating that we are ready to connect
      Serial.println("Device found. Connecting!");
    }
  }
};

//When the BLE Server sends a new humidity reading with the notify property
static void humidityNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  //store humidity value
  humidityChar = (char*)pData;
  newHumidity = true;
  Serial.println(newHumidity);
}


void setup() {
  //Start serial communication
  Wire.begin();
  Serial.begin(115200);
  ADXL345.begin();
  ADXL345.powerOn();
  Serial.println("\nI2C Scanner");
  _tare_1=tare(FX29_1);
  _tare_2=tare(FX29_2);
  _tare_3=tare(FX29_3);
  _tare_4=tare(FX29_4);
  //Serial.print("Grams1\tGrams\tGrams3\tGram4\tTotal\troll\tpitch\n");

  Serial.println("Starting Arduino BLE Client application...");
  pinMode(23,OUTPUT);
  //Init BLE device
  BLEDevice::init("");
 
  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 30 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(30);
}

void loop() {

  ADXL345.readAccel(accval);
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
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer(*pServerAddress)) {
      Serial.println("We are now connected to the BLE Server.");
      //Activate the Notify property of each Characteristic
      //temperatureCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
      humidityCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
      connected = true;
    } else {
      Serial.println("We have failed to connect to the server; Restart your device to scan for nearby BLE server again.");
    }
    doConnect = false;
  }
  //if new temperature readings are available, print in the OLED
  if (newHumidity){
    //newTemperature = false;
    newHumidity = false;
    digitalWrite(23,HIGH);
    delay(100);
    digitalWrite(23,LOW);
    Serial.println(humidityChar);
    //printReadings();
  }
  delay(1000); // Delay a second between loops.
}