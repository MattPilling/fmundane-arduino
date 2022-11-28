/***************************************************
  Adafruit MQTT Library WINC1500 Example

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <SPI.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <WiFi101.h>
#include <Adafruit_NeoPixel.h>
/************************* WiFI Setup *****************************/
#define WINC_CS   8
#define WINC_IRQ  7
#define WINC_RST  4
#define WINC_EN   2     // or, tie EN to VCC


#define LED_PIN    6
#define LED_COUNT 300
char ssid[] = "DYNAMIC-USER-N-2GHz";     //  your network SSID (name)
char pass[] = "immersion";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "192.168.1.113"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "fmundane"
#define AIO_KEY         ""

/************ Global State (you don't need to change this!) ******************/

//Set up the wifi client
WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// You don't need to change anything below this line!
#define halt(s) { Serial.println(F( s )); while(1);  }

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
//Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/photocell");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe lights = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/lights/");

/*************************** Sketch Code ************************************/




void setup() {
  WiFi.setPins(WINC_CS, WINC_IRQ, WINC_RST, WINC_EN);
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();
  strip.setBrightness(50);  
  
  
  //while (!Serial);
  //Serial.begin(9600);

 // Serial.println(F("Adafruit MQTT demo for WINC1500"));

  // Initialise the Client
  //Serial.print(F("\nInit the WiFi module..."));
  // check for the presence of the breakout
  if (WiFi.status() == WL_NO_SHIELD) {
    colorFlood(strip.Color(255,   0,   0)); // Red
     
    Serial.println("WINC1500 not present");
    // don't continue:
    while (true);
  }
  //Serial.println("ATWINC OK!");
  colorFlood(strip.Color(0,   0,   255)); // Blue
  mqtt.subscribe(&lights);
  
 
}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &lights) {
      Serial.print(F("Got: "));
      Serial.println((char *)lights.lastread);

      if (0 == strcmp((char *)lights.lastread, "OFF")) {
        strip.clear();
        strip.show();
      }
      if (0 == strcmp((char *)lights.lastread, "RED")) {
        colorWipe(strip.Color(255,   0,   0), 50); // Red
      }
      if (0 == strcmp((char *)lights.lastread, "GREEN")) {
        colorWipe(strip.Color(  0, 255,   0), 50); // Green
      }
      if (0 == strcmp((char *)lights.lastread, "RED")) {
        colorWipe(strip.Color(  0,   0, 255), 50); // Blue
      }
      if (0 == strcmp((char *)lights.lastread, "RAINBOW")) {
        theaterChaseRainbow(50);
        strip.clear();
        strip.show();
      }
    }
  }

  // Now we can publish stuff!
  //Serial.print(F("\nSending photocell val "));
  //Serial.print(x);
  //Serial.print("...");
  //if (! photocell.publish(x++)) {
   // Serial.println(F("Failed"));
  //} else {
  //  Serial.println(F("OK!"));
  //}
}

void colorFlood(uint32_t color) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
  }
  strip.show(); 
}

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<100; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
 
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    uint8_t timeout = 5;
    while (timeout && (WiFi.status() != WL_CONNECTED)) {
      timeout--;
      delay(1000);
    }
    colorFlood(strip.Color(255,   0,   0)); // Green
  }
  
  
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  
  colorFlood(strip.Color(0,   255,   0)); // Green
  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       //Serial.println(mqtt.connectErrorString(ret));
       //Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(2000);  // wait 2 seconds
  }
  colorFlood(strip.Color(255,   255,   255));
  delay(2000);
  strip.clear();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();       
}
