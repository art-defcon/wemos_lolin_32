/*
  Arduino Project for WeMoS LOLIN, ESP32/ESP-WROOM-32 Board
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include "SSD1306Wire.h"
#include "OLEDDisplayUi.h"

const char* ssid = "...";
const char* password = "...";
int visitCounter = 0;

// TCP server at port 80 will respond to HTTP requests
WiFiServer server(80);
 
uint64_t chipid;
String chipid_hex;

// setting up LCD
SSD1306Wire display(0x3c, 5, 4);
OLEDDisplayUi ui( &display );

void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  /* dissable overlay for now
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(128, 0, String(millis()));
  */
}

void loop(void)
{ 
  drawScreen();
  
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
            client.println("<meta chartset=\"utf-8\">");
            client.println("<meta chartset=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">");
            client.println("<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/css/bootstrap.min.css\" integrity=\"sha384-Gn5384xqQ1aoWXA+058RXPxPg6fy4IWvTNh0E263XmFcJlSAwiGgFAW/dAiS6JXm\" crossorigin=\"anonymous\">");
            client.println("<title>ESP32 IoT Webserver in Local Wifi</title>");
            client.println("</head>");
            client.println("<body>");

            IPAddress ip = WiFi.localIP();
            String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
            //client.println("<div class=\"container\" style=\"width: 100%; max-width: 420px; padding: 15px; margin: 0 auto;\" >");
            client.println("<div class=\"jumbotron\">");
            client.println("<div class=\"container\">");
            client.println("<h1>server is up</h1>");          
            client.println("<code class=\"lead\">Server is up and running at "+ipStr+":80 @ \n at Wifi with SSID: "+WiFi.SSID()+"</code>");
            client.println("<br />");
            client.println("<br />");
            client.println("<code class=\"lead\">HTTP Server running at http://"+ipStr+" has served "+visitCounter+" requests during this lifetime</code>");
            client.println("<br />");
            client.println("<br />");
            client.println("<a class=\"btn btn-primary btn-lg\" href=\".\" role=\"button\">Reload</a>");            
            client.println("</div>");
            client.println("</div>");
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

void drawScreenWifi(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {  
  
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

  // draw topic
  display->drawString(0 + x, 0 + y, "NET AND DEVICE DATA");
  
  // drawing strings to display  
  display->drawString(0 + x, 10 + y, "ChipID:"+chipid_hex);

  // draw ip
  IPAddress ip = WiFi.localIP();
  String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
  display->drawString(0 + x, 20 + y, "IPv4:"+ipStr);

  // draw ssid
  display->drawString(0 + x, 30 + y, "ssid:"+WiFi.SSID());

  // draw signal strength
  long rssi = WiFi.RSSI();  
  display->drawString(0 + x, 40 + y, "rssi:"+String(rssi, DEC)+" dBm");
}

void drawScreenHttpd(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {  
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0 + x, 0 + y, "HTTP SERVER");
  
  // draw ip
  IPAddress ip = WiFi.localIP();
  String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
  display->drawString(0 + x, 10 + y, "Server is running on");
  // draw server addr
  display->drawString(0 + x, 20 + y, "http://"+ipStr);
  // draw visits
  display->drawString(0 + x, 30 + y, "visits:"+String(visitCounter, DEC));
}

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = { drawScreenHttpd, drawScreenWifi };
// how many frames are there?
int frameCount = 2;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { msOverlay };
int overlaysCount = 1;

// main draw screen
void drawScreen(){
  int remainingTimeBudget = ui.update();

  if (remainingTimeBudget > 0) {
    // You can do some work here
    // Don't do stuff if you are below your
    // time budget.
    delay(remainingTimeBudget);
  }
}
void setup() {
  // setup serial com
  Serial.begin(115200);
  delay(10);
    
  // The ESP is capable of rendering 60fps in 80Mhz mode
  // but that won't give you much time for anything else
  // run it in 160Mhz mode or just set it to 30 fps
  ui.setTargetFPS(60);

  // Customize the active and inactive symbol
  //ui.setActiveSymbol(activeSymbol);
  //ui.setInactiveSymbol(inactiveSymbol);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(BOTTOM);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  // Add frames
  ui.setFrames(frames, frameCount);

  // Add overlays
  ui.setOverlays(overlays, overlaysCount);

  // Initialising the UI will init the display too.
  ui.init();

  display.flipScreenVertically();
  
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
