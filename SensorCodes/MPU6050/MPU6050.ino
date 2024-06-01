#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>

Adafruit_MPU6050 mpu;

float acceleration_x,acceleration_y,acceleration_z,rotation_x,rotation_y,rotation_z;

void setup() {
  
  Serial.begin(9600);

  if(mpu.begin(0x68)){
    Serial.println("MPU initialized successfully!");
    Serial.println();
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  }
  else{
    Serial.println("Error initialising MPU!");
    while(1){

    }
  }

}

void loop() {

  getMpuData();
  delay(1000);

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

  Serial.print("Acceleration_x : ");
  Serial.print(acceleration_x);
  Serial.print(" m/s2");
    
  Serial.print("          Acceleration_y : ");
  Serial.print(acceleration_y);
  Serial.print(" m/s2");
  
  Serial.print("          Acceleration_z : ");
  Serial.print(acceleration_z);
  Serial.println(" m/s2");
  
  Serial.print("Rotation_x     : ");
  Serial.print(rotation_x);
  Serial.print(" deg");
  
  Serial.print("          Rotation_y     : ");
  Serial.print(rotation_y);
  Serial.print(" deg");
  
  Serial.print("           Rotation_z     : ");
  Serial.print(rotation_z);
  Serial.println(" deg");

  Serial.println();
}