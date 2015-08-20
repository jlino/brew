#define DEBUG

// ######################### CONSTANTS #########################

// ######################### SETTINGS #########################
// ++++++++++++++++++++++++ Heating Element Relay ++++++++++++++++++++++++
#define HEATING_ELEMENT_DEFAULT_WINDOW_SIZE       1000
#define HEATING_ELEMENT_OUTPUT_PIN                24
#define HEATING_ELEMENT_MAX_HEAT_PWM_INTEGER      5
#define HEATING_ELEMENT_MAX_HEAT_PWM_FLOAT        5.0
#define HEATING_ELEMENT_MAX_WATTAGE               3000.0          // Minimum = 2000.0
#define HEATING_ELEMENT_AC_FREQUENCY_HZ           50.0

// ++++++++++++++++++++++++ Temperature ++++++++++++++++++++++++
#define PT100_BASE_INPUT_PIN                      A4
#define PT100_BASE_OUTPUT_PIN                     32
#define PT100_BASE_TIME_BETWEEN_READINGS          100
#define PT100_UP_INPUT_PIN                        A5
#define PT100_UP_OUTPUT_PIN                       30
#define PT100_UP_TIME_BETWEEN_READINGS            100
#define PT100_DOWN_INPUT_PIN                      A6
#define PT100_DOWN_OUTPUT_PIN                     31
#define PT100_DOWN_TIME_BETWEEN_READINGS          100

#define PT100_BASE_DEFAULT_ADC_VMAX               1.1
#define PT100_BASE_DEFAULT_VS                     5.0
#define PT100_BASE_DEFAULT_R1_RESISTENCE          608.99
#define PT100_BASE_DEFAULT_LINE_RESISTENCE        0.5408314 
#define PT100_BASE_DEFAULT_OPERATION_RESISTENCE   0.0
#define PT100_UP_DEFAULT_ADC_VMAX                 1.1
#define PT100_UP_DEFAULT_VS                       5.0
#define PT100_UP_DEFAULT_R1_RESISTENCE            608.95
#define PT100_UP_DEFAULT_LINE_RESISTENCE          0.5408314
#define PT100_UP_DEFAULT_OPERATION_RESISTENCE     0.0
#define PT100_DOWN_DEFAULT_ADC_VMAX               1.1
#define PT100_DOWN_DEFAULT_VS                     5.0
#define PT100_DOWN_DEFAULT_R1_RESISTENCE          608.95
#define PT100_DOWN_DEFAULT_LINE_RESISTENCE        0.5408314
#define PT100_DOWN_DEFAULT_OPERATION_RESISTENCE   0.0

// ++++++++++++++++++++++++ Mixer ++++++++++++++++++++++++
//#define MIXER_PIN     12
//#define MIXER_MAX_POSITION   255

// ++++++++++++++++++++++++ Pump ++++++++++++++++++++++++
#include "Melody.h"
#define PIEZO_PIN                                 25

// ++++++++++++++++++++++++ Pump ++++++++++++++++++++++++
#define PUMP_PIN                                  6
#define PUMP_SPEED_STOP                           0
#define PUMP_SPEED_SLOW                           64
#define PUMP_SPEED_AVERAGE                        128
#define PUMP_SPEED_FAST                           192
#define PUMP_SPEED_MAX                            255

// ++++++++++++++++++++++++ Rotary Encoder ++++++++++++++++++++++++
#define ROTARY_ENCODER_INTERRUPT_NUMBER           1    // On Mega2560 boards, interrupt 1 is on pin 3
#define ROTARY_ENCODER_CLK_PIN                    3    // Used for generating interrupts using CLK signal
#define ROTARY_ENCODER_DT_PIN                     22    // Used for reading DT signal
#define ROTARY_ENCODER_SW_PIN                     23    // Used for the push button switch
#define ROTARY_ENCODER_DEBOUNCE_TIME              20    // Number of miliseconds to ignore new signals a signal is received

