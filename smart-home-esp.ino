#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESPAsyncWebSrv.h>
#include <Arduino.h>
#include <ArduinoJson.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "const.h"
#include "DHT.h"

#define DHTTYPE DHT22

uint8_t DHTPin = 12;
DHT dht(DHTPin, DHTTYPE);

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

AsyncWebServer server(80);

float temperature;
float humidity;

unsigned long lastTime = 0;
// 30 s
unsigned long timerDelay = 30000;

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
    DynamicJsonDocument json(1024);
    json["temperature"] = temperature;
    json["humidity"] = humidity;
    serializeJson(json, *response);
    request->send(response);
  });

  server.begin();
}


void setupDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  display.display();
  delay(100);  // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  display.display();
  display.setTextSize(1.75);
  display.setTextColor(WHITE);
}

void displayData() {
  display.clearDisplay();
  delay(100);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("tmp: ");
  display.print(temperature, 1);
  display.print(" ");

  display.setCursor(0, 17);
  display.print("hum: ");
  display.print(humidity, 1);
  display.print(" ");

  display.display();
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
  setupDisplay();
  connectToWiFi();
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    getData();
    displayData();
    sendData();

    Serial.print(F("Humidity: "));
    Serial.print(humidity);
    Serial.print(F("%  Temperature: "));
    Serial.print(temperature);
    Serial.println(F("°C "));

    lastTime = millis();
  }
}