TaskHandle_t task_loop1;

void esploop1(void* pvParameters){
  setup1();
  for(;;){
    loop1();
  }
}

int count0,count1;

void setup() {
  xTaskCreatePinnedToCore(esploop1,"loop1",10000,NULL,1,&task_loop1,!ARDUINO_RUNNING_CORE);
  Serial.begin(9600);
  count0=0;
}

void setup1(){
  count1=1000;
}

void loop() {
  Serial.printf("loop%d(); %d\n",xPortGetCoreID(),count0);
  count0++;
  if(count0>1000){
    count0=0;
  }
  delay(1000);  
}

void loop1(){
  Serial.printf("loop%d(); %d\n",xPortGetCoreID(),count1);
  count1--;
  if(count1<=0){
    count1=1000;
  }
  delay(2500);
}