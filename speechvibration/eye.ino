#include <Adafruit_NeoPixel.h>

// this constant won't change:
const int  buttonPin = 15;    // the pin that the pushbutton is attached to
const int ledPin = 14;       // the pin that the LED is attached to

// Variables will change:
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, 14, NEO_GRB + NEO_KHZ800);

void setup() {
  // initialize the button pin as a input:
  pinMode(buttonPin, INPUT);
  // initialize the LED as an output:
  pinMode(ledPin, OUTPUT);
  // initialize serial communication:
  //Serial.begin(9600);
  pixel.begin();
}


void loop() {
 
  // read the pushbutton input pin:
  buttonState = digitalRead(buttonPin);
   
  //Serial.println(buttonState);
 
  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      //Serial.println("on");
     
      turnOn(5);
    } else {
      // if the current state is LOW then the button went from on to off:
      //Serial.println("off");
      turnOff();
    }
    // Delay a little bit to avoid bouncing
    delay(50);
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;
 
}

void turnOn(int tInterval){
  //Serial.println("setting pixel colour red");
  pixel.setPixelColor(0, pixel.Color(255, 0, 0));
  pixel.show();
  delay(tInterval);
}

void turnOff(){
  ////Serial.println("setting pixel colour off");
  pixel.setPixelColor(0,  pixel.Color(0, 0, 0));
  pixel.show();
  delay(10);
}
