/*
  brew.ino - Main execution file.
  Created by João Lino, August 28, 2014.
  Released into the public domain.
*/

#define DEBUG

// ######################### LIBRARIES #########################

// ++++++++++++++++++++++++ LiquidCrystal_I2C ++++++++++++++++++++++++
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

// ++++++++++++++++++++++++ PT100 +++++++++++++++++++++++++++++++++
//#include <PT100.h>

// ++++++++++++++++++++++++ OTHER +++++++++++++++++++++++++++++++++
#include "debug.h"

#include "config.h"

#include "CustomDataStructures.h"

#include "Melody.h"
#include "Display.h"
#include "Temperature.h"
#include "Profiles.h"

#include "brew.h"

// ######################### VARIABLES #########################
// ++++++++++++++++++++++++ State Machine ++++++++++++++++++++++++
eRotaryEncoderMode      rotaryEncoderMode;

eCookingStages          cookingStage;
eBeerProfile            beerProfile;

eMenuType               eMenuType;

MenuData                mdMainMenu;
MenuData                mdStartFromStageMenu;
MenuData                mdBeerProfileMenu;
MenuData                mdStageMenu;
MenuData                mdMaltMenu;
MenuData                mdSettingsMenu;

eMainMenuOptions        eMainMenuPosition;
eMainMenuOptions        eMainMenuSelection;
eStageMenuOptions       eStartFromStageMenuPosition;
eStageMenuOptions       eStartFromStageMenuSelection;
eBeerProfileMenuOptions eBeerProfileMenuPosition;
eBeerProfileMenuOptions eBeerProfileMenuSelection;
eStageMenuOptions       eStageMenuPosition;
eStageMenuOptions       eStageMenuSelection;
eMaltMenuOptions        eMaltMenuPosition;
eMaltMenuOptions        eMaltMenuSelection; 
eSettingsMenuOptions    eSettingsMenuPosition;
eSettingsMenuOptions    eSettingsMenuSelection;

// ++++++++++++++++++++++++ Global Variables ++++++++++++++++++++++++
boolean                 cooking;

unsigned long           clockStartTime;
unsigned long           clockLastUpdate;
long                    clockCounter;
unsigned long           clockIgnore;
boolean                 clockStart;
boolean                 clockEnd;

unsigned long           cookTime;
int                     cookTemperature;
//cook_mode_list        cookMode;
//int                   cookMixerSpeed;
int                     finalYield;
        
unsigned long           startpointTime;
unsigned long           betaGlucanaseTime;
unsigned long           debranchingTime;
unsigned long           proteolyticTime;
unsigned long           betaAmylaseTime;
unsigned long           alphaAmylaseTime;
unsigned long           mashoutTime;
unsigned long           recirculationTime;
unsigned long           spargeTime;
unsigned long           boilTime;
unsigned long           coolingTime;
unsigned long           cleaningTime;
        
int                     startpointTemperature;
int                     betaGlucanaseTemperature;
int                     debranchingTemperature;
int                     proteolyticTemperature;
int                     betaAmylaseTemperature;
int                     alphaAmylaseTemperature;
int                     mashoutTemperature;
int                     recirculationTemperature;
int                     spargeTemperature;
int                     boilTemperature;
int                     coolingTemperature;
int                     cleaningTemperature;

boolean                 refresh;
boolean                 repaint;

boolean                 bStatusElement;

// ++++++++++++++++++++++++ Interrupts ++++++++++++++++++++++++
static unsigned long    lastInterruptTime;

// ++++++++++++++++++++++++ Rotary Encoder ++++++++++++++++++++++++
volatile int            rotaryEncoderVirtualPosition = 0;
volatile int            rotaryEncoderMaxPosition = 1;
volatile int            rotaryEncoderMinPosition = 0;
volatile int            rotaryEncoderSingleStep = 1;
volatile int            rotaryEncoderMultiStep = 1;

volatile boolean        onISR = false;

unsigned long           rotarySwDetectTime;

// ++++++++++++++++++++++++ Heating Element Relay ++++++++++++++++++++++++
int                     iWindowSize;             // Time frame to operate in
unsigned long           windowStartTime;
double                  dWattPerPulse;

// ++++++++++++++++++++++++ Pump ++++++++++++++++++++++++
int                     iPumpSpeed;             // Time frame to operate in

