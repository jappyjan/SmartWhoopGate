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

    WLAN::currentSSID.clear();
    for (int i = WIFI_SSID_EEPROM_START_POS; i < WIFI_SSID_EEPROM_END_POS + 1; ++i)
    {
        WLAN::currentSSID.concat(char(EEPROM.read(i)));
    }
    WLAN::currentSSID.trim();

    WLAN::currentPassword.clear();
    for (int i = WIFI_PASSWORD_EEPROM_START_POS; i < WIFI_PASSWORD_EEPROM_END_POS + 1; ++i)
    {
        WLAN::currentPassword.concat(char(EEPROM.read(i)));
    }
    WLAN::currentPassword.trim();

    #ifdef WIFI_SET_SSID
        WLAN::setSSID(WIFI_SET_SSID);
    #endif
    
    #ifdef WIFI_SET_PASSWORD
        WLAN::setPassword(WIFI_SET_PASSWORD);
    #endif

    Serial.println("Wifi Credentials");
    Serial.println(WLAN::currentSSID);
    Serial.println(WLAN::currentPassword);

    WLAN::connect();
    Serial.println("WLAN::setup -> done");

    #if defined(ARDUINO_ARCH_ESP8266)
        WiFi.onEvent(WLAN::onESP8266WiFiEvent);
    #endif
}

#if defined(ARDUINO_ARCH_ESP8266)
    void WLAN::onESP8266WiFiEvent(WiFiEvent_t event) {
        Serial.printf("[WiFi-event] event: %d\n", event);

        switch(event) {
            case WIFI_EVENT_STAMODE_CONNECTED:
                Serial.println("WiFi connected");
            case WIFI_EVENT_STAMODE_DHCP_TIMEOUT:
                Serial.println("WiFi DHCP timeout");
            case WIFI_EVENT_STAMODE_GOT_IP:
                Serial.println("WiFi got IP address");
                Serial.println("IP address: ");
                Serial.println(WiFi.localIP());
                break;
            case WIFI_EVENT_STAMODE_DISCONNECTED:
                Serial.println("WiFi lost connection");
                break;
        }
    }
#endif

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

    #if defined(ARDUINO_ARCH_ESP8266)
        WLAN::connectEsp8266();
    #endif

    #if defined(ARDUINO_ARCH_ESP32)
        WLAN::connectEsp32());
    #endif

    WLAN::state = 1;
    WLAN::testStartTime = millis();
}

#if defined(ARDUINO_ARCH_ESP8266)
    void WLAN::connectEsp8266() {
        Serial.print("Trying to connect to '");
        Serial.print(WLAN::currentSSID);
        Serial.print("' using password '");
        Serial.print(WLAN::currentPassword);
        Serial.println("'");

        WiFi.begin(WLAN::currentSSID, WLAN::currentPassword);
    }
#endif

#if defined(ARDUINO_ARCH_ESP32)
    void WLAN::connectEsp32() {
        WiFi.begin(WLAN::currentSSID, WLAN::currentPassword);
    }
#endif

void WLAN::updateWifiStatus() {
    WLAN::state = 1;

    #if defined(ARDUINO_ARCH_ESP8266)
        WLAN::updateEsp8266WifiStatus();
    #endif
    
    #if defined(ARDUINO_ARCH_ESP32)
        WLAN::updateEsp32WifiStatus();
    #endif
    
}

#if defined(ARDUINO_ARCH_ESP32)
    void WLAN::updateEsp32WifiStatus() {

    }
#endif

#if defined(ARDUINO_ARCH_ESP8266)
    void WLAN::updateEsp8266WifiStatus() {
        wl_status_t status = WiFi.status();

        if (status == WL_CONNECTED) {
            WLAN::state = 2;
        }

        if (status == WL_DISCONNECTED) {
            WLAN::state = 0;
        }
    }
#endif

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
    WLAN::currentSSID.clear();
    WLAN::currentSSID.concat(ssid);

    for (int i = WIFI_SSID_EEPROM_START_POS; i < WIFI_SSID_EEPROM_END_POS + 1; ++i)
    {
        EEPROM.put(i, WLAN::currentSSID[i - WIFI_SSID_EEPROM_START_POS]);
    }

    EEPROM.commit();

    Serial.println("WLAN::setSSID -> SSID set");
    Serial.print("SSID Input: "); Serial.println(ssid);
    Serial.print("Current SSID: "); Serial.println(WLAN::currentSSID);

    WLAN::connect();
}

void WLAN::setPassword(char* password) {
    WLAN::currentPassword.clear();
    WLAN::currentPassword.concat(password);

    for (int i = WIFI_PASSWORD_EEPROM_START_POS; i < WIFI_PASSWORD_EEPROM_END_POS + 1; ++i)
    {
        EEPROM.put(i, WLAN::currentPassword[i - WIFI_PASSWORD_EEPROM_START_POS]);
    }
    EEPROM.commit();

    Serial.println("WLAN::setPassword -> Password set");
    Serial.print("PW Input: "); Serial.println(password);
    Serial.print("Current PW: "); Serial.println(WLAN::currentPassword);

    WLAN::connect();
}