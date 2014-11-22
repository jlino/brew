// ######################### DECLARE #########################
// ++++++++++++++++++++++++ ERROR ++++++++++++++++++++++++
//#include "error.h"
// ++++++++++++++++++++++++ ERROR ++++++++++++++++++++++++
// ++++++++++++++++++++++++ Library - DHT ++++++++++++++++++++++++
//#include "DHT.h"      //DHT humidity/temperature sensors
//#define DHTPIN 2        // What pin we're connected to
//#define DHTTYPE DHT11   // DHT 11 
#define ponto1 A4//
//#define ponto2 A5//
//#define R1 99.45
//#define R2 100.2
//#define R3 100.05
//#define calibragem 12.709066573

float amostra1;//
float temperatura, temperatura1;//
float tabela[] = {100.00, 100.39, 100.78, 101.17, 101.56, 101.95, 102.34, 102.73, 103.12, 103.51, 103.90, 104.29, 104.68, 105.07, 105.46, 105.85, 106.24, 106.63, 107.02, 107.40, 107.79, 108.18, 108.57, 108.96, 109.35, 109.73, 110.12, 110.51, 110.90, 111.29, 111.67, 112.06, 112.45, 112.83, 113.22, 113.61, 114.00, 114.38, 114.77, 115.15, 115.54, 115.93, 116.31, 116.70, 117.08, 117.47, 117.86, 118.24, 118.63, 119.01, 119.40, 119.78, 120.17, 120.55, 120.94, 121.32, 121.71, 122.09, 122.47, 122.86, 123.24, 123.63, 124.01, 124.39, 124.78, 125.16, 125.54, 125.93, 126.31, 126.69, 127.08, 127.46, 127.84, 128.22, 128.61, 128.99, 129.37, 129.75, 130.13, 130.52, 130.90, 131.28, 131.66, 132.04, 132.42, 132.80, 133.18, 133.57, 133.95, 134.33, 134.71, 135.09, 135.47, 135.85, 136.23, 136.61, 136.99, 137.37, 137.75, 138.13, 138.51, 138.88, 139.26, 139.64, 140.02, 140.40, 140.78, 141.16, 141.54, 141.91, 142.29, 142.67, 143.05, 143.43, 143.80, 144.18, 144.56, 144.94, 145.31, 145.69, 146.07, 146.44, 146.82, 147.20, 147.57, 147.95, 148.33, 148.70, 149.08, 149.46, 149.83, 150.21, 150.58, 150.96, 151.33, 151.71, 152.08, 152.46, 152.83, 153.21, 153.58, 153.96, 154.33, 154.71, 155.08, 155.46, 155.83, 156.20, 156.58, 156.95, 157.33, 157.70, 158.07, 158.45, 158.82, 159.19, 159.56, 159.94, 160.31, 160.68, 150.00, 160.00, 161.05, 161.43, 161.80, 162.17, 162.54, 162.91, 163.29, 163.66, 164.03, 164.40, 164.77, 165.14, 165.51, 165.89, 166.26, 166.63, 167.00, 167.37, 167.74, 168.11, 168.48, 168.85, 169.22, 169.59, 169.96, 170.33, 170.70, 171.07, 171.43, 171.80, 172.17, 172.54, 172.91, 173.28, 173.65, 174.02, 174.38, 174.75, 175.12, 175.49, 175.86, 176.22, 176.59, 176.96, 177.33, 177.69, 178.06, 178.43, 178.79, 179.16};
//float amostras[] = {630.00,630.00,630.00}; //,630.00,630.00,630.00,630.00,630.00,630.00,630.00,630.00,630.00,630.00,630.00,630.00,630.00,630.00,630.00,630.00,630.00};

float Va;
float Rx;

