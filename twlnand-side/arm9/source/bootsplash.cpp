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

#include "white.h"
#include "Top001.h"
#include "Top002.h"
#include "Top003.h"
#include "Top004.h"
#include "Top005.h"
#include "Top006.h"
#include "Top007.h"
#include "Top008.h"
#include "Top009.h"
#include "Top010.h"
#include "Top011.h"
#include "Top012.h"
#include "Top013.h"
#include "Top014.h"
#include "Top015.h"
#include "Top016.h"
#include "Top017.h"
#include "Top018.h"
#include "Top019.h"
#include "Top020.h"
#include "Top021.h"
#include "Top022.h"
#include "Top023.h"
#include "Top024.h"
#include "Top025.h"
#include "Top026.h"
#include "Top027.h"
#include "Top028.h"
#include "Top029.h"
#include "Top030.h"
#include "Top031.h"
#include "Top032.h"
#include "Top033.h"
#include "Top034.h"
#include "Top035.h"
#include "Top036.h"
#include "Top037.h"
#include "Top038.h"
#include "Top039.h"
#include "Top040.h"
#include "Top041.h"
#include "Top042.h"
#include "Top043.h"
#include "Top044.h"
#include "Top045.h"
#include "Top046.h"
#include "Top047.h"
#include "Top048.h"
#include "Top049.h"
#include "Top050.h"
#include "Top051.h"
#include "Top052.h"
#include "Top053.h"
#include "Top054.h"
#include "Top055.h"
#include "Top056.h"
#include "Top057.h"
#include "Top058.h"
#include "Top059.h"
#include "Top060.h"
#include "Top061.h"
#include "Top062.h"
#include "Top063.h"
#include "Top064.h"
#include "Top065.h"
#include "Top066.h"
#include "Top067.h"
#include "Top068.h"
#include "Top069.h"
#include "Top070.h"
#include "Top071.h"
#include "Top072.h"
#include "Top073.h"
#include "Top074.h"
#include "Top075.h"
#include "Top076.h"
#include "Top077.h"
#include "Top078.h"
#include "Top079.h"
#include "Top080.h"
#include "Top081.h"
#include "Top082.h"
#include "Top083.h"
#include "Top084.h"
#include "Top085.h"
#include "Top086.h"
#include "Top087.h"
#include "Top088.h"
#include "Top089.h"
#include "Top090.h"
#include "Top091.h"
#include "Top092.h"
#include "Top093.h"
#include "Top094.h"
#include "Top095.h"
#include "Top096.h"
#include "Top097.h"
#include "Top098.h"
#include "Top099.h"
#include "Top100.h"
#include "Top101.h"
#include "Top102.h"
#include "Top103.h"
#include "Top104.h"
#include "Top105.h"
#include "Top106.h"
#include "Top107.h"
#include "Top108.h"
#include "Top109.h"
#include "Top110.h"
#include "Top111.h"
#include "Top112.h"
#include "Top113.h"
#include "Top114.h"
#include "Top115.h"
#include "Top116.h"
#include "Top117.h"
#include "Top118.h"
#include "Top119.h"
#include "Top120.h"
#include "Top121.h"
#include "Top122.h"
#include "Top123.h"
#include "Top124.h"


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
	swiDecompressLZSSVram ((void*)whiteTiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	swiDecompressLZSSVram ((void*)Bot00Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top001Pal, Top001PalLen);
	vramcpy_ui (&BG_PALETTE_SUB[0], Bot01Pal, Bot01PalLen);


	swiDecompressLZSSVram ((void*)whiteTiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	swiDecompressLZSSVram ((void*)Bot00Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], whitePal, whitePalLen);
	vramcpy_ui (&BG_PALETTE_SUB[0], Bot00Pal, Bot00PalLen);


	swiDecompressLZSSVram ((void*)whiteTiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	swiDecompressLZSSVram ((void*)Bot00Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], whitePal, whitePalLen);
	vramcpy_ui (&BG_PALETTE_SUB[0], Bot00Pal, Bot00PalLen);

	
	swiDecompressLZSSVram ((void*)Top001Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot01Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
	vramcpy_ui (&BG_PALETTE[0], Top002Pal, Top002PalLen);
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
	
	
	swiDecompressLZSSVram ((void*)Top002Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
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
	vramcpy_ui (&BG_PALETTE[0], Top003Pal, Top003PalLen);
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
	

	swiDecompressLZSSVram ((void*)Top003Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
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
	vramcpy_ui (&BG_PALETTE[0], Top004Pal, Top004PalLen);
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


	swiDecompressLZSSVram ((void*)Top004Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
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
	vramcpy_ui (&BG_PALETTE[0], Top005Pal, Top005PalLen);
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


	swiDecompressLZSSVram ((void*)Top005Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
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
	vramcpy_ui (&BG_PALETTE[0], Top005Pal, Top005PalLen);
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


	
	swiDecompressLZSSVram ((void*)Top006Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top006Pal, Top006PalLen);

	swiDecompressLZSSVram ((void*)Top007Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top007Pal, Top007PalLen);

	swiDecompressLZSSVram ((void*)Top008Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top008Pal, Top008PalLen);

	swiDecompressLZSSVram ((void*)Top009Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top009Pal, Top009PalLen);

	swiDecompressLZSSVram ((void*)Top010Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top010Pal, Top010PalLen);

	swiDecompressLZSSVram ((void*)Top011Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top011Pal, Top011PalLen);

	swiDecompressLZSSVram ((void*)Top012Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top012Pal, Top012PalLen);
	
	swiDecompressLZSSVram ((void*)Top013Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top013Pal, Top013PalLen);

	swiDecompressLZSSVram ((void*)Top014Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top014Pal, Top014PalLen);

	swiDecompressLZSSVram ((void*)Top015Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top015Pal, Top015PalLen);

	swiDecompressLZSSVram ((void*)Top016Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top016Pal, Top016PalLen);

	swiDecompressLZSSVram ((void*)Top017Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top017Pal, Top017PalLen);

	swiDecompressLZSSVram ((void*)Top018Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top018Pal, Top018PalLen);

	swiDecompressLZSSVram ((void*)Top019Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top019Pal, Top019PalLen);

	swiDecompressLZSSVram ((void*)Top020Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top020Pal, Top020PalLen);

	swiDecompressLZSSVram ((void*)Top021Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top021Pal, Top021PalLen);

	swiDecompressLZSSVram ((void*)Top022Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top022Pal, Top022PalLen);

	swiDecompressLZSSVram ((void*)Top023Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top023Pal, Top023PalLen);

	swiDecompressLZSSVram ((void*)Top024Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top024Pal, Top024PalLen);

	swiDecompressLZSSVram ((void*)Top025Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top025Pal, Top025PalLen);

	BootJingle();
	swiDecompressLZSSVram ((void*)Top026Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top026Pal, Top026PalLen);

	swiDecompressLZSSVram ((void*)Top027Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top027Pal, Top027PalLen);

	swiDecompressLZSSVram ((void*)Top028Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top028Pal, Top028PalLen);

	swiDecompressLZSSVram ((void*)Top029Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top029Pal, Top029PalLen);

	swiDecompressLZSSVram ((void*)Top030Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top030Pal, Top030PalLen);

	swiDecompressLZSSVram ((void*)Top031Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top031Pal, Top031PalLen);

	swiDecompressLZSSVram ((void*)Top032Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top032Pal, Top032PalLen);

	swiDecompressLZSSVram ((void*)Top033Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top033Pal, Top033PalLen);

	swiDecompressLZSSVram ((void*)Top034Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top034Pal, Top034PalLen);

	swiDecompressLZSSVram ((void*)Top035Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top035Pal, Top035PalLen);

	swiDecompressLZSSVram ((void*)Top036Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top036Pal, Top036PalLen);

	swiDecompressLZSSVram ((void*)Top037Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top037Pal, Top037PalLen);

	swiDecompressLZSSVram ((void*)Top038Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top038Pal, Top038PalLen);

	swiDecompressLZSSVram ((void*)Top039Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top039Pal, Top039PalLen);

	swiDecompressLZSSVram ((void*)Top040Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top040Pal, Top040PalLen);

	swiDecompressLZSSVram ((void*)Top041Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top041Pal, Top041PalLen);

	swiDecompressLZSSVram ((void*)Top042Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top042Pal, Top042PalLen);
	
	swiDecompressLZSSVram ((void*)Top043Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top043Pal, Top043PalLen);

	swiDecompressLZSSVram ((void*)Top044Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top044Pal, Top044PalLen);

	swiDecompressLZSSVram ((void*)Top045Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top045Pal, Top045PalLen);

	swiDecompressLZSSVram ((void*)Top046Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top046Pal, Top046PalLen);

	swiDecompressLZSSVram ((void*)Top047Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top047Pal, Top047PalLen);

	swiDecompressLZSSVram ((void*)Top048Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top048Pal, Top048PalLen);

	swiDecompressLZSSVram ((void*)Top049Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top049Pal, Top049PalLen);

	swiDecompressLZSSVram ((void*)Top050Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top050Pal, Top050PalLen);

	swiDecompressLZSSVram ((void*)Top051Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top051Pal, Top051PalLen);

	swiDecompressLZSSVram ((void*)Top052Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top052Pal, Top052PalLen);

	swiDecompressLZSSVram ((void*)Top053Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top053Pal, Top053PalLen);

	swiDecompressLZSSVram ((void*)Top054Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top054Pal, Top054PalLen);

	swiDecompressLZSSVram ((void*)Top055Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top055Pal, Top055PalLen);

	swiDecompressLZSSVram ((void*)Top056Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top056Pal, Top056PalLen);

	swiDecompressLZSSVram ((void*)Top057Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top057Pal, Top057PalLen);

	swiDecompressLZSSVram ((void*)Top058Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top058Pal, Top058PalLen);

	swiDecompressLZSSVram ((void*)Top059Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top059Pal, Top059PalLen);

	swiDecompressLZSSVram ((void*)Top060Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top060Pal, Top060PalLen);

	swiDecompressLZSSVram ((void*)Top061Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top061Pal, Top061PalLen);

	swiDecompressLZSSVram ((void*)Top062Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top062Pal, Top062PalLen);

	swiDecompressLZSSVram ((void*)Top063Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top063Pal, Top063PalLen);

	swiDecompressLZSSVram ((void*)Top064Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top064Pal, Top064PalLen);

	swiDecompressLZSSVram ((void*)Top065Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top065Pal, Top065PalLen);

	swiDecompressLZSSVram ((void*)Top066Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top066Pal, Top066PalLen);

	swiDecompressLZSSVram ((void*)Top067Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top067Pal, Top067PalLen);

	swiDecompressLZSSVram ((void*)Top068Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top068Pal, Top068PalLen);

	swiDecompressLZSSVram ((void*)Top069Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top069Pal, Top069PalLen);

	swiDecompressLZSSVram ((void*)Top070Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top070Pal, Top070PalLen);

	swiDecompressLZSSVram ((void*)Top071Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top071Pal, Top071PalLen);

	swiDecompressLZSSVram ((void*)Top072Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top072Pal, Top072PalLen);

	swiDecompressLZSSVram ((void*)Top073Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top073Pal, Top073PalLen);

	swiDecompressLZSSVram ((void*)Top074Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top074Pal, Top074PalLen);

	swiDecompressLZSSVram ((void*)Top075Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top075Pal, Top075PalLen);

	swiDecompressLZSSVram ((void*)Top076Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top076Pal, Top076PalLen);

	swiDecompressLZSSVram ((void*)Top077Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top077Pal, Top077PalLen);

	swiDecompressLZSSVram ((void*)Top078Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top078Pal, Top078PalLen);

	swiDecompressLZSSVram ((void*)Top079Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top079Pal, Top079PalLen);

	swiDecompressLZSSVram ((void*)Top080Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top080Pal, Top080PalLen);

	swiDecompressLZSSVram ((void*)Top081Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top081Pal, Top081PalLen);

	swiDecompressLZSSVram ((void*)Top082Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top082Pal, Top082PalLen);

	swiDecompressLZSSVram ((void*)Top083Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top083Pal, Top083PalLen);

	swiDecompressLZSSVram ((void*)Top084Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top084Pal, Top084PalLen);

	swiDecompressLZSSVram ((void*)Top085Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top085Pal, Top085PalLen);

	swiDecompressLZSSVram ((void*)Top086Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top086Pal, Top086PalLen);

	swiDecompressLZSSVram ((void*)Top087Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top087Pal, Top087PalLen);

	swiDecompressLZSSVram ((void*)Top088Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top088Pal, Top088PalLen);

	swiDecompressLZSSVram ((void*)Top089Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top089Pal, Top089PalLen);

	swiDecompressLZSSVram ((void*)Top090Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top090Pal, Top090PalLen);

	swiDecompressLZSSVram ((void*)Top091Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top091Pal, Top091PalLen);

	swiDecompressLZSSVram ((void*)Top092Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top092Pal, Top092PalLen);

	swiDecompressLZSSVram ((void*)Top093Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top093Pal, Top093PalLen);

	swiDecompressLZSSVram ((void*)Top094Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top094Pal, Top094PalLen);

	swiDecompressLZSSVram ((void*)Top095Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top095Pal, Top095PalLen);

	swiDecompressLZSSVram ((void*)Top096Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top096Pal, Top096PalLen);

	swiDecompressLZSSVram ((void*)Top097Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top097Pal, Top097PalLen);

	swiDecompressLZSSVram ((void*)Top098Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top098Pal, Top098PalLen);

	swiDecompressLZSSVram ((void*)Top099Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top099Pal, Top099PalLen);

	swiDecompressLZSSVram ((void*)Top100Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top100Pal, Top100PalLen);

	swiDecompressLZSSVram ((void*)Top101Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top101Pal, Top101PalLen);

	swiDecompressLZSSVram ((void*)Top102Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top102Pal, Top102PalLen);

	swiDecompressLZSSVram ((void*)Top103Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top103Pal, Top103PalLen);

	swiDecompressLZSSVram ((void*)Top104Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top104Pal, Top104PalLen);

	swiDecompressLZSSVram ((void*)Top105Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top105Pal, Top105PalLen);

	swiDecompressLZSSVram ((void*)Top106Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top106Pal, Top106PalLen);

	swiDecompressLZSSVram ((void*)Top107Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top107Pal, Top107PalLen);

	swiDecompressLZSSVram ((void*)Top108Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top108Pal, Top108PalLen);

	swiDecompressLZSSVram ((void*)Top109Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top109Pal, Top109PalLen);

	swiDecompressLZSSVram ((void*)Top110Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top110Pal, Top110PalLen);
	
	swiDecompressLZSSVram ((void*)Top111Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top111Pal, Top111PalLen);

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

	// Pause on frame 111 for a second		
	for (int i = 0; i < 120; i++) { swiWaitForVBlank(); }

	swiDecompressLZSSVram ((void*)Top112Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top112Pal, Top112PalLen);

	swiDecompressLZSSVram ((void*)Top113Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top113Pal, Top113PalLen);

	swiDecompressLZSSVram ((void*)Top114Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top114Pal, Top114PalLen);

	swiDecompressLZSSVram ((void*)Top115Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top115Pal, Top115PalLen);

	swiDecompressLZSSVram ((void*)Top116Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top116Pal, Top116PalLen);

	swiDecompressLZSSVram ((void*)Top117Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top117Pal, Top117PalLen);

	swiDecompressLZSSVram ((void*)Top118Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top118Pal, Top118PalLen);

	swiDecompressLZSSVram ((void*)Top119Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], Top119Pal, Top119PalLen);

	swiDecompressLZSSVram ((void*)Top120Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
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
	vramcpy_ui (&BG_PALETTE[0], Top120Pal, Top120PalLen);
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

	
	swiDecompressLZSSVram ((void*)Top121Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
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
	vramcpy_ui (&BG_PALETTE[0], Top121Pal, Top121PalLen);
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


	swiDecompressLZSSVram ((void*)Top122Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
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
	vramcpy_ui (&BG_PALETTE[0], Top122Pal, Top122PalLen);
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


	swiDecompressLZSSVram ((void*)Top123Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot09Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
	vramcpy_ui (&BG_PALETTE[0], Top123Pal, Top123PalLen);
	if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot09Pal, Bot09PalLen); }


	swiDecompressLZSSVram ((void*)Top124Tiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	if(HealthandSafety_MSG) { swiDecompressLZSSVram ((void*)Bot10Tiles, (void*)CHAR_BASE_BLOCK_SUB(2), 0, &decompressBiosCallback); }
	vramcpy_ui (&BG_PALETTE[0], Top124Pal, Top124PalLen);
	if(HealthandSafety_MSG) { vramcpy_ui (&BG_PALETTE_SUB[0], Bot10Pal, Bot10PalLen); }


	swiDecompressLZSSVram ((void*)whiteTiles, (void*)CHAR_BASE_BLOCK(2), 0, &decompressBiosCallback);
	vramcpy_ui (&BG_PALETTE[0], whitePal, whitePalLen);

		
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

