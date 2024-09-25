/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-many-to-one-esp32/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <esp_now.h>
#include <WiFi.h>
#include <HardwareSerial.h>

#define led_red 42
#define switch_1 2

HardwareSerial SerialPort(1);

uint8_t broadcastAddress1[] = { 0x68, 0xB6, 0xB3, 0x37, 0xF5, 0x80 };
uint8_t broadcastAddress2[] = { 0x30, 0xAE, 0xA4, 0x97, 0x93, 0x98 };
// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int id;
  int temperature;
  int humidity;
  int uv;
  int ir;
  int vl;
  int b1;
  int b2;
} struct_message;

typedef struct struct_message1 {
  int id;  // must be unique for each sender board
  int dust;
  int LPG;
  int Co;
  int Smoke;
} struct_message1;

typedef struct struct_message2 {
  int id;  // must be unique for each sender board
  int watt;
} struct_message2;

typedef struct struct_message3 {
  int id;  // must be unique for each sender board
  int temperature;
  int humidity;
  int uv;
  int ir;
  int vl;
} struct_message3;

typedef struct struct_command {
  int relay1;
  int relay2;
  int relay3;
  int relay4;
  int relay5;
  int relay6;
  int relay7;
  int relay8;

} struct_command;

typedef struct struct_command2 {  
  int powerOn;
  int temp;
  int fan;
} struct_command2;

// Create a struct_message called DataBoard1
struct_message myData;
struct_message1 DataBoard1;
struct_message2 DataBoard2;
struct_message3 DataBoard3;

struct_command myCommand_light;
struct_command2 myCommand_AC;
// Create a structure to hold the readings from each board




esp_now_peer_info_t peerInfo;


// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}


// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  //Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  //Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
 // Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  if (myData.id == 1) {

    memcpy(&DataBoard1, incomingData, sizeof(DataBoard1));
    // Serial.printf("Board ID %u: %u bytes\n", DataBoard1.id, len);
    // Serial.printf("Dust : %d\n", DataBoard1.dust);
    // Serial.printf("LPG : %d \n", DataBoard1.LPG);
    // Serial.printf("Co value: %d \n", DataBoard1.Co);
    // Serial.printf("Smoke value: %d \n", DataBoard1.Smoke);
    String data="D"+(String)(DataBoard1.dust)+",L"+(String)(DataBoard1.LPG) +",C"+(String)(DataBoard1.Co) +",S"+(String)(DataBoard1.Smoke) +",*";
    SerialPort.print(data);
    // Serial.println();


  } else if (myData.id == 2) {

    memcpy(&DataBoard2, incomingData, sizeof(DataBoard2));
    // Serial.printf("Board ID %u: %u bytes\n", DataBoard2.id, len);
    // Serial.printf("Watt value: %d \n", DataBoard2.watt);
    String data="W"+(String)(DataBoard2.watt)+",*";
    SerialPort.print(data);
    // Serial.println();

  } else if (myData.id == 3) {
    memcpy(&DataBoard3, incomingData, sizeof(DataBoard3));
    // Serial.printf("Board ID %u: %u bytes\n", DataBoard3.id, len);
    // Serial.printf("Temperature value: %d \n", DataBoard3.temperature);
    // Serial.printf("humidity value: %d \n", DataBoard3.humidity);
    // Serial.printf("UV value: %d \n", DataBoard3.uv);
    // Serial.printf("Infarate value: %d \n", DataBoard3.ir);
    // Serial.printf("Visible light value: %d \n", DataBoard3.vl);
    String data="T"+(String)(DataBoard3.temperature)+",H"+(String)(DataBoard3.humidity) +",U"+(String)(DataBoard3.uv) +",I"+(String)(DataBoard3.ir) + ",V"+(String)(DataBoard3.vl)+",*";
    SerialPort.print(data);
    // Serial.println();
  }
}

int stoi(String x, String y) {

  String str = x;
  String subStr = y;
  int index = str.indexOf(subStr);
  if (index == -1) return index;
  return (int)x[index + 1 + y.length()]-48;
}

void setup() {
  //Initialize Serial Monitor
  Serial.begin(115200);
  pinMode(led_red, OUTPUT);
  pinMode(switch_1, INPUT);

  SerialPort.begin(9600, SERIAL_8N1, 18, 17);  //hardware Serial
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  peerInfo.channel = 0;
  peerInfo.encrypt = false;


  // Add peer
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Acess the variables for each board
  /*int board1X = boardsStruct[0].x;
  int board1Y = boardsStruct[0].y;
  int board2X = boardsStruct[1].x;
  int board2Y = boardsStruct[1].y;
  int board3X = boardsStruct[2].x;
  int board3Y = boardsStruct[2].y;*/


  //Serial.println(myCommand.light1);
  //esp_err_t result = esp_now_send(broadcastAddress1, (uint8_t *)&myCommand_light, sizeof(myCommand));
  //if (result == ESP_OK) {
  //  Serial.println("Sent with success__1");
  //} else {
  //  Serial.println("Error sending the data");
  //}

  if (SerialPort.available()) {
    char data[100] = " ";
    char temp = SerialPort.read();
    int i = 0;
    while (temp != (char)'*') {
      data[i] = temp;
      i++;
      while (!SerialPort.available()) delay(1);
      temp = SerialPort.read();
      if (temp == '*') break;
    }
    String Data1 = String(data);
    Serial.println(Data1);

    myCommand_light.relay1 = stoi(Data1, "light1");
    myCommand_light.relay2 = stoi(Data1, "light2");
    myCommand_light.relay3 = stoi(Data1, "light3");
    myCommand_light.relay4 = stoi(Data1, "light4");
    myCommand_light.relay5 = stoi(Data1, "light5");
    myCommand_light.relay6 = stoi(Data1, "light6");
    myCommand_light.relay7 = stoi(Data1, "light7");
    myCommand_light.relay8 = stoi(Data1, "light8");

    myCommand_AC.powerOn = stoi(Data1,"ac");    
    myCommand_AC.temp = stoi(Data1,"temp")+20;   
    myCommand_AC.fan = stoi(Data1,"acfan");
    Serial.println("---------ESP_NOW--------");
    Serial.print("relay1 : "); Serial.print(myCommand_light.relay1);
    Serial.print("  relay2 : "); Serial.print(myCommand_light.relay2);
    Serial.print("  relay3 : "); Serial.print(myCommand_light.relay3);
    Serial.print("  relay4 : "); Serial.print(myCommand_light.relay4);
    Serial.print("  relay5 : "); Serial.print(myCommand_light.relay5);
    Serial.print("  relay6 : "); Serial.print(myCommand_light.relay6);
    Serial.print("  relay7 : "); Serial.print(myCommand_light.relay7);
    Serial.print("  relay8 : "); Serial.println(myCommand_light.relay8);
    Serial.print("AC : "); Serial.print(myCommand_AC.powerOn);
    Serial.print("Temp : "); Serial.print(myCommand_AC.temp);
    Serial.print("Fan mode : "); Serial.println(myCommand_AC.fan);
    Serial.println("------------------------");





    esp_err_t result = esp_now_send(broadcastAddress2, (uint8_t *)&myCommand_light, sizeof(myCommand_light));
    esp_err_t result2 = esp_now_send(broadcastAddress1, (uint8_t *)&myCommand_AC, sizeof(myCommand_AC));
    
    if (result == ESP_OK) {
      Serial.println("Sent light with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    if (result2 == ESP_OK) {
      Serial.println("Sent AC with success");
    }
    else {
      Serial.println("Error sending the data");
    }
  }



  delay(1000);
}