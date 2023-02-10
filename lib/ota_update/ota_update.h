#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP32)
  #include <HttpsOTAUpdate.h>
#endif

class OTA_UPDATE
{
  public:
    static void setup();
    static void loop();

    #if defined(ARDUINO_ARCH_ESP32)
      static void onHttpEvent(HttpEvent_t *event);
    #endif
  
  private:
    static boolean checkIfNewVersionIsAvailable();
    static void doUpdate();
};
