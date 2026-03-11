#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2
#define DHTTYPE DHT11

#define PIR 3
#define BUZZER 4

#define RED 5
#define GREEN 6
#define BLUE 7

#define DOOR_SENSOR 8

LiquidCrystal_I2C lcd(0x27,16,2);
DHT dht(DHTPIN,DHTTYPE);

const char* ssid="Airtel 4G MiFi_9034";
const char* password="50333266";

String server="https://YOUR_SERVER.onrender.com";

void scrollText(String text){

for(int i=0;i<text.length();i++){

lcd.clear();
lcd.setCursor(0,0);
lcd.print(text.substring(i,i+16));
delay(300);

}

}

void reconnectWiFi(){

if(WiFi.status()!=WL_CONNECTED){

WiFi.begin(ssid,password);

while(WiFi.status()!=WL_CONNECTED){

delay(500);

}

}

}

void sendData(){

float temp=dht.readTemperature();
float hum=dht.readHumidity();

int motion=digitalRead(PIR);
int door=digitalRead(DOOR_SENSOR);

String doorStatus="Closed";

if(door==HIGH){
doorStatus="Open";
}

String zone="NONE";

if(motion==HIGH){

zone="MOTION";
digitalWrite(BUZZER,HIGH);

}else{

digitalWrite(BUZZER,LOW);

}

HTTPClient http;

String url=server+"/arduino/data?zone="+zone+"&temp="+String(temp)+"&hum="+String(hum)+"&door="+doorStatus;

http.begin(url);
http.GET();
http.end();

lcd.clear();
lcd.setCursor(0,0);
lcd.print("Temp:");
lcd.print(temp);

lcd.setCursor(0,1);
lcd.print("Hum:");
lcd.print(hum);

}

void getCommand(){

HTTPClient http;

http.begin(server+"/arduino/get_command");

int code=http.GET();

if(code==200){

String payload=http.getString();

if(payload.indexOf("LED ON")!=-1){
digitalWrite(RED,HIGH);
}

if(payload.indexOf("LED OFF")!=-1){
digitalWrite(RED,LOW);
}

if(payload.indexOf("DISPLAY")!=-1){

int i=payload.indexOf("DISPLAY");
String msg=payload.substring(i+7);

scrollText(msg);

}

}

http.end();

}

void sendHeartbeat(){

HTTPClient http;

http.begin(server+"/heartbeat");

http.GET();

http.end();

}

void setup(){

Serial.begin(115200);

pinMode(PIR,INPUT);
pinMode(BUZZER,OUTPUT);

pinMode(RED,OUTPUT);
pinMode(GREEN,OUTPUT);
pinMode(BLUE,OUTPUT);

pinMode(DOOR_SENSOR,INPUT_PULLUP);

lcd.init();
lcd.backlight();

scrollText("Ultimate Home Guardian");

WiFi.begin(ssid,password);

while(WiFi.status()!=WL_CONNECTED){
delay(500);
}

dht.begin();

}

void loop(){

reconnectWiFi();

sendData();

getCommand();

sendHeartbeat();

delay(5000);
