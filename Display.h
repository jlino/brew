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
boolean displayMainMenu(LiquidCrystal_I2C *lcd, eMainMenuOptions position, boolean repaint);
boolean displayBeerProfileMenu(LiquidCrystal_I2C *lcd, eBeerProfileMenuOptions position, boolean repaint);
boolean displayStageMenu(LiquidCrystal_I2C *lcd, eStageMenuOptions position, boolean repaint);
boolean displayMaltMenu(LiquidCrystal_I2C *lcd, eMaltMenuOptions position, boolean repaint);
boolean displaySettingsMenu(LiquidCrystal_I2C *lcd, eSettingsMenuOptions position, boolean repaint);

#endif
