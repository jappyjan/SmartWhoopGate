#include <Arduino.h>
#include "rx5808.h"
#include "const.h"

void RX5808::setup()
{
  Serial.println("RX5808::setup -> start");
  pinMode(PIN_RSSI, INPUT);
  pinMode(PIN_SLAVE_SELECT, OUTPUT);
  pinMode(PIN_SPI_DATA, OUTPUT);
  pinMode(PIN_SPI_CLOCK, OUTPUT);
  Serial.println("RX5808::setup -> done");
}

void RX5808::setFrequency(uint16_t frequency)
{
  Serial.print("RX5808::setFrequency ");
  Serial.println(frequency);
  uint8_t frequencyIndex = getFrequencyIndex(frequency);
  setFrequencyByIndex(frequencyIndex);
}

uint8_t RX5808::getFrequencyIndex(uint16_t frequency)
{
  for (uint8_t channelIndex = 0; channelIndex < sizeof(channelFreqTable); channelIndex++) {
    if (frequency == pgm_read_word_near(channelFreqTable + channelIndex)) return channelIndex;
  }

  return 0;
}

uint16_t RX5808::readRssi()
{
  volatile uint16_t rssi = 0;

  for (uint8_t i = 0; i < RSSI_READS; i++) {
    rssi += analogRead(PIN_RSSI);
  }

  rssi = rssi / RSSI_READS; // average of RSSI_READS readings

  return rssi;
}

void RX5808::waitRssi()
{
#ifdef MIN_TUNE_TIME
  delay(MIN_TUNE_TIME);
#endif
}

void RX5808::setFrequencyByIndex(uint8_t index)
{
  uint8_t i;
  uint16_t channelData;

  channelData = pgm_read_word_near(channelTable + index);

  // bit bash out 25 bits of data
  // Order: A0-3, !R/W, D0-D19
  // A0=0, A1=0, A2=0, A3=1, RW=0, D0-19=0
  serialEnable(HIGH);
  delayMicroseconds(1);
  serialEnable(LOW);

  serialSendBit(LOW);
  serialSendBit(LOW);
  serialSendBit(LOW);
  serialSendBit(HIGH);

  serialSendBit(LOW);

  // Remaining zeros
  for (i = 20; i > 0; i--) {
    serialSendBit(LOW);
  }

  // Clock the data in
  serialEnable(HIGH);
  delayMicroseconds(1);
  serialEnable(LOW);

  // Second is the channel data from the lookup table
  // 20 bytes of register data are sent, but the MSB 4 bits are zeros
  // register address = 0x1, write, data0-15=channelData data15-19=0x0
  serialEnable(HIGH);
  serialEnable(LOW);

  // Register 0x1
  serialSendBit(HIGH);
  serialSendBit(LOW);
  serialSendBit(LOW);
  serialSendBit(LOW);

  // Write to register
  serialSendBit(HIGH);

  // D0-D15
  // Note: Loop runs backwards as more efficent on AVR
  for (i = 16; i > 0; i--) {
    // Is bit high or low?
    if (channelData & 0x1) {
      serialSendBit(HIGH);
    } else {
      serialSendBit(LOW);
    }

    // Shift bits along to check the next one
    channelData >>= 1;
  }

  // Remaining D16-D19
  for (i = 4; i > 0; i--) {
    serialSendBit(LOW);
  }

  // Finished clocking data in
  serialEnable(HIGH);
  delayMicroseconds(1);

  digitalWrite(PIN_SLAVE_SELECT, LOW);
  digitalWrite(PIN_SPI_CLOCK, LOW);
  digitalWrite(PIN_SPI_DATA, LOW);

  // Wait to allow frequency to be tuned
  waitRssi();
}

void RX5808::serialSendBit(const uint8_t bit)
{
  digitalWrite(PIN_SPI_CLOCK, LOW);
  delayMicroseconds(1);

  digitalWrite(PIN_SPI_DATA, bit);
  delayMicroseconds(1);
  digitalWrite(PIN_SPI_CLOCK, HIGH);
  delayMicroseconds(1);

  digitalWrite(PIN_SPI_CLOCK, LOW);
  delayMicroseconds(1);
}

void RX5808::serialEnable(const uint8_t level)
{
  delayMicroseconds(1);
  digitalWrite(PIN_SLAVE_SELECT, level);
  delayMicroseconds(1);
}