// ######################### INITIALIZE #########################
// ++++++++++++++++++++++++ Library - LiquidCrystal_I2C ++++++++++++++++++++++++
LiquidCrystal_I2C       lcd(LCD_I2C_ADDR, LCD_EN_PIN, LCD_RW_PIN, LCD_RS_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

// +++++++++++++++++++++++ Temperature +++++++++++++++++++++++
Temperature                   basePT100("base", 
                          PT100_BASE_OUTPUT_PIN, 
                          PT100_BASE_INPUT_PIN, 
                          PT100_BASE_TIME_BETWEEN_READINGS, 
                          2.0298, 2.0259, 665.24, 662.17);
Temperature                   upPT100("up", 
                          PT100_UP_OUTPUT_PIN, 
                          PT100_UP_INPUT_PIN,  
                          PT100_UP_TIME_BETWEEN_READINGS, 
                          2.0274, 2.0245, 659.43, 656.72);
Temperature                   downPT100("down", 
                          PT100_DOWN_OUTPUT_PIN,  
                          PT100_DOWN_INPUT_PIN, 
                          PT100_DOWN_TIME_BETWEEN_READINGS, 
                          2.0309, 2.0288, 658.15, 655.35);

// ######################### INTERRUPTS #########################
void isr ()  {    // Interrupt service routine is executed when a HIGH to LOW transition is detected on CLK
  unsigned long interruptTime = millis();
  unsigned long diff = interruptTime - lastInterruptTime;
  
  // If interrupts come faster than [ROTARY_ENCODER_DEBOUNCE_TIME]ms, assume it's a bounce and ignore
  if (diff > ROTARY_ENCODER_DEBOUNCE_TIME) {
    lastInterruptTime = interruptTime;
  
    switch(rotaryEncoderMode) {
      // Input of rotary encoder controling menus
      case eRotaryEncoderMode_Menu: {
        if (!digitalRead(ROTARY_ENCODER_DT_PIN)) {
            rotaryEncoderVirtualPosition = (rotaryEncoderVirtualPosition + rotaryEncoderSingleStep);
        }
        else {
            rotaryEncoderVirtualPosition = rotaryEncoderVirtualPosition - rotaryEncoderSingleStep;
        }
        if (rotaryEncoderVirtualPosition > rotaryEncoderMaxPosition) {
            rotaryEncoderVirtualPosition = rotaryEncoderMinPosition;
        }
        if (rotaryEncoderVirtualPosition < rotaryEncoderMinPosition) {
            rotaryEncoderVirtualPosition = rotaryEncoderMaxPosition;
        }
        
        break;
      }
      
      // Input of rotary encoder controling time variables
      case eRotaryEncoderMode_Time: {
        if (!digitalRead(ROTARY_ENCODER_DT_PIN)) {
          if(rotaryEncoderVirtualPosition >= 60) {
            rotaryEncoderVirtualPosition = (rotaryEncoderVirtualPosition + rotaryEncoderMultiStep);
          }
          else {
            rotaryEncoderVirtualPosition = (rotaryEncoderVirtualPosition + rotaryEncoderSingleStep);
          }
        }
        else {
          if(rotaryEncoderVirtualPosition == rotaryEncoderMinPosition) {
            rotaryEncoderVirtualPosition = (rotaryEncoderVirtualPosition + 60);
          }
          else {
            if(rotaryEncoderVirtualPosition >= (60 + rotaryEncoderMultiStep)) {
              rotaryEncoderVirtualPosition = (rotaryEncoderVirtualPosition - rotaryEncoderMultiStep);
            }
            else {
              rotaryEncoderVirtualPosition = rotaryEncoderVirtualPosition - rotaryEncoderSingleStep;
            }
          }
        }
        if (rotaryEncoderVirtualPosition > rotaryEncoderMaxPosition) {
            rotaryEncoderVirtualPosition = rotaryEncoderMaxPosition;
        }
        if (rotaryEncoderVirtualPosition < rotaryEncoderMinPosition) {
            rotaryEncoderVirtualPosition = rotaryEncoderMinPosition;
        }
        
        break;
      }
      
      // Input of rotary encoder controling generic integer variables within a range between rotaryEncoderMinPosition and rotaryEncoderMaxPosition
      case eRotaryEncoderMode_Generic: {
        if (!digitalRead(ROTARY_ENCODER_DT_PIN)) {
          rotaryEncoderVirtualPosition = (rotaryEncoderVirtualPosition + rotaryEncoderSingleStep);
        }
        else {
          rotaryEncoderVirtualPosition = (rotaryEncoderVirtualPosition - rotaryEncoderSingleStep);
        }
        if (rotaryEncoderVirtualPosition > rotaryEncoderMaxPosition) {
            rotaryEncoderVirtualPosition = rotaryEncoderMaxPosition;
        }
        if (rotaryEncoderVirtualPosition < rotaryEncoderMinPosition) {
            rotaryEncoderVirtualPosition = rotaryEncoderMinPosition;
        }
       
        break;
      }
      default: {
        
      }
    }
    
    repaint = true;
    refresh = true;
  }
}

void xSetupRotaryEncoder( eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep ) {
  if( newMode >= 0 ) rotaryEncoderMode = newMode;
  if( newPosition >= 0 ) rotaryEncoderVirtualPosition = newPosition;
  if( newMaxPosition >= 0 ) rotaryEncoderMaxPosition = newMaxPosition;
  if( newMinPosition >= 0 ) rotaryEncoderMinPosition = newMinPosition;
  if( newSingleStep >= 0 ) rotaryEncoderSingleStep = newSingleStep;
  if( newMultiStep >= 0 ) rotaryEncoderMultiStep = newMultiStep;
}

// ######################### START #########################
void xSafeHardwarePowerOff() {
  // Turn off gracefully
  iPumpSpeed = PUMP_SPEED_STOP_MOSFET;
  xRegulatePumpSpeed();

  // Force shutdown
  analogWrite(PUMP_PIN, PUMP_SPEED_STOP_MOSFET);  // analogWrite values from 0 to 255
  digitalWrite(HEATING_ELEMENT_OUTPUT_PIN, LOW);  // Turn heading element OFF for safety
  bStatusElement = false;

  basePT100.setPumpStatus( false );
  upPT100.setPumpStatus( false );
  downPT100.setPumpStatus( false );

  //analogWrite(MIXER_PIN, 0);        // Turn mixer OFF for safety
}

void xWelcomeUser() {
  //#ifndef DEBUG
  lcdPrint(&lcd, "  Let's start", "    Brewing!");    // Write welcome

  // Play Melody;
  sing(MELODY_SUPER_MARIO_START, PIEZO_PIN);

  //termometerCalibration();
  delay(SETTING_WELCOME_TIMEOUT);      // pause for effect
  //#endif
}

void setup() {
  // ++++++++++++++++++++++++ Rotary Encoder ++++++++++++++++++++++++
  pinMode                         (ROTARY_ENCODER_CLK_PIN,INPUT);
  pinMode                         (ROTARY_ENCODER_DT_PIN, INPUT);
  pinMode                         (ROTARY_ENCODER_SW_PIN, INPUT);
  attachInterrupt                 (ROTARY_ENCODER_INTERRUPT_NUMBER, isr, FALLING);

  // ++++++++++++++++++++++++ Heating Element Relay ++++++++++++++++++++++++
  pinMode                         (HEATING_ELEMENT_OUTPUT_PIN, OUTPUT);
  digitalWrite                    (HEATING_ELEMENT_OUTPUT_PIN, LOW);
  bStatusElement              =   false;
  windowStartTime             =   millis();
  dWattPerPulse               =   HEATING_ELEMENT_MAX_WATTAGE / HEATING_ELEMENT_AC_FREQUENCY_HZ;

  // ++++++++++++++++++++++++ Mixer ++++++++++++++++++++++++

  // ++++++++++++++++++++++++ Pump ++++++++++++++++++++++++
  pinMode(PUMP_PIN, OUTPUT);   // sets the pin as output
  iPumpSpeed                  =   PUMP_SPEED_STOP_MOSFET;             // Time frame to operate in
  analogWrite(PUMP_PIN, iPumpSpeed);  // analogWrite values from 0 to 255

  // ++++++++++++++++++++++++ Piezo ++++++++++++++++++++++++
  pinMode(PIEZO_PIN, OUTPUT);

  // ++++++++++++++++++++++++ Temperature Sensor PT100 ++++++++++++++++++++++++

  // ++++++++++++++++++++++++ Serial Monitor ++++++++++++++++++++++++
  Serial.begin                    (SETTING_SERIAL_MONITOR_BAUD_RATE);    // setup terminal baud rate
  Serial.println                  (SETTING_SERIAL_MONITOR_WELCOME_MESSAGE);  // print a start message to the terminal

  // ++++++++++++++++++++++++ Library - LiquidCrystal_I2C ++++++++++++++++++++++++
  lcd.begin                       (LCD_HORIZONTAL_RESOLUTION,LCD_VERTICAL_RESOLUTION);    //  <<----- My LCD was 16x2
  lcd.setBacklightPin             (LCD_BACKLIGHT_PIN,POSITIVE);        // Setup backlight pin
  lcd.setBacklight                (HIGH);              // Switch on the backlight

  // ######################### INITIALIZE #########################
  // ++++++++++++++++++++++++ Rotary Encoder ++++++++++++++++++++++++
  // set operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
  xSetupRotaryEncoder             ( eRotaryEncoderMode_Disabled, 0, 0, 0, 0, 0 );
  rotarySwDetectTime = 0;

  // ++++++++++++++++++++++++ State Machine ++++++++++++++++++++++++
  eMenuType                   =   eMenuType_Main;

  mdMainMenu._position        =   eMainMenu_GO;
  mdMainMenu._selection       =   eMainMenu_NULL;
  
  eMainMenuPosition           =   eMainMenu_GO;
  eMainMenuSelection          =   eMainMenu_NULL;
  eBeerProfileMenuPosition    =   eBeerProfileMenu_Basic;
  eBeerProfileMenuSelection   =   eBeerProfileMenu_NULL;
  eStageMenuPosition          =   eStageMenu_Startpoint;
  eStageMenuSelection         =   eStageMenu_NULL;
  eMaltMenuPosition           =   eMaltMenu_CastleMalting_Chteau_Pilsen_2RS;
  eMaltMenuSelection          =   eMaltMenu_NULL;
  eSettingsMenuPosition       =   eSettingsMenu_PT100_Element;
  eSettingsMenuSelection      =   eSettingsMenu_NULL;

  cookingStage                =   eCookingStage_Startpoint;
  beerProfile                 =   eBeerProfile_Basic;
  // ++++++++++++++++++++++++ Global Variables ++++++++++++++++++++++++

  cooking                     =   false;

  clockStartTime              =   0;
  clockLastUpdate             =   0;
  clockCounter                =   0;
  clockIgnore                 =   0;
  clockStart                  =   false;
  clockEnd                    =   false;
                        
  cookTime                    =   3600;
  cookTemperature             =   25;
  //cookMode                  =   quick_start;
  //cookMixerSpeed            =   120;
  finalYield                  =   25;

  startpointTime              =   PROFILE_BASIC_STARTPOINT_TIME;
  betaGlucanaseTime           =   PROFILE_BASIC_BETAGLUCANASE_TIME;
  debranchingTime             =   PROFILE_BASIC_DEBRANCHING_TIME;
  proteolyticTime             =   PROFILE_BASIC_PROTEOLYTIC_TIME;
  betaAmylaseTime             =   PROFILE_BASIC_BETAAMYLASE_TIME;
  alphaAmylaseTime            =   PROFILE_BASIC_ALPHAAMYLASE_TIME;
  mashoutTime                 =   PROFILE_BASIC_MASHOUT_TIME;
  recirculationTime           =   PROFILE_BASIC_RECIRCULATION_TIME;
  spargeTime                  =   PROFILE_BASIC_SPARGE_TIME;
  boilTime                    =   PROFILE_BASIC_BOIL_TIME;
  coolingTime                 =   PROFILE_BASIC_COOLING_TIME;
  cleaningTime                =   SETTING_CLEANING_TIME;

  startpointTemperature       =   PROFILE_BASIC_STARTPOINT_TEMPERATURE;
  betaGlucanaseTemperature    =   PROFILE_BASIC_BETAGLUCANASE_TEMPERATURE;
  debranchingTemperature      =   PROFILE_BASIC_DEBRANCHING_TEMPERATURE;
  proteolyticTemperature      =   PROFILE_BASIC_PROTEOLYTIC_TEMPERATURE;
  betaAmylaseTemperature      =   PROFILE_BASIC_BETAAMYLASE_TEMPERATURE;
  alphaAmylaseTemperature     =   PROFILE_BASIC_ALPHAAMYLASE_TEMPERATURE;
  mashoutTemperature          =   PROFILE_BASIC_MASHOUT_TEMPERATURE;
  recirculationTemperature    =   PROFILE_BASIC_RECIRCULATION_TEMPERATURE;
  spargeTemperature           =   PROFILE_BASIC_SPARGE_TEMPERATURE;
  boilTemperature             =   PROFILE_BASIC_BOIL_TEMPERATURE;
  coolingTemperature          =   PROFILE_BASIC_COOLING_TEMPERATURE;
  cleaningTemperature         =   SETTING_CLEANING_TEMPERATURE;

  refresh                     =   true;
  repaint                     =   true;

  // ++++++++++++++++++++++++ Interrupts ++++++++++++++++++++++++
  lastInterruptTime           =   0;

  // ++++++++++++++++++++++++ PID  ++++++++++++++++++++++++
  iWindowSize                 =   HEATING_ELEMENT_DEFAULT_WINDOW_SIZE;    // Time frame to operate in

  // ######################### Code - Run Once #########################
  xSafeHardwarePowerOff           ();
  xWelcomeUser                    ();
  
  xSetupRotaryEncoder             ( eRotaryEncoderMode_Menu, eMainMenu_GO, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
}

// ######################### MAIN LOOP #########################

void loop() {
  unsigned long inactivityTime = getInactivityTime();

  if( inactivityTime > SETTING_MAX_INACTIVITY_TIME ) {    // Inactivity check
    if(refresh) {
      repaint = true;
      refresh = false;
    }
    repaint = displayStatus( &lcd, cooking, cookTemperature, basePT100.getCurrentTemperature(), upPT100.getCurrentTemperature(), downPT100.getCurrentTemperature(), clockCounter, repaint );
  }
  else {
    runMenu();
  }
  
  xManageMachineSystems();
}

// ######################### FUNCTIONS ########################

void runMenu() {
  #ifdef DEBUG_OFF
  boolean debug_go = repaint;
  if(debug_go) {
    debugPrintFunction("runMenu");
    debugPrintVar("repaint", repaint);
    debugPrintVar("eMenuType", eMenuType);
    debugPrintVar("rotaryEncoderVirtualPosition", rotaryEncoderVirtualPosition);
  }
  #endif

  switch(eMenuType) {
    case eMenuType_Main: {
      eMainMenuPosition = static_cast<eMainMenuOptions>(rotaryEncoderVirtualPosition);

      repaint = displayMainMenu( &lcd, eMainMenuPosition, repaint );

      if ( gotButtonPress( ROTARY_ENCODER_SW_PIN ) ) {
        eMainMenuSelection = eMainMenuPosition;
      }

      runMainMenuSelection();

      break;
    }
    case eMenuType_StartFromStage: {
      eStartFromStageMenuPosition = static_cast<eStageMenuOptions>(rotaryEncoderVirtualPosition);
      
      repaint = displayStageMenu( &lcd, eStartFromStageMenuPosition, repaint );

      if ( gotButtonPress( ROTARY_ENCODER_SW_PIN ) ) {
        eStartFromStageMenuSelection = eStartFromStageMenuPosition;
      }

      runStartFromStageSelection();
      
      break;
    }
    case eMenuType_BeerProfile: {
      eBeerProfileMenuPosition = static_cast<eBeerProfileMenuOptions>(rotaryEncoderVirtualPosition);
      
      repaint = displayBeerProfileMenu( &lcd, eBeerProfileMenuPosition, repaint );

      if ( gotButtonPress( ROTARY_ENCODER_SW_PIN ) ) {
        eBeerProfileMenuSelection = eBeerProfileMenuPosition;
      }

      runBeerProfileSelection();
      
      break;
    }
    case eMenuType_Stage: {
      eStageMenuPosition = static_cast<eStageMenuOptions>(rotaryEncoderVirtualPosition);
      
      repaint = displayStageMenu( &lcd, eStageMenuPosition, repaint );

      if ( gotButtonPress( ROTARY_ENCODER_SW_PIN ) ) {
        eStageMenuSelection = eStageMenuPosition;
      }

      runStageSelection();
      
      break;
    }
    case eMenuType_Malt: {
      eMaltMenuPosition = static_cast<eMaltMenuOptions>(rotaryEncoderVirtualPosition);
      
      repaint = displayMaltMenu( &lcd, eMaltMenuPosition, repaint );

      if ( gotButtonPress( ROTARY_ENCODER_SW_PIN ) ) {
        eMaltMenuSelection = eMaltMenuPosition;
      }

      runMaltSelection();
      
      break;
    }
    case eMenuType_Settings: {
      eSettingsMenuPosition = static_cast<eSettingsMenuOptions>(rotaryEncoderVirtualPosition);
      
      repaint = displaySettingsMenu( &lcd, eSettingsMenuPosition, repaint );

      if ( gotButtonPress( ROTARY_ENCODER_SW_PIN ) ) {
        eSettingsMenuSelection = eSettingsMenuPosition;
      }

      runSettingsSelection();
      
      break;
    }
  }

  #ifdef DEBUG_OFF
  if(debug_go) {
    debugPrintVar("repaint", repaint);
  }
  #endif
}

void runSettingsSelection() {
  switch(eSettingsMenuSelection) {
    case eSettingsMenu_Pump: {
      // Stuff
      if( xSetGenericValue( iPumpSpeed?0:1, 0, 1, "pump", "bool" ) ) {
        iPumpSpeed = PUMP_SPEED_MAX_MOSFET;
      } else {
        iPumpSpeed = PUMP_SPEED_STOP_MOSFET;
      }
      analogWrite(PUMP_PIN, iPumpSpeed);

      backToStatus();
      
      break;
    }
    case eSettingsMenu_PT100_Element: {
      // Stuff

      backToStatus();
      
      break;
    }
    case eSettingsMenu_PT100_Up: {
      // Stuff

      backToStatus();
      
      break;
    }
    case eSettingsMenu_PT100_Down: {
      // Stuff

      backToStatus();
      
      break;
    }
    case eSettingsMenu_Back: {
      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );

      break;
    }
    default: {
    }
  }

  eSettingsMenuSelection = eSettingsMenu_NULL;
}

void runMaltSelection() {
  switch(eMaltMenuSelection) {
    case eMaltMenu_CastleMalting_Chteau_Pilsen_2RS: {
      // Stuff

      backToStatus();
      
      break;
    }
    case eMaltMenu_CastleMalting_Wheat_Blanc: {
      // Stuff

      backToStatus();
      
      break;
    }
    case eMaltMenu_Back: {
      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );

      break;
    }
    default: {
    }
  }

  eMaltMenuSelection = eMaltMenu_NULL;
}

