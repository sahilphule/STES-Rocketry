#include <SPI.h>
#include <SD.h>

File myFile;

const int CS=5;
String path="/output.txt";

void setup() {

  Serial.begin(9600);

  delay(5000);

  if(SD.begin(CS)){
    Serial.println("SD initialized successfully!");
    Serial.println();
    if(SD.exists(path)){
      Serial.println("Deleting old file with same name!");
      Serial.println();
      SD.remove(path);
    }
  }
  else{
    Serial.println("Error initialising SD!");
    while(1){

    }
  }
}

void loop() {

  fileWrite();

  delay(1000);

}

void fileWrite(){
  
  myFile=SD.open(path,FILE_APPEND);

  if(myFile){
    myFile.println("STES ROCKETRY");
    Serial.println("STES ROCKETRY");
    myFile.close();
  }
  else{
    Serial.println("Error opening file!");
    while(1){
      
    }
  }
}