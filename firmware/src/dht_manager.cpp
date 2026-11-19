
#include <Arduino.h>
#include <DHT.h>

#include "config.h"
#include "dht_manager.h"

static DHT dht(DHT_PIN, DHT11);

void dhtManagerInit() {

    dht.begin();

    Serial.println("DHT11 initialized");
}

void dhtManagerUpdate(SensorData &data) {

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {

        data.dhtValid = false;

        Serial.println("DHT11 read error");

        return;
    }

    data.temperature = temperature;
    data.humidity = humidity;
    data.dhtValid = true;
}
