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

#define FIRMWARE_UPDATE_SERVER_CERTIFICATE "-----BEGIN CERTIFICATE-----\n" \
    "MIIFajCCBPCgAwIBAgIQBRiaVOvox+kD4KsNklVF3jAKBggqhkjOPQQDAzBWMQsw\n" \
    "CQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMTAwLgYDVQQDEydEaWdp\n" \
    "Q2VydCBUTFMgSHlicmlkIEVDQyBTSEEzODQgMjAyMCBDQTEwHhcNMjIwMzE1MDAw\n" \
    "MDAwWhcNMjMwMzE1MjM1OTU5WjBmMQswCQYDVQQGEwJVUzETMBEGA1UECBMKQ2Fs\n" \
    "aWZvcm5pYTEWMBQGA1UEBxMNU2FuIEZyYW5jaXNjbzEVMBMGA1UEChMMR2l0SHVi\n" \
    "LCBJbmMuMRMwEQYDVQQDEwpnaXRodWIuY29tMFkwEwYHKoZIzj0CAQYIKoZIzj0D\n" \
    "AQcDQgAESrCTcYUh7GI/y3TARsjnANwnSjJLitVRgwgRI1JlxZ1kdZQQn5ltP3v7\n" \
    "KTtYuDdUeEu3PRx3fpDdu2cjMlyA0aOCA44wggOKMB8GA1UdIwQYMBaAFAq8CCkX\n" \
    "jKU5bXoOzjPHLrPt+8N6MB0GA1UdDgQWBBR4qnLGcWloFLVZsZ6LbitAh0I7HjAl\n" \
    "BgNVHREEHjAcggpnaXRodWIuY29tgg53d3cuZ2l0aHViLmNvbTAOBgNVHQ8BAf8E\n" \
    "BAMCB4AwHQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMIGbBgNVHR8EgZMw\n" \
    "gZAwRqBEoEKGQGh0dHA6Ly9jcmwzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydFRMU0h5\n" \
    "YnJpZEVDQ1NIQTM4NDIwMjBDQTEtMS5jcmwwRqBEoEKGQGh0dHA6Ly9jcmw0LmRp\n" \
    "Z2ljZXJ0LmNvbS9EaWdpQ2VydFRMU0h5YnJpZEVDQ1NIQTM4NDIwMjBDQTEtMS5j\n" \
    "cmwwPgYDVR0gBDcwNTAzBgZngQwBAgIwKTAnBggrBgEFBQcCARYbaHR0cDovL3d3\n" \
    "dy5kaWdpY2VydC5jb20vQ1BTMIGFBggrBgEFBQcBAQR5MHcwJAYIKwYBBQUHMAGG\n" \
    "GGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNvbTBPBggrBgEFBQcwAoZDaHR0cDovL2Nh\n" \
    "Y2VydHMuZGlnaWNlcnQuY29tL0RpZ2lDZXJ0VExTSHlicmlkRUNDU0hBMzg0MjAy\n" \
    "MENBMS0xLmNydDAJBgNVHRMEAjAAMIIBfwYKKwYBBAHWeQIEAgSCAW8EggFrAWkA\n" \
    "dgCt9776fP8QyIudPZwePhhqtGcpXc+xDCTKhYY069yCigAAAX+Oi8SRAAAEAwBH\n" \
    "MEUCIAR9cNnvYkZeKs9JElpeXwztYB2yLhtc8bB0rY2ke98nAiEAjiML8HZ7aeVE\n" \
    "P/DkUltwIS4c73VVrG9JguoRrII7gWMAdwA1zxkbv7FsV78PrUxtQsu7ticgJlHq\n" \
    "P+Eq76gDwzvWTAAAAX+Oi8R7AAAEAwBIMEYCIQDNckqvBhup7GpANMf0WPueytL8\n" \
    "u/PBaIAObzNZeNMpOgIhAMjfEtE6AJ2fTjYCFh/BNVKk1mkTwBTavJlGmWomQyaB\n" \
    "AHYAs3N3B+GEUPhjhtYFqdwRCUp5LbFnDAuH3PADDnk2pZoAAAF/jovErAAABAMA\n" \
    "RzBFAiEA9Uj5Ed/XjQpj/MxQRQjzG0UFQLmgWlc73nnt3CJ7vskCICqHfBKlDz7R\n" \
    "EHdV5Vk8bLMBW1Q6S7Ga2SbFuoVXs6zFMAoGCCqGSM49BAMDA2gAMGUCMCiVhqft\n" \
    "7L/stBmv1XqSRNfE/jG/AqKIbmjGTocNbuQ7kt1Cs7kRg+b3b3C9Ipu5FQIxAM7c\n" \
    "tGKrYDGt0pH8iF6rzbp9Q4HQXMZXkNxg+brjWxnaOVGTDNwNH7048+s/hT9bUQ==\n" \
    "-----END CERTIFICATE-----"

// #define FIRMWARE_UPDATE_SERVER_CERTIFICATE_SHA1_FINGERPRINT "1E:16:CC:3F:84:2F:65:FC:C0:AB:93:2D:63:8A:C6:4A:95:C9:1B:7A"

#define FIRMWARE_UPDATE_DOWNLOAD_HOST "https://github.com"
#define FIRMWARE_UPDATE_DOWNLOAD_LATEST_VERSION_URI "/jappyjan/SmartWhoopGate/releases/latest/download/" TARGET_NAME "_firmware.bin"
// TODO: make it possible to download a specific version by using the following template
#define FIRMWARE_UPDATE_DOWNLOAD_SPECIFIC_VERSION_URI "/jappyjan/SmartWhoopGate/releases/download/{{VERSION}}/" TARGET_NAME "_firmware.bin"
#define FIRMWARE_UPDATE_DOWNLOAD_PORT 443
#define FIRMWARE_UPDATE_LED_PIN LED_BUILTIN
#define FIRMWARE_UPDATE_LED_PIN_HIGH_VALUE LOW

// uncomment and define to set wifi credentials once
// #define WIFI_SET_SSID "not-my-wifi"
// #define WIFI_SET_PASSWORD "not-my-password"