#include <ESP8266WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <Arduino.h>
#include <ArduinoJson.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "const.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

IPAddress local_IP(192, 168, 1, IPADDR);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

AsyncWebServer server(80);

// JSON data buffer
StaticJsonDocument<250> jsonDocument;
char buffer[250];


void testscrolltext(int temp, int hum) {
  delay(1000);
  display.clearDisplay();
  delay(1000);

  display.setTextSize(4); 
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(F("tem: 20C hum: 50%"));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.clearDisplay();
}

void setupDisplay() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
}

void connectToWiFi() {  
  WiFi.mode(WIFI_STA); // Optional
  WiFi.begin(SSID, PASSWORD);
  Serial.println("\nConnecting");

  while (WiFi.status() != WL_CONNECTED)
  {
      Serial.print(".");
      delay(500);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.println(WiFi.localIP());
}

void setup()
{
    Serial.begin(115200);

    setupDisplay();

    connectToWiFi();
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        DynamicJsonDocument json(1024);
        json["temperature"] = 20;
        json["humidity"] = 50;
        serializeJson(json, *response);
        
        delay(1000);
        display.clearDisplay();
        delay(1000);

        display.setTextSize(4); 
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println(F("20C 50%"));
        delay(1000);
        display.clearDisplay();
        request->send(response);
    });

    server.begin();
    testscrolltext(1,2);
}


void loop() {
  
}