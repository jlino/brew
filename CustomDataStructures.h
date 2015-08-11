/*
  CustomDataStructures.h - Data structures helper for the brew project.
  Created by João Lino, August 11, 2015.
  Released into the public domain.
*/
#ifndef CustomDataStructures_h

enum eRotaryEncoderMode {
	eRotaryEncoderMode_Menu,
	eRotaryEncoderMode_Time,
	eRotaryEncoderMode_Generic,
	eRotaryEncoderMode_Disabled
};

// menu
enum eMainMenuOptions {
	eMainMenu_GO,
	eMainMenu_Presets,
	eMainMenu_Malt,
	eMainMenu_Startpoint,
	eMainMenu_BetaGlucanase,
	eMainMenu_Debranching,
	eMainMenu_Proteolytic,
	eMainMenu_BetaAmylase,
	eMainMenu_AlphaAmylase,
	eMainMenu_Mashout,
	eMainMenu_Recirculation,
	eMainMenu_Sparge,
	eMainMenu_Boil,
	eMainMenu_Hops,
	eMainMenu_Cooling,
	eMainMenu_Settings,
	eMainMenu_Back
};

enum ePresetsMenuOptions {
	ePresetsMenu_Trigo,
	ePresetsMenu_IPA,
	ePresetsMenu_Belga,
	ePresetsMenu_Red,
	ePresetsMenu_APA,
	ePresetsMenu_Back
};

enum eMaltMenuOptions {
	eMaltMenu_CastleMalting_Chteau_Pilsen_2RS,
	eMaltMenu_CastleMalting_Wheat_Blanc,
	eMaltMenu_Back
};

enum eSettingsMenuOptions {
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
	eCookingStage_Done
};

#endif