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
static GameCardType twl_card_type = CARD_TYPE_UNKNOWN;
static char twl_product_code[16] = { };
static u8 twl_revision = 0xFF;
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
	twl_card_type = CARD_TYPE_UNKNOWN;
	twl_product_code[0] = 0;
	twl_revision = 0xFF;
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
	sNDSHeader header;
	if (R_FAILED(FSUSER_GetLegacyRomHeader(MEDIATYPE_GAME_CARD, 0, (u8*)&header))) {
		// Unable to read the ROM header.
		gamecardClearCache();
		return true;
	}

	// Get the game ID from the ROM header.
	u32 gameid;
	memcpy(&gameid, header.gameCode, sizeof(gameid));
	twl_gameid.d = gameid;

	// Card type.
	const char *prefix;
	switch (header.unitCode & 0x03) {
		case 0x00:
		default:
			twl_card_type = CARD_TYPE_NTR;
			prefix = "NTR";
			break;
		case 0x02:
			twl_card_type = CARD_TYPE_TWL_ENH;
			prefix = "TWL";
			break;
		case 0x03:
			twl_card_type = CARD_TYPE_TWL_ONLY;
			prefix = "TWL";
			break;
	}

	// Product code. Format: NTR-P-XXXX or TWL-P-XXXX
	snprintf(twl_product_code, sizeof(twl_product_code), "%s-P-%.4s", prefix, twl_gameid.id4);

	// Revision.
	twl_revision = header.romversion;

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
	return true;
}

/**
 * Get the game card's type.
 * @return Game card type.
 */
GameCardType gamecardGetType(void)
{
	return twl_card_type;
}

/**
 * Get the game card's product code.
 * @return Product code, or NULL if not a TWL card.
 */
const char *gamecardGetProductCode(void)
{
	return twl_product_code;
}

/**
 * Get the game card's revision..
 * @return Game card revision. (0xFF if unknown.)
 */
u8 gamecardGetRevision(void)
{
	return twl_revision;
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
