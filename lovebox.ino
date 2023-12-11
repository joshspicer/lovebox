#include "variables.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>

// 20 seconds
unsigned long timerDelayLED = 20000;
unsigned long lastTimeLED = 0;

// 5 seconds
unsigned long timerDelayButton = 5000;
unsigned long lastTimeButton = 0;

const int ledPin = 2;
const int buttonPin = 4;

void setup()
{
    Serial.begin(9600);
    Serial.println();

    WiFi.begin(ssid, password);

    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());

    pinMode(ledPin, OUTPUT);    // LED
    pinMode(buttonPin, INPUT);  // Button

    // Provision
    if (!provision())
    {
        Serial.println("[-] Provisioning failed");
        blinkLEDForever();
    }
}

bool provision()
{
    WiFiClient client;
    HTTPClient http;

    http.begin(client, provisionMeAddress.c_str());
    // http.setAuthorization("user", "password");

    // Send HTTP POST request
    int httpResponseCode = http.GET();
    Serial.print("PROVISION ");
    Serial.println(httpResponseCode);

    http.end();
    
   return httpResponseCode == 200;
}

void blinkLEDForever()
{
    while (true)
    {
        digitalWrite(ledPin, HIGH);
        delay(1000);
        digitalWrite(ledPin, LOW);
        delay(1000);
    }
}


void loop()
{
    // Check for button press
    // POSTs message to server, enabling the LED of the peer device :')
    int buttonState = digitalRead(buttonPin);
    if (buttonState == LOW && (millis() - lastTimeButton) > timerDelayButton)
    {
        Serial.println("[!] Button press detected");
        if (WiFi.status() == WL_CONNECTED)
        {
            WiFiClient client;
            HTTPClient http;

            // Your Domain name with URL path or IP address with path
            http.begin(client, postPeerAddress.c_str());
            // http.setAuthorization("user", "password");

            // Send HTTP POST request
            int httpResponseCode = http.POST("from="+me);
            Serial.print("POST ");
            Serial.println(httpResponseCode);

            http.end();
            lastTimeButton = millis();
        }
        else
        {
            Serial.println("[-] WiFi Disconnected");
        }
    }

        // Get state from server
        if ((millis() - lastTimeLED) > timerDelayLED)
        {
            Serial.println("[+] Polling server...");

            // Check WiFi connection status
            if (WiFi.status() == WL_CONNECTED)
            {
                WiFiClient client;
                HTTPClient http;

                // Your Domain name with URL path or IP address with path
                http.begin(client, getMeAddress.c_str());

                // If you need Node-RED/server authentication, insert user and password below
                // http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");

                // Send HTTP GET request
                int httpResponseCode = http.GET();
                Serial.print("GET ");
                Serial.println(httpResponseCode);

                if (httpResponseCode > 0)
                {
                    String payload = http.getString();

                    // Parse payload as JSON (get 'enable' bool value)
                    JSONVar myObject = JSON.parse(payload);

                    if (JSON.typeof(myObject) == "undefined")
                    {
                        Serial.println("[-] Parsing input failed!");
                        return;
                    }

                    // Serial.println(myObject);

                    // Get the value of the "enable" element
                    int enable = myObject["enable"];
                    if (enable)
                    {
                        // Turn on LED
                        digitalWrite(2, HIGH);
                    }
                    else
                    {
                        // Turn off LED
                        digitalWrite(2, LOW);
                    }
                }
                else
                {
                    Serial.print("[-] Error code: ");
                    Serial.println(httpResponseCode);
                }
                // Free
                http.end();
            }
            else
            {
                Serial.println("[-] WiFi Disconnected");
            }

            lastTimeLED = millis();
        }
    }