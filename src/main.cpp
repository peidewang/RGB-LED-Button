#include <Arduino.h>


#include <Arduino.h>
#define LED D0  

#define Button D1

#define redPin D2
#define bluePin D3
#define greenPin D4

#define RED 1
#define BLUE 2
#define GREEN 3


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

void setup() {
  Serial.begin(9600);
   pinMode(LED, OUTPUT);
   pinMode(Button, INPUT);


   pinMode(redPin, OUTPUT);
   pinMode(bluePin, OUTPUT);
   pinMode(greenPin, OUTPUT);
 
}

void rgb(int color){
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

void loop() {
  buttonState = digitalRead(Button);
  
  if ( buttonState == true){
   Serial.print("High") ;
   digitalWrite(LED, HIGH);
   rgb(RED);

  }
  else {
    digitalWrite(LED, LOW);
    rgb(BLUE);
    Serial.print("low") ;
  }
}