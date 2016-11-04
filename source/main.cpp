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

CIniFile settingsini( "sdmc:/_nds/twloader/settings.ini" );	

int color_Rvalue;
int color_Gvalue;
int color_Bvalue;
	
char* topbgloc;
	
// Settings .ini file
char* settingsini_frontend = "FRONTEND";
char* settingsini_frontend_color = "COLOR";
char* settingsini_frontend_botlayout = "BOTTOM_LAYOUT";
	
char* settingsini_twlmode = "TWL-MODE";
char* settingsini_twl_clock = "TWL_CLOCK";
char* settingsini_twl_bootani = "BOOT_ANIMATION";
char* settingsini_twl_hsmsg = "HEALTH&SAFETY_MSG";
char* settingsini_twl_launchslot1 = "LAUNCH_SLOT1";
char* settingsini_twl_resetslot1 = "RESET_SLOT1";
// End

CIniFile bootstrapini( "sdmc:/_nds/nds-bootstrap.ini" );

// Bootstrap .ini file
char* bootstrapini_ndsbootstrap = "NDS-BOOTSTRAP";
char* bootstrapini_ndspath = "NDS_PATH";
char* bootstrapini_debug = "DEBUG";
// End

// Frontend Settings text
char* settingstext = "Frontend Settings";

char* settings_colortext = "Color (app relaunch required)";

char* settings_colorvaluetext;
// End

int settings_colorvalue;

// NTR/TWL-mode Settings text
char* twlsettingstext = "NTR/TWL-mode Settings";

char* twlsettings_cpuspeedtext = "ARM9 CPU Speed";
char* twlsettings_bootscreentext = "DS/DSi Boot Screen";
char* twlsettings_healthsafetytext = "Health and Safety message";
char* twlsettings_resetslot1text = "Reset Slot-1";
char* twlsettings_consoletext = "Console output";
	
char* twlsettings_cpuspeedvaluetext;
char* twlsettings_bootscreenvaluetext;
char* twlsettings_healthsafetyvaluetext;
char* twlsettings_resetslot1valuetext;
char* twlsettings_consolevaluetext;
// End
	
int twlsettings_cpuspeedvalue;
int twlsettings_bootscreenvalue;
int twlsettings_healthsafetyvalue;
int twlsettings_resetslot1value;
int twlsettings_consolevalue;


void LoadColor() {
	if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 0) {
		topbgloc = "romfs:/assets/topbg/0-gray.png";
		color_Rvalue = 99;
		color_Gvalue = 127;
		color_Bvalue = 127;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 1) {
		topbgloc = "romfs:/assets/topbg/1-brown.png";
		color_Rvalue = 139;
		color_Gvalue = 99;
		color_Bvalue = 0;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 2) {
		topbgloc = "romfs:/assets/topbg/2-red.png";
		color_Rvalue = 255;
		color_Gvalue = 0;
		color_Bvalue = 0;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 3) {
		topbgloc = "romfs:/assets/topbg/3-pink.png";
		color_Rvalue = 255;
		color_Gvalue = 127;
		color_Bvalue = 127;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 4) {
		topbgloc = "romfs:/assets/topbg/4-orange.png";
		color_Rvalue = 169;
		color_Gvalue = 31;
		color_Bvalue = 0;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 5) {
		topbgloc = "romfs:/assets/topbg/5-yellow.png";
		color_Rvalue = 215;
		color_Gvalue = 215;
		color_Bvalue = 0;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 6) {
		topbgloc = "romfs:/assets/topbg/6-yellowgreen.png";
		color_Rvalue = 215;
		color_Gvalue = 255;
		color_Bvalue = 0;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 7) {
		topbgloc = "romfs:/assets/topbg/7-green1.png";
		color_Rvalue = 0;
		color_Gvalue = 255;
		color_Bvalue = 0;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 8) {
		topbgloc = "romfs:/assets/topbg/8-green2.png";
		color_Rvalue = 63;
		color_Gvalue = 255;
		color_Bvalue = 63;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 9) {
		topbgloc = "romfs:/assets/topbg/9-lightgreen.png";
		color_Rvalue = 31;
		color_Gvalue = 231;
		color_Bvalue = 31;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 10) {
		topbgloc = "romfs:/assets/topbg/10-skyblue.png";
		color_Rvalue = 0;
		color_Gvalue = 63;
		color_Bvalue = 255;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 11) {
		topbgloc = "romfs:/assets/topbg/11-lightblue.png";
		color_Rvalue = 63;
		color_Gvalue = 63;
		color_Bvalue = 255;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 12) {
		topbgloc = "romfs:/assets/topbg/12-blue.png";
		color_Rvalue = 0;
		color_Gvalue = 0;
		color_Bvalue = 255;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 13) {
		topbgloc = "romfs:/assets/topbg/13-violet.png";
		color_Rvalue = 127;
		color_Gvalue = 0;
		color_Bvalue = 255;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 14) {
		topbgloc = "romfs:/assets/topbg/14-purple.png";
		color_Rvalue = 255;
		color_Gvalue = 0;
		color_Bvalue = 255;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 15) {
		topbgloc = "romfs:/assets/topbg/15-fuschia.png";
		color_Rvalue = 255;
		color_Gvalue = 0;
		color_Bvalue = 127;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 16) {
		topbgloc = "romfs:/assets/topbg/16-red&blue.png";
		color_Rvalue = 255;
		color_Gvalue = 0;
		color_Bvalue = 255;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 17) {
		topbgloc = "romfs:/assets/topbg/17-green&yellow.png";
		color_Rvalue = 215;
		color_Gvalue = 215;
		color_Bvalue = 0;
	}
}

