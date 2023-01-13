#include "Arduino.h"

#pragma once

class WLAN
{
  public:
    static void setup();
    static void loop();
    static void setSSID(String ssid);
    static void setPassword(String ssid);
    static String currentSSID;
    static String currentPassword;
    static unsigned long testStartTime;
    static uint8_t state;

  private:
    static void connect();
    static void testConnection();
};