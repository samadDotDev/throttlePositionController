#define DRIVER_GATE 3 //D5
#define ETC_RELAY 7 //D7

#define APS_1 A0 //
#define APS_2 A1 //A1
#define APS_3 A2 //A2
#define APS_4 A3 //A3`

#define TPS_1 A4 //A8 for pro micro / lenardo
#define TPS_2 A5 //A9 for pro micro / lenardo

volatile int updatedPercentage;
volatile int throttlePosition;
volatile int calculatedThrottle;

volatile int apsValue1;
volatile int apsValue2;
volatile int apsValue3;
volatile int apsValue4;
volatile int apsAverage;

volatile int tpsValue1;
volatile int tpsValue2;
volatile int tpsAverage;

volatile int warningLevel;

int thresholdAnalogWarningValue = 100;

void setup() {

Serial.begin(9600);

pinMode(APS_1,INPUT_PULLUP);
pinMode(APS_2,INPUT_PULLUP);
pinMode(APS_3,INPUT_PULLUP);
pinMode(APS_4,INPUT_PULLUP);

pinMode(TPS_1,INPUT_PULLUP);
pinMode(TPS_2,INPUT_PULLUP);

pinMode(DRIVER_GATE,OUTPUT);
pinMode(ETC_RELAY,OUTPUT);

}

void loop() {

  warningLevel = 0;

  apsValue1 = analogRead(APS_1);
  apsValue2 = analogRead(APS_2);
  apsValue3 = analogRead(APS_3);
  apsValue4 = analogRead(APS_4);

  apsAverage = (apsValue1+apsValue2+apsValue3+apsValue4)/4;
  
  tpsValue1 = analogRead(TPS_1);
  tpsValue2 = 1024 - analogRead(TPS_2);

  tpsAverage = (tpsValue1+tpsValue2)/2;

  Serial.print("APS1: ");
  Serial.print(apsValue1);
  Serial.print(", APS2: ");
  Serial.print(apsValue2);
  Serial.print(", APS3: ");
  Serial.print(apsValue3);
  Serial.print(", APS4: ");
  Serial.print(apsValue4);
  Serial.print(", TPS1: ");
  Serial.print(tpsValue1);
  Serial.print(", TPS2: ");
  Serial.print(tpsValue2);
  Serial.println();

  Serial.print("APS Average: ");
  Serial.print(apsAverage);
  Serial.print(", TPS Average: ");
  Serial.print(tpsAverage);
  Serial.println();

  // check O.C:

   if(apsValue1>1000){
    warningLevel++;
   }
   if(apsValue2>1000){
    warningLevel++;
   }
   if(apsValue3>1000){
    warningLevel++;
   }
   if(apsValue4>1000){
    warningLevel++;
   }


  // Compare Sensors:

  if(abs(apsValue1-apsValue2) > thresholdAnalogWarningValue){
    warningLevel++;
  }else if(abs(apsValue1-apsValue3) > thresholdAnalogWarningValue){
    warningLevel++;
  }else if(abs(apsValue1-apsValue4) > thresholdAnalogWarningValue){
    warningLevel++;
  }

  if(abs(apsValue2-apsValue3) > thresholdAnalogWarningValue){
    warningLevel++;
  }else if(abs(apsValue2-apsValue4) > thresholdAnalogWarningValue){
    warningLevel++;
  }
  
  if(abs(apsValue3-apsValue4) > thresholdAnalogWarningValue){
    warningLevel++;
  }

  // Compare Sensors ^

  if(warningLevel>8){ // debug 8
      shutdownETB();
  }else{

    // Not enough warning to disable ETC, it has a Go!

   //calculatedThrottle = (((apsValue1+apsValue2+apsValue3+apsValue4)/4))/10.24;
   calculatedThrottle = (1024-apsAverage)/10.24;// debug
    Serial.print("Average Calculated: ");
    Serial.print(calculatedThrottle);
    Serial.println("%");
    setThrottlePercentage(calculatedThrottle);    
    
  }  
  

}

void setThrottlePercentage(int percentage){

if(updatedPercentage > percentage ){
    analogWrite(DRIVER_GATE,0);
    delay(50);
}

if(percentage<50){
 analogWrite(DRIVER_GATE,0);
}
  //analogWrite(3,0);
  Serial.print("Setting Throttle to ");
  Serial.print(percentage);
  Serial.println("%");

if(percentage<10){
 throttlePosition = 0;
}else if(percentage>90){
 throttlePosition =255;
}else{
  throttlePosition = 100+(percentage*0.6);
}

 
  //analogWrite(2,throttlePosition);

  analogWrite(DRIVER_GATE,throttlePosition);
  Serial.println(throttlePosition);

  /*
  
  //throttlePosition = percentage*2.55;
  throttlePosition = 40+(percentage*0.7);
  Serial.println(throttlePosition);
  //analogWrite(DRIVER_IN_1,HIGH);
  //analogWrite(DRIVER_IN_3,HIGH);

  //analogWrite(DRIVER_IN_4,throttlePosition);
  analogWrite(DRIVER_IN_1,100);
  analogWrite(DRIVER_IN_2,100);
  analogWrite(DRIVER_IN_3,100);
  analogWrite(DRIVER_IN_4,100);
  //digitalWrite(DRIVER_IN_4,LOW);
  digitalWrite(DRIVER_EN_A,HIGH);
  digitalWrite(DRIVER_EN_B,HIGH);

  */

  //analogWrite(DRIVER_IN_1,throttlePosition);
  //analogWrite(DRIVER_IN_3,throttlePosition);
  //digitalWrite(DRIVER_IN_4,LOW);
  //digitalWrite(DRIVER_IN_4,LOW);
  updatedPercentage = percentage;
}

void shutdownETB(){
  digitalWrite(ETC_RELAY,LOW);
  digitalWrite(DRIVER_GATE,LOW);
  Serial.println("ETB Shutdown");
}

void enableETB(){
  digitalWrite(ETC_RELAY,HIGH);
}

