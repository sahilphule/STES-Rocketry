#include <SD.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>
#include <Wire.h>
#include <TinyGPSPlus.h>

Adafruit_BMP280 bmp;
Adafruit_MPU6050 mpu;
TinyGPSPlus gps;
File myFile;

const int CS=5,n=15;
int buzzer=13,led=15,drogue_pin=12,main_pin=14,loopDelay=200;
String dataFile="/data.txt",logFile="/log.txt";
bool flag_drogue=false,flag_main=false;
float sec=0.00000,pressure,curr_altitude,temperature,last_altitude,latitude,longitude,acceleration_x,acceleration_y,acceleration_z,rotation_x,rotation_y,rotation_z,apogee,avg=0,drogue_altitude,main_altitude,location_altitude;

void setup() {

  pinMode(buzzer,OUTPUT);
  pinMode(led,OUTPUT);
  pinMode(drogue_pin,OUTPUT);
  pinMode(main_pin,OUTPUT);

  digitalWrite(buzzer,LOW);
  digitalWrite(led,LOW);
  digitalWrite(drogue_pin,LOW);
  digitalWrite(main_pin,LOW);

  Serial.begin(9600);
  Serial2.begin(9600,SERIAL_8N1,16,17);
  
  delay(5000);

  if(SD.begin(CS)){
    Serial.println("SD initialized successfully! \n");
    digitalWrite(buzzer,HIGH);
    digitalWrite(led,HIGH);
    delay(1000);
    digitalWrite(led,LOW);
    digitalWrite(buzzer,LOW);
  }
  else{
    Serial.println("Error initializing SD! \n");
    while(1){

    }
  }  

  if(SD.exists(dataFile)){
    Serial.println("Deleting the data file! \n");
    SD.remove(dataFile);
  }

  if(SD.exists(logFile)){
    Serial.println("Deleting the log file! \n");
    SD.remove(logFile);
  }

  delay(1000);
  
  myFile=SD.open(logFile,FILE_APPEND);
  
  if(myFile){
    
    Serial.println("Time(s),Pressure(Pa),Altitude(ft),Temperature(C),Latitude(deg),Longitude(deg),Acceleration_x(m/s2),Acceleration_y(m/s2),Acceleration_z(m/s2),Rotation_x(deg),Rotation_y(deg),Rotation_z(deg) \n");
    myFile.println("Time(s),Pressure(Pa),Altitude(ft),Temperature(C),Latitude(deg),Longitude(deg),Acceleration_x(m/s2),Acceleration_y(m/s2),Acceleration_z(m/s2),Rotation_x(deg),Rotation_y(deg),Rotation_z(deg) \n");
    
    if(bmp.begin(0x76)){
      Serial.println("BMP initialized successfully! \n");
      myFile.println("BMP initialized successfully! \n");
      
      digitalWrite(buzzer,HIGH);
      digitalWrite(led,HIGH);
      delay(1000);
      digitalWrite(led,LOW);
      digitalWrite(buzzer,LOW);
    }
    else{
      Serial.println("Error initializing BMP! \n");
      myFile.println("Error initializing BMP! \n");
      while(1){
      
      }
    }
    
    delay(1000);
    
    if(mpu.begin(0x68)){
      Serial.println("MPU initialized successfully! \n");
      myFile.println("MPU initialized successfully! \n");

      digitalWrite(buzzer,HIGH);
      digitalWrite(led,HIGH);
      delay(1000);
      digitalWrite(led,LOW);
      digitalWrite(buzzer,LOW);

      mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
      mpu.setGyroRange(MPU6050_RANGE_500_DEG);
      mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    }
    else{
      Serial.println("Error initializing MPU! \n");
      myFile.println("Error initializing MPU! \n");
      while(1){
        
      }
    }

    myFile.close();
  }
  

  else{
    Serial.println("Error opening file in setup! \n");
    while(1){

    }
  }    

}