void runStageSelection() {
  switch(eStageMenuSelection) {
    case eStageMenu_Startpoint: {
      startpointTime = getTimer( startpointTime );
  
      startpointTemperature = xSetGenericValue( startpointTemperature, TEMPERATURE_MIN_VALUE, TEMPERATURE_MAX_VALUE, "temperature", "*C" );

      backToStatus();
      
      break;
    }
    case eStageMenu_BetaGlucanase: {
      betaGlucanaseTime = getTimer( betaGlucanaseTime );
  
      betaGlucanaseTemperature = xSetGenericValue( betaGlucanaseTemperature, TEMPERATURE_MIN_VALUE, TEMPERATURE_MAX_VALUE, "temperature", "*C" );

      backToStatus();
      
      break;
    }
    case eStageMenu_Debranching: {
      debranchingTime = getTimer( debranchingTime );
  
      debranchingTemperature = xSetGenericValue( debranchingTemperature, TEMPERATURE_MIN_VALUE, TEMPERATURE_MAX_VALUE, "temperature", "*C" );

      backToStatus();
      
      break;
    }
    case eStageMenu_Proteolytic: {
      proteolyticTime = getTimer( proteolyticTime );
      
      proteolyticTemperature = xSetGenericValue( proteolyticTemperature, TEMPERATURE_MIN_VALUE, TEMPERATURE_MAX_VALUE, "temperature", "*C" );

      backToStatus();
      
      break;
    }
    case eStageMenu_BetaAmylase: {
      betaAmylaseTime = getTimer( betaAmylaseTime );
  
      betaAmylaseTemperature = xSetGenericValue( betaAmylaseTemperature, TEMPERATURE_MIN_VALUE, TEMPERATURE_MAX_VALUE, "temperature", "*C" );

      backToStatus();
      
      break;
    }
    case eStageMenu_AlphaAmylase: {
      alphaAmylaseTime = getTimer( alphaAmylaseTime );
  
      alphaAmylaseTemperature = xSetGenericValue( alphaAmylaseTemperature, TEMPERATURE_MIN_VALUE, TEMPERATURE_MAX_VALUE, "temperature", "*C" );

      backToStatus();
      
      break;
    }
    case eStageMenu_Mashout: {
      mashoutTime = getTimer( mashoutTime );
  
      mashoutTemperature = xSetGenericValue( mashoutTemperature, TEMPERATURE_MIN_VALUE, TEMPERATURE_MAX_VALUE, "temperature", "*C" );

      backToStatus();
      
      break;
    }
    case eStageMenu_Recirculation: {
      recirculationTime = getTimer( recirculationTime );
  
      recirculationTemperature = xSetGenericValue( recirculationTemperature, TEMPERATURE_MIN_VALUE, TEMPERATURE_MAX_VALUE, "temperature", "*C" );

      backToStatus();
      
      break;
    }
    case eStageMenu_Sparge: {
      spargeTime = getTimer( spargeTime );
  
      spargeTemperature = xSetGenericValue( spargeTemperature, TEMPERATURE_MIN_VALUE, TEMPERATURE_MAX_VALUE, "temperature", "*C" );

      backToStatus();
      
      break;
    }
    case eStageMenu_Boil: {
      boilTime = getTimer( boilTime );
      
      boilTemperature = xSetGenericValue( boilTemperature, TEMPERATURE_MIN_VALUE, TEMPERATURE_MAX_VALUE, "temperature", "*C" );

      backToStatus();
      
      break;
    }
    case eStageMenu_Cooling: {
      coolingTime = getTimer( coolingTime );
      
      coolingTemperature = xSetGenericValue( coolingTemperature, TEMPERATURE_MIN_VALUE, TEMPERATURE_MAX_VALUE, "temperature", "*C" );

      backToStatus();
      
      break;
    }
    case eStageMenu_Back: {
      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );

      break;
    }
    default: {
    }
  }

  eStageMenuSelection = eStageMenu_NULL;
}