void LoadSettings() {
	if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 17) {
		settings_colorvalue = 17;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 16) {
		settings_colorvalue = 16;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 15) {
		settings_colorvalue = 15;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 14) {
		settings_colorvalue = 14;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 13) {
		settings_colorvalue = 13;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 12) {
		settings_colorvalue = 12;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 11) {
		settings_colorvalue = 11;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 10) {
		settings_colorvalue = 10;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 9) {
		settings_colorvalue = 9;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 8) {
		settings_colorvalue = 8;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 7) {
		settings_colorvalue = 7;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 6) {
		settings_colorvalue = 6;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 5) {
		settings_colorvalue = 5;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 4) {
		settings_colorvalue = 4;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 3) {
		settings_colorvalue = 3;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 2) {
		settings_colorvalue = 2;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 1) {
		settings_colorvalue = 1;
	} else {
		settings_colorvalue = 0;
	}
}

void SaveSettings() {
	settingsini.SetInt(settingsini_frontend, settingsini_frontend_color, settings_colorvalue);
	settingsini.SaveIniFile( "sdmc:/_nds/twloader/settings.ini");
}

void LoadTWLSettings() {
	if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_clock, 0) == 1) {
		twlsettings_cpuspeedvalue = 1;
	} else {
		twlsettings_cpuspeedvalue = 0;
	}
	if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_bootani, 0) == 1) {
		twlsettings_bootscreenvalue = 1;
	} else {
		twlsettings_bootscreenvalue = 0;
	}
	if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_hsmsg, 0) == 1) {
		twlsettings_healthsafetyvalue = 1;
	} else {
		twlsettings_healthsafetyvalue = 0;
	}
	if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_resetslot1, 0) == 1) {
		twlsettings_resetslot1value = 1;
	} else {
		twlsettings_resetslot1value = 0;
	}
	if (bootstrapini.GetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, 0) == 1) {
		twlsettings_resetslot1value = 2;
	} else if (bootstrapini.GetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, 0) == 0) {
		twlsettings_resetslot1value = 1;
	} else if (bootstrapini.GetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, 0) == -1) {
		twlsettings_resetslot1value = 0;
	}
}

