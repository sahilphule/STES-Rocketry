#include <SoftwareSerial.h>
SoftwareSerial sserial(2,4);

byte packet[]={0X7E ,0X00 ,0X12 ,0X10 ,0X01 ,0X00 ,0X13 ,0XA2 ,0X00 ,0X41 ,0X92 ,0X5C ,0X75 ,0XFF ,0XFE ,0X00 ,0X00 ,0X48 ,0X45 ,0X4C ,0X4C ,0X73};
void setup() {
Serial.begin(9600);
sserial.begin(9600);
delay(1000);

Serial.println("Setup complete\n");
Serial.println("Check receiving radio console");
}

void loop() {
sserial.write(packet,sizeof(packet));
delay(1000);
}