void runBeerProfileSelection() {
  switch(eBeerProfileMenuSelection) {
    case eBeerProfileMenu_Basic: {
      beerProfile                 =   eBeerProfile_Basic;

      startpointTime              =   PROFILE_BASIC_STARTPOINT_TIME;
      betaGlucanaseTime           =   PROFILE_BASIC_BETAGLUCANASE_TIME;
      debranchingTime             =   PROFILE_BASIC_DEBRANCHING_TIME;
      proteolyticTime             =   PROFILE_BASIC_PROTEOLYTIC_TIME;
      betaAmylaseTime             =   PROFILE_BASIC_BETAAMYLASE_TIME;
      alphaAmylaseTime            =   PROFILE_BASIC_ALPHAAMYLASE_TIME;
      mashoutTime                 =   PROFILE_BASIC_MASHOUT_TIME;
      recirculationTime           =   PROFILE_BASIC_RECIRCULATION_TIME;
      spargeTime                  =   PROFILE_BASIC_SPARGE_TIME;
      boilTime                    =   PROFILE_BASIC_BOIL_TIME;
      coolingTime                 =   PROFILE_BASIC_COOLING_TIME;

      startpointTemperature       =   PROFILE_BASIC_STARTPOINT_TEMPERATURE;
      betaGlucanaseTemperature    =   PROFILE_BASIC_BETAGLUCANASE_TEMPERATURE;
      debranchingTemperature      =   PROFILE_BASIC_DEBRANCHING_TEMPERATURE;
      proteolyticTemperature      =   PROFILE_BASIC_PROTEOLYTIC_TEMPERATURE;
      betaAmylaseTemperature      =   PROFILE_BASIC_BETAAMYLASE_TEMPERATURE;
      alphaAmylaseTemperature     =   PROFILE_BASIC_ALPHAAMYLASE_TEMPERATURE;
      mashoutTemperature          =   PROFILE_BASIC_MASHOUT_TEMPERATURE;
      recirculationTemperature    =   PROFILE_BASIC_RECIRCULATION_TEMPERATURE;
      spargeTemperature           =   PROFILE_BASIC_SPARGE_TEMPERATURE;
      boilTemperature             =   PROFILE_BASIC_BOIL_TEMPERATURE;
      coolingTemperature          =   PROFILE_BASIC_COOLING_TEMPERATURE;

      backToStatus();
          
      break;
    }
    case eBeerProfileMenu_Trigo: {
      beerProfile                 =   eBeerProfile_Trigo;

      startpointTime              =   PROFILE_TRIGO_STARTPOINT_TIME;
      betaGlucanaseTime           =   PROFILE_TRIGO_BETAGLUCANASE_TIME;
      debranchingTime             =   PROFILE_TRIGO_DEBRANCHING_TIME;
      proteolyticTime             =   PROFILE_TRIGO_PROTEOLYTIC_TIME;
      betaAmylaseTime             =   PROFILE_TRIGO_BETAAMYLASE_TIME;
      alphaAmylaseTime            =   PROFILE_TRIGO_ALPHAAMYLASE_TIME;
      mashoutTime                 =   PROFILE_TRIGO_MASHOUT_TIME;
      recirculationTime           =   PROFILE_TRIGO_RECIRCULATION_TIME;
      spargeTime                  =   PROFILE_TRIGO_SPARGE_TIME;
      boilTime                    =   PROFILE_TRIGO_BOIL_TIME;
      coolingTime                 =   PROFILE_TRIGO_COOLING_TIME;

      startpointTemperature       =   PROFILE_TRIGO_STARTPOINT_TEMPERATURE;
      betaGlucanaseTemperature    =   PROFILE_TRIGO_BETAGLUCANASE_TEMPERATURE;
      debranchingTemperature      =   PROFILE_TRIGO_DEBRANCHING_TEMPERATURE;
      proteolyticTemperature      =   PROFILE_TRIGO_PROTEOLYTIC_TEMPERATURE;
      betaAmylaseTemperature      =   PROFILE_TRIGO_BETAAMYLASE_TEMPERATURE;
      alphaAmylaseTemperature     =   PROFILE_TRIGO_ALPHAAMYLASE_TEMPERATURE;
      mashoutTemperature          =   PROFILE_TRIGO_MASHOUT_TEMPERATURE;
      recirculationTemperature    =   PROFILE_TRIGO_RECIRCULATION_TEMPERATURE;
      spargeTemperature           =   PROFILE_TRIGO_SPARGE_TEMPERATURE;
      boilTemperature             =   PROFILE_TRIGO_BOIL_TEMPERATURE;
      coolingTemperature          =   PROFILE_TRIGO_COOLING_TEMPERATURE;

      backToStatus();
      
      break;
    }
    case eBeerProfileMenu_IPA: {
      beerProfile                 =   eBeerProfile_IPA;

      startpointTime              =   PROFILE_IPA_STARTPOINT_TIME;
      betaGlucanaseTime           =   PROFILE_IPA_BETAGLUCANASE_TIME;
      debranchingTime             =   PROFILE_IPA_DEBRANCHING_TIME;
      proteolyticTime             =   PROFILE_IPA_PROTEOLYTIC_TIME;
      betaAmylaseTime             =   PROFILE_IPA_BETAAMYLASE_TIME;
      alphaAmylaseTime            =   PROFILE_IPA_ALPHAAMYLASE_TIME;
      mashoutTime                 =   PROFILE_IPA_MASHOUT_TIME;
      recirculationTime           =   PROFILE_IPA_RECIRCULATION_TIME;
      spargeTime                  =   PROFILE_IPA_SPARGE_TIME;
      boilTime                    =   PROFILE_IPA_BOIL_TIME;
      coolingTime                 =   PROFILE_IPA_COOLING_TIME;

      startpointTemperature       =   PROFILE_IPA_STARTPOINT_TEMPERATURE;
      betaGlucanaseTemperature    =   PROFILE_IPA_BETAGLUCANASE_TEMPERATURE;
      debranchingTemperature      =   PROFILE_IPA_DEBRANCHING_TEMPERATURE;
      proteolyticTemperature      =   PROFILE_IPA_PROTEOLYTIC_TEMPERATURE;
      betaAmylaseTemperature      =   PROFILE_IPA_BETAAMYLASE_TEMPERATURE;
      alphaAmylaseTemperature     =   PROFILE_IPA_ALPHAAMYLASE_TEMPERATURE;
      mashoutTemperature          =   PROFILE_IPA_MASHOUT_TEMPERATURE;
      recirculationTemperature    =   PROFILE_IPA_RECIRCULATION_TEMPERATURE;
      spargeTemperature           =   PROFILE_IPA_SPARGE_TEMPERATURE;
      boilTemperature             =   PROFILE_IPA_BOIL_TEMPERATURE;
      coolingTemperature          =   PROFILE_IPA_COOLING_TEMPERATURE;

      backToStatus();

      break;
    }
    case eBeerProfileMenu_Belga: {
      beerProfile                 =   eBeerProfile_Belga;

      startpointTime              =   PROFILE_BELGA_STARTPOINT_TIME;
      betaGlucanaseTime           =   PROFILE_BELGA_BETAGLUCANASE_TIME;
      debranchingTime             =   PROFILE_BELGA_DEBRANCHING_TIME;
      proteolyticTime             =   PROFILE_BELGA_PROTEOLYTIC_TIME;
      betaAmylaseTime             =   PROFILE_BELGA_BETAAMYLASE_TIME;
      alphaAmylaseTime            =   PROFILE_BELGA_ALPHAAMYLASE_TIME;
      mashoutTime                 =   PROFILE_BELGA_MASHOUT_TIME;
      recirculationTime           =   PROFILE_BELGA_RECIRCULATION_TIME;
      spargeTime                  =   PROFILE_BELGA_SPARGE_TIME;
      boilTime                    =   PROFILE_BELGA_BOIL_TIME;
      coolingTime                 =   PROFILE_BELGA_COOLING_TIME;

      startpointTemperature       =   PROFILE_BELGA_STARTPOINT_TEMPERATURE;
      betaGlucanaseTemperature    =   PROFILE_BELGA_BETAGLUCANASE_TEMPERATURE;
      debranchingTemperature      =   PROFILE_BELGA_DEBRANCHING_TEMPERATURE;
      proteolyticTemperature      =   PROFILE_BELGA_PROTEOLYTIC_TEMPERATURE;
      betaAmylaseTemperature      =   PROFILE_BELGA_BETAAMYLASE_TEMPERATURE;
      alphaAmylaseTemperature     =   PROFILE_BELGA_ALPHAAMYLASE_TEMPERATURE;
      mashoutTemperature          =   PROFILE_BELGA_MASHOUT_TEMPERATURE;
      recirculationTemperature    =   PROFILE_BELGA_RECIRCULATION_TEMPERATURE;
      spargeTemperature           =   PROFILE_BELGA_SPARGE_TEMPERATURE;
      boilTemperature             =   PROFILE_BELGA_BOIL_TEMPERATURE;
      coolingTemperature          =   PROFILE_BELGA_COOLING_TEMPERATURE;
      
      backToStatus();
      
      break;
    }
    case eBeerProfileMenu_Red: {
      beerProfile                 =   eBeerProfile_Red;
      
      startpointTime              =   PROFILE_RED_STARTPOINT_TIME;
      betaGlucanaseTime           =   PROFILE_RED_BETAGLUCANASE_TIME;
      debranchingTime             =   PROFILE_RED_DEBRANCHING_TIME;
      proteolyticTime             =   PROFILE_RED_PROTEOLYTIC_TIME;
      betaAmylaseTime             =   PROFILE_RED_BETAAMYLASE_TIME;
      alphaAmylaseTime            =   PROFILE_RED_ALPHAAMYLASE_TIME;
      mashoutTime                 =   PROFILE_RED_MASHOUT_TIME;
      recirculationTime           =   PROFILE_RED_RECIRCULATION_TIME;
      spargeTime                  =   PROFILE_RED_SPARGE_TIME;
      boilTime                    =   PROFILE_RED_BOIL_TIME;
      coolingTime                 =   PROFILE_RED_COOLING_TIME;

      startpointTemperature       =   PROFILE_RED_STARTPOINT_TEMPERATURE;
      betaGlucanaseTemperature    =   PROFILE_RED_BETAGLUCANASE_TEMPERATURE;
      debranchingTemperature      =   PROFILE_RED_DEBRANCHING_TEMPERATURE;
      proteolyticTemperature      =   PROFILE_RED_PROTEOLYTIC_TEMPERATURE;
      betaAmylaseTemperature      =   PROFILE_RED_BETAAMYLASE_TEMPERATURE;
      alphaAmylaseTemperature     =   PROFILE_RED_ALPHAAMYLASE_TEMPERATURE;
      mashoutTemperature          =   PROFILE_RED_MASHOUT_TEMPERATURE;
      recirculationTemperature    =   PROFILE_RED_RECIRCULATION_TEMPERATURE;
      spargeTemperature           =   PROFILE_RED_SPARGE_TEMPERATURE;
      boilTemperature             =   PROFILE_RED_BOIL_TEMPERATURE;
      coolingTemperature          =   PROFILE_RED_COOLING_TEMPERATURE;

      backToStatus();
      
      break;
    }
    case eBeerProfileMenu_APA: {
      beerProfile                 =   eBeerProfile_APA;

      startpointTime              =   PROFILE_APA_STARTPOINT_TIME;
      betaGlucanaseTime           =   PROFILE_APA_BETAGLUCANASE_TIME;
      debranchingTime             =   PROFILE_APA_DEBRANCHING_TIME;
      proteolyticTime             =   PROFILE_APA_PROTEOLYTIC_TIME;
      betaAmylaseTime             =   PROFILE_APA_BETAAMYLASE_TIME;
      alphaAmylaseTime            =   PROFILE_APA_ALPHAAMYLASE_TIME;
      mashoutTime                 =   PROFILE_APA_MASHOUT_TIME;
      recirculationTime           =   PROFILE_APA_RECIRCULATION_TIME;
      spargeTime                  =   PROFILE_APA_SPARGE_TIME;
      boilTime                    =   PROFILE_APA_BOIL_TIME;
      coolingTime                 =   PROFILE_APA_COOLING_TIME;

      startpointTemperature       =   PROFILE_APA_STARTPOINT_TEMPERATURE;
      betaGlucanaseTemperature    =   PROFILE_APA_BETAGLUCANASE_TEMPERATURE;
      debranchingTemperature      =   PROFILE_APA_DEBRANCHING_TEMPERATURE;
      proteolyticTemperature      =   PROFILE_APA_PROTEOLYTIC_TEMPERATURE;
      betaAmylaseTemperature      =   PROFILE_APA_BETAAMYLASE_TEMPERATURE;
      alphaAmylaseTemperature     =   PROFILE_APA_ALPHAAMYLASE_TEMPERATURE;
      mashoutTemperature          =   PROFILE_APA_MASHOUT_TEMPERATURE;
      recirculationTemperature    =   PROFILE_APA_RECIRCULATION_TEMPERATURE;
      spargeTemperature           =   PROFILE_APA_SPARGE_TEMPERATURE;
      boilTemperature             =   PROFILE_APA_BOIL_TEMPERATURE;
      coolingTemperature          =   PROFILE_APA_COOLING_TEMPERATURE;
      
      backToStatus();
      
      break;
    }
    case eBeerProfileMenu_Custom: {
      beerProfile                 =   eBeerProfile_Custom;

      startpointTime              =   PROFILE_CUSTOM_STARTPOINT_TIME;
      betaGlucanaseTime           =   PROFILE_CUSTOM_BETAGLUCANASE_TIME;
      debranchingTime             =   PROFILE_CUSTOM_DEBRANCHING_TIME;
      proteolyticTime             =   PROFILE_CUSTOM_PROTEOLYTIC_TIME;
      betaAmylaseTime             =   PROFILE_CUSTOM_BETAAMYLASE_TIME;
      alphaAmylaseTime            =   PROFILE_CUSTOM_ALPHAAMYLASE_TIME;
      mashoutTime                 =   PROFILE_CUSTOM_MASHOUT_TIME;
      recirculationTime           =   PROFILE_CUSTOM_RECIRCULATION_TIME;
      spargeTime                  =   PROFILE_CUSTOM_SPARGE_TIME;
      boilTime                    =   PROFILE_CUSTOM_BOIL_TIME;
      coolingTime                 =   PROFILE_CUSTOM_COOLING_TIME;

      startpointTemperature       =   PROFILE_CUSTOM_STARTPOINT_TEMPERATURE;
      betaGlucanaseTemperature    =   PROFILE_CUSTOM_BETAGLUCANASE_TEMPERATURE;
      debranchingTemperature      =   PROFILE_CUSTOM_DEBRANCHING_TEMPERATURE;
      proteolyticTemperature      =   PROFILE_CUSTOM_PROTEOLYTIC_TEMPERATURE;
      betaAmylaseTemperature      =   PROFILE_CUSTOM_BETAAMYLASE_TEMPERATURE;
      alphaAmylaseTemperature     =   PROFILE_CUSTOM_ALPHAAMYLASE_TEMPERATURE;
      mashoutTemperature          =   PROFILE_CUSTOM_MASHOUT_TEMPERATURE;
      recirculationTemperature    =   PROFILE_CUSTOM_RECIRCULATION_TEMPERATURE;
      spargeTemperature           =   PROFILE_CUSTOM_SPARGE_TEMPERATURE;
      boilTemperature             =   PROFILE_CUSTOM_BOIL_TEMPERATURE;
      coolingTemperature          =   PROFILE_CUSTOM_COOLING_TEMPERATURE;

      backToStatus();
      
      break;
    }
    case eBeerProfileMenu_Back: {
      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );

      break;
    }
    default: {
    }
  }

  eBeerProfileMenuSelection = eBeerProfileMenu_NULL;
}