// ++++++++++++++++++++++++ State Machine ++++++++++++++++++++++++
#define SETTING_WELCOME_TIMEOUT                   100
#define SETTING_MAX_INACTIVITY_TIME               3000
#define MENU_MAX_DEPTH                            10
#define MENU_INIT_VALUES                          -1,-1,-1,-1,-1,-1,-1,-1,-1,-1
#define MENU_SIZE_MAIN_MENU                       17
#define SETTING_SERIAL_MONITOR_BAUD_RATE          9600
#define SETTING_SERIAL_MONITOR_WELCOME_MESSAGE    "Let's start Brewing!"

// ######################### LIBRARIES #########################
// ++++++++++++++++++++++++ LiquidCrystal_I2C ++++++++++++++++++++++++
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define LCD_I2C_ADDR                              0x27    // <<----- Add your address here.  Find it from I2C Scanner
#define LCD_HORIZONTAL_RESOLUTION                 16
#define LCD_VERTICAL_RESOLUTION                   2
#define LCD_BACKLIGHT_PIN                         3
#define LCD_EN_PIN                                2
#define LCD_RW_PIN                                1
#define LCD_RS_PIN                                0
#define LCD_D4_PIN                                4
#define LCD_D5_PIN                                5
#define LCD_D6_PIN                                6
#define LCD_D7_PIN                                7

// ++++++++++++++++++++++++ PT100 +++++++++++++++++++++++++++++++++
#include <PT100.h>

// ++++++++++++++++++++++++ ENUM +++++++++++++++++++++++++++++++++
#include "CustomDataStructures.h"

// ######################### TEMPLATES #########################
// ++++++++++++++++++++++++ Debug ++++++++++++++++++++++++
template <class T> void debugPrintVar( char *name, const T& value );
template <class T> void debugPrintVar( char *name, const T& value ) {
  Serial.print("[");
  Serial.print(name);
  Serial.print(":");
  Serial.print(value);
  Serial.println("]");
}
void debugPrintFunction( char *name ) {
  Serial.print("++++++++++++++++++++++++ ");
  Serial.print(name);
  Serial.println("++++++++++++++++++++++++");
}

// ######################### VARIABLES #########################
// ++++++++++++++++++++++++ State Machine ++++++++++++++++++++++++
eRotaryEncoderMode      rotaryEncoderMode;
eMainMenuOptions        mainMenuOption;
ePresetsMenuOptions     presetsMenuOption;
eMaltMenuOptions        maltMenuOption;
eSettingsMenuOptions    settingsMenuOption;
eCookingStages          cookingStage;

// ++++++++++++++++++++++++ Global Variables ++++++++++++++++++++++++
boolean                 cooking;
boolean                 bStageFirstRun;

int                     clockStartTime;
int                     clockCounter;
int                     clockIgnore;
boolean                 clockStart;
boolean                 clockEnd;

int                     cookTime;
int                     cookTemperature;
//cook_mode_list        cookMode;
//int                   cookMixerSpeed;
int                     cookHeatPWM;
        
int                     startpointTime;
int                     betaGlucanaseTime;
int                     debranchingTime;
int                     proteolyticTime;
int                     betaAmylaseTime;
int                     alphaAmylaseTime;
int                     mashoutTime;
int                     recirculationTime;
int                     spargeTime;
int                     boilTime;
int                     coolingTime;
        
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

//int                   menuSize;
int                     menu_position[MENU_MAX_DEPTH] = {MENU_INIT_VALUES};

boolean                 refresh;
boolean                 repaint;

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
PT100                   basePT100("base", PT100_BASE_OUTPUT_PIN, PT100_BASE_INPUT_PIN, PT100_BASE_TIME_BETWEEN_READINGS, PT100_BASE_DEFAULT_ADC_VMAX, PT100_BASE_DEFAULT_VS, PT100_BASE_DEFAULT_R1_RESISTENCE, PT100_BASE_DEFAULT_LINE_RESISTENCE, PT100_BASE_DEFAULT_OPERATION_RESISTENCE);
PT100                   upPT100("up", PT100_UP_OUTPUT_PIN, PT100_UP_INPUT_PIN, PT100_UP_TIME_BETWEEN_READINGS, PT100_UP_DEFAULT_ADC_VMAX, PT100_UP_DEFAULT_VS, PT100_UP_DEFAULT_R1_RESISTENCE, PT100_UP_DEFAULT_LINE_RESISTENCE, PT100_UP_DEFAULT_OPERATION_RESISTENCE);
PT100                   downPT100("down", PT100_DOWN_OUTPUT_PIN, PT100_DOWN_INPUT_PIN, PT100_DOWN_TIME_BETWEEN_READINGS, PT100_DOWN_DEFAULT_ADC_VMAX, PT100_DOWN_DEFAULT_VS, PT100_DOWN_DEFAULT_R1_RESISTENCE, PT100_DOWN_DEFAULT_LINE_RESISTENCE, PT100_DOWN_DEFAULT_OPERATION_RESISTENCE);

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

