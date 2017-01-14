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
#include <malloc.h>
#include <unistd.h>

#include "nds_loader_arm9.h"
#include "file_browse.h"

#include "hbmenu_banner.h"

#include "ndsheaderbanner.h"
#include "iconTitle.h"

#include "inifile.h"
#include "log.h"

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

	bool TriggerExit = false;
	

	if (fatInitDefault()) {
		CIniFile twloaderini( "sd:/_nds/twloader/settings.ini" );
		LogFM("Flashcard.Main", "Fat inited");
	}

	// overwrite reboot stub identifier
	extern u64 *fake_heap_end;
	*fake_heap_end = 0;

	defaultExceptionHandler();

	int pathLen;
	std::string filename;
	const char* bannerfilepath;
	std::string bannerfilepathfixed;

	iconTitleInit();

	// Subscreen as a console
	videoSetModeSub(MODE_0_2D);
	vramSetBankH(VRAM_H_SUB_BG);
	consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 15, 0, false, true);	
	
	if (!fatInitDefault()) {
		iprintf ("fatinitDefault failed!\n");		
			
		doPause();
		
		TriggerExit = true;
	}

	chdir ("fat:/");	// Change directory to flashcard root
	
	keysSetRepeat(25,5);
	
	vector<string> extensionList;
	extensionList.push_back(".nds");

	while(1) {

		if(TriggerExit) { 
		do { swiWaitForVBlank(); scanKeys(); } while (!keysDown());
		break;
		}

		filename = browseForFile(extensionList);

		// Construct a command line
		getcwd (filePath, PATH_MAX);
		pathLen = strlen (filePath);
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
			char *name = argarray.at(0);
			strcpy (filePath + pathLen, name);

			free(bannerfilepath);
			bannerfilepath = malloc(256);
			strcpy(bannerfilepath, "sd:/_nds/twloader/bnricons/flashcard/");
			strcat(bannerfilepath, filename.c_str());
			strcat(bannerfilepath, ".bin");
			
			bannerfilepathfixed = ReplaceAll(bannerfilepath, ".nds", ".ini");
			
			if( access( bannerfilepath, F_OK ) == -1 ) {
				FILE* ndsFile = fopen(filePath, "rb");
				FILE* filetosave = fopen(bannerfilepathfixed.c_str(), "wb");
				
				TWLDsNDSHeader NDSHeader;
				TWLDsNDSBannersize1 myBannersize1;
				TWLDsNDSBannersize2 myBannersize2;
				TWLDsNDSBannersize3 myBanner;
				
				iprintf ("Reading NDS file.\n");
				iprintf (filePath);
				iprintf (" \n");

				LogFMA("Flashcard.Main", "Reading .NDS file:", filePath);
				fseek ( ndsFile , 0 , SEEK_SET );
				fread(&NDSHeader,1,sizeof(NDSHeader),ndsFile);
				LogFMA("Flashcard.Main", ".NDS file read:", filePath);

				if (NDSHeader.bannerOffset != 0x00000000) {
					fseek ( ndsFile , NDSHeader.bannerOffset , SEEK_SET );
					
					fread(&myBanner,1,sizeof(myBanner),ndsFile);
					
					iprintf ("Now caching banner data.\n");
					LogFMA("Flashcard.Main", "Caching banner data:", bannerfilepath);
					if (myBanner.version == 0x0103 || myBanner.version == 0x0003) {
						fwrite(&myBanner,1,sizeof(myBanner),filetosave);
					} else if (myBanner.version == 0x0002) {
						fwrite(&myBanner,1,sizeof(myBannersize2),filetosave);
					} else {
						fwrite(&myBanner,1,sizeof(myBannersize1),filetosave);
					}

					iprintf ("Banner data cached.");
					LogFMA("Flashcard.Main", "Banner data cached:", bannerfilepath);
				} else {
					iprintf ("Banner data doesn't exist.");
					for (int i = 0; i < 60; i++) { swiWaitForVBlank(); }
				}
				fclose(ndsFile);
				fclose(filetosave);
			} else {
				iprintf ("Banner data already exists.");
				for (int i = 0; i < 60; i++) { swiWaitForVBlank(); }
			}

		}

		/* while (1) {
			swiWaitForVBlank();
			scanKeys();
			if (!(keysHeld() & KEY_A)) break;
		} */
	}

	return 0;
}
