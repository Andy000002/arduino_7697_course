#include <DHT.h>
#include <Wire.h>
#include <SeeedOLED.h>
#include "SI114X.h" //sunlight

SI114X SI1145 = SI114X();
DHT dht(A0,DHT11);
int SoilMoistureValue = 0, btnstate = 0, counter=0,encoder0PinA =3,encoder0PinB = 4;
float h,t,f;
bool SIGB = LOW;

void setup(){
  Serial.begin(9600);
  dht.begin();
  Wire.begin();
  Serial.print("Beginning Si1145!");
  while(!SI1145.Begin()){
    Serial.println("Si1145 is not ready!");
    delay(1000);
  }
  Serial.println("Si1145 is ready!");
  SeeedOled.init();
  SeeedOled.setBrightness(255);
  SeeedOled.clearDisplay();
  SeeedOled.setTextXY(0,0);
  pinMode(2,INPUT);//btn
  pinMode(10,OUTPUT);
  pinMode(encoder0PinA,INPUT);
  pinMode(encoder0PinB,INPUT);
  attachInterrupt(2,Relay,HIGH);
  attachInterrupt(encoder0PinA,encoder,RISING);
}

void loop() {
  SoilMoistureValue = analogRead(A1)/4;
  btnstate = digitalRead(2);//readbtn
  delay(500);
  SeeedOled.clearDisplay();
  h = dht.readHumidity();
  t = dht.readTemperature();
  f = dht.readTemperature(true);
  if(isnan(h)||isnan(t)||isnan(f)){
    Serial.println("Fail to connect temp sensor");
    return; 
  }
  SeeedOled.putString("SoilM:");
  SeeedOled.putNumber(SoilMoistureValue);
  SeeedOled.setTextXY(1,0);
  SeeedOled.putString("Humi:");
  SeeedOled.putFloat(h);
  SeeedOled.setTextXY(2,0);
  SeeedOled.putString("Temp:");
  SeeedOled.putFloat(t);
  SeeedOled.putString("*C");
  SeeedOled.setTextXY(3,0);
  SeeedOled.putString("Vis:");
  SeeedOled.putNumber(SI1145.ReadVisible());
  SeeedOled.setTextXY(4,0);
  SeeedOled.putNumber(btnstate);
}
void Relay(){
 if(digitalRead(2)==HIGH){
    digitalWrite(10,HIGH);
    Serial.println("抽你老母");
  } 
  else{
    digitalWrite(10,LOW);
  }
 }
void encoder(){
  SIGB = digitalRead(encoder0PinB);
  if(SIGB==LOW){
    Serial.println("你逆");
    counter--;
    }
    else{
      Serial.println("你順");
      counter++;
    }
    Serial.print(counter);
  }
