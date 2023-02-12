#include <Arduino.h>
#include <WiFiClientSecure.h>

#if defined(ARDUINO_ARCH_ESP32)
  #include <HttpsOTAUpdate.h>
#endif

class OTA_UPDATE
{
  public:
    static WiFiClientSecure client;
    static void setup();
    static void loop();
    static void doUpdateToLatestVersion();
    static void doUpdateToSpecificVersion(char* targetVersion);

    #if defined(ARDUINO_ARCH_ESP32)
      static void onHttpEvent(HttpEvent_t *event);
    #endif
  
  private:
    static boolean checkIfNewVersionIsAvailable();
    static void doUpdate(char* uri);
    static bool validateServerCertificate();
};
