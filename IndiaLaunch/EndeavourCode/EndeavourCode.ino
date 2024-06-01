/*
Microcontroller - ESP Wroom 32

ESP Wroom 32
Vin = Battery +ve
Gnd = Battery -ve

BMP280
VCC = 3V3
GND = GND
SCL = GPIO22
SDA = GPIO21

MPU6050
VCC = 3V3
GND = GND
SCL = GPIO22
SDA = GPIO21

NEO GPS
VCC = 3V3
RX = GPIO2
TX = GPIO4
GND = GND

LORA
VCC = 3V3
RX = GPIO17
TX = GPIO16
Gnd = Gnd

SD Card Reader
CS = GPIO5
SCK = GPIO18
MOSI = GPIO23
MISO = GPIO19
VCC = 3V3
Gnd = Gnd

LED
Anode = GPIO15
Cathode = Gnd

Buzzer
Anode = GPIO13
Cathode = Gnd

TIP122 npn(Drouge)
Base = GPIO25
Collector = battery +ve
Emitter = Gnd

TIP122 npn(Main)
Base = GPIO12
Collector = battery +ve
Emitter = Gnd

*/

#include <SD.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>
#include <TinyGPSPlus.h>
#include <Wire.h>
#include <SoftwareSerial.h>

Adafruit_BMP280 bmp;
Adafruit_MPU6050 mpu;
TinyGPSPlus gps;
File myFile,myFile1;
TaskHandle_t task_loop1;
SoftwareSerial LORA(16,17);
SoftwareSerial GPS(4,2);

int buzzer=13,led=15,drogue_pin=25,main_pin=12,flag_drogue=0,flag_main=0;
const int CS=5,n=10;
String dataFile="/data.txt",logFile="/log.txt";
float pressure,curr_altitude=0,prev_altitude,temperature,acc_x,acc_y,acc_z,rot_x,rot_y,rot_z,latitude,longitude,avg=0,apogee,drogue_deployed,main_deployed,curr,prev,agl=0,main_altitude=1500;
bool flag_arm=false,flag_bmp=false,flag_mpu=false;


void esploop1(void* pvParameters){
  delay(15000);
  setup1();

  for(;;){
    loop1();
  }
}

