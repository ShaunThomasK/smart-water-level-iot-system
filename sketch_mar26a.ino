#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* serverName = "http://api.thingspeak.com/update";
String apiKey = "YOUR_THINGSPEAK_WRITE_API_KEY";
const int trigPin = 5;
const int echoPin = 18;      
const int waterSensorPin = 34;
const int irSensorPin = 27;

// Calibrated values of slope and y-intercept from previous experiments
float m = -42.5; 
float c = 1115.0;

void setup() 
{
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(irSensorPin, INPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
}

void loop() 
{
  int containerPresent = digitalRead(irSensorPin); 
  if (containerPresent == HIGH) 
  { 
    Serial.println("Container not detected. System Idle.");
    delay(2000);
    return;
  }
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;
  int waterADC = analogRead(waterSensorPin);
  float calibratedADC = (m * distance) + c; 
  if (WiFi.status() == WL_CONNECTED) 
  {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "api_key=" + apiKey + "&field1=" + String(distance) + "&field2=" + String(waterADC) + "&field3=" + String(calibratedADC);
    int httpResponseCode = http.POST(httpRequestData);
    Serial.print("ThingSpeak Response code: ");
    Serial.println(httpResponseCode);
    http.end();
  }
  Serial.print("Dist: "); Serial.print(distance);
  Serial.print("cm | Ref ADC: "); Serial.print(waterADC);
  Serial.print(" | Calibrated: "); Serial.println(calibratedADC);
  delay(16000); 
}