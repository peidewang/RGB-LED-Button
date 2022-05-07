#include <Arduino.h>
#include "IRrecv.h"
#include "IRsend.h"


uint16_t RECV_PIN = D5; // for ESP8266 micrcontroller
uint16_t SEND_PIN = D6; // for ESP8266 micrcontroller



uint32_t message = (uint32_t) 1;

#define LED D0

#define Button D1

#define redPin D2
#define bluePin D3
#define greenPin D4

#define RED 1
#define BLUE 2
#define GREEN 3

IRrecv irrecv(RECV_PIN);
IRsend irsend(SEND_PIN);

decode_results results;

// Button module pin:
// S -> 3V
//  - > GPIO
// center -> GND

// RGB LED pin
// -  -> GND
// R -> D2
// G -> D4
// B -> D3

bool buttonState = false;

void setup()
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(Button, INPUT);

  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  irrecv.enableIRIn();
  irsend.begin();
}

void rgb(int color)
{
  switch (color)
  {
  case RED:
    analogWrite(redPin, 255);
    analogWrite(bluePin, 0);
    analogWrite(greenPin, 0);
    break;
  case BLUE:
    analogWrite(redPin, 0);
    analogWrite(bluePin, 255);
    analogWrite(greenPin, 0);
    break;
  case GREEN:
    analogWrite(redPin, 0);
    analogWrite(bluePin, 0);
    analogWrite(greenPin, 255);
    break;

  default:
    analogWrite(redPin, 255);
    analogWrite(bluePin, 255);
    analogWrite(greenPin, 255);
    break;
  }
}

void loop()
{
  buttonState = digitalRead(Button);
  delay(50);

  if (buttonState == true)
  {
    digitalWrite(LED, HIGH);
    rgb(RED);

    irsend.sendNEC(message, 32);
    Serial.print("Send:" + message);

  }
  else
  {
    digitalWrite(LED, LOW);
    rgb(BLUE);
  }

  if (irrecv.decode(&results))
  {
     Serial.print("results.value=");
     Serial.println(results.value);

    if (results.value >> 32){
      Serial.print("first part: ");
      Serial.println((uint32_t)(results.value >> 32), HEX);        // print the first part of the message
    }                                     // print() & println() can't handle printing long longs. (uint64_t)

      Serial.print("second part: ");
    Serial.println((uint32_t)(results.value & 0xFFFFFFFF), HEX); // print the second part of the message
    irrecv.resume();                                             // Receive the next value
  }
}