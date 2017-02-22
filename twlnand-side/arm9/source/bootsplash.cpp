/*
    NitroHax -- Cheat tool for the Nintendo DS
    Copyright (C) 2008  Michael "Chishm" Chisholm

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <nds.h>
#include <maxmod9.h>

#include "soundbank.h"
#include "soundbank_bin.h"
 
#include "bios_decompress_callback.h"

// English
#include "CartPrompt01.h"
#include "CartPrompt02.h"
#include "CartPrompt03.h"
#include "CartPrompt04.h"
// French
#include "CartPrompt01FR.h"
#include "CartPrompt02FR.h"
#include "CartPrompt03FR.h"
#include "CartPrompt04FR.h"
// Italian
#include "CartPrompt01IT.h"
#include "CartPrompt02IT.h"
#include "CartPrompt03IT.h"
#include "CartPrompt04IT.h"

#include "DSiCartPrompt01.h"
#include "DSiCartPrompt02.h"
#include "DSiCartPrompt03.h"
#include "DSiCartPrompt04.h"

#include "CartPromptNoHS01.h"
#include "CartPromptNoHS02.h"
#include "CartPromptNoHS03.h"
#include "CartPromptNoHS04.h"

#include "Bot00.h"
#include "Bot01.h"
// Japanese
#include "Bot02JA.h"
#include "Bot03JA.h"
#include "Bot04JA.h"
#include "Bot05JA.h"
// English
#include "Bot02.h"
#include "Bot03.h"
#include "Bot04.h"
#include "Bot05.h"
// French
#include "Bot02FR.h"
#include "Bot03FR.h"
#include "Bot04FR.h"
#include "Bot05FR.h"
// German
#include "Bot02DE.h"
#include "Bot03DE.h"
#include "Bot04DE.h"
#include "Bot05DE.h"
// Italian
#include "Bot02IT.h"
#include "Bot03IT.h"
#include "Bot04IT.h"
#include "Bot05IT.h"
// Spanish
#include "Bot02ES.h"
#include "Bot03ES.h"
#include "Bot04ES.h"
#include "Bot05ES.h"

#include "Bot09.h"
#include "Bot10.h"

#include "BotDSi00.h"
#include "BotDSi01.h"
#include "BotDSi02.h"
#include "BotDSi03.h"
#include "BotDSi04.h"
#include "BotDSi05.h"
#include "BotDSi06.h"
#include "BotDSi07.h"
#include "BotDSi08.h"
#include "BotDSi09.h"
#include "BotDSi10.h"
#include "BotDSi11.h"

#include "Top00.h"
#include "Top01.h"
#include "Top02.h"
#include "Top03.h"
#include "Top04.h"
#include "Top05.h"
#include "Top06.h"
#include "Top07.h"
#include "Top08.h"
#include "Top09.h"
#include "Top10.h"
#include "Top11.h"
#include "Top12.h"
#include "Top13.h"
#include "Top14.h"
#include "Top15.h"
#include "Top16.h"
#include "Top17.h"
#include "Top18.h"
#include "Top19.h"
#include "Top20.h"
#include "Top21.h"
#include "Top22.h"
#include "Top23.h"
#include "Top24.h"
#include "Top25.h"
#include "Top26.h"
#include "Top27.h"
#include "Top28.h"
#include "Top29.h"
#include "Top30.h"
#include "Top31.h"
#include "Top32.h"
#include "Top33.h"
#include "Top34.h"
#include "Top35.h"
#include "Top36.h"
#include "Top37.h"

#include "DSi01.h"
#include "DSi02.h"
#include "DSi03.h"
#include "DSi04.h"
#include "DSi05.h"
#include "DSi06.h"
#include "DSi07.h"
#include "DSi08.h"
#include "DSi09.h"
#include "DSi10.h"
#include "DSi11.h"
#include "DSi12.h"
#include "DSi13.h"
#include "DSi14.h"
#include "DSi15.h"
#include "DSi16.h"
#include "DSi17.h"
#include "DSi18.h"
#include "DSi19.h"
#include "DSi20.h"
#include "DSi21.h"
#include "DSi22.h"
#include "DSi23.h"
#include "DSi24.h"
#include "DSi25.h"
#include "DSi26.h"
#include "DSi27.h"
#include "DSi28.h"
#include "DSi29.h"
#include "DSi30.h"
#include "DSi31.h"
#include "DSi32.h"
#include "DSi33.h"
#include "DSi34.h"
#include "DSi35.h"

#include "inifile.h"
#include "bootsplash.h"

#define CONSOLE_SCREEN_WIDTH 32
#define CONSOLE_SCREEN_HEIGHT 24

void vramcpy_ui (void* dest, const void* src, int size) 
{
	u16* destination = (u16*)dest;
	u16* source = (u16*)src;
	while (size > 0) {
		*destination++ = *source++;
		size-=2;
	}
}

void BootJingle() {
	mmInitDefaultMem((mm_addr)soundbank_bin);
	
	mmLoadEffect( SFX_DSBOOT );

	mm_sound_effect dsboot = {
		{ SFX_DSBOOT } ,			// id
		(int)(1.0f * (1<<10)),	// rate
		0,		// handle
		255,	// volume
		128,	// panning
	};
	
	mmEffectEx(&dsboot);
}

void BootJingleDSi() {
	
	mmInitDefaultMem((mm_addr)soundbank_bin);

	mmLoadEffect( SFX_DSIBOOT );

	mm_sound_effect dsiboot = {
		{ SFX_DSIBOOT } ,			// id
		(int)(1.0f * (1<<10)),	// rate
		0,		// handle
		255,	// volume
		128,	// panning
	};
	
	mmEffectEx(&dsiboot);
}

void CartridgePrompt() {
	
	swiDecompressLZSSVram ((void*)Bot05Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], Bot05Pal, Bot05PalLen);

	for (int i = 0; i < 20; i++) { swiWaitForVBlank(); }
	
	swiDecompressLZSSVram ((void*)CartPrompt04Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPrompt04Pal, CartPrompt04PalLen);

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPrompt03Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPrompt03Pal, CartPrompt03PalLen);

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPrompt02Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPrompt02Pal, CartPrompt02PalLen);	

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPrompt01Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPrompt01Pal, CartPrompt01PalLen);	

	for (int i = 0; i < 40; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPrompt02Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPrompt02Pal, CartPrompt02PalLen);	

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPrompt03Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPrompt03Pal, CartPrompt03PalLen);

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPrompt04Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPrompt04Pal, CartPrompt04PalLen);

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)Bot05Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], Bot05Pal, Bot05PalLen);

	for (int i = 0; i < 20; i++) { swiWaitForVBlank(); }

}

void CartridgePromptFR() {
	
	swiDecompressLZSSVram ((void*)Bot05FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], Bot05FRPal, Bot05FRPalLen);

	for (int i = 0; i < 20; i++) { swiWaitForVBlank(); }
	
	swiDecompressLZSSVram ((void*)CartPrompt04FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPrompt04FRPal, CartPrompt04FRPalLen);

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPrompt03FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPrompt03FRPal, CartPrompt03FRPalLen);

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPrompt02FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPrompt02FRPal, CartPrompt02FRPalLen);	

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPrompt01FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPrompt01FRPal, CartPrompt01FRPalLen);	

	for (int i = 0; i < 40; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPrompt02FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPrompt02FRPal, CartPrompt02FRPalLen);	

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPrompt03FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPrompt03FRPal, CartPrompt03FRPalLen);

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPrompt04FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPrompt04FRPal, CartPrompt04FRPalLen);

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)Bot05FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], Bot05FRPal, Bot05PalLen);

	for (int i = 0; i < 20; i++) { swiWaitForVBlank(); }

}

void CartridgePromptIT() {
	
	swiDecompressLZSSVram ((void*)Bot05ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], Bot05ITPal, Bot05ITPalLen);

	for (int i = 0; i < 20; i++) { swiWaitForVBlank(); }
	
	swiDecompressLZSSVram ((void*)CartPrompt04ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPrompt04ITPal, CartPrompt04ITPalLen);

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPrompt03ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPrompt03ITPal, CartPrompt03ITPalLen);

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPrompt02ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPrompt02ITPal, CartPrompt02ITPalLen);	

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPrompt01ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPrompt01ITPal, CartPrompt01ITPalLen);	

	for (int i = 0; i < 40; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPrompt02ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPrompt02ITPal, CartPrompt02ITPalLen);	

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPrompt03ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPrompt03ITPal, CartPrompt03ITPalLen);

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPrompt04ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPrompt04ITPal, CartPrompt04ITPalLen);

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)Bot05ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], Bot05ITPal, Bot05ITPalLen);

	for (int i = 0; i < 20; i++) { swiWaitForVBlank(); }

}


void CartridgePromptDSi() {
	
	swiDecompressLZSSVram ((void*)BotDSi05Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], BotDSi05Pal, Bot05PalLen);

	for (int i = 0; i < 20; i++) { swiWaitForVBlank(); }
	
	swiDecompressLZSSVram ((void*)DSiCartPrompt04Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], DSiCartPrompt04Pal, DSiCartPrompt04PalLen);

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSiCartPrompt03Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], DSiCartPrompt03Pal, DSiCartPrompt03PalLen);

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSiCartPrompt02Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], DSiCartPrompt02Pal, DSiCartPrompt02PalLen);	

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSiCartPrompt01Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], DSiCartPrompt01Pal, DSiCartPrompt01PalLen);	

	for (int i = 0; i < 40; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSiCartPrompt02Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], DSiCartPrompt02Pal, DSiCartPrompt02PalLen);	

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSiCartPrompt03Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], DSiCartPrompt03Pal, DSiCartPrompt03PalLen);

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSiCartPrompt04Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], DSiCartPrompt04Pal, DSiCartPrompt04PalLen);

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)BotDSi05Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], BotDSi05Pal, BotDSi05PalLen);

	for (int i = 0; i < 20; i++) { swiWaitForVBlank(); }

}

void CartridgePromptNoHS() {
	
	swiDecompressLZSSVram ((void*)Bot00Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], Bot00Pal, Bot00PalLen);

	for (int i = 0; i < 20; i++) { swiWaitForVBlank(); }
	
	swiDecompressLZSSVram ((void*)CartPromptNoHS04Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPromptNoHS04Pal, CartPrompt04PalLen);

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPromptNoHS03Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPromptNoHS03Pal, CartPrompt03PalLen);

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPromptNoHS02Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPromptNoHS02Pal, CartPrompt02PalLen);	

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPromptNoHS01Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPromptNoHS01Pal, CartPrompt01PalLen);	

	for (int i = 0; i < 40; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPromptNoHS02Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPromptNoHS02Pal, CartPrompt02PalLen);	

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPromptNoHS03Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPromptNoHS03Pal, CartPrompt03PalLen);

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)CartPromptNoHS04Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], CartPromptNoHS04Pal, CartPrompt04PalLen);

	for (int i = 0; i < 4; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)Bot00Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE_SUB[0], Bot00Pal, Bot00PalLen);

	for (int i = 0; i < 20; i++) { swiWaitForVBlank(); }

}

void BootSplashDSi(bool HealthandSafety_MSG, int language) {

	CIniFile twloaderini( "sd:/_nds/twloader/settings.ini" );

	swiDecompressLZSSVram ((void*)DSi01Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	swiDecompressLZSSVram ((void*)BotDSi00Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi01Pal, DSi01PalLen);
	vramcpy_ui (&BG_PALETTE_SUB[0], BotDSi00Pal, BotDSi00PalLen);

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi01Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	swiDecompressLZSSVram ((void*)BotDSi00Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi02Pal, DSi02PalLen);
	switch (language) {
		case 0:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			vramcpy_ui (&BG_PALETTE_SUB[0], Bot01Pal, Bot01PalLen);
			break;
		case 1:
		default:
			vramcpy_ui (&BG_PALETTE_SUB[0], BotDSi01Pal, BotDSi01PalLen);
			break;
	}

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi01Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)BotDSi00Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
	vramcpy_ui (&BG_PALETTE[0], DSi02Pal, DSi02PalLen);
	switch (language) {
		case 0:
		case 2:
		case 3:
		case 4:
		case 5:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot01Pal, Bot01PalLen); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], BotDSi01Pal, BotDSi01PalLen); }
			break;
		case 6:
		case 7:
			break;
	}

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }
	
	swiDecompressLZSSVram ((void*)DSi01Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	switch (language) {
		case 0:
		case 2:
		case 3:
		case 4:
		case 5:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot01Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)BotDSi01Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 6:
		case 7:
			break;
	}
	vramcpy_ui (&BG_PALETTE[0], DSi02Pal, DSi02PalLen);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02JAPal, Bot02JAPalLen); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], BotDSi02Pal, BotDSi02PalLen); }
			break;
		case 2:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02FRPal, Bot02FRPalLen); }
			break;
		case 3:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02DEPal, Bot02DEPalLen); }
			break;
		case 4:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02ITPal, Bot02ITPalLen); }
			break;
		case 5:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02ESPal, Bot02ESPalLen); }
			break;
		case 6:
		case 7:
			break;
	}
	
	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }
	
	swiDecompressLZSSVram ((void*)DSi02Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02JATiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)BotDSi02Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 2:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 3:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02DETiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 4:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 5:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02ESTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 6:
		case 7:
			break;
	}
	vramcpy_ui (&BG_PALETTE[0], DSi03Pal, DSi03PalLen);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03JAPal, Bot03JAPalLen); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], BotDSi03Pal, BotDSi03PalLen); }
			break;
		case 2:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03FRPal, Bot03FRPalLen); }
			break;
		case 3:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03DEPal, Bot03DEPalLen); }
			break;
		case 4:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03ITPal, Bot03ITPalLen); }
			break;
		case 5:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03ESPal, Bot03ESPalLen); }
			break;
		case 6:
		case 7:
			break;
	}	
	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi03Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot03JATiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)BotDSi03Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 2:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot03FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 3:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot03DETiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 4:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot03ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 5:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot03ESTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 6:
		case 7:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot03JATiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
	}
	vramcpy_ui (&BG_PALETTE[0], DSi03Pal, DSi03PalLen);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03JAPal, Bot03JAPalLen); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], BotDSi03Pal, BotDSi03PalLen); }
			break;
		case 2:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03FRPal, Bot03FRPalLen); }
			break;
		case 3:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03DEPal, Bot03DEPalLen); }
			break;
		case 4:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03ITPal, Bot03ITPalLen); }
			break;
		case 5:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03ESPal, Bot03ESPalLen); }
			break;
		case 6:
		case 7:
			break;
	}	

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi04Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04JATiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)BotDSi04Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 2:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 3:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04DETiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 4:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 5:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04ESTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 6:
		case 7:
			break;
	}
	vramcpy_ui (&BG_PALETTE[0], DSi04Pal, DSi04PalLen);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04JAPal, Bot04JAPalLen); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], BotDSi04Pal, BotDSi04PalLen); }
			break;
		case 2:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04FRPal, Bot04FRPalLen); }
			break;
		case 3:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04DEPal, Bot04DEPalLen); }
			break;
		case 4:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04ITPal, Bot04ITPalLen); }
			break;
		case 5:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04ESPal, Bot04ESPalLen); }
			break;
		case 6:
		case 7:
			break;
	}	

	BootJingleDSi();

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi05Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot05JATiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)BotDSi05Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 2:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot05FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 3:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot05DETiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 4:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot05ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 5:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot05ESTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 6:
		case 7:
			break;
	}
	vramcpy_ui (&BG_PALETTE[0], DSi05Pal, DSi05PalLen);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot05JAPal, Bot05JAPalLen); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], BotDSi05Pal, BotDSi05PalLen); }
			break;
		case 2:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot05FRPal, Bot05FRPalLen); }
			break;
		case 3:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot05DEPal, Bot05DEPalLen); }
			break;
		case 4:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot05ITPal, Bot05ITPalLen); }
			break;
		case 5:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot05ESPal, Bot05ESPalLen); }
			break;
		case 6:
		case 7:
			break;
	}	

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }
	
	swiDecompressLZSSVram ((void*)DSi06Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi06Pal, DSi06PalLen);
		
	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi07Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi07Pal, DSi07PalLen);

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi08Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi08Pal, DSi08PalLen);

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }
	
	swiDecompressLZSSVram ((void*)DSi09Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi09Pal, DSi09PalLen);

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi10Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi10Pal, DSi10PalLen);

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi11Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi11Pal, DSi11PalLen);

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi12Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi12Pal, DSi12PalLen);
	
	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi13Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi13Pal, DSi13PalLen);

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi14Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi14Pal, DSi14PalLen);

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi15Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi15Pal, DSi15PalLen);

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi16Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi16Pal, DSi16PalLen);

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi17Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi17Pal, DSi17PalLen);

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }
	
	swiDecompressLZSSVram ((void*)DSi18Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi18Pal, DSi18PalLen);

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi19Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi19Pal, DSi19PalLen);

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi20Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi20Pal, DSi20PalLen);

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi21Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi21Pal, DSi21PalLen);

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi22Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi22Pal, DSi22PalLen);

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi23Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi23Pal, DSi23PalLen);

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi24Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi24Pal, DSi24PalLen);

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi25Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi25Pal, DSi25PalLen);

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi26Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi26Pal, DSi26PalLen);

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi27Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi27Pal, DSi27PalLen);

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi28Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi28Pal, DSi28PalLen);

	if(twloaderini.GetInt("TWL-MODE","LAUNCH_SLOT1",0) == 1) {
		// Display Cartridge Prompt animation until cartridge inserted. (skipped if one already inserted)
		if(REG_SCFG_MC == 0x11) { 
			if(HealthandSafety_MSG) {
				do {
					switch (language) {
						case 0:
							CartridgePromptNoHS();
							break;
						case 1:
						default:
							CartridgePromptDSi();
							break;
						case 2:
							CartridgePromptFR();
							break;
						case 3:
							CartridgePromptNoHS();
							break;
						case 4:
							CartridgePromptIT();
							break;
						case 5:
							CartridgePromptNoHS();
							break;
						case 6:
						case 7:
							CartridgePromptNoHS();
							break;
					}
				}
				while (REG_SCFG_MC == 0x11);
			} else {
				do { CartridgePromptNoHS(); } 
				while (REG_SCFG_MC == 0x11);
			}
		}
	}

	// Pause on frame 31 for a second		
	for (int i = 0; i < 80; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi29Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04JATiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)BotDSi06Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 2:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 3:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04DETiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 4:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 5:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04ESTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 6:
		case 7:
			break;
	}
	vramcpy_ui (&BG_PALETTE[0], DSi29Pal, DSi29PalLen);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04JAPal, Bot04JAPalLen); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], BotDSi06Pal, BotDSi06PalLen); }
			break;
		case 2:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04FRPal, Bot04FRPalLen); }
			break;
		case 3:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04DEPal, Bot04DEPalLen); }
			break;
		case 4:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04ITPal, Bot04ITPalLen); }
			break;
		case 5:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04ESPal, Bot04ESPalLen); }
			break;
		case 6:
		case 7:
			break;
	}	

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi30Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot03JATiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)BotDSi07Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 2:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot03FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 3:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot03DETiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 4:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot03ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 5:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot03ESTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 6:
		case 7:
			break;
	}
	vramcpy_ui (&BG_PALETTE[0], DSi30Pal, DSi30PalLen);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03JAPal, Bot03JAPalLen); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], BotDSi07Pal, BotDSi07PalLen); }
			break;
		case 2:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03FRPal, Bot03FRPalLen); }
			break;
		case 3:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03DEPal, Bot03DEPalLen); }
			break;
		case 4:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03ITPal, Bot03ITPalLen); }
			break;
		case 5:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03ESPal, Bot03ESPalLen); }
			break;
		case 6:
		case 7:
			break;
	}	

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi31Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02JATiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)BotDSi08Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 2:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 3:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02DETiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 4:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 5:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02ESTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 6:
		case 7:
			break;
	}
	vramcpy_ui (&BG_PALETTE[0], DSi31Pal, DSi31PalLen);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02JAPal, Bot02JAPalLen); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], BotDSi08Pal, BotDSi08PalLen); }
			break;
		case 2:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02FRPal, Bot02FRPalLen); }
			break;
		case 3:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02DEPal, Bot02DEPalLen); }
			break;
		case 4:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02ITPal, Bot02ITPalLen); }
			break;
		case 5:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02ESPal, Bot02ESPalLen); }
			break;
		case 6:
		case 7:
			break;
	}	

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }
	
	swiDecompressLZSSVram ((void*)DSi32Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	switch (language) {
		case 0:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot01Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)BotDSi09Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
	}
	vramcpy_ui (&BG_PALETTE[0], DSi32Pal, DSi32PalLen);
	switch (language) {
		case 0:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot01Pal, Bot01PalLen); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], BotDSi09Pal, BotDSi09PalLen); }
			break;
	}

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }
	
	swiDecompressLZSSVram ((void*)DSi33Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	switch (language) {
		case 0:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot00Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)BotDSi10Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
	}
	vramcpy_ui (&BG_PALETTE[0], DSi33Pal, DSi33PalLen);
	switch (language) {
		case 0:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot00Pal, Bot00PalLen); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], BotDSi10Pal, BotDSi10PalLen); }
			break;
	}

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }	

	swiDecompressLZSSVram ((void*)DSi34Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	switch (language) {
		case 0:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)BotDSi11Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
	}
	vramcpy_ui (&BG_PALETTE[0], DSi34Pal, DSi34PalLen);
	switch (language) {
		case 0:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], BotDSi11Pal, BotDSi11PalLen); }
			break;
	}

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)DSi35Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)BotDSi00Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
	vramcpy_ui (&BG_PALETTE[0], DSi35Pal, DSi35PalLen);
	if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], BotDSi00Pal, BotDSi00PalLen); }

	for (int i = 0; i < 2; i++) { swiWaitForVBlank(); }	

	swiDecompressLZSSVram ((void*)DSi01Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], DSi01Pal, DSi01PalLen);
}

void BootSplashDS(bool HealthandSafety_MSG, int language) {

	CIniFile twloaderini( "sd:/_nds/twloader/settings.ini" );

	// offsetting palletes by one frame during the fade in seems to fix black flicker at start.	
	// only did this for about 5 frames. (time it takes for bottom screen to fade in)
	swiDecompressLZSSVram ((void*)Top00Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	swiDecompressLZSSVram ((void*)Bot00Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top01Pal, Top01PalLen);
	vramcpy_ui (&BG_PALETTE_SUB[0], Bot01Pal, Bot01PalLen);


	swiDecompressLZSSVram ((void*)Top00Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	swiDecompressLZSSVram ((void*)Bot00Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top00Pal, Top00PalLen);
	vramcpy_ui (&BG_PALETTE_SUB[0], Bot00Pal, Bot00PalLen);


	swiDecompressLZSSVram ((void*)Top00Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	swiDecompressLZSSVram ((void*)Bot00Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top00Pal, Top00PalLen);
	vramcpy_ui (&BG_PALETTE_SUB[0], Bot00Pal, Bot00PalLen);

	
	swiDecompressLZSSVram ((void*)Top01Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot01Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
	vramcpy_ui (&BG_PALETTE[0], Top02Pal, Top02PalLen);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02JAPal, Bot02JAPalLen); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02Pal, Bot02PalLen); }
			break;
		case 2:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02FRPal, Bot02FRPalLen); }
			break;
		case 3:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02DEPal, Bot02DEPalLen); }
			break;
		case 4:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02ITPal, Bot02ITPalLen); }
			break;
		case 5:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02ESPal, Bot02ESPalLen); }
			break;
		case 6:
		case 7:
			break;
	}
	
	
	swiDecompressLZSSVram ((void*)Top02Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02JATiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 2:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 3:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02DETiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 4:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 5:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02ESTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 6:
		case 7:
			break;
	}
	vramcpy_ui (&BG_PALETTE[0], Top03Pal, Top03PalLen);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03JAPal, Bot03JAPalLen); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03Pal, Bot03PalLen); }
			break;
		case 2:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03FRPal, Bot03FRPalLen); }
			break;
		case 3:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03DEPal, Bot03DEPalLen); }
			break;
		case 4:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03ITPal, Bot03ITPalLen); }
			break;
		case 5:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03ESPal, Bot03ESPalLen); }
			break;
		case 6:
		case 7:
			break;
	}
	

	swiDecompressLZSSVram ((void*)Top03Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot03JATiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot03Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 2:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03FRPal, Bot03FRPalLen); }
			break;
		case 3:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03DEPal, Bot03DEPalLen); }
			break;
		case 4:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03ITPal, Bot03ITPalLen); }
			break;
		case 5:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03ESPal, Bot03ESPalLen); }
			break;
		case 6:
		case 7:
			break;
	}
	vramcpy_ui (&BG_PALETTE[0], Top04Pal, Top04PalLen);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04JAPal, Bot04JAPalLen); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04Pal, Bot04PalLen); }
			break;
		case 2:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04FRPal, Bot04FRPalLen); }
			break;
		case 3:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04DEPal, Bot04DEPalLen); }
			break;
		case 4:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04ITPal, Bot04ITPalLen); }
			break;
		case 5:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04ESPal, Bot04ESPalLen); }
			break;
		case 6:
		case 7:
			break;
	}


	swiDecompressLZSSVram ((void*)Top04Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04JATiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 2:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 3:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04DETiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 4:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 5:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04ESTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 6:
		case 7:
			break;
	}
	vramcpy_ui (&BG_PALETTE[0], Top05Pal, Top05PalLen);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04JAPal, Bot04JAPalLen); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04Pal, Bot04PalLen); }
			break;
		case 2:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04FRPal, Bot04FRPalLen); }
			break;
		case 3:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04DEPal, Bot04DEPalLen); }
			break;
		case 4:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04ITPal, Bot04ITPalLen); }
			break;
		case 5:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04ESPal, Bot04ESPalLen); }
			break;
		case 6:
		case 7:
			break;
	}


	swiDecompressLZSSVram ((void*)Top05Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot05JATiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot05Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 2:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot05FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 3:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot05DETiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 4:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot05ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 5:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot05ESTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 6:
		case 7:
			break;
	}
	vramcpy_ui (&BG_PALETTE[0], Top05Pal, Top05PalLen);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot05JAPal, Bot05JAPalLen); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot05Pal, Bot05PalLen); }
			break;
		case 2:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot05FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 3:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot05DETiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 4:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot05ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 5:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot05ESTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 6:
		case 7:
			break;
	}


	BootJingle();
	
	swiDecompressLZSSVram ((void*)Top06Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top06Pal, Top06PalLen);


	swiDecompressLZSSVram ((void*)Top07Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top07Pal, Top07PalLen);


	swiDecompressLZSSVram ((void*)Top08Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top08Pal, Top08PalLen);

	
	swiDecompressLZSSVram ((void*)Top09Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top09Pal, Top09PalLen);


	swiDecompressLZSSVram ((void*)Top10Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top10Pal, Top10PalLen);


	swiDecompressLZSSVram ((void*)Top11Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top11Pal, Top11PalLen);


	swiDecompressLZSSVram ((void*)Top12Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top12Pal, Top12PalLen);
	

	swiDecompressLZSSVram ((void*)Top13Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top13Pal, Top13PalLen);


	swiDecompressLZSSVram ((void*)Top14Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top14Pal, Top14PalLen);


	swiDecompressLZSSVram ((void*)Top15Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top15Pal, Top15PalLen);


	swiDecompressLZSSVram ((void*)Top16Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top16Pal, Top16PalLen);


	swiDecompressLZSSVram ((void*)Top17Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top17Pal, Top17PalLen);

	
	swiDecompressLZSSVram ((void*)Top18Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top18Pal, Top18PalLen);


	swiDecompressLZSSVram ((void*)Top19Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top19Pal, Top19PalLen);


	swiDecompressLZSSVram ((void*)Top20Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top20Pal, Top20PalLen);


	swiDecompressLZSSVram ((void*)Top21Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top21Pal, Top21PalLen);


	swiDecompressLZSSVram ((void*)Top22Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top22Pal, Top22PalLen);


	swiDecompressLZSSVram ((void*)Top23Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top23Pal, Top23PalLen);


	swiDecompressLZSSVram ((void*)Top24Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top24Pal, Top24PalLen);


	swiDecompressLZSSVram ((void*)Top25Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top25Pal, Top25PalLen);


	swiDecompressLZSSVram ((void*)Top26Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top26Pal, Top26PalLen);


	swiDecompressLZSSVram ((void*)Top27Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top27Pal, Top27PalLen);


	swiDecompressLZSSVram ((void*)Top28Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top28Pal, Top28PalLen);


	swiDecompressLZSSVram ((void*)Top29Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top29Pal, Top29PalLen);


	swiDecompressLZSSVram ((void*)Top30Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top30Pal, Top30PalLen);


	swiDecompressLZSSVram ((void*)Top31Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top31Pal, Top31PalLen);

	if(twloaderini.GetInt("TWL-MODE","LAUNCH_SLOT1",0) == 1) {
		// Display Cartridge Prompt animation until cartridge inserted. (skipped if one already inserted)
		if(REG_SCFG_MC == 0x11) { 
			if(HealthandSafety_MSG) {
				do {
					switch (language) {
						case 0:
							CartridgePromptNoHS();
							break;
						case 1:
						default:
							CartridgePrompt();
							break;
						case 2:
							CartridgePromptFR();
							break;
						case 3:
							CartridgePromptNoHS();
							break;
						case 4:
							CartridgePromptIT();
							break;
						case 5:
							CartridgePromptNoHS();
							break;
						case 6:
						case 7:
							CartridgePromptNoHS();
							break;
					}
				}
				while (REG_SCFG_MC == 0x11);
			} else {
				do { CartridgePromptNoHS(); } 
				while (REG_SCFG_MC == 0x11);
			}
		}
	}

	// Pause on frame 31 for a second		
	for (int i = 0; i < 80; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)Top32Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04JATiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 2:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 3:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04DETiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 4:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 5:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot04ESTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 6:
		case 7:
			break;
	}
	vramcpy_ui (&BG_PALETTE[0], Top32Pal, Top32PalLen);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04JAPal, Bot04JAPalLen); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04Pal, Bot04PalLen); }
			break;
		case 2:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04FRPal, Bot04FRPalLen); }
			break;
		case 3:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04DEPal, Bot04DEPalLen); }
			break;
		case 4:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04ITPal, Bot04ITPalLen); }
			break;
		case 5:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot04ESPal, Bot04ESPalLen); }
			break;
		case 6:
		case 7:
			break;
	}

	
	swiDecompressLZSSVram ((void*)Top33Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot03JATiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot03Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 2:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot03FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 3:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot03DETiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 4:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot03ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 5:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot03ESTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 6:
		case 7:
			break;
	}
	vramcpy_ui (&BG_PALETTE[0], Top33Pal, Top33PalLen);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03JAPal, Bot03JAPalLen); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03Pal, Bot03PalLen); }
			break;
		case 2:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03FRPal, Bot03FRPalLen); }
			break;
		case 3:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03DEPal, Bot03DEPalLen); }
			break;
		case 4:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03ITPal, Bot03ITPalLen); }
			break;
		case 5:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot03ESPal, Bot03ESPalLen); }
			break;
		case 6:
		case 7:
			break;
	}


	swiDecompressLZSSVram ((void*)Top34Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02JATiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 2:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02FRTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 3:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02DETiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 4:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02ITTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 5:
			if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot02ESTiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
			break;
		case 6:
		case 7:
			break;
	}
	vramcpy_ui (&BG_PALETTE[0], Top34Pal, Top34PalLen);
	switch (language) {
		case 0:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02JAPal, Bot02JAPalLen); }
			break;
		case 1:
		default:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02Pal, Bot02PalLen); }
			break;
		case 2:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02FRPal, Bot02FRPalLen); }
			break;
		case 3:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02DEPal, Bot02DEPalLen); }
			break;
		case 4:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02ITPal, Bot02ITPalLen); }
			break;
		case 5:
			if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot02ESPal, Bot02ESPalLen); }
			break;
		case 6:
		case 7:
			break;
	}


	swiDecompressLZSSVram ((void*)Top35Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot09Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
	vramcpy_ui (&BG_PALETTE[0], Top35Pal, Top35PalLen);
	if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot09Pal, Bot09PalLen); }


	swiDecompressLZSSVram ((void*)Top36Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot10Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
	vramcpy_ui (&BG_PALETTE[0], Top36Pal, Top36PalLen);
	if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot10Pal, Bot10PalLen); }


	swiDecompressLZSSVram ((void*)Top37Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top37Pal, Top37PalLen);

		
	swiWaitForVBlank();

}

void BootSplashInit(bool UseNTRSplash, bool HealthandSafety_MSG, int language) {

	videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);
	videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);
	vramSetBankA (VRAM_A_MAIN_BG_0x06000000);
	vramSetBankC (VRAM_C_SUB_BG_0x06200000);
	REG_BG0CNT = BG_MAP_BASE(0) | BG_COLOR_256 | BG_TILE_BASE(2);
	REG_BG0CNT_SUB = BG_MAP_BASE(0) | BG_COLOR_256 | BG_TILE_BASE(2);
	BG_PALETTE[0]=0;
	BG_PALETTE[255]=0xffff;
	u16* bgMapTop = (u16*)SCREEN_BASE_BLOCK(0);
	u16* bgMapSub = (u16*)SCREEN_BASE_BLOCK_SUB(0);
	for (int i = 0; i < CONSOLE_SCREEN_WIDTH*CONSOLE_SCREEN_HEIGHT; i++) {
		bgMapTop[i] = (u16)i;
		bgMapSub[i] = (u16)i;
	}
	
	if( UseNTRSplash ) {
		BootSplashDS(HealthandSafety_MSG, language);
	} else {
		BootSplashDSi(HealthandSafety_MSG, language);
	}
}

