
#pragma once

// ===============================
// SMART LAB MONITOR CONFIGURATION
// ESP32-WROOM-32
// ===============================

// DHT11
#define DHT_PIN 16

// OLED SSD1306
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_ADDRESS 0x3C

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

// mmWave sensor
#define MMWAVE_PIN 34
#define MMWAVE_ACTIVE_STATE HIGH

// BUZZER
#define BUZZER_PIN 25
#define BUZZER_ON HIGH
#define BUZZER_OFF LOW

// Temperature alarm threshold
#define TEMP_ALARM_LIMIT 45.0

// PZEM-004T
#define PZEM_RX_PIN 26
#define PZEM_TX_PIN 27
#define PZEM_ADDRESS 0x01

// Serial communication
#define SERIAL_BAUD 115200

