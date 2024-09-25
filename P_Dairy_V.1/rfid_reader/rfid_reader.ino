#define RXD2 16
#define TXD2 17
#define buzzer 23
#define led 15
char buffer[70];
int count = 0;
int decimalNumber = 0,power=0;
// Storing hexa number digits in one array
char hexDigits[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

void clearBufferArray()  // function to clear buffer array
{
  digitalWrite(buzzer,HIGH);
  digitalWrite(led,HIGH);
  delay(100);
  digitalWrite(buzzer,LOW);
  digitalWrite(led,LOW);
  for (int i = count-4 ; i > 2; i--) {
    Serial.print(buffer[i]);
    for(int j=0;j<16;j++)
    {
      if (buffer[i] == hexDigits[j]){
        decimalNumber += j * pow(16,power);
      }
    }
    power++;
  }
  //Serial.print(count);
  Serial.println("");
  power=0;
  Serial.println(decimalNumber);
  
  decimalNumber=0;
  // clear all index of array with command NULL
  for (int i = 0; i < count; i++) {
    buffer[i] = NULL;
  }
}
void setup() {
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(115200);
  //Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial Txd is on pin: " + String(TX));
  Serial.println("Serial Rxd is on pin: " + String(RX));
  pinMode(buzzer,OUTPUT);
  pinMode(led,OUTPUT);
}

void loop() {  //Choose Serial1 or Serial2 as required
  if (Serial2.available()) {
    buffer[count++] = Serial2.read();
    while ((int)buffer[count-1] != 3) {
    //while (Serial2.available()){
      //Serial.print(char(Serial2.read()));

      while (!Serial2.available()) delay(1);
      buffer[count++] = Serial2.read();  // writing data into array
      if (count == 64) break;

      //Serial.println(count);
      //Serial.println((int)buffer[count-1]);
    }
    clearBufferArray();  // call clearBufferArray function to clear the stored data from the array
    count = 0;
    
  }
}
