#include "language.h"
#include "textfns.h"
#include "settings.h"

#include <malloc.h>

#include <3ds.h>

/** Language data. **/
// NOTE: These strings must be encoded as UTF-8.

// Japanese
static const char *const lang_JP[STR_MAX] = {
	": HOMEメニューに戻る",	// "Return to HOME Menu"
	"スタート",			// "START" (used on cartridge indicator)

	// Settings
	"START：更新TWLoader",	// "START: Update TWLoader"
	"言語",			// "Language"
	"色",			// "Color"
	"メニューの色",			// "Menu Color"
	"表示するファイル名",		// "Show filename"
	"ゲームのカウンター",			// "Game counter"
};

// English
static const char *const lang_EN[STR_MAX] = {
	": Return to HOME Menu",	// "Return to HOME Menu"
	"START",			// "START" (used on cartridge indicator)

	// Settings
	"START: Update TWLoader",	// "START: Update TWLoader"
	"Language",			// "Language"
	"Color",			// "Color"
	"Menu color",			// "Menu Color"
	"Show filename",		// "Show filename"
	"Game counter",			// "Game counter"
};

// French
static const char *const lang_FR[STR_MAX] = {
	": Retour au menu HOME",	// "Return to HOME Menu"
	"OK",				// "START" (used on cartridge indicator)

	// Settings
	"START: Mettre à jour TWLoader",	// "START: Update TWLoader"
	"Langue",				// "Language"
	"Couleur",				// "Color"
	"Couleur du menu",			// "Menu Color"
	"Afficher le nom du fichier",		// "Show filename"
	"Compteur de jeux",			// "Game counter"
};

// German
static const char *const lang_DE[STR_MAX] = {
	": Zurück zum HOME-Menü",	// "Return to HOME Menu"
	"START",			// "START" (used on cartridge indicator)

	// Settings
	"START: TWLoader aktualisieren",	// "START: Update TWLoader"
	"Sprache",				// "Language"
	"Farbe",				// "Color"
	"Menüfarbe",				// "Menu Color"
	"Dateiname anzeigen",			// "Show filename"
	"Spielzähler",				// "Game counter"
};

// Italian
static const char *const lang_IT[STR_MAX] = {
    ": Ritorna al menu HOME",    // "Return to HOME Menu"
    "AVVIA",            // "START" (used on cartridge indicator)

    // Settings
    "START: Aggiornamento TWLoader",    // "START: Update TWLoader"
    "Lingua",                // "Language"
    "Colore",                // "Color"
    "Colore menu",                // "Menu Color"
    "Visualizza il nome del file",        // "Show filename"
    "Contatore giochi",            // "Game counter"
};

// Spanish
static const char *const lang_ES[STR_MAX] = {
	": Volver al menú HOME",	// "Return to HOME Menu"
	"INICIO",			// "START" (used on cartridge indicator)

	// Settings
	"START: Actualizar TWLoader",		// "START: Update TWLoader"
	"Idioma",				// "Language"
	"Color",				// "Color"
	"Color del menú",			// "Menu Color"
	"Mostrar nombre del archivo",		// "Show filename"
	"Contador de juegos",			// "Game counter"
};

// Dutch
static const char *const lang_NL[STR_MAX] = {
	": Keer terug naar HOME-menu",	// "Return to HOME Menu"
	"BEGIN",			// "START" (used on cartridge indicator)

	// Settings
	"START: update TWLoader",	// "START: Update TWLoader"
	"Taal",				// "Language"
	"Kleur",			// "Color"
	"menukleur",			// "Menu Color"
	"Toon bestandsnaam",		// "Show filename"
	"Game teller",			// "Game counter"
};

// Portuguese
static const char *const lang_PT[STR_MAX] = {
	": Retornar ao Menu HOME",	// "Return to HOME Menu"
	"COMEÇAR",			// "START" (used on cartridge indicator)

	// Settings
	"START: Atualizar o TWLoader",		// "START: Update TWLoader"
	"Língua",				// "Language"
	"Cor",					// "Color"
	"Cor do menu",				// "Menu Color"
	"Mostrar nome do arquivo",		// "Show filename"
	"Contador de jogos",			// "Game counter"
};

// All languages.
static const char *const *lang_all[12] = {
	lang_JP,	// Japanese
	lang_EN,	// English
	lang_FR,	// French
	lang_DE,	// German
	lang_IT,	// Italian
	lang_ES,	// Spanish
	lang_EN,	// Simplified Chinese (TODO)
	lang_EN,	// Korean (TODO)
	lang_NL,	// Dutch
	lang_PT,	// Portuguese
	lang_EN,	// Russian (TODO)
	lang_EN,	// Traditional Chinese (TODO)
};

/** Functions. **/

// System language setting.
u8 language = 1;	// Default to English.
u8 ntrtwlmode_language;
static const char *const *lang_data = lang_all[1];

// Translation cache.
static wchar_t *lang_cache[STR_MAX] = { };

/**
 * Initialize translations.
 * Uses the language ID specified in settings.ui.language.
 */
void langInit(void)
{
	CFGU_GetSystemLanguage(&ntrtwlmode_language);
	language = settings.ui.language;
	if (language < 0 || language >= 12) {
		// Get the system language setting.
		CFGU_GetSystemLanguage(&language);
		if (language < 0 || language >= 12) {
			// Invalid language.
			// Default to English.
			language = 1;
		}
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
