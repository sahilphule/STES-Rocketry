 #include <Wire.h>
//#include <Streaming.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <TinyGPS++.h>
Adafruit_BMP280 bmp;
#include<SoftwareSerial.h>
SoftwareSerial sserial (10,9);
SoftwareSerial ss(2,1);

void checksum();
TinyGPSPlus gps;

float pressure,altitude,alt2,pre2,lati2,lon2,spd2,gps_alt2;
long alt1,pre1,lati1,lon1,spd1,gps_alt1;
int sum1,sum2;
char char1,n;
byte x=0;
TinyGPSCustom pdop(gps, "GNGLL", 1); // $GPGSA sentence, 15th element
TinyGPSCustom hdop(gps, "GNGLL", 3); // $GPGSA sentence, 16th element

byte packet[]={0X7E ,0X00 ,0X37 ,0X10 ,0X01 ,0X00 ,0X13 ,0XA2 ,0X00 ,0X41 ,0X92 ,0X5C ,0X75 ,0XFF ,0XFE ,0X00 ,0X00 //0-16
,0X00 ,0X00 ,0X00 ,0X00 ,0X2E ,0X00 ,0X00//17-23 bmp altitude
,0X00 ,0X00 ,0X2E ,0X00 ,0X00 ,0X00 ,0X00, 0X00, 0X00, 0X00//24-33 latitude
,0X00 ,0X00 ,0X2E ,0X00 ,0X00 ,0X00 ,0X00, 0X00, 0X00, 0X00//34-43 longitude
,0X00 ,0X00 ,0X00 ,0X00 ,0X2E ,0X00 ,0X00//44-50 Velocity
,0X00 ,0X00 ,0X00 ,0X00 ,0X2E ,0X00 ,0X00//51-57 gps altitude
,0X00};// 58 checksum

void setup() {
  sserial.begin(19200);
  Serial.begin(9600);
  ss.begin(9600);

   if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);}
 }

void loop() {

    while (ss.available() > 0) {
    gps.encode(ss.read());
  }

  pressure = bmp.readPressure();
  altitude = bmp.readAltitude(1013)*3.28084;
  
  float lati=gps.location.lat();
  float lon= gps.location.lng();
  float speed=gps.speed.mps();
  float gps_alt=gps.altitude.feet();
  
  alt2=altitude*100;
  alt1=(long(alt2));

  spd2=speed*100;
  spd1=(long(spd2));
  
  gps_alt2=gps_alt*100;
  gps_alt1=(long(gps_alt2));
  
  int lati_1=lati;
  float lati_2=lati-float(lati_1);
  long lati_3=lati_2*10000000;
  
  int lon_1=lon;
  float lon_2=lon-float(lon_1);
  long lon_3=lon_2*10000000;
  
  for(int i=0;i<7;i++)
  {
   if(i!=2)
   {
    char char1=((alt1%10)+48);
    packet[23-i]=byte(char1);
    alt1=alt1/10;
   }
  }
 
  for(int i=0;i<2;i++)
  {
    char char1=((lati_1 % 10)+48);
    packet[25-i]=byte(char1);
    lati_1=lati_1/10;
  }
 for(int i=0;i<7;i++)
  {
    char char1=((lati_3 % 10)+48);
    packet[33-i]=byte(char1);
    lati_3=lati_3/10;
  }
  
 for(int i=0;i<2;i++)
  {
    char char1=((lon_1 % 10)+48);
    packet[35-i]=byte(char1);
    lon_1=lon_1/10;
  }
 for(int i=0;i<7;i++)
  {
    char char1=((lon_3%10)+48);
    packet[43-i]=byte(char1);
    lon_3=lon_3/10;
  }
  for(int i=0;i<7;i++)
  {
   if(i!=2)
   {
    char char1=((spd1%10)+48);
    packet[50-i]=byte(char1);
    spd1=spd1/10;
   }
  }

  for(int i=0;i<7;i++)
  {
   if(i!=2)
   {
    char char1=((gps_alt1%10)+48);
    packet[57-i]=byte(char1);
    gps_alt1=gps_alt1/10;
   }
  }
  
  checksum(); 
 
  
  sserial.write(packet,sizeof(packet));
  delay(500);
}
void checksum(){
  sum1=0;
  for(int i=3;i<58;i++)
  {
   sum1=sum1+long(packet[i]);  
  }
  
  byte ff = byte(sum1);
  byte xyz=0xFF - ff;
  packet[58]=xyz;
}