//analogWrite(MIXER_PIN, 0);        // Turn mixer OFF for safety
}

void displayWelcome() {
#ifndef DEBUG
  lcdPrint("  Let's start", "    Brewing!");    // Write welcome

  // Play Melody;
  sing(MELODY_SUPER_MARIO_START, PIEZO_PIN);

  //termometerCalibration();
  delay(SETTING_WELCOME_TIMEOUT);      // pause for effect
#endif
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
  presetsMenuOption           =   ePresetsMenu_Trigo;
  maltMenuOption              =   eMaltMenu_CastleMalting_Chteau_Pilsen_2RS;
  settingsMenuOption          =   eSettingsMenu_PT100_Element;
  cookingStage                =   eCookingStage_Startpoint;
  // ++++++++++++++++++++++++ Global Variables ++++++++++++++++++++++++
  cooking                     =   false;
  bStageFirstRun              =   true;

  clockStartTime              =   0;
  clockCounter                =   0;
  clockIgnore                 =   0;
  clockStart                  =   false;
  clockEnd                    =   false;
                        
  cookTime                    =   3600;
  cookTemperature             =   25;
  //cookMode                  =   quick_start;
  //cookMixerSpeed            =   120;
  cookHeatPWM                 =   5;

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

  startpointTemperature       =   45;
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

  refresh                     =   true;
  repaint                     =   true;

  // ++++++++++++++++++++++++ Interrupts ++++++++++++++++++++++++
  lastInterruptTime           =   0;

  // ++++++++++++++++++++++++ PID  ++++++++++++++++++++++++
  iWindowSize                 =   HEATING_ELEMENT_DEFAULT_WINDOW_SIZE;    // Time frame to operate in

// ######################### Code - Run Once #########################
  xSafeHardwarePowerOff           ();
  displayWelcome                  ();
  
  xSetupRotaryEncoder             ( eRotaryEncoderMode_Menu, eMainMenu_GO, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
}

// ######################### MAIN LOOP #########################

void loop() {
  unsigned long inactivityTime = millis() - lastInterruptTime;

  if(inactivityTime > SETTING_MAX_INACTIVITY_TIME) {    // Inactivity check
    if(refresh) {
      repaint = true;
      refresh = false;
    }
    displayStatus();
  }
  else {
    displayMainMenu();
  }
  
  operateMachine();
}

// ######################### FUNCTIONS ########################

void xPaintStatusTemplate() {
  // Clear LCD
  lcd.clear();        

  // Position the cursor at the begining of where the temperature template goes onto the screen
  lcd.home();    

  // Print the target and measured temperature template
  if(cooking) {
    lcd.print("ON : 000*C/000*C");
  }
  else {
    lcd.print("OFF: 000*C/000*C");
  }

  // Position the cursor at the begining of where the mode and time template goes onto the screen
  lcd.setCursor (0,LCD_VERTICAL_RESOLUTION-1);

  lcd.print("****       00:00");
}

void displayStatus() {

  // Check whether a template repaint is required
  if(repaint) {
    // Repaint the LCD template
    xPaintStatusTemplate();
    
    // Reset the repaint flag after the repaint has been done
    repaint = false;
  }

  // Print positions with no numbers, before the measured temperature value
  lcd.setCursor (3,0);
  if (basePT100.getCurrentTemperature() < 10) {
    lcd.print("  ");
  }
  else {
    if (basePT100.getCurrentTemperature() < 100) {
      lcd.print(" ");
    }
  }

  // Print measured temperature value onto the LCD
  lcd.print(basePT100.getCurrentTemperature(), 1);

  // Print positions with no numbers, before the target temperature value
  lcd.setCursor (11,0);
  if (cookTemperature < 10) {
    lcd.print("  ");
  }
  else {
    if (cookTemperature < 100) {
      lcd.print(" ");
    }
  }

  // Print target temperature value onto the LCD
  lcd.print(cookTemperature);
  
  // Calculate the numbers on the timer clock
  int minutes = clockCounter / 60;
  int seconds = clockCounter - minutes * 60;

  // Position the cursor at the begining of where the timer goes onto the screen
  lcd.setCursor (10, 1);
  
  // Print the timer values onto the LCD
  if (minutes < 10) {
    lcd.print(" 0");
  }
  else {
    if (minutes < 100) {
      lcd.print(" ");
    }
  }
  lcd.print(minutes);
  lcd.print(":");
  if(seconds<10) {
    lcd.print("0");
  }
  lcd.print(seconds);
}

void displayMainMenu() {
  switch(menu_position[0]) {
    case eMainMenu_GO: {
      MainMenu_GO();
      
      menu_position[0] = -1;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_GO, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      
      break;
    }
    case eMainMenu_STOP: {
      MainMenu_STOP();
      
      menu_position[0] = -1;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_GO, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      
      break;
    }
    case eMainMenu_Presets: {
      MainMenu_Presets();
      
      menu_position[0] = -1;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_Presets, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      
      break;
    }
    case eMainMenu_Malt: {
      MainMenu_Malt();
      
      menu_position[0] = -1;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_Malt, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      
      break;
    }
    case eMainMenu_Startpoint: {
      MainMenu_Startpoint();
      
      menu_position[0] = -1;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_Startpoint, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      
      break;
    }
    case eMainMenu_BetaGlucanase: {
      MainMenu_BetaGlucanase();
      
      menu_position[0] = -1;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_BetaGlucanase, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      
      break;
    }
    case eMainMenu_Debranching: {
      MainMenu_Debranching();
      
      menu_position[0] = -1;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_Settings, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      
      break;
    }
    case eMainMenu_Proteolytic: {
      MainMenu_Proteolytic();
      
      menu_position[0] = -1;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_Proteolytic, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      
      break;
    }
    case eMainMenu_BetaAmylase: {
      MainMenu_BetaAmylase();
      
      menu_position[0] = -1;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_BetaAmylase, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      
      break;
    }
    case eMainMenu_AlphaAmylase: {
      MainMenu_AlphaAmylase();
      
      menu_position[0] = -1;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_AlphaAmylase, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      
      break;
    }
    case eMainMenu_Mashout: {
      MainMenu_Mashout();
      
      menu_position[0] = -1;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_Mashout, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      
      break;
    }
    case eMainMenu_Recirculation: {
      MainMenu_Recirculation();
      
      menu_position[0] = -1;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_Recirculation, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      
      break;
    }
    case eMainMenu_Sparge: {
      MainMenu_Sparge();
      
      menu_position[0] = -1;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_Sparge, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      
      break;
    }
    case eMainMenu_Boil: {
      MainMenu_Boil();
      
      menu_position[0] = -1;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_Boil, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      
      break;
    }
    case eMainMenu_Hops: {
      MainMenu_Hops();
      
      menu_position[0] = -1;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_Hops, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      
      break;
    }
    case eMainMenu_Cooling: {
      MainMenu_Cooling();
      
      menu_position[0] = -1;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_Cooling, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      
      break;
    }
    case eMainMenu_Purge: {
      MainMenu_Purge();

      menu_position[0] = -1;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_Purge, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      
      break;
    }
    case eMainMenu_Settings: {
      MainMenu_Settings();
      
      menu_position[0] = -1;
      
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_Settings, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
      
      break;
    }
    case eMainMenu_Back: {
      MainMenu_Back();
        
      menu_position[0] = -1;
        
      // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
      xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_Back, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );

      break;
    }
    default: {
      if(repaint) {

        // display menu
        lcd.clear();
        lcd.home (); // go home
        lcd.print("Brewery Menu");
        
        switch(rotaryEncoderVirtualPosition) {
          case eMainMenu_GO: {
            lcd.setCursor (0,1);        // go to start of 2nd line
            lcd.print("-> GO           ");
            break;
          }
          case eMainMenu_STOP: {
            lcd.setCursor (0,1);        // go to start of 2nd line
            lcd.print("-> STOP         ");
            break;
          }
          case eMainMenu_Presets: {
            lcd.setCursor (0,1);        // go to start of 2nd line
            lcd.print("-> Presets      ");
            break;
          }
          case eMainMenu_Malt: {
            lcd.setCursor (0,1);        // go to start of 2nd line
            lcd.print("-> Malt         ");
            break;
          }
          case eMainMenu_Startpoint: {
            lcd.setCursor (0,1);        // go to start of 2nd line
            lcd.print("-> Startpoint   ");
            break;
          }
          case eMainMenu_BetaGlucanase: {
            lcd.setCursor (0,1);        // go to start of 2nd line
            lcd.print("-> BetaGlucanase");
            break;
          }
          case eMainMenu_Debranching: {
            lcd.setCursor (0,1);        // go to start of 2nd line
            lcd.print("-> Debranching  ");
            break;
          }
          case eMainMenu_Proteolytic: {
            lcd.setCursor (0,1);        // go to start of 2nd line
            lcd.print("-> Proteolytic  ");
            break;
          }
          case eMainMenu_BetaAmylase: {
            lcd.setCursor (0,1);        // go to start of 2nd line
            lcd.print("-> Beta Amylase ");
            break;
          }
          case eMainMenu_AlphaAmylase: {
            lcd.setCursor (0,1);        // go to start of 2nd line
            lcd.print("-> Alpha Amylase");
            break;
          }
          case eMainMenu_Mashout: {
            lcd.setCursor (0,1);        // go to start of 2nd line
            lcd.print("-> Mashout      ");
            break;
          }
          case eMainMenu_Recirculation: {
            lcd.setCursor (0,1);        // go to start of 2nd line
            lcd.print("-> Recirculation");
            break;
          }
          case eMainMenu_Sparge: {
            lcd.setCursor (0,1);        // go to start of 2nd line
            lcd.print("-> Sparge       ");
            break;
          }
          case eMainMenu_Boil: {
            lcd.setCursor (0,1);        // go to start of 2nd line
            lcd.print("-> Boil         ");
            break;
          }
          case eMainMenu_Hops: {
            lcd.setCursor (0,1);        // go to start of 2nd line
            lcd.print("-> Hops         ");
            break;
          }
          case eMainMenu_Cooling: {
            lcd.setCursor (0,1);        // go to start of 2nd line
            lcd.print("-> Cooling      ");
            break;
          }
          case eMainMenu_Purge: {
            lcd.setCursor (0,1);        // go to start of 2nd line
            lcd.print("-> Purge        ");
            break;
          }
          case eMainMenu_Settings: {
            lcd.setCursor (0,1);        // go to start of 2nd line
            lcd.print("-> Settings     ");
            break;
          }
          case eMainMenu_Back: {
            lcd.setCursor (0,1);        // go to start of 2nd line
            lcd.print("-> Back         ");
            break;
          }
          default: {
            // reset operation state | INPUT : eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep
            xSetupRotaryEncoder( eRotaryEncoderMode_Menu, eMainMenu_GO, MENU_SIZE_MAIN_MENU - 1, 0, 1, 0 );
          } 
        }
        
        repaint = false;
      }
      
      if ((digitalRead(ROTARY_ENCODER_SW_PIN))) {    // check if pushbutton is pressed
        menu_position[0] = rotaryEncoderVirtualPosition;
        while (digitalRead(ROTARY_ENCODER_SW_PIN)) {}    // wait til switch is released
        delay(10);                            // debounce
        break;
      }
    } 
  }
}

