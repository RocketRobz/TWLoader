// Game card functions.
#ifndef TWLOADER_GAMECARD_H
#define TWLOADER_GAMECARD_H

#include <sf2d.h>

#include <vector>
#include <string>

/**
 * Poll for a game card change.
 * @param force If true, force a re-cache. (Use if polling hasn't been done in a while.)
 * @return True if the card has changed; false if not.
 */
bool gamecardPoll(bool force);

/**
 * Clear the cached icon and text.
 */
void gamecardClearCache(void);

/**
 * Get the game card's product code.
 * @return Product code, or NULL if not a TWL card.
 */
const char *gamecardGetProductCode(void);

/**
 * Get the game card's revision..
 * @return Game card revision. (0xFF if unknown.)
 */
u8 gamecardGetRevision(void);

/**
 * Get the game card's icon.
 * @return Game card icon, or NULL if not a TWL card.
 */
sf2d_texture *gamecardGetIcon(void);

/**
 * Get the game card's banner text.
 * @return Game card banner text, or empty vector if not a TWL card.
 */
std::vector<std::wstring> gamecardGetText(void);

#endif /* TWLOADER_GAMECARD_H */
