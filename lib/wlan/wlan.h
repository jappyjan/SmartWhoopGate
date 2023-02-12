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
    static String currentSSID;
    static String currentPassword;
    static unsigned long testStartTime;
    static uint8_t state;

  private:
    static void connect();
    static void testConnection();
    static void updateWifiStatus();

    #if defined(ARDUINO_ARCH_ESP8266)
      static void onESP8266WiFiEvent(WiFiEvent_t event);
    #endif
};