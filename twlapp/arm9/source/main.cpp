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

#include "bootsplash.h"
#include "inifile.h"

using namespace std;

//---------------------------------------------------------------------------------
void stop (void) {
//---------------------------------------------------------------------------------
	while (1) {
		swiWaitForVBlank();
	}
}

char filePath[PATH_MAX];

//---------------------------------------------------------------------------------
void doPause() {
//---------------------------------------------------------------------------------
	iprintf("Press start...\n");
	while(1) {
		scanKeys();
		if(keysDown() & KEY_START)
			break;
		swiWaitForVBlank();
	}
	scanKeys();
}

void runFile(string filename) {
	vector<char*> argarray;
	
	if ( strcasecmp (filename.c_str() + filename.size() - 5, ".argv") == 0) {
		FILE *argfile = fopen(filename.c_str(),"rb");
		char str[PATH_MAX], *pstr;
		const char seps[]= "\n\r\t ";

		while( fgets(str, PATH_MAX, argfile) ) {
			// Find comment and end string there
			if( (pstr = strchr(str, '#')) )
				*pstr= '\0';

			// Tokenize arguments
			pstr= strtok(str, seps);

			while( pstr != NULL ) {
				argarray.push_back(strdup(pstr));
				pstr= strtok(NULL, seps);
			}
		}
		fclose(argfile);
		filename = argarray.at(0);
	} else {
		argarray.push_back(strdup(filename.c_str()));
	}

	if ( strcasecmp (filename.c_str() + filename.size() - 4, ".nds") != 0 || argarray.size() == 0 ) {
		iprintf("no nds file specified\n");
	} else {
		iprintf("Running %s with %d parameters\n", argarray[0], argarray.size());
		int err = runNdsFile (argarray[0], argarray.size(), (const char **)&argarray[0]);
		iprintf("Start failed. Error %i\n", err);
		doPause();
	}
}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}


