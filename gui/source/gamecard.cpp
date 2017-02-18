// Game card functions.
#include "gamecard.h"
#include "language.h"
#include "ndsheaderbanner.h"
#include "textfns.h"

#include <3ds.h>
#include "smdh.h"

#include <malloc.h>

#include <string>
#include <vector>
using std::string;
using std::vector;
using std::wstring;

// Current card information.
static union {
	// TODO: Use this field for CTR as well?
	u32 d;
	char id4[4];
} twl_gameid;	// 4-character game ID
static bool card_inserted = false;
static GameCardType card_type = CARD_TYPE_UNKNOWN;
static char card_product_code[20] = { };
static u8 card_revision = 0xFF;
static u64 card_tid = 0;
static sf2d_texture *card_icon = NULL;
static vector<wstring> card_text;

/**
 * Clear the cached icon and text.
 */
void gamecardClearCache(void)
{
	// NOTE: card_inserted is NOT reset here.
	twl_gameid.d = 0;
	sf2d_free_texture(card_icon);
	card_type = CARD_TYPE_UNKNOWN;
	card_product_code[0] = 0;
	card_revision = 0xFF;
	card_tid = 0;
	card_icon = NULL;
	card_text.clear();
}

/**
 * Cache TWL game card banner data.
 */
static void gamecardCacheTWL(void)
{
	// Based on FBI 2.4.7's listtitles.c, task_populate_titles_add_twl()
	static_assert(sizeof(sNDSHeader) == 0x3B4, "sizeof(sNDSHeader) is not 0x3B4.");

	// Get the NDS ROM header.
	sNDSHeader header;
	if (R_FAILED(FSUSER_GetLegacyRomHeader(MEDIATYPE_GAME_CARD, 0, (u8*)&header))) {
		// Unable to read the ROM header.
		gamecardClearCache();
		return;
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
			card_type = CARD_TYPE_NTR;
			prefix = "NTR";
			break;
		case 0x02:
			card_type = CARD_TYPE_TWL_ENH;
			prefix = "TWL";
			break;
		case 0x03:
			card_type = CARD_TYPE_TWL_ONLY;
			prefix = "TWL";
			break;
	}

	// Product code. Format: NTR-P-XXXX or TWL-P-XXXX
	snprintf(card_product_code, sizeof(card_product_code), "%s-P-%.4s", prefix, twl_gameid.id4);

	// Revision.
	card_revision = header.romversion;

	// Title ID.
	if (header.dsi_tid != 0) {
		card_tid = (0x00030000ULL << 32) | header.dsi_tid;
	} else {
		// No title ID.
		card_tid = 0;
	}

	// Get the banner.
	sNDSBanner ndsBanner;
	if (R_FAILED(FSUSER_GetLegacyBannerData(MEDIATYPE_GAME_CARD, 0, (u8*)&ndsBanner))) {
		// Unable to obtain the banner.
		gamecardClearCache();
		return;
	}

	// Store the icon and banner text.
	sf2d_free_texture(card_icon);
	card_icon = grabIcon(&ndsBanner);
	card_text = grabText(&ndsBanner, language);
}

/**
 * Cache CTR game card banner data.
 */
