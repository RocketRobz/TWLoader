// TWLoader: Settings screen.
#ifndef TWLOADER_SETTINGS_H
#define TWLOADER_SETTINGS_H

#include <string>
#include "graphic.h"
#include "pp2d/pp2d.h"

/** Settings **/

// Color settings.
// Use SET_ALPHA() to replace the alpha value.
#define SET_ALPHA(color, alpha) ((((alpha) & 0xFF) << 24) | ((color) & 0x00FFFFFF))
typedef struct _ColorData {
	const char *topbgloc;
	const char *dotcircleloc;
	const char *startborderloc;
	u32 color;
} ColorData;
extern const ColorData *color_data;
extern u32 menucolor;

// 3D offsets.
typedef struct _Offset3D {
	float topbg;
	float boxart;
	float disabled;
} Offset3D;
extern Offset3D offset3D[2];	// 0 == Left; 1 == Right

// Location of the bottom screen image.
extern const char* bottomloc;

typedef struct _Settings_t {
	struct {
		bool toplayout;
		//	0: Show box art
		//	1: Hide box art
	} romselect;

	// TODO: Use int8_t instead of int?
	struct {
		std::string name;
		std::string romfolder;
		std::string fcromfolder;
 		std::string gbromfolder;
 		std::string nesromfolder;

		bool firstTimeMsgViewed;

		int pagenum;
		int cursorPosition;
		bool quickStart;
		int language;	// Language. (0-11; other for system)
		int theme;
		int subtheme;
		int color;
		int menucolor;
		bool woodIconScaleEffect;
		bool filename;
		bool locswitch;
		bool topborder;
		bool iconsize;
		bool counter;
		bool custombot;
		int filetype;	// 0 = CIA, 1 = 3DSX, 2 = CIA & 3DSX
		int autoupdate;	// 0 = Off, 1 = Release, 2 = Unofficial
		bool autoupdate_twldr;
		int showbootscreen;	// 0 = No, 1 = Before ROM select screen, 2 = After launching a ROM
		int bootscreen;	// 0 = Nintendo DS, 1 = Nintendo DS (OAR), 2 = Nintendo DSi
		bool healthsafety;
	} ui;

	struct {
		int rainbowled;	// 0 = None, 1 = Green, 2 = Rainbow
		bool cpuspeed;	// false == NTR, true == TWL
		bool soundfreq;	// false == 32.73 kHz, true == 47.61 kHz
		bool enablesd;
		int romtype;
		/* ROM type value
			0: .nds
			1: .gb/.gbc/.sgb
		*/

		bool forwarder;

		int flashcard;
		/* Flashcard value
			0: DSTT/R4i Gold/R4i-SDHC/R4 SDHC Dual-Core/R4 SDHC Upgrade/SC DSONE
			1: R4DS (Original Non-SDHC version)/ M3 Simply
			2: R4iDSN/R4i Gold RTS
			3: Acekard 2(i)/Galaxy Eagle/M3DS Real
			4: Acekard RPG
			5: Ace 3DS+/Gateway Blue Card/R4iTT
			6: SuperCard DSTWO
		*/

		bool launchslot1;
		bool resetslot1;
		int loadingscreen;	// 0 = None, 1 = Regular, 2 = Pong
		int console;	// 0 = Off, 1 = On, 2 = On (Debug)
		bool lockarm9scfgext;
		
		int donorSdkVer;
		
		bool run_timeout;
		
		int mpuregion; // Region 0, 1, 2, 3
		int mpusize; // Size 0, 1, 3145728
		
		int bootstrapfile; // 0 = release, 1 = unofficial, 2 = old
	} twl;
	
	struct {
		// -1 == default; 0 == off, 1 == on
		s8 cpuspeed;	// false == NTR, true == TWL
		s8 usedonor;
		s8 donor;
		int red;
		int green;
		int blue;
	} pergame;
} Settings_t;
extern Settings_t settings;

/**
 * Reset the settings screen's subscreen mode.
 */
void settingsResetSubScreenMode(void);

/**
 * Load the settings textures.
 */
void settingsLoadTextures(void);

/**
 * Unload the settings textures.
 */
void settingsUnloadTextures(void);

/**
 * Draw the top settings screen.
 */
void settingsDrawTopScreen(void);

/**
 * Draw the bottom settings screen.
 */
void settingsDrawBottomScreen(void);

/**
 * Move the cursor if necessary.
 * @param hDown Key value from hidKeysDown().
 * @return True if the bottom screen needs to be updated.
 */
bool settingsMoveCursor(u32 hDown);

/**
 * Load the primary color from the configuration.
 */
void LoadColor(void);

/**
 * Load the menu color from the configuration.
 */
void LoadMenuColor(void);

/**
 * Load the filename of the bottom screen image.
 */
void LoadBottomImage(void);

/**
 * Load settings.
 */
void LoadSettings(void);

/**
 * Save settings.
 */
void SaveSettings(void);

#endif /* TWLOADER_SETTINGS_H */
