
#pragma once

#include "sensor_data.h"

// Initialize mmWave sensor
void mmwaveManagerInit();

// Read human presence status
void mmwaveManagerUpdate(SensorData &data);
