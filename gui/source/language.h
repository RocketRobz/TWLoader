// Language functions.
#ifndef TWLOADER_LANGUAGE_H
#define TWLOADER_LANGUAGE_H

#include <3ds/types.h>

// System language setting.
extern u8 language;

/**
 * Initialize translations.
 */
void langInit(void);

/**
 * Clear the translations cache.
 */
void langClear(void);

// String IDs.
typedef enum _StrID {
	STR_RETURN_TO_HOME_MENU = 0,	// "Return to HOME Menu"
	STR_START,			// "START" (used on cartridge indicator)

	// Settings
	STR_SETTINGS_START_UPDATE_TWLOADER,	// "START: Update TWLoader"
	STR_SETTINGS_COLOR,			// "Color"
	STR_SETTINGS_MENUCOLOR,			// "Menu Color"
	STR_SETTINGS_FILENAME,			// "Show filename"
	STR_SETTINGS_LOCSWITCH,			// "Game location switcher"
	STR_SETTINGS_TOPBORDER,			// "Top border"
	STR_SETTINGS_COUNTER,			// "Game counter"

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
