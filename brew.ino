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
#include <PT100.h>

// ++++++++++++++++++++++++ OTHER +++++++++++++++++++++++++++++++++
#include "debug.h"

#include "config.h"

#include "CustomDataStructures.h"

#include "Melody.h"
#include "Display.h"

// ######################### VARIABLES #########################
// ++++++++++++++++++++++++ State Machine ++++++++++++++++++++++++
eRotaryEncoderMode      rotaryEncoderMode;

eCookingStages          cookingStage;
eBeerProfile            beerProfile;

eMenuType               eMenuType;

eMainMenuOptions        eMainMenuPosition;
eMainMenuOptions        eMainMenuSelection;
eBeerProfileMenuOptions eBeerProfileMenuPosition;
eBeerProfileMenuOptions eBeerProfileMenuSelection;
eStageMenuOptions       eStageMenuPosition;
eStageMenuOptions       eStageMenuSelection;
eMaltMenuOptions        eMaltMenuPosition;
eMaltMenuOptions        eMaltMenuSelection; 
eSettingsMenuOptions    eSettingsMenuPosition;
eSettingsMenuOptions    eSettingsMenuSelection;

eMaltMenuOptions        maltMenuOption;
eSettingsMenuOptions    settingsMenuOption;

// ++++++++++++++++++++++++ Global Variables ++++++++++++++++++++++++
boolean                 cooking;
boolean                 bStageFirstRun;

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

// ++++++++++++++++++++++++ Heating Element Relay ++++++++++++++++++++++++
int                     iWindowSize;             // Time frame to operate in
unsigned long           windowStartTime;
double                  dWattPerPulse;

// ++++++++++++++++++++++++ Pump ++++++++++++++++++++++++
int                     iPumpSpeed;             // Time frame to operate in

