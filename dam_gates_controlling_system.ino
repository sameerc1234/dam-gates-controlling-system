#define BLYNK_TEMPLATE_ID "TMPL38B9XUXpB"
#define BLYNK_TEMPLATE_NAME "iot"
#define BLYNK_AUTH_TOKEN "ndbF0KJFRqxGqaS0W9ukBx_J51XEZhHR"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Servo.h>

// WiFi credentials
char auth[] = BLYNK_AUTH_TOKEN; // Enter your Blynk Auth Token
const char* ssid = "SAMEER!"; // Enter your WIFI SSID
const char* password = "11551155"; // Enter your WIFI Password

// Telegram credentials
#define BOTtoken "7588548414:AAFX1jQbQCTs0XLQpu0bd8ugeZINpO7J3PE" // Enter the bot token you got from botfather
#define CHAT_ID "1083232259" // Enter your chat ID you got from chatid bot

// Pins
const int waterSensorPin = A0;  // Analog pin for water level sensor
const int servoPin1 = D2;       // Servo motor pin for gate 1
const int servoPin2 = D1;       // Servo motor pin for gate 2
const int buzzerPin = D5;       // Buzzer pin

// Default Water level thresholds in percentage
float thresholdValue1 = 40.0; // Threshold for opening gate 1
float thresholdValue2 = 40.0; // Threshold for opening both gates
float floodThreshold = 40.0;  // Threshold for sending flood alert
float ledThreshold = 40.0;    // Threshold for turning on the LED

// Maximum height of the tank (in cm)
const float maxHeight = 100.0; // Replace with your tank's maximum height

// Initialize the Servos
Servo gateServo1;
Servo gateServo2;

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

unsigned long previousMillis = 0;
const long interval = 20000; // Interval for sending status to Telegram (20 seconds)

// Test case variables
bool testMode = false;
bool testExecuted = false; // Flag to indicate if test case has been executed
unsigned long testStartTime = 0;
const int testInterval = 5000; // Test case duration (5 seconds)

// Malfunction detection variables
const int sensorCheckInterval = 60000; // Interval to check for sensor malfunction (60 seconds)
unsigned long lastSensorCheck = 0;
const float minValidSensorValue = 0.0; // Minimum expected sensor value
const float maxValidSensorValue = 100.0; // Maximum expected sensor value

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  configTime(0, 0, "pool.ntp.org");
  client.setTrustAnchors(&cert);

  // Initialize WiFi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize Blynk
  Blynk.begin(auth, ssid, password);

  // Initialize the Servos and close gates
  gateServo1.attach(servoPin1);
  gateServo2.attach(servoPin2);
  gateServo1.write(0); // Close gate 1
  gateServo2.write(0); // Close gate 2

  // Initialize the buzzer pin
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW); // Turn off the buzzer initially

  // Send startup messages to Telegram
  bot.sendMessage(CHAT_ID, "Wifi Connected!", "");
  bot.sendMessage(CHAT_ID, "System has Started!!", "");
}

// Blynk function to control both gates
BLYNK_WRITE(V1) {
  int gateControl = param.asInt();
  if (gateControl == 0) {
    gateServo1.write(0); // Close gate 1
    gateServo2.write(0); // Close gate 2
    bot.sendMessage(CHAT_ID, "Both Gates are CLOSED", "");
  } else if (gateControl == 1) {
    gateServo1.write(45); // Partially open gate 1
    gateServo2.write(45); // Partially open gate 2
    bot.sendMessage(CHAT_ID, "Both Gates are PARTIALLY OPEN", "");
  } else if (gateControl == 2) {
    gateServo1.write(90); // Fully open gate 1
    gateServo2.write(90); // Fully open gate 2
    bot.sendMessage(CHAT_ID, "Both Gates are FULLY OPEN", "");
  }
  testMode = true; // Start test case
  testExecuted = false; // Reset test executed flag
  testStartTime = millis(); // Start test timer
}

