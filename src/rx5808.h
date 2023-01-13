#ifndef rx5808_h
#define rx5808_h

#include "Arduino.h"

class RX5808
{
  public:
    static void setup();
    static void setFrequency(uint16_t frequency);
    static void setFrequencyByIndex(uint8_t index);
    static uint8_t getFrequencyIndex(uint16_t frequency);
    static uint16_t readRssi();

  private:
    static void waitRssi();
    static void serialEnable(const uint8_t);
    static void serialSendBit(const uint8_t);

    static uint8_t rssiPin;
    static uint8_t spiDataPin;
    static uint8_t slaveSelectPin;
    static uint8_t spiClockPin;
};

#endif
