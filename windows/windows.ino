#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(9222);

int MotorControl = 7;    // Digital Arduino Pin used to control the motor
 
// the setup routine runs once when you press reset:
void setup()  {
 
  Serial.begin(9600);
  // declare pin 5 to be an output:
  pinMode(MotorControl, OUTPUT);
       // initialize serial communication
  pinMode(9, OUTPUT);      // set the LED pin mode

  // check for the WiFi module:

  if (WiFi.status() == WL_NO_MODULE) {

    Serial.println("Communication with WiFi module failed!");

    // don't continue

    while (true);

  }

  String fv = WiFi.firmwareVersion();

  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {

    Serial.println("Please upgrade the firmware");

  }

  // attempt to connect to Wifi network:

  while (status != WL_CONNECTED) {

    Serial.print("Attempting to connect to Network named: ");

    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:

    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:

    delay(10000);

  }

  server.begin();                           // start the web server on port 80

  printWifiStatus();                        // you're connected now, so print out the status
}
 
// the loop routine runs over and over again forever:
void loop()  {

  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,

    Serial.println("new client");           // print a message out the serial port

    String currentLine = "";                // make a String to hold incoming data from the client

    while (client.connected()) {            // loop while the client's connected

      if (client.available()) {             // if there's bytes to read from the client,

        char c = client.read();             // read a byte, then

        Serial.write(c);                    // print it out the serial monitor

        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.

          // that's the end of the client HTTP request, so send a response:

          if (currentLine.length() == 0) {

            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)

            // and a content-type so the client knows what's coming, then a blank line:

            client.println("HTTP/1.1 200 OK");

            client.println("Content-type:text/html");

            client.println();

            // the content of the HTTP response follows the header:

            client.print("Click <a href=\"/H\">here</a> turn the LED on pin 9 on<br>");

            client.print("Click <a href=\"/L\">here</a> turn the LED on pin 9 off<br>");

            // The HTTP response ends with another blank line:

            client.println();

            // break out of the while loop:

            break;

          } else {    // if you got a newline, then clear currentLine:

            currentLine = "";

          }

        } else if (c != '\r') {  // if you got anything else but a carriage return character,

          currentLine += c;      // add it to the end of the currentLine

        }

        // Check to see if the client request was "GET /H" or "GET /L":

        if (currentLine.endsWith("GET /H")) {
            digitalWrite(MotorControl,HIGH);// NO3 and COM3 Connected (the motor is running)
            digitalWrite(MotorControl,HIGH);
            digitalWrite(MotorControl,HIGH);
            delay(1000);
            digitalWrite(MotorControl,LOW);
            digitalWrite(MotorControl,LOW);
            digitalWrite(MotorControl,LOW);
            delay(1000);
            digitalWrite(MotorControl,HIGH);
            digitalWrite(MotorControl,HIGH);
            digitalWrite(MotorControl,HIGH);
        }
      }
    }

    // close the connection:

    client.stop();

    Serial.println("client disonnected");

  //digitalWrite(MotorControl,HIGH);// NO3 and COM3 Connected (the motor is running)
  //delay(1000); // wait 1000 milliseconds (1 second)
  //digitalWrite(MotorControl,LOW);// NO3 and COM3 Disconnected (the motor is not running)
  //delay(1000); // wait 1000 milliseconds (1 second)
}
}

void printWifiStatus() {

  // print the SSID of the network you're attached to:

  Serial.print("SSID: ");

  Serial.println(WiFi.SSID());

  // print your board's IP address:

  IPAddress ip = WiFi.localIP();

  Serial.print("IP Address: ");

  Serial.println(ip);

  // print the received signal strength:

  long rssi = WiFi.RSSI();

  Serial.print("signal strength (RSSI):");

  Serial.print(rssi);

  Serial.println(" dBm");

  // print where to go in a browser:

  Serial.print("To see this page in action, open a browser to http://");

  Serial.println(ip);
}