// Blynk function to control gate 1
BLYNK_WRITE(V2) {
  int gateControl = param.asInt();
  if (gateControl == 0) {
    gateServo1.write(0); // Close gate 1
    bot.sendMessage(CHAT_ID, "Gate 1 is CLOSED", "");
  } else if (gateControl == 1) {
    gateServo1.write(45); // Partially open gate 1
    bot.sendMessage(CHAT_ID, "Gate 1 is PARTIALLY OPEN", "");
  } else if (gateControl == 2) {
    gateServo1.write(90); // Fully open gate 1
    bot.sendMessage(CHAT_ID, "Gate 1 is FULLY OPEN", "");
  }
}

// Blynk function to control gate 2
BLYNK_WRITE(V3) {
  int gateControl = param.asInt();
  if (gateControl == 0) {
    gateServo2.write(0); // Close gate 2
    bot.sendMessage(CHAT_ID, "Gate 2 is CLOSED", "");
  } else if (gateControl == 1) {
    gateServo2.write(45); // Partially open gate 2
    bot.sendMessage(CHAT_ID, "Gate 2 is PARTIALLY OPEN", "");
  } else if (gateControl == 2) {
    gateServo2.write(90); // Fully open gate 2
    bot.sendMessage(CHAT_ID, "Gate 2 is FULLY OPEN", "");
  }
}

// Blynk function to read water level from V0
BLYNK_READ(V0) {
  int waterLevel = analogRead(waterSensorPin);
  float waterLevelPercentage = (waterLevel / 1023.0) * 100;
  Blynk.virtualWrite(V0, waterLevelPercentage);
}

void checkForMalfunctions(float waterLevelPercentage) {
  // Check for sensor malfunction
  if (millis() - lastSensorCheck > sensorCheckInterval) {
    lastSensorCheck = millis();
    if (waterLevelPercentage < minValidSensorValue || waterLevelPercentage > maxValidSensorValue) {
      bot.sendMessage(CHAT_ID, "Sensor Malfunction: Water level out of expected range!", "");
    }
  }

  // Add more checks if needed for gate malfunctions
  // Example: Check if gates are not moving or responding correctly
}

void handleSerialCommands() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("THRESHOLD1=")) {
      thresholdValue1 = command.substring(11).toFloat();
      Serial.println("Threshold 1 updated to " + String(thresholdValue1));
    } else if (command.startsWith("THRESHOLD2=")) {
      thresholdValue2 = command.substring(11).toFloat();
      Serial.println("Threshold 2 updated to " + String(thresholdValue2));
    } else if (command.startsWith("FLOODTHRESHOLD=")) {
      floodThreshold = command.substring(15).toFloat();
      Serial.println("Flood Threshold updated to " + String(floodThreshold));
    } else if (command.startsWith("LEDTHRESHOLD=")) {
      ledThreshold = command.substring(13).toFloat();
      Serial.println("LED Threshold updated to " + String(ledThreshold));
    } else if (command == "GATE1_CLOSE") {
      gateServo1.write(0);
      Serial.println("Gate 1 is CLOSED");
    } else if (command == "GATE1_PARTIAL") {
      gateServo1.write(45);
      Serial.println("Gate 1 is PARTIALLY OPEN");
    } else if (command == "GATE1_OPEN") {
      gateServo1.write(90);
      Serial.println("Gate 1 is FULLY OPEN");
    } else if (command == "GATE2_CLOSE") {
      gateServo2.write(0);
      Serial.println("Gate 2 is CLOSED");
    } else if (command == "GATE2_PARTIAL") {
      gateServo2.write(45);
      Serial.println("Gate 2 is PARTIALLY OPEN");
    } else if (command == "GATE2_OPEN") {
      gateServo2.write(90);
      Serial.println("Gate 2 is FULLY OPEN");
    } else {
      Serial.println("Unknown command");
    }
  }
}

