#include <SPI.h> // include libraries
#include <LoRa.h>
#include <HardwareSerial.h>
HardwareSerial SerialPort(2); // use UART2

String mas;
const int nssPin = 5;          // LoRa radio chip select
const int rstPin = 15;       // LoRa radio reset
const int dio0Pin = 2;         // change for your board; must be a hardware interrupt pin
unsigned long previousMillis = 0; // เก็บค่าเวลาก่อนหน้า
const long interval = 10000; // 3 นาทีในหน่วยมิลลิวินาที
String outgoing;              // outgoing message
byte msgCount = 0;            // count of outgoing messages
//byte localAddress = 0xA;     // address of this device
String localAddress = "C";
byte destination = 0xAB;      // destination to send to
long lastSendTime = 0;        // last send time

String Local_ID;

//int interval = 10000;          // interval between sends

void setup() {
  Serial.begin(115200);                   // initialize serial
  SerialPort.begin(9600, SERIAL_8N1, 16, 17);
  while (!Serial);

  Serial.println("LoRa Duplex with callback Test2");

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(nssPin, rstPin, dio0Pin);// set CS, reset, IRQ pin

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa_2 init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  LoRa.setSyncWord(0xAB);
  LoRa.onReceive(onReceive);
  LoRa.receive();
  Serial.println("LoRa_2 init succeeded.");
}

void loop() {
//  if (millis() - lastSendTime > interval) {
//    String msg = "Hi. I'm LoRa_2";
//    String message;                       // send a message
//    msgCount++;                           // increment message ID
//    message = String(msgCount) + "@" +
//              String(localAddress) + "/" +
//              String(msg); //+ "&" +
////              String(Humid) + "$" +
////              String(light) + "#" +
////              String(batteryLevel);
//    sendMessage(message);
//    Serial.println("Sending " + message);Serial.println();
//    lastSendTime = millis();            // timestamp the message
//    interval = random(9000) + 1000;     // 2-3 seconds
////    LoRa.receive();                     // go back into receive mode
//  }

unsigned long currentMillis = millis(); // ดึงค่าเวลาปัจจุบัน
  if (currentMillis - previousMillis >= interval) { // เช็คว่าผ่านไปเวลา 3 นาทีหรือไม่
    previousMillis = currentMillis; // อัปเดตค่าเวลาก่อนหน้าเป็นเวลาปัจจุบัน

    mas = "X"; // กำหนดค่า "X" ให้กับตัวแปร mas
    String message = "@" + localAddress + "/" + mas;
    Serial.print("Sending message: ");
    Serial.println(message);
    sendMessage(message); // ส่งข้อมูลไปยัง LoRa
  }
if (SerialPort.available()) {
    char number = SerialPort.read();
    if (number == '1') {
      mas = "Y";
    }
    else if (number == '0') {
      mas = "N";
      }
      else{
      mas = "";  
      }
    String message = "@" + localAddress + "/" + mas;
    sendMessage(message); // ส่งข้อมูลไปยัง LoRa
    
    Serial.print("Received message: ");
    Serial.println(message);
  }
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();
  LoRa.print(outgoing);
  LoRa.endPacket();
  LoRa.receive();
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  Serial.print("Received packet '");
  // read packet
  while (LoRa.available()) {
    String LoRaData = LoRa.readString();
    // LoRaData format: readingID/temperature&humidity$light#batterylevel
    // String example: 1/27.43&654$555#95.34
    Serial.print(LoRaData);
    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.print(LoRa.packetRssi());
    Serial.print(", Snr: ");
    Serial.println(LoRa.packetSnr());

    int pos1 = LoRaData.indexOf('@');
    int pos2 = LoRaData.indexOf('/');
//     Serial.print("pos1= ");Serial.println(LoRaData.indexOf('@'));
//    Serial.print("pos2= ");Serial.println(LoRaData.indexOf('/'));
    Local_ID = LoRaData.substring(pos1 + 1, pos2);

    if (Local_ID == "A") {
      Serial.println("LoRa ID 'A'");
      int pos3 = LoRaData.indexOf('&');
      int pos4 = LoRaData.indexOf('$');
      int pos5 = LoRaData.indexOf('#');
      
      String LoRa_A1_str = LoRaData.substring(pos2 + 1, LoRaData.length());
      Serial.print("Data Lora_A1 onReceive : "); Serial.println(LoRa_A1_str);
      Serial.println();
    }
    else {
      Serial.println("error Local_ID onReceive not use");
      Serial.println();
    }
  }
}