void setup() {
  
  pinMode(drogue_pin,OUTPUT);
  pinMode(main_pin,OUTPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(led,OUTPUT);

  digitalWrite(drogue_pin,LOW);
  digitalWrite(main_pin,LOW);
  digitalWrite(buzzer,LOW);
  digitalWrite(led,LOW);

  xTaskCreatePinnedToCore(esploop1,"loop1",10000,NULL,1,&task_loop1,!ARDUINO_RUNNING_CORE);
  LORA.begin(115200);
  GPS.begin(9600);

  delay(3000);
  
  if(SD.begin(CS)){
    digitalWrite(buzzer,HIGH);
    digitalWrite(led,HIGH);
    delay(1000);
    digitalWrite(led,LOW);
    digitalWrite(buzzer,LOW);
  }
  
  // if(SD.exists(dataFile)){
  //   SD.remove(dataFile);
  // }

  delay(500);
  
  // if(SD.exists(logFile)){
  //   SD.remove(logFile);
  // }

  if(bmp.begin(0x76)){
    flag_bmp=true;  
    digitalWrite(buzzer,HIGH);
    digitalWrite(led,HIGH);  
    delay(1000);
    digitalWrite(led,LOW);
    digitalWrite(buzzer,LOW);
  }

  delay(500);

  if(mpu.begin(0x68)){
    flag_mpu=true;
    digitalWrite(buzzer,HIGH);
    digitalWrite(led,HIGH);
    delay(1000);
    digitalWrite(led,LOW);
    digitalWrite(buzzer,LOW);
  }

  myFile=SD.open(dataFile,FILE_APPEND);

  if(myFile){
    myFile.println();
    myFile.println();
    myFile.println();
    myFile.println();
    myFile.close();
  }

  myFile1=SD.open(logFile,FILE_APPEND);

  if(myFile1){
    myFile1.println();
    myFile1.println();
    myFile1.println();
    myFile1.println();
    myFile1.println("Pressure(Pa),Altitude(ft),Temperature(C),Acceleration_x(m/s2),Acceleration_y(m/s2),Acceleration_z(m/s2),Rotation_x(deg),Rotation_y(deg),Rotation_z(deg),Latitude(deg),Longitude(deg)");
    myFile1.println();

    if(flag_bmp){
      myFile1.println("BMP initialised successfully!");
      myFile1.println();
    }
    else{
      myFile1.println("Error initialising BMP!");
    }

    if(flag_mpu){
      myFile1.println("MPU initialised successfully!");
      myFile1.println();
    }
    else{
      myFile1.println("Error initialising MPU!");
    }
    myFile1.close();
  }
}

void setup1(){
  AGL();
  while(!flag_arm){
    Serial.println("In setup1");
    if(curr_altitude>(agl+100)){
      flag_arm=true;
    }
    delay(50);
  }
}

void loop() {
  getBmpData();
  getMpuData();
  getGpsData();

  dataLog();
  lora();

  delay(50);
}

void loop1(){
  if(!flag_main){
    myFile1=SD.open(logFile,FILE_APPEND);
    if(!flag_drogue){
      apogee_func();
    }
    else if(flag_drogue){
      if(curr_altitude<=(agl+main_altitude)){
        myFile1.println("Trigger 3 triggered!");
        myFile1.println("Deploying main chute!");
        main_func();
      }
    }
    myFile1.close();
  }
  delay(200);
}

void AGL(){
  myFile1=SD.open(logFile,FILE_APPEND);
  for(int i=0;i<10;i++){
    agl+=curr_altitude;
    myFile1.println(agl);
    delay(500);
  }
  agl=agl/10;

  myFile1.print("AGL : ");
  myFile1.println(agl);
  myFile1.close();
}

void apogee_func(){
  if(curr_altitude<prev_altitude){
    apogee=prev_altitude;
    myFile1.print("Trigger 1 triggered at : ");
    myFile1.println(apogee); //apogee and not prev_altitude as it keeps changing continuously
    myFile1.println("Might be error, verifying trigger 2: ");
    curr=curr_altitude;
    prev=prev_altitude;
    for(int i=0;i<n;i++){
      avg=avg+(curr-prev);
      prev=curr;
      delay(200);
      curr=curr_altitude;
    }
    avg=avg/n;
    if(int(avg)<0){
      myFile1.println("Trigger 2 triggered!");
      myFile1.print("Apogee : ");
      myFile1.println(apogee);
      myFile1.println("Deploying drogue chute!");
      drogue_func();
    }
    else{
      avg=0;
    }
    myFile1.println();
  }
}

void drogue_func(){
  digitalWrite(drogue_pin,HIGH);
  digitalWrite(buzzer,HIGH);
  digitalWrite(led,HIGH);
  drogue_deployed=curr_altitude;
  myFile1.print("Drogue deployed at : ");
  myFile1.println(drogue_deployed);
  delay(7000);
  digitalWrite(led,LOW);
  digitalWrite(buzzer,LOW);
  digitalWrite(drogue_pin,LOW);
  flag_drogue=1;
}

void main_func(){  
  digitalWrite(main_pin,HIGH);
  digitalWrite(buzzer,HIGH);
  digitalWrite(led,HIGH);
  main_deployed=curr_altitude;
  myFile1.print("Main deployed at : ");
  myFile1.println(main_deployed);
  delay(7000);
  digitalWrite(led,LOW);
  digitalWrite(buzzer,LOW);
  digitalWrite(main_pin,LOW);
  flag_main=1;
}

void getBmpData(){
  prev_altitude=curr_altitude;

  pressure=bmp.readPressure();
  curr_altitude=bmp.readAltitude()*3.28084;
  temperature=bmp.readTemperature();
}

void getMpuData(){
  sensors_event_t a,g,temp;
  mpu.getEvent(&a,&g,&temp);
  
  acc_x=a.acceleration.x * (-1);
  acc_y=a.acceleration.y;
  acc_z=a.acceleration.z;
  
  rot_x=g.gyro.x;
  rot_y=g.gyro.y;
  rot_z=g.gyro.z;
}

void getGpsData(){  
  while(GPS.available()>0){
    if(gps.encode(GPS.read())){
      if(gps.location.isValid()){
        latitude=gps.location.lat();
        longitude=gps.location.lng();
      }
    }
  } 
}

void dataLog(){
  myFile=SD.open(dataFile,FILE_APPEND);
  if(myFile){
    myFile.print(pressure);
    myFile.print(',');
    myFile.print(curr_altitude);
    myFile.print(',');
    myFile.print(temperature);
    myFile.print(',');
    myFile.print(acc_x);
    myFile.print(',');
    myFile.print(acc_y);
    myFile.print(',');
    myFile.print(acc_z);
    myFile.print(',');
    myFile.print(rot_x);
    myFile.print(',');
    myFile.print(rot_y);
    myFile.print(',');
    myFile.print(rot_z);
    myFile.print(',');
    myFile.print(latitude,6);
    myFile.print(',');
    myFile.println(longitude,6);

    myFile.close();
  }
}


void lora(){
  
  //String lora_values = String(latitude,6) +","+ String(longitude,6) +","+ String(flag_drogue) +","+ String(flag_main) +","+ String(curr_altitude);
  String lora_values = String(latitude,6) +","+ String(longitude,6) +","+ String(curr_altitude);

  String cmd = "AT+SEND=0,"+String(lora_values.length())+","+lora_values + "\r";
  LORA.println(cmd);
  
  while(LORA.available()){
    Serial.write(LORA.read());    
  }
}