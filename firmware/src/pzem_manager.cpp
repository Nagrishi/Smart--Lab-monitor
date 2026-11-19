
#include <Arduino.h>
#include <MycilaPZEM.h>

#include "config.h"
#include "pzem_manager.h"

static Mycila::PZEM pzem;

static SensorData *sensorData = nullptr;

void pzemManagerInit(SensorData &data) {

    sensorData = &data;

    pzem.begin(
        Serial2,
        PZEM_RX_PIN,
        PZEM_TX_PIN,
        PZEM_ADDRESS,
        true
    );

    pzem.setCallback(
        [](const Mycila::PZEM::EventType event,
           const Mycila::PZEM::Data &reading) {

            if (event != Mycila::PZEM::EventType::EVT_READ) {
                return;
            }

            if (sensorData == nullptr) {
                return;
            }

            sensorData->voltage = reading.voltage;
            sensorData->current = reading.current;
            sensorData->power = reading.activePower;
            sensorData->energy = reading.activeEnergy;
            sensorData->frequency = reading.frequency;
            sensorData->powerFactor = reading.powerFactor;

            sensorData->pzemValid = true;

            Serial.println("\n--- PZEM DATA ---");

            Serial.printf("Voltage: %.2f V\n", reading.voltage);
            Serial.printf("Current: %.3f A\n", reading.current);
            Serial.printf("Power: %.2f W\n", reading.activePower);
            Serial.printf("Energy: %.4f kWh\n", reading.activeEnergy);
            Serial.printf("Frequency: %.2f Hz\n", reading.frequency);
            Serial.printf("Power Factor: %.2f\n", reading.powerFactor);
        }
    );

    Serial.println("PZEM initialized");
}