/*
float amostra2;//
float diferenca;

float tensaoPorAmostra;
float Vg;
float Vs;
float a;
float Rx;
*/
// ++++++++++++++++++++++++ Library - DHT ++++++++++++++++++++++++
// ++++++++++++++++++++++++ Library - LiquidCrystal_I2C ++++++++++++++++++++++++
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#define LCD_I2C_ADDR    0x27 // <<----- Add your address here.  Find it from I2C Scanner
#define LCD_X   16
#define LCD_Y   2
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
// ++++++++++++++++++++++++ Library - LiquidCrystal_I2C ++++++++++++++++++++++++
// ++++++++++++++++++++++++ Heating Element Relay ++++++++++++++++++++++++
#include <PID_v1.h>
#define HEATING_ELEMENT  24
// ++++++++++++++++++++++++ Heating Element Relay ++++++++++++++++++++++++
// ++++++++++++++++++++++++ Temperature ++++++++++++++++++++++++
#define TEMPERATURE_MAX_POSITION 120
// ++++++++++++++++++++++++ Temperature ++++++++++++++++++++++++
// ++++++++++++++++++++++++ Mixer ++++++++++++++++++++++++
#define MIXER_PIN 12
#define MIXER_MAX_POSITION 255
const int mixerPin = 12;
const int mixerMaxSpeed = 255;
// ++++++++++++++++++++++++ Mixer ++++++++++++++++++++++++
// ++++++++++++++++++++++++ State Machine ++++++++++++++++++++++++
#define SETTING_WELCOME_TIMEOUT  2000
#define MENU_MAX_DEPTH 10
#define MENU_INIT_VALUES -1,-1,-1,-1,-1,-1,-1,-1,-1,-1
// ++++++++++++++++++++++++ State Machine ++++++++++++++++++++++++
// ######################### DECLARE #########################


// ######################### INITIALIZE #########################
// ++++++++++++++++++++++++ Library - DHT ++++++++++++++++++++++++
//DHT dht(DHTPIN, DHTTYPE);
// ++++++++++++++++++++++++ Library - DHT ++++++++++++++++++++++++
// ++++++++++++++++++++++++ Library - LiquidCrystal_I2C ++++++++++++++++++++++++
int n = 1;
LiquidCrystal_I2C	lcd(LCD_I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);
// ++++++++++++++++++++++++ Library - LiquidCrystal_I2C ++++++++++++++++++++++++
// ++++++++++++++++++++++++ Rotary Encoder ++++++++++++++++++++++++
volatile int  rotaryEncoderVirtualPosition = 0;
volatile int rotaryEncoderMaxPosition;
volatile int menuSize = 2;
const int PinCLK = 3;                   // Used for generating interrupts using CLK signal
const int PinDT = 22;                    // Used for reading DT signal
const int PinSW = 23;                    // Used for the push button switch
// ++++++++++++++++++++++++ Rotary Encoder ++++++++++++++++++++++++
// ++++++++++++++++++++++++ Heating Element Relay ++++++++++++++++++++++++
//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
float Kp = 200;
float Ki = 50;
float Kd = 0;
//PID myPID(&Input, &Output, &Setpoint,Kp,Ki,Kd, DIRECT);
//PID myPID(&Input, &Output, &Setpoint,1000,50,0, DIRECT);
PID myPID(&Input, &Output, &Setpoint,200,50,0, DIRECT);

int WindowSize = 1000;
unsigned long windowStartTime;
// ++++++++++++++++++++++++ Heating Element Relay ++++++++++++++++++++++++
// ++++++++++++++++++++++++ Mixer ++++++++++++++++++++++++
//const int mixerSpeed = 130;
// ++++++++++++++++++++++++ Mixer ++++++++++++++++++++++++
// ++++++++++++++++++++++++ Error ++++++++++++++++++++++++
enum sys_error {
  no_error,
  catastrofic_failure_sensor_temperature,
  catastrofic_failure_program,
  catastrofic_failure_logic_auto_manual
};
sys_error error = no_error;
// ++++++++++++++++++++++++ Error ++++++++++++++++++++++++
// ++++++++++++++++++++++++ State Machine ++++++++++++++++++++++++

// global
enum state_machine {
  state_welcome,
  state_menu,
  state_manual_time,
  state_manual_temperature,
  state_manual_mode,
  state_manual_mixer,
  state_error
};
state_machine state = state_welcome;

// menu
enum main_menu_list {
  main_manual,
  main_auto
};
enum manual_menu_list {
  manual_temperature,
  manual_time,
  manual_mode,
  manual_mixer,
  manual_start,
  manual_back
};
enum auto_menu_list {
  auto_recipe,
  auto_start,
  auto_back
};
int menu_position[MENU_MAX_DEPTH]= {MENU_INIT_VALUES};

// cooking
enum cook_mode_list {
  quick_start,
  start_at_temperature
};
int cookTime = 3600;
int cookTemperature = 36;
cook_mode_list cookMode = quick_start;
int cookMixerSpeed = 130;
// ++++++++++++++++++++++++ State Machine ++++++++++++++++++++++++
// ++++++++++++++++++++++++ Global Variables ++++++++++++++++++++++++
//float currentTemperatureCelsius;
//float targetTemperatureCelsius = -1;
// ++++++++++++++++++++++++ Global Variables ++++++++++++++++++++++++
// ######################### INITIALIZE #########################

