#include <SPI.h>
#include <WiFi101.h>
#include "arduino_secrets.h"
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;
WiFiServer server(80);

#define gate 11

void setup() {
  Serial.begin(9600);
  WiFi.setPins(8,7,4,2);
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);       // don't continue
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    Serial.println("attempting to connect..");
    delay(10000);
  }
  Serial.println("server listening!");
  printWiFiStatus();
  server.begin();
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
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

void loop() {
  WiFiClient client = server.available();   // listen for incoming clients
  if (client) {                             // if you get a client,
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
//        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
//            client.print("Click <a href=\"/on1\">ON 1</a><br>");
//            client.print("Click <a href=\"/on2\">ON 2</a><br>");
//            client.print("Click <a href=\"/on3\">ON 3</a><br>");
//            client.print("Click <a href=\"/off\">OFF</a><br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
        
        if (currentLine.endsWith("GET /on1")) {
          turnOn(55);              // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /on2")) {
          turnOn(155);              // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /on3")) {
          turnOn(255);              // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /off")) {
          turnOff();                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
  }
}

void turnOn(long val) {
  analogWrite(gate, val);
}

void turnOff() {
  analogWrite(gate, 0);
}
