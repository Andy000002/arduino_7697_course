#include <LWiFi.h>
#include <PubSubClient.h>
#include <SeeedOLED.h>
#include <Wire.h>
#include "DHT.h"
#define MQTT_SUB_TOPIC "757113"
#define MQTT_SUB_TOPIC_1 "00757113"
#define MQTT_SERVER_IP "140.121.198.203"
#define MQTT_SERVER_PORT 1883
#define MQTT_CLIENT_ID "linklt7697_111dsf"
char _lwifi_ssid[] = "NTOU-WPA2";     //wifi名稱
char _lwifi_pass[] = "19991113";  //wifi密碼
char st = '0';
DHT __dht3(A0, DHT11);   //設置濕度感測器物件的腳位與型號
WiFiClient mqttClient;  //建立wifi用戶端物件
PubSubClient client(mqttClient);    //以wifi用戶端物件進行MQTT連線
void callback(char* topic, byte* payload, unsigned int length); //宣告接收訂閱內容的回傳參數
void setup() {  //初始配置 只做一次
  Serial.begin(9600);   //設定7697與電腦溝通的頻率
  pinMode(10, OUTPUT);   //設定繼電器腳位
  while (WiFi.begin(_lwifi_ssid, _lwifi_pass) != WL_CONNECTED) { delay(1000); }     //連接wifi
  Serial.println(WiFi.localIP().toString());  //列印7697分配到的IO
  __dht3.begin();   //啟用溫溼度感測器
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);   //設置MQTT Broker
  client.setCallback(callback); //設置MQTT訂閱呼叫函數
  connectMQTT();    //連接MQTT Broker
  //oled
  Wire.begin();
  SeeedOled.init();
  SeeedOled.setBrightness(255);
  SeeedOled.clearDisplay();
}

void loop() {   //無限執行區塊
  float h=__dht3.readHumidity();       //獲取濕度值
  float t=__dht3.readTemperature();    //獲取溫度值
  delay(5000);  //停頓5秒
  String jsonStr = "{\"temp\":"+String(t)+",\"humid\":"+String(h)+"}";  // 串接JSON字串
  Serial.println(jsonStr); 
  connectMQTT();    //若有斷線會再度連接動作
  char json[jsonStr.length()+1];    //宣告JSON字串長度+1的Char型態陣列 最後的位置需留做null當作結尾
  jsonStr.toCharArray(json,  jsonStr.length()+1);   //將JSON字串轉換成Char型態放入陣列
  client.publish(MQTT_SUB_TOPIC, json); //將JSON資料指定主題發送至Broker
  OledShow(t,h);
  client.loop();    //保持與MQTT Broker的連線 並且更新用戶狀態
}
void connectMQTT() {    //與MQTT Broker的連線方式
   // Loop until we're reconnected
  while (!client.connected())   //確認是否連線 無連線才執行動作
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID))     //指定7697的連上MQTT Broker的名稱
    {
      Serial.println("connected");
      client.subscribe(MQTT_SUB_TOPIC_1);   //向MQTT Broker訂閱的主題
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());     //印出連接狀態代碼
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);  //等待5秒後再確認是否成功連接上MQTT Broker
    }
  }
}
void callback(char* topic, byte* payload, unsigned int length) {    //詳細動作
  Serial.print("Input Message arrived [");
  Serial.print(MQTT_SUB_TOPIC_1);
  Serial.print("] ");
  Serial.print((char)payload[0]);
  if((char)payload[0] == '1'){      //判斷接收對繼電器控制的內容 1為開啟 0為關閉
    digitalWrite(10, HIGH);
  }else if((char)payload[0] == '0'){
   digitalWrite(10, LOW);
  }else{
    Serial.print("value error");
  }
  Serial.println();
  st = (char)payload[0];
}
void OledShow(float t,float h){
  SeeedOled.clearDisplay();
  SeeedOled.putString("temp:");
  SeeedOled.putFloat(t);
//  SeeedOled.putString(",\"humid\":");
//  SeeedOled.putFloat(h);
  SeeedOled.putString("\n");
  SeeedOled.setTextXY(7,15);
  SeeedOled.putChar(st);
}