void xStartStage( unsigned long *stageTime, int *stageTemperature, eCookingStages nextStage, bool bPurgePump, bool bSetFinalYield, bool bSetTime, bool bSetTemperature ) {
  xSafeHardwarePowerOff();                      // Stop anything that might be still going on
  
  if(bSetFinalYield) { finalYield = xSetFinalYield( finalYield ); }
  if(bSetTime) { (*stageTime) = getTimer( clockCounter/1000, (*stageTime) ); }
  if(bSetTemperature) { (*stageTemperature) = xSetTemperature( (*stageTemperature) ); }
  if(bPurgePump) { xPurgePump(); }
  
  startBrewing();
  xSetupStage( nextStage );
  backToStatus();
}

void xStartStageHeadless( eCookingStages nextStage, bool bPurgePump ) {
  xStartStage( NULL, NULL, nextStage, bPurgePump, false, false, false );
}

void xStartStageInteractive( unsigned long *stageTime, int *stageTemperature, eCookingStages nextStage ) {
  xStartStage( stageTime, stageTemperature, nextStage, true, true, true, true );
}

void runStartFromStageSelection() {
  switch(eStartFromStageMenuSelection) {
    case eStageMenu_Startpoint: {
      xStartStageInteractive( &startpointTime, &startpointTemperature, eCookingStage_Startpoint );
      break;
    }
    case eStageMenu_BetaGlucanase: {
      xStartStageInteractive( &betaGlucanaseTime, &betaGlucanaseTemperature, eCookingStage_BetaGlucanase );
      break;
    }
    case eStageMenu_Debranching: {
      xStartStageInteractive( &debranchingTime, &debranchingTemperature, eCookingStage_Debranching );
      break;
    }
    case eStageMenu_Proteolytic: {
      xStartStageInteractive( &proteolyticTime, &proteolyticTemperature, eCookingStage_Proteolytic );
      break;
    }
    case eStageMenu_BetaAmylase: {
      xStartStageInteractive( &betaAmylaseTime, &betaAmylaseTemperature, eCookingStage_BetaAmylase );
      break;
    }
    case eStageMenu_AlphaAmylase: {
      xStartStageInteractive( &alphaAmylaseTime, &alphaAmylaseTemperature, eCookingStage_AlphaAmylase );
      break;
    }
    case eStageMenu_Mashout: {
      xStartStageInteractive( &mashoutTime, &mashoutTemperature, eCookingStage_Mashout );
      break;
    }
    case eStageMenu_Recirculation: {
      xStartStageInteractive( &recirculationTime, &recirculationTemperature, eCookingStage_Recirculation );
      break;
    }
    case eStageMenu_Sparge: {
      xStartStageInteractive( &spargeTime, &spargeTemperature, eCookingStage_Sparge );
      break;
    }
    case eStageMenu_Boil: {
      xStartStageInteractive( &boilTime, &boilTemperature, eCookingStage_Boil );
      break;
    }
    case eStageMenu_Cooling: {
      xStartStageInteractive( &coolingTime, &coolingTemperature, eCookingStage_Cooling );
      break;
    }
    case eStageMenu_Back: {
      resetMenu( true );
      break;
    }
    default: {
    }
  }
  eStartFromStageMenuSelection = eStageMenu_NULL;
}

void runMainMenuSelection() {
  switch(eMainMenuSelection) {
    case eMainMenu_GO: {
      xStartStage( NULL, NULL, eCookingStage_Startpoint, true, true, false, false );
      break;
    }
    case eMainMenu_GO_FROM_STAGE: {
      eMenuType = eMenuType_StartFromStage;
      repaint = true;
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eBeerProfileMenuPosition, MENU_SIZE_PROFILES_MENU - 1, 1, 1, 0 );
      break;
    }
    case eMainMenu_STOP: {
      stopBrewing();
      backToStatus();
      break;
    }
    case eMainMenu_SKIP: {
      cookTime = 0;
      backToStatus();
      break;
    }
    case eMainMenu_BeerProfile: {
      eMenuType = eMenuType_BeerProfile;
      repaint = true;
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eBeerProfileMenuPosition, MENU_SIZE_PROFILES_MENU - 1, 1, 1, 0 );
      break;
    }
    case eMainMenu_Stage: {
      eMenuType = eMenuType_Stage;
      repaint = true;
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eStageMenuPosition, MENU_SIZE_STAGE_MENU - 1, 1, 1, 0 );
      break;
    }
    case eMainMenu_Malt: {
      eMenuType = eMenuType_Malt;
      repaint = true;
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMaltMenuPosition, MENU_SIZE_MALT_MENU - 1, 1, 1, 0 );
      break;
    }
    case eMainMenu_Hops: {
      backToStatus();
      break;
    }
    case eMainMenu_Clean: {
      xStartStageHeadless( eCookingStage_Clean, true );
      break;
    }
    case eMainMenu_Purge: {
      xStartStageHeadless( eCookingStage_Purge, true );
      break;
    }
    case eMainMenu_Settings: {
      eMenuType = eMenuType_Settings;
      repaint = true;
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eSettingsMenuPosition, MENU_SIZE_SETTINGS_MENU - 1, 1, 1, 0 );
      break;
    }
    case eMainMenu_Back: {
      backToStatus();
      break;
    }
    default: {
    }
  }
  eMainMenuSelection = eMainMenu_NULL;
}

