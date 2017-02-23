#ifndef JAPANESE_H
#define JAPANESE_H

#include <3ds/types.h>

// JAPANESE
static const char *const lang_JP[STR_MAX] = {
	
	/** GUI **/
    ": Return to HOME Menu",									// "Return to HOME Menu"
	"START",													// "START" (used on cartridge indicator)
	"No cartridge",												// "No cartridge"
	"Unknown Cartridge",										// "Unknown Cartridge"
	"Settings",													// "Settings"
	"B: Back",													// "B: Back"	
	
	/** Settings: GUI **/
	"Language",													// "Language"
	"Theme",													// "Theme"
	"Color",													// "Color"
	"Menu color",												// "Menu Color"
	"Show filename",											// "Show filename"
	"Game counter",												// "Game counter"
	"Custom bottom image",										// "Custom bottom image"
	"Auto-update bootstrap",									// "Auto-update bootstrap"
	"Auto-update to latest TWLoader",							// "Auto-update to latest TWLoader"
	
	/** Settings: NTR/TWL_mode **/
	"Flashcard(s) select",										// "Flashcard(s) select"
	"Rainbow LED",												// "Rainbow LED"
	"ARM9 CPU Speed",											// "ARM9 CPU Speed"
	"VRAM boost",												// "VRAM boost"
	"DS/DSi Boot Screen",										// "DS/DSi Boot Screen"
	"Health and Safety message",								// "Health and Safety message"
	"Reset Slot-1",												// "Reset Slot-1"
	"Console output",											// "Console output"
	"Lock ARM9 SCFG_EXT",										// "Lock ARM9 SCFG_EXT"
	"Bootstrap",												// "Bootstrap"
	
	/** Settings: Top Screen **/
	"X: Update bootstrap (Official release)",					// "X: Update bootstrap (Official release)"
	"Y: Update bootstrap (Unofficial release)",					// "Y: Update bootstrap (Unofficial release)"
    "START: Update TWLoader",    								// "START: Update TWLoader"
	
	/** Settings: GUI values **/
	
	// Color
	"Gray",														// "Gray"
	"Brown",													// "Brown"
	"Red",														// "Red"
	"Pink",												 		// "Pink"
	"Orange",													// "Orange"
	"Yellow",													// "Yellow"
	"Yellow-Green",												// "Yellow-Green"
	"Green 1",													// "Green 1"
	"Green 2",													// "Green 2"
	"Light green",												// "Light green"
	"Sky blue",													// "Sky blue"
	"Light blue",												// "Light blue"
	"Blue",														// "Blue"
	"Violet",													// "Violet"
	"Purple",													// "Purple"
	"Fuchsia",													// "Fuchsia"
	"Red&blue",													// "Red and blue"
	"Green&yellow",												// "Green and yellow"
	"Christmas",												// "Christmas"

	// Menu Color
	"White",													// "White"
	"Black",													// "Black"

	/** Settings: GUI descriptions **/
	"The language to use for the UI,",							// "The language to use for the UI,"
	"including game banner text.",								// "including game banner text."
	
	"The theme to use in TWLoader.",							// "The theme to use in TWLoader."
	"Press A for sub-themes.",									// "Press A for sub-themes."
	
	"The color of the top background,",							// "The color of the top background,"
	"the START border, and the circling dots.",					// "the START border, and the circling dots."
	
	"The color of the top border,",								// "The color of the top border,"
	"and the bottom background.",								// "and the bottom background."
	
	"Shows game filename at the top of the bubble.",			// "Shows game filename at the top of the bubble."
	"",															// (empty)
	
	"A number of selected game and listed games",				// "A number of selected game and listed games"
	"is shown below the text bubble.",							// "is shown below the text bubble."
	
	"Loads a custom bottom screen image",						// "Loads a custom bottom screen image"
	"for the game menu.",										// "for the game menu."
	
	"Auto-update nds-bootstrap at launch.",						// "Auto-update nds-bootstrap at launch."
	"",															// (empty)

	"Auto-download the CIA of the latest",						// "Auto-download the CIA of the latest"
	"TWLoader version at launch.",								// "TWLoader version at launch."
	
	/** Settings: NTR/TWL_mode descriptions **/
	"Pick a flashcard to use to",								// "Pick a flashcard to use to"
	"run ROMs from it.",										// "run ROMs from it."
	
	"See rainbow colors glowing in",							// "See rainbow colors glowing in"
	"the Notification LED.",									// "the Notification LED."
	
	"Set to TWL to get rid of lags in some games.",				// "Set to TWL to get rid of lags in some games."
	"",															// (empty)
	
	"Allows 8 bit VRAM writes",									// "Allows 8 bit VRAM writes"
	"and expands the bus to 32 bit.",							// "and expands the bus to 32 bit."
	
	"Displays the DS/DSi boot animation",						// "Displays the DS/DSi boot animation"
	"before launched game.",									// "before launched game."
	
	"Displays the Health and Safety",							// "Displays the Health and Safety"
	"message on the bottom screen.",							// "message on the bottom screen."
	
	"Enable this if Slot-1 carts are stuck",					// "Enable this if Slot-1 carts are stuck"
	"on white screens.",										// "on white screens."
	
	"Displays some text before launched game.",					// "Displays some text before launched game."
	"",															// (empty)
	
	"Locks the ARM9 SCFG_EXT,",									// "Locks the ARM9 SCFG_EXT,"
	"avoiding conflict with recent libnds.",					// "avoiding conflict with recent libnds."
	
	"Change between release and",								// "Change between release and"
	"unofficial bootstrap file.",								// "unofficial bootstrap file."
	
	/** Start menu **/
	// Options
	"Game location",											// "Game location"
	"Box Art: On",												// "Box Art: On"
	"Box Art: Off",												// "Box Art: OFF"
	"Start GBARunner2",											// "Start GBARunner2"
	"Top border: On",											// "Top border: On"
	"Top border: Off",											// "Top border: Off"
	"Unset donor ROM",											// "Unset donor rom"	
	"Search",													// "Search"
	// Values
	"SD Card",													// "SD Card"
	"Flashcard",												// "Flashcard"
	"Use the keyboard to find roms",							// "Use the keyboard to find roms"
	
	/** Select menu **/
	// Options
	"ARM9 CPU Speed",											// "ARM9 CPU Speed"
	"VRAM boost",												// "VRAM boost"
	"Lock ARM9 SCFG_EXT",										// "Lock ARM9 SCFG_EXT"
	"Set as donor ROM",											// "Set as donor ROM"
	"Set LED color",											// "Set LED color"
	// Values
	"Default",													// "Default"
	
	/** Sub-theme **/
	"Sub-theme select: DSi Menu",								// "Sub-theme select: DSi Menu"
	"Sub-theme select: R4",										// "Sub-theme select: R4"
	"Sub-theme select: akMenu/Wood",									// "Sub-theme select: Wood"
	
	"No sub-themes exist for this theme.",						// "No sub-themes exist for this theme."
	
	/** Settings others minor strings **/
	"A/B: Save and Return",										// "A/B: Save and Return"
	"Left/Right: Pick",											// "Left/Right: Pick"
	"Settings: GUI",											// "Settings: GUI"
	"Settings: NTR/TWL-mode",									// "Settings: NTR/TWL-mode"
	
};


#endif /* JAPANESE_H */
