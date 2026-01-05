#include "RF24.h"
#include "printf.h"
#include <SPI.h>

#define CE_PIN 7
#define CSN_PIN 8

RF24 radio(CE_PIN, CSN_PIN);

uint8_t address[][6] = {"1Node", "2Node"};
bool radioNumber = 0; // opposite of the car
bool role = true;     // controller starts as TX
float payload = 0.0;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setPayloadSize(sizeof(payload));

  radio.openWritingPipe(address[radioNumber]);
  radio.openReadingPipe(1, address[!radioNumber]);

  printf_begin();
  radio.printPrettyDetails();

  radio.stopListening();
}

void loop() {

  if (role) {
    payload += 0.5;
    radio.write(&payload, sizeof(float));
    Serial.print("Sent: ");
    Serial.println(payload);
    delay(1000);
  }

  if (Serial.available()) {
    char c = toupper(Serial.read());

    if (c == 'R') {
      role = false;
      radio.startListening();
      Serial.println("Controller RX mode");
    }

    if (c == 'T') {
      role = true;
      radio.stopListening();
      Serial.println("Controller TX mode");
    }
  }

  if (!role) {
    if (radio.available()) {
      radio.read(&payload, sizeof(payload));
      Serial.print("Received: ");
      Serial.println(payload);
    }
  }
}
