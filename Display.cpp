/*
  Display.cpp - Display functions code file.
  Created by João Lino, September 25, 2015.
  Released into the public domain.
*/

#include "Display.h"

boolean lcdPrint(LiquidCrystal_I2C *lcd, String title, String message) {
  int messageLength = message.length();
  
  lcd->clear();
    
  // print title
  lcd->home();
  lcd->print(title);
    
  // print message
  if(messageLength <= LCD_HORIZONTAL_RESOLUTION) {
    lcd->setCursor(0,LCD_VERTICAL_RESOLUTION-1);
    lcd->print(message);
    delay(1000);
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
        // Wait until switch is released
        while (digitalRead(ROTARY_ENCODER_SW_PIN)) {}  
        
        // debounce
        delay(10);

        // Job is done, break the circle
        return false;
      } else {
        lcd->setCursor(0,LCD_VERTICAL_RESOLUTION-1);
        lcd->print(output_message.substring(cursor, cursor+16));
        delay(500);
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

boolean displayMainMenu(LiquidCrystal_I2C *lcd, eMainMenuOptions position, boolean repaint) {
  boolean ret = repaint;

  if(repaint) {
    // display menu
    lcd->clear();
    lcd->home (); // go home
    lcd->print("Brewery Menu");
    
    switch(position) {
      case eMainMenu_GO_FROM_STAGE: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> GO FROM STAGE");
        break;
      }
      case eMainMenu_GO: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> GO           ");
        break;
      }
      case eMainMenu_STOP: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> STOP         ");
        break;
      }
      case eMainMenu_SKIP: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> SKIP         ");
        break;
      }
      case eMainMenu_BeerProfile: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> BeerProfile  ");
        break;
      }
      case eMainMenu_Stage: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Stages       ");
        break;
      }
      case eMainMenu_Malt: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Malt         ");
        break;
      }
      case eMainMenu_Hops: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Hops         ");
        break;
      }
      case eMainMenu_Clean: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Clean        ");
        break;
      }
      case eMainMenu_Purge: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Purge        ");
        break;
      }
      case eMainMenu_Settings: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Settings     ");
        break;
      }
      case eMainMenu_Back: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Back         ");
        break;
      }
      default: {
        // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
        //xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_GO, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      } 
    }
    
    ret = false;
  }

  return ret;
}

boolean displayBeerProfileMenu(LiquidCrystal_I2C *lcd, eBeerProfileMenuOptions position, boolean repaint) {
  boolean ret = repaint;

  if(repaint) {
    // display menu
    lcd->clear();
    lcd->home (); // go home
    lcd->print("Preset Menu");

    switch(position) {
      case eBeerProfileMenu_Basic: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Basic        ");
        break;
      }
      case eBeerProfileMenu_Trigo: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Trigo        ");
        break;
      }
      case eBeerProfileMenu_IPA: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> IPA          ");
        break;
      }
      case eBeerProfileMenu_Belga: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Belga        ");
        break;
      }
      case eBeerProfileMenu_Red: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Red          ");
        break;
      }
      case eBeerProfileMenu_APA: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> APA          ");
        break;
      }
      case eBeerProfileMenu_Custom: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Custom       ");
        break;
      }
      case eBeerProfileMenu_Back: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Back         ");
        break;
      }
      default: {
        // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
        //xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_GO, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      } 
    }
    
    ret = false;
  }

  return ret;
}
boolean displayStageMenu(LiquidCrystal_I2C *lcd, eStageMenuOptions position, boolean repaint) {
  boolean ret = repaint;

  if(repaint) {
    // display menu
    lcd->clear();
    lcd->home (); // go home
    lcd->print("Stage Menu");

    switch(position) {
      case eStageMenu_Startpoint: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Startpoint   ");
        break;
      }
      case eStageMenu_BetaGlucanase: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> BetaGlucanase");
        break;
      }
      case eStageMenu_Debranching: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Debranching  ");
        break;
      }
      case eStageMenu_Proteolytic: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Proteolytic  ");
        break;
      }
      case eStageMenu_BetaAmylase: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Beta Amylase ");
        break;
      }
      case eStageMenu_AlphaAmylase: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Alpha Amylase");
        break;
      }
      case eStageMenu_Mashout: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Mashout      ");
        break;
      }
      case eStageMenu_Recirculation: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Recirculation");
        break;
      }
      case eStageMenu_Sparge: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Sparge       ");
        break;
      }
      case eStageMenu_Boil: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Boil         ");
        break;
      }
      case eStageMenu_Cooling: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Cooling      ");
        break;
      }
      case eStageMenu_Back: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Back         ");
        break;
      }
      default: {
        // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
        //xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_GO, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      } 
    }
    
    ret = false;
  }

  return ret;
}

boolean displayMaltMenu(LiquidCrystal_I2C *lcd, eMaltMenuOptions position, boolean repaint) {
  boolean ret = repaint;

  if(repaint) {
    // display menu
    lcd->clear();
    lcd->home (); // go home
    lcd->print("Malt Menu");

    switch(position) {
      case eMaltMenu_CastleMalting_Chteau_Pilsen_2RS: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> CM Ch. Pilsen");
        break;
      }
      case eMaltMenu_CastleMalting_Wheat_Blanc: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> CM Wheat Blan");
        break;
      }
      case eMaltMenu_Back: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Back         ");
        break;
      }
      default: {
        // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
        //xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_GO, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      } 
    }
    
    ret = false;
  }

  return ret;
}

boolean displaySettingsMenu(LiquidCrystal_I2C *lcd, eSettingsMenuOptions position, boolean repaint) {
  boolean ret = repaint;

  if(repaint) {
    // display menu
    lcd->clear();
    lcd->home (); // go home
    lcd->print("Settings Menu");

    switch(position) {
      case eSettingsMenu_PT100_Element: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> PT100 Element");
        break;
      }
      case eSettingsMenu_PT100_Up: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> PT100 Up     ");
        break;
      }
      case eSettingsMenu_PT100_Down: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> PT100 Down   ");
        break;
      }
      case eSettingsMenu_Back: {
        lcd->setCursor (0,1);        // go to start of 2nd line
        lcd->print("-> Back         ");
        break;
      }
      default: {
        // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
        //xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_GO, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      } 
    }
    
    ret = false;
  }

  return ret;
}
