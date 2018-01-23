#ifndef JAPANESE_H
#define JAPANESE_H

#include <3ds/types.h>

// JAPANESE
static const char *const lang_JP[STR_MAX] = {

	/** DS/DSi boot screen **/
	"",															// "WARNING - HEALTH AND SAFETY"
	"",															// "BEFORE PLAYING, READ THE HEALTH"
	"",															// "AND SAFETY PRECAUTIONS BOOKLET"
	"",															// "FOR IMPORTANT INFORMATION"
	"",															// "ABOUT YOUR HEALTH AND SAFETY."
	"",															// "TO GET AN EXTRA COPY FOR YOUR REGION, GO ONLINE AT"
	"www.nintendo.co.jp/anzen/",								// "www.nintendo.com/healthsafety/"
	"",
	
	"",															// "Touch the Touch Screen to continue."
	
	/** GUI **/
	": HOMEメニューに戻る",											// "Return to HOME Menu"
	"スタート",														// "START" (used on cartridge indicator)
	"No cartridge",												// "No cartridge"
	"Unknown Cartridge",										// "Unknown Cartridge"
	"セッティング",													// "Settings"
	": バック",													// "B: Back"	
	": Add Games",												// "Y: Add Games"

	/** Settings: GUI **/
	"言語",														// "Language"
	"テーマ",														// "Theme"
	"セッティングの色",													// "Color"
	"メニューの色",													// "Menu Color"
	"表示するファイル名",												// "Show filename"
	"ゲームのカウンター",													// "Game counter"
	"Custom bottom image",										// "Custom bottom image"
	"Auto-update TWLoader",										// "Auto-update TWLoader"
	"TWLoader filetype",										// "TWLoader filetype"
	"TWLoaderの自動更新",											// "Update TWLoader"
	"DS/DSi Boot Screen",										// "DS/DSi Boot Screen"
	"Health and Safety message",								// "Health and Safety message"
	"ブートスクリーンの見せる",												// "Show Boot Screen"
	"ROM path",													// "ROM path"
	"Quick start",												// "Quick start"

	/** Settings: NTR_mode **/
	"Flashcard(s) select",										// "Flashcard(s) select"
	"Notification LED color",									// "Notification LED color"
	"ARM9 CPUスピード",												// "ARM9 CPU Speed"
	"Sound/Microphone frequency",								// "Sound/Microphone frequency"
	"SD card access for Slot-1",								// "SD card access for Slot-1"
	"Reset Slot-1",												// "Reset Slot-1"
	"Bootstrap loading screen",									// "Bootstrap loading screen"
	"Console output",											// "Console output"
	"ブートストラップの自動更新",											// "Auto-update bootstrap"
	"ブートストラップ",													// "Bootstrap"

	/** Settings: Top Screen **/
	": ブートストラップを更新する (公式)",									// "X: Update bootstrap (Official release)"
	": ブートストラップを更新する (民間)",									// "Y: Update bootstrap (Unofficial release)"

	/** Settings: GUI values **/

	// Color
	"灰色",														// "Gray"
	"茶色",														// "Brown"
	"赤",														// "Red"
	"桃色",												 		// "Pink"
	"橙色",														// "Orange"
	"黄色",														// "Yellow"
	"黄海",														// "Yellow-Green"
	"緑",														// "Green 1"
	"碧色",														// "Green 2"
	"黄緑",														// "Light green"
	"空色",														// "Sky blue"
	"水色",														// "Light blue"
	"青",														// "Blue"
	"濃紫",														// "Violet"
	"紫",														// "Purple"
	"濃桃",														// "Fuchsia"
	"Red&blue",													// "Red and blue"
	"Green&yellow",												// "Green and yellow"
	"クリスマス",													// "Christmas"

	// Menu Color
	"白",														// "White"
	"灰色-黒",													// "Gray-Black"

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

	"通知設定LEDの虹,"												// "See rainbow colors glowing in"
	"",															// "the Notification LED."

	"Set to TWL to get rid of lags in some games.",				// "Set to TWL to get rid of lags in some games."
	"",															// (empty)

	"32.73 kHz: Original quality",								// "32.73 kHz: Original quality"
	"47.61 kHz: High quality",									// "47.61 kHz: High quality"

	// "Allows 8 bit VRAM writes",								// "Allows 8 bit VRAM writes"
	// "and expands the bus to 32 bit.",						// "and expands the bus to 32 bit."

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

	// "Locks the ARM9 SCFG_EXT,",								// "Locks the ARM9 SCFG_EXT,"
	// "avoiding conflict with recent libnds.",					// "avoiding conflict with recent libnds."

	"Auto-update nds-bootstrap at launch.",						// "Auto-update nds-bootstrap at launch."
	"",															// (empty)

	"Pick release or unofficial bootstrap.",					// "Pick release or unofficial bootstrap."
	"",															// (empty)

	/** Start menu **/
	// Options
	"ゲームロケーション",													// "Game location"
	"Box Art: はい",												// "Box Art: On"
	"Box Art: いいえ",												// "Box Art: OFF"
	"Select ROM type",											// "Select ROM type"
	"Top border: はい",											// "Top border: On"
	"Top border: いいえ",											// "Top border: Off"
	"Unset donor ROM",											// "Unset donor rom"	
	"探す",														// "Search"
	// Values
	"SDカード",													// "SD Card"
	"フラッシュカード",													// "Flashcard"
	"Use the keyboard to find roms",							// "Use the keyboard to find roms"

	/** Select ROM type **/
	"GBARunner2スタート",											// "Start GBARunner2"

	/** Select menu **/
	// Options
	"ARM9 CPUスピード",												// "ARM9 CPU Speed"
	"SD access",												// "SD access"
	"Use set donor ROM",										// "Use set donor ROM"
	"Set as donor ROM",											// "Set as donor ROM"
	"Set LED color",											// "Set LED color"
	// Values
	"Default",													// "Default"

	/** Sub-theme **/
	"Sub-theme select: DSiメニュー",								// "Sub-theme select: DSi Menu"
	"Sub-theme select: R4",										// "Sub-theme select: R4"
	"Sub-theme select: akMenu/Wood",							// "Sub-theme select: Wood"

	"No sub-themes exist for this theme.",						// "No sub-themes exist for this theme."

	/** Settings others minor strings **/
	"/: Save and Return",										// "A/B: Save and Return"
	"Left/Right: Pick",											// "Left/Right: Pick"
	"セッティングのGUI:",												// "Settings: GUI"
	"セッティングのNTRモード",												// "Settings: NTR mode"

};


#endif /* JAPANESE_H */
