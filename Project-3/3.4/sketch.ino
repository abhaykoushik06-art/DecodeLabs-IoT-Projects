#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// --------------------
// Ultrasonic pins
// --------------------
#define TRIG_PIN 5
#define ECHO_PIN 18

// --------------------
// Wi-Fi
// --------------------
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6

// --------------------
// Adafruit IO MQTT
// --------------------
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883

#define AIO_USERNAME "name"
#define AIO_KEY "password"

// --------------------
// MQTT client
// --------------------
WiFiClient client;

Adafruit_MQTT_Client mqtt(
  &client,
  AIO_SERVER,
  AIO_SERVERPORT,
  AIO_USERNAME,
  AIO_KEY
);

// --------------------
// Adafruit IO feeds
// --------------------
Adafruit_MQTT_Publish distanceFeed =
  Adafruit_MQTT_Publish(
    &mqtt,
    AIO_USERNAME "/feeds/distance"
  );

Adafruit_MQTT_Publish statusFeed =
  Adafruit_MQTT_Publish(
    &mqtt,
    AIO_USERNAME "/feeds/security-status"
  );

// --------------------
// MQTT connection
// --------------------
void MQTT_connect() {

  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to Adafruit IO MQTT... ");

  uint8_t retries = 3;

  while (mqtt.connect() != 0) {

    Serial.println(mqtt.connectErrorString(mqtt.connect()));

    mqtt.disconnect();

    delay(5000);

    if (--retries == 0) {
      Serial.println("MQTT connection failed.");
      return;
    }
  }

  Serial.println("MQTT Connected!");
}

// --------------------
// Setup
// --------------------
void setup() {

  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Connect to Wi-Fi
  Serial.println("Cloud Security Node Starting...");
  Serial.print("Connecting to WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected!");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Connect to Adafruit IO
  MQTT_connect();
}

// --------------------
// Main loop
// --------------------
void loop() {

  // Make sure MQTT is connected
  MQTT_connect();

  // Trigger ultrasonic sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure echo
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate distance
  float distance = duration * 0.0343 / 2;

  // Determine security status
  String status;

  if (distance < 50) {
    status = "INTRUDER DETECTED";
  }
  else if (distance <= 100) {
    status = "WARNING";
  }
  else {
    status = "SAFE";
  }

  // Print to Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm | Status: ");
  Serial.println(status);

  // Publish distance
  if (!distanceFeed.publish(distance)) {
    Serial.println("Distance publish failed!");
  }
  else {
    Serial.println("Distance published.");
  }

  // Publish security status
  if (!statusFeed.publish(status.c_str())) {
    Serial.println("Status publish failed!");
  }
  else {
    Serial.println("Status published.");
  }

  delay(2000);
}