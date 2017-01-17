// Game card functions.
#include "gamecard.h"
#include "ndsheaderbanner.h"
#include "textfns.h"

#include <3ds.h>

#include <string>
#include <vector>
using std::string;
using std::vector;
using std::wstring;

// from main.cpp
extern u8 language;

// Current TWL card information.
static union {
	u32 d;
	char id4[4];
} twl_gameid;	// 4-character game ID
bool twl_inserted = false;
static string twl_product_code;
static sf2d_texture *twl_icon = NULL;
static vector<wstring> twl_text;

/**
 * Clear the cached icon and text.
 */
void gamecardClearCache(void)
{
	// NOTE: twl_inserted is NOT reset here.
	twl_gameid.d = 0;
	sf2d_free_texture(twl_icon);
	twl_product_code.clear();
	twl_icon = NULL;
	twl_text.clear();
}

/**
 * Poll for a game card change.
 * @param force If true, force a re-cache. (Use if polling hasn't been done in a while.)
 * @return True if the card has changed; false if not.
 */
bool gamecardPoll(bool force)
{
	// Check if a TWL card is present.
	bool inserted;
	FS_CardType type;
	if (R_FAILED(FSUSER_CardSlotIsInserted(&inserted)) || !inserted ||
	    R_FAILED(FSUSER_GetCardType(&type)))
	{
		// Card is not present.
		if (twl_inserted || force) {
			gamecardClearCache();
			twl_inserted = false;
			return true;
		}
		return false;
	}

	if (twl_inserted && !force) {
		// No card change.
		return false;
	}

	// New card detected.
	gamecardClearCache();
	twl_inserted = true;

	if (type != CARD_TWL) {
		// Unsupported card type.
		// TODO: Support CTR cards?
		gamecardClearCache();
		return true;
	}

	// Based on FBI 2.4.7's listtitles.c, task_populate_titles_add_twl()

	// Get the NDS ROM header.
	u8 header[0x3B4];
	if (R_FAILED(FSUSER_GetLegacyRomHeader(MEDIATYPE_GAME_CARD, 0, header))) {
		// Unable to read the ROM header.
		gamecardClearCache();
		return true;
	}

	// Get the game ID from the ROM header.
	u32 gameid;
	memcpy(&gameid, &header[0x0C], sizeof(gameid));
	twl_gameid.d = gameid;

	// Product code. Format: NTR-P-XXXX or TWL-P-XXXX
	char buf[16];
	snprintf(buf, sizeof(buf), "%s-P-%.4s",
		 (header[0x12] & 0x02 ? "TWL" : "NTR"),
		 twl_gameid.id4);
	twl_product_code = string(buf);

	twl_text.push_back(L"Banner Placeholder");
	return true;

	// TODO: Get the banner data.
#if 0
	// Get the banner.
	sNDSBanner ndsBanner;
	if (R_FAILED(FSUSER_GetLegacyBannerData(MEDIATYPE_GAME_CARD, 0, (u8*)&ndsBanner))) {
		// Unable to obtain the banner.
		gamecardClearCache();
		return true;
	}

	// Store the icon and banner text.
	sf2d_free_texture(twl_icon);
	twl_icon = grabIcon(&ndsBanner);
	twl_text = grabText(&ndsBanner, language);
#endif
}

/**
 * Get the game card's product code.
 * @return Product code, or NULL if not a TWL card.
 */
const char *gamecardGetProductCode(void)
{
	return twl_product_code.c_str();
}

/**
 * Get the game card's icon.
 * @return Game card icon, or NULL if not a TWL card.
 */
sf2d_texture *gamecardGetIcon(void)
{
	return twl_icon;
}

/**
 * Get the game card's banner text.
 * @return Game card banner text, or empty vector if not a TWL card.
 */
vector<wstring> gamecardGetText(void)
{
	return twl_text;
}
