// #include <WiFiClientSecure.h>
// #include <FS.h>
#include "ota_update.h"
#include "../../src/const.h"

#if defined(ARDUINO_ARCH_ESP8266)
    #include <ESP8266WiFi.h>
    #include <ESP8266httpUpdate.h>
    #include <WiFiClientSecure.h>
    #include <FS.h>
#endif

#if defined(ARDUINO_ARCH_ESP32)
    #include <WiFi.h>
    #include <HttpsOTAUpdate.h>
#endif

boolean OTA_UPDATE::checkIfNewVersionIsAvailable() {
    return true;
}

void OTA_UPDATE::loop() {
}

void OTA_UPDATE::doUpdateToLatestVersion() {
    OTA_UPDATE::doUpdate(FIRMWARE_UPDATE_DOWNLOAD_LATEST_VERSION_URI);
}

void OTA_UPDATE::doUpdateToSpecificVersion(char* targetVersion) {
    String uriString(FIRMWARE_UPDATE_DOWNLOAD_SPECIFIC_VERSION_URI);

    uriString.replace("{{VERSION}}", targetVersion);

    OTA_UPDATE::doUpdate(const_cast<char*>(uriString.c_str()));
}

#if defined(ARDUINO_ARCH_ESP8266)
    void OTA_UPDATE::setup() {
        // configure time
        // configTime(3 * 3600, 0, "pool.ntp.org");
    }

    void OTA_UPDATE::doUpdate(char* uri) {
        Serial.println("ESP8266 starting OTA update");
        WiFiClientSecure client;
        client.setFingerprint(FIRMWARE_UPDATE_SERVER_CERTIFICATE_SHA1_FINGERPRINT);

        if (!client.connect(FIRMWARE_UPDATE_DOWNLOAD_HOST, FIRMWARE_UPDATE_DOWNLOAD_PORT)) {
            Serial.println("connection to ota update server failed");
            return;
        }

        String url(FIRMWARE_UPDATE_DOWNLOAD_HOST);
        url.concat(uri);
        ESPhttpUpdate.update(client, url);
    }
#endif

#if defined(ARDUINO_ARCH_ESP32)
    void OTA_UPDATE::setup() {
        HttpsOTA.onHttpEvent(OTA_UPDATE::onHttpEvent);
    }

    void OTA_UPDATE::onHttpEvent(HttpEvent_t *event) {
        switch(event->event_id) {
            case HTTP_EVENT_ERROR:
                Serial.println("Http Event Error");
                break;
            case HTTP_EVENT_ON_CONNECTED:
                Serial.println("Http Event On Connected");
                break;
            case HTTP_EVENT_HEADER_SENT:
                Serial.println("Http Event Header Sent");
                break;
            case HTTP_EVENT_ON_HEADER:
                Serial.printf("Http Event On Header, key=%s, value=%s\n", event->header_key, event->header_value);
                break;
            case HTTP_EVENT_ON_DATA:
                break;
            case HTTP_EVENT_ON_FINISH:
                Serial.println("Http Event On Finish");
                break;
            case HTTP_EVENT_DISCONNECTED:
                Serial.println("Http Event Disconnected");
                break;
        }
    }

    void OTA_UPDATE::doUpdate(char* uri) {
        Serial.println("Starting OTA");
        String url(FIRMWARE_UPDATE_DOWNLOAD_HOST);
        url.concat(uri);
        HttpsOTA.begin(url.c_str(), FIRMWARE_UPDATE_SERVER_CERTIFICATE); 

        Serial.println("Please Wait it takes some time ...");
    }
#endif