// ######################### INTERRUPTS #########################
static unsigned long lastInterruptTime = 0;
void isr ()  {                    // Interrupt service routine is executed when a HIGH to LOW transition is detected on CLK
  
  unsigned long interruptTime = millis();    
  
  // If interrupts come faster than 5ms, assume it's a bounce and ignore
  if ((interruptTime - lastInterruptTime) > 5) {
    switch(state) {
      
      // Input of rotay encoder controling menus
      case state_manual_mode:
      case state_menu: {
        if (!digitalRead(PinDT)) {
            rotaryEncoderVirtualPosition = (rotaryEncoderVirtualPosition + 1);
        }
        else {
            rotaryEncoderVirtualPosition = rotaryEncoderVirtualPosition - 1;
        }
        if (rotaryEncoderVirtualPosition >= menuSize) {
            rotaryEncoderVirtualPosition = 0;
        }
        if (rotaryEncoderVirtualPosition < 0) {
            rotaryEncoderVirtualPosition = menuSize -1;
        }
        
        break;
      }
      
      // Input of rotay encoder controling time variables
      case state_manual_time: {
        if (!digitalRead(PinDT)) {
          if(rotaryEncoderVirtualPosition >= 60) {
            rotaryEncoderVirtualPosition = (rotaryEncoderVirtualPosition + 30);
          }
          else {
            rotaryEncoderVirtualPosition = (rotaryEncoderVirtualPosition + 1);
          }
        }
        else {
          if(rotaryEncoderVirtualPosition == 0) {
            rotaryEncoderVirtualPosition = (rotaryEncoderVirtualPosition + 60);
          }
          else {
            if(rotaryEncoderVirtualPosition >= 90) {
              rotaryEncoderVirtualPosition = (rotaryEncoderVirtualPosition - 30);
            }
            else {
              rotaryEncoderVirtualPosition = rotaryEncoderVirtualPosition - 1;
            }
          }
        }
        if (rotaryEncoderVirtualPosition > 7200) {
            rotaryEncoderVirtualPosition = 7200;
        }
        if (rotaryEncoderVirtualPosition < 0) {
            rotaryEncoderVirtualPosition = 0;
        }
        
        break;
      }
      
      // Input of rotay encoder controling integer variables within a range between 0 and rotaryEncoderMaxPosition
      case state_manual_mixer:
      case state_manual_temperature: {
        if (!digitalRead(PinDT)) {
          if ((interruptTime - lastInterruptTime) < 10) {
            rotaryEncoderVirtualPosition = (rotaryEncoderVirtualPosition + 5);
          }
          else {
            rotaryEncoderVirtualPosition = (rotaryEncoderVirtualPosition + 1);
          }
        }
        else {
          if ((interruptTime - lastInterruptTime) < 10) {
            rotaryEncoderVirtualPosition = (rotaryEncoderVirtualPosition - 5);
          }
          else {
            rotaryEncoderVirtualPosition = (rotaryEncoderVirtualPosition - 1);
          }
        }
        if (rotaryEncoderVirtualPosition > rotaryEncoderMaxPosition) {
            rotaryEncoderVirtualPosition = rotaryEncoderMaxPosition;
        }
        if (rotaryEncoderVirtualPosition < 0) {
            rotaryEncoderVirtualPosition = 0;
        }
       
        break;
      }
      default: {
        
      }
    }
  }
  
  lastInterruptTime = interruptTime;
}
// ######################### INTERRUPTS #########################

