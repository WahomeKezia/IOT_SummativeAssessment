#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define pins for ultrasonic sensors
const int frontSensorTriggerPin = 3;
const int frontSensorEchoPin = 2;
const int rearSensorTriggerPin = 4;
const int rearSensorEchoPin = 5;

// Define pins for LEDs and buzzer
const int redLEDPin = 12;
const int greenLEDPin = 9;
const int buzzerPin = 10;

// Define variables to store distances
int frontDistance = 0;
int rearDistance = 0;

// Set the LCD address
const int lcdAddr = 0x20; // I2C address of the LCD

// Define LCD dimensions (16x2)
const int lcdCols = 16;
const int lcdRows = 2;

// Initialize LCD
LiquidCrystal_I2C lcd(lcdAddr, lcdCols, lcdRows);

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Set ultrasonic sensor pins as input/output
  pinMode(frontSensorTriggerPin, OUTPUT);
  pinMode(frontSensorEchoPin, INPUT);
  pinMode(rearSensorTriggerPin, OUTPUT);
  pinMode(rearSensorEchoPin, INPUT);

  // Set LED and buzzer pins as output
  pinMode(redLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // Initialize LCD
  lcd.init();
  lcd.backlight(); // Turn on backlight
}

void loop() {
  // Detect distance using front sensor
  frontDistance = detectDistance(frontSensorTriggerPin, frontSensorEchoPin);
  // Display distance on LCD
  displayDistanceOnLCD(frontDistance, "Front");

  // Detect distance using rear sensor
  rearDistance = detectDistance(rearSensorTriggerPin, rearSensorEchoPin);
  // Display distance on LCD
  displayDistanceOnLCD(rearDistance, "Rear");

  // Check if a suspect is detected
  if (frontDistance <= 200 || rearDistance <= 200) {
    // Suspect detected, activate alarm
    digitalWrite(redLEDPin, HIGH); // Blink red LED
    digitalWrite(greenLEDPin, LOW); // Turn off green LED
    tone(buzzerPin, 1000); // Activate buzzer
    delay(1000); // Wait for 1 second
    noTone(buzzerPin); // Stop buzzer
    lcd.setCursor(0, 1);
    lcd.print("Suspect Detected ");
  } else {
    // No suspect detected, deactivate alarm
    digitalWrite(redLEDPin, LOW); // Turn off red LED
    digitalWrite(greenLEDPin, HIGH); // Turn on green LED
    lcd.setCursor(0, 1);
    lcd.print("Safe               ");
  }
  delay(2000); // Wait for 2 seconds before next reading
}

int detectDistance(int triggerPin, int echoPin) {
  // Generate ultrasonic pulse
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  // Measure the duration of the pulse
  long duration = pulseIn(echoPin, HIGH);

  // Convert the duration to distance
  int distance = duration * 0.034 / 2; // Speed of sound in cm/us

  return distance;
}

void displayDistanceOnLCD(int distance, String location) {
  lcd.setCursor(0, 0);
  lcd.print(location + ": ");
  if (distance > 0) {
    lcd.print(distance);
    lcd.print(" cm       ");
  } else {
    lcd.print("No object detected ");
  }
}