// ######################### INITIALIZE #########################
// ++++++++++++++++++++++++ Library - LiquidCrystal_I2C ++++++++++++++++++++++++
LiquidCrystal_I2C       lcd(LCD_I2C_ADDR, LCD_EN_PIN, LCD_RW_PIN, LCD_RS_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

// +++++++++++++++++++++++ PT100 +++++++++++++++++++++++
PT100                   basePT100("base", PT100_BASE_OUTPUT_PIN, PT100_BASE_OUTPUT_R_PIN, PT100_BASE_INPUT_PIN, PT100_BASE_INPUT_R_PIN, PT100_BASE_TIME_BETWEEN_READINGS, PT100_BASE_DEFAULT_ADC_VMAX, PT100_BASE_DEFAULT_VS, PT100_BASE_DEFAULT_R1_RESISTENCE, PT100_BASE_DEFAULT_R2_RESISTENCE, 0.0, 0.0);//0.0, 0.0);
PT100                   upPT100("up", PT100_UP_OUTPUT_PIN, PT100_UP_OUTPUT_R_PIN, PT100_UP_INPUT_PIN, PT100_UP_INPUT_R_PIN, PT100_UP_TIME_BETWEEN_READINGS, PT100_UP_DEFAULT_ADC_VMAX, PT100_UP_DEFAULT_VS, PT100_UP_DEFAULT_R1_RESISTENCE, PT100_UP_DEFAULT_R2_RESISTENCE, -0.2, 8.0);//8.0);//0.17,  -7.2); //0.195, -7.6);//0.0, 0.0);//0.38, -3.0);  //0.112329092, -3.57); //0.0, 0.0);
PT100                   downPT100("down", PT100_DOWN_OUTPUT_PIN, PT100_DOWN_OUTPUT_R_PIN, PT100_DOWN_INPUT_PIN, PT100_DOWN_INPUT_R_PIN, PT100_DOWN_TIME_BETWEEN_READINGS, PT100_DOWN_DEFAULT_ADC_VMAX, PT100_DOWN_DEFAULT_VS, PT100_DOWN_DEFAULT_R1_RESISTENCE, PT100_DOWN_DEFAULT_R2_RESISTENCE, 0.0, -2.2);//0.228, -9); //0.26,  -10);//0.0, 0.0);//0.53, -6.6);  //0.22, -5.5); //0.0, 0.0);

// ######################### INTERRUPTS #########################
void isr ()  {    // Interrupt service routine is executed when a HIGH to LOW transition is detected on CLK
  unsigned long interruptTime = millis();
  unsigned long diff = interruptTime - lastInterruptTime;
  lastInterruptTime = interruptTime;
  
  // If interrupts come faster than [ROTARY_ENCODER_DEBOUNCE_TIME]ms, assume it's a bounce and ignore
  if (diff > ROTARY_ENCODER_DEBOUNCE_TIME) {
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
  }
  
  repaint = true;
  refresh = true;
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
  iPumpSpeed = PUMP_SPEED_STOP;
  xRegulatePumpSpeed();

  // Force shutdown
  analogWrite(PUMP_PIN, PUMP_SPEED_STOP);  // analogWrite values from 0 to 255
  digitalWrite(HEATING_ELEMENT_OUTPUT_PIN, LOW);  // Turn heading element OFF for safety
  bStatusElement = false;

  basePT100.setSampleDeviation( 0.0 );
  upPT100.setSampleDeviation( 0.0 );
  downPT100.setSampleDeviation( 0.0 );

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
  //  pinMode    (MIXER_PIN, OUTPUT);
  //  analogWrite    (MIXER_PIN, 0);

  // ++++++++++++++++++++++++ Pump ++++++++++++++++++++++++
  pinMode(PUMP_PIN, OUTPUT);   // sets the pin as output
  iPumpSpeed                  =   PUMP_SPEED_STOP;             // Time frame to operate in
  analogWrite(PUMP_PIN, iPumpSpeed);  // analogWrite values from 0 to 255

  // ++++++++++++++++++++++++ Pump ++++++++++++++++++++++++
  pinMode(PIEZO_PIN, OUTPUT);

  // ++++++++++++++++++++++++ Temperature Sensor PT100 ++++++++++++++++++++++++
  //basePT100.setup();
  /*
  analogReference  (INTERNAL1V1);          // EXTERNAL && INTERNAL2V56 && INTERNAL1V1
  pinMode    (PT100_OUTPUT_PIN, OUTPUT);  // setup temperature sensor input pin
  digitalWrite    (PT100_OUTPUT_PIN, LOW);    // initialize sensor off
  */
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

  // ++++++++++++++++++++++++ State Machine ++++++++++++++++++++++++
  eMenuType                   =   eMenuType_Main;
  
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
  bStageFirstRun              =   true;

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

  startpointTime              =   120;
  betaGlucanaseTime           =   0;
  debranchingTime             =   0;
  proteolyticTime             =   0;
  betaAmylaseTime             =   3600;
  alphaAmylaseTime            =   1800;
  mashoutTime                 =   300;
  recirculationTime           =   1200;
  spargeTime                  =   1200;
  boilTime                    =   5400;
  coolingTime                 =   120;
  cleaningTime                =   SETTING_CLEANING_TIME;

  startpointTemperature       =   30;
  betaGlucanaseTemperature    =   40;
  debranchingTemperature      =   40;
  proteolyticTemperature      =   50;
  betaAmylaseTemperature      =   60;
  alphaAmylaseTemperature     =   70;
  mashoutTemperature          =   80;
  recirculationTemperature    =   80;
  spargeTemperature           =   80;
  boilTemperature             =   100;
  coolingTemperature          =   25;
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
  //cleanSerialMonitor();

  unsigned long inactivityTime = millis() - lastInterruptTime;

  if(inactivityTime > SETTING_MAX_INACTIVITY_TIME) {    // Inactivity check
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
    case eSettingsMenu_PT100_Element: {
      // Stuff

      backToStatus();
      
      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eSettingsMenu_PT100_Up: {
      // Stuff

      backToStatus();
      
      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eSettingsMenu_PT100_Down: {
      // Stuff

      backToStatus();
      
      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
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
      
      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eMaltMenu_CastleMalting_Wheat_Blanc: {
      // Stuff

      backToStatus();
      
      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
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
  
      startpointTemperature = xSetGenericValue( startpointTemperature, 0, 120, "temperature", "*C" );

      backToStatus();

      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eStageMenu_BetaGlucanase: {
      betaGlucanaseTime = getTimer( betaGlucanaseTime );
  
      betaGlucanaseTemperature = xSetGenericValue( betaGlucanaseTemperature, 0, 120, "temperature", "*C" );

      backToStatus();

      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eStageMenu_Debranching: {
      debranchingTime = getTimer( debranchingTime );
  
      debranchingTemperature = xSetGenericValue( debranchingTemperature, 0, 120, "temperature", "*C" );

      backToStatus();

      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eStageMenu_Proteolytic: {
      proteolyticTime = getTimer( proteolyticTime );
      
      proteolyticTemperature = xSetGenericValue( proteolyticTemperature, 0, 120, "temperature", "*C" );

      backToStatus();

      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eStageMenu_BetaAmylase: {
      betaAmylaseTime = getTimer( betaAmylaseTime );
  
      betaAmylaseTemperature = xSetGenericValue( betaAmylaseTemperature, 0, 120, "temperature", "*C" );

      backToStatus();

      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eStageMenu_AlphaAmylase: {
      alphaAmylaseTime = getTimer( alphaAmylaseTime );
  
      alphaAmylaseTemperature = xSetGenericValue( alphaAmylaseTemperature, 0, 120, "temperature", "*C" );

      backToStatus();

      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eStageMenu_Mashout: {
      mashoutTime = getTimer( mashoutTime );
  
      mashoutTemperature = xSetGenericValue( mashoutTemperature, 0, 120, "temperature", "*C" );

      backToStatus();

      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eStageMenu_Recirculation: {
      recirculationTime = getTimer( recirculationTime );
  
      recirculationTemperature = xSetGenericValue( recirculationTemperature, 0, 120, "temperature", "*C" );

      backToStatus();

      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eStageMenu_Sparge: {
      spargeTime = getTimer( spargeTime );
  
      spargeTemperature = xSetGenericValue( spargeTemperature, 0, 120, "temperature", "*C" );

      backToStatus();

      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eStageMenu_Boil: {
      boilTime = getTimer( boilTime );
      
      boilTemperature = xSetGenericValue( boilTemperature, 0, 120, "temperature", "*C" );

      backToStatus();

      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eStageMenu_Cooling: {
      coolingTime = getTimer( coolingTime );
      
      coolingTemperature = xSetGenericValue( coolingTemperature, 0, 120, "temperature", "*C" );

      backToStatus();

      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
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

      startpointTime              =   120;
      betaGlucanaseTime           =   0;
      debranchingTime             =   0;
      proteolyticTime             =   0;
      betaAmylaseTime             =   3600;
      alphaAmylaseTime            =   1800;
      mashoutTime                 =   300;
      recirculationTime           =   1200;
      spargeTime                  =   1200;
      boilTime                    =   5400;
      coolingTime                 =   120;

      startpointTemperature       =   30;
      betaGlucanaseTemperature    =   40;
      debranchingTemperature      =   40;
      proteolyticTemperature      =   50;
      betaAmylaseTemperature      =   60;
      alphaAmylaseTemperature     =   70;
      mashoutTemperature          =   80;
      recirculationTemperature    =   80;
      spargeTemperature           =   80;
      boilTemperature             =   100;
      coolingTemperature          =   25;

      backToStatus();

      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
          
      break;
    }
    case eBeerProfileMenu_Trigo: {
      beerProfile                 =   eBeerProfile_Trigo;

      startpointTime              =   120;
      betaGlucanaseTime           =   0;
      debranchingTime             =   0;
      proteolyticTime             =   0;
      betaAmylaseTime             =   3600;
      alphaAmylaseTime            =   1800;
      mashoutTime                 =   300
      recirculationTime           =   1200
      spargeTime                  =   1200;
      boilTime                    =   5400;
      coolingTime                 =   120;

      startpointTemperature       =   45;
      betaGlucanaseTemperature    =   40;
      debranchingTemperature      =   40;
      proteolyticTemperature      =   50;
      betaAmylaseTemperature      =   62;
      alphaAmylaseTemperature     =   70;
      mashoutTemperature          =   78;
      recirculationTemperature    =   80;
      spargeTemperature           =   80;
      boilTemperature             =   100;
      coolingTemperature          =   25;

      backToStatus();

      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eBeerProfileMenu_IPA: {
      beerProfile                 =   eBeerProfile_IPA;

      backToStatus();
      
      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );

      break;
    }
    case eBeerProfileMenu_Belga: {
      beerProfile                 =   eBeerProfile_Belga;

      backToStatus();
      
      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eBeerProfileMenu_Red: {
      beerProfile                 =   eBeerProfile_Red;

      backToStatus();
      
      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eBeerProfileMenu_APA: {
      beerProfile                 =   eBeerProfile_APA;

      backToStatus();
      
      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eBeerProfileMenu_Custom: {
      beerProfile                 =   eBeerProfile_Custom;

      startpointTime              =   120;
      betaGlucanaseTime           =   120;
      debranchingTime             =   120;
      proteolyticTime             =   120;
      betaAmylaseTime             =   120;
      alphaAmylaseTime            =   120;
      mashoutTime                 =   120;
      recirculationTime           =   120;
      spargeTime                  =   120;
      boilTime                    =   120;
      coolingTime                 =   120;

      startpointTemperature       =   50;
      betaGlucanaseTemperature    =   55;
      debranchingTemperature      =   60;
      proteolyticTemperature      =   65;
      betaAmylaseTemperature      =   70;
      alphaAmylaseTemperature     =   75;
      mashoutTemperature          =   80;
      recirculationTemperature    =   85;
      spargeTemperature           =   90;
      boilTemperature             =   95;
      coolingTemperature          =   100;

      backToStatus();
      
      eMenuType = eMenuType_Main;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
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

void runMainMenuSelection() {
  switch(eMainMenuSelection) {
    case eMainMenu_GO: {
      finalYield = xSetGenericValue( finalYield, 0, 50, "Final Yield", "l" );

      startBrewing();

      xSetupGlobalVariablesForStage( eCookingStage_Startpoint );

      backToStatus();
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      xPurgePump();

      break;
    }
    case eMainMenu_STOP: {
      stopBrewing();

      backToStatus();
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eMainMenu_SKIP: {
      cookTime = 0;

      backToStatus();
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eMainMenu_BeerProfile: {
      eMenuType = eMenuType_BeerProfile;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eBeerProfileMenuPosition, MENU_SIZE_PROFILES_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eMainMenu_Stage: {
      eMenuType = eMenuType_Stage;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eStageMenuPosition, MENU_SIZE_STAGE_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eMainMenu_Malt: {
      eMenuType = eMenuType_Malt;
      repaint = true;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMaltMenuPosition, MENU_SIZE_MALT_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eMainMenu_Hops: {
      backToStatus();
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eMainMenu_Clean: {
      // Stop anything that might be still going on
      xSafeHardwarePowerOff();

      // Start at the Clean stage
      startBrewing();

      xSetupGlobalVariablesForStage( eCookingStage_Clean );

      backToStatus();
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      xPurgePump();

      break;
    }
    case eMainMenu_Purge: {
      // Stop anything that might be still going on
      xSafeHardwarePowerOff();

      // Start at the Purge stage
      startBrewing();

      xSetupGlobalVariablesForStage( eCookingStage_Purge );

      backToStatus();
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );
      
      xPurgePump();

      break;
    }
    case eMainMenu_Settings: {
      eMenuType = eMenuType_Settings;
      repaint = true;

      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eSettingsMenuPosition, MENU_SIZE_SETTINGS_MENU - 1, 1, 1, 0 );
      
      break;
    }
    case eMainMenu_Back: {
      backToStatus();
        
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenuPosition, MENU_SIZE_MAIN_MENU - 1, 1, 1, 0 );

      break;
    }
    default: {
    }
  }

  eMainMenuSelection = eMainMenu_NULL;
}


void xCountTheTime( int temperatureRange, boolean bAverageUpDown ) {
  unsigned long now = millis();
  unsigned long elapsedTime = now - clockLastUpdate;
  double temperatureCount = 0;
  
  if( bAverageUpDown ) {
    float tup = upPT100.getCurrentTemperature();
    float tdown = downPT100.getCurrentTemperature();
    if(tup > tdown) {
      temperatureCount = tup;
    }
    else {
      temperatureCount = tdown;
    }
  } else {
    temperatureCount = basePT100.getCurrentTemperature();
  }

  // Check if the machine is in the right temperature range, for the current mode,
  //if(!( temperatureCount > (cookTemperature - temperatureRange) && temperatureCount < (cookTemperature + temperatureRange))) {
  float margin = temperatureRange;
  if( cookTemperature >= 100.0 ) {
    margin = 2.0;
  }
  if( temperatureCount < (cookTemperature - margin) ) {
    clockIgnore += elapsedTime;
  }
  
  // Calculate the remaining time on the clock
  clockCounter = cookTime * 1000 - (now - clockStartTime - clockIgnore);

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

bool xRegulateTemperature( boolean bAverageUpDown ) {
  double difference = 0;
  bool overTemperature = false;
  double wattage = 0.0;
  
  if( bAverageUpDown ) {
    float tbase = basePT100.getCurrentTemperature();
    float tup = upPT100.getCurrentTemperature();
    float tdown = downPT100.getCurrentTemperature();

    if(tup > tdown) {
      difference = cookTemperature - tup;
    }
    else {
      difference = cookTemperature - tdown;
    }

    if (tbase > (cookTemperature + 2.0)) {
      difference = 0.0;
    }

    if (tbase < (cookTemperature)) {
      difference = cookTemperature - tbase;
    }

  } else {
    difference = cookTemperature - basePT100.getCurrentTemperature();
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
    if(difference <= 0.1) {
      // turn it off
      wattage = 0.0;
    } else {
      if(difference <= 0.5) {
        // pulse lightly at 500 watt
        if(cookTemperature > 99.0) {
          wattage = 1500.0;
        }
        else {
          wattage = 500.0;
        }
      } else {
        if(difference <= 1.0) {
          // pulse moderately at 1000 watt
          wattage = 1000.0;
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
    }
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
    analogWrite(PUMP_PIN, PUMP_SPEED_MAX);  // analogWrite values from 0 to 255
    delay(1000);
    analogWrite(PUMP_PIN, PUMP_SPEED_STOP);  // analogWrite values from 0 to 255
    delay(1500);
  }
}

bool xRegulatePumpSpeed() {
  //  analogWrite(PUMP_PIN, iPumpSpeed);  // analogWrite values from 0 to 255

  if(basePT100.getCurrentTemperature() > PUMP_TEMPERATURE_MAX_OPERATION) {
    analogWrite(PUMP_PIN, PUMP_SPEED_STOP);  // analogWrite values from 0 to 255

    basePT100.setSampleDeviation( 0.0 );
    upPT100.setSampleDeviation( 0.0 );
    downPT100.setSampleDeviation( 0.0 );
  }
  else {
    analogWrite(PUMP_PIN, iPumpSpeed);  // analogWrite values from 0 to 255

    basePT100.setSampleDeviation( -2.0 );
    upPT100.setSampleDeviation( -2.0 );
    downPT100.setSampleDeviation( -2.0 );
  }
}

void xWarnClockEnded() {
  sing(MELODY_SUPER_MARIO_START, PIEZO_PIN);
}

void xWarnCookEnded() {
  sing(MELODY_UNDERWORLD_SHORT, PIEZO_PIN);
}

void xStageFirstRun( int stageTime, int stageTemperature, int stagePumpSpeed, eCookingStages stage ) {
  #ifdef DEBUG_OFF
  debugPrintFunction("xStageFirstRun");
  debugPrintVar("cookingStage", stage);
  #endif

  // Set Stage
  bStageFirstRun = true;
  cookingStage = stage;

  // Set the clock
  cookTime = stageTime;
  
  // Set the target temperature
  cookTemperature = stageTemperature;
  
  // Reset the clock
  clockStartTime = millis();
  clockLastUpdate = clockStartTime;
  clockIgnore = 0;

  // Set the pump speed
  iPumpSpeed = stagePumpSpeed;
}

void xSetupGlobalVariablesForStage(eCookingStages nextStage) {
  #ifdef DEBUG_OFF
  debugPrintFunction("xSetupGlobalVariablesForStage");
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
        default: {

        }
      }

      // Make sure there is water
      xWaitForAction("Water", "Make sure there is water in the machine before start cooking.");

      repaint = true;

      // A basic operation for a basic stage
      xStageFirstRun( startpointTime, startpointTemperature, PUMP_SPEED_MAX, eCookingStage_Startpoint );
      
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
        default: {

        }
      }

      // A basic operation for a basic stage
      xStageFirstRun( betaGlucanaseTime, betaGlucanaseTemperature, PUMP_SPEED_MAX, eCookingStage_BetaGlucanase );
      
      break;
    }
    case eCookingStage_Debranching: {
      // A basic operation for a basic stage
      xStageFirstRun( debranchingTime, debranchingTemperature, PUMP_SPEED_MAX, eCookingStage_Debranching );
      
      break;
    }
    case eCookingStage_Proteolytic: {
      // A basic operation for a basic stage
      xStageFirstRun( proteolyticTime, proteolyticTemperature, PUMP_SPEED_MAX, eCookingStage_Proteolytic );
      
      break;
    }
    case eCookingStage_BetaAmylase: {
      // A basic operation for a basic stage
      xStageFirstRun( betaAmylaseTime, betaAmylaseTemperature, PUMP_SPEED_MAX, eCookingStage_BetaAmylase );
      
      break;
    }
    case eCookingStage_AlphaAmylase: {
      // A basic operation for a basic stage
      xStageFirstRun( alphaAmylaseTime, alphaAmylaseTemperature, PUMP_SPEED_MAX, eCookingStage_AlphaAmylase );
      
      break;
    }
    case eCookingStage_Mashout: {
      // A basic operation for a basic stage
      xStageFirstRun( mashoutTime, mashoutTemperature, PUMP_SPEED_MAX, eCookingStage_Mashout );
      
      break;
    }
    case eCookingStage_Recirculation: {// Make sure there is water
      xWaitForAction("Sparge Water", "Start heating your sparge water.");

      repaint = true;

      // A basic operation for a basic stage
      xStageFirstRun( recirculationTime, recirculationTemperature, PUMP_SPEED_MAX, eCookingStage_Recirculation );
      
      break;
    }
    case eCookingStage_Sparge: {
      // Make sure there is water
      xWaitForAction("Sparge Water", "Start pouring the sparge water.");

      repaint = true;

      // A basic operation for a basic stage
      xStageFirstRun( spargeTime, spargeTemperature, PUMP_SPEED_MAX, eCookingStage_Sparge );
      
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
        default: {
          xWaitForAction("Hops", "Add the hops in the right order, at the right time.");

        }
      }

      repaint = true;

      // A basic operation for a basic stage
      xStageFirstRun( boilTime, boilTemperature, PUMP_SPEED_MAX, eCookingStage_Boil );
      
      break;
    }
    case eCookingStage_Cooling: {
      // Make sure there is water
      xWaitForAction("Coil", "Add the coil and connect it to the main water supply.");

      repaint = true;

      // A basic operation for a basic stage
      xStageFirstRun( coolingTime, coolingTemperature, PUMP_SPEED_MAX, eCookingStage_Cooling );

      break;
    }
    case eCookingStage_Clean: {
      // Make sure there is water
      xWaitForAction("Water", "Add 13 liters.");

      // Make sure there is water
      xWaitForAction("Star San HB", "Add 0.89oz/26ml.");

      repaint = true;

      // A basic operation for a basic stage
      xStageFirstRun( cleaningTime, cleaningTemperature, PUMP_SPEED_MAX, eCookingStage_Clean );

      break;
    }
    case eCookingStage_Purge: {
      // A basic operation for a basic stage
      xStageFirstRun( 0, 0, PUMP_SPEED_MAX, eCookingStage_Purge );

      xRegulatePumpSpeed();

      break;
    }
    case eCookingStage_Done: {
      // A basic operation for a basic stage
      xStageFirstRun( 0, 0, PUMP_SPEED_STOP, eCookingStage_Done );

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
  xSetupGlobalVariablesForStage( nextStage );
}

void xBasicStageOperation( int iStageTime, int iStageTemperature, int iStageTemperatureRange, eCookingStages nextStage, boolean bAverageUpDown ) {
  if(bStageFirstRun) {
    // Don't run this again
    bStageFirstRun = false;

    //xStageFirstRun( iStageTime, iStageTemperature, 255 );
    
    // When the stage should be skipped
    if( iStageTime == 0) {
      // Continue to the next stage
      //xSetupGlobalVariablesForStage( nextStage );
      
      // There is nothing to do, in this stage
      return;
    } else {
      // Set the clock, target temperature and Reset the clock
      //xStageFirstRun( iStageTime, iStageTemperature, PUMP_SPEED_SLOW );
      //xStageFirstRun( iStageTime, iStageTemperature, 255 );
    }
  } else {
    // Account for time spent at the target temperature | Input 1: range in ºC within which the target temperature is considered to be reached
    #ifdef DEBUG_OFF
    xCountTheTime( iStageTemperatureRange, false );
    #else
    xCountTheTime( iStageTemperatureRange, bAverageUpDown );
    #endif
    
    if( isTimeLeft() ) {
      // Do temperature control
      xRegulateTemperature( bAverageUpDown );

      // Do flow control
      xRegulatePumpSpeed();
      
    } else {
      #ifdef DEBUG_OFF
      debugPrintFunction("xBasicStageOperation");
      debugPrintVar("clockCounter", clockCounter);
      #endif
      // Continue to the next stage
      xTransitionIntoStage( nextStage );
      
      // There is nothing to do, in this stage
      return;
    }
  }
  // There is nothing to do, in this iteration
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
  basePT100.measure1(false, false);
  upPT100.measure1(false, false);
  downPT100.measure1(true, false);

  // If cooking is done, return (this is a nice place to double check safety and ensure the cooking parts aren't on.
  if(!cooking) {
    xSafeHardwarePowerOff();

    return;
  }
  
  // Operate the machine according to the current mode
  switch(cookingStage) {
    case eCookingStage_Startpoint: {
      // A basic operation for a basic stage
      xBasicStageOperation( startpointTime, startpointTemperature, 1, eCookingStage_BetaGlucanase, false);
      
      break;
    }
    case eCookingStage_BetaGlucanase: {
      // A basic operation for a basic stage
      xBasicStageOperation( betaGlucanaseTime, betaGlucanaseTemperature, 0, eCookingStage_Debranching, true );
      
      break;
    }
    case eCookingStage_Debranching: {
      // A basic operation for a basic stage
      xBasicStageOperation( debranchingTime, debranchingTemperature, 0, eCookingStage_Proteolytic, true );
      
      break;
    }
    case eCookingStage_Proteolytic: {
      // A basic operation for a basic stage
      xBasicStageOperation( proteolyticTime, proteolyticTemperature, 0, eCookingStage_BetaAmylase, true );
      
      break;
    }
    case eCookingStage_BetaAmylase: {
      // A basic operation for a basic stage
      xBasicStageOperation( betaAmylaseTime, betaAmylaseTemperature, 0, eCookingStage_AlphaAmylase, true );
      
      break;
    }
    case eCookingStage_AlphaAmylase: {
      // A basic operation for a basic stage
      xBasicStageOperation( alphaAmylaseTime, alphaAmylaseTemperature, 0, eCookingStage_Mashout, true );
      
      break;
    }
    case eCookingStage_Mashout: {
      // A basic operation for a basic stage
      xBasicStageOperation( mashoutTime, mashoutTemperature, 0, eCookingStage_Recirculation, true );
      
      break;
    }
    case eCookingStage_Recirculation: {
      // A basic operation for a basic stage
      xBasicStageOperation( recirculationTime, recirculationTemperature, 0, eCookingStage_Sparge, true );
      
      break;
    }
    case eCookingStage_Sparge: {
      // A basic operation for a basic stage
      xBasicStageOperation( spargeTime, spargeTemperature, 1, eCookingStage_Boil, false );
      
      break;
    }
    case eCookingStage_Boil: {
      // A basic operation for a basic stage
      xBasicStageOperation( boilTime, boilTemperature, 2, eCookingStage_Cooling, false );
      
      break;
    }
    case eCookingStage_Cooling: {
      // A basic operation for a basic stage
      xBasicStageOperation( coolingTime, coolingTemperature, 1, eCookingStage_Done, false );

      break;
    }
    case eCookingStage_Clean: {
      // A basic operation for a basic stage
      xBasicStageOperation( cleaningTime, cleaningTemperature, 1, eCookingStage_Done, false );

      break;
    }
    case eCookingStage_Purge: {
      // A basic operation for a basic stage
      //xBasicStageOperation( coolingTime, coolingTemperature, 1, eCookingStage_Done );
      iPumpSpeed = PUMP_SPEED_MAX;

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

void backToStatus() {
  lastInterruptTime = millis() - SETTING_MAX_INACTIVITY_TIME - 1;
}
// #################################################### Helpers ##################################################################

// #################################################### Set Variables ##################################################################
int getTimer(int init) {
  // set operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
  xSetupRotaryEncoder( eRotaryEncoderMode_Time, init, 7200, 0, 1, 30 );

  // initialize variables
  int rotaryEncoderPreviousPosition = 0;
  int minutes = 0;
  int seconds = 0;
  
  // Setup Screen
  lcd.clear();
  lcd.home();        
  lcd.print("Set Time");
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
      lcd.print("      ");
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

int getTemperature(int init) {
  
  // set operation state
  rotaryEncoderMode = eRotaryEncoderMode_Generic;
  rotaryEncoderVirtualPosition = init;  

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

int xSetGenericValue(int init, int min, int max, char *valueName, char *unit) {  
  // set operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
  xSetupRotaryEncoder( eRotaryEncoderMode_Generic, init, max, min, 1, 5 );

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

void cleanSerialMonitor() {
  for (int i = 0; i < 40; i++) {
    Serial.println();
  }
}