// ######################### START #########################
void setup() {
// ++++++++++++++++++++++++ Library - DHT ++++++++++++++++++++++++
  //dht.begin();
// ++++++++++++++++++++++++ Library - DHT ++++++++++++++++++++++++
// ++++++++++++++++++++++++ Library - LiquidCrystal_I2C ++++++++++++++++++++++++
  lcd.begin (LCD_X,LCD_Y); //  <<----- My LCD was 16x2
  // Switch on the backlight
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(HIGH);
// ++++++++++++++++++++++++ Library - LiquidCrystal_I2C ++++++++++++++++++++++++
// ++++++++++++++++++++++++ Rotary Encoder ++++++++++++++++++++++++
  pinMode(PinCLK,INPUT);
  pinMode(PinDT, INPUT);
  pinMode(PinSW, INPUT);
  attachInterrupt(1, isr, FALLING);   // interrupt 0 is always connected to pin 2 on Arduino UNO
// ++++++++++++++++++++++++ Rotary Encoder ++++++++++++++++++++++++
// ++++++++++++++++++++++++ Heating Element Relay ++++++++++++++++++++++++
  pinMode(HEATING_ELEMENT, OUTPUT);
  digitalWrite(HEATING_ELEMENT,LOW);
  
  windowStartTime = millis();

  //initialize the variables we're linked to
  Setpoint = 100;

  //tell the PID to range between 0 and the full window size
  myPID.SetOutputLimits(0, WindowSize);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
// ++++++++++++++++++++++++ Heating Element Relay ++++++++++++++++++++++++
// ++++++++++++++++++++++++ Mixer ++++++++++++++++++++++++
  pinMode(mixerPin, OUTPUT);
  analogWrite(mixerPin, 0);
// ++++++++++++++++++++++++ Mixer ++++++++++++++++++++++++
// ++++++++++++++++++++++++ Temperature Sensor PT100 ++++++++++++++++++++++++
  analogReference(EXTERNAL);
// ++++++++++++++++++++++++ Temperature Sensor PT100 ++++++++++++++++++++++++
  Serial.begin(9600); 
  Serial.println("Let's start Brewing!");
}
// ######################### START #########################

void loop() {
  switch(state) {
    case state_welcome: {
      runWelcome();
      
      break;
    }
    case state_menu: {
      runMenu();
      
      break;
    }
    /*case state_manual_time: {
      //runManualTime();
      
      break;
    }
    case state_manual_temperature: {
      //runManualTemp();
      
      break;
    }
    case state_manual_mixer: {
      //runManualTemp();
      
      break;
    }*/
    case state_error: {
      runError();
      
      break;
    }
    default: {
      lcdPrint("Error", "The brewer has stoped working properly. Please shutdown NOW for your safety!");
      state = state_error;
      error = catastrofic_failure_program;
    }
  }
}

void runWelcome() {
  // Write welcome
  lcd.clear();
  lcd.home (); // go home
  lcd.print("  Let's start");
  lcd.setCursor (0,1);        // go to start of 2nd line
  lcd.print("    Brewing!");
  
  delay(SETTING_WELCOME_TIMEOUT);
  
  state = state_menu;
}