void SaveTWLSettings() {
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_clock, twlsettings_cpuspeedvalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_bootani, twlsettings_bootscreenvalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_hsmsg, twlsettings_healthsafetyvalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_resetslot1, twlsettings_resetslot1value);

	if (twlsettings_consolevalue == 2) {
		bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, 1);
	} else if (twlsettings_consolevalue == 1) {
		bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, 0);
	} else {
		bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, -1);
	}
	settingsini.SaveIniFile( "sdmc:/_nds/twloader/settings.ini");
	bootstrapini.SaveIniFile( "sdmc:/_nds/nds-bootstrap.ini");
}

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

	//sf2d_texture *boxarttex = sfil_load_PNG_file("romfs:/assets/boxart_unknown.png", SF2D_PLACE_RAM);
	sf2d_texture *toptex = sfil_load_PNG_file("romfs:/assets/top.png", SF2D_PLACE_RAM);
	LoadColor();
	sf2d_texture *topbgtex = sfil_load_PNG_file(topbgloc, SF2D_PLACE_RAM);
	//sf2d_texture *vol0tex = sfil_load_PNG_file("romfs:/assets/volume0.png", SF2D_PLACE_RAM);
	//sf2d_texture *vol1tex = sfil_load_PNG_file("romfs:/assets/volume1.png", SF2D_PLACE_RAM);
	//sf2d_texture *vol2tex = sfil_load_PNG_file("romfs:/assets/volume2.png", SF2D_PLACE_RAM);
	//sf2d_texture *vol3tex = sfil_load_PNG_file("romfs:/assets/volume3.png", SF2D_PLACE_RAM);
	sf2d_texture *shoulderLtex = sfil_load_PNG_file("romfs:/assets/shoulder_L.png", SF2D_PLACE_RAM);
	sf2d_texture *shoulderRtex = sfil_load_PNG_file("romfs:/assets/shoulder_R.png", SF2D_PLACE_RAM);
	sf2d_texture *batterychrgtex = sfil_load_PNG_file("romfs:/assets/battery_charging.png", SF2D_PLACE_RAM);
	sf2d_texture *bottomtex = sfil_load_PNG_file("romfs:/assets/bottom.png", SF2D_PLACE_RAM);
	sf2d_texture *startbordertex = sfil_load_PNG_file("romfs:/assets/start_border.png", SF2D_PLACE_RAM);
	sf2d_texture *boxfulltex = sfil_load_PNG_file("romfs:/assets/box_full.png", SF2D_PLACE_RAM);
	sf2d_texture *bubbletex = sfil_load_PNG_file("romfs:/assets/bubble.png", SF2D_PLACE_RAM);
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
			u32 settingscursorPosition = 0, twlsettingscursorPosition = 0;
			
			char* rom = (char*)malloc(256);
			
			char* boxartpath = malloc(256);
			
			bool whileloop = true;
			
			//char* nickname = "Nickname";
			
			char* Lshouldertext;
			char* Rshouldertext;
			
			int LshoulderYpos = 220;
			int RshoulderYpos = 220;
			
			int filenameYpos;
			int titleboxXpos;
			int titleboxXmovepos = 0;
			//int titleboxXmovetimer = 0;
			//bool titleboxXmovelefton = false;
			//bool titleboxXmoverighton = false;
			
			int screenmode = 0;
			// 0: ROM select
			// 1: Settings

			int romselect_layout;
			// 0: File browser (Text only)
			// 1: DSi Menu
			if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_botlayout, 0) == 1) {
				romselect_layout = 1;
			} else {
				romselect_layout = 0;
			}
			int settings_subscreenmode = 0;
			// 0: Frontend settings
			// 1: NTR/TWL-mode settings
			
			int settingsXpos = 24;
			int settingsvalueXpos = 240;
			int settingsYpos;
		
			while(whileloop){
				if (screenmode == 0) {
					sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
					sf2d_draw_texture(bottomtex, 320/2 - bottomtex->width/2, 240/2 - bottomtex->height/2);
					
					if (romselect_layout == 0) {
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
									sftd_draw_textf(font, 10, filenameYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, files.at(i).c_str());
									filenameYpos += 12;
								} else {
									sftd_draw_textf(font, 10, filenameYpos, RGBA8(0, 0, 0, 255), 12, files.at(i).c_str());
									filenameYpos += 12;
								}
							}
						} else {
							for(i = 0; i < files.size(); i++){
								if(cursorPosition == i) {
									sftd_draw_textf(font, 10, filenameYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, files.at(i).c_str());
									filenameYpos += 12;
								} else {
									sftd_draw_textf(font, 10, filenameYpos, RGBA8(0, 0, 0, 255), 12, files.at(i).c_str());
									filenameYpos += 12;
								}
							}
						}
					} else {
						sf2d_draw_texture(bubbletex, 0, 0);
						sftd_draw_textf(font, 76, 220, RGBA8(0, 0, 0, 255), 13, "Start: Return to HOME Menu");

						filenameYpos = 0;
						titleboxXpos = 128;
						if(files.size() >= 29) {
							for(i = 0; i < 30; i++){
								sftd_draw_textf(font, 10, 8+filenameYpos-240*cursorPosition, RGBA8(127, 127, 127, 255), 12, files.at(i).c_str());
								sf2d_draw_texture(boxfulltex, titleboxXpos+titleboxXmovepos, 120);
								titleboxXpos += 80;
								filenameYpos += 240;
							}
						} else {
							for(i = 0; i < files.size(); i++){
								sftd_draw_textf(font, 10, 8+filenameYpos-240*cursorPosition, RGBA8(127, 127, 127, 255), 12, files.at(i).c_str());
								sf2d_draw_texture(boxfulltex, titleboxXpos+titleboxXmovepos, 120);
								titleboxXpos += 80;
								filenameYpos += 240;
							}
						}
						sf2d_draw_texture(startbordertex, 128, 116);
						sftd_draw_textf(font, 141, 176, RGBA8(255, 255, 255, 255), 12, "START");
					}
				} else if(screenmode == 1) {
					sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
					sf2d_draw_texture(bottomsettingstex, 320/2 - bottomsettingstex->width/2, 240/2 - bottomsettingstex->height/2);
					if (settings_subscreenmode == 0) {
						if (settings_colorvalue == 0) {
							settings_colorvaluetext = "Gray";
						} else if (settings_colorvalue == 1) {
							settings_colorvaluetext = "Brown";
						} else if (settings_colorvalue == 2) {
							settings_colorvaluetext = "Red";
						} else if (settings_colorvalue == 3) {
							settings_colorvaluetext = "Pink";
						} else if (settings_colorvalue == 4) {
							settings_colorvaluetext = "Orange";
						} else if (settings_colorvalue == 5) {
							settings_colorvaluetext = "Yellow";
						} else if (settings_colorvalue == 6) {
							settings_colorvaluetext = "Yellow-Green";
						} else if (settings_colorvalue == 7) {
							settings_colorvaluetext = "Green 1";
						} else if (settings_colorvalue == 8) {
							settings_colorvaluetext = "Green 2";
						} else if (settings_colorvalue == 9) {
							settings_colorvaluetext = "Light Green";
						} else if (settings_colorvalue == 10) {
							settings_colorvaluetext = "Sky Blue";
						} else if (settings_colorvalue == 11) {
							settings_colorvaluetext = "Light Blue";
						} else if (settings_colorvalue == 12) {
							settings_colorvaluetext = "Blue";
						} else if (settings_colorvalue == 13) {
							settings_colorvaluetext = "Violet";
						} else if (settings_colorvalue == 14) {
							settings_colorvaluetext = "Purple";
						} else if (settings_colorvalue == 15) {
							settings_colorvaluetext = "Fuschia";
						} else if (settings_colorvalue == 16) {
							settings_colorvaluetext = "Red & Blue";
						} else if (settings_colorvalue == 17) {
							settings_colorvaluetext = "Green & Yellow";
						}
						
						settingsYpos = 64;
						sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 16, settingstext);
						if(settingscursorPosition == 0) {
							sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, settings_colortext);
							sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, settings_colorvaluetext);
							settingsYpos += 12;
						} else {
							sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_colortext);
							sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_colorvaluetext);
							settingsYpos += 12;
						}
					} else if (settings_subscreenmode == 1) {
						if (twlsettings_cpuspeedvalue == 1) {
							twlsettings_cpuspeedvaluetext = "133mhz (TWL)";
						} else {
							twlsettings_cpuspeedvaluetext = "67mhz (NTR)";
						}
						if (twlsettings_bootscreenvalue == 1) {
							twlsettings_bootscreenvaluetext = "On";
						} else {
							twlsettings_bootscreenvaluetext = "Off";
						}
						if (twlsettings_healthsafetyvalue == 1) {
							twlsettings_healthsafetyvaluetext = "On";
						} else {
							twlsettings_healthsafetyvaluetext = "Off";
						}
						if (twlsettings_resetslot1value == 1) {
							twlsettings_resetslot1valuetext = "On";
						} else {
							twlsettings_resetslot1valuetext = "Off";
						}
						if (twlsettings_consolevalue == 1) {
							twlsettings_consolevaluetext = "On";
						} else if (twlsettings_consolevalue == 2) {
							twlsettings_consolevaluetext = "On (Debug)";
						} else {
							twlsettings_consolevaluetext = "Off";
						}
				
						settingsYpos = 64;
						sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 16, twlsettingstext);
						if(twlsettingscursorPosition == 0) {
							sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_cpuspeedtext);
							sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_cpuspeedvaluetext);
							settingsYpos += 12;
						} else {
							sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_cpuspeedtext);
							sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_cpuspeedvaluetext);
							settingsYpos += 12;
						}
						if(twlsettingscursorPosition == 1) {
							sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_bootscreentext);
							sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_bootscreenvaluetext);
							settingsYpos += 12;
						} else {
							sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_bootscreentext);
							sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_bootscreenvaluetext);
							settingsYpos += 12;
						}
						if(twlsettingscursorPosition == 2) {
							sftd_draw_textf(font, settingsXpos+16, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_healthsafetytext);
							sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_healthsafetyvaluetext);
							settingsYpos += 12;
						} else {
							sftd_draw_textf(font, settingsXpos+16, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_healthsafetytext);
							sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_healthsafetyvaluetext);
							settingsYpos += 12;
						}
						if(twlsettingscursorPosition == 3) {
							sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_resetslot1text);
							sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_resetslot1valuetext);
							settingsYpos += 12;
						} else {
							sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_resetslot1text);
							sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_resetslot1valuetext);
							settingsYpos += 12;
						}
						if(twlsettingscursorPosition == 4) {
							sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_consoletext);
							sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_consolevaluetext);
							settingsYpos += 12;
						} else {
							sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_consoletext);
							sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_consolevaluetext);
							settingsYpos += 12;
						}
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
					sf2d_draw_texture(shoulderLtex, 0, LshoulderYpos);
					sf2d_draw_texture(shoulderRtex, 336, RshoulderYpos);
					sftd_draw_textf(font, 17, LshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Lshouldertext);
					sftd_draw_textf(font, 340, RshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Rshouldertext);

					//if (ctr::battery::charging::level) {
					//	sf2d_draw_texture(batterychrgtex, 370, 2);
					//}
					sf2d_end_frame();
					
					sf2d_start_frame(GFX_TOP, GFX_RIGHT);
					sf2d_draw_texture(topbgtex, 430/2 - topbgtex->width/2, 240/2 - topbgtex->height/2);
					//sf2d_draw_texture(boxarttex, 408/2 - boxarttex->width/2, 240/2 - boxarttex->height/2);
					sf2d_draw_texture(toptex, 400/2 - toptex->width/2, 240/2 - toptex->height/2);
					//sftd_draw_textf(font, 24, 2, RGBA8(0, 0, 0, 255), 12, nickname);
					sf2d_draw_texture(shoulderLtex, -1, LshoulderYpos);
					sf2d_draw_texture(shoulderRtex, 335, RshoulderYpos);
					sftd_draw_textf(font, 16, LshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Lshouldertext);
					sftd_draw_textf(font, 339, RshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Rshouldertext);
					sf2d_end_frame();
					
					sf2d_swapbuffers();
					
					if(hHeld & KEY_L){
						if (LshoulderYpos != 223)
						{LshoulderYpos += 1;}
					} else {
						if (LshoulderYpos != 220)
						{LshoulderYpos -= 1;}
					}
					if(hHeld & KEY_R){
						if (RshoulderYpos != 223)
						{RshoulderYpos += 1;}
					} else {
						if (RshoulderYpos != 220)
						{RshoulderYpos -= 1;}
					}

					if (screenmode == 0) {
						Lshouldertext = "???";
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
							sf2d_free_texture(bubbletex);
							sf2d_free_texture(boxfulltex);
							sf2d_free_texture(startbordertex);
							sf2d_free_texture(bottomsettingstex);
							sf2d_fini();
							gfxExit();
							return 0;
						} 
						if (romselect_layout == 0) {
							Rshouldertext = "Boxes";
							if(hDown & KEY_R) {
								romselect_layout = 1;
								settingsini.SetInt(settingsini_frontend, settingsini_frontend_botlayout, romselect_layout);
								settingsini.SaveIniFile( "sdmc:/_nds/twloader/settings.ini");
								break;
							} else if(hDown & KEY_A){
								rom = (char*)(files.at(cursorPosition)).c_str();
								whileloop = false;
								settingsini.SetInt(settingsini_twlmode, settingsini_twl_launchslot1, 0);
								settingsini.SaveIniFile( "sdmc:/_nds/twloader/settings.ini");
								sf2d_swapbuffers();
								break;
							} else if((hDown & KEY_DOWN) && cursorPosition != 29){
								titleboxXmovepos -= 80;
								cursorPosition++;
								break;
							} else if((hDown & KEY_UP) && cursorPosition != 0){
								titleboxXmovepos += 80;
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
								screenmode = 1;
								if (settings_subscreenmode == 1) {
									LoadTWLSettings();
								} else {
									LoadSettings();
								}
								break;
							}
						} else {
							Rshouldertext = "Filenames";
							if(hDown & KEY_R) {
								romselect_layout = 0;
								settingsini.SetInt(settingsini_frontend, settingsini_frontend_botlayout, romselect_layout);
								settingsini.SaveIniFile( "sdmc:/_nds/twloader/settings.ini");
								break;
							} else if(hDown & KEY_A){
								rom = (char*)(files.at(cursorPosition)).c_str();
								whileloop = false;
								settingsini.SetInt(settingsini_twlmode, settingsini_twl_launchslot1, 0);
								settingsini.SaveIniFile( "sdmc:/_nds/twloader/settings.ini");
								sf2d_swapbuffers();
								break;
							} else if((hDown & KEY_RIGHT) && cursorPosition != 29){
								titleboxXmovepos -= 80;
								cursorPosition++;
								break;
							} else if((hDown & KEY_LEFT) && cursorPosition != 0){
								titleboxXmovepos += 80;
								cursorPosition--;
								break;
							} else if (hDown & KEY_SELECT) {
								screenmode = 1;
								if (settings_subscreenmode == 1) {
									LoadTWLSettings();
								} else {
									LoadSettings();
								}
								break;
							}
						}
					} else if (screenmode == 1) {
						Lshouldertext = "Frontend";
						Rshouldertext = "NTR/TWL";
						if (settings_subscreenmode == 1) {
							if(hDown & KEY_A){
								if (twlsettingscursorPosition == 0) {
									twlsettings_cpuspeedvalue++; // CPU speed
									if(twlsettings_cpuspeedvalue == 2) {
										twlsettings_cpuspeedvalue = 0;
									}
								} else if (twlsettingscursorPosition == 1) {
									twlsettings_bootscreenvalue++; // Boot screen
									if(twlsettings_bootscreenvalue == 2) {
										twlsettings_bootscreenvalue = 0;
									}
								} else if (twlsettingscursorPosition == 2) {
									twlsettings_healthsafetyvalue++; // H&S message
									if(twlsettings_healthsafetyvalue == 2) {
										twlsettings_healthsafetyvalue = 0;
									}
								} else if (twlsettingscursorPosition == 3) {
									twlsettings_resetslot1value++; // Reset Slot-1
									if(twlsettings_resetslot1value == 2) {
										twlsettings_resetslot1value = 0;
									}
								} else if (twlsettingscursorPosition == 4) {
									twlsettings_consolevalue++; // Console output
									if(twlsettings_consolevalue == 3) {
										twlsettings_consolevalue = 0;
									}
								}
								break;
							} else if((hDown & KEY_DOWN) && twlsettingscursorPosition != 4){
								twlsettingscursorPosition++;
								break;
							} else if((hDown & KEY_UP) && twlsettingscursorPosition != 0){
								twlsettingscursorPosition--;
								break;
							} else if(hDown & KEY_L){
								SaveTWLSettings();
								LoadSettings();
								settings_subscreenmode = 0;
								break;
							} else if(hDown & KEY_SELECT){
								SaveTWLSettings();
								screenmode = 0;
								break;
							}
						} else {
							if(hDown & KEY_A || hDown & KEY_RIGHT){
								if (settingscursorPosition == 0) {
									settings_colorvalue++; // Color
									if(settings_colorvalue == 18) {
										settings_colorvalue = 0;
									}
								} 
								break;
							} if(hDown & KEY_LEFT){
								if (settingscursorPosition == 0) {
									settings_colorvalue--; // Color
									if(settings_colorvalue == -1) {
										settings_colorvalue = 17;
									}
								} 
								break;
							} else if(hDown & KEY_R){
								SaveSettings();
								LoadTWLSettings();
								settings_subscreenmode = 1;
								break;
							} else if(hDown & KEY_SELECT){
								SaveSettings();
								screenmode = 0;
								break;
							}
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
	sf2d_free_texture(bubbletex);
	sf2d_free_texture(boxfulltex);
	sf2d_free_texture(startbordertex);
	sf2d_free_texture(bottomsettingstex);
    sf2d_fini();
	gfxExit();

    return 0;
}