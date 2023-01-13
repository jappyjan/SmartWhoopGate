#include <Arduino.h>
#include <EEPROM.h>

#include "wlan.h"
#include "rx5808.h"
#include "drone-detector.h"
#include "cli.h"

String serialCommandInputString = "";
bool serialCommandInputStringComplete = false;

void setup() {
  Serial.begin(9600);
  EEPROM.begin(512);
  serialCommandInputString.reserve(200);

  RX5808::setup();
  DroneDetector::setup();
  WLAN::setup();

  Serial.println("Setup Done");
}

void loop() {
  if (serialCommandInputStringComplete) {
    Serial.print("Received: ");
    Serial.println(serialCommandInputString);

    CLI::handleCommand(serialCommandInputString);

    serialCommandInputString = "";
    serialCommandInputStringComplete = false;
  }

  DroneDetector::loop();
  WLAN::loop();
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    serialCommandInputString += inChar;

    if (inChar == '\n') {
      serialCommandInputStringComplete = true;
    }
  }
}
