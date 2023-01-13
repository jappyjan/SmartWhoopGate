#include <Arduino.h>
#include <EEPROM.h>

#include "drone-detector.h"
#include "rx5808.h"
#include "kalman.h"
#include "const.h"

KalmanFilter kalmanFilter;


unsigned long DroneDetector::lastDetection[sizeof(channelFreqTable)];
int DroneDetector::minRssi[sizeof(channelFreqTable)];
int DroneDetector::maxRssi[sizeof(channelFreqTable)];
int DroneDetector::rssiThreshold[sizeof(channelFreqTable)];
volatile uint16_t DroneDetector::rssi[sizeof(channelFreqTable)];
volatile uint16_t DroneDetector::rssiPeak[sizeof(channelFreqTable)];
unsigned long DroneDetector::rssiPeakTime[sizeof(channelFreqTable)];
int DroneDetector::rssiThresholdOffset = 0;
boolean DroneDetector::enabledChannels[sizeof(channelFreqTable)];

void DroneDetector::setup() {
    DroneDetector::rssiThresholdOffset = RSSI_THRESHOLD_OFFSET;

    DroneDetector::initVariables();
}

void DroneDetector::initVariables() {
    for (unsigned int index = 0; index < sizeof(DroneDetector::rssiThreshold)/sizeof(DroneDetector::rssiThreshold[0]); index++) {
        DroneDetector::rssiThreshold[index] = EEPROM.read((RSSI_CALIBRATION_EEPROM_START_POS + index) * CHANNEL_RSSI_THRESHOLD_EEPROM_OFFSET);
    }
    for (unsigned int index = 0; index < sizeof(DroneDetector::minRssi)/sizeof(DroneDetector::minRssi[0]); index++) {
        DroneDetector::minRssi[index] = 1024;
    }
    for (unsigned int index = 0; index < sizeof(DroneDetector::maxRssi)/sizeof(DroneDetector::maxRssi[0]); index++) {
        DroneDetector::maxRssi[index] = 0;
    }
    for (unsigned int index = 0; index < sizeof(DroneDetector::rssi)/sizeof(DroneDetector::rssi[0]); index++) {
        DroneDetector::rssi[index] = 0;
    }
    for (unsigned int index = 0; index < sizeof(DroneDetector::rssiPeak)/sizeof(DroneDetector::rssiPeak[0]); index++) {
        DroneDetector::rssiPeak[index] = 0;
    }
    for (unsigned int index = 0; index < sizeof(DroneDetector::rssiPeakTime)/sizeof(DroneDetector::rssiPeakTime[0]); index++) {
        DroneDetector::rssiPeakTime[index] = 0;
    }
    for (unsigned int index = 0; index < sizeof(DroneDetector::lastDetection)/sizeof(DroneDetector::lastDetection[0]); index++) {
        DroneDetector::lastDetection[index] = 0;
    }
    for (unsigned int index = 0; index < sizeof(DroneDetector::enabledChannels)/sizeof(DroneDetector::enabledChannels[0]); index++) {
        DroneDetector::enabledChannels[index] = EEPROM.read((RSSI_CALIBRATION_EEPROM_START_POS + index) * CHANNEL_ENABLED_EEPROM_OFFSET) == 1;

        if (DroneDetector::enabledChannels[index] == 1) {
            Serial.print("DroneDetector::init -> Channel ");
            Serial.print(index);
            Serial.print(" enabled with a threshold of ");
            Serial.println(DroneDetector::rssiThreshold[index]);
        }
    }
}

void DroneDetector::loop()
{
    for (uint8_t channelIndex = 0; channelIndex < (uint8_t)sizeof(channelFreqTable); channelIndex++) {
        if (!DroneDetector::enabledChannels[channelIndex]) {
            continue;
        }
        DroneDetector::detectOnChannel(channelIndex);
    }
}

