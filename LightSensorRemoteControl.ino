#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

// WiFi credentials
const char* ssid = ""; // wifi name 
const char* password = ""; ?//wifi password 

// Adafruit IO configuration
#define IO_USERNAME  ""
#define IO_KEY       ""
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    IO_USERNAME
#define AIO_KEY         IO_KEY

// ThingSpeak configuration
#define THINGSPEAK_CHANNEL_ID "Enter channel ID here"
#define THINGSPEAK_API_KEY "" // enter API key here 

#define LED_PIN D1 // Digital pin connected to the LED
#define LIGHT_SENSOR_PIN A0  // Analog pin connected to the light sensor

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Adafruit IO feeds
Adafruit_MQTT_Publish lightIntensityFeed = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/light-intensity");
Adafruit_MQTT_Subscribe controlLED = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/controlLED");

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    
    // Disconnect only if the client is connected
    if (mqtt.connected()) {
        mqtt.disconnect();
    }
    
    delay(1000);
    retries--;
    if (retries == 0) {
        Serial.println("Max retries reached, unable to connect to MQTT server.");
        while (1);
    }
  }

  // If the code reaches here, it means the connection was successful
  Serial.println("MQTT Connected!");
}

void setup() {
  Serial.begin(9600);

  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Turn off LED initially

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected.");

  // Connect to MQTT
  MQTT_connect();

  // Subscribe to controlLED feed
  mqtt.subscribe(&controlLED);

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  // Read light intensity
  int lightIntensity = analogRead(LIGHT_SENSOR_PIN);
  Serial.print("Light Intensity(lux): ");
  Serial.println(lightIntensity);

  // Publish light intensity to Adafruit IO
  if (!lightIntensityFeed.publish(String(lightIntensity).c_str())) {
    Serial.println("Failed to publish light intensity to Adafruit IO!");
  }

  // Publish light intensity to ThingSpeak
  ThingSpeak.writeField(THINGSPEAK_CHANNEL_ID, 1, lightIntensity, THINGSPEAK_API_KEY);

  // Control LED based on light intensity
  if (lightIntensity > 250) {
    digitalWrite(LED_PIN, HIGH); // Turn on LED
  } else {
    digitalWrite(LED_PIN, LOW); // Turn off LED
  }

  // Check for controlLED messages
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &controlLED) {
      Serial.print("Received MQTT message: ");
      Serial.println((char *)controlLED.lastread);

      // Convert message to uppercase for comparison
      String message = String((char *)controlLED.lastread);
      message.toUpperCase();

      // Control LED based on switch state
      if (message == "ON") {
        digitalWrite(LED_PIN, HIGH); // Turn on LED
      } else if (message == "OFF") {
        digitalWrite(LED_PIN, LOW); // Turn off LED
      } else {
        Serial.println("Invalid switch state received!");
      }
    }
  }

  delay(1000); // Adjust delay as needed
}
