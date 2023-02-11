#include <Arduino.h>
#include "cli.h"
#include "drone-detector.h"
#include "wlan.h"
#include "ota_update.h"

void CLI::handleCommand(String cmd) {
    Serial.println("CLI::handleCommand -> Parsing Command");
    cmd.trim();

    if (cmd.startsWith("calibrate_rssi:")) {
        uint8_t channelIndex = cmd.substring(cmd.indexOf(":") + 1).toInt();
        
        DroneDetector::calibrateRSSIForOneChannel(channelIndex);
        return;
    }

    if (cmd.startsWith("enable_channel:")) {
        uint8_t channelIndex = cmd.substring(cmd.indexOf(":") + 1).toInt();
        
        DroneDetector::enableChannel(channelIndex);
        return;
    }

    if (cmd.startsWith("disable_channel:")) {
        uint8_t channelIndex = cmd.substring(cmd.indexOf(":") + 1).toInt();
        
        DroneDetector::disableChannel(channelIndex);
        return;
    }

    if (cmd.startsWith("wifi_set_ssid:")) {
        String ssid = cmd.substring(cmd.indexOf(":") + 1);
        ssid.trim();
        
        WLAN::setSSID(const_cast<char*>(ssid.c_str()));
        return;
    }

    if (cmd.startsWith("wifi_set_password:")) {
        String password = cmd.substring(cmd.indexOf(":") + 1);
        password.trim();
        
        WLAN::setPassword(const_cast<char*>(password.c_str()));
        return;
    }

    if (cmd.startsWith("update:")) {
        String version = cmd.substring(cmd.indexOf(":") + 1);

        if (version == "latest") {
            OTA_UPDATE::doUpdateToLatestVersion();
        } else {
            OTA_UPDATE::doUpdateToSpecificVersion(const_cast<char*>(version.c_str()));
        }
    }

    Serial.println("CLI::handleCommand -> Could not parse command: ");
    Serial.println(cmd);
}