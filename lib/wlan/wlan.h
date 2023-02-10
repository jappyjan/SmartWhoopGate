#include "Arduino.h"

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
    static void connectEsp32();
    static void connectEsp8266();
    static void testConnection();
    static void updateWifiStatus();
    static void updateEsp8266WifiStatus();
    static void updateEsp32WifiStatus();
};