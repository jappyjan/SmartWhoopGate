#include "ota_update.h"
#include "../../src/const.h"
#include "../../src/generated/github-certificates.h"

#if defined(ARDUINO_ARCH_ESP8266)
    #include <ESP8266WiFi.h>
    #include <ESP8266httpUpdate.h>
    #include <WiFiClientSecure.h>
#endif

#if defined(ARDUINO_ARCH_ESP32)
    #include <WiFi.h>
    #include <HttpsOTAUpdate.h>
#endif


WiFiClientSecure OTA_UPDATE::client = WiFiClientSecure();

boolean OTA_UPDATE::checkIfNewVersionIsAvailable() {
    return true;
}

void OTA_UPDATE::loop() {
}

#if defined(ARDUINO_ARCH_ESP8266)
    bool OTA_UPDATE::validateServerCertificate() {
        Serial.println("Waiting for Time to Sync with ntp server..");
        time_t now = time(nullptr);
        while (now < 8 *3600 * 2) {
            delay(500);
            Serial.print(".");
            now = time(nullptr);
        }
        Serial.println("");

        Serial.print("Time synchronized with NTP Server! -> ");
        struct tm timeinfo;
        gmtime_r(&now, &timeinfo);
        Serial.println(asctime(&timeinfo));

        String host(FIRMWARE_UPDATE_DOWNLOAD_HOST);
        host.replace("https://", "");
        host.replace("http://", "");

        X509List cert(cert_DigiCert_Global_Root_CA);
        Serial.printf("Verifying: %s:%d...", host, FIRMWARE_UPDATE_DOWNLOAD_PORT);
        Serial.printf("Using certificate: %s\n", cert_DigiCert_Global_Root_CA);

        OTA_UPDATE::client.setTrustAnchors(&cert);

        if (!OTA_UPDATE::client.connect(host, FIRMWARE_UPDATE_DOWNLOAD_PORT)) {
            Serial.println("SSL Connection failed");
            return false;
        }

        return true;
    }
#endif

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
        configTime(3 * 3600, 0, "pool.ntp.org");
    }

    void OTA_UPDATE::doUpdate(char* uri) {
        Serial.println("ESP8266 starting OTA update");

        bool isTrustWorthy = OTA_UPDATE::validateServerCertificate();
        if (!isTrustWorthy) {
            Serial.println("OTA Update server is not trustworthy, aborting!");
            return;
        }

        String url(FIRMWARE_UPDATE_DOWNLOAD_HOST);
        url.concat(uri);
        Serial.println("OTA Update server is trustworthy, starting update process...");
        Serial.println(url);
        t_httpUpdate_return ret = ESPhttpUpdate.update(OTA_UPDATE::client, url);
        switch(ret) {
            case HTTP_UPDATE_FAILED:
                Serial.println("[update] Update failed.");
                break;
            case HTTP_UPDATE_NO_UPDATES:
                Serial.println("[update] Update no Update.");
                break;
            case HTTP_UPDATE_OK:
                Serial.println("[update] Update ok."); // may not be called since we reboot the ESP
                break;
        }
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
