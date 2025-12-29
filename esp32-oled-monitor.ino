/*
* Example code for ESP32-C3_OLED_kit 
*
* ESP32-C3-LPKit reads data from:
* * BME280: It shows the pressure, temperature and humidity
* on OLED display
*
* Hardware:
* LaskaKit ESP32-C3-LPKit                                   - https://www.laskakit.cz/laskkit-esp-12-board/
* LaskaKit BME280 Senzor tlaku, teploty a vlhkosti vzduchu  - https://www.laskakit.cz/arduino-senzor-tlaku--teploty-a-vlhkosti-bme280/
* LaskaKit OLED displej 128x64 1.3" I²C                     - https://www.laskakit.cz/laskakit-oled-displej-128x64-1-3--i2c/
*
* Library:
* https://github.com/adafruit/Adafruit_BME280_Library
* https://github.com/adafruit/Adafruit_SH110x
*
* CONFIGURATION:
* Copy config_template.h to config.h and fill in your WiFi credentials
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Math.h>
#include <time.h>

// Include configuration (if config.h doesn't exist, compilation will fail with clear error)
#include "config.h"

// NTPClient
#include <NTPClient.h>
NTPClient ntpClient;

// WiFi
#include <WiFi.h>

// Sensor
#include <Adafruit_BME280.h> 
Adafruit_BME280 bme;

// fonts
#include "Roboto_Mono_Bold_12px.h"

Adafruit_SH1106G display = Adafruit_SH1106G(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, -1);

int co2 = 0;
float temperature = 0.0;
int pressure = 0;
int humidity = 0;

void setup() {
  
  // Speed of Serial
  Serial.begin(SERIAL_BAUDRATE);
  pinMode(POWER_PIN, OUTPUT); 
  digitalWrite(POWER_PIN, HIGH); // enable power supply for uSup
  delay(500);   

  Serial.println("Setup start");
  // set dedicated I2C pins from config
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); // SDA, SCL pins from config

  delay(250);                        // wait for the OLED to power up
  display.begin(OLED_ADDRESS, true);
  //display.setContrast (0); // dim display

  // Clear the buffer.
  display.clearDisplay();

  unsigned status;
  status = bme.begin();  
  if (!status) {
      Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
      while (1) delay(10);
  }

  // WiFi connection
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ...");
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  Serial.println();
  Serial.println("WiFi connected.");

  // Setup NTP Client with DST rules for Czech Republic
  ntpClient.setTimeOffset(TIME_OFFSET);           // Base offset from config
  ntpClient.addDSTRule(29, 3, 2026, TIME_OFFSET); // Last Sunday in March, 2:00 AM -> 3:00 AM (GMT+2)
  ntpClient.addDSTRule(25, 10, 2026, 0);          // Last Sunday in October, 3:00 AM -> 2:00 AM (GMT+1)
  ntpClient.addDSTRule(28, 3, 2027, TIME_OFFSET); // DST starts 2027
  ntpClient.addDSTRule(31, 10, 2027, 0);          // DST ends 2027
  ntpClient.addDSTRule(27, 3, 2028, TIME_OFFSET); // DST starts 2028
  ntpClient.addDSTRule(29, 10, 2028, 0);          // DST ends 2028

  Serial.println("Setup done.");
}

void loop() {
  ntpClient.update();

  temperature = bme.readTemperature();
  humidity    = bme.readHumidity();
  pressure    = bme.readPressure() / 100.0F;  

  Serial.println();
  Serial.print("Pressure:\t");
  Serial.print(pressure);
  Serial.println(" kPa");
  Serial.print("Temperature:\t");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("Humidity:\t");
  Serial.print(humidity);
  Serial.println("% rH");
  Serial.print("Time:\t");
  Serial.print(ntpClient.getFormattedTime());
  Serial.println("");
  Serial.print("Date:\t");
  Serial.print(ntpClient.getFormattedDate());
  Serial.println("");

  display.clearDisplay();

  // Date
  display.setFont(&Roboto_Mono_Bold_12);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(20, 14);
  display.println(ntpClient.getFormattedDate());

  // Time
  display.setFont(&Roboto_Mono_Bold_12);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(30, 30);
  display.println(ntpClient.getFormattedTime());

  // Pressure
  display.setFont(&Roboto_Mono_Bold_12);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(40, 46);
  display.print(pressure);
  display.println("kPa");
  
  // Temperature
  display.setFont(&Roboto_Mono_Bold_12);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(20, 64);
  display.print((round(temperature * 10) / 10.0), 1);
  display.println("C");
  
  // Humidity
  display.setFont(&Roboto_Mono_Bold_12);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(80, 64);
  display.print(humidity);
  display.println("%");

  // update display
  display.display();
  
  delay(1000);
}
