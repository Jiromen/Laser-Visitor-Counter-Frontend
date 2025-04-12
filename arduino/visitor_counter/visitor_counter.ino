#define sensorPin D2
String lastVisitorID = "";
int visitor;
int visitorDetected;

#include <ArduinoJson.h>

// Internet Components
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Bianca WiFi
const char* ssid = "IoT";
const char* password = "AccessPoint.2024";
const char* host = "http://192.168.68.100";

// const char* ssid = "ACM2";
// const char* password = "0495452821@2024";
// const char* host = "http://192.168.1.42";

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);

  wifiConfig();   // WiFi Configuration / SetUp
  initCount();    // Fetch latest visitor count from DB
  
  lcd.init();
  lcd.backlight();
  visualFeedback();
  pinMode(sensorPin, INPUT);
}

void loop() {
  visitorDetected = digitalRead(sensorPin);

  if (visitorDetected == HIGH) {
    visitor++;
    newVisit();
    visualFeedback();
    while(visitorDetected == HIGH){
      visitorDetected = digitalRead(sensorPin);
    }
  }
  
  delay(200);
}

void newVisit(){
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient wifi;
    String server_newVisit = String(host) + "/laser-visitor-counter-IoT-NodeMCU-RFID/queries/new_visitor.php?vID=" + String(visitor);
    http.begin(wifi, server_newVisit); 
    http.addHeader("Content-Type", "text/plain");
    int httpCode = http.GET();
    if (httpCode > 0) {
      String response = http.getString();
      //Serial.println(response);
    } else {
      Serial.println("HTTP Error: " + http.errorToString(httpCode));
    }
    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }

  return;
}

void wifiConfig(){
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);  // Start the Wi-Fi connection

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
}

void initCount(){
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient wifi;
    String server_fetch = String(host) + "/laser-visitor-counter-IoT-NodeMCU-RFID/queries/count_visitor.php";
    http.begin(wifi, server_fetch); 
    int httpCode = http.GET();
    if (httpCode > 0) {
      String response = http.getString();
      // Parse the JSON response
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, response);
      const char* visitor_id = doc["visitor_id"];
      visitor = atoi(visitor_id); // Convert visitor_id to integer
    } else {
      Serial.println("HTTP Error: " + http.errorToString(httpCode));
    }
    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }

  return;
}

void visualFeedback(){
  Serial.print("Visitor #: ");
  Serial.println(visitor);

  lcd.clear();
  lcd.setCursor(0, 0); 
  lcd.print("Visitor #: ");
  lcd.setCursor(11, 0); 
  lcd.print(visitor); 
}
