/**
 * @file main.ino
 * @author Coert Vonk
 * @brief 
 * @version 0.1
 * @date 2022-05-16
 * 
 * For testing, use https://www.szynalski.com/tone-generator/
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>  // git@github.com:adafruit/Adafruit-GFX-Library.git
#include <Adafruit_LEDBackpack.h>  // git@github.com:adafruit/Adafruit_LED_Backpack.git
#include "sample_t.h"
#include "display.h"

#define msg7Strobe 7
#define msg7RESET 8
#define msg7DCout 0
//const int LEDpins[7] = {3,5,5,6,9,9,10}; // there are 5 LEDs and 7 freq bands. So, repeat LEDs

#define pushButton 2

DisplayClass _display;

void setup() 
{

    Serial.begin(9600);
    _display.begin();

   pinMode(msg7RESET, OUTPUT);
   pinMode(msg7Strobe, OUTPUT);

   pinMode(pushButton, INPUT); // never actually used in this example.
   digitalWrite(pushButton, HIGH); // Enable internal pull-up
}

void loop() 
{
    digitalWrite(msg7RESET, HIGH); // reset the MSGEQ7's counter
    delay(5);
    digitalWrite(msg7RESET, LOW);

    sample_t samples[MSGEQ7_CHANNEL_CNT];

    for (int ii = 0; ii < MSGEQ7_CHANNEL_CNT; ii++){
        digitalWrite(msg7Strobe, LOW); // output each DC value for each freq band
        delayMicroseconds(35); // to allow the output to settle
 
        int spectrumRead = analogRead(msg7DCout);
        if (spectrumRead < 150) {
            spectrumRead /= 2;
        }
        samples[ii] = map(spectrumRead, 0, 950, 0, 8);  // scale to [0..8]
        digitalWrite(msg7Strobe, HIGH);
    }

    for (int ii = 0; ii < MSGEQ7_CHANNEL_CNT; ii++){
        _display.write(samples);
    }
}