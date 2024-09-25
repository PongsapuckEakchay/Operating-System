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

#define sensor_gas 17
#define led_red 42

// REPLACE WITH THE RECEIVER'S MAC Address
uint8_t broadcastAddress[] = {0x68, 0xB6, 0xB3, 0x37, 0xF2, 0x74};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
    int id; // must be unique for each sender board
    int gas;
    int x;
    int y;
} struct_message;

typedef struct struct_command {
  int light1;
}struct_command;

// Create a struct_message called myData
struct_message myData;

struct_command myCommand;
// Create peer interface
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 

void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myCommand, incomingData, sizeof(myCommand));
  
  
  
  Serial.printf("light status: %d \n", myCommand.light1);
  Serial.println();
  if(myCommand.light1==1){
    digitalWrite(led_red,HIGH);
  }
  else{
    digitalWrite(led_red,LOW);
  }
}
  
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);


  pinMode(led_red,OUTPUT);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
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
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }


  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  // Set values to send
  myData.id = 1;
  myData.x = random(0,50);
  myData.y = random(0,50);

  int val_gas = analogRead(sensor_gas);
  Serial.println(val_gas);
  if(val_gas > 1800){
    myData.gas = 1;
  }
  else {
    myData.gas = 0;
  }

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }

  
  delay(1000);
}