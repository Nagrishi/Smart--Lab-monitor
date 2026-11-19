
#pragma once

#include "sensor_data.h"

// Initialize DHT11
void dhtManagerInit();

// Read temperature and humidity
void dhtManagerUpdate(SensorData &data);
