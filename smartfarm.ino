#include <ArduinoWiFiServer.h>
#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiGratuitous.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureBearSSL.h>
// #include <WiFi.h>
#include <WiFiClient.h>
// #include <WiFiUdp.h>
#include <LiquidCrystal.h>
#include <ThingSpeak.h>
#include <DHT.h>

// // minimal testing 

// void setup() {
//   Serial.begin(9600); // Set baud rate to 9600
//   delay(1000); // Wait for serial port to initialize
// }

// void loop() {
//   Serial.println("Hello, world!"); // Print "Hello, world!" to the serial monitor
//   delay(1000); // Wait for a second before repeating
// }

#include <LiquidCrystal.h>
#include <ThingSpeak.h>
#include <DHT.h>


#define DHTTYPE DHT11 // DHT 11
#define SOIL_MOISTURE_PIN A0 // Analog pin connected to the soil moisture sensor

const int DHTPIN = D2; // Digital pin connected to the DHT sensor initilize it as an int 


char ssid[] = ""; // WiFi SSID
char password[] = ""; // WiFi password


const char* thingSpeakApiKey = ""; // get the  value from channel APIkey
const unsigned long postingInterval = 10 * 60 * 1000; // Interval to post data (in milliseconds)




WiFiClient client;
DHT dht(DHTPIN, DHTTYPE);

// LCD connections to NodeMCU
LiquidCrystal lcd(D3, D4, D5, D6, D7, D8); // RS, E, DB4, DB5, DB6, DB7


void setup() {
  Serial.begin(9600); // match serial monitor baud

  delay(100); // Give time to initialize
  
  // Connect to WiFi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);

  // Initialize DHT sensor
  dht.begin();

  // Initialize LCD
  lcd.begin(16, 2); // Adjust the parameters if your LCD is different
  lcd.clear();
  lcd.print("Temperature:");
  lcd.setCursor(0, 1);
  lcd.print("Humidity:");
}

void loop() {
  // Read temperature and humidity
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

   // Read soil moisture
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN);
  
  // Print sensor values to LCD
  // lcd.setCursor(13, 0);
  lcd.print(temperature);
  lcd.setCursor(13, 1);
  lcd.print(humidity);

  // Print sensor values to Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print("Soil Moisture: ");
  Serial.println(soilMoisture);

  // Update ThingSpeak channel
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, soilMoisture);
  
  int updateStatus = ThingSpeak.writeFields(2488468, thingSpeakApiKey);

  if (updateStatus == 200) {
    Serial.println("Channel update successful");
  } else {
    Serial.print("Problem updating channel. HTTP error code ");
    Serial.println(updateStatus);
    // Add delay before retrying
    delay(5000); // Wait for 5 seconds before retrying
  }
  
  // Wait for the next posting interval
  delay(postingInterval);
}
