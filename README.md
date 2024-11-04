# smart-home-esp

ESP32 rest server providing DHT22 temperature and humidity in json.

# Arduino ide setup

preferences -> additional board managers: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
tools -> board -> esp32 dev module
tools -> manage libraries isntall based on includes

## initialization

set DHTPin value based on your esp.

// praha izba pin 15
// praha vonku 23

Relay has default port 4203.
Ssid and passwd are for wifi.
Device id is unique identifier for storage of values can be any string.

Create `const.h` with:

        #define SSID "ssid"
        #define PASSWORD "passwd"
        #define RELAY_IP "[http://addr:port]"
        #define DEVICE_ID "device id as stored in db"
