//Wi-fi
#include <WiFi.h>
#include <FirebaseESP32.h>
#define FIREBASE_HOST "afaea-b2995.firebaseio.com"
#define FIREBASE_AUTH "4tbNsSAzy1lyIC5eAbQhdKVVCiU7fKHyXApM0Q6g"
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASSWORD"

FirebaseData firebaseData;
FirebaseJson json;

//Root Path
String path = "/ESP32_Device";

//Sensor water level
int water;
int pin_data = 34;

//Soil Moisture
int sensor_pin = 35;
int valueMoisture;

//dht
#include "DHT.h"
#define DHTPIN 4 
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE); 

float prev_temp;
float prev_humidity;
float prev_water;
float prev_moisture;
float h,t;
int i = 0;

void setup() {  
  //Water Level
  pinMode(pin_data,INPUT);
  
  //Serial
  Serial.begin(115200);
  Serial.println("Reading");
  delay(2000);

  dht.begin();
  initWifi();
}

void loop() {
  //Water level
  water = analogRead(pin_data);

  //Soil
  valueMoisture = analogRead(sensor_pin);

  //dht
  h = dht.readHumidity();
  t = dht.readTemperature();
    if (isnan(h) || isnan(t) ) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  
  //Output
  Serial.print("Water level: ");
  Serial.println(water);
  updateWater(water);
  
  Serial.print("Soil Moisture: ");
  Serial.println(valueMoisture);
  updateMoisture(valueMoisture);
  
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.println("%");
  updateHumidity(h);
  
  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  updateTemp(t);

  delay(1000);
}

void updateWater(float water){
  if(prev_water != water){
    String waterString = "";
    waterString += (int)water;
    prev_water = water;
    i++;

    Firebase.setDouble(firebaseData, path +"/WaterLevel/Data", water);
    Firebase.setDouble(firebaseData, path +"/WaterLevel/Data"+i, water);
  }
}

void updateMoisture(float valueMoisture){
  if(prev_moisture != valueMoisture){
    String moistureString = "";
    moistureString += (int)valueMoisture;
    prev_moisture = valueMoisture;
    i++;

    Firebase.setDouble(firebaseData, path +"/SoilMoisture/Data", valueMoisture);
    Firebase.setDouble(firebaseData, path +"/SoilMoisture/Data"+i, valueMoisture);
  }
}

void updateTemp(float temp){
  if(prev_temp != temp){
    String tempString = "";
    tempString += (int)temp;
    tempString += "C";
    tempString += temp;
    prev_temp = temp;
    i++;

    Firebase.setDouble(firebaseData, path +"/Temperature/Data", temp);
    Firebase.setDouble(firebaseData, path +"/Temperature/Data"+i, temp);
  }
}


void updateHumidity(float humidity){
  if(prev_humidity != humidity){
    String humidityString = "";
    humidityString += (int)humidity;
    humidityString += "%";
    prev_humidity = humidity;
    i++;

    Firebase.setDouble(firebaseData, path +"/Humidity/Data", humidity);
    Firebase.setDouble(firebaseData, path +"/Humidity/Data"+i, humidity);
  }
}

void initWifi(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected wifi IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
}
