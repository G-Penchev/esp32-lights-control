#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#include "webpage.h"

//On board LED Connected to GPIO2

#define LED_GPIO   16
#define LED_WIFI   23
#define PWM1_Ch    0
#define PWM1_Res   8
#define PWM1_Freq  5000

int ledState = 0;
int maxLedIntensity = 200;

long previousMillis = 0;

int inputDebounce;
int debounceSamples = 0;

//State flags
bool iterate = false;
bool turnOnLed = false;
bool switching = false;
bool dimmingHigh = false;
bool touchDetected = false;
bool ledOn = false;

//SSID and Password of your WiFi router
const char* ssid = "cocoapop_deco";
const char* password = "BruteForce_8991";

//Declare a global object variable from the ESP8266WebServer class.
WebServer server(80); //Server on port 80

//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot() {
  Serial.println("You called root page");
  String s = MAIN_page; //Read HTML contents
  server.send(200, "text/html", s); //Send web page
}

void handleLEDon() {
  Serial.println("LED on page");
  turnOnLed = true;
  switching = true;
  server.send(200, "text/html", MAIN_page);
}

void handleLEDoff() {
  Serial.println("LED off page");
  turnOnLed = false;
  switching = true;
  server.send(200, "text/html", MAIN_page);
}
//==============================================================
//                  SETUP
//==============================================================
void setup(void) {
  Serial.begin(115200);

  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  //Onboard LED port Direction output
  ledcAttachPin(LED_GPIO, PWM1_Ch);
  ledcSetup(PWM1_Ch, PWM1_Freq, PWM1_Res);

  pinMode(LED_WIFI, OUTPUT);

  pinMode(34, INPUT);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_WIFI, LOW);
    delay(150);
    Serial.print(".");
    digitalWrite(LED_WIFI, HIGH);
    delay(150);
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  digitalWrite(LED_WIFI, HIGH);
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  server.on("/", handleRoot);  
  server.on("/ledOn", handleLEDon); 
  server.on("/ledOff", handleLEDoff);

  server.begin(); 
  Serial.println("HTTP server started");
}

bool senseTouch(uint8_t pin) {

  if (analogRead(pin) < 30) {
    int touch_debounced = 0;
    for (int i = 0; i < 5; i++) {
      delay(2);
      touch_debounced += analogRead(pin);
    }
    touch_debounced = touch_debounced / 5;

    if (touch_debounced < 20) {
      return true;
    } else {
      return false;
    }
  }
  return false;
}

//==============================================================
//                     LOOPsdad
//==============================================================
void loop(void) {
  server.handleClient();          //Handle client requests
  boolean touchEvent
  
  if (senseTouch(34)) {
    delay(250);
    if (senseTouch(34)) {
      //Long press action
      while (senseTouch(34)) {
        if (!dimmingHigh && ledState > 0) {
          ledState -= 1;
          ledcWrite(PWM1_Ch, ledState);
          Serial.print("Dimming Low ");
          Serial.println(ledState);
        } else if (dimmingHigh && ledState < maxLedIntensity) {
          ledState += 1;
          ledcWrite(PWM1_Ch, ledState);
          Serial.print("Dimming High ");
          Serial.println(ledState);
        }
        delay(20);
      }
      dimmingHigh = !dimmingHigh;
    } else {
      //Tap actions
      turnOnLed = ledState == 0;
      switching = !switching;
      Serial.println("Switching");
    }
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 10) {
    previousMillis = currentMillis;
    iterate = true;
  }

  if (iterate && switching) {
    if (ledState < maxLedIntensity && turnOnLed) {
      dimmingHigh = false;
      ledState += 1;
      ledcWrite(PWM1_Ch, ledState);
    } else if (ledState > 0 && !turnOnLed) {
      dimmingHigh = true;
      ledState -= 1;
      ledcWrite(PWM1_Ch, ledState);
    } else {
      switching = false;
    }
  }
  iterate = false;
  if (ledState > maxLedIntensity) {
    ledState = maxLedIntensity;
  }
  if (ledState < 0) {
    ledState = 0;
  }
}
