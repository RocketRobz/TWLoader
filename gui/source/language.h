// Language functions.
#ifndef TWLOADER_LANGUAGE_H
#define TWLOADER_LANGUAGE_H

#include <3ds/types.h>

// Active language ID.
extern u8 language;
// System language ID.
extern u8 sys_language;

/**
 * Initialize translations.
 * Uses the language ID specified in settings.ui.language.
 *
 * Check the language variable outside of settings to determine
 * the actual language in use.
 */
void langInit(void);

/**
 * Clear the translations cache.
 */
void langClear(void);

// String IDs.
typedef enum _StrID {
	
	/** GUI **/
	STR_RETURN_TO_HOME_MENU = 0,								// "Return to HOME Menu"
	STR_START,													// "START" (used on cartridge indicator)
	STR_NO_CARTRIDGE,											// "No cartridge"
	STR_UNKOWN,													// "Unknown Cartridge"
	STR_SETTINGS_TEXT,											// "Settings"
	STR_BACK,													// "B: Back"

	/** Settings: GUI **/
	STR_SETTINGS_LANGUAGE,										// "Language"
	STR_SETTINGS_THEME,											// "Theme"
	STR_SETTINGS_COLOR,											// "Color"
	STR_SETTINGS_MENUCOLOR,										// "Menu Color"
	STR_SETTINGS_FILENAME,										// "Show filename"
	STR_SETTINGS_COUNTER,										// "Game counter"
	STR_SETTINGS_CUSTOM_BOTTOM,									// "Custom bottom image"
	STR_SETTINGS_AUTOUPDATE_BOOTSTRAP,							// "Auto-update bootstrap"
	STR_SETTINGS_AUTOUPDATE_TWLOADER,							// "Auto-update to latest TWLoader"
	
	/** Settings: NTR/TWL_mode **/
	STR_SETTINGS_FLASHCARD_SELECT,								// "Flashcard(s) select"
	STR_SETTINGS_RAINBOW_LED,									// "Rainbow LED"
	SRT_SETTINGS_ARM9_CPU_SPEED,								// "ARM9 CPU Speed"
	STR_SETTINGS_VRAM_BOOST,									// "VRAM boost"
	STR_SETTINGS_DS_DSi_BOOT_SCREEN,							// "DS/DSi Boot Screen"
	STR_SETTINGS_DS_DSi_SAFETY_MESSAGE,							// "Health and Safety message"
	STR_SETTINGS_RESET_SLOT_1,									// "Reset Slot-1"
	STR_SETTINGS_CONSOLE_OUTPUT,								// "Console output"
	STR_SETTINGS_LOCK_ARM9_SCFG_EXT,							// "Lock ARM9 SCFG_EXT"
	STR_SETTINGS_BOOTSTRAP,										// "Bootstrap"
	
	/** Settings: Top Screen **/
	STR_SETTINGS_XBUTTON_RELEASE,								// "X: Update bootstrap (Official release)"
	STR_SETTINGS_YBUTTON_UNOFFICIAL,							// "Y: Update bootstrap (Unofficial release)"
	STR_SETTINGS_SETTINGS_START_UPDATE_TWLOADER,				// "START: Update TWLoader"
	
	/** Settings: GUI values **/
	
	// Color
	STR_SETTINGS_VALUES_GRAY,									// "Gray"
	STR_SETTINGS_VALUES_BROWN,									// "Brown"
	STR_SETTINGS_VALUES_RED,									// "Red"
	STR_SETTINGS_VALUES_PINK,									// "Pink"
	STR_SETTINGS_VALUES_ORANGE,									// "Orange"
	STR_SETTINGS_VALUES_YELLOW,									// "Yellow"
	STR_SETTINGS_VALUES_YELLOW_GREEN,							// "Yellow-Green"
	STR_SETTINGS_VALUES_GREEN_1,								// "Green 1"
	STR_SETTINGS_VALUES_GREEN_2,								// "Green 2"
	STR_SETTINGS_VALUES_LIGHT_GREEN,							// "Light green"
	STR_SETTINGS_VALUES_SKY_BLUE,								// "Sky blue"
	STR_SETTINGS_VALUES_LIGHT_BLUE,								// "Light blue"
	STR_SETTINGS_VALUES_BLUE,									// "Blue"
	STR_SETTINGS_VALUES_VIOLET,									// "Violet"
	STR_SETTINGS_VALUES_PURPLE,									// "Purple"
	STR_SETTINGS_VALUES_FUCHSIA,								// "Fuchsia"
	STR_SETTINGS_VALUES_RED_AND_BLUE,							// "Red and blue"
	STR_SETTINGS_VALUES_GREEN_AND_YELLOW,						// "Green and yellow"
	STR_SETTINGS_VALUES_CHRISTMAS,								// "Christmas"

	// Menu Color
	STR_SETTINGS_VALUES_WHITE,									// "White"
	STR_SETTINGS_VALUES_BLACK,									// "Black"

	/** Settings: GUI descriptions **/
	STR_SETTINGS_DESCRIPTION_LANGUAGE_1,						// "The language to use for the UI,"
	STR_SETTINGS_DESCRIPTION_LANGUAGE_2,						// "including game banner text."
	
	STR_SETTINGS_DESCRIPTION_THEME_1,							// "The theme to use in TWLoader."
	STR_SETTINGS_DESCRIPTION_THEME_2,							// "Press A for sub-themes."
	
	STR_SETTINGS_DESCRIPTION_COLOR_1,							// "The color of the top background,"
	STR_SETTINGS_DESCRIPTION_COLOR_2,							// "the START border, and the circling dots."
	
	STR_SETTINGS_DESCRIPTION_MENUCOLOR_1,						// "The color of the top border,"
	STR_SETTINGS_DESCRIPTION_MENUCOLOR_2,						// "and the bottom background."
	
	STR_SETTINGS_DESCRIPTION_FILENAME_1,						// "Shows game filename at the top of the bubble."
	STR_SETTINGS_DESCRIPTION_FILENAME_2,						// (empty)
	
	STR_SETTINGS_DESCRIPTION_COUNTER_1,							// "A number of selected game and listed games"
	STR_SETTINGS_DESCRIPTION_COUNTER_2,							// "is shown below the text bubble."
	
	STR_SETTINGS_DESCRIPTION_CUSTOM_BOTTOM_1,					// "Loads a custom bottom screen image"
	STR_SETTINGS_DESCRIPTION_CUSTOM_BOTTOM_2,					// "for the game menu."
	
	STR_SETTINGS_DESCRIPTION_AUTOUPDATE_BOOTSTRAP_1,			// "Auto-update nds-bootstrap at launch."
	STR_SETTINGS_DESCRIPTION_AUTOUPDATE_BOOTSTRAP_2,			// (empty)
	
	STR_SETTINGS_DESCRIPTION_AUTOUPDATE_TWLOADER_1,				// "Auto-download the CIA of the latest"
	STR_SETTINGS_DESCRIPTION_AUTOUPDATE_TWLOADER_2,				// "TWLoader version at launch."
	
	/** Settings: NTR/TWL_mode descriptions **/
	STR_SETTINGS_DESCRIPTION_FLASHCARD_SELECT_1,				// "Pick a flashcard to use to"
	STR_SETTINGS_DESCRIPTION_FLASHCARD_SELECT_2,				// "run ROMs from it."
	
	STR_SETTINGS_DESCRIPTION_RAINBOW_LED_1,						// "See rainbow colors glowing in"
	STR_SETTINGS_DESCRIPTION_RAINBOW_LED_2,						// "the Notification LED."
	
	SRT_SETTINGS_DESCRIPTION_ARM9_CPU_SPEED_1,					// "Set to TWL to get rid of lags in some games."
	SRT_SETTINGS_DESCRIPTION_ARM9_CPU_SPEED_2,					// (empty)
	
	STR_SETTINGS_DESCRIPTION_VRAM_BOOST_1,						// "Allows 8 bit VRAM writes"
	STR_SETTINGS_DESCRIPTION_VRAM_BOOST_2,						// "and expands the bus to 32 bit."
	
	STR_SETTINGS_DESCRIPTION_DS_DSi_BOOT_SCREEN_1,				// "Displays the DS/DSi boot animation"
	STR_SETTINGS_DESCRIPTION_DS_DSi_BOOT_SCREEN_2,				// "before launched game."
	
	STR_SETTINGS_DESCRIPTION_DS_DSi_SAFETY_MESSAGE_1,			// "Displays the Health and Safety"
	STR_SETTINGS_DESCRIPTION_DS_DSi_SAFETY_MESSAGE_2,			// "message on the bottom screen."
	
	STR_SETTINGS_DESCRIPTION_RESET_SLOT_1_1,					// "Enable this if Slot-1 carts are stuck"
	STR_SETTINGS_DESCRIPTION_RESET_SLOT_1_2,					// "on white screens."
	
	STR_SETTINGS_DESCRIPTION_CONSOLE_OUTPUT_1,					// "Displays some text before launched game."
	STR_SETTINGS_DESCRIPTION_CONSOLE_OUTPUT_2,					// (empty)
	
	STR_SETTINGS_DESCRIPTION_LOCK_ARM9_SCFG_EXT_1,				// "Locks the ARM9 SCFG_EXT,"
	STR_SETTINGS_DESCRIPTION_LOCK_ARM9_SCFG_EXT_2,				// "avoiding conflict with recent libnds."
	
	STR_SETTINGS_DESCRIPTION_BOOTSTRAP_1,						// "Change between release and"
	STR_SETTINGS_DESCRIPTION_BOOTSTRAP_2,						// "unofficial bootstrap file."
	
	/** Start menu **/
	// Options
	STR_START_GAMELOCATION,										// "Game location"
	STR_START_BOXART_ON,										// "Box Art: On"
	STR_START_BOXART_OFF,										// "Box Art: OFF"
	STR_START_START_GBARUNNER2,									// "Start GBARunner2"
	STR_START_TOP_BORDER_ON,									// "Top border: On"
	STR_START_TOP_BORDER_OFF,									// "Top border: Off"
	STR_START_UNSET_DONOR,										// "Unset donor rom"
	STR_START_SEARCH,											// "Search"
	// Values
	STR_START_SD_CARD,											// "SD Card"
	STR_START_FLASHCARD,										// "Flashcard"
	STR_START_SEARCH_HINT,										// "Use the keyboard to find roms"
	
	/** Select menu **/
	// Options
	STR_START_ARM9_CPU_SPEED,									// "ARM9 CPU Speed"
	STR_START_VRAM_BOOST,										// "VRAM boost"
	STR_START_LOCK_ARM9_SCFG_EXT,								// "Lock ARM9 SCFG_EXT"
	STR_START_SET_DONOR,										// "Set as donor ROM"
	STR_START_SET_LED,											// "Set LED color"
	// Values
	STR_START_DEFAULT,											// "Default"
	
	/** Sub-theme **/
	STR_SETTINGS_SUBTHEME_DSi,									// "Sub-theme select: DSi Menu"
	STR_SETTINGS_SUBTHEME_R4,									// "Sub-theme select: R4"
	STR_SETTINGS_SUBTHEME_WOOD,									// "Sub-theme select: Wood"
	
	STR_SETTINGS_NO_SUB_THEMES,									// "No sub-themes exist for this theme."
	
	/** Settings others minor strings **/
	STR_SETTINGS_AB_SAVE_RETURN,								// "A/B: Save and Return"
	STR_SETTINGS_LEFTRIGHT_PICK,								// "Left/Right: Pick"
	STR_SETTINGS_GUI,											// "Settings: GUI"
	STR_SETTINGS_NTR_TWL,										// "Settings: NTR/TWL-mode"
	
	STR_MAX
} StrID;

/**
 * Get a translation.
 *
 * NOTE: Call langInit() before using TR().
 *
 * @param strID String ID.
 * @return Translation, or error string if strID is invalid.
 */
const wchar_t *TR(StrID strID);

#endif /* TWLOADER_LANGUAGE_H */
