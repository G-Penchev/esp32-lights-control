#include <Arduino.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>

#include "webpage.h"

const int numButtons = 1;
const int buttonPins[numButtons] = {34};
const int ledPins[numButtons] = {16};

bool buttonState[numButtons] = {false};
bool lastButtonState[numButtons] = {false};
unsigned long buttonPressStartTime[numButtons] = {0};
unsigned long buttonPressDuration[numButtons] = {0};
bool longPressActive[numButtons] = {false};
bool ledState[numButtons] = {false};
bool dimmingDirection[numButtons] = {true}; // true for increasing, false for decreasing
int brightness[numButtons] = {0};
const int maxBrightness[numButtons] = {900};
int currentBrightness[numButtons] = {0};

const int fadeStep = 1; // Adjust the step size for fading
const int freq = 500;
const int resolution = 10;

// SSID and Password of your WiFi router
const char *ssid = "cocoapop_deco";
const char *password = "BruteForce_8991";

// Declare a global object variable from the ESP8266WebServer class.
AsyncWebServer server(80); // Server on port 80

void setup()
{
    Serial.begin(115200);

    WiFi.begin(ssid, password); // Connect to your WiFi router
    pinMode(23, OUTPUT);
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        digitalWrite(23, LOW);
        delay(150);
        Serial.print(".");
        digitalWrite(23, HIGH);
        delay(150);
    }

    // If connection successful show IP address in serial monitor
    Serial.println("");
    digitalWrite(23, HIGH);
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP()); // IP address assigned to your ESP

    // Route setup
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    String html = MAIN_page;
    request->send(200, "text/html", html); });

    server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    if (request->hasParam("light") && request->hasParam("brightness")) {
      int i = request->getParam("light")->value().toInt();
      int newBrightness = request->getParam("brightness")->value().toInt();
      if (i >= 0 && i < numButtons && newBrightness >= 0 && newBrightness <= 800) {
        brightness[i] = newBrightness;
        ledState[i] = brightness[i] != 0;
        ledcWrite(i, getLogValueForTenBit(brightness[i]));
        request->send(200, "text/plain", "OK");
      } else {
        request->send(400, "text/plain", "Invalid parameters");
      }
    } else {
      request->send(400, "text/plain", "Missing parameters");
    } });

    server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    if (request->hasParam("light")) {
      int i = request->getParam("light")->value().toInt();
      
      if (i >= 0 && i < numButtons) {
        toggleLight(i);
        request->send(200, "text/plain", "OK");
      } else {
        request->send(400, "text/plain", "Invalid parameters");
      }
    } else {
      request->send(400, "text/plain", "Missing parameters");
    } });

    server.begin();
    Serial.println("HTTP server started");

    for (int i = 0; i < numButtons; i++)
    {
        pinMode(buttonPins[i], INPUT_PULLUP);
        ledcSetup(i, freq, resolution);
        ledcAttachPin(ledPins[i], i);
        ledcWrite(i, 0);
    }
}

void loop()
{
    delay(5);
    for (int i = 0; i < numButtons; i++)
    {
        buttonState[i] = digitalRead(buttonPins[i]);
        if (buttonState[i] == LOW)
        {

            if (lastButtonState[i] == HIGH)
            {
                buttonPressStartTime[i] = millis();
                Serial.print("State changed = ");
                Serial.println(buttonState[i]);
            }

            // Check for long press

            buttonPressDuration[i] = millis() - buttonPressStartTime[i];
            if (buttonPressDuration[i] > 250 && !longPressActive[i])
            {
                // Long press
                longPressActive[i] = true;
                dimmingDirection[i] = !dimmingDirection[i]; // Toggle dimming direction
                Serial.print("Dimming dir changed = ");
                Serial.println(dimmingDirection[i]);
            }

            // Dimming logic for long press
            if (longPressActive[i])
            {
                if (dimmingDirection[i] && brightness[i] < maxBrightness[i])
                {
                    if (brightness[i] < 300)
                    {
                        brightness[i] = 300;
                    }
                    brightness[i] += fadeStep;
                }
                else if (!dimmingDirection[i] && brightness[i] > 0)
                {
                    brightness[i] -= fadeStep;
                }
                ledState[i] = brightness[i] != 0;
                ledcWrite(i, getLogValueForTenBit(brightness[i]));
            }
        }
        else
        {
            if (buttonPressDuration[i] < 250 && buttonPressDuration[i] > 20)
            {
                // Short press
                Serial.print("Short press detected = ");
                Serial.println(ledState[i]);
                toggleLight(i);
            }
            // Button released
            longPressActive[i] = false;
            buttonPressDuration[i] = 0;
        }

        lastButtonState[i] = buttonState[i];
    }
}

int getLogValueForTenBit(int value)
{
    Serial.print(value);
    Serial.print(" = ");
    int log = int(pow(2, value / 102.3) - 1);
    Serial.println(log);
    return log;
}

void toggleLight(int i)
{
    ledState[i] = !ledState[i];
    dimmingDirection[i] = ledState[i];
    currentBrightness[i] = brightness[i];
    brightness[i] = ledState[i] ? maxBrightness[i] : 0;
    while (currentBrightness[i] != brightness[i])
    {
        currentBrightness[i] > brightness[i] ? currentBrightness[i]-- : currentBrightness[i]++;
        ledcWrite(i, getLogValueForTenBit(currentBrightness[i]));
        delay(1);
    }
}