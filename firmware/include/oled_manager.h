

#pragma once

#include "sensor_data.h"
#include "wifi_manager.h"

// Initialize SSD1306 OLED
bool oledManagerInit();

// Update animated OLED interface
void oledManagerUpdate(const SensorData &data);
