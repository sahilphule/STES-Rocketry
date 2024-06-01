#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <LoRa.h>
 
WebServer server(80);

const int csPin = 5;     // LoRa radio chip select
const int resetPin = 14;  // LoRa radio reset
const int irqPin = 2;    // Must be a hardware interrupt pin
const char *apSSID = "esp32";
const char *apPassword = "12345678";
bool launch = false;
byte msgCount = 0;


void launchRocket(){
  Serial.println("Launching Rocket!!!");
  LoRa.beginPacket();
  LoRa.print("Launch");
  LoRa.endPacket();
}
 
void setupAP(){
  WiFi.softAP(apSSID, apPassword);
  IPAddress myIP = WiFi.softAPIP();
  Serial.println();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}
 
void handleLaunch(){
  launch = true;

  DynamicJsonDocument response(1024);
  response["message"] = "Launching in 10 seconds!";
 
  String jsonResponse;
  serializeJson(response, jsonResponse);
 
  server.send(200, "application/json", jsonResponse);
}
 
void setup(){
  Serial.begin(115200);
  setupAP();
  server.on("/launch", HTTP_GET, handleLaunch);
  server.begin();
  Serial.println("HTTP server started");

  LoRa.setPins(csPin, resetPin, irqPin);
  Serial.println("LoRa Sender Test");
 
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
 
void loop(){
  server.handleClient();
  if (launch == true){
    launchRocket();
    launch = false;
  }
 
  delay(200);
}