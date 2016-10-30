#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <3ds.h>
#include <malloc.h>
#include <sys/stat.h>
#include <citrus/app.hpp>
#include <citrus/core.hpp>
#include <citrus/fs.hpp>

#include "inifile.h"

u32 kDown;

int main()
{
	// I think you must init the services you want to use, before using them
	// TO DO: CORRECT GFXINIT
	aptInit();
	amInit();
	sdmcInit();
	romfsInit();
	srvInit();
	hidInit();
	
	// making nds folder if it doesn't exist
	mkdir("sdmc:/nds", 0777);
	mkdir("sdmc:/_nds/twloader/tmp", 0777);
	// We need a way to install the SRL CIAs from romfs
//	ctr::app::install(ctr::fs::NAND, "romfs:/0x000480054B425345LL-bootstrap-loader.cia", 0, u64 size);
//	ctr::app::install(ctr::fs::NAND, "romfs:/0x000480154B4B4750-ntr-launcher.cia", 0, u64 size);
	std::string	bootstrapPath = "";

	gfxInitDefault(); // Init graphic stuff

	// We need these 2 buffers for APT_DoAppJump() later. They can be smaller too
	u8 param[0x300];
	u8 hmac[0x20];
	// Clear both buffers
	memset(param, 0, sizeof(param));
	memset(hmac, 0, sizeof(hmac));

	// Loop as long as the status is not exit
	while(aptMainLoop()) {
		// Scan hid shared memory for input events
		hidScanInput();

			{ // If the A button got pressed, start the app launch 
			
			consoleInit(GFX_BOTTOM, NULL);
			
			CIniFile bootstrapini( "sdmc:/_nds/nds-bootstrap.ini" );	

			std::vector<std::string> files = {};
		
			std::string extension = ".nds";
		
			std::string fat = "fat:/nds/";
		
			DIR *dir;
			struct dirent *ent;
			
			if ((dir = opendir ("sdmc:/nds/")) != NULL) {
			/* print all the files and directories within directory */
				while ((ent = readdir (dir)) != NULL) {
					std::string fname = (ent->d_name);
					if(fname.find(extension, (fname.length() - extension.length())) != std::string::npos)
						files.push_back(ent->d_name);
				}
				closedir (dir);
			}
		
			u32 cursorPosition = 0, i = 0;
			
			char* rom = (char*)malloc(256);
			
			bool whileloop = true;
		
			while(whileloop){
				if(files.size() >= 29) {
					for(i = 0; i < 30; i++){
						if(cursorPosition == i)
							printf("--> %s\n", files.at(i).c_str());
						else 
							printf("%s\n", files.at(i).c_str());
					}
				} else {
					for(i = 0; i < files.size(); i++){
						if(cursorPosition == i)
							printf("--> %s\n", files.at(i).c_str());
						else 
							printf("%s\n", files.at(i).c_str());
					}
				}
					
				while(true){
					hidScanInput();
					
					u32 hDown = hidKeysDown();
					
					if(hDown & KEY_A){
						rom = (char*)(files.at(cursorPosition)).c_str();
						consoleClear();
						whileloop = false;
						//break;
						break;
					} else if((hDown & KEY_DOWN) && cursorPosition != 29){
						consoleClear();
						cursorPosition++;
						break;
					} else if((hDown & KEY_UP) && cursorPosition != 0){
						consoleClear();
						cursorPosition--;
						break;
					} else if(hDown & KEY_X) {
						// Prepare for the slot-1 launch throung NTR Launcher
						APT_PrepareToDoApplicationJump(0, 0x000480154B4B4750, 0); // ntr_launcher title ID
						// Tell APT to trigger the app launch and set the status of this app to exit
						APT_DoApplicationJump(param, sizeof(param), hmac);
						break;
					} else if (hDown & KEY_START) {
						//WARNING! BEFORE RETURNING TO THE HOME, YOU MUST CLOSE ALL THE OPEN SERVICES!
						hidExit();
						srvExit();
						romfsExit();
						sdmcExit();
						aptExit();
						gfxExit();
						return 0;
					}
				}
			}
		

			bootstrapini.SetString("NDS-BOOTSTRAP", "NDS_PATH",fat+rom);
			bootstrapini.SaveIniFile( "sdmc:/_nds/nds-bootstrap.ini");

			// Prepare for the app launch
			APT_PrepareToDoApplicationJump(0, 0x000480054B425345LL, 0); // bootstrap title ID
			// Tell APT to trigger the app launch and set the status of this app to exit
			APT_DoApplicationJump(param, sizeof(param), hmac);
		}

		// Flush + swap framebuffers and wait for VBlank. Not really needed in this example
		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
	}

	
	hidExit();
	srvExit();
	romfsExit();
	sdmcExit();
	aptExit();
	gfxExit();

    return 0;
}