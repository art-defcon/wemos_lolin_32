/*
  Arduino Project for WeMoS LOLIN, ESP32/ESP-WROOM-32 Board
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include "SSD1306Wire.h"

const char* ssid = "...";
const char* password = "...";
int visitCounter = 0;

// TCP server at port 80 will respond to HTTP requests
WiFiServer server(80);
 
uint64_t chipid;
String chipid_hex;

// setting up LCD
SSD1306Wire display(0x3c, 5, 4);

void setup() {
  // setup serial com
  Serial.begin(115200);
  delay(10);
    
  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  
  // store chip ID, might come handy one day =)
  chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).;
  uint64_t number;
  unsigned long long1 = (unsigned long)((chipid & 0xFFFF0000) >> 16 );
  unsigned long long2 = (unsigned long)((chipid & 0x0000FFFF));
  chipid_hex = String(long1, HEX) + String(long2, HEX); // six octets


  setupWifi();

  server.begin();
}

void setupWifi(){
  // Setup Wifi connection
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
      } 
      Serial.println("");
      Serial.print("Connected to ");
      Serial.println(ssid);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
}


void loop(void)
{
  // write the buffer to the display
  redrawScreen();
  
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character          

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            visitCounter++;
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head>");
            client.println("<title>"+currentLine+"</title>");
            client.println("</head>");
            client.println("<body>");

            IPAddress ip = WiFi.localIP();
            String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
                      
            client.println("<p>Server is "+ipStr+":80@"+WiFi.SSID()+"</p>");
            client.println("</body>");
            client.println("</html>");
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
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void redrawScreen() {
  // clear the display
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  
  // drawing strings to display  
  display.drawString(0, 0, "ChipID:"+chipid_hex);

  // draw ip
  IPAddress ip = WiFi.localIP();
  String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
  display.drawString(0, 10, "IPv4:"+ipStr);

  // draw ssid
  display.drawString(0, 20, "ssid:"+WiFi.SSID());

  // draw visits
  display.drawString(0, 30, "visits:"+String(visitCounter, DEC));
  
  // draw signal strength
  //long rssi = WiFi.RSSI();
  //String rssiStr =String(rssi, DEC);
  //display.drawString(0, 20, String("rssi:"+rssiStr+" dBm");

  // print the received signal strength:
  //long rssi = WiFi.RSSI();
  //display.drawString(0, 30, String("ssid:"));  
  // write the buffer to the display
  
  display.display();

}

