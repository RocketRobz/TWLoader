#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#include <sftd.h>
#include <malloc.h>
#include <sys/stat.h>
#include <citrus/app.hpp>
#include <citrus/battery.hpp>
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

	// Code doesn't want to compile
	// ctr::app::install(ctr::fs::NAND, "sdmc:/_nds/twloader/cia/twlapp.cia", 0, 1287328);

	// We need a way to install the SRL CIA from romfs
	//	ctr::app::install(ctr::fs::NAND, "romfs:/twloader-twl.cia", 0, u64 size);

	std::string	bootstrapPath = "";

	gfxInitDefault(); // Init graphic stuff
	
    sf2d_init();
    sf2d_set_clear_color(RGBA8(0x00, 0x00, 0x00, 0x00));
	
	// Font loading
	sftd_init();
	sftd_font *font = sftd_load_font_file("romfs:/font.ttf");

	sf2d_texture *topbgtex = sfil_load_PNG_file("romfs:/assets/topbg.png", SF2D_PLACE_RAM);
	//sf2d_texture *boxarttex = sfil_load_PNG_file("romfs:/assets/boxart_unknown.png", SF2D_PLACE_RAM);
	sf2d_texture *toptex = sfil_load_PNG_file("romfs:/assets/top.png", SF2D_PLACE_RAM);
	//sf2d_texture *vol0tex = sfil_load_PNG_file("romfs:/assets/volume0.png", SF2D_PLACE_RAM);
	//sf2d_texture *vol1tex = sfil_load_PNG_file("romfs:/assets/volume1.png", SF2D_PLACE_RAM);
	//sf2d_texture *vol2tex = sfil_load_PNG_file("romfs:/assets/volume2.png", SF2D_PLACE_RAM);
	//sf2d_texture *vol3tex = sfil_load_PNG_file("romfs:/assets/volume3.png", SF2D_PLACE_RAM);
	sf2d_texture *shoulderLtex = sfil_load_PNG_file("romfs:/assets/shoulder_L.png", SF2D_PLACE_RAM);
	sf2d_texture *shoulderRtex = sfil_load_PNG_file("romfs:/assets/shoulder_R.png", SF2D_PLACE_RAM);
	sf2d_texture *batterychrgtex = sfil_load_PNG_file("romfs:/assets/battery_charging.png", SF2D_PLACE_RAM);
	sf2d_texture *bottomtex = sfil_load_PNG_file("romfs:/assets/bottom.png", SF2D_PLACE_RAM);
	sf2d_texture *bottomsettingstex = sfil_load_PNG_file("romfs:/assets/bottom_settings.png", SF2D_PLACE_RAM);

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
			
			gfxSet3D(true);
			
			// consoleInit(GFX_BOTTOM, NULL);
			
			CIniFile settingsini( "sdmc:/_nds/twloader/settings.ini" );	
			CIniFile bootstrapini( "sdmc:/_nds/nds-bootstrap.ini" );

			// Settings .ini file
			char* settingsini_frontend = "FRONTEND";
			
			char* settingsini_twlmode = "TWL-MODE";
			char* settingsini_twl_clock = "TWL_CLOCK";
			char* settingsini_twl_bootani = "BOOT_ANIMATION";
			char* settingsini_twl_hsmsg = "HEALTH&SAFETY_MSG";
			char* settingsini_twl_launchslot1 = "LAUNCH_SLOT1";
			char* settingsini_twl_resetslot1 = "RESET_SLOT1";
			// End
			
			// Bootstrap .ini file
			char* bootstrapini_ndsbootstrap = "NDS-BOOTSTRAP";
			char* bootstrapini_ndspath = "NDS_PATH";
			char* bootstrapini_debug = "DEBUG";
			// End

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
			u32 settingscursorPosition = 0;
			
			char* rom = (char*)malloc(256);
			
			char* boxartpath = malloc(256);
			
			bool whileloop = true;
			
			//char* nickname = "Nickname";
			
			// Settings text
			char* settingstext = "Settings";
			
			char* settings_cpuspeedtext = "ARM9 CPU Speed";
			char* settings_bootscreentext = "DS/DSi Boot Screen";
			char* settings_healthsafetytext = "Health and Safety message";
			char* settings_resetslot1text = "Reset Slot-1";
			char* settings_consoletext = "Console output";
			
			char* settings_cpuspeedvaluetext;
			char* settings_bootscreenvaluetext;
			char* settings_healthsafetyvaluetext;
			char* settings_resetslot1valuetext;
			char* settings_consolevaluetext;
			// End of Settings text
			
			int settings_cpuspeedvalue;
			int settings_bootscreenvalue;
			int settings_healthsafetyvalue;
			int settings_resetslot1value;
			int settings_consolevalue;
			
			int screenmode = 0;
			// 0: ROM select
			// 1: Settings
			
			int filenameYpos;
			
			int settingsXpos = 24;
			int settingsvalueXpos = 240;
			int settingsYpos;
		
			while(whileloop){
				if (screenmode == 0) {
					sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
					sf2d_draw_texture(bottomtex, 320/2 - bottomtex->width/2, 240/2 - bottomtex->height/2);
					
					// Box art loading code (doesn't work)
					//boxartpath = malloc(256);
					//	
					//strcat(boxartpath, "sdmc:/_nds/twloader/boxart/");
					//strcat(boxartpath, rom);
					//strcat(boxartpath, ".png");
					//
					//if (fopen(boxartpath, "r")) {
					//	sf2d_texture *boxarttex = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM);
					//} else {
					//	sf2d_texture *boxarttex = sfil_load_PNG_file("romfs:/assets/boxart_unknown.png", SF2D_PLACE_RAM);
					//}
					
					filenameYpos = 0;
					if(files.size() >= 29) {
						for(i = 0; i < 30; i++){
							if(cursorPosition == i) {
								sftd_draw_textf(font, 10, filenameYpos, RGBA8(0, 0, 255, 255), 12, files.at(i).c_str());
								filenameYpos += 12;
							} else {
								sftd_draw_textf(font, 10, filenameYpos, RGBA8(0, 0, 0, 255), 12, files.at(i).c_str());
								filenameYpos += 12;
							}
						}
					} else {
						for(i = 0; i < files.size(); i++){
							if(cursorPosition == i) {
								sftd_draw_textf(font, 10, filenameYpos, RGBA8(0, 0, 255, 255), 12, files.at(i).c_str());
								filenameYpos += 12;
							} else {
								sftd_draw_textf(font, 10, filenameYpos, RGBA8(0, 0, 0, 255), 12, files.at(i).c_str());
								filenameYpos += 12;
							}
						}
					}
				} else if(screenmode == 1) {
					sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
					sf2d_draw_texture(bottomsettingstex, 320/2 - bottomsettingstex->width/2, 240/2 - bottomsettingstex->height/2);
					
					if (settings_cpuspeedvalue == 1) {
						settings_cpuspeedvaluetext = "133mhz (TWL)";
					} else {
						settings_cpuspeedvaluetext = "67mhz (NTR)";
					}
					if (settings_bootscreenvalue == 1) {
						settings_bootscreenvaluetext = "On";
					} else {
						settings_bootscreenvaluetext = "Off";
					}
					if (settings_healthsafetyvalue == 1) {
						settings_healthsafetyvaluetext = "On";
					} else {
						settings_healthsafetyvaluetext = "Off";
					}
					if (settings_resetslot1value == 1) {
						settings_resetslot1valuetext = "On";
					} else {
						settings_resetslot1valuetext = "Off";
					}
					if (settings_consolevalue == 1) {
						settings_consolevaluetext = "On";
					} else if (settings_consolevalue == 2) {
						settings_consolevaluetext = "On (Debug)";
					} else {
						settings_consolevaluetext = "Off";
					}
			
					settingsYpos = 64;
					sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 16, settingstext);
					if(settingscursorPosition == 0) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(0, 0, 255, 255), 12, settings_cpuspeedtext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(0, 0, 255, 255), 12, settings_cpuspeedvaluetext);
						settingsYpos += 12;
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_cpuspeedtext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_cpuspeedvaluetext);
						settingsYpos += 12;
					}
					if(settingscursorPosition == 1) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(0, 0, 255, 255), 12, settings_bootscreentext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(0, 0, 255, 255), 12, settings_bootscreenvaluetext);
						settingsYpos += 12;
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_bootscreentext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_bootscreenvaluetext);
						settingsYpos += 12;
					}
					if(settingscursorPosition == 2) {
						sftd_draw_textf(font, settingsXpos+16, settingsYpos, RGBA8(0, 0, 255, 255), 12, settings_healthsafetytext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(0, 0, 255, 255), 12, settings_healthsafetyvaluetext);
						settingsYpos += 12;
					} else {
						sftd_draw_textf(font, settingsXpos+16, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_healthsafetytext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_healthsafetyvaluetext);
						settingsYpos += 12;
					}
					if(settingscursorPosition == 3) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(0, 0, 255, 255), 12, settings_resetslot1text);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(0, 0, 255, 255), 12, settings_resetslot1valuetext);
						settingsYpos += 12;
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_resetslot1text);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_resetslot1valuetext);
						settingsYpos += 12;
					}
					if(settingscursorPosition == 4) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(0, 0, 255, 255), 12, settings_consoletext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(0, 0, 255, 255), 12, settings_consolevaluetext);
						settingsYpos += 12;
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_consoletext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_consolevaluetext);
						settingsYpos += 12;
					}
				}
					
				while(true){
					hidScanInput();
					
					u32 hDown = hidKeysDown();
					u32 hHeld = hidKeysHeld();
					
					sf2d_start_frame(GFX_TOP, GFX_LEFT);
					//Draws a 100x100 yellow rectangle (255, 255, 00, 255) at (150, 70)
					//sf2d_draw_rectangle(150, 70, 100, 100, RGBA8(0xFF, 0xFF, 0x00, 0xFF));

					sf2d_draw_texture(topbgtex, 400/2 - topbgtex->width/2, 240/2 - topbgtex->height/2);
					//sf2d_draw_texture(boxarttex, 400/2 - boxarttex->width/2, 240/2 - boxarttex->height/2);
					sf2d_draw_texture(toptex, 400/2 - toptex->width/2, 240/2 - toptex->height/2);
					//sftd_draw_textf(font, 24, 2, RGBA8(0, 0, 0, 255), 12, nickname);
					if(hHeld & KEY_L){
						sf2d_draw_texture(shoulderLtex, 0, 223);
					} else {
						sf2d_draw_texture(shoulderLtex, 0, 220);
					}
					if(hHeld & KEY_R){
						sf2d_draw_texture(shoulderRtex, 336, 223);
					} else {
						sf2d_draw_texture(shoulderRtex, 336, 220);
					}

					//if (ctr::battery::charging::level) {
					//	sf2d_draw_texture(batterychrgtex, 370, 2);
					//}
					sf2d_end_frame();
					
					sf2d_start_frame(GFX_TOP, GFX_RIGHT);
					sf2d_draw_texture(topbgtex, 430/2 - topbgtex->width/2, 240/2 - topbgtex->height/2);
					//sf2d_draw_texture(boxarttex, 408/2 - boxarttex->width/2, 240/2 - boxarttex->height/2);
					sf2d_draw_texture(toptex, 400/2 - toptex->width/2, 240/2 - toptex->height/2);
					//sftd_draw_textf(font, 24, 2, RGBA8(0, 0, 0, 255), 12, nickname);
					if(hHeld & KEY_L){
						sf2d_draw_texture(shoulderLtex, -1, 223);
					} else {
						sf2d_draw_texture(shoulderLtex, -1, 220);
					}
					if(hHeld & KEY_R){
						sf2d_draw_texture(shoulderRtex, 335, 223);
					} else {
						sf2d_draw_texture(shoulderRtex, 335, 220);
					}
					sf2d_end_frame();
					
					sf2d_swapbuffers();
					
					if (screenmode == 0) {
						if (hDown & KEY_START) {
							//WARNING! BEFORE RETURNING TO THE HOME, YOU MUST CLOSE ALL THE OPEN SERVICES!
							hidExit();
							srvExit();
							romfsExit();
							sdmcExit();
							aptExit();
							sf2d_free_texture(topbgtex);
							//sf2d_free_texture(boxarttex);
							sf2d_free_texture(toptex);
							sf2d_free_texture(shoulderLtex);
							sf2d_free_texture(shoulderRtex);
							sf2d_free_texture(batterychrgtex);
							sf2d_free_texture(bottomtex);
							sf2d_free_texture(bottomsettingstex);
							sf2d_fini();
							gfxExit();
							return 0;
						} else if(hDown & KEY_A){
							rom = (char*)(files.at(cursorPosition)).c_str();
							//consoleClear();
							whileloop = false;
							settingsini.SetInt(settingsini_twlmode, settingsini_twl_launchslot1, 0);
							settingsini.SaveIniFile( "sdmc:/_nds/twloader/settings.ini");
							sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
							sf2d_end_frame();
							sf2d_swapbuffers();
							break;
						} else if((hDown & KEY_DOWN) && cursorPosition != 29){
							//consoleClear();
							cursorPosition++;
							break;
						} else if((hDown & KEY_UP) && cursorPosition != 0){
							//consoleClear();
							cursorPosition--;
							break;
						} else if(hDown & KEY_X) {
							whileloop = false;
							settingsini.SetInt(settingsini_twlmode, settingsini_twl_launchslot1, 1);
							settingsini.SaveIniFile( "sdmc:/_nds/twloader/settings.ini");
							sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
							sf2d_end_frame();
							sf2d_swapbuffers();
							break;
						} else if (hDown & KEY_SELECT) {
							if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_clock, 0) == 1) {
								settings_cpuspeedvalue = 1;
							} else {
								settings_cpuspeedvalue = 0;
							}
							if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_bootani, 0) == 1) {
								settings_bootscreenvalue = 1;
							} else {
								settings_bootscreenvalue = 0;
							}
							if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_hsmsg, 0) == 1) {
								settings_healthsafetyvalue = 1;
							} else {
								settings_healthsafetyvalue = 0;
							}
							if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_resetslot1, 0) == 1) {
								settings_resetslot1value = 1;
							} else {
								settings_resetslot1value = 0;
							}
							if (bootstrapini.GetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, 0) == 1) {
								settings_resetslot1value = 2;
							} else if (bootstrapini.GetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, 0) == 0) {
								settings_resetslot1value = 1;
							} else {
								settings_resetslot1value = 0;
							}
							screenmode = 1;
							break;
						}
					} else if (screenmode == 1) {
						if(hDown & KEY_A){
							if (settingscursorPosition == 0) {
								settings_cpuspeedvalue++; // CPU speed
								if(settings_cpuspeedvalue == 2) {
									settings_cpuspeedvalue = 0;
								}
							} else if (settingscursorPosition == 1) {
								settings_bootscreenvalue++; // Boot screen
								if(settings_bootscreenvalue == 2) {
									settings_bootscreenvalue = 0;
								}
							} else if (settingscursorPosition == 2) {
								settings_healthsafetyvalue++; // H&S message
								if(settings_healthsafetyvalue == 2) {
									settings_healthsafetyvalue = 0;
								}
							} else if (settingscursorPosition == 3) {
								settings_resetslot1value++; // Reset Slot-1
								if(settings_resetslot1value == 2) {
									settings_resetslot1value = 0;
								}
							} else if (settingscursorPosition == 4) {
								settings_consolevalue++; // Console output
								if(settings_consolevalue == 3) {
									settings_consolevalue = 0;
								}
							}
							break;
						} else if((hDown & KEY_DOWN) && settingscursorPosition != 4){
							settingscursorPosition++;
							break;
						} else if((hDown & KEY_UP) && settingscursorPosition != 0){
							settingscursorPosition--;
							break;
						} else if(hDown & KEY_SELECT){
							if (settings_cpuspeedvalue == 1) {
								settingsini.SetInt(settingsini_twlmode, settingsini_twl_clock, 1);
							} else {
								settingsini.SetInt(settingsini_twlmode, settingsini_twl_clock, 0);
							}
							if (settings_bootscreenvalue == 1) {
								settingsini.SetInt(settingsini_twlmode, settingsini_twl_bootani, 1);
							} else {
								settingsini.SetInt(settingsini_twlmode, settingsini_twl_bootani, 0);
							}
							if (settings_healthsafetyvalue == 1) {
								settingsini.SetInt(settingsini_twlmode, settingsini_twl_hsmsg, 1);
							} else {
								settingsini.SetInt(settingsini_twlmode, settingsini_twl_hsmsg, 0);
							}
							if (settings_resetslot1value == 1) {
								settingsini.SetInt(settingsini_twlmode, settingsini_twl_resetslot1, 1);
							} else {
								settingsini.SetInt(settingsini_twlmode, settingsini_twl_resetslot1, 0);
							}
							if (settings_consolevalue == 2) {
								bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, 1);
							} else if (settings_consolevalue == 1) {
								bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, 0);
							} else {
								bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, -1);
							}
							settingsini.SaveIniFile( "sdmc:/_nds/twloader/settings.ini");
							bootstrapini.SaveIniFile( "sdmc:/_nds/nds-bootstrap.ini");
							screenmode = 0;
							break;
						} 
					}
				}
			}

			if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_launchslot1, 0) == 0) {
				bootstrapini.SetString(bootstrapini_ndsbootstrap, bootstrapini_ndspath,fat+rom);
				bootstrapini.SaveIniFile( "sdmc:/_nds/nds-bootstrap.ini");
			}

			// Prepare for the app launch
			APT_PrepareToDoApplicationJump(0, 0x0004800554574C44LL, 0); // TWL app's title ID
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
	sf2d_free_texture(topbgtex);
	//sf2d_free_texture(boxarttex);
	sf2d_free_texture(toptex);
	sf2d_free_texture(shoulderLtex);
	sf2d_free_texture(shoulderRtex);
	sf2d_free_texture(batterychrgtex);
	sf2d_free_texture(bottomtex);
	sf2d_free_texture(bottomsettingstex);
    sf2d_fini();
	gfxExit();

    return 0;
}