void MainMenu_GO() {
  startBrewing();

  xTransitionIntoStage_GlobalVariables( eCookingStage_Startpoint );

  backToStatus();
}

void MainMenu_STOP() {
  stopBrewing();

  backToStatus();
}

void MainMenu_Presets() {

  backToStatus();
  
}

void MainMenu_Malt() {

  backToStatus();
  
}

void MainMenu_Startpoint() {
  startpointTime = getTimer( startpointTime );
  
  startpointTemperature = xSetGenericValue( startpointTemperature, 0, 120, "temperature", "*C" );

  backToStatus();
}

void MainMenu_BetaGlucanase() {
  betaGlucanaseTime = getTimer( betaGlucanaseTime );
  
  betaGlucanaseTemperature = xSetGenericValue( betaGlucanaseTemperature, 0, 120, "temperature", "*C" );

  backToStatus();
}

void MainMenu_Debranching() {
  debranchingTime = getTimer( debranchingTime );
  
  debranchingTemperature = xSetGenericValue( debranchingTemperature, 0, 120, "temperature", "*C" );

  backToStatus();
}

void MainMenu_Proteolytic() {
  proteolyticTime = getTimer( proteolyticTime );
  
  proteolyticTemperature = xSetGenericValue( proteolyticTemperature, 0, 120, "temperature", "*C" );

  backToStatus();
}

