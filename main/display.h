// display.cpp
#pragma once
#include <Arduino.h>
#include "sample_t.h"

class DisplayClass
{
  public:
      DisplayClass( void );
      boolean begin( void );  // returns false on error
      void write( sample_t const * const spectrum );
      void dump( sample_t const * const spectrum );
  private:
      Adafruit_BicolorMatrix * matrix;
};
