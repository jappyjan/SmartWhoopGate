#include <Arduino.h>
#include <EEPROM.h>

#include "../../src/const.h"
#include "wlan.h"

#if defined(ARDUINO_ARCH_ESP8266)
    #include <ESP8266WiFi.h>
#endif

#if defined(ARDUINO_ARCH_ESP32)
    #include <WiFi.h>
#endif

char* WLAN::currentSSID = 0;
char* WLAN::currentPassword = 0;
unsigned long WLAN::testStartTime = 0;

#define DISCONNECTED 0
#define CONNECTING 1
#define CONNECTED 2

/**
 * 0 = disconnected
 * 1 = connecting / testing connection
 * 2 = connected
*/
uint8_t WLAN::state = DISCONNECTED;

void WLAN::setup() {
    Serial.println("WLAN::setup -> start");

    for (int i = WIFI_SSID_EEPROM_START_POS; i < WIFI_SSID_EEPROM_END_POS + 1; ++i)
    {
        WLAN::currentSSID += char(EEPROM.read(i));
    }

    for (int i = WIFI_PASSWORD_EEPROM_START_POS; i < WIFI_PASSWORD_EEPROM_END_POS + 1; ++i)
    {
        WLAN::currentPassword += char(EEPROM.read(i));
    }

    Serial.println("Wifi Credentials");
    Serial.println(WLAN::currentSSID);
    Serial.println(WLAN::currentPassword);

    WLAN::connect();
    Serial.println("WLAN::setup -> done");
}

void WLAN::loop()
{
    WLAN::testConnection();
}

void WLAN::connect()
{
    Serial.println("WLAN::connect -> start");
    if ((WLAN::currentSSID != NULL) && (WLAN::currentSSID[0] == '\0')) {
        Serial.println("WLAN::connect -> No SSID given");
        return;
    }

    #if defined(ARDUINO_ARCH_ESP8266)
        WLAN::connectEsp8266();
    #endif

    #if defined(ARDUINO_ARCH_ESP32)
        WLAN::connectEsp32();
    #endif

    WLAN::state = 1;
    WLAN::testStartTime = millis();

    Serial.println("WLAN::connect -> done");
}

void WLAN::connectEsp8266() {
    #if defined(ARDUINO_ARCH_ESP8266)
        WiFi.disconnect();
        WiFi.begin(WLAN::currentSSID, WLAN::currentPassword);
    #endif
}

void WLAN::connectEsp32() {
    #if defined(ARDUINO_ARCH_ESP32)
        WiFi.disconnect();
        WiFi.begin(WLAN::currentSSID, WLAN::currentPassword);
    #endif
}

void WLAN::updateWifiStatus() {
    WLAN::state = 1;

    #if defined(ARDUINO_ARCH_ESP8266)
        WLAN::updateEsp8266WifiStatus();
    #endif
    
    #if defined(ARDUINO_ARCH_ESP32)
        WLAN::updateEsp32WifiStatus();
    #endif
    
}

void WLAN::updateEsp32WifiStatus() {
    #if defined(ARDUINO_ARCH_ESP32)

    #endif
}

void WLAN::updateEsp8266WifiStatus() {
    #if defined(ARDUINO_ARCH_ESP8266)
        wl_status_t status = WiFi.status();

        if (status == WL_CONNECTED) {
            WLAN::state = 2;
        }

        if (status == WL_DISCONNECTED) {
            WLAN::state = 0;
        }
    #endif
}

void WLAN::testConnection()
{
    uint8_t previousState = WLAN::state;

    WLAN::updateWifiStatus();

    if (WLAN::state == previousState) {
        // nothing happened
        return;
    }

    unsigned long testDuration = millis() - WLAN::testStartTime;

    if (WLAN::state == CONNECTED) {
        Serial.println("Wifi connected!");
        return;
    }

    if (WLAN::state == CONNECTING) {
        Serial.println("Waiting for Wifi to connect");

        if (testDuration > 15000) {
            Serial.println("WiFi Connection not established after 15 seconds. Please check credentials...");
            WLAN::testStartTime = millis();
        }
    }

    return;
}

void WLAN::setSSID(char* ssid) {
    WLAN::currentSSID = ssid;

    for (int i = WIFI_SSID_EEPROM_START_POS; i < WIFI_SSID_EEPROM_END_POS + 1; ++i)
    {
        EEPROM.put(i, WLAN::currentSSID[i]);
    }
    EEPROM.commit();

    WLAN::connect();
}

void WLAN::setPassword(char* password) {
    WLAN::currentPassword = password;

    for (int i = WIFI_PASSWORD_EEPROM_START_POS; i < WIFI_PASSWORD_EEPROM_END_POS + 1; ++i)
    {
        EEPROM.put(i, WLAN::currentPassword[i]);
    }
    EEPROM.commit();

    Serial.println("WLAN::setPassword -> Password set");    

    WLAN::connect();
}