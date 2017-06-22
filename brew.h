#ifndef __BREW
#define __BREW

void xSetupRotaryEncoder( eRotaryEncoderMode newMode, int newPosition, int newMaxPosition, int newMinPosition, int newSingleStep, int newMultiStep );

// ######################### START #########################

void xSafeHardwarePowerOff();

void xWelcomeUser();

// ######################### FUNCTIONS ########################

void runMenu();

void runSettingsSelection();

void runMaltSelection();

void runStageSelection();

void runBeerProfileSelection();

void xStartStage( unsigned long *stageTime, int *stageTemperature, eCookingStages nextStage, bool bPurgePump, bool bSetFinalYield, bool bSetTime, bool bSetTemperature );

void xStartStageHeadless( eCookingStages nextStage, bool bPurgePump );

void xStartStageInteractive( unsigned long *stageTime, int *stageTemperature, eCookingStages nextStage );

void runStartFromStageSelection();

void runMainMenuSelection();

void xCountTheTime( int temperatureRange, boolean bAverageUpDown );

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

void xBasicStageOperation( int iStageTime, int iStageTemperature, int iStageTemperatureRange, eCookingStages nextStage, boolean bAverageUpDown );

void xManageMachineSystems();

// #################################################### Helpers ##################################################################

void startBrewing();

void stopBrewing();

void resetMenu( boolean requestRepaintPaint );

void backToStatus();

// #################################################### Set Variables ##################################################################

int getTimer( int initialValue, int defaultValue );

int getTimer( int initialValue );

int getTemperature(int initialValue);

int xSetGenericValue(int initialValue, int minimumValue, int maximumValue, const char *valueName, const char *unit);

int xSetTemperature( int initialValue );

int xSetFinalYield( int initialValue );

unsigned long getInactivityTime();

// ###################### Set Variables ##################################################

void xWaitForAction(String title, String message);

boolean gotButtonPress(int iPin);

#endif //__BREW
