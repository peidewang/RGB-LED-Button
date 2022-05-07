#include <Arduino.h>
#include "IRrecv.h"
#include "IRsend.h"
#include <ESP8266WiFi.h> 

const char* ssid     = "devolo-178";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "APYLKAGMLRSOYXUB"; 


uint16_t RECV_PIN = D5; 
uint16_t SEND_PIN = D6; 



uint32_t message = (uint32_t) 1;
uint32_t device_id = (uint32_t) 0;

#define LED D0

#define Button D1

#define redPin D2
#define bluePin D3
#define greenPin D4

#define RED 1
#define BLUE 2
#define GREEN 3
#define CYAN 4


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
  case CYAN:
    analogWrite(redPin, 255);
    analogWrite(bluePin, 255);
    analogWrite(greenPin, 0);
    break;  

  default:
    analogWrite(redPin, 255);
    analogWrite(bluePin, 255);
    analogWrite(greenPin, 255);
    break;
  }
}


void setup()
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(Button, INPUT);

  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  Serial.print("setup IR");
  irrecv.enableIRIn();
  irsend.begin();


  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");


  rgb(RED);

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

   Serial.print("connected!!");
   Serial.println(WiFi.localIP());
   IPAddress address = WiFi.localIP();
   Serial.println(u32_t(address));  
   device_id = u32_t(address);
  

}


void loop()
{
  buttonState = digitalRead(Button);
  delay(50);

  if (buttonState == true)
  {
    digitalWrite(LED, HIGH);
    rgb(RED);

    irsend.sendNEC(device_id, 32);
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
    irrecv.resume();                                             // Receive the next value
  }
}