#include <WiFi.h>
#include <Wire.h>
#include <ThingSpeak.h> // ThingSpeak library

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ThingSpeak credentials
const char* thingSpeakApiKey = "MBNF6MX7ODG2YI4G";
const unsigned long postingInterval = 10 * 1000; // Interval to post data (in milliseconds)

// Analog pin assignments
const int tempSensorPin = A0;
const int moistureSensorPin = A1;

void setup() {
  Serial.begin(9600);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  // Read temperature and soil moisture
  float temperature = readTemperature();
  int soilMoisture = readSoilMoisture();
  
  // Print sensor values
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("Soil Moisture: ");
  Serial.println(soilMoisture);
  
  // Update ThingSpeak channel
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, soilMoisture);
  int updateStatus = ThingSpeak.writeFields(YOUR_CHANNEL_ID, thingSpeakApiKey);

  if (updateStatus == 200) {
    Serial.println("Channel update successful");
  } else {
    Serial.print("Problem updating channel. HTTP error code ");
    Serial.println(updateStatus);
  }
  
  // Wait for the next posting interval
  delay(postingInterval);
}

float readTemperature() {
  // Read temperature sensor data (replace with your sensor code)
  // For example, if you're using a TMP36 temperature sensor:
  int tempRaw = analogRead(tempSensorPin);
  float tempVoltage = tempRaw * (5.0 / 1023.0);
  float temperature = (tempVoltage - 0.5) * 100.0; // Convert voltage to Celsius
  return temperature;
}

int readSoilMoisture() {
  // Read soil moisture sensor data (replace with your sensor code)
  // For example, if you're using a resistive soil moisture sensor:
  int moisture = analogRead(moistureSensorPin);
  return moisture;
}
void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
