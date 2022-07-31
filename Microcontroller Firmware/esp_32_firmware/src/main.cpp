// Arduino Framework
#include <Arduino.h>

// Libraries
#include "WiFi.h"
#include "AsyncUDP.h"

// WiFi Credentials.
#include "credentials.h"
const char* ssid = WIFI_SSID;
const char* pass = WIFI_PWD;

// Set the PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

// Declare UDP Object
AsyncUDP udp;

// Configure PWM Write Pin Properties
const int ledPin = 26; // LED connected to GPIO pin 26
int ledBrightness = 255;

// Prints Debug Info from a UDP Packet.
void printUDPPacketInfo(AsyncUDPPacket packet) {
    Serial.print("UDP Packet Type: ");
    Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast" : "Unicast");
    Serial.print(", From: ");
    Serial.print(packet.remoteIP());
    Serial.print(":");
    Serial.print(packet.remotePort());
    Serial.print(", To: ");
    Serial.print(packet.localIP());
    Serial.print(":");
    Serial.print(packet.localPort());
    Serial.print(", Length: ");
    Serial.print(packet.length()); //dlzka packetu
    Serial.print(", Data: ");
    Serial.write(packet.data(), packet.length());
    Serial.println();
}

void setup()
{
    // Initialise Serial
    Serial.begin(115200);
    
    // Configure pin modes.
    pinMode(ledPin, OUTPUT);

    // esp32 has 16 PWM Channels.
    // Configure the required channel 
    // to the desired frequency and resolution.
    ledcSetup(ledChannel, freq, resolution);
    

    // attach the GPIO pin to the PWM channel:
    ledcAttachPin(ledPin, ledChannel);

    // WiFi Initialisation
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
    
    // Listen for UDP Packets on port 1234
    if (udp.listen(1234)) {
        
        // Debug message
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());

        // Event Handler for receiving a packet.
        udp.onPacket([](AsyncUDPPacket packet) {
            
            // Debug message
            printUDPPacketInfo(packet);
            
            //   size_t bufIndex = packet.readBytesUntil('\n', buf, 10);
            //   String myString = buf;
            //   memset(buf, 0, 10);
            
            // Read the packet into a string. 
            String myString = (const char*)packet.data();
            
            // Message protocol standardised to 16 bytes length.
            myString = myString.substring(0, 16);
            
            // Debug Message
            Serial.println(myString);
            
            // Message protocol:
            // XXXX,YYYY,RRRR,e
            // 
            // Check if the enable is set:
            if (myString[15] == '1') {
                
                // Debug Message
                Serial.println("Turning LED On...");
                
                // Convert XXXX to 0-255 PWM levels.
                ledBrightness = map(myString.substring(0,3).toInt(), 0,2000, 0,255);
                
                // Write to PWM
                ledcWrite(ledChannel, ledBrightness);
            } 
            else if (myString[15] == '0') {
                
                // Debug Message
                Serial.println("Turning LED Off...");

                // Write the LED Low.
                ledcWrite(ledChannel, 0);
            }
            else {
                
                // Debug Message
                Serial.println("Invalid. LED Off");

                // Write to PWM
                ledcWrite(ledChannel, 0);
            }
            
            // flush the packet buffer.
            packet.flush();
            
            // Optional Debug: send a response.
            packet.printf("Got %u bytes of data", packet.length());
        });
    }
}

void loop()
{
  delay(1000);
  udp.broadcast("Anyone here?");
}