void DroneDetector::calibrateRSSIForOneChannel(uint8_t channelIndex) {
    for(uint8_t i = 0; i < 3; i++) {
        Serial.print("DroneDetector::calibrateRSSIForOneChannel(");
        Serial.print(channelIndex);
        Serial.print(") -> Starting in ");
        Serial.print(3 - i);
        Serial.println("s");

        delay(1000);
    }

    Serial.print("DroneDetector::calibrateRSSIForOneChannel(");
    Serial.print(channelIndex);
    Serial.println(") -> Calibration in progress...");

    DroneDetector::minRssi[channelIndex] = 1024;
    DroneDetector::maxRssi[channelIndex] = 0;

    uint32_t rssi_average = 0;

    for (uint8_t i = 0; i < RSSI_THRESHOLD_CALIBRATE_READS; i++) {
        rssi_average += round(kalmanFilter.filter(RX5808::readRssi(), 0));
    }

    DroneDetector::rssiThreshold[channelIndex] = (rssi_average / RSSI_THRESHOLD_CALIBRATE_READS) - 6;

    EEPROM.put((RSSI_CALIBRATION_EEPROM_START_POS + channelIndex) * CHANNEL_RSSI_THRESHOLD_EEPROM_OFFSET, DroneDetector::rssiThreshold[channelIndex]);
    EEPROM.commit();

    Serial.print("DroneDetector::calibrateRSSIForOneChannel(");
    Serial.print(channelIndex);
    Serial.println(") -> Done");

    Serial.print("RSSI Threshold: ");
    Serial.println(DroneDetector::rssiThreshold[channelIndex]);
}

void DroneDetector::enableChannel(uint8_t channelIndex) {
    Serial.print("DroneDetector::enableChannel(");
    Serial.print(channelIndex);
    Serial.println(")");

    DroneDetector::enabledChannels[channelIndex] = true;

    EEPROM.put((RSSI_CALIBRATION_EEPROM_START_POS + channelIndex) * CHANNEL_ENABLED_EEPROM_OFFSET, 1);
    EEPROM.commit();
}

void DroneDetector::disableChannel(uint8_t channelIndex) {
    Serial.print("DroneDetector::disableChannel(");
    Serial.print(channelIndex);
    Serial.println(")");

    DroneDetector::enabledChannels[channelIndex] = false;

    EEPROM.put((RSSI_CALIBRATION_EEPROM_START_POS + channelIndex) * CHANNEL_ENABLED_EEPROM_OFFSET, 0);
    EEPROM.commit();
}

void DroneDetector::detectOnChannel(uint8_t channelIndex) {
    RX5808::setFrequencyByIndex(channelIndex);

    DroneDetector::rssi[channelIndex] = RX5808::readRssi();
    int rssi_filter = round(kalmanFilter.filter(DroneDetector::rssi[channelIndex], 0));

    // Update RSSI minimum and maximum values
    DroneDetector::minRssi[channelIndex] = min(DroneDetector::minRssi[channelIndex], rssi_filter);
    DroneDetector::maxRssi[channelIndex] = max(DroneDetector::maxRssi[channelIndex], rssi_filter);

    DroneDetector::handleDroneEntering(channelIndex, rssi_filter);
    DroneDetector::handleDroneLeaving(channelIndex, rssi_filter);
}

void DroneDetector::handleDroneEntering(uint8_t channelIndex, int rssi) {
    if (rssi < DroneDetector::rssiThreshold[channelIndex]) {
        return;
    }

    if (rssi <= DroneDetector::rssiPeak[channelIndex]) {
        return;
    }

    // Check if RSSI is greater than the previous detected peak
    DroneDetector::rssiPeak[channelIndex] = rssi;
    DroneDetector::rssiPeakTime[channelIndex] = millis();
}

void DroneDetector::handleDroneLeaving(uint8_t channelIndex, int rssi) {
    if (!(
        rssi < DroneDetector::rssiPeak[channelIndex] &&
        rssi < (DroneDetector::rssiThreshold[channelIndex] - DroneDetector::rssiThresholdOffset)
    )) {
        return;
    }

    if (millis() < (DroneDetector::lastDetection[channelIndex] + DRONE_DETECTION_DEBOUNCE_TIME)) {
        return;
    }

    Serial.print("Drone on Channel ");
    Serial.print(channelIndex);
    Serial.print(" passed Gate at time ");
    Serial.print(DroneDetector::rssiPeakTime[channelIndex]);
    Serial.print(" with RSSI of ");
    Serial.println(DroneDetector::rssi[channelIndex]);

    DroneDetector::rssiPeak[channelIndex] = 0;
    DroneDetector::rssiPeakTime[channelIndex] = 0;
    DroneDetector::lastDetection[channelIndex] = millis();
}