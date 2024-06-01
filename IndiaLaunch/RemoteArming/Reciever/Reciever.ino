#include <SPI.h>
#include <LoRa.h>
 
const int csPin = 5;     // LoRa radio chip select
const int resetPin = 14;  // LoRa radio reset
const int irqPin = 2;    // Must be a hardware interrupt pin
String x;
int ignite=22;
 
void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
 
  pinMode(ignite,OUTPUT);
  LoRa.setPins(csPin, resetPin, irqPin);
  Serial.println("LoRa Receiver Test");
 
  // Start LoRa module at local frequency
  // 433E6 for Asia
  // 866E6 for Europe
  // 915E6 for North America
 
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
}
 
void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available()) {
      x+=(char)LoRa.read();
    }
    Serial.println(x);  

    if(x=="Launch"){
      digitalWrite(ignite,HIGH);
      delay(10000);
      digitalWrite(ignite,LOW);
      x="";
    }
  }
}