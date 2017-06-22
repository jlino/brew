/*
  debug.h - Debug functions.
  Created by João Lino, September 25, 2015.
  Released into the public domain.
*/

#ifndef CONFIG_h
#define CONFIG_h

// ######################### SETTINGS #########################
#define SETTING_CLEANING_TIME					      1200 //1200
#define SETTING_CLEANING_TEMPERATURE			  45 //45
#define SETTING_MACHINE_YIELD_CAPACITY_MIN  0
#define SETTING_MACHINE_YIELD_CAPACITY_MAX  50

// ++++++++++++++++++++++++ Heating Element Relay ++++++++++++++++++++++++
#define HEATING_ELEMENT_DEFAULT_WINDOW_SIZE       1000
#define HEATING_ELEMENT_OUTPUT_PIN                24
#define HEATING_ELEMENT_MAX_HEAT_PWM_INTEGER      5
#define HEATING_ELEMENT_MAX_HEAT_PWM_FLOAT        5.0
#define HEATING_ELEMENT_MAX_WATTAGE               3000.0          // Minimum = 2000.0
#define HEATING_ELEMENT_AC_FREQUENCY_HZ           50.0

// ++++++++++++++++++++++++ Temperature ++++++++++++++++++++++++
#define TEMPERATURE_MIN_VALUE                     0
#define TEMPERATURE_MAX_VALUE                     120
#define PT100_BASE_INPUT_PIN                      A4
#define PT100_BASE_OUTPUT_PIN                     32
//#define PT100_BASE_INPUT_R_PIN                    A7
//#define PT100_BASE_OUTPUT_R_PIN                   7
#define PT100_BASE_TIME_BETWEEN_READINGS          1
#define PT100_UP_INPUT_PIN                        A5
#define PT100_UP_OUTPUT_PIN                       30
//#define PT100_UP_INPUT_R_PIN                      A8
//#define PT100_UP_OUTPUT_R_PIN                     9
#define PT100_UP_TIME_BETWEEN_READINGS            1
#define PT100_DOWN_INPUT_PIN                      A6
#define PT100_DOWN_OUTPUT_PIN                     31
//#define PT100_DOWN_INPUT_R_PIN                    A9
//#define PT100_DOWN_OUTPUT_R_PIN                   8
#define PT100_DOWN_TIME_BETWEEN_READINGS          1
/*
#define PT100_BASE_DEFAULT_ADC_VMAX               1.1
#define PT100_BASE_DEFAULT_VS                     5.0
#define PT100_BASE_DEFAULT_R1_RESISTENCE          605.2
#define PT100_BASE_DEFAULT_R2_RESISTENCE          605.2
#define PT100_UP_DEFAULT_ADC_VMAX                 1.1
#define PT100_UP_DEFAULT_VS                       5.0
#define PT100_UP_DEFAULT_R1_RESISTENCE            630.0 //620.0
#define PT100_UP_DEFAULT_R2_RESISTENCE            610.0
#define PT100_DOWN_DEFAULT_ADC_VMAX               1.1
#define PT100_DOWN_DEFAULT_VS                     5.0
#define PT100_DOWN_DEFAULT_R1_RESISTENCE          616.0
#define PT100_DOWN_DEFAULT_R2_RESISTENCE          611.0
*/

// ++++++++++++++++++++++++ Mixer ++++++++++++++++++++++++
//#define MIXER_PIN     12
//#define MIXER_MAX_POSITION   255

// ++++++++++++++++++++++++ Pump ++++++++++++++++++++++++
#define PIEZO_PIN                                 25

// ++++++++++++++++++++++++ Pump ++++++++++++++++++++++++
#define PUMP_PIN                                  6
#define PUMP_TEMPERATURE_MAX_OPERATION            90
#define PUMP_SPEED_STOP                           0
#define PUMP_SPEED_STOP_MOSFET                    255
#define PUMP_SPEED_SLOW                           64
#define PUMP_SPEED_AVERAGE                        128
#define PUMP_SPEED_FAST                           192
#define PUMP_SPEED_MAX_MOSFET                     0
#define PUMP_SPEED_MAX                            255

// ++++++++++++++++++++++++ Rotary Encoder ++++++++++++++++++++++++
#define ROTARY_ENCODER_INTERRUPT_NUMBER           1    // On Mega2560 boards, interrupt 1 is on pin 3
#define ROTARY_ENCODER_CLK_PIN                    3    // Used for generating interrupts using CLK signal
#define ROTARY_ENCODER_DT_PIN                     22    // Used for reading DT signal
#define ROTARY_ENCODER_SW_PIN                     23    // Used for the push button switch
#define ROTARY_ENCODER_DEBOUNCE_TIME              50 //20    // Number of miliseconds to ignore new signals a signal is received

// ++++++++++++++++++++++++ State Machine ++++++++++++++++++++++++
#define SETTING_WELCOME_TIMEOUT                   100
#define SETTING_MAX_INACTIVITY_TIME               3000
#define MENU_SIZE_MAIN_MENU                       13
#define MENU_SIZE_PROFILES_MENU                   9
#define MENU_SIZE_STAGE_MENU                      13
#define MENU_SIZE_MALT_MENU                    	  4
#define MENU_SIZE_SETTINGS_MENU                   6

// ++++++++++++++++++++++++ Serial Monotor ++++++++++++++++++++++++
#define SETTING_SERIAL_MONITOR_BAUD_RATE          9600
#define SETTING_SERIAL_MONITOR_WELCOME_MESSAGE    "Let's start Brewing!"

// ######################### LIBRARIES #########################
// ++++++++++++++++++++++++ LiquidCrystal_I2C ++++++++++++++++++++++++
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

#endif