void MainMenu_BetaAmylase() {
  betaAmylaseTime = getTimer( betaAmylaseTime );
  
  betaAmylaseTemperature = xSetGenericValue( betaAmylaseTemperature, 0, 120, "temperature", "*C" );

  backToStatus();
}

void MainMenu_AlphaAmylase() {
  alphaAmylaseTime = getTimer( alphaAmylaseTime );
  
  alphaAmylaseTemperature = xSetGenericValue( alphaAmylaseTemperature, 0, 120, "temperature", "*C" );

  backToStatus();
}

void MainMenu_Mashout() {
  mashoutTime = getTimer( mashoutTime );
  
  mashoutTemperature = xSetGenericValue( mashoutTemperature, 0, 120, "temperature", "*C" );

  backToStatus();
}

void MainMenu_Recirculation() {
  recirculationTime = getTimer( recirculationTime );
  
  recirculationTemperature = xSetGenericValue( recirculationTemperature, 0, 120, "temperature", "*C" );

  backToStatus();
}

void MainMenu_Sparge() {
  spargeTime = getTimer( spargeTime );
  
  spargeTemperature = xSetGenericValue( spargeTemperature, 0, 120, "temperature", "*C" );

  backToStatus();
}

void MainMenu_Boil() {
  boilTime = getTimer( boilTime );
  
  boilTemperature = xSetGenericValue( boilTemperature, 0, 120, "temperature", "*C" );

  backToStatus();
}

