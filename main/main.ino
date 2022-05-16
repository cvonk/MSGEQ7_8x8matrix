/**
 * @file main.ino
 * @author Coert Vonk
 * @brief Spectrum analyzer display using MSGEQ7, 8x8 bicolor matrix and Arduino Uno. (https://github.com/cvonk/MSGEQ7_8x8matrix)
 * @version 1.0
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

#define STROBE_PIN (7)
#define RESET_PIN (8)
#define ANALOG_PIN (0)

typedef uint8_t sample_t;
#define MSGEQ7_CHANNEL_CNT (7)

void
_update_matrix(Adafruit_BicolorMatrix * matrix, sample_t const * const spectrum)
{
    static sample_t peak[MSGEQ7_CHANNEL_CNT + 1] = {}; // peak level of each column
    static uint_least8_t dotCount = 0;

    matrix->clear();
    matrix->fillRect(0, 0, 3, MSGEQ7_CHANNEL_CNT, LED_GREEN);  // bottom 3 LEDs are green
    matrix->fillRect(3, 0, 2, MSGEQ7_CHANNEL_CNT, LED_YELLOW); // middle 2 LEDs are yellow
    matrix->fillRect(5, 0, 3, MSGEQ7_CHANNEL_CNT, LED_RED);    // top 3 LEDs are red

    for (int xx = 0; xx < MSGEQ7_CHANNEL_CNT; xx++) {

        if (spectrum[xx] > peak[xx]) {  // mark peak
            peak[xx] = spectrum[xx];
        }

        if (peak[xx] == 0) {
            matrix->drawLine(0, xx, 7, xx, LED_OFF);
            continue;
        }
        if (spectrum[xx] < 7) {
            matrix->drawLine(spectrum[xx], xx, 7, xx, LED_OFF);
        }

        matrix->drawPixel(peak[xx] - 1, xx, peak[xx] - 1 < 3 ? LED_GREEN : 
                                                  peak[xx] - 1 < 5 ? LED_YELLOW : LED_RED);
    }
    matrix->writeDisplay();

    // make the peak pixels drop
    if (++dotCount >= 50) {
        dotCount = 0;
        for (uint_least8_t xx = 0; xx < MSGEQ7_CHANNEL_CNT; xx++) {
            if (peak[xx] > 0) peak[xx]--;
        }
    }
}

namespace {
    Adafruit_BicolorMatrix * _matrix;
}

void setup() 
{
    Serial.begin(9600);

    _matrix = new Adafruit_BicolorMatrix();
    _matrix->begin(0x70);

    pinMode(RESET_PIN, OUTPUT);
    pinMode(STROBE_PIN, OUTPUT);
}

void loop() 
{
    digitalWrite(RESET_PIN, HIGH); // reset the MSGEQ7's counter
    delay(5);
    digitalWrite(RESET_PIN, LOW);

    sample_t samples[MSGEQ7_CHANNEL_CNT];

    for (int ii = 0; ii < MSGEQ7_CHANNEL_CNT; ii++){
        digitalWrite(STROBE_PIN, LOW); // output each DC value for each freq band
        delayMicroseconds(35); // to allow the output to settle
 
        int spectrumRead = analogRead(ANALOG_PIN);
        if (spectrumRead < 150) {
            spectrumRead /= 2;
        }
        samples[ii] = map(spectrumRead, 0, 950, 0, 8);  // scale to [0..8]
        digitalWrite(STROBE_PIN, HIGH);
    }

    for (int ii = 0; ii < MSGEQ7_CHANNEL_CNT; ii++){
        _update_matrix(_matrix, samples);
    }
}