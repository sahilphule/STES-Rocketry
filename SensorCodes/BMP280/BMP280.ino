#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>

Adafruit_BMP280 bmp;

float pressure,altitude,temperature;

void setup() {
  Serial.begin(9600);

  if(bmp.begin(0x76)){
    Serial.println("BMP initialised successfully!");
    Serial.println();
  }
  else{
    Serial.println("Error initialising BMP!");
    while(1){

    }
  }

}

void loop() {

  getBmpData();
  delay(1000);
  
}

void getBmpData(){

  pressure=bmp.readPressure();
  altitude=bmp.readAltitude()*3.28084;
  temperature=bmp.readTemperature();

  Serial.print("Pressure : ");
  Serial.print(pressure);
  Serial.println(" Pa");
  
  Serial.print("Altitude : ");
  Serial.print(altitude);
  Serial.println(" ft");

  Serial.print("Temperature : ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.println();
}