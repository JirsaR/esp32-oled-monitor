/*
 * Configuration file template
 * 
 * Copy this file as "config.h" an fill with correct values
 */

#pragma once

// WiFi settings
#define WIFI_SSID "WiFi_SSID"
#define WIFI_PASSWORD "WiFi_Password"

// OLED settings
#define OLED_ADDRESS 0x3c

// NTP settings  
#define NTP_SERVER "pool.ntp.org"
#define TIME_OFFSET 3600

// Power pin for ESP32-C3-LPKit
#define POWER_PIN 4

// I2C pins for ESP32-C3-LPKit v3
#define I2C_SDA_PIN 8
#define I2C_SCL_PIN 10

// Display settings
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

// Serial baudrate
#define SERIAL_BAUDRATE 9600