void loop() {
  Blynk.run();
  handleSerialCommands();

  // Read water level sensor
  float waterLevelPercentage;

  if (testMode) {
    // Generate random water level values in test mode
    waterLevelPercentage = random(0, 101); // Random value between 0 and 100
  } else {
    // Read actual water level
    int waterLevel = analogRead(waterSensorPin);
    waterLevelPercentage = (waterLevel / 1023.0) * 100;
  }
  
  // Calculate volume in cubic centimeters (cm³)
  float radius = 7.5 / 2; // Radius in cm
  float volume = 3.14159 * radius * radius * (waterLevelPercentage / 100 * maxHeight);

  if (waterLevelPercentage >= floodThreshold) {
    // Send flood alert if water level is at or above floodThreshold
    bot.sendMessage(CHAT_ID, "Flood Alert: Water level is at or above " + String(floodThreshold) + "%!", "");
    digitalWrite(buzzerPin, HIGH); // Turn on the buzzer
  } else {
    digitalWrite(buzzerPin, LOW); // Turn off the buzzer if water level is below floodThreshold
  }

  String gateStatus1 = "CLOSED";
  String gateStatus2 = "CLOSED";
  int gateControl1 = 0; // For V2
  int gateControl2 = 0; // For V3

  if (waterLevelPercentage >= thresholdValue2) {
    // Open both gates
    gateServo1.write(90); // Fully open gate 1
    gateServo2.write(90); // Fully open gate 2
    gateStatus1 = "FULLY OPEN";
    gateStatus2 = "FULLY OPEN";
    gateControl1 = 2; // Fully open gate 1
    gateControl2 = 2; // Fully open gate 2
  } else if (waterLevelPercentage >= thresholdValue1) {
    // Open gate 1
    gateServo1.write(90); // Fully open gate 1
    gateServo2.write(0);  // Close gate 2
    gateStatus1 = "FULLY OPEN";
    gateStatus2 = "CLOSED";
    gateControl1 = 2; // Fully open gate 1
    gateControl2 = 0; // Close gate 2
  } else {
    // Close both gates
    gateServo1.write(0); // Close gate 1
    gateServo2.write(0); // Close gate 2
    gateStatus1 = "CLOSED";
    gateStatus2 = "CLOSED";
    gateControl1 = 0; // Close gate 1
    gateControl2 = 0; // Close gate 2
  }

  // Turn on the LED if the water level is above the ledThreshold
  if (waterLevelPercentage > ledThreshold) {
    Blynk.virtualWrite(V4, 255); // Turn on the LED
  } else {
    Blynk.virtualWrite(V4, 0); // Turn off the LED
  }

  // Test cases for Blynk virtual pin V1
  if (testMode && !testExecuted) {
    // Check if test interval has passed
    if (millis() - testStartTime >= testInterval) {
      // Reset to initial stage
      testMode = false;
      testExecuted = true; // Mark test as executed
      Blynk.virtualWrite(V1, 0); // Close both gates
      bot.sendMessage(CHAT_ID, "Test case completed. Both Gates are CLOSED", "");
    }
  }

  // Send status to Telegram every 20 seconds
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    String message = "Water Level: " + String(waterLevelPercentage, 2) + "%\n" +
                     "Gate 1: " + gateStatus1 + "\n" +
                     "Gate 2: " + gateStatus2 + "\n" +
                     "Volume: " + String(volume, 2) + " cm³";
    bot.sendMessage(CHAT_ID, message, "");

    // Send water level to Blynk virtual pin V1
    Blynk.virtualWrite(V1, waterLevelPercentage);
  }

  // Update the water level gauge in Blynk
  Blynk.virtualWrite(V0, waterLevelPercentage);

  // Update gate control sliders in Blynk
  Blynk.virtualWrite(V2, gateControl1);
  Blynk.virtualWrite(V3, gateControl2);

  // Check for malfunctions
  checkForMalfunctions(waterLevelPercentage);

  delay(1000); // Wait for a second before the next measurement
} 