// display.cpp
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include "display.h"

DisplayClass::DisplayClass(void)
{
}

boolean DisplayClass::begin(void)  // returns false on error
{
    this->matrix = new Adafruit_BicolorMatrix();
    if (!this->matrix) {
        return false;
    }
    this->matrix->begin(0x70);
    return true;
}

void DisplayClass::write(sample_t const * const spectrum)
{
    static sample_t peak[MSGEQ7_CHANNEL_CNT + 1] = {}; // peak level of each column
    static uint_least8_t dotCount = 0;

    this->matrix->clear();
    this->matrix->fillRect(0, 0, 3, MSGEQ7_CHANNEL_CNT, LED_GREEN);  // bottom 3 LEDs are green
    this->matrix->fillRect(3, 0, 2, MSGEQ7_CHANNEL_CNT, LED_YELLOW); // middle 2 LEDs are yellow
    this->matrix->fillRect(5, 0, 3, MSGEQ7_CHANNEL_CNT, LED_RED);    // top 3 LEDs are red

    for (int xx = 0; xx < MSGEQ7_CHANNEL_CNT; xx++) {

        if (spectrum[xx] > peak[xx]) {  // mark peak
            peak[xx] = spectrum[xx];
        }

        if (peak[xx] == 0) {
            this->matrix->drawLine(0, xx, 7, xx, LED_OFF);
            continue;
        }
        if (spectrum[xx] < 7) {
            this->matrix->drawLine(spectrum[xx], xx, 7, xx, LED_OFF);
        }

        this->matrix->drawPixel(peak[xx] - 1, xx, peak[xx] - 1 < 3 ? LED_GREEN : 
                                                  peak[xx] - 1 < 5 ? LED_YELLOW : LED_RED);
    }

    this->matrix->writeDisplay();

    // every third frame, make the peak pixels drop by 1:
    if (++dotCount >= 50) {
        dotCount = 0;
        for (uint_least8_t xx = 0; xx < MSGEQ7_CHANNEL_CNT; xx++) {
            if (peak[xx] > 0) peak[xx]--;
        }
    }
}

void DisplayClass::dump(sample_t const * const spectrum)
{
    for (int xx = 0; xx < MSGEQ7_CHANNEL_CNT; xx++) {
        if (spectrum[xx] < 100) Serial.print(" ");
        if (spectrum[xx] < 10) Serial.print(" ");
        Serial.print(spectrum[xx]); Serial.print(" ");
    }
}
