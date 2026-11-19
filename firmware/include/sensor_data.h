
#pragma once

#include <Arduino.h>
#include <math.h>

// Central data structure for Smart Lab Monitor

struct SensorData {

    // DHT11 sensor
    float temperature = NAN;
    float humidity = NAN;
    bool dhtValid = false;

    // mmWave sensor
    bool presence = false;
    bool presenceValid = false;

    // PZEM-004T sensor
    float voltage = NAN;
    float current = NAN;
    float power = NAN;
    float energy = NAN;
    float frequency = NAN;
    float powerFactor = NAN;

    bool pzemValid = false;
};
