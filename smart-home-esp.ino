#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiMulti.h>

#include "ESPAsyncWebServer.h"

#include <SPI.h>
#include <Wire.h>

#include "const.h"
#include "DHT.h"

#define DHTTYPE DHT22

// praha izba pin 15
// praha vonku 23
uint8_t DHTPin = 15;
DHT dht(DHTPin, DHTTYPE);

AsyncWebServer server(80);

float temperature;
float humidity;

unsigned long lastTime = 0;
// 15 m = 900000
unsigned long timerDelay = 900000;

void connectToWiFi() {
  WiFi.mode(WIFI_STA);  // Optional
  WiFi.begin(SSID, PASSWORD);
  Serial.println("\nConnecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.println(WiFi.localIP());

  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    JsonDocument json;
    json["temperature"] = temperature;
    json["humidity"] = humidity;
    serializeJson(json, *response);
    request->send(response);
  });

  server.begin();
}

void getData() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
}

void sendData() {
  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, RELAY_IP);

    http.addHeader("Content-Type", "application/json");

    String httpRequestData = "{\"device\":\"" + String(DEVICE_ID) + "\",\"temperature\":\"" + String(temperature) + "\",\"humidity\":\"" + String(humidity) + "\"}";
    int httpResponseCode = http.POST(httpRequestData);

    Serial.println(httpResponseCode);
    // Free resources
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  connectToWiFi();
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    getData();
    sendData();

    Serial.print(F("Humidity: "));
    Serial.print(humidity);
    Serial.print(F("%  Temperature: "));
    Serial.print(temperature);
    Serial.println(F("°C "));

    lastTime = millis();
  }
}
