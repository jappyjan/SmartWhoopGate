#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
#endif

#pragma once

class WLAN
{
  public:
    static void setup();
    static void loop();
    static void setSSID(char* ssid);
    static void setPassword(char* ssid);
    static char* currentSSID;
    static char* currentPassword;
    static unsigned long testStartTime;
    static uint8_t state;

  private:
    static void connect();
    static void testConnection();
    static void updateWifiStatus();

    #if defined(ARDUINO_ARCH_ESP32)
      static void connectEsp32();
    #endif
    #if defined(ARDUINO_ARCH_ESP32)
      static void updateEsp32WifiStatus();
    #endif

    #if defined(ARDUINO_ARCH_ESP8266)
    static void connectEsp8266();
    #endif
    #if defined(ARDUINO_ARCH_ESP8266)
      static void updateEsp8266WifiStatus();
    #endif

    #if defined(ARDUINO_ARCH_ESP8266)
      static void onESP8266WiFiEvent(WiFiEvent_t event);
    #endif
};