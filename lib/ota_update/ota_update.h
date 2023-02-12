#ifndef ota_update_h
#define ota_update_h

#include <Arduino.h>
#include <WiFiClientSecure.h>

#if defined(ARDUINO_ARCH_ESP32)
  #include <HttpsOTAUpdate.h>
#endif

struct urlDetails_t {
    String protocol;
    String host;
	  int port;
    String uri;
};

class OTA_UPDATE
{
  public:
    static WiFiClientSecure client;
    static BearSSL::X509List certList;
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
    #if defined(ARDUINO_ARCH_ESP8266)
      static bool validateServerCertificate();
      static void waitForNTP();
      static bool resolveDownloadUrl(String host, int port, String uri, String *resolvedUri);
      static urlDetails_t parseUrl(String url);
    #endif
};

#endif