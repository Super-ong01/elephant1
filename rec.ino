#include <LoRa.h>
#include <SPI.h>

#define ss 5
#define rst 15
#define dio0 2
#define timeSecond 180
String localAddress = "A";
byte destination = 0xAB;
String Local_ID;
int led1 = 12;
int led2 = 33;
int led3 = 26;
int sw_top = 14;
int sw_buttom = 32;//bottom
int sw_run = 22;
int vibratio_motor_1 = 4;
int vibratio_motor_2 = 13;
byte msgCount = 0; 
unsigned long now = millis();
unsigned long lasttrigger = now;
bool isLightOn = false;
bool runnn = false;

bool boxopen = false;
bool boxclose = false;
bool boxshake = false;
bool boxstop = false;
bool newDataReceived = false;
boolean starttimer = true;
boolean btincoming = false;
void setup() {
  Serial.begin(115200);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(vibratio_motor_1, OUTPUT);
  pinMode(vibratio_motor_2, OUTPUT);
  pinMode(sw_top, INPUT_PULLUP);
  pinMode(sw_buttom, INPUT_PULLUP);
  pinMode(sw_run, INPUT_PULLUP);
  while (!Serial);
  Serial.println("LoRa Sender");

  LoRa.setPins(ss, rst, dio0);    // Setup LoRa transceiver module

  while (!LoRa.begin(433E6)) {    // Initialize LoRa at 433 MHz
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xAB);
  Serial.println("LoRa Initializing OK!");
}

void loop() {
  String msg = "cam1";
  String message;// = "Hi. I'm LoRa_1";
  msgCount++;
  if (Serial.available()) {
    String data = Serial.readString();
    String message = "@" + localAddress + "/" + data; // Append local ID to the message
    sendMessage(message);
    Serial.println("Sending message: " + message);
  }

  if (LoRa.parsePacket()) {
    while (LoRa.available()) {
      btincoming = true;//ไม่เเน่ใจ
      String LoRaData = LoRa.readString();
      int pos1 = LoRaData.indexOf('@');
      int pos2 = LoRaData.indexOf('/');
      Local_ID = LoRaData.substring(pos1 + 1, pos2);

      if (Local_ID == "C") {
        Serial.println("LoRa ID 'C'");
        int pos3 = LoRaData.indexOf('&');
        int pos4 = LoRaData.indexOf('$');
        int pos5 = LoRaData.indexOf('#');

        String LoRa_A_str = LoRaData.substring(pos2 + 1, LoRaData.length());
        Serial.print("Data Lora_A onReceive : "); Serial.println(LoRa_A_str);
        Serial.println();
        if (LoRa_A_str == "X") {
          message =  "@" +
                     String(localAddress) + "/" +
                     String(LoRa_A_str);
          sendMessage(message);
          Serial.println("Sending " + message); Serial.println();
          digitalWrite(led1, HIGH);
        } else if (LoRa_A_str == "Y") {
          message = "@" +
                    String(localAddress) + "/" +
                    String(LoRa_A_str);
          sendMessage(message);
          Serial.println("Sending " + message); Serial.println();
          digitalWrite(led2, HIGH);
          Serial.println("control_box");
          if (runnn == false) {
            runnn = true;
            boxcontrol();
          }
        }
        //newDataReceived = false;//ไม่เเน่ใจ
      }
      else if (Local_ID == "B") {
        //      Serial.println("LoRa ID 'B'");
        int pos3 = LoRaData.indexOf('&');
        int pos4 = LoRaData.indexOf('$');
        int pos5 = LoRaData.indexOf('#');

        String LoRa_B_str = LoRaData.substring(pos2 + 1, LoRaData.length());
        Serial.print("Data Lora_B onReceive : "); Serial.println(LoRa_B_str);
        if (LoRa_B_str == "manual control&box_OPEN") {
          Serial.println("box OPEN");
          boxopen = true;

        } else if (LoRa_B_str == "manual control&box_CLOSE") {
          Serial.println("box CLOSE");
          boxclose = true;

        } else if (LoRa_B_str == "manual control&box_SHAKE") {
          Serial.println("box SHAKE");
          boxshake = true;

        } else if (LoRa_B_str == "manual control&box_STOP") {
          Serial.println("box STOP");
          boxstop = true;


        }

      } else {
        Serial.println("error Local_ID onReceive not use");
        Serial.println();
      }

    }
    if (starttimer == true && (now - lasttrigger > (timeSecond * 1000))) {
      if (message == "") {
        Serial.println("Bluetooth Lost");
        starttimer = false;
        message =  "@" +
                   String(localAddress) + "/" +
                   "Bluetooth Lost";
        sendMessage(message);
        Serial.println("Sending " + message); Serial.println();
        digitalWrite(led1, LOW);
        ESP.restart();
      }

    }
    else {
      if ((message != "") && (btincoming == true)) {
        Serial.print("message incoming = "); Serial.println(message);
        starttimer = true;
        lasttrigger = millis();
        message = "";
        btincoming = false;
      }
    }
  }
  if (boxopen == true) {
    digitalWrite(led1, HIGH);
    analogWrite(25, 0); //up
    analogWrite(27, 255);
    Serial.println("up");
    while (digitalRead(sw_top) == HIGH) {
      Serial.println("wait up");
    }
    analogWrite(25, 255); //stop
    analogWrite(27, 255);
    Serial.println("stop");
    digitalWrite(led1, LOW);
    boxopen = false;
  } else if (boxclose == true) {
    digitalWrite(led2, HIGH);
    analogWrite(25, 255); //down
    analogWrite(27, 0);
    while (digitalRead(sw_buttom) == LOW) {
      Serial.println("wait down");
    }
    analogWrite(25, 255); //stop
    analogWrite(27, 255);
    Serial.println("stop");
    digitalWrite(led2, LOW);
    boxclose = false;
  } else if (boxshake == true) {
    digitalWrite(led3, HIGH);
    digitalWrite(vibratio_motor_1, HIGH);
    digitalWrite(vibratio_motor_2, LOW);
    boxshake = false;
  } else if (boxstop == true) {
    digitalWrite(led3, LOW);
    digitalWrite(vibratio_motor_1, LOW);
    digitalWrite(vibratio_motor_2, LOW);
    boxstop = false;
  }
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();
  LoRa.print(outgoing);
  LoRa.endPacket();
}
void boxcontrol() {

  Serial.println("incoming_box");
  // Serial.print(sw3);Serial.print(sw2);Serial.println(sw1);
  while (runnn == true) {
    digitalWrite(led1, HIGH);
    digitalWrite(led3, HIGH);
    analogWrite(25, 0); //up
    analogWrite(27, 255);
    digitalWrite(vibratio_motor_1, HIGH);
    digitalWrite(vibratio_motor_2, LOW);
    Serial.println("up");
    while (digitalRead(sw_top) == HIGH) {
      Serial.println("wait up");
    }
    analogWrite(25, 255); //stop
    analogWrite(27, 255);
    digitalWrite(led1, LOW);
    digitalWrite(led2, HIGH);
    Serial.println("stop");
    delay(10000);
    analogWrite(25, 255); //down
    analogWrite(27, 0);
    while (digitalRead(sw_buttom) == LOW) {
      Serial.println("wait down");
    }
    analogWrite(25, 255); //stop
    analogWrite(27, 255);
    digitalWrite(vibratio_motor_1, LOW);
    digitalWrite(vibratio_motor_2, LOW);
    Serial.println("stop");
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    runnn = false;
    Serial.println("finish");
  }
}