void xCountTheTime( int temperatureMarginRange, boolean bMaximumOfUpDown ) {
  unsigned long now = millis();

  // Get current maximum sensed temperaure
  double temperatureCount = 0;
  if( bMaximumOfUpDown ) {
    float tup = upPT100.getCurrentTemperature();
    float tdown = downPT100.getCurrentTemperature();
    if(tup > tdown) {
      temperatureCount = tdown;
    }
    else {
      temperatureCount = tup;
    }
  } else {
    temperatureCount = basePT100.getCurrentTemperature();
  }

  // Ignote time ticks if temperature is not within the acceptable margin for this stage
  unsigned long elapsedTime = now - clockLastUpdate;
  if( temperatureCount < (cookTemperature - temperatureMarginRange) ) {
    clockIgnore += elapsedTime;
  }
  
  // Calculate the remaining time on the clock
  clockCounter = cookTime * 1000 - (elapsedTime - clockIgnore);

  // Don't let clock get bellow 0
  if ( clockCounter < 0 ) {
    clockCounter = 0;
  }

  clockLastUpdate = now;

  #ifdef DEBUG_OFF
  debugPrintFunction("xCountTheTime");
  debugPrintVar("millis()", now);
  debugPrintVar("cookTime", cookTime);
  debugPrintVar("clockStartTime", clockStartTime);
  debugPrintVar("clockIgnore", clockIgnore);
  debugPrintVar("clockCounter", clockCounter); 
  #endif
}

bool isTimeLeft() {
  if( clockCounter > 0 ) {
    return true;
  }
  return false;
}

//HEATING_ELEMENT_MAX_WATTAGE / HEATING_ELEMENT_AC_FREQUENCY_HZ
double ulWattToWindowTime( double ulAppliedWatts ) {
  double ulPulsesRequired = ulAppliedWatts / dWattPerPulse;
  return (double)iWindowSize / 1000.0 * ulPulsesRequired * 1000.0 / HEATING_ELEMENT_AC_FREQUENCY_HZ;
}

bool xRegulateTemperature( boolean bMaximumOfUpDown ) {
  double difference = 0;
  bool overTemperature = false;
  double wattage = 0.0;
  
  float tup = upPT100.getCurrentTemperature();
  float tdown = downPT100.getCurrentTemperature();
  float tbase = basePT100.getCurrentTemperature();

  if( bMaximumOfUpDown ) {
    if(tup > tdown) {
      difference = cookTemperature - tup;
    }
    else {
      difference = cookTemperature - tdown;
    }

    if(tbase > cookTemperature && (tbase >= (PUMP_TEMPERATURE_MAX_OPERATION - 2.0) || difference >= 5.0)) {
      difference = cookTemperature - tbase;
    }

    if( (tbase < cookTemperature) && (difference < (cookTemperature - tbase)) ) {
      difference = cookTemperature - tbase;
    }
  } else {
    difference = cookTemperature - tbase;
  }

  // Deviation between the cook temperature set and the cook temperature measured
  if( difference < 0.0 ) {
    difference = difference * (-1.0);
    overTemperature = true;
  }
  
  // Calculate applied wattage, based on the distance from the target temperature
  if( overTemperature ) {
    // turn it off
    wattage = 0.0;
  } else {
    //if(difference <= 0.1) {
      // turn it off
    //  wattage = 0.0;
    //} else {
      if(difference <= 0.5) {
        // pulse lightly at 500 watt
        if(cookTemperature > 99.0) {
          wattage = 2000.0;
        }
        else {
          if(cookTemperature > 70.0) {
            wattage = 1000.0;
          }
          else {
            wattage = 500.0;
          }
        }
      } else {
        if(difference <= 1.0) {
          // pulse moderately at 1000 watt
          if(cookTemperature > 99.0) {
            wattage = 2000.0;
          }
          else {
            wattage = 1000.0;
          }
          
        } else {
          if(difference <= 3.0) {
            // pulse hardly at 2000 watt
            wattage = 2000.0;
          } else {
            //pulse constantly at HEATING_ELEMENT_MAX_WATTAGE watt
            wattage = HEATING_ELEMENT_MAX_WATTAGE;
          }
        }
      }
    //}
  }
  
  // Update the recorded time for the begining of the window, if the previous window has passed
  while((millis() - windowStartTime) > iWindowSize) { // Check if it's time to vary the pulse width modulation and if so do it by shifting the "Relay in ON" Window
    windowStartTime += iWindowSize;
  }
  
  // Apply wattage to the element at the right time
  if( ulWattToWindowTime( wattage ) > (millis() - windowStartTime) ) {
    digitalWrite(HEATING_ELEMENT_OUTPUT_PIN,HIGH);
    bStatusElement = true;
  } else {
    digitalWrite(HEATING_ELEMENT_OUTPUT_PIN,LOW);
    bStatusElement = false;
  }

  #ifdef DEBUG_OFF
  //debugPrintFunction("xRegulateTemperature");
  debugPrintVar("difference", difference);
  //debugPrintVar("overTemperature", overTemperature);
  debugPrintVar("wattage", wattage);
  //debugPrintVar("ulWattToWindowTime( wattage )", ulWattToWindowTime( wattage ) );
  //debugPrintVar("millis()", millis());
  //debugPrintVar("windowStartTime", windowStartTime);
  //debugPrintVar("test", ulWattToWindowTime( wattage ) > (millis() - windowStartTime) ); 
  #endif
}

void xPurgePump() {
  for(int i = 0; i < 2; i++) {
    analogWrite(PUMP_PIN, PUMP_SPEED_MAX_MOSFET);  // analogWrite values from 0 to 255
    delay(1000);
    analogWrite(PUMP_PIN, PUMP_SPEED_STOP_MOSFET);  // analogWrite values from 0 to 255
    delay(1500);
  }
}

bool xRegulatePumpSpeed() {
  //  analogWrite(PUMP_PIN, iPumpSpeed);  // analogWrite values from 0 to 255

  if(basePT100.getCurrentTemperature() > PUMP_TEMPERATURE_MAX_OPERATION) {
    analogWrite(PUMP_PIN, PUMP_SPEED_STOP_MOSFET);  // analogWrite values from 0 to 255

    basePT100.setPumpStatus( false );
    upPT100.setPumpStatus( false );
    downPT100.setPumpStatus( false );
  }
  else {
    analogWrite(PUMP_PIN, iPumpSpeed);  // analogWrite values from 0 to 255

    basePT100.setPumpStatus( true );
    upPT100.setPumpStatus( true );
    downPT100.setPumpStatus( true );
  }
}

void xWarnClockEnded() {
  sing(MELODY_SUPER_MARIO_START, PIEZO_PIN);
}

void xWarnCookEnded() {
  sing(MELODY_UNDERWORLD_SHORT, PIEZO_PIN);
}

void xPrepareForStage( int stageTime, int stageTemperature, int stagePumpSpeed, eCookingStages stage ) {
  #ifdef DEBUG_OFF
  debugPrintFunction("xPrepareForStage");
  debugPrintVar("cookingStage", stage);
  #endif

  // Reset the clock
  unsigned long now = millis();
  clockStartTime  = now;
  clockLastUpdate = now;
  clockIgnore     = 0;

  iPumpSpeed      = stagePumpSpeed;         // Set the pump speed
  cookingStage    = stage;                  // Set Stage
  cookTime        = stageTime;              // Set the clock
  cookTemperature = stageTemperature;       // Set the target temperature
}

