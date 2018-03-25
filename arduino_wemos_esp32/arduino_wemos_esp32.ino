/*
  Arduino Project for WeMoS LOLIN, ESP32/ESP-WROOM-32 Board
*/

#include <WiFi.h>
#include <WiFiClient.h>

const char* ssid = "orchcamp";
const char* password = "greenskins";

// TCP server at port 80 will respond to HTTP requests
WiFiServer server(80);

uint64_t chipid;  

void setup() {
    // store chip ID, might come handy one day =)
    chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).;  

    // Wait for connection
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
    // Check if a client has connected
    WiFiClient client = server.available();
    if (!client) {
        return;
    }
    Serial.println("");
    Serial.println("New client");

    // Wait for data from client to become available
    while(client.connected() && !client.available()){
        delay(1);
    }

    // Read the first line of HTTP request
    String req = client.readStringUntil('\r');

    // First line of HTTP request looks like "GET /path HTTP/1.1"
    // Retrieve the "/path" part by finding the spaces
    int addr_start = req.indexOf(' ');
    int addr_end = req.indexOf(' ', addr_start + 1);
    if (addr_start == -1 || addr_end == -1) {
        Serial.print("Invalid request: ");
        Serial.println(req);
        return;
    }
    req = req.substring(addr_start + 1, addr_end);
    Serial.print("Request: ");
    Serial.println(req);
    client.flush();

    String s;
    if (req == "/")
    {
        IPAddress ip = WiFi.localIP();
        String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
        s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Hello from ESP32 at ";
        s += ipStr;
        s += "</html>\r\n\r\n";
        Serial.println("Sending 200");
    }
    else
    {
        s = "HTTP/1.1 404 Not Found\r\n\r\n";
        Serial.println("Sending 404");
    }
    client.print(s);

    Serial.println("Done with client");
}

