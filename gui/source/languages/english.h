#ifndef ENGLISH_H
#define ENGLISH_H

#include <3ds/types.h>

// ENGLISH
static const char *const lang_EN[STR_MAX] = {
	
	/** DS/DSi boot screen **/
	"WARNING - HEALTH AND SAFETY",								// "WARNING - HEALTH AND SAFETY"
	"BEFORE PLAYING, READ THE HEALTH",							// "BEFORE PLAYING, READ THE HEALTH"
	"AND SAFETY PRECAUTIONS BOOKLET",							// "AND SAFETY PRECAUTIONS BOOKLET"
	"FOR IMPORTANT INFORMATION",								// "FOR IMPORTANT INFORMATION"
	"ABOUT YOUR HEALTH AND SAFETY.",							// "ABOUT YOUR HEALTH AND SAFETY."
	"TO GET AN EXTRA COPY FOR YOUR REGION, GO ONLINE AT",		// "TO GET AN EXTRA COPY FOR YOUR REGION, GO ONLINE AT"
	"www.nintendo.com/healthsafety/",							// "www.nintendo.com/healthsafety/"
	"",
	
	"Touch the Touch Screen to continue.",						// "Touch the Touch Screen to continue."
	
	/** GUI **/
	": Return to HOME Menu",									// "Return to HOME Menu"
	"START",													// "START" (used on cartridge indicator)
	"No cartridge",												// "No cartridge"
	"Unknown Cartridge", 										// "Unknown Cartridge"
	"Settings",													// "Settings"
	": Back",													// "B: Back"	
	": Add Games",												// "Y: Add Games"

	/** Settings: GUI **/
	"Language",													// "Language"
	"Theme",													// "Theme"
	"Color",													// "Color"
	"Menu color",												// "Menu Color"
	"Show filename",											// "Show filename"
	"Game counter",												// "Game counter"
	"Custom bottom image",										// "Custom bottom image"
	"Auto-update TWLoader",										// "Auto-update TWLoader"
	"TWLoader filetype",										// "TWLoader filetype"
	"Update TWLoader",											// "Update TWLoader"
	"DS/DSi Boot Screen",										// "DS/DSi Boot Screen"
	"H&S Message",												// "Health and Safety message"
	"Show Boot Screen",											// "Show Boot Screen"
	"ROM path",													// "ROM path"
	"Quick start",												// "Quick start"

	/** Settings: NTR_mode **/
	"Flashcard(s) select",										// "Flashcard(s) select"
	"Notification LED color",									// "Notification LED color"
	"ARM9 CPU Speed",											// "ARM9 CPU Speed"
	"Sound/Microphone frequency",								// "Sound/Microphone frequency"
	"SD card access for Slot-1",								// "SD card access for Slot-1"
	"Reset Slot-1",												// "Reset Slot-1"
	"Bootstrap loading screen",									// "Bootstrap loading screen"
	"Console output",											// "Console output"
	"Auto-update bootstrap",									// "Auto-update bootstrap"
	"Bootstrap",												// "Bootstrap"

	/** Settings: Top Screen **/
	": Update bootstrap (Official release)",					// "X: Update bootstrap (Official release)"
	": Update bootstrap (Unofficial release)",					// "Y: Update bootstrap (Unofficial release)"

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
	"Gray-Black",												// "Gray-Black"

	/** Settings: GUI descriptions **/
	"The language to use for the UI,",							// "The language to use for the UI,"
	"including game banner text.",								// "including game banner text."

	"The theme to use in TWLoader.",							// "The theme to use in TWLoader."
	"Press START for sub-themes.",								// "Press START for sub-themes."

	"The color of the top background,",							// "The color of the top background,"
	"the START border, and the circling dots.",					// "the START border, and the circling dots."

	"The color of the top border,",								// "The color of the top border,"
	"and the bottom background.",								// "and the bottom background."

	"Shows game filename at the",								// "Shows game filename at the"
	"top of the bubble.",										// "top of the bubble."

	"A number of selected game and listed",						// "A number of selected game and listed"
	"games is shown below the text bubble.",					// "games is shown below the text bubble."

	"Loads a custom bottom screen image",						// "Loads a custom bottom screen image"
	"for the DSi Menu theme.",									// "for the DSi Menu theme."

	"Auto-update to the latest",								// "Auto-update to the latest"
	"TWLoader version at launch.",								// "TWLoader version at launch."

	"Where do you want to see the",								// "Where do you want to see the"
	"DS/DSi boot screen?",										// "DS/DSi boot screen?"

	"On startup, WiFi features and",							// "On startup, WiFi features and"
	"banner caching will be skipped.",							// "banner caching will be skipped."

	/** Settings: NTR/TWL_mode descriptions **/
	"Pick a flashcard to use to",								// "Pick a flashcard to use to"
	"run ROMs from it.",										// "run ROMs from it."

	"Set a color to glow in",									// "Set a color to glow in"
	"the Notification LED.",									// "the Notification LED."

	"Set to TWL to get rid of lags",							// "Set to TWL to get rid of lags"
	"in some games.",											// "in some games."

	"32.73 kHz: Original quality",								// "32.73 kHz: Original quality"
	"47.61 kHz: High quality",									// "47.61 kHz: High quality"

	// "Allows 8 bit VRAM writes",									// "Allows 8 bit VRAM writes"
	// "and expands the bus to 32 bit.",							// "and expands the bus to 32 bit."

	"Enables SD card access for",								// "Enables SD card access for"
	"Slot-1 cards.",											// "Slot-1 cards."

	"Displays the DS/DSi boot animation",						// "Displays the DS/DSi boot animation"
	"before launched game.",									// "before launched game."

	"Displays the Health and Safety",							// "Displays the Health and Safety"
	"message on the bottom screen.",							// "message on the bottom screen."

	"Enable this if Slot-1 carts are stuck",					// "Enable this if Slot-1 carts are stuck"
	"on white screens.",										// "on white screens."

	"Shows a loading screen before ROM",						// "Shows a loading screen before ROM"
	"is started in nds-bootstrap.",								// "is started in nds-bootstrap."

	"Displays some text before launched game.",					// "Displays some text before launched game."
	"",															// (empty)

	// "Locks the ARM9 SCFG_EXT,",									// "Locks the ARM9 SCFG_EXT,"
	// "avoiding conflict with recent libnds.",					// "avoiding conflict with recent libnds."

	"Auto-update nds-bootstrap at launch.",						// "Auto-update nds-bootstrap at launch."
	"",															// (empty)

	"Pick release or unofficial bootstrap.",					// "Pick release or unofficial bootstrap."
	"",															// (empty)

	/** Start menu **/
	// Options
	"Game location",											// "Game location"
	"Box Art: On",												// "Box Art: On"
	"Box Art: Off",												// "Box Art: OFF"
	"Select ROM type",											// "Select ROM type"
	"Top border: On",											// "Top border: On"
	"Top border: Off",											// "Top border: Off"
	"Unset donor ROM",											// "Unset donor rom"
	"Search",													// "Search"
	// Values
	"SD Card",													// "SD Card"
	"Flashcard",												// "Flashcard"
	"Use the keyboard to find roms",							// "Use the keyboard to find roms"

	/** Select ROM type **/
	"Start GBARunner2",											// "Start GBARunner2"

	/** Select menu **/
	// Options
	"ARM9 CPU Speed",											// "ARM9 CPU Speed"
	"SD access",												// "SD access"
	"Use set donor ROM",										// "Use set donor ROM"
	"Set as donor ROM",											// "Set as donor ROM"
	"Set LED color",											// "Set LED color"
	// Values
	"Default",													// "Default"

	/** Sub-theme **/
	"Sub-theme select: DSi Menu",								// "Sub-theme select: DSi Menu"
	"Sub-theme select: R4",										// "Sub-theme select: R4"
	"Sub-theme select: akMenu/Wood",							// "Sub-theme select: Wood"

	"No sub-themes exist for this theme.",						// "No sub-themes exist for this theme."

	/** Settings others minor strings **/
	"/: Save and Return",										// "A/B: Save and Return"
	"Left/Right: Pick",											// "Left/Right: Pick"
	"Settings: GUI",											// "Settings: GUI"
	"Settings: NTR mode",										// "Settings: NTR mode"

};


#endif /* ENGLISH_H */