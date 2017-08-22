/*-----------------------------------------------------------------
 Copyright (C) 2005 - 2013
	Michael "Chishm" Chisholm
	Dave "WinterMute" Murphy
	Claudio "sverx"

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

------------------------------------------------------------------*/
#include <nds.h>
#include <nds/fifocommon.h>
#include <stdio.h>
#include <fat.h>
#include <sys/stat.h>
#include <limits.h>
#include <nds/disc_io.h>

#include <string.h>
#include <unistd.h>

#include "nds_loader_arm9.h"
#include "inifile.h"
#include "log.h"

using namespace std;

bool logEnabled = false;

//---------------------------------------------------------------------------------
void stop (void) {
//---------------------------------------------------------------------------------
	while (1) {
		swiWaitForVBlank();
	}
}

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------

	if (fatInitDefault()) {
		/* Log file is dissabled by default. If _nds/twloader/log exist, we turn log file on, else, log is dissabled */
		struct stat logBuf;
		logEnabled = stat("sd:/_nds/twloader/log", &logBuf) == 0;
		/* Log configuration file end */
		
		CIniFile twloaderini( "sd:/_nds/twloader/settings.ini" );
		
		char *p = (char*)PersonalData->name;
		
		// text
		for (int i = 0; i < 10; i++) {
			if (p[i*2] == 0x00)
				p[i*2/2] = 0;
			else
				p[i*2/2] = p[i*2];
		}
		
		if (logEnabled)	LogFMA("TWL.Main", "Got username", p);
		
		twloaderini.SetString("FRONTEND","NAME", p);
		twloaderini.SaveIniFile( "sd:/_nds/twloader/settings.ini" );
		if (logEnabled)	LogFMA("TWL.Main", "Saved username to GUI", p);
		
		runNdsFile ("sd:/_nds/twloader/TWLD.twldr", 0, 0);

		// Subscreen as a console
		videoSetModeSub(MODE_0_2D);
		vramSetBankH(VRAM_H_SUB_BG);
		consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 15, 0, false, true);	

		iprintf ("sd:/_nds/twloader/TWLD.twldr\n");		
		iprintf ("not found.\n");		
		iprintf ("\n");		
		iprintf ("Press B to return to\n");		
		iprintf ("HOME Menu.\n");		
		
	} else {
		
		// Subscreen as a console
		videoSetModeSub(MODE_0_2D);
		vramSetBankH(VRAM_H_SUB_BG);
		consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 15, 0, false, true);	

		iprintf ("fatinitDefault failed!\n");		
		iprintf ("\n");		
		iprintf ("Press B to return to\n");		
		iprintf ("HOME Menu.\n");		
	}
	
	while (1) {
		scanKeys();
		if (keysHeld() & KEY_B) fifoSendValue32(FIFO_USER_01, 1);	// Tell ARM7 to reboot into 3DS HOME Menu (power-off/sleep mode screen skipped)
	}

	return 0;
}
