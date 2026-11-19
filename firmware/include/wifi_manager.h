
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

// Initialize Wi-Fi connection
void wifiManagerInit();

// Maintain Wi-Fi connection and reconnect if disconnected
void wifiManagerUpdate();

// Check whether ESP32 is connected
bool wifiManagerIsConnected();

#endif
