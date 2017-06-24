/*
  Display.cpp - Display functions code file.
  Created by João Lino, September 25, 2015.
  Released into the public domain.
*/

#include "Display.h"

boolean lcdPrint(LiquidCrystal_I2C *lcd, String title, String message) {
  int messageLength = message.length();
  /*unsigned long timeToReadTheMessage = messageLength * 16;
  unsigned long numberOfTimeDivisions = messageLength - LCD_HORIZONTAL_RESOLUTION;
  int messagePosition(millis()%timeToReadTheMessage)/numberOfTimeDivisions*/
  
  lcd->clear();
    
  // print title
  lcd->home();
  lcd->print(title);
    
  // print message
  if(messageLength <= LCD_HORIZONTAL_RESOLUTION) {
    lcd->setCursor(0,LCD_VERTICAL_RESOLUTION-1);
    lcd->print(message);
    delay(50);
  }
  // print scrolling message
  else {
    String output_message = "                ";
    output_message += message;
    messageLength = output_message.length();
    
    // Adjust the message size for proper printing
    if ( messageLength & 1 == 1 ) {
      output_message+=" ";
      messageLength+=2;
    }
    
    // print scrolling message
    for (int cursor = 0; cursor < messageLength - LCD_HORIZONTAL_RESOLUTION; cursor+=2) {
      if ((digitalRead(ROTARY_ENCODER_SW_PIN))) {
        while (digitalRead(ROTARY_ENCODER_SW_PIN)) {}           // Wait until switch is released
        return false;                                           // Job is done, break the circle
      } else {
        lcd->setCursor(0,LCD_VERTICAL_RESOLUTION-1);
        lcd->print(output_message.substring(cursor, cursor+16));
        delay(250);
      }
    }
  }

  return true;
}

void xPaintStatusTemplate(LiquidCrystal_I2C *lcd, boolean cooking) {
  // Clear LCD
  lcd->clear();        

  // Position the cursor at the begining of where the temperature template goes onto the screen
  lcd->home();    

  // Print the target and measured temperature template
  if(cooking) {
    lcd->print("ON  XX 000.0/000C");
  }
  else {
    lcd->print("OFF XX 000.0/000C");
  }

  // Position the cursor at the begining of where the mode and time template goes onto the screen
  lcd->setCursor (0,LCD_VERTICAL_RESOLUTION-1);

  lcd->print("****       00:00");
}

boolean displayStatus(LiquidCrystal_I2C *lcd, boolean cooking, float cookTemperature, float baseTemperature, float upTemperature, float downTemperature, unsigned long clockCounter, boolean repaint) {
	boolean ret = repaint;

  // Check whether a template repaint is required
  if(repaint) {
    // Repaint the LCD template
    xPaintStatusTemplate(lcd, cooking);
    
    // Reset the repaint flag after the repaint has been done
    ret = false;
  }
  
  double displayTemperature = 0.0;
  unsigned long ulTimeToShow = millis() % 6000;

  lcd->setCursor (4,0);
  if(ulTimeToShow < 2000) {
    displayTemperature = baseTemperature;

    lcd->print("TS");
  }
  else {
    if(ulTimeToShow < 4000) {
      displayTemperature = upTemperature;

      lcd->print("UP");
    }
    else {
      displayTemperature = downTemperature;

      lcd->print("DW");
    }
  }

  // Print positions with no numbers, before the measured temperature value
  lcd->setCursor (7,0);
  if (displayTemperature < 10) {
    lcd->print("  ");
  }
  else {
    if (displayTemperature < 100) {
      lcd->print(" ");
    }
  }

  // Print measured temperature value onto the LCD
  lcd->print(displayTemperature, 1);

  // Print positions with no numbers, before the target temperature value
  lcd->setCursor (13,0);
  if (cookTemperature < 10) {
    lcd->print("  ");
  }
  else {
    if (cookTemperature < 100) {
      lcd->print(" ");
    }
  }

  // Print target temperature value onto the LCD
  lcd->print(cookTemperature);
  
  // Calculate the numbers on the timer clock
  unsigned long minutes = clockCounter / 1000 / 60;
  unsigned long seconds = (clockCounter / 1000) % 60;

  // Position the cursor at the begining of where the timer goes onto the screen
  lcd->setCursor (10, 1);
  
  // Print the timer values onto the LCD
  if (minutes < 10) {
    lcd->print(" 0");
  }
  else {
    if (minutes < 100) {
      lcd->print(" ");
    }
  }
  lcd->print(minutes);
  lcd->print(":");
  if(seconds<10) {
    lcd->print("0");
  }
  lcd->print(seconds);

  return ret;
}

boolean displayGenericMenu( LiquidCrystal_I2C *lcd, MenuData *data ) {
  boolean repaintRequired = data->_repaint;
  if(repaintRequired) {
    lcd->clear();
    lcd->home ();                   // go home
    lcd->print(data->_title);
    lcd->setCursor (0,1);           // go to start of 2nd line
    lcd->print((data->_dialog)[data->_position]);
    repaintRequired = false;
  }
  return repaintRequired;
}

