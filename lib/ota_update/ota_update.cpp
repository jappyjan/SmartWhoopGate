#include "ota_update.h"
#include "../../src/const.h"
#include "../../src/generated/github-certificates.h"

#if defined(ARDUINO_ARCH_ESP8266)
    #include <ESP8266WiFi.h>
    #include <ESP8266httpUpdate.h>
    #include <WiFiClient.h>
    #include <WiFiClientSecure.h>
#endif

#if defined(ARDUINO_ARCH_ESP32)
    #include <WiFi.h>
    #include <HttpsOTAUpdate.h>
#endif


WiFiClientSecure OTA_UPDATE::client = WiFiClientSecure();
BearSSL::X509List OTA_UPDATE::certList(cert_DigiCert_Global_Root_CA);

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
        configTime(3 * 3600, 0, "pool.ntp.org");
    }

    void OTA_UPDATE::waitForNTP() {
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
    }

    bool OTA_UPDATE::validateServerCertificate() {
        String host(FIRMWARE_UPDATE_DOWNLOAD_HOST);
        host.replace("https://", "");
        host.replace("http://", "");

        Serial.println("validateServerCertificate() -> connecting to: " + host + ":" + FIRMWARE_UPDATE_DOWNLOAD_PORT);
        if (!OTA_UPDATE::client.connect(host, FIRMWARE_UPDATE_DOWNLOAD_PORT)) {
            Serial.println("SSL Connection failed");
            return false;
        }

        Serial.println("SSL Connection successful");
        return true;
    }

    urlDetails_t OTA_UPDATE::parseUrl(String url) {
        String protocol = "";
        int port = 80;
        if (url.startsWith("http://")) {
            protocol = "http://";
            url.replace("http://", "");
        } else {
            protocol = "https://";
            port = 443;
            url.replace("https://", "");
        }
        int firstSlash = url.indexOf('/');
        String host = url.substring(0, firstSlash);
        String uri = url.substring(firstSlash);

        urlDetails_t urlDetail;

        urlDetail.protocol = protocol;
        urlDetail.host = host;
        urlDetail.port = port;
        urlDetail.uri = uri;

        return urlDetail;
    }

    bool OTA_UPDATE::resolveDownloadUrl(String host, int port, String uri, String *resolvedUrl) {
        bool isFinalURL = false;
        String protocol = "https://";

        while (!isFinalURL) {
            host.replace("https://", "");
            host.replace("http://", "");
            Serial.println("resolveDownloadUrl() -> connecting to: " + host + ":" + port);
            if (!OTA_UPDATE::client.connect(host, port)) {
                Serial.println("Connection Failed.");
                return false;
            }

            OTA_UPDATE::client.print(String("GET ") + uri + " HTTP/1.1\r\n" +
                "Host: " + host + "\r\n" +
                "User-Agent: ESP_OTA_GitHubArduinoLibrary\r\n" +
                "Connection: close\r\n\r\n");

            while (OTA_UPDATE::client.connected()) {
                String response = OTA_UPDATE::client.readStringUntil('\n');
                if (response.startsWith("location: ") || response.startsWith("Location: ")) {
                    isFinalURL = false;
                    String location = response;
                    if (response.startsWith("location: ")) {
                        location.replace("location: ", "");
                    } else {
                        location.replace("Location: ", "");
                    }
                    location.remove(location.length() - 1);

                    if (location.startsWith("http://") || location.startsWith("https://")) {
                        //absolute URL - separate host from path
                        urlDetails_t url = OTA_UPDATE::parseUrl(location);
                        protocol = url.protocol;
                        host = url.host;
                        port = url.port;
                        uri = url.uri;
                    } else {
                        //relative URL - host is the same as before, location represents the new path.
                        uri = location;
                    }
                    //leave the while loop so we don't set isFinalURL on the next line of the header.
                    break;
                } else {
                    //location header not present - this must not be a redirect. Treat this as the final address.
                    isFinalURL = true;
                }
                if (response == "\r") {
                    break;
                }
            }
        }

        if(isFinalURL) {
            resolvedUrl->clear();
            resolvedUrl->concat(protocol);
            resolvedUrl->concat(host);
            resolvedUrl->concat(uri);
            return true;
        } else {
            Serial.println("CONNECTION FAILED");
            return false;
        }
    }

    void OTA_UPDATE::doUpdate(char* uri) {
        Serial.println("ESP8266 starting OTA update");

        OTA_UPDATE::waitForNTP();
        OTA_UPDATE::client.setTrustAnchors(&OTA_UPDATE::certList);

        bool isTrustWorthy = OTA_UPDATE::validateServerCertificate();
        if (!isTrustWorthy) {
            Serial.println("OTA Update server is not trustworthy, aborting!");
            return;
        }
        Serial.println("OTA Update server is trustworthy, starting update process...");

        String resolvedUrl = "";
        bool urlWasResolved = OTA_UPDATE::resolveDownloadUrl(FIRMWARE_UPDATE_DOWNLOAD_HOST, FIRMWARE_UPDATE_DOWNLOAD_PORT, uri, &resolvedUrl);
        if (!urlWasResolved) {
            Serial.println("Could not resolve final OTA Update url...");
            return;
        }
        Serial.println("Final download url was resolved:");
        Serial.println(resolvedUrl);
    
        bool mfln = client.probeMaxFragmentLength(FIRMWARE_UPDATE_DOWNLOAD_HOST, FIRMWARE_UPDATE_DOWNLOAD_PORT, 1024);
        if (mfln) {
            client.setBufferSizes(1024, 1024);
        }

        ESPhttpUpdate.setLedPin(FIRMWARE_UPDATE_LED_PIN, FIRMWARE_UPDATE_LED_PIN_HIGH_VALUE);

        Serial.println("Starting update/download now...");
        t_httpUpdate_return ret = ESPhttpUpdate.update(OTA_UPDATE::client, resolvedUrl);
        switch(ret) {
            case HTTP_UPDATE_FAILED:
                Serial.println("[update] Update failed.");
                Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
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
