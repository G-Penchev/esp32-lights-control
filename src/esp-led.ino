#include <Arduino.h>

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
const int fadeStep = 1; // Adjust the step size for fading

const int freq = 5000;
const int resolution = 10;

void setup()
{
    Serial.begin(115200);

    for (int i = 0; i < numButtons; i++)
    {
        pinMode(buttonPins[i], INPUT_PULLUP);
        ledcSetup(i, freq, resolution);
        ledcAttachPin(ledPins[i], i);
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
                if (dimmingDirection[i] && brightness[i] < 1023)
                {
                    brightness[i] += fadeStep;
                }
                else if (!dimmingDirection[i] && brightness[i] > 0)
                {
                    brightness[i] -= fadeStep;
                }
                ledState[i] = brightness[i] != 0;
                dimLed(brightness[i], i);
            }
        }
        else
        {
            if (buttonPressDuration[i] < 250 && buttonPressDuration[i] > 20)
            {
                // Short press
                Serial.print("Short press detected = ");
                Serial.println(ledState[i]);
                ledState[i] = !ledState[i];
                dimmingDirection[i] = ledState[i];
                brightness[i] = ledState[i] ? 1023 : 0;
                dimLed(brightness[i], i);
            }
            // Button released
            longPressActive[i] = false;
            buttonPressDuration[i] = 0;
        }

        lastButtonState[i] = buttonState[i];
    }
}

void dimLed(int value, int channel){
    Serial.print("Dim set to = ");
    Serial.print(value);
    Serial.print(" and mappedValue set to = ");

    int mappedValue = int(pow(2, value / 102.3) - 1);
    Serial.println(mappedValue);
    ledcWrite(channel, mappedValue);
}