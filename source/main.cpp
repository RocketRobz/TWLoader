#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <3ds.h>
#include <malloc.h>

#include "inifile.h"



int main()
{
	std::string	bootstrapPath = "";

	gfxInitDefault(); // Init graphic stuff
	

	// We need these 2 buffers for APT_DoAppJump() later. They can be smaller too
	u8 buf0[0x300];
	u8 buf1[0x20];


	// Loop as long as the status is not exit
	while(aptMainLoop())
	{
		// Scan hid shared memory for input events
		//hidScanInput();

		if(1) // If the A button got pressed, start the app launch
		{
		
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
			if(fname.find(extension, (fname.length() - extension.length())) != std::string::npos){
			 files.push_back(ent->d_name);
		}
	}
		closedir (dir);
	}
	
	int cursorPosition = 0;
	
	int i = 0;
	
	char* rom = malloc(256);
	
	bool whileloop = true;
	
	while(whileloop){
		if(files.size() >= 29){
			for(i = 0; i < 30; i++){
			if(cursorPosition == i){
				printf("--> %s\n", files.at(i).c_str());
			} else {
				printf("%s\n", files.at(i).c_str());
			}
			}
		} else {
			for(i = 0; i < files.size(); i++){
			if(cursorPosition == i){
				printf("--> %s\n", files.at(i).c_str());
			} else {
				printf("%s\n", files.at(i).c_str());
			}
			}
			}
			
			while(true){
				hidScanInput();
				
				u32 hDown = hidKeysDown();
				
				if(hDown & KEY_A){
					rom = (char*)(files.at(cursorPosition)).c_str();
					consoleClear;
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
				}
			}
			
		}
	

	bootstrapini.SetString("NDS-BOOTSTRAP", "NDS_PATH",fat+rom);
	bootstrapini.SaveIniFile( "sdmc:/_nds/nds-bootstrap.ini");
	
	
			// Clear both buffers
			memset(buf0, 0, 0x300);
			memset(buf1, 0, 0x20);

			// Open an APT session so we can talk to the APT service
			aptOpenSession();
			// Prepare for the app launch
			APT_PrepareToDoAppJump(0, 0x000480054B425345LL, 0); // bootstrap title ID
			// Tell APT to trigger the app launch and set the status of this app to exit
			APT_DoAppJump(0x300 /* size of buf0 */, 0x20 /* size of buf1 */, buf0, buf1);
			// Close the APT session because we don't need APT anymore
			aptCloseSession();
		}

		// Flush + swap framebuffers and wait for VBlank. Not really needed in this example
		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
	}


	gfxExit();

	return 0;
}