static void gamecardCacheCTR(void)
{
	// Based on FBI 2.4.7's listtitles.c, task_populate_titles_add_ctr()
	card_type = CARD_TYPE_CTR;

	// We need to get the title ID from the cartridge.
	u32 titleCount = 0;
	if (R_FAILED(AM_GetTitleCount(MEDIATYPE_GAME_CARD, &titleCount)) || titleCount == 0) {
		return;
	}
	u64* titleIds = (u64*)calloc(titleCount, sizeof(u64));
	if (!titleIds) {
		return;
	}
	if (R_FAILED(AM_GetTitleList(&titleCount, MEDIATYPE_GAME_CARD, titleCount, titleIds))) {
		free(titleIds);
		return;
	}

	// First entry is the correct title ID.
	card_tid = titleIds[0];
	free(titleIds);

	// Get the title entry.
	AM_TitleEntry entry;
	if (R_FAILED(AM_GetTitleInfo(MEDIATYPE_GAME_CARD, 1, &card_tid, &entry))) {
		// Unable to read the title entry.
		return;
	}

	// Get the product code.
	if (R_FAILED(AM_GetTitleProductCode(MEDIATYPE_GAME_CARD, card_tid, card_product_code))) {
		// Unable to get the product code.
		gamecardClearCache();
		return;
	}
	// Make sure it's NULL-terminated.
	card_product_code[20-1] = 0;

	// Get the SMDH, which contains the icon and banner.
	static const u32 filePathData[] = {0x00000000, 0x00000000, 0x00000002, 0x6E6F6369, 0x00000000};
	static const FS_Path filePath = {PATH_BINARY, sizeof(filePathData), filePathData};
	const u32 archivePathData[4] = {
		(u32)(card_tid & 0xFFFFFFFFU),
		(u32)((card_tid >> 32) & 0xFFFFFFFFU),
		MEDIATYPE_GAME_CARD,
		0x00000000
	};
	const FS_Path archivePath = {PATH_BINARY, sizeof(archivePathData), archivePathData};

	Handle fileHandle;
	if (R_FAILED(FSUSER_OpenFileDirectly(&fileHandle, ARCHIVE_SAVEDATA_AND_CONTENT,
	    archivePath, filePath, FS_OPEN_READ, 0)))
	{
		// Could not open the SMDH file.
		gamecardClearCache();
		return;
	}

	static_assert(sizeof(SMDH) == 14016, "sizeof(SMDH) is not 14,016 bytes");
	SMDH *smdh = (SMDH*)calloc(1, sizeof(SMDH));
	if (!smdh) {
		// calloc() failed.
		gamecardClearCache();
		return;
	}

	u32 bytesRead = 0;
	Result res = FSFILE_Read(fileHandle, &bytesRead, 0, smdh, sizeof(SMDH));
	FSFILE_Close(fileHandle);

	if (R_FAILED(res) || bytesRead != sizeof(*smdh)) {
		// Either the read failed, or it was a short read.
		free(smdh);
		gamecardClearCache();
		return;
	}

	// Verify the SMDH magic.
	static const u8 smdh_magic[] = {'S','M','D','H'};
	if (memcmp(smdh->magic, smdh_magic, sizeof(smdh_magic)) != 0) {
		// Incorrect SMDH magic.
		free(smdh);
		gamecardClearCache();
		return;
	}

	// Check if the title for the selected language is valid.
	u8 lang = language;
	if (smdh->titles[lang].shortDescription[0] == 0) {
		// Not valid. Try English.
		lang = 1;
		if (smdh->titles[lang].shortDescription[0] == 0) {
			// Still not valid. Try Japanese.
			lang = 0;
			if (smdh->titles[lang].shortDescription[0] == 0) {
				// Invalid banner.
				free(smdh);
				gamecardClearCache();
			}
		}
	}

	// Check which description to use.
	if (smdh->titles[lang].longDescription[0]) {
		// Use the long description.
		card_text = utf16_nl_to_vwstring(smdh->titles[lang].longDescription, 128);
	} else {
		// Use the short description.
		card_text = utf16_nl_to_vwstring(smdh->titles[lang].shortDescription, 64);
	}

	// Add the publisher.
	// TODO: Make sure we don't have more than two lines here.
	smdh->titles[lang].publisher[0x40-1] = 0;
	card_text.push_back(utf16_to_wstring(smdh->titles[lang].publisher));
	free(smdh);

	// FIXME: Untile the large icon and load it.
	// TODO: Add RGB565 and RGB555 loading.
	//screen_load_texture_tiled(titleInfo->meta.texture, smdh->largeIcon, sizeof(smdh->largeIcon), 48, 48, GPU_RGB565, false);
	return;
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
	if (R_FAILED(FSUSER_CardSlotIsInserted(&inserted)) || !inserted) {
		// Card is not present.
		if (card_inserted || force) {
			gamecardClearCache();
			card_inserted = false;
			return true;
		}
		return false;
	}

	FS_CardType type;
	if (R_FAILED(FSUSER_GetCardType(&type))) {
		// Unable to get card type.
		// This may be a blocked flashcard.
		if (card_inserted || force) {
			gamecardClearCache();
			return true;
		}
		return false;
	}

	if (card_inserted && !force) {
		// No card change.
		return false;
	}

	// New card detected.
	gamecardClearCache();
	card_inserted = true;

	switch (type) {
		case CARD_TWL:
			// NTR/TWL game card.
			gamecardCacheTWL();
			break;
		case CARD_CTR:
			// CTR game card.
			gamecardCacheCTR();
			break;
		default:
			// Unsupported card type.
			gamecardClearCache();
			break;
	}

	return true;
}

/**
 * Is a game card inserted?
 * @return True if a game card is inserted.
 */
bool gamecardIsInserted(void)
{
	return card_inserted;
}

/**
 * Get the game card's type.
 * @return Game card type.
 */
GameCardType gamecardGetType(void)
{
	return card_type;
}

/**
 * Get the game card's game ID.
 * @return Game ID, or NULL if not a TWL card.
 */
const char *gamecardGetGameID(void)
{
	return (twl_gameid.d != 0 ? twl_gameid.id4 : NULL);
}

/**
 * Get the game card's game ID as a u32.
 * @return Game ID, or 0 if not a TWL card.
 */
u32 gamecardGetGameID_u32(void)
{
	return twl_gameid.d;
}

/**
 * Get the game card's product code.
 * @return Product code, or NULL if not a TWL card.
 */
const char *gamecardGetProductCode(void)
{
	return card_product_code;
}

/**
 * Get the game card's revision..
 * @return Game card revision. (0xFF if unknown.)
 */
u8 gamecardGetRevision(void)
{
	return card_revision;
}

/**
 * Get the game card's Title ID.
 * NOTE: Only applicable to TWL and CTR titles.
 * @return Title ID, or 0 if no card or the card doesn't have a title ID.
 */
u64 gamecardGetTitleID(void)
{
	return card_tid;
}

/**
 * Get the game card's icon.
 * @return Game card icon, or NULL if not a TWL card.
 */
sf2d_texture *gamecardGetIcon(void)
{
	return card_icon;
}

/**
 * Get the game card's banner text.
 * @return Game card banner text, or empty vector if not a TWL card.
 */
vector<wstring> gamecardGetText(void)
{
	return card_text;
}