//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------
	// REG_SCFG_CLK = 0x80;
	REG_SCFG_CLK = 0x85;

	bool HealthandSafety_MSG = false;
	bool UseNTRSplash = true;
	bool TriggerExit = false;
	// std::string	bootstrapPath = "";

	bool consoleOn = false;

	/* scanKeys();
	int pressed = keysDown(); */

	if (fatInitDefault()) {
		CIniFile twloaderini( "sd:/_nds/twloader/settings.ini" );
		
		// bootstrapPath = twloaderini.GetString( "TWL-MODE", "BOOTSTRAP_INI", "");
		
		if(twloaderini.GetInt("TWL-MODE","HEALTH&SAFETY_MSG",0) == 1) { HealthandSafety_MSG = true; }
		if(twloaderini.GetInt("TWL-MODE","TWL_CLOCK",0) == 1) { UseNTRSplash = false; }
		if(twloaderini.GetInt("TWL-MODE","GBARUNNER",0) == 0)
			if(twloaderini.GetInt("TWL-MODE","BOOT_ANIMATION",0) == 1) { BootSplashInit(UseNTRSplash, HealthandSafety_MSG); }
		if(twloaderini.GetInt("TWL-MODE","DEBUG",0) != -1) {
			consoleDemoInit();
			consoleOn = true;
		}
		if(twloaderini.GetInt("TWL-MODE","TWL_CLOCK",0) == 1) {
			REG_SCFG_CLK |= 0x1;
			if(twloaderini.GetInt("TWL-MODE","DEBUG",0) == 1) {
				printf("TWL_CLOCK ON\n");		
			}
		} else {
			REG_SCFG_CLK = 0x80;
			fifoSendValue32(FIFO_USER_04, 1);
		}
		
		if(twloaderini.GetInt("TWL-MODE","TWL_VRAM",0) == 1) {
			REG_SCFG_EXT |= 0x2000;
			if(twloaderini.GetInt("TWL-MODE","DEBUG",0) == 1) {
				printf("TWL_VRAM ON\n");		
			}
		} else {
			REG_SCFG_CLK = 0x80;
			fifoSendValue32(FIFO_USER_04, 1);
		}

		if(twloaderini.GetInt("TWL-MODE","BOOT_ANIMATION",0) == 0) {
			if(twloaderini.GetInt("TWL-MODE","LAUNCH_SLOT1",0) == 1) {
				if(REG_SCFG_MC == 0x11) { 
					if (consoleOn == false) {
						consoleDemoInit(); }
					printf("Please insert a cartridge...\n");
					do { swiWaitForVBlank(); } 
					while (REG_SCFG_MC == 0x11);
				}
			}
		}
		
		if(twloaderini.GetInt("TWL-MODE","RESET_SLOT1",0) == 1) {
			if(twloaderini.GetInt("TWL-MODE","FORWARDER",0) == 1) {
				if(twloaderini.GetInt("TWL-MODE","FLASHCARD",0) == 0 || twloaderini.GetInt("TWL-MODE","FLASHCARD",0) == 1 || twloaderini.GetInt("TWL-MODE","FLASHCARD",0) == 2 || twloaderini.GetInt("TWL-MODE","FLASHCARD",0) == 4) {} else {
					fifoSendValue32(FIFO_USER_02, 1);
					if(twloaderini.GetInt("TWL-MODE","DEBUG",0) == 1) {
						printf("RESET_SLOT1 ON\n");		
					}
				}
			} else {
				fifoSendValue32(FIFO_USER_02, 1);
				if(twloaderini.GetInt("TWL-MODE","DEBUG",0) == 1) {
					printf("RESET_SLOT1 ON\n");		
				}
			}
		}

		fifoSendValue32(FIFO_USER_01, 1);
		fifoWaitValue32(FIFO_USER_03);

		// Only time SCFG should be locked is for compatiblity with NTR retail stuff.
		// So NTR SCFG values (that preserve SD access) are always used when locking.
		// Locking Arm9 SCFG kills SD access. So that will not occur here.
		if(twloaderini.GetInt("TWL-MODE","LOCK_ARM7_SCFG_EXT",0) == 1) {
			fifoSendValue32(FIFO_USER_05, 1);
			REG_SCFG_EXT = 0x83000000;
		} else {
			if(twloaderini.GetInt("TWL-MODE","ENABLE_ALL_TWLSCFG",0) == 1) {
				fifoSendValue32(FIFO_USER_06, 1);
				REG_SCFG_EXT = 0x8307F100;
			} else {
				REG_SCFG_EXT = 0x83000000;
			}
		}
		// Tell Arm7 to apply changes.
		fifoSendValue32(FIFO_USER_07, 1);

		for (int i = 0; i < 20; i++) { swiWaitForVBlank(); }
		
		if(twloaderini.GetInt("TWL-MODE","LAUNCH_SLOT1",0) == 1) {
			if(twloaderini.GetInt("TWL-MODE","DEBUG",0) != -1) {
				printf("Now booting Slot-1 card\n");					
			}
		}
		if(twloaderini.GetInt("TWL-MODE","FORWARDER",0) == 1) {
			if(twloaderini.GetInt("TWL-MODE","FLASHCARD",0) == 0) {
				runFile("sd:/_dsttfwd/loadcard.nds");
			} else if(twloaderini.GetInt("TWL-MODE","FLASHCARD",0) == 1) {
				runFile("sd:/_nds/twloader/loadflashcard/r4.nds");
			} else if(twloaderini.GetInt("TWL-MODE","FLASHCARD",0) == 2) {
				runFile("sd:/_nds/twloader/loadflashcard/r4idsn.nds");
			} else if(twloaderini.GetInt("TWL-MODE","FLASHCARD",0) == 4) {
				runFile("sd:/_nds/twloader/loadflashcard/ace_rpg.nds");
			}
		}
		
		if(twloaderini.GetInt("TWL-MODE","LAUNCH_SLOT1",0) == 1) {
			runFile("sd:/_nds/twloader/NTR_Launcher.nds");
		}
		
		if(twloaderini.GetInt("TWL-MODE","LAUNCH_SLOT1",0) == 0) {
			if(twloaderini.GetInt("TWL-MODE","DEBUG",0) != -1) {
				printf("Now setting .nds path\n");	
				printf ("and booting bootstrap\n");					
			}
		}
	}

	// overwrite reboot stub identifier
	extern u64 *fake_heap_end;
	*fake_heap_end = 0;

	defaultExceptionHandler();

	std::string filename;
	std::string gamename;

	if (!fatInitDefault()) {
		// Subscreen as a console
		videoSetModeSub(MODE_0_2D);
		vramSetBankH(VRAM_H_SUB_BG);
		consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 15, 0, false, true);	

		iprintf ("fatinitDefault failed!\n");		
			
		doPause();
		
		TriggerExit = true;
	}

	vector<string> extensionList;
	extensionList.push_back(".nds");
	extensionList.push_back(".argv");
	FILE* ndsfile;

	while(1) {

		if(TriggerExit) { 
		do { swiWaitForVBlank(); scanKeys(); } while (!keysDown());
		break;
		}
		
		sNDSHeader NDSHeader;
		
		CIniFile bootstrapini( "sd:/_nds/nds-bootstrap.ini" );
		gamename = bootstrapini.GetString("NDS-BOOTSTRAP", "NDS_PATH","");
		gamename = ReplaceAll( gamename, "fat:/", "sd:/");
		ndsfile = fopen(gamename.c_str(),"rb");
		fread(&NDSHeader,1,sizeof(NDSHeader),ndsfile);
		
		if (NDSHeader.makercode != 0x0000) {	// if the launched game is a retail ROM
			filename = bootstrapini.GetString("NDS-BOOTSTRAP", "BOOTSTRAP_PATH","");
			filename = ReplaceAll( filename, "fat:/", "sd:/");
			runFile(filename);
		} else {	// or if the launched game is a homebrew ROM
			filename = bootstrapini.GetString("NDS-BOOTSTRAP", "BOOTSTRAP_PATH_HB","");
			filename = ReplaceAll( filename, "fat:/", "sd:/");
			runFile(filename);
		}
		
		// Subscreen as a console
		videoSetModeSub(MODE_0_2D);
		vramSetBankH(VRAM_H_SUB_BG);
		consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 15, 0, false, true);	
		
		iprintf ("bootstrap not found\n");
		doPause();
		
		TriggerExit = true;

		while (1) {
			swiWaitForVBlank();
			scanKeys();
			if (!(keysHeld() & KEY_A)) break;
		}
	}

	return 0;
}
