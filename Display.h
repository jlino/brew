/*
  Display.h - Display functions header file.
  Created by João Lino, September 25, 2015.
  Released into the public domain.
*/

#ifndef DISPLAY_h
#define DISPLAY_h

#include <arduino.h>

#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#include "config.h"
#include "CustomDataStructures.h"

boolean lcdPrint(LiquidCrystal_I2C *lcd, String title, String message);

void xPaintStatusTemplate(LiquidCrystal_I2C *lcd, boolean cooking);

boolean displayStatus(LiquidCrystal_I2C *lcd, boolean cooking, float cookTemperature, float baseTemperature, float upTemperature, float downTemperature, unsigned long clockCounter, boolean repaint);

boolean displayGenericMenu( LiquidCrystal_I2C *lcd, MenuData *data );

#endif
