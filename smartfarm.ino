
#include <dht.h>

#include <DHT.h>
#include <DHT_U.h>

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
#include <WiFiUdp.h>


// #include <WiFi.h>
// #include <WiFiClient.h>
// #include <WiFiServer.h>
// #include <WiFiUdp.h>


// // minimal testing (WORKS!!)

// void setup() {
//   Serial.begin(9600); // Set baud rate to 9600
//   delay(1000); // Wait for serial port to initialize
// }

// void loop() {
//   Serial.println("Hello, world!"); // Print "Hello, world!" to the serial monitor
//   delay(1000); // Wait for a second before repeating
// }

// Connection secrets 
// const char* ssid = "CANALBOX-6A42-2G";
// const char* password = "citadelvillas00";
// const char* thingSpeakApiKey = "MBNF6MX7ODG2YI4G";
// channel ID =int updateStatus = ThingSpeak.writeFields(2488468, thingSpeakApiKey);


// #include <WiFi.h>
// #include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>

#define DHTPIN 3      // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 // DHT 11

char ssid[] = "CANALBOX-6A42-2G"; // WiFi SSID
char password[] = "citadelvillas00";  // WiFi password
const char* thingSpeakApiKey = "MBNF6MX7ODG2YI4G";
const unsigned long postingInterval = 10 * 60 * 1000; // Interval to post data (in milliseconds)

WiFiClient client;

DHT dht(DHTPIN, DHTTYPE);

void setup() {

  Serial.begin(115200); // match serial monito baud

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
}

void loop() {
  // Read temperature and humidity
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  // Print sensor values
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Update ThingSpeak channel
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
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
