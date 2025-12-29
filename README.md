# OLED display

Uses ESP32 and BME280 to display data:
- temperature
- pressure
- humidity
- date and time

## Configuration

### WiFi and other settings
For security, WiFi credentials and other settings are separated into a configuration file:

1. Copy `config_template.h` as `config.h`
2. Edit WiFi credentials in `config.h`:
   ```cpp
   #define WIFI_SSID "Your_WiFi_SSID"
   #define WIFI_PASSWORD "Your_WiFi_Password"
   ```

### config.h example
```cpp
// WiFi settings
#define WIFI_SSID "MyWiFi"
#define WIFI_PASSWORD "MyPassword"

// NTP settings
#define NTP_SERVER "pool.ntp.org"
#define TIME_OFFSET 3600  // GMT+1