void MainMenu_Hops() {

  backToStatus();
  
}

void MainMenu_Cooling() {
  coolingTime = getTimer( coolingTime );
  
  coolingTemperature = xSetGenericValue( coolingTemperature, 0, 120, "temperature", "*C" );

  backToStatus();
}

void MainMenu_Purge() {
  // Stop anything that might be still going on
  xSafeHardwarePowerOff();

  // Start at the Purge stage
  startBrewing();
  xTransitionIntoStage_GlobalVariables( eCookingStage_Purge );

  backToStatus();
}

void MainMenu_Settings() {
  iPumpSpeed = xSetGenericValue( iPumpSpeed, 0, 255, "Pump Speed", "PWM" );

  backToStatus();
}

void MainMenu_Back() {
  backToStatus();
}

void xCountTheTime( int temperatureRange ) {
  unsigned long now = millis();

  // Check if the machine is in the right temperature range, for the current mode,
  if(!(basePT100.getCurrentTemperature() > (cookTemperature - temperatureRange) && basePT100.getCurrentTemperature() < (cookTemperature + temperatureRange))) {
    clockIgnore += now - clockStartTime - clockIgnore;
  }
  
  // Calculate the remaining time on the clock
  clockCounter = cookTime - (now - clockStartTime - clockIgnore);

#ifdef DEBUG_OFF
  debugPrintFunction("xCountTheTime");
  debugPrintVar("millis()", now);
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

double ulWattToWindowTime( double ulAppliedWatts ) {
  double ulPulsesRequired = ulAppliedWatts / dWattPerPulse;
  return (double)iWindowSize / 1000.0 * ulPulsesRequired;
}

bool xRegulateTemperature() {
  double difference = basePT100.getCurrentTemperature() - cookTemperature;
  bool overTemperature = false;
  double wattage = 0.0;
  
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
    if(difference <= 1) {
      // turn it off
      wattage = 0.0;
    } else {
      if(difference <= 3) {
        // pulse lightly at 500 watt
        wattage = 500.0;
      } else {
        if(difference <= 6) {
          // pulse moderately at 1000 watt
          wattage = 1000.0;
        } else {
          if(difference <= 9) {
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
  if( ulWattToWindowTime( wattage ) > (millis() - windowStartTime)) {
    digitalWrite(HEATING_ELEMENT_OUTPUT_PIN,HIGH);
  } else {
    digitalWrite(HEATING_ELEMENT_OUTPUT_PIN,LOW);
  }
}

bool xRegulatePumpSpeed() {
  analogWrite(PUMP_PIN, iPumpSpeed);  // analogWrite values from 0 to 255

  if( iPumpSpeed ) {
    //basePT100.setPower( PT100_BASE_DARLINGTON_ADC_VMAX, PT100_BASE_DARLINGTON_VS );
    basePT100.setMeasuredTemperatureDeviation( -1.00 );
    upPT100.setMeasuredTemperatureDeviation( -1.00 );
    downPT100.setMeasuredTemperatureDeviation( -1.00 );
  }
  else {
    //basePT100.setPower( PT100_BASE_DEFAULT_ADC_VMAX, PT100_BASE_DEFAULT_VS );
    basePT100.setMeasuredTemperatureDeviation( 0.0 );
    upPT100.setMeasuredTemperatureDeviation( 0.0 );
    downPT100.setMeasuredTemperatureDeviation( 0.0 );
  }
}

void xWarnClockEnded() {
  sing(MELODY_SUPER_MARIO_START, PIEZO_PIN);
}

void xStageFirstRun( int stageTime, int stageTemperature, int stagePumpSpeed ) {
  // Set the clock
  cookTime = stageTime;
  
  // Set the target temperature
  cookTemperature = stageTemperature;
  
  // Reset the clock
  clockStartTime = millis();
  clockIgnore = 0;

  // Set the pump speed
  iPumpSpeed = stagePumpSpeed;
}

void xTransitionIntoStage_GlobalVariables(eCookingStages nextStage) {
  // Reset global stage variables
  bStageFirstRun = true;
  cookingStage = nextStage;
}

void xTransitionIntoStage(eCookingStages nextStage) {
  
  // Turn off all hardware that can damage itself if the machine is not cooking
  xSafeHardwarePowerOff();    
  
  // Warn the user a stage has ended
  xWarnClockEnded();
  
  // Reset global stage variables
  xTransitionIntoStage_GlobalVariables( nextStage );
}

void xBasicStageOperation( int iStageTime, int iStageTemperature, int iStageTemperatureRange, eCookingStages nextStage ) {
  if(bStageFirstRun) {
    // Don't run this again
    bStageFirstRun = false;
    
    // When the stage should be skipped
    if( iStageTime == 0) {
      // Continue to the next stage
      xTransitionIntoStage_GlobalVariables( nextStage );
      
      // There is nothing to do, in this stage
      return;
    } else {
      // Set the clock, target temperature and Reset the clock
      //xStageFirstRun( iStageTime, iStageTemperature, PUMP_SPEED_SLOW );
      xStageFirstRun( iStageTime, iStageTemperature, 255 );
    }
  } else {
    // Account for time spent at the target temperature | Input 1: range in ºC within which the target temperature is considered to be reached
    xCountTheTime( iStageTemperatureRange );
    
    if( isTimeLeft() ) {
      // Do temperature control
      xRegulateTemperature();

      // Do flow control
      xRegulatePumpSpeed();
      
    } else {
      // Continue to the next stage
      xTransitionIntoStage( nextStage );
      
      // There is nothing to do, in this stage
      return;
    }
  }
  // There is nothing to do, in this iteration
  return;
}

void xWarnCookEnded() {
  sing(MELODY_UNDERWORLD, PIEZO_PIN);
}

void operateMachine() {

  // Measure temperature, for effect
  //analogWrite(6, 0);
  //delay(1);
  basePT100.measure();
  upPT100.measure();
  downPT100.measure();
  //analogWrite(6, iPumpSpeed);

  // If cooking is done, return (this is a nice place to double check safety and ensure the cooking parts aren't on.
  if(!cooking) {
    xSafeHardwarePowerOff();

    return;
  }
  
  // Operate the machine according to the current mode
  switch(cookingStage) {
    case eCookingStage_Startpoint: {
      // A basic operation for a basic stage
      xBasicStageOperation( startpointTime, startpointTemperature, 1, eCookingStage_BetaGlucanase );
      
      break;
    }
    case eCookingStage_BetaGlucanase: {
      // A basic operation for a basic stage
      xBasicStageOperation( betaGlucanaseTime, betaGlucanaseTemperature, 1, eCookingStage_Debranching );
      
      break;
    }
    case eCookingStage_Debranching: {
      // A basic operation for a basic stage
      xBasicStageOperation( debranchingTime, debranchingTemperature, 1, eCookingStage_Proteolytic );
      
      break;
    }
    case eCookingStage_Proteolytic: {
      // A basic operation for a basic stage
      xBasicStageOperation( proteolyticTime, proteolyticTemperature, 1, eCookingStage_BetaAmylase );
      
      break;
    }
    case eCookingStage_BetaAmylase: {
      // A basic operation for a basic stage
      xBasicStageOperation( betaAmylaseTime, betaAmylaseTemperature, 1, eCookingStage_AlphaAmylase );
      
      break;
    }
    case eCookingStage_AlphaAmylase: {
      // A basic operation for a basic stage
      xBasicStageOperation( alphaAmylaseTime, alphaAmylaseTemperature, 1, eCookingStage_Mashout );
      
      break;
    }
    case eCookingStage_Mashout: {
      // A basic operation for a basic stage
      xBasicStageOperation( mashoutTime, mashoutTemperature, 1, eCookingStage_Recirculation );
      
      break;
    }
    case eCookingStage_Recirculation: {
      // A basic operation for a basic stage
      xBasicStageOperation( recirculationTime, recirculationTemperature, 1, eCookingStage_Sparge );
      
      break;
    }
    case eCookingStage_Sparge: {
      // A basic operation for a basic stage
      xBasicStageOperation( spargeTime, spargeTemperature, 1, eCookingStage_Boil );
      
      break;
    }
    case eCookingStage_Boil: {
      // A basic operation for a basic stage
      xBasicStageOperation( boilTime, boilTemperature, 1, eCookingStage_Cooling );
      
      break;
    }
    case eCookingStage_Cooling: {
      // A basic operation for a basic stage
      xBasicStageOperation( coolingTime, coolingTemperature, 1, eCookingStage_Done );

      break;
    }
    case eCookingStage_Purge: {
      // A basic operation for a basic stage
      //xBasicStageOperation( coolingTime, coolingTemperature, 1, eCookingStage_Done );
      iPumpSpeed = PUMP_SPEED_MAX;

      xRegulatePumpSpeed();

      break;
    }
    default : {
      // Update state
      cooking = false;
      
      // Warn the user that the cooking is done
      xWarnCookEnded();
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
      operateMachine();
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
      operateMachine();
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
      operateMachine();
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

void lcdPrint(String title, String message) {
  int messageLength = message.length();
  
  lcd.clear();
    
  // print title
  lcd.home();
  lcd.print(title);
    
  // print message
  if(messageLength <= LCD_HORIZONTAL_RESOLUTION) {
    lcd.setCursor(0,LCD_VERTICAL_RESOLUTION-1);
    lcd.print(message);
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
      lcd.setCursor(0,LCD_VERTICAL_RESOLUTION-1);
      lcd.print(output_message.substring(cursor, cursor+16));
      delay(500);
    }
  }
}


