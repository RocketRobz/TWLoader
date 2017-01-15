// TWLoader: Settings screen.
#ifndef TWLOADER_SETTINGS_H
#define TWLOADER_SETTINGS_H

// Textures.
#include <sf2d.h>
extern sf2d_texture *settingstex;

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

extern int settings_colorvalue;
extern int settings_menucolorvalue;
extern int settings_filenamevalue;
extern int settings_locswitchvalue;
extern int settings_topbordervalue;
extern int settings_countervalue;
extern int settings_custombotvalue;
extern int settings_autoupdatevalue;
extern int settings_autodlvalue;

extern int twlsettings_rainbowledvalue;
extern int twlsettings_cpuspeedvalue;
extern int twlsettings_extvramvalue;
extern int twlsettings_forwardervalue;
extern int twlsettings_flashcardvalue;
/* Flashcard value
	0: DSTT/R4i Gold/R4i-SDHC/R4 SDHC Dual-Core/R4 SDHC Upgrade/SC DSONE
	1: R4DS (Original Non-SDHC version)/ M3 Simply
	2: R4iDSN/R4i Gold RTS
	3: Acekard 2(i)/M3DS Real/R4i-SDHC v1.4.x
	4: Acekard RPG
	5: Ace 3DS+/Gateway Blue Card/R4iTT
	6: SuperCard DSTWO
*/
extern int twlsettings_bootscreenvalue;
extern int twlsettings_healthsafetyvalue;
extern int twlsettings_launchslot1value;
extern int twlsettings_resetslot1value;
extern int twlsettings_consolevalue;
extern int twlsettings_lockarm9scfgextvalue;

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

#endif /* TWLOADER_SETTINGS_H */
