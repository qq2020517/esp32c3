#include <Arduino.h>
#include <WiFi.h>
#include "TinyIRReceiver.hpp"
#if !defined(STR_HELPER)
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#endif
#if defined(ESP32)
#define IR_INPUT_PIN 3
#endif
#define LED 12
#define LED_PIN 13
volatile struct TinyIRReceiverCallbackDataStruct sReadData;
String input;
bool PAD = true;

void setup() {
  Serial.begin(115200);
  delay(4000);
  pinMode(LED, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  //Serial.println(F("START " __FILE__ " from " __DATE__));
  initPCIInterruptForTinyReceiver();
  Serial.println(F("Receive signals at Pins " STR(IR_INPUT_PIN)));
}

void loop() {
  RUN();
}

void RUN() {
  char date[5];
  if (sReadData.justWritten) {
    const char* addDate = itoa(sReadData.Command, date, 10);
    sprintf_P(date, PSTR("0x%03d"), sReadData.Command);
    Serial.println(date);
    input = date;
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    sReadData.justWritten = false;
    if (input) {
      if (input == "0x220") {
        digitalWrite(LED, !digitalRead(LED));
        Serial.println("ON/OFF");
        PAD = false;
      }
      if (input == "0x202") {
        digitalWrite(LED, HIGH);
        Serial.println("上");
        PAD = false;
      }
      if (input == "0x210") {
        digitalWrite(LED, LOW);
        Serial.println("下");
        PAD = false;
      }
      if (input == "0x153") {
        Serial.println("左");
        PAD = false;
      }
      if (input == "0x193") {
        Serial.println("右");
        PAD = false;
      }
      if (input == "0x206") {
        Serial.println("确认");
        PAD = false;
      }
      if (input == "0x128") {
        Serial.println("CH+");
        PAD = false;
      }
      if (input == "0x129") {
        Serial.println("CH-");
        PAD = false;
      }
    }
  }
  delay(200);
}

#if defined(ESP32C3)
void IRAM_ATTR handleReceivedTinyIRData(uint16_t aAddress, uint8_t aCommand, bool isRepeat)
#else
void handleReceivedTinyIRData(uint16_t aAddress, uint8_t aCommand, bool isRepeat)
#endif
{
#if defined(ARDUINO_ARCH_MBED) || defined(ESP32)
  sReadData.Address = aAddress;
  sReadData.Command = aCommand;
  sReadData.isRepeat = isRepeat;
  sReadData.justWritten = true;
#else
  Serial.print(F(" A = 0x"));
  Serial.print(aAddress, HEX);
  Serial.print(F(" C = 0x"));
  Serial.print(aCommand, HEX);
  Serial.print(F(" R = "));
  Serial.print(isRepeat);
  Serial.println();
#endif
}
