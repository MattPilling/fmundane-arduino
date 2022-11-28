/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/gpio/0 will set the GPIO2 low,
      http://server_ip/gpio/1 will set the GPIO2 high
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

#ifndef STASSID
#define STASSID "DYNAMIC-USER-N-2GHz"
#define STAPSK "immersion"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
   
// Create an instance of the server
// specify the port to listen on as an argument
ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
}

void handleOpenDrawer(){

 bool dooropen = digitalRead(D7) == 1;
 if (!dooropen){
    //server.send(200, "application/json", "{\"success\":false, \"error\":\"door is shut\"}");
    //return;
 }
  
  Serial.write("G0 X1310\n");
  
  int count = 0;
  delay(2000);
  int frontswitch = 1;
  
  while (count < 5){
      frontswitch = digitalRead(D5);
    
      if (frontswitch == 0){
        server.send(200, "application/json", "{\"success\":true}");
        return;
      }
      delay(1000);
      count +=1;
   }
   server.send(200, "application/json", "{\"success\":false, \"error\":\"drawer did not fully open\"}");
}

void handleCloseDrawer(){  
  
  Serial.write("G0 X0\n");
  
  int count = 0;
  delay(2000);
  int backswitch = 1;
  
  while (count < 5){
      backswitch = digitalRead(D6);
     
      if (backswitch == 0){
        server.send(200, "application/json", "{\"success\":true}");
        return;
      }
      delay(1000);
      count +=1;
   }
   server.send(200, "application/json", "{\"success\":false, \"error\":\"drawer did fully close\"}");
}

void handleStatus(){
  bool dooropen = digitalRead(D7) == 1;
  int frontswitch = digitalRead(D5);
  int backswitch = digitalRead(D6);
  char response[64];
  sprintf(response, "{\"ts\":%u, \"dooropen\":%s, \"draweropen\":%s, \"drawershut\":%s}\n", millis(), dooropen ? "true" : "false" , frontswitch == 0 ? "true" : "false" ,backswitch == 0 ? "true" : "false"); 
  server.send(200, "application/json", response);
}

void handleOpenDoor(){
  bool dooropen = digitalRead(D7) == 1;
 
   if (!dooropen){
     Serial.write("M3\n");
     Serial.write("S300\n");
     delay(1400);
     Serial.write("S0\n");
   }else{
     server.send(200, "application/json", "{\"success\":false, \"error\": \"door already open\"}");
   }
   int count = 0;
   delay(500);
   while (count < 10){
      dooropen = digitalRead(D7) == 1;
      
      if (dooropen){
        server.send(200, "application/json", "{\"success\":true}");
        return;
      }
      delay(500);
      count +=1;
   }
   server.send(200, "application/json", "{\"success\":false, \"error\":\"door did not receive signal\"}");
}

void handleCloseDoor(){
   bool dooropen = digitalRead(D7) == 1;
   int frontswitch = digitalRead(D5);
   int backswitch = digitalRead(D6);
   
   if (backswitch == 1){
     server.send(200, "application/json", "{\"success\":false, \"error\":\"drawer is not closed\"}");
     return;
   }
   
   if (frontswitch == 0){
    server.send(200, "application/json", "{\"success\":false, \"error\":\"drawer is open\"}");
    return;
   }
   
   if (dooropen){
     Serial.write("M3\n");
     Serial.write("S300\n");
     delay(1400);
     Serial.write("S0\n");
   }else{
     server.send(200, "application/json", "{\"success\":false, \"error\":\"door already closed\"}");
     return;
   }
   
   int count = 0;
   delay(4000);
   
   while (count < 20){
    dooropen = digitalRead(D7) == 1;
  
    if (!dooropen){
      server.send(200, "application/json", "{\"success\":true}");
      return;
    }
    delay(1000);
    count +=1;
   }
   server.send(200, "application/json", "{\"success\":false, \"error\":\"door did not receive signal\"}");
}

void setup() {
  Serial.begin(115200);
  pinMode(A0,INPUT);
  pinMode(D5, INPUT_PULLUP);
  pinMode(D6, INPUT_PULLUP);
  pinMode(D7, INPUT_PULLUP);
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));

  // Start the server
  server.begin();
  //Serial.println(F("Server started"));

  // Print the IP address
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/drawer/open", handleOpenDrawer);
  server.on("/drawer/close", handleCloseDrawer);
  server.on("/door/open", handleOpenDoor);
  server.on("/door/close", handleCloseDoor);
  server.on("/status", handleStatus);
  server.begin();
  
  //Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
