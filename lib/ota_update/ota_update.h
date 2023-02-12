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
    static void setup();
    static void loop();
    static void doUpdateToLatestVersion();
    static void doUpdateToSpecificVersion(char* targetVersion);
    static bool sslIsPrepared;

    #if defined(ARDUINO_ARCH_ESP32)
      static void onHttpEvent(HttpEvent_t *event);
      static bool updateStarted;
    #endif

    static WiFiClientSecure client;
    #if defined(ARDUINO_ARCH_ESP8266)
      static BearSSL::X509List certList;
    #endif
  
  private:
    static void doUpdate(char* uri);
    static bool resolveDownloadUrl(String host, int port, String uri, String *resolvedUri);
    static urlDetails_t parseUrl(String url);
    static void prepareSSL();
    #if defined(ARDUINO_ARCH_ESP8266)
      static bool validateServerCertificate();
      static void waitForNTP();
    #endif
};

#endif