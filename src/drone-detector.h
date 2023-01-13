#include "Arduino.h"
#include "rx5808.h"
#include "const.h"

class DroneDetector
{
  public:
    static void setup();
    static void loop();
    static void calibrateRSSIForOneChannel(uint8_t channelIndex);
    static void enableChannel(uint8_t channelIndex);
    static void disableChannel(uint8_t channelIndex);
    static int rssiThreshold[sizeof(channelFreqTable)];
    static int minRssi[sizeof(channelFreqTable)];
    static int maxRssi[sizeof(channelFreqTable)];
    static unsigned long lastDetection[sizeof(channelFreqTable)];
    static volatile uint16_t rssi[sizeof(channelFreqTable)];
    static volatile uint16_t rssiPeak[sizeof(channelFreqTable)];
    static unsigned long rssiPeakTime[sizeof(channelFreqTable)];
    static int rssiThresholdOffset;
    static boolean enabledChannels[sizeof(channelFreqTable)];

  private:
    static void detectOnChannel(uint8_t channelIndex);
    static void handleDroneEntering(uint8_t channelIndex, int rssi);
    static void handleDroneLeaving(uint8_t channelIndex, int rssi);
    static void initVariables();
};