void runMenu() {
  digitalWrite(HEATING_ELEMENT,LOW);
  
  switch(menu_position[0]) {
    case main_manual: {
      switch(menu_position[1]) {
        case manual_temperature: {
          // do work
          cookTemperature = getTemperature(cookTemperature);
          
          menu_position[1] = -1;
          rotaryEncoderVirtualPosition = manual_temperature;
          
          break;
        }
        case manual_time: {
          // do work
          cookTime = getTimer(cookTime);
          
          menu_position[1] = -1;
          rotaryEncoderVirtualPosition = manual_time;
          
          break;
        }
        case manual_mode: {
          // do work
          int returnMode = getMode();
          switch(returnMode) {
            case quick_start: {
              cookMode = quick_start;
              break;
            }
            case start_at_temperature: {
              cookMode = start_at_temperature;
              
              break;
            }
            default: {
              cookMode = quick_start;
            }
          }
          
          menu_position[1] = -1;
          rotaryEncoderVirtualPosition = manual_mode;
          
          break;
        }
        case manual_mixer: {
          // do work
          cookMixerSpeed = getMixerSpeed(cookMixerSpeed);
          
          menu_position[1] = -1;
          rotaryEncoderVirtualPosition = manual_time;
          
          break;
        }
        case manual_start: {
          // do work
          runStart();
          
          menu_position[1] = -1;
          rotaryEncoderVirtualPosition = manual_start;
          
          break;
        }
        case manual_back: {
          menu_position[0] = -1;
          menu_position[1] = -1;
          
          rotaryEncoderVirtualPosition = manual_back;
          
          break;
        }
        default: {
          // reset menu variables
          menuSize = 6;
          
          // display menu
          lcd.clear();
          lcd.home (); // go home
          lcd.print("Manual Menu");
          
          while(true) {
            switch(rotaryEncoderVirtualPosition) {
              case manual_temperature: {
                lcd.setCursor (0,1);        // go to start of 2nd line
                lcd.print("-> Temperature  ");
                delay(100);
                break;
              }
              case manual_time: {
                lcd.setCursor (0,1);        // go to start of 2nd line
                lcd.print("-> Time         ");
                delay(100);
                break;
              }
              case manual_mode: {
                lcd.setCursor (0,1);        // go to start of 2nd line
                lcd.print("-> Mode         ");
                delay(100);
                break;
              }
              case manual_mixer: {
                lcd.setCursor (0,1);        // go to start of 2nd line
                lcd.print("-> Mixer Speed  ");
                delay(100);
                break;
              }
              case manual_start: {
                lcd.setCursor (0,1);        // go to start of 2nd line
                lcd.print("-> Start        ");
                delay(100);
                break;
              }
              case manual_back: {
                lcd.setCursor (0,1);        // go to start of 2nd line
                lcd.print("-> Back         ");
                delay(100);
                break;
              }
              default: {
                //lcd.setCursor (0,1);        // go to start of 2nd line
                //lcd.print("*** Error ***");
                rotaryEncoderVirtualPosition = 0;
              } 
            }
            
            if (!(digitalRead(PinSW))) {        // check if pushbutton is pressed
              menu_position[1] = rotaryEncoderVirtualPosition;
              while (!digitalRead(PinSW)) {}  // wait til switch is released
              delay(10);                      // debounce
              break;
            }
          }
        } 
      }
      break;
    }
    case main_auto: {
      switch(menu_position[1]) {
        case auto_recipe: {
          // do work
          
          menu_position[1] = -1;
          rotaryEncoderVirtualPosition = auto_recipe;
          
          break;
        }
        case auto_start: {
          // do work
          
          menu_position[1] = -1;
          rotaryEncoderVirtualPosition = auto_start;
          
          break;
        }
        case auto_back: {
          // reset choces
          menu_position[0] = -1;
          menu_position[1] = -1;
          
          // set menu position
          rotaryEncoderVirtualPosition = main_auto;
          
          break;
        }
        default: {
          // reset menu variables
          //rotaryEncoderVirtualPosition = 0;
          menuSize = 3;
          
          // display menu
          lcd.clear();
          lcd.home (); // go home
          lcd.print("Automatic Menu");
          
          while(true) {
            switch(rotaryEncoderVirtualPosition) {
              case auto_recipe: {
                lcd.setCursor (0,1);        // go to start of 2nd line
                lcd.print("-> Recipe       ");
                delay(100);
                break;
              }
              case auto_start: {
                lcd.setCursor (0,1);        // go to start of 2nd line
                lcd.print("-> Start        ");
                delay(100);
                break;
              }
              case auto_back: {
                lcd.setCursor (0,1);        // go to start of 2nd line
                lcd.print("-> Back         ");
                delay(100);
                break;
              }
              default: {
                //lcd.setCursor (0,1);        // go to start of 2nd line
                //lcd.print("*** Error ***");
                rotaryEncoderVirtualPosition = 0;
              } 
            }
            
            if (!(digitalRead(PinSW))) {        // check if pushbutton is pressed
              menu_position[1] = rotaryEncoderVirtualPosition;
              while (!digitalRead(PinSW)) {}  // wait til switch is released
              delay(10);                      // debounce
              break;
            }
          }
        } 
      }
      break;
    }
    default: {
      // reset menu variables
      //rotaryEncoderVirtualPosition = 0;
      menuSize = 2;
      
      // display menu
      lcd.clear();
      lcd.home (); // go home
      lcd.print("Main Menu");
      
      while(true) {
        switch(rotaryEncoderVirtualPosition) {
          case main_manual: {
            lcd.setCursor (0,1);        // go to start of 2nd line
            lcd.print("-> Manual       ");
            delay(100);
            break;
          }
          case main_auto: {
            lcd.setCursor (0,1);        // go to start of 2nd line
            lcd.print("-> Automatic    ");
            delay(100);
            break;
          }
          default: {
            //lcd.setCursor (0,1);        // go to start of 2nd line
            //lcd.print("*** Error ***");
            rotaryEncoderVirtualPosition = 0;
          } 
        }
        
        if (!(digitalRead(PinSW))) {        // check if pushbutton is pressed
          menu_position[0] = rotaryEncoderVirtualPosition;
          while (!digitalRead(PinSW)) {}  // wait til switch is released
          delay(10);                      // debounce
          break;
        }
      }
    } 
  }
}

