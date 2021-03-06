#include <Arduino.h>
#include "IRrecv.h"
#include "IRsend.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SSD1306.h"
#include <Adafruit_GrayOLED.h>



//#include "AudioFileSourceSPIFFS.h"
#include "AudioFileSourceLittleFS.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"

AudioGeneratorMP3 *mp3;
AudioFileSourceLittleFS *file;
AudioOutputI2SNoDAC *out;


#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 32
#define OLED_RESET 14




const char *ssid = "devolo-178"; // The SSID (name) of the Wi-Fi network you want to connect to
const char *password = "APYLKAGMLRSOYXUB";



uint16_t RECV_PIN = D5;
uint16_t SEND_PIN = D6;

uint32_t message = (uint32_t)1;
uint32_t device_id = (uint32_t)0;

HTTPClient sender;
WiFiClient wifiClient;

#define LED D0

#define Button D7

#define redPin D8
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


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup()
{

  LittleFS.begin();
  file = new AudioFileSourceLittleFS("/blaster.mp3");
  mp3 = new AudioGeneratorMP3();
  out = new AudioOutputI2SNoDAC();
  mp3->begin(file, out);

  Wire.begin(4,5);
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(Button, INPUT);

  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  Serial.print("setup IR");
  irrecv.enableIRIn();
  irsend.begin();

  WiFi.begin(ssid, password); // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println(" ...");

  rgb(RED);



  	// initialize with the I2C addr 0x3C
	if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println("SSD1306");
  };  

  // Clear the buffer.
	display.clearDisplay();

	// Display Text
	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.setCursor(0,1);
	display.println("Try to connect to network");
	display.display();
  delay(1000);
	

  int i = 0;
  while (WiFi.status() != WL_CONNECTED)
  { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i);
    Serial.print(' ');
  }
    rgb(GREEN);

  Serial.print("connected!!");
  Serial.println(WiFi.localIP());
  IPAddress address = WiFi.localIP();
  Serial.println(address[3]);
  device_id = address[3];

  display.clearDisplay();
  display.setCursor(0,1);
	display.println("gun id = " + String(device_id));
	display.display();

  //delay(2000);
	//display.clearDisplay();



  
  

  // test http get request

  if (sender.begin(wifiClient, "http://192.168.2.114:3000/records"))
  {

    // HTTP-Code der Response speichern
    int httpCode = sender.GET();
    Serial.println("Get success!!");

    if (httpCode > 0)
    {

      // Anfrage wurde gesendet und Server hat geantwortet
      // Info: Der HTTP-Code f??r 'OK' ist 200
      if (httpCode == HTTP_CODE_OK)
      {

        // Hier wurden die Daten vom Server empfangen

        // String vom Webseiteninhalt speichern
        String payload = sender.getString();

        // Hier kann mit dem Wert weitergearbeitet werden
        // ist aber nicht unbedingt notwendig
        Serial.println(payload);
      }
      Serial.print("httpCode!= OK");
      Serial.print(httpCode);
    }
    else
    {
      // Falls HTTP-Error
      Serial.print("Error happens");
      Serial.print(httpCode);
    }

    // Wenn alles abgeschlossen ist, wird die Verbindung wieder beendet
    sender.end();
  }
  else
  {
    Serial.printf("HTTP-Verbindung konnte nicht hergestellt werden!");
  }

  Serial.print("setup ended");
}

void loop()
{

  if (mp3->isRunning()) {
    if (!mp3->loop()) mp3->stop(); 
  } else {
    Serial.printf("MP3 done\n");
    delay(1000);
    mp3->begin(file, out);
    Serial.printf("play again\n");

  }


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
    irrecv.resume(); // Receive the next value
  }
}