void loop() {

  myFile=SD.open(dataFile,FILE_APPEND);
  
  if(myFile){
    digitalWrite(led,HIGH);
    getBmpData();
    getGpsData();
    getMpuData();
    myFile.close();
    digitalWrite(led,LOW);
    sec=sec+loopDelay;
  }
  else{
    Serial.println("Error opening data file! \n");
    while(1){
      
    }
  }

  if(!flag_drogue){
    drogue_func();
  }

  if(flag_drogue==true && flag_main==false){
    main_func();    
  }

  delay(loopDelay);
}


void getBmpData(){
  last_altitude=curr_altitude;
  pressure=bmp.readPressure();
  curr_altitude=bmp.readAltitude()*3.28084;
  temperature=bmp.readTemperature();

  Serial.print(sec);
  Serial.print(",");
  Serial.print(pressure);
  Serial.print(",");
  Serial.print(curr_altitude);
  Serial.print(",");
  Serial.print(temperature);
  Serial.print(",");

  myFile.print(sec);
  myFile.print(",");
  myFile.print(pressure);
  myFile.print(",");
  myFile.print(curr_altitude);
  myFile.print(",");
  myFile.print(temperature);
  myFile.print(",");
}


void getGpsData(){
  
  while(Serial2.available()>0){
    if(gps.encode(Serial2.read())){
      if(gps.location.isValid()){
        latitude=gps.location.lat();
        longitude=gps.location.lng();
      }
    }
  } 

  Serial.print(latitude,6);
  Serial.print(",");
  Serial.print(longitude,6);
  Serial.print(",");

  myFile.print(latitude,6);
  myFile.print(",");
  myFile.print(longitude,6);
  myFile.print(",");
}


void getMpuData(){
  
  sensors_event_t a,g,temp;
  mpu.getEvent(&a,&g,&temp);
  
  acceleration_x=a.acceleration.x;
  acceleration_y=a.acceleration.y;
  acceleration_z=a.acceleration.z;
  
  rotation_x=g.gyro.x;
  rotation_y=g.gyro.y;
  rotation_z=g.gyro.z;

  Serial.print(acceleration_x);
  Serial.print(",");
  Serial.print(acceleration_y);
  Serial.print(",");
  Serial.print(acceleration_z);
  Serial.print(",");
  Serial.print(rotation_x);
  Serial.print(",");
  Serial.print(rotation_y);
  Serial.print(",");
  Serial.println(rotation_z);
  
  myFile.print(acceleration_x);
  myFile.print(",");
  myFile.print(acceleration_y);
  myFile.print(",");
  myFile.print(acceleration_z);
  myFile.print(",");
  myFile.print(rotation_x);
  myFile.print(",");
  myFile.print(rotation_y);
  myFile.print(",");
  myFile.println(rotation_z);
}


void drogue_func(){  

  if(curr_altitude<last_altitude){
    apogee=last_altitude;
            
    for(int i=0;i<n;i++){
      avg=avg+(curr_altitude-last_altitude);       
      getBmpData();
      delay(200);
    }

    myFile=SD.open(logFile,FILE_APPEND);

  
    myFile.println("Trigger 1 triggered!");
    avg=avg/n;
        
    if(int(avg)<0){
      myFile.printf("Apogee : %d ft \n",apogee);
      myFile.println("Trigger 2 triggered!");
      
      drogue_altitude=curr_altitude;
      
      digitalWrite(buzzer,HIGH);
      digitalWrite(drogue_pin,HIGH);
      delay(5000);
      digitalWrite(drogue_pin,LOW);
      digitalWrite(buzzer,LOW);

      flag_drogue=true;
      myFile.printf("Drogue parachute deployed at : %d ft \n",drogue_altitude);
    }
    else{
      avg=0;
    }
    myFile.close();      
  }    
}

void main_func(){
  
  myFile=SD.open(logFile,FILE_APPEND);
  
  if(curr_altitude<=(location_altitude + 1500)){
    myFile.println("Trigger 3 triggered! \n");
    
    main_altitude=curr_altitude;

    digitalWrite(buzzer,HIGH);
    digitalWrite(main_pin,HIGH);
    delay(5000);
    digitalWrite(main_pin,LOW);
    digitalWrite(buzzer,LOW);

    flag_main=true;
    myFile.printf("Main parachute deployed at : %d ft \n",main_altitude);
    myFile.close();
  }
}