void xSetupStage(eCookingStages nextStage) {
  #ifdef DEBUG_OFF
  debugPrintFunction("xSetupStage");
  debugPrintVar("cookingStage", nextStage);
  #endif

  // Operate the machine according to the current mode
  switch(nextStage) {
    case eCookingStage_Startpoint: {
      switch(beerProfile) {
        case eBeerProfile_Trigo: {
          float wheatAmount = 0.05 * ((float) finalYield);
          float pilsnerAmount = 0.2 * ((float) finalYield);
          String say = "Cruch ";
          say += String(wheatAmount);
          say += String("Kg of Wheat and ");
          say += String(pilsnerAmount);
          say += String("Kg of Pilsner Malt into a pot.");

          xWaitForAction("Malt", say);
          repaint = true;
          break;
        }
        case eBeerProfile_IPA: {
          float caramelAmount = 0.013157895 * ((float) finalYield);
          float wheatAmount = 0.060526316 * ((float) finalYield);
          float pilsnerAmount = 0.115789474 * ((float) finalYield);
          float munichAmount = 0.028947368 * ((float) finalYield);
          String say = "Cruch ";
          say += String(caramelAmount);
          say += String("Kg of Caramel 120, ");
          say += String(wheatAmount);
          say += String("Kg of Wheat, ");
          say += String(pilsnerAmount);
          say += String("Kg of Pilsner, ");
          say += String(munichAmount);
          say += String("Kg of Munich into a pot.");

          xWaitForAction("Malt", say);
          repaint = true;
          break;
        }
        default: {

        }
      }

      // Make sure there is water
      xWaitForAction("Water", "Make sure there is water in the machine before start cooking.");

      repaint = true;
      xPrepareForStage( startpointTime, startpointTemperature, PUMP_SPEED_MAX_MOSFET, eCookingStage_Startpoint );
      break;
    }
    case eCookingStage_BetaGlucanase: {
      switch(beerProfile) {
        case eBeerProfile_Trigo: {
          float wheatAmount = 0.05 * ((float) finalYield);
          float pilsnerAmount = 0.2 * ((float) finalYield);
          String say = "Put ";
          say += String(wheatAmount);
          say += String("Kg of Wheat and ");
          say += String(pilsnerAmount);
          say += String("Kg of Pilsner Malt in.");
          
          xWaitForAction("Malt", say);
          repaint = true;
          break;
        }
        case eBeerProfile_IPA: {
          float caramelAmount = 0.013157895 * ((float) finalYield);
          float wheatAmount = 0.060526316 * ((float) finalYield);
          float pilsnerAmount = 0.115789474 * ((float) finalYield);
          float munichAmount = 0.028947368 * ((float) finalYield);
          String say = "Cruch ";
          say += String(caramelAmount);
          say += String("Kg of Caramel 120, ");
          say += String(wheatAmount);
          say += String("Kg of Wheat, ");
          say += String(pilsnerAmount);
          say += String("Kg of Pilsner, ");
          say += String(munichAmount);
          say += String("Kg of Munich into a pot.");

          xWaitForAction("Malt", say);
          repaint = true;
          break;
        }
        default: {}
      }
      xPrepareForStage( betaGlucanaseTime, betaGlucanaseTemperature, PUMP_SPEED_MAX_MOSFET, eCookingStage_BetaGlucanase );
      break;
    }
    case eCookingStage_Debranching: {
      xPrepareForStage( debranchingTime, debranchingTemperature, PUMP_SPEED_MAX_MOSFET, eCookingStage_Debranching );
      break;
    }
    case eCookingStage_Proteolytic: {
      xPrepareForStage( proteolyticTime, proteolyticTemperature, PUMP_SPEED_MAX_MOSFET, eCookingStage_Proteolytic );
      break;
    }
    case eCookingStage_BetaAmylase: {
      xPrepareForStage( betaAmylaseTime, betaAmylaseTemperature, PUMP_SPEED_MAX_MOSFET, eCookingStage_BetaAmylase );
      break;
    }
    case eCookingStage_AlphaAmylase: {
      xPrepareForStage( alphaAmylaseTime, alphaAmylaseTemperature, PUMP_SPEED_MAX_MOSFET, eCookingStage_AlphaAmylase );
      break;
    }
    case eCookingStage_Mashout: {
      xPrepareForStage( mashoutTime, mashoutTemperature, PUMP_SPEED_MAX_MOSFET, eCookingStage_Mashout );
      break;
    }
    case eCookingStage_Recirculation: {
      xWaitForAction("Sparge Water", "Start heating your sparge water.");
      repaint = true;
      xPrepareForStage( recirculationTime, recirculationTemperature, PUMP_SPEED_MAX_MOSFET, eCookingStage_Recirculation );
      break;
    }
    case eCookingStage_Sparge: {
      xWaitForAction("Sparge Water", "Start pouring the sparge water.");
      repaint = true;
      xPrepareForStage( spargeTime, spargeTemperature, PUMP_SPEED_MAX_MOSFET, eCookingStage_Sparge );
      break;
    }
    case eCookingStage_Boil: {
      switch(beerProfile) {
        case eBeerProfile_Trigo: {
          String say = "Get ";

          float hopAmount = 0.8 * ((float) finalYield);
          say += String(hopAmount);

          say += String("g of Magnum 9.4\% and Styrian Golding 5\% ready.");

          xWaitForAction("Hops", say);

          break;
        }
        case eBeerProfile_IPA: {
          String say = "Get ";

          float hopAmount = 0.8 * ((float) finalYield);
          say += String(hopAmount);

          say += String("g of Chinook, Cascade and Styrian Golding ready.");

          xWaitForAction("Hops", say);

          break;
        }
        default: {
          xWaitForAction("Hops", "Add the hops in the right order, at the right time.");

        }
      }

      repaint = true;

      // A basic operation for a basic stage
      xPrepareForStage( boilTime, boilTemperature, PUMP_SPEED_MAX_MOSFET, eCookingStage_Boil );
      
      break;
    }
    case eCookingStage_Cooling: {
      // Make sure there is water
      xWaitForAction("Coil", "Add the coil and connect it to the main water supply.");

      repaint = true;

      // A basic operation for a basic stage
      xPrepareForStage( coolingTime, coolingTemperature, PUMP_SPEED_MAX_MOSFET, eCookingStage_Cooling );

      break;
    }
    case eCookingStage_Clean: {
      // Make sure there is water
      xWaitForAction("Water", "Add 13 liters.");

      // Make sure there is water
      xWaitForAction("Star San HB", "Add 0.89oz/26ml.");

      repaint = true;

      // A basic operation for a basic stage
      xPrepareForStage( cleaningTime, cleaningTemperature, PUMP_SPEED_MAX_MOSFET, eCookingStage_Clean );

      break;
    }
    case eCookingStage_Purge: {
      // A basic operation for a basic stage
      xPrepareForStage( 0, 0, PUMP_SPEED_MAX_MOSFET, eCookingStage_Purge );

      xRegulatePumpSpeed();

      break;
    }
    case eCookingStage_Done: {
      // A basic operation for a basic stage
      xPrepareForStage( 0, 0, PUMP_SPEED_STOP_MOSFET, eCookingStage_Done );

      break;
    }
  }
}

void xTransitionIntoStage(eCookingStages nextStage) {
  // Turn off all hardware that can damage itself if the machine is not cooking
  xSafeHardwarePowerOff();    
  
  // Warn the user a stage has ended
  xWarnClockEnded();
  
  // Reset global stage variables
  xSetupStage( nextStage );
}

void xBasicStageOperation( int iStageTime, int iStageTemperature, int iStageTemperatureRange, eCookingStages nextStage, boolean bMaximumOfUpDown ) {

  // Account for time spent at the target temperature | Input 1: range in ºC within which the target temperature is considered to be reached
  #ifdef DEBUG_OFF
  xCountTheTime( iStageTemperatureRange, false );
  #else
  xCountTheTime( iStageTemperatureRange, bMaximumOfUpDown );
  #endif
  
  if( isTimeLeft() ) {
    // Do temperature control
    xRegulateTemperature( bMaximumOfUpDown );

    // Do flow control
    xRegulatePumpSpeed();
    
  } else {
    #ifdef DEBUG_OFF
    debugPrintFunction("xBasicStageOperation");
    debugPrintVar("clockCounter", clockCounter);
    #endif
    
    // Continue to the next stage, there is nothing to do, in this stage
    xTransitionIntoStage( nextStage );
    return;
  }
  
  return;
}

void xManageMachineSystems() {

  #ifdef DEBUG
  Serial.print(millis());
  Serial.print(",");
  if(cooking) {
    Serial.print("1");
  }
  else {
    Serial.print("0");
  }
  Serial.print(",");
  Serial.print(cookTemperature);
  Serial.print(",");
  if(bStatusElement) {
    Serial.print("1");
  }
  else {
    Serial.print("0");
  }
  Serial.print(",");
  #endif

  // Measure temperature, for effect
  basePT100.measure(false);
  upPT100.measure(false);
  downPT100.measure(true);

  // If cooking is done, return (this is a nice place to double check safety and ensure the cooking parts aren't on.
  if(!cooking) {
    xSafeHardwarePowerOff();

    return;
  }
  
  // Operate the machine according to the current mode
  switch(cookingStage) {
    case eCookingStage_Startpoint: {
      // A basic operation for a basic stage
      xBasicStageOperation( startpointTime, startpointTemperature, 0, eCookingStage_BetaGlucanase, false);
      
      break;
    }
    case eCookingStage_BetaGlucanase: {
      // A basic operation for a basic stage
      xBasicStageOperation( betaGlucanaseTime, betaGlucanaseTemperature, 3, eCookingStage_Debranching, true );
      
      break;
    }
    case eCookingStage_Debranching: {
      // A basic operation for a basic stage
      xBasicStageOperation( debranchingTime, debranchingTemperature, 3, eCookingStage_Proteolytic, true );
      
      break;
    }
    case eCookingStage_Proteolytic: {
      // A basic operation for a basic stage
      xBasicStageOperation( proteolyticTime, proteolyticTemperature, 3, eCookingStage_BetaAmylase, true );
      
      break;
    }
    case eCookingStage_BetaAmylase: {
      // A basic operation for a basic stage
      xBasicStageOperation( betaAmylaseTime, betaAmylaseTemperature, 4, eCookingStage_AlphaAmylase, true );
      
      break;
    }
    case eCookingStage_AlphaAmylase: {
      // A basic operation for a basic stage
      xBasicStageOperation( alphaAmylaseTime, alphaAmylaseTemperature, 2, eCookingStage_Mashout, true );
      
      break;
    }
    case eCookingStage_Mashout: {
      // A basic operation for a basic stage
      xBasicStageOperation( mashoutTime, mashoutTemperature, 1, eCookingStage_Recirculation, true );
      
      break;
    }
    case eCookingStage_Recirculation: {
      // A basic operation for a basic stage
      xBasicStageOperation( recirculationTime, recirculationTemperature, 1, eCookingStage_Sparge, true );
      
      break;
    }
    case eCookingStage_Sparge: {
      // A basic operation for a basic stage
      xBasicStageOperation( spargeTime, spargeTemperature, 3, eCookingStage_Boil, false );
      
      break;
    }
    case eCookingStage_Boil: {
      // A basic operation for a basic stage
      xBasicStageOperation( boilTime, boilTemperature, 2, eCookingStage_Cooling, false );
      
      break;
    }
    case eCookingStage_Cooling: {
      // A basic operation for a basic stage
      xBasicStageOperation( coolingTime, coolingTemperature, 0, eCookingStage_Done, false );

      break;
    }
    case eCookingStage_Clean: {
      // A basic operation for a basic stage
      xBasicStageOperation( cleaningTime, cleaningTemperature, 0, eCookingStage_Done, false );

      break;
    }
    case eCookingStage_Purge: {
      // A basic operation for a basic stage
      //xBasicStageOperation( coolingTime, coolingTemperature, 1, eCookingStage_Done );
      iPumpSpeed = PUMP_SPEED_MAX_MOSFET;

      xRegulatePumpSpeed();

      break;
    }
    case eCookingStage_Done: {
      // Update cooking state
      stopBrewing();

      // Ask for screen refresh
      repaint = true;
      
      // Warn the user that the cooking is done
      xWarnCookEnded();

      break;
    }
  }
}

