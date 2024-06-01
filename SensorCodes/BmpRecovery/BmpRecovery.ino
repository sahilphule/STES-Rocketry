#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>

Adafruit_BMP280 bmp;

int buzzer=13,led=15,drogue_pin=12;
const int n=15;
float pressure,curr_altitude=0,last_altitude,temperature,avg=0,apogee;
bool flag_drogue=false;

void setup() {
  
  pinMode(buzzer,OUTPUT);
  pinMode(led,OUTPUT);
  pinMode(drogue_pin,OUTPUT);

  digitalWrite(buzzer,LOW);
  digitalWrite(led,LOW);
  digitalWrite(drogue_pin,LOW);

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

  if(!flag_drogue){
    drogue_func();
  }
  delay(100);
  
}

void getBmpData(){

  last_altitude=curr_altitude;

  pressure=bmp.readPressure();
  curr_altitude=bmp.readAltitude()*3.28084;
  temperature=bmp.readTemperature();

  Serial.print("Pressure : ");
  Serial.print(pressure);
  Serial.println(" Pa");
  
  Serial.print("Altitude : ");
  Serial.print(curr_altitude);
  Serial.println(" ft");

  Serial.print("Temperature : ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.println();
}

void drogue_func(){

  if(curr_altitude<last_altitude){
    apogee=last_altitude;

    for(int i=0;i<n;i++){
      avg=avg+(curr_altitude-last_altitude);
      getBmpData();
      delay(200);
    }
    avg=avg/n;
    if(int(avg)<0){
      digitalWrite(buzzer,HIGH);
      digitalWrite(led,HIGH);
      digitalWrite(drogue_pin,HIGH);
      delay(10000);
      digitalWrite(drogue_pin,LOW);
      digitalWrite(led,LOW);
      digitalWrite(buzzer,LOW);
      flag_drogue=true;
    }
    else{
      avg=0;
    }
  }
}