#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <const.h>
#include "wlan.h"

String WLAN::currentSSID = "";
String WLAN::currentPassword = "";
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
    WLAN::currentSSID.trim();

    for (int i = WIFI_PASSWORD_EEPROM_START_POS; i < WIFI_PASSWORD_EEPROM_END_POS + 1; ++i)
    {
        WLAN::currentPassword += char(EEPROM.read(i));
    }
    WLAN::currentPassword.trim();

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
    if (WLAN::currentSSID == "") {
        Serial.println("WLAN::connect -> No SSID given");
        return;
    }

    WiFi.disconnect();
    WiFi.begin(WLAN::currentSSID, WLAN::currentPassword);

    WLAN::state = 1;
    WLAN::testStartTime = millis();

    Serial.println("WLAN::connect -> done");
}

void WLAN::testConnection()
{
    wl_status_t status = WiFi.status();

    uint8_t previousState = WLAN::state;

    WLAN::state = 1;
    if (status == WL_CONNECTED) {
        WLAN::state = 2;
    }

    if (status == WL_DISCONNECTED) {
        WLAN::state = 0;
    }

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

void WLAN::setSSID(String ssid) {
    WLAN::currentSSID = ssid;

    for (int i = WIFI_SSID_EEPROM_START_POS; i < WIFI_SSID_EEPROM_END_POS + 1; ++i)
    {
        EEPROM.put(i, WLAN::currentSSID.charAt(i));
    }
    EEPROM.commit();

    WLAN::connect();
}

void WLAN::setPassword(String password) {
    WLAN::currentPassword = password;

    for (int i = WIFI_PASSWORD_EEPROM_START_POS; i < WIFI_PASSWORD_EEPROM_END_POS + 1; ++i)
    {
        EEPROM.put(i, WLAN::currentPassword.charAt(i));
    }
    EEPROM.commit();

    Serial.println("WLAN::setPassword -> Password set");    

    WLAN::connect();
}