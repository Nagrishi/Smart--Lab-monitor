
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

#include "config.h"
#include "oled_manager.h"
#include "wifi_manager.h"

// ==========================================
// OLED CONFIGURATION
// ==========================================

static Adafruit_SSD1306 display(
    OLED_WIDTH,
    OLED_HEIGHT,
    &Wire,
    -1
);

static bool displayReady = false;

static const unsigned long PAGE_INTERVAL = 3500;

static const uint8_t PAGE_COUNT = 6;

// ==========================================
// COMMON DISPLAY FUNCTIONS
// ==========================================

static void drawHeader(const char *title, uint8_t page) {

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // Page title
    display.setCursor(0, 0);
    display.print(title);

    // Live Wi-Fi status
    display.setCursor(76, 0);

    if (wifiManagerIsConnected()) {
        display.print("W:ON");
    } else {
        display.print("W:OFF");
    }

    // Page number
    display.setCursor(108, 0);
    display.print(page + 1);
    display.print("/6");

    // Header separator
    display.drawFastHLine(0, 10, 128, SSD1306_WHITE);
}

static void drawFooter(const char *message) {

    display.drawFastHLine(0, 55, 128, SSD1306_WHITE);

    display.setCursor(2, 57);
    display.print(message);
}

static void drawValue(
    const char *label,
    float value,
    const char *unit,
    bool valid,
    int y
) {

    display.setCursor(2, y);
    display.print(label);

    display.setCursor(65, y);

    if (valid && !isnan(value)) {

        display.print(value, 1);
        display.print(unit);

    } else {

        display.print("--");
    }
}

// ==========================================
// CHIBI ROBOT ANIMATION
// ==========================================

static void drawChibiRobot() {

    unsigned long now = millis();

    // Small floating animation
    int floatOffset = (now / 400) % 2;

    // Eyes blink periodically
    bool blink = (now % 2200 < 160);

    int y = 19 + floatOffset;

    // Robot body / face
    display.drawRoundRect(
        7, y, 45, 32, 9,
        SSD1306_WHITE
    );

    // Antenna
    display.drawLine(
        29, y, 29, y - 5,
        SSD1306_WHITE
    );

    display.fillCircle(
        29, y - 6, 2,
        SSD1306_WHITE
    );

    // Ears
    display.drawRoundRect(
        3, y + 10, 5, 10, 2,
        SSD1306_WHITE
    );

    display.drawRoundRect(
        51, y + 10, 5, 10, 2,
        SSD1306_WHITE
    );

    // Eyes
    if (blink) {

        display.drawFastHLine(
            17, y + 13, 7,
            SSD1306_WHITE
        );

        display.drawFastHLine(
            35, y + 13, 7,
            SSD1306_WHITE
        );

    } else {

        display.fillCircle(
            20, y + 13, 3,
            SSD1306_WHITE
        );

        display.fillCircle(
            38, y + 13, 3,
            SSD1306_WHITE
        );
    }

    // Cute smile
    display.drawPixel(28, y + 23, SSD1306_WHITE);
    display.drawPixel(29, y + 24, SSD1306_WHITE);
    display.drawPixel(30, y + 23, SSD1306_WHITE);

    // Cheeks
    display.drawPixel(13, y + 20, SSD1306_WHITE);
    display.drawPixel(45, y + 20, SSD1306_WHITE);
}

// ==========================================
// PAGE 1: HOME
// ==========================================

static void drawHome(const SensorData &data) {

    drawHeader("LAB MONITOR", 0);

    drawChibiRobot();

    display.setCursor(61, 18);
    display.print("HELLO!");

    display.setCursor(61, 29);
    display.print("TEMP:");

    if (data.dhtValid) {

        display.print(data.temperature, 1);
        display.print("C");

    } else {

        display.print("--");
    }

    display.setCursor(61, 41);
    display.print("STATUS:");

    bool alarm =
        (data.dhtValid &&
         data.temperature >= TEMP_ALARM_LIMIT) ||
        (data.presenceValid && !data.presence);

    display.print(alarm ? "ALERT" : "SAFE");

    drawFooter("SMART LAB MONITOR");
}

// ==========================================
// PAGE 2: ENVIRONMENT
// ==========================================

