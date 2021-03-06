#ifndef __BREW
#define __BREW


// ++++++++++++++++++++++++ LiquidCrystal_I2C ++++++++++++++++++++++++
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

// ++++++++++++++++++++++++ PT100 +++++++++++++++++++++++++++++++++
//#include <PT100.h>

// ++++++++++++++++++++++++ Defines +++++++++++++++++++++++++++++++++
#define NELEMS(x)               (sizeof(x) / sizeof((x)[0]))

// ++++++++++++++++++++++++ OTHER +++++++++++++++++++++++++++++++++
#include "CustomDataStructures.h"
#include "debug.h"

// ++++++++++++++++++++++++ FUNCTIONS (used in configuration) +++++++++++++++++++++++++++++++++
void runMainMenuSelection();
void runStartFromStageSelection();
void runBeerProfileSelection();
void runStageSelection();
void runMaltSelection();
void runSettingsSelection();

#include "config.h"

#include "Melody.h"
#include "Display.h"
#include "Temperature.h"
#include "Profiles.h"

// ++++++++++++++++++++++++ FUNCTIONS +++++++++++++++++++++++++++++++++

void xSetupRotaryEncoder( eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep );

// ######################### START #########################

void xSafeHardwarePowerOff();

void xWelcomeUser();

// ######################### FUNCTIONS ########################

void runMenu();

void runMenuProcessor( MenuData *data );

void runStageSelection_Generic( unsigned long * selectedStageTime, int *selectedStageTemperature);

void xStartStage( unsigned long *stageTime, int *stageTemperature, eCookingStages nextStage, bool bPurgePump, bool bSetFinalYield, bool bSetTime, bool bSetTemperature );

void xStartStageHeadless( eCookingStages nextStage, bool bPurgePump );

void xStartStageInteractive( unsigned long *stageTime, int *stageTemperature, eCookingStages nextStage );

void xCountTheTime( float temperatureRange, boolean bAverageUpDown );

bool isTimeLeft();

double ulWattToWindowTime( double ulAppliedWatts );

bool xRegulateTemperature( boolean bAverageUpDown );

void xPurgePump();

bool xRegulatePumpSpeed();

void xWarnClockEnded();

void xWarnCookEnded();

void xPrepareForStage( int stageTime, int stageTemperature, int stagePumpSpeed, eCookingStages stage );

void xSetupStage(eCookingStages nextStage);

void xTransitionIntoStage(eCookingStages nextStage);

void xBasicStageOperation( int iStageTime, int iStageTemperature, float iStageTemperatureRange, eCookingStages nextStage, boolean bAverageUpDown );

void xManageMachineSystems();

// #################################################### Helpers ##################################################################

void startBrewing();

void stopBrewing();

void resetMenu( boolean requestRepaintPaint );

void backToStatus();

// #################################################### Set Variables ##################################################################

boolean checkForEncoderSwitchPush( bool cancelable );

int getTimer( int initialValue );
int getTimer( int initialValue, int defaultValue );

int getTemperature( int initialValue );
int getTemperature( int initialValue, int defaultValue );

int getFinalYield( int initialValue );
int getFinalYield( int initialValue, int defaultValue );

int xSetGenericValue( int initialValue, int defaultValue, int minimumValue, int maximumValue, const char *valueName, const char *unit );

unsigned long getInactivityTime();

// ###################### Set Variables ##################################################

void xWaitForAction(String title, String message);

void printBeerProfile( void );
void printTime( unsigned long timeToPrint );
void printTemperature( int temparatureToPrint );

#endif //__BREW