int getTimer(int init) {
  state_machine initialState = state;
  
  // set operation state
  state = state_manual_time;
  rotaryEncoderVirtualPosition = init;

  // initialize variables
  int rotaryEncoderPreviousPosition = 0;
  int minutes = 0;
  int seconds = 0;
  
  // Setup Screen
  lcd.clear();
  lcd.home();        
  lcd.print("Set Time");
  lcd.setCursor (0,LCD_Y-1);
  lcd.print("      0:00");
  
  while(true) {
    // Check for timer set
    if (!(digitalRead(PinSW))) {        // check if pushbutton is pressed
      while (!digitalRead(PinSW)) {}  // wait til switch is released
      delay(10);                      // debounce
      break;
    }
    
    // display current timer
    if (rotaryEncoderVirtualPosition != rotaryEncoderPreviousPosition) {
      rotaryEncoderPreviousPosition = rotaryEncoderVirtualPosition;
      minutes = rotaryEncoderVirtualPosition/60;
      seconds = rotaryEncoderVirtualPosition-minutes*60;
      
      lcd.setCursor (0,LCD_Y-1);
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
  
  state = initialState;
  return rotaryEncoderVirtualPosition;
}

int getTemperature(int init) {
  state_machine initialState = state;
  
  // set operation state
  state = state_manual_temperature;
  rotaryEncoderVirtualPosition = init;  

  // initialize variables
  int rotaryEncoderPreviousPosition = 0;
  
  // Setup Screen
  lcd.clear();
  lcd.home();        
  lcd.print("Set Temperature");
  lcd.setCursor (0,LCD_Y-1);
  lcd.print("       0 *C");
  
  rotaryEncoderMaxPosition = TEMPERATURE_MAX_POSITION;
  
  while(true) {
    // Check for timer set
    if (!(digitalRead(PinSW))) {        // check if pushbutton is pressed
      while (!digitalRead(PinSW)) {}  // wait til switch is released
      delay(10);                      // debounce
      break;
    }
    
    // display current timer
    if (rotaryEncoderVirtualPosition != rotaryEncoderPreviousPosition) {
      rotaryEncoderPreviousPosition = rotaryEncoderVirtualPosition;
      
      lcd.setCursor (0,LCD_Y-1);
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
  
  state = initialState;
  return rotaryEncoderVirtualPosition;
}

int getMode() {
  state_machine initialState = state;
  int initialMenuSize = menuSize;
  
  // set operation state
  state = state_manual_mode;
  menuSize = 2;
  rotaryEncoderVirtualPosition = cookMode;  

  // initialize variables
  int rotaryEncoderPreviousPosition = 0;
  
  // Setup Screen
  lcd.clear();
  lcd.home();        
  lcd.print("Set Mode");
  lcd.setCursor (0,LCD_Y-1);
  lcd.print(" Quick Start");
  
  while(true) {
    // Check for timer set
    if (!(digitalRead(PinSW))) {        // check if pushbutton is pressed
      while (!digitalRead(PinSW)) {}  // wait til switch is released
      delay(10);                      // debounce
      break;
    }
    
    // display current timer
    if (rotaryEncoderVirtualPosition != rotaryEncoderPreviousPosition) {
      rotaryEncoderPreviousPosition = rotaryEncoderVirtualPosition;
      
      lcd.setCursor (0,LCD_Y-1);
      
      switch(rotaryEncoderVirtualPosition) {
        case quick_start: {
          lcd.print("  Quick Start   ");
          
          break;
        }
        case start_at_temperature: {
          lcd.print("Start After Temp");
          
          break;
        }
        default: {
          lcd.print("*** error ***");
        }
      }
      lcd.println("                ");
    }
  }
  
  state = initialState;
  menuSize = initialMenuSize;
  return rotaryEncoderVirtualPosition;
}

int getMixerSpeed(int init) {
  state_machine initialState = state;
  
  // set operation state
  state = state_manual_mixer;
  rotaryEncoderVirtualPosition = init;

  // initialize variables
  int rotaryEncoderPreviousPosition = 0;
  int mixerSpeed = 0;
  
  // Setup Screen
  lcd.clear();
  lcd.home();        
  lcd.print("Set Mixer Speed");
  lcd.setCursor (0,LCD_Y-1);
  lcd.print("       000");
  
  rotaryEncoderMaxPosition = MIXER_MAX_POSITION;
  
  while(true) {
    // Check for timer set
    if (!(digitalRead(PinSW))) {        // check if pushbutton is pressed
      while (!digitalRead(PinSW)) {}  // wait til switch is released
      delay(10);                      // debounce
      break;
    }
    
    // display current timer
    if (rotaryEncoderVirtualPosition != rotaryEncoderPreviousPosition) {
      rotaryEncoderPreviousPosition = rotaryEncoderVirtualPosition;
      
      lcd.setCursor (0,LCD_Y-1);
      lcd.print("      ");
      //lcd.print(minutes);
      //lcd.print(":");
      if(rotaryEncoderVirtualPosition<100) {
        lcd.print("0");
        if(rotaryEncoderVirtualPosition<10) {
          lcd.print("0");
        }
      }
      lcd.print(rotaryEncoderVirtualPosition);
      lcd.println("                ");
    }
  }
  
  state = initialState;
  return rotaryEncoderVirtualPosition;
}

void runStart() {
  
  // Working environment variables
  float runCurrentTemperature;
  long runTargetTemperature;
  unsigned long runStartTime;
  unsigned long runTargetTime;
  cook_mode_list runCurrentMode;
  unsigned long runMixerSpeed;
  
  // Configure environment
  boolean clockStart = false;
  boolean clockEnd = false;
  
  // Iterative work
  while(true) {
    
    // Build Instructions
    switch(menu_position[0]) {
      case main_manual: {
        
        // whenever changes are made to the cook variables, they must be applied to the running variables
        runTargetTemperature = cookTemperature;
        runTargetTime = cookTime;
        runCurrentMode = cookMode;
        runMixerSpeed = cookMixerSpeed;
        
        // check if time ended and is time to stop and return to menu
        if(clockStart && clockEnd) {
          
          // Turn mixer OFF for safety
          analogWrite(mixerPin, 0);
          
          // Turn heading element OFF for safety
          digitalWrite(HEATING_ELEMENT,LOW);
          
          // Pause for efect
          delay(1000);
          
          return;
        }
        
        break;
      }
      case main_auto: {
        // get the next instruction
        
        // if it's time for a new instruction
        
          // if human interaction is required
          
            // display message
            
            // sound buzzer
            
            // check if rotary encoder press to continue
        
          // check what variables are afected
          
          // check what the new values are
          
          // apply the values
        
        // if it's NOT time for a new instruction
           //delay(1000);
        
        break;
      }
      default: {
        lcdPrint("Error", "The system has failed to be set into manual or automatic mode!");
        state = state_error;
        error = catastrofic_failure_logic_auto_manual;
        return;
      } 
    }
    
    // ************ Run Instructions ************
    
    // Setup LCD
    lcd.clear();
    lcd.home();
    lcd.print("Hot: 000*C/000*C");
    //lcd.print("Cool:000*C/000*C");
    lcd.setCursor (0,LCD_Y-1);
    if(runCurrentMode == quick_start) {
      lcd.print("Quick      00:00");
      
      clockStart = true;
      runStartTime = millis();
    }
    else {
      lcd.print("Wait       00:00");
    }
    
    while(true) { 
      amostra1 = analogRead(ponto1);
      float Vs = 4.965; //4.9345;
      float Vin = 8.84; //8.8345;
      float adcStepCount = 1024.0;
      float R1 = 149.3;
      float Rl = 3.37;
      float R_OpTemp = 19.7; //21.597; //24.37;
      
      Va = amostra1 * Vs / adcStepCount;
      //Rx = 151.5 * Va / ( 11.01 - Va ) - 30;
      
      Rx = R1 / ( Vin / Va - 1.0) - Rl - R_OpTemp;
      
      int i;
      float i_f;
      int tabelaSize = sizeof(tabela) / sizeof(float);
      for( i = 0, i_f = 0.0; i <= tabelaSize; i++, i_f++) {
        if((tabela[i]) > Rx) {
          if(i == 0) {
            runCurrentTemperature = -1.0;
          }
          else {
            if(i == tabelaSize) {
              runCurrentTemperature = 210.0;
            }
            else {
              runCurrentTemperature = (i_f - 1.0) + ((Rx - tabela[i-1])/(tabela[i] - tabela[i-1]));
            }
          }
          
          break;
        }
      }
      
      /*
      Serial.print("O valor das entradas sao: [");
      Serial.print(amostra1);
      Serial.print(", ");
      Serial.print(amostra2);
      Serial.print("]");
      Serial.print("diferenca: [");
      Serial.print(diferenca, 6);
      Serial.print("] tensao por amortra: [");
      Serial.print(tensaoPorAmostra, 6);
      Serial.print("] Vg: [");
      Serial.print(Vg, 6);
      Serial.print("] Vs: [");
      Serial.print(Vs);
      Serial.print("] a: [");
      Serial.print(a, 6);
      Serial.print("] Rx: [");
      Serial.print(Rx, 6);
      */
      
      // Operate mixer
      analogWrite(mixerPin, runMixerSpeed);
      
      // Operate the heating element
      Input = runCurrentTemperature;
      Setpoint = runTargetTemperature;
      myPID.Compute();
      
      // Get time and use as "date" of the sensor readings
      unsigned long now = millis();
      
      // Check if it's time to vary the pulse width modulation and if so do it by shifting the "Relay in ON" Window
      if(now - windowStartTime>WindowSize)
      { 
        windowStartTime += WindowSize;
      }
      
      // Check if the "date" is inside the "Relay ON" window and turn it ON; if not, turn it OFF
      if(Output > now - windowStartTime) {
        digitalWrite(HEATING_ELEMENT,HIGH);
       
        if(Output != 0) {
          Serial.print("O valor das entradas sao: [");
          Serial.print(amostra1);
          Serial.print("] Va[");
          Serial.print(Va,6);
          Serial.print("] Rx[");
          Serial.print(Rx,6);
          Serial.print("] Temperature[");
          Serial.print(runCurrentTemperature,6);
          Serial.print("] ");
          
          Serial.print("+++  Diff: ");
          Serial.print(now - windowStartTime);
          Serial.print(" | Output: ");
          Serial.println(Output);
          
        }
      }
      else {
        digitalWrite(HEATING_ELEMENT,LOW);
       
        if(Output != 0) {
          Serial.print("O valor das entradas sao: [");
          Serial.print(amostra1);
          Serial.print("] Va[");
          Serial.print(Va,6);
          Serial.print("] Rx[");
          Serial.print(Rx,6);
          Serial.print("] Temperature[");
          Serial.print(runCurrentTemperature,6);
          Serial.print("] ");
          
          Serial.print("---  Diff: ");
          Serial.print(now - windowStartTime);
          Serial.print(" | Output: ");
          Serial.println(Output);
          
        }
        else {
          Serial.print("O valor das entradas sao: [");
          Serial.print(amostra1);
          Serial.print("] Va[");
          Serial.print(Va,6);
          Serial.print("] Rx[");
          Serial.print(Rx,6);
          Serial.print("] Temperature[");
          Serial.print(runCurrentTemperature,6);
          Serial.println("]"); 
        }
      }
      
      // Print status to LCD
      lcd.setCursor (5,0);
      if (runCurrentTemperature < 10) {
        lcd.print("  ");
      }
      else {
        if (runCurrentTemperature < 100) {
          lcd.print(" ");
        }
      }
      lcd.print(runCurrentTemperature, 1);
      
      lcd.setCursor (11,0);
      if (runTargetTemperature < 10) {
        lcd.print("  ");
      }
      else {
        if (runTargetTemperature < 100) {
          lcd.print(" ");
        }
      }
      lcd.print(runTargetTemperature);
      
      // Check if the target temperature has been reached so the clock can move forward.
      if(runCurrentTemperature == runTargetTemperature && clockStart == false) {
        clockStart = true;
        runStartTime = now;
      }
      
      // If the clock can move forward, calculate it's display time; if not, set it to the total cooking time
      int displayTime = 0;
      if(clockStart) {
        displayTime = runTargetTime - ((now - runStartTime) / 1000);
        
        if(displayTime <= 0) {
          displayTime = 0;
          clockEnd = true;
          
          break;
        }
      }
      else {
        displayTime = runTargetTime;
      }
      
      // Print the clock to the LCD
      int minutes = displayTime/60;
      int seconds = displayTime-minutes*60;
      lcd.setCursor (10,1);
      //lcd.print("      ");
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
  }
}

void runError() {
  if(error != no_error) {
    if (error == catastrofic_failure_sensor_temperature)
    {
      Serial.println("[Error] catastrofic_failure_sensor_temperature!");
      
      lcdPrint("ERROR", "A catastrofic failure was detected on the temperature sensor!");
      /*
      if (!isnan(dht.readTemperature())) {  // Check if any reads failed and exit early (to try again).
        error = no_error;
        return;
      }
      */
    }
  }
  else {
    state = state_welcome;
  }
}

void lcdPrint(String title, String message) {
  int messageLength = message.length();
  
  lcd.clear();
    
  // print title
  lcd.home();
  lcd.print(title);
    
  // print message
  if(messageLength <= LCD_X) {
    lcd.setCursor(0,LCD_Y-1);
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
    for (int cursor = 0; cursor < messageLength - LCD_X; cursor+=2) {
      lcd.setCursor(0,LCD_Y-1);
      lcd.print(output_message.substring(cursor, cursor+16));
      delay(500);
    }
  }
}

