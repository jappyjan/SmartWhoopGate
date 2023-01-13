#include <Arduino.h>
#include "cli.h"
#include "drone-detector.h"
#include "wlan.h"

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
        
        WLAN::setSSID(ssid);
        return;
    }

    if (cmd.startsWith("wifi_set_password:")) {
        String password = cmd.substring(cmd.indexOf(":") + 1);
        
        WLAN::setPassword(password);
        return;
    }

    Serial.println("CLI::handleCommand -> Could not parse command: ");
    Serial.println(cmd);
}