// #################################################### Helpers ##################################################################

void startBrewing() {
  //sing(MELODY_SUPER_MARIO, PIEZO_PIN);

  cooking = true;
}

void stopBrewing() {
  cooking = false;
}

void resetMenu( boolean requestRepaintPaint ) {
  eMenuType = eMenuType_Main;

  if( requestRepaintPaint ) {
    repaint = true;
  }
  
  // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
  xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
}

void backToStatus() {
  lastInterruptTime = millis() - SETTING_MAX_INACTIVITY_TIME - 1;
  resetMenu(true);
}
// #################################################### Helpers ##################################################################

// #################################################### Set Variables ##################################################################
int getTimer( int initialValue, int defaultValue ) {
  // set operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
  xSetupRotaryEncoder( eRotaryEncoderMode_Time, initialValue, 7200, 0, 1, 30 );

  // initialize variables
  int rotaryEncoderPreviousPosition = 0;
  int minutes = 0;
  int seconds = 0;
  
  // Setup Screen
  lcd.clear();
  lcd.home();        
  lcd.print("Set Time (");
  minutes = defaultValue/60;
  lcd.print(minutes);
  seconds = defaultValue-minutes*60;
  lcd.print(":");
  if(seconds<10) {
    lcd.print("0");
  }
  lcd.print(seconds);
  lcd.print(")");
  lcd.setCursor (0,LCD_VERTICAL_RESOLUTION-1);
  lcd.print("      0:00");
  
  while(true) {
    // Check if pushbutton is pressed
    if ((digitalRead(ROTARY_ENCODER_SW_PIN))) {  
      // Wait until switch is released
      while (digitalRead(ROTARY_ENCODER_SW_PIN)) {}  
      
      // debounce
      delay(10);

      // Job is done, break the circle
      break;
    } else {
      // Don't forget to keep an eye on the cooking
      xManageMachineSystems();
    }
    
    // display current timer
    if (rotaryEncoderVirtualPosition != rotaryEncoderPreviousPosition) {
      rotaryEncoderPreviousPosition = rotaryEncoderVirtualPosition;
      minutes = rotaryEncoderVirtualPosition/60;
      seconds = rotaryEncoderVirtualPosition-minutes*60;
      
      lcd.setCursor (0,LCD_VERTICAL_RESOLUTION-1);
      
      if(minutes<100) {
        lcd.print(" ");
      }
      if(minutes<10) {
        lcd.print(" ");
      }
      lcd.print("    ");
      lcd.print(minutes);
      lcd.print(":");
      if(seconds<10) {
        lcd.print("0");
      }
      lcd.print(seconds);
      lcd.println("                ");
    }
  }
  
  return rotaryEncoderVirtualPosition;
}

int getTimer( int initialValue ) {
  return getTimer( initialValue, initialValue );
}

int getTemperature(int initialValue) {
  
  // set operation state
  rotaryEncoderMode = eRotaryEncoderMode_Generic;
  rotaryEncoderVirtualPosition = initialValue;  

  // initialize variables
  int rotaryEncoderPreviousPosition = 0;
  
  // Setup Screen
  lcd.clear();
  lcd.home();        
  lcd.print("Set Temperature");
  lcd.setCursor (0,LCD_VERTICAL_RESOLUTION-1);
  lcd.print("       0 *C");
  
  rotaryEncoderMaxPosition = TEMPERATURE_SETTING_MAX_VALUE;
  
  while(true) {
    // Check if pushbutton is pressed
    if ((digitalRead(ROTARY_ENCODER_SW_PIN))) {  
      // Wait until switch is released
      while (digitalRead(ROTARY_ENCODER_SW_PIN)) {}  
      
      // debounce
      delay(10);

      // Job is done, break the circle
      break;
    } else {
      // Don't forget to keep an eye on the cooking
      xManageMachineSystems();
    }
    
    // display current timer
    if (rotaryEncoderVirtualPosition != rotaryEncoderPreviousPosition) {
      rotaryEncoderPreviousPosition = rotaryEncoderVirtualPosition;
      
      lcd.setCursor (0,LCD_VERTICAL_RESOLUTION-1);
      lcd.print("     ");
      if(rotaryEncoderVirtualPosition<10) {
        lcd.print("  ");
      }
      else {
        if(rotaryEncoderVirtualPosition<100) {
          lcd.print(" ");
        }
      }
      lcd.print(rotaryEncoderVirtualPosition);
      lcd.print(" *C");
      lcd.println("                ");
    }
  }
  
  return rotaryEncoderVirtualPosition;
}

int xSetGenericValue(int initialValue, int minimumValue, int maximumValue, const char *valueName, const char *unit) {  
  // set operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
  xSetupRotaryEncoder( eRotaryEncoderMode_Generic, initialValue, maximumValue, minimumValue, 1, 5 );

  // initialize variables
  int rotaryEncoderPreviousPosition = 0;
  
  // Setup Screen
  lcd.clear();
  lcd.home();
  lcd.print( "Set " );
  lcd.print( valueName );
  lcd.setCursor ( 0 , LCD_VERTICAL_RESOLUTION - 1 );
  lcd.print( "       0 " );
  lcd.print( unit );
  
  while(true) {
    // Check if pushbutton is pressed
    if ( digitalRead(ROTARY_ENCODER_SW_PIN) ) {
      // Wait until switch is released
      while ( digitalRead(ROTARY_ENCODER_SW_PIN) ) {}  
      
      // debounce
      delay( 10 );

      // Job is done, break the circle
      break;
    } else {
      // Don't forget to keep an eye on the cooking
      xManageMachineSystems();
    }
    
    // Check if there was an update by the rotary encoder
    if( rotaryEncoderVirtualPosition != rotaryEncoderPreviousPosition ) {
      rotaryEncoderPreviousPosition = rotaryEncoderVirtualPosition;
      
      lcd.setCursor( 0, LCD_VERTICAL_RESOLUTION - 1 );
      lcd.print( "     " );
      if( rotaryEncoderVirtualPosition < 10 ) {
        lcd.print( "  " );
      }
      else {
        if( rotaryEncoderVirtualPosition < 100 ) {
          lcd.print( " " );
        }
      }
      lcd.print( rotaryEncoderVirtualPosition );
      lcd.print( " " );
      lcd.print( unit );
      lcd.println( "                " );
    }
  }
  
  return rotaryEncoderVirtualPosition;
}

int xSetTemperature( int initialValue ) {  
  return xSetGenericValue( initialValue, TEMPERATURE_MIN_VALUE, TEMPERATURE_MAX_VALUE, "temperature", "*C" );
}

int xSetFinalYield( int initialValue ) {  
  return xSetGenericValue( initialValue, SETTING_MACHINE_YIELD_CAPACITY_MIN, SETTING_MACHINE_YIELD_CAPACITY_MAX, "Final Yield", "l" );
}

unsigned long getInactivityTime() {
  unsigned long now = millis();
  unsigned long rotaryEncoderInactivityTime = now - lastInterruptTime;

  if(rotaryEncoderInactivityTime > SETTING_MAX_INACTIVITY_TIME) {
    if (digitalRead(ROTARY_ENCODER_SW_PIN)) {
        while(digitalRead(ROTARY_ENCODER_SW_PIN)) {
          delay(ROTARY_ENCODER_SW_DEBOUNCE_TIME);
        }
        
        now = millis();
        rotaryEncoderInactivityTime = now - lastInterruptTime;
        rotarySwDetectTime = now;
        
        repaint = true;
        refresh = true;
    }
  }
  
  unsigned long switchInactivityTime = now - rotarySwDetectTime;
  return rotaryEncoderInactivityTime > switchInactivityTime ? switchInactivityTime : rotaryEncoderInactivityTime ;
}

// ###################### Set Variables ##################################################

void xWaitForAction(String title, String message) {
  while(true) {
    // Check if pushbutton is pressed
    if ( digitalRead(ROTARY_ENCODER_SW_PIN) ) {
      // Wait until switch is released
      while ( digitalRead(ROTARY_ENCODER_SW_PIN) ) {}  
      
      // debounce
      delay( 10 );

      // Job is done, break the circle
      break;
    } else {
      sing(BUZZ_1, PIEZO_PIN);

      // Print the message
      if(! lcdPrint(&lcd, title, message)) {
        break;
      }
    }
  }
}

boolean gotButtonPress(int iPin) {
  boolean ret = false;

  if ((digitalRead(iPin))) {    // check if pushbutton is pressed
    ret = true;
    while (digitalRead(iPin)) {}    // wait til switch is released
    delay(10);                            // debounce
  } 

  return ret;
}
