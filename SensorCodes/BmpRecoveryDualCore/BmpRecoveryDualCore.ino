#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>

Adafruit_BMP280 bmp;
TaskHandle_t task_loop1;

int buzzer=13,led=15,drogue_pin=12;
const int n=15;
float pressure,curr_altitude=0,last_altitude,temperature,avg=0,apogee,down,up;
bool flag_drogue=false;

void esploop1(void* pvParameters){
  setup1();
  for(;;){
    loop1();
  }
}

void setup() {
  xTaskCreatePinnedToCore(esploop1,"loop1",10000,NULL,1,&task_loop1,!ARDUINO_RUNNING_CORE);  
  pinMode(buzzer,OUTPUT);
  pinMode(led,OUTPUT);

  digitalWrite(buzzer,LOW);
  digitalWrite(led,LOW);

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

void setup1(){
  pinMode(drogue_pin,OUTPUT);
  digitalWrite(drogue_pin,LOW);
}

void loop() {
  getBmpData();
  delay(50);  
}

void loop1(){
  if(!flag_drogue){
    drogue_func();
  }
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
    down=curr_altitude;
    up=last_altitude;
    for(int i=0;i<n;i++){
      avg=avg+(down-up);
      up=down;
      delay(200);
      down=curr_altitude;
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