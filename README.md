# smart-home-esp

ESP32 rest server providing DHT22 temperature and humidity in json.

## initialization

Create `const.h` with:

        #define SSID "ssid"
        #define PASSWORD "passwd"
        #define RELAY_IP "[http://addr:port]"
        #define DEVICE_ID "device id as stored in db"
