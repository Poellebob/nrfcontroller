#include "RF24.h"
#include "printf.h"
#include <LiquidCrystal_I2C.h>
#include <SPI.h>

#define CE_PIN 9
#define CSN_PIN 10

#define up_button 2
#define down_button 4
#define left_button 5
#define right_button 3
#define start_button 6
#define select_button 7
#define analog_button 8
#define x_axis A0
#define y_axis A1
int buttons[] = {up_button,    down_button,   left_button,  right_button,
                 start_button, select_button, analog_button};

RF24 radio(CE_PIN, CSN_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

uint8_t address[][6] = {"1Node", "2Node"};
bool radioNumber = 0; // opposite of car
bool role = true;     // controller starts as TX
int16_t payload[9];

struct Speeds {
  int left;
  int right;
};

struct Sensors {
  int leftOuter;
  int leftInner;
  int center;
  int rightInner;
  int rightOuter;
};

struct Data {
  Speeds speed;
  Sensors sensors;
};

Data display;

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Hello World");

  if (!radio.begin()) {
    Serial.println(F("nRF24 hardware not responding!"));
    while (true) {
    }
  }

  radio.setPALevel(RF24_PA_LOW);

  radio.openWritingPipe(address[radioNumber]);
  radio.openReadingPipe(1, address[!radioNumber]);

  printf_begin();
  radio.printPrettyDetails();

  if (role)
    radio.stopListening();
  else
    radio.startListening();

  Serial.println(F("Controller ready. Press T to transmit, R to receive"));
}

void loop() {
  payload[0] = analogRead(x_axis); // throttle
  payload[1] = analogRead(y_axis); // steering

  payload[2] = digitalRead(up_button);
  payload[3] = digitalRead(down_button);
  payload[4] = digitalRead(left_button);
  payload[5] = digitalRead(right_button);
  payload[6] = digitalRead(start_button);
  payload[7] = digitalRead(select_button);
  payload[8] = digitalRead(analog_button);

  radio.write(&payload, sizeof(payload));

  if (radio.available()) {
    radio.read(&display, sizeof(display));
  }
  delay(10);
}
