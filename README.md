# smart-home-esp

ESP32 rest server providing DHT22 temperature and humidity in json.

## initialization

Create `const.h` with:

        #define SSID "ssid"
        #define PASSWORD "passwd"
        #define SENSOR_ID 0
        #define RELAY_IP "relay address"
