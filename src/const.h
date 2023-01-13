#ifndef const_h
#define const_h

#pragma once

// EEPROM ADDRESSES
// WIFI SSID
#define WIFI_SSID_EEPROM_START_POS          0
#define WIFI_SSID_EEPROM_END_POS            31
// WIFI PASSWORD
#define WIFI_PASSWORD_EEPROM_START_POS      32
#define WIFI_PASSWORD_EEPROM_END_POS        95
#define RSSI_CALIBRATION_EEPROM_START_POS   96
#define CHANNEL_RSSI_THRESHOLD_EEPROM_OFFSET 1
#define CHANNEL_ENABLED_EEPROM_OFFSET       2


// Default RSSI threshold
#define RSSI_THRESHOLD                      200
#define RSSI_THRESHOLD_OFFSET               36
#define RSSI_THRESHOLD_CALIBRATE_READS      100

#define DRONE_DETECTION_DEBOUNCE_TIME       400

// Pin outs
#define PIN_SPI_DATA                        5
#define PIN_SLAVE_SELECT                    2
#define PIN_SPI_CLOCK                       4
#define PIN_RSSI                            A0

// Number of analog RSSI reads per tick
#define RSSI_READS                          5

// Minimum time required for RX module to tune to frequency
#define MIN_TUNE_TIME                       35


// Channels to sent to the SPI registers
const uint16_t channelTable[] PROGMEM = {
  // Channel 1 - 8
  0x2A05, 0x299B, 0x2991, 0x2987, 0x291D, 0x2913, 0x2909, 0x289F,    // Band A
  0x2903, 0x290C, 0x2916, 0x291F, 0x2989, 0x2992, 0x299C, 0x2A05,    // Band B
  0x2895, 0x288B, 0x2881, 0x2817, 0x2A0F, 0x2A19, 0x2A83, 0x2A8D,    // Band E
  0x2906, 0x2910, 0x291A, 0x2984, 0x298E, 0x2998, 0x2A02, 0x2A0C,    // Band F / Airwave
  0x281D, 0x288F, 0x2902, 0x2914, 0x2987, 0x2999, 0x2A0C, 0x2A1E     // Band C / Immersion Raceband
};

// Channels with their Mhz Values
const uint16_t channelFreqTable[] PROGMEM = {
  // Channel 1 - 8
  5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725, // Band A
  5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866, // Band B
  5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945, // Band E
  5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880, // Band F / Airwave
  5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917  // Band C / Immersion Raceband
};

#endif