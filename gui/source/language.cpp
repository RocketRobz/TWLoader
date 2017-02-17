#include "language.h"
#include "textfns.h"
#include "settings.h"

#include <malloc.h>

#include <3ds.h>

/** Language data. **/
// NOTE: These strings must be encoded as UTF-8.
#include "languages/japanese.h"
#include "languages/english.h"
#include "languages/french.h"
#include "languages/german.h"
#include "languages/italian.h"
#include "languages/spanish.h"
#include "languages/dutch.h"
#include "languages/portuguese.h"
#include "languages/korean.h"
#include "languages/russian.h"

// All languages.
static const char *const *lang_all[12] = {
	lang_JP,	// Japanese
	lang_EN,	// English
	lang_FR,	// French
	lang_DE,	// German
	lang_IT,	// Italian
	lang_ES,	// Spanish
	lang_EN,	// Simplified Chinese (TODO)
	lang_KO,	// Korean
	lang_NL,	// Dutch
	lang_PT,	// Portuguese
	lang_RU,	// Russian
	lang_EN,	// Traditional Chinese (TODO)
};

/** Functions. **/

// System language setting.
u8 language = 1;	// UI language ID.
u8 sys_language = 1;	// System language ID.
static const char *const *lang_data = lang_all[1];

// Translation cache.
static wchar_t *lang_cache[STR_MAX] = { };

/**
 * Initialize translations.
 * Uses the language ID specified in settings.ui.language.
 */
void langInit(void)
{
	if (R_FAILED(CFGU_GetSystemLanguage(&sys_language)) ||
	    (sys_language < 0 || sys_language >= 12))
	{
		// Invalid system language ID.
		// Default to English.
		sys_language = 1;
	}

	language = settings.ui.language;
	if (language < 0 || language >= 12) {
		// Invalid language ID.
		// Default to the system language setting.
		language = sys_language;
	}

	// Clear the language cache.
	langClear();
	// Set the selected language.
	lang_data = lang_all[language];
}

/**
 * Clear the translations cache.
 */
void langClear(void)
{
	for (int i = STR_MAX-1; i >= 0; i--) {
		free(lang_cache[i]);
		lang_cache[i] = NULL;
	}
}

/**
 * Get a translation.
 *
 * NOTE: Call langInit() before using TR().
 *
 * @param strID String ID.
 * @return Translation, or error string if strID is invalid.
 */
const wchar_t *TR(StrID strID)
{
	if (strID < 0 || strID >= STR_MAX) {
		// Invalid string ID.
		return L"STRID ERR";
	}

	if (lang_cache[strID]) {
		// String has already been converted to wchar_t*.
		return lang_cache[strID];
	}

	// Convert the string to wchar_t*.
	lang_cache[strID] = utf8_to_wchar(lang_data[strID]);
	return lang_cache[strID];
}