static void drawEnvironment(const SensorData &data) {

    drawHeader("ENVIRONMENT", 1);

    drawValue(
        "TEMP:",
        data.temperature,
        " C",
        data.dhtValid,
        17
    );

    drawValue(
        "HUMIDITY:",
        data.humidity,
        " %",
        data.dhtValid,
        30
    );

    display.setCursor(2, 43);
    display.print("LIMIT: 45 C");

    drawFooter(
        data.dhtValid ? "DHT11 ONLINE" : "DHT11 ERROR"
    );
}

// ==========================================
// PAGE 3: ELECTRICAL
// ==========================================

static void drawElectrical(const SensorData &data) {

    drawHeader("ELECTRICAL", 2);

    drawValue(
        "VOLTAGE:",
        data.voltage,
        " V",
        data.pzemValid,
        16
    );

    drawValue(
        "CURRENT:",
        data.current,
        " A",
        data.pzemValid,
        29
    );

    drawValue(
        "POWER:",
        data.power,
        " W",
        data.pzemValid,
        42
    );

    drawFooter(
        data.pzemValid ? "PZEM ONLINE" : "PZEM WAITING"
    );
}

// ==========================================
// PAGE 4: ENERGY
// ==========================================

static void drawEnergy(const SensorData &data) {

    drawHeader("ENERGY", 3);

    drawValue(
        "ENERGY:",
        data.energy,
        "Wh",
        data.pzemValid,
        15
    );

    drawValue(
        "FREQ:",
        data.frequency,
        "Hz",
        data.pzemValid,
        28
    );

    drawValue(
        "PF:",
        data.powerFactor,
        "",
        data.pzemValid,
        41
    );

    drawFooter("PZEM-004T");
}

// ==========================================
// PAGE 5: SECURITY
// ==========================================

static void drawSecurity(const SensorData &data) {

    drawHeader("SECURITY", 4);

    display.setCursor(2, 16);
    display.print("PRESENCE:");

    display.setCursor(75, 16);

    if (data.presenceValid) {

        display.print(data.presence ? "YES" : "NO");

    } else {

        display.print("WAIT");
    }

    // Same conditions as buzzer_manager.cpp
    bool alarm =
        (data.dhtValid &&
         data.temperature >= TEMP_ALARM_LIMIT) ||
        (data.presenceValid && !data.presence);

    display.setCursor(2, 31);
    display.print("BUZZER:");

    display.setCursor(75, 31);
    display.print(alarm ? "ON" : "OFF");

    display.setCursor(2, 44);
    display.print("TEMP LIMIT: 45C");

    drawFooter(
        alarm ? "WARNING ACTIVE" : "SYSTEM NORMAL"
    );
}

// ==========================================
// PAGE 6: MQ-2 GAS SENSOR
// ==========================================

static void drawGas() {

    drawHeader("GAS MONITOR", 5);

    display.setCursor(5, 18);
    display.print("MQ-2 SENSOR");

    display.setCursor(5, 32);
    display.print("NOT TESTED");

    display.setCursor(5, 44);
    display.print("ADC DIAGNOSIS");

    drawFooter("WAITING FOR TEST");
}

// ==========================================
// OLED INITIALIZATION
// ==========================================

bool oledManagerInit() {

    Wire.begin(OLED_SDA, OLED_SCL);

    if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        OLED_ADDRESS
    )) {

        Serial.println("OLED initialization failed");

        return false;
    }

    displayReady = true;

    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    display.setCursor(10, 20);
    display.println("SMART LAB MONITOR");

    display.setCursor(25, 38);
    display.println("BOOTING...");

    display.display();

    Serial.println("OLED initialized successfully");

    return true;
}

// ==========================================
// MAIN OLED UPDATE
// ==========================================

void oledManagerUpdate(const SensorData &data) {

    if (!displayReady) {
        return;
    }

    // Automatically rotate through six pages
    uint8_t currentPage =
        (millis() / PAGE_INTERVAL) % PAGE_COUNT;

    display.clearDisplay();

    switch (currentPage) {

        case 0:
            drawHome(data);
            break;

        case 1:
            drawEnvironment(data);
            break;

        case 2:
            drawElectrical(data);
            break;

        case 3:
            drawEnergy(data);
            break;

        case 4:
            drawSecurity(data);
            break;

        case 5:
            drawGas();
            break;
    }

    display.display();
}
