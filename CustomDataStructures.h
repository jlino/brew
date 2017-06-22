/*
  CustomDataStructures.h - Data structures helper for the brew project.
  Created by João Lino, August 11, 2015.
  Released into the public domain.
*/
#ifndef CustomDataStructures_h
#define CustomDataStructures_h

enum eRotaryEncoderMode {
	eRotaryEncoderMode_Menu,
	eRotaryEncoderMode_Time,
	eRotaryEncoderMode_Generic,
	eRotaryEncoderMode_Disabled
};

// menu
enum eMenuType {
	eMenuType_Main,
  eMenuType_StartFromStage,
	eMenuType_BeerProfile,
	eMenuType_Stage,
	eMenuType_Malt,
	eMenuType_Settings
};
enum eMainMenuOptions {
	eMainMenu_NULL,
  eMainMenu_GO_FROM_STAGE,
  eMainMenu_GO,
	eMainMenu_STOP,
	eMainMenu_SKIP,
	eMainMenu_BeerProfile,
	eMainMenu_Stage,
	eMainMenu_Malt,
	eMainMenu_Hops,
	eMainMenu_Clean,
	eMainMenu_Purge,
	eMainMenu_Settings,
	eMainMenu_Back
};

enum eStageMenuOptions {
	eStageMenu_NULL,
	eStageMenu_Startpoint,
	eStageMenu_BetaGlucanase,
	eStageMenu_Debranching,
	eStageMenu_Proteolytic,
	eStageMenu_BetaAmylase,
	eStageMenu_AlphaAmylase,
	eStageMenu_Mashout,
	eStageMenu_Recirculation,
	eStageMenu_Sparge,
	eStageMenu_Boil,
	eStageMenu_Cooling,
	eStageMenu_Back
};

enum eBeerProfileMenuOptions {
  eBeerProfileMenu_NULL,
  eBeerProfileMenu_Basic,
  eBeerProfileMenu_Trigo,
  eBeerProfileMenu_IPA,
  eBeerProfileMenu_Belga,
  eBeerProfileMenu_Red,
  eBeerProfileMenu_APA,
  eBeerProfileMenu_Custom,
  eBeerProfileMenu_Back
};

enum eMaltMenuOptions {
	eMaltMenu_NULL,
	eMaltMenu_CastleMalting_Chteau_Pilsen_2RS,
	eMaltMenu_CastleMalting_Wheat_Blanc,
	eMaltMenu_Back
};

enum eSettingsMenuOptions {
	eSettingsMenu_NULL,
	eSettingsMenu_Pump,
	eSettingsMenu_PT100_Element,
	eSettingsMenu_PT100_Up,
	eSettingsMenu_PT100_Down,
	eSettingsMenu_Back
};

// cooking
enum eCookingStages {
	eCookingStage_Startpoint,
	eCookingStage_BetaGlucanase,
	eCookingStage_Debranching,
	eCookingStage_Proteolytic,
	eCookingStage_BetaAmylase,
	eCookingStage_AlphaAmylase,
	eCookingStage_Mashout,
	eCookingStage_Recirculation,
	eCookingStage_Sparge,
	eCookingStage_Boil,
	eCookingStage_Cooling,
	eCookingStage_Clean,
	eCookingStage_Purge,
	eCookingStage_Done
};

enum eBeerProfile {
	eBeerProfile_Basic,
	eBeerProfile_Trigo,
	eBeerProfile_IPA,
	eBeerProfile_Belga,
	eBeerProfile_Red,
	eBeerProfile_APA,
	eBeerProfile_Custom
};

#endif
