#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#include <sftd.h>
#include <malloc.h>
#include <sys/stat.h>
//#include <citrus/app.hpp>
//#include <citrus/battery.hpp>
//#include <citrus/core.hpp>
//#include <citrus/fs.hpp>

#include "inifile.h"

u32 kDown;
u32 kHeld;

CIniFile settingsini( "sdmc:/_nds/twloader/settings.ini" );	

int color_Rvalue;
int color_Gvalue;
int color_Bvalue;
	
char* topbgloc;
char* boxartpath;
char* boxartfile;
char* boxartfile_fullpath;
int boxartnum = 0;
char* startborderloc;
	
// Settings .ini file
char* settingsini_frontend = "FRONTEND";
//char* settingsini_frontend_twlappinstalled = "TWLAPP_INSTALLED";
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
int twlsettings_launchslot1value;
int twlsettings_resetslot1value;
int twlsettings_consolevalue;


void LoadColor() {
	if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 0) {
		topbgloc = "romfs:/assets/topbg/0-gray.png";
		startborderloc = "romfs:/assets/start_border/0-gray.png";
		color_Rvalue = 99;
		color_Gvalue = 127;
		color_Bvalue = 127;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 1) {
		topbgloc = "romfs:/assets/topbg/1-brown.png";
		startborderloc = "romfs:/assets/start_border/1-brown.png";
		color_Rvalue = 139;
		color_Gvalue = 99;
		color_Bvalue = 0;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 2) {
		topbgloc = "romfs:/assets/topbg/2-red.png";
		startborderloc = "romfs:/assets/start_border/2-red.png";
		color_Rvalue = 255;
		color_Gvalue = 0;
		color_Bvalue = 0;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 3) {
		topbgloc = "romfs:/assets/topbg/3-pink.png";
		startborderloc = "romfs:/assets/start_border/3-pink.png";
		color_Rvalue = 255;
		color_Gvalue = 127;
		color_Bvalue = 127;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 4) {
		topbgloc = "romfs:/assets/topbg/4-orange.png";
		startborderloc = "romfs:/assets/start_border/4-orange.png";
		color_Rvalue = 169;
		color_Gvalue = 31;
		color_Bvalue = 0;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 5) {
		topbgloc = "romfs:/assets/topbg/5-yellow.png";
		startborderloc = "romfs:/assets/start_border/5-yellow.png";
		color_Rvalue = 215;
		color_Gvalue = 215;
		color_Bvalue = 0;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 6) {
		topbgloc = "romfs:/assets/topbg/6-yellowgreen.png";
		startborderloc = "romfs:/assets/start_border/6-yellowgreen.png";
		color_Rvalue = 215;
		color_Gvalue = 255;
		color_Bvalue = 0;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 7) {
		topbgloc = "romfs:/assets/topbg/7-green1.png";
		startborderloc = "romfs:/assets/start_border/7-green1.png";
		color_Rvalue = 0;
		color_Gvalue = 255;
		color_Bvalue = 0;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 8) {
		topbgloc = "romfs:/assets/topbg/8-green2.png";
		startborderloc = "romfs:/assets/start_border/8-green2.png";
		color_Rvalue = 63;
		color_Gvalue = 255;
		color_Bvalue = 63;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 9) {
		topbgloc = "romfs:/assets/topbg/9-lightgreen.png";
		startborderloc = "romfs:/assets/start_border/9-lightgreen.png";
		color_Rvalue = 31;
		color_Gvalue = 231;
		color_Bvalue = 31;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 10) {
		topbgloc = "romfs:/assets/topbg/10-skyblue.png";
		startborderloc = "romfs:/assets/start_border/10-skyblue.png";
		color_Rvalue = 0;
		color_Gvalue = 63;
		color_Bvalue = 255;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 11) {
		topbgloc = "romfs:/assets/topbg/11-lightblue.png";
		startborderloc = "romfs:/assets/start_border/11-lightblue.png";
		color_Rvalue = 63;
		color_Gvalue = 63;
		color_Bvalue = 255;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 12) {
		topbgloc = "romfs:/assets/topbg/12-blue.png";
		startborderloc = "romfs:/assets/start_border/12-blue.png";
		color_Rvalue = 0;
		color_Gvalue = 0;
		color_Bvalue = 255;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 13) {
		topbgloc = "romfs:/assets/topbg/13-violet.png";
		startborderloc = "romfs:/assets/start_border/13-violet.png";
		color_Rvalue = 127;
		color_Gvalue = 0;
		color_Bvalue = 255;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 14) {
		topbgloc = "romfs:/assets/topbg/14-purple.png";
		startborderloc = "romfs:/assets/start_border/14-purple.png";
		color_Rvalue = 255;
		color_Gvalue = 0;
		color_Bvalue = 255;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 15) {
		topbgloc = "romfs:/assets/topbg/15-fuschia.png";
		startborderloc = "romfs:/assets/start_border/15-fuschia.png";
		color_Rvalue = 255;
		color_Gvalue = 0;
		color_Bvalue = 127;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 16) {
		topbgloc = "romfs:/assets/topbg/16-red&blue.png";
		startborderloc = "romfs:/assets/start_border/16-red&blue.png";
		color_Rvalue = 255;
		color_Gvalue = 0;
		color_Bvalue = 255;
	} else if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0) == 17) {
		topbgloc = "romfs:/assets/topbg/17-green&yellow.png";
		startborderloc = "romfs:/assets/start_border/17-green&yellow.png";
		color_Rvalue = 215;
		color_Gvalue = 215;
		color_Bvalue = 0;
	}
}

void LoadBoxArtFile() {
	char* boxartfile_fullpath = malloc(256);
	strcat(boxartfile_fullpath, "sdmc:/_nds/twloader/boxart/");
	strcat(boxartfile_fullpath, boxartfile);

	if (fopen (boxartfile_fullpath+3, "rb") != NULL) {
		boxartpath = boxartfile_fullpath+3;
	} else {
		boxartpath = "romfs:/assets/boxart_unknown.png";
	} fclose (fopen (boxartfile_fullpath+3, "rb"));
	boxartnum++;
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
		twlsettings_consolevalue = 2;
	} else if (bootstrapini.GetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, 0) == 0) {
		twlsettings_consolevalue = 1;
	} else if (bootstrapini.GetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, 0) == -1) {
		twlsettings_consolevalue = 0;
	}
}

void SaveTWLSettings() {
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_clock, twlsettings_cpuspeedvalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_bootani, twlsettings_bootscreenvalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_hsmsg, twlsettings_healthsafetyvalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_launchslot1, twlsettings_launchslot1value);
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

#define CONFIG_3D_SLIDERSTATE (*(float *)0x1FF81080)

void screenoff()
{
    gspLcdInit();\
    GSPLCD_PowerOffBacklight(GSPLCD_SCREEN_BOTH);\
    gspLcdExit();
}

void screenon()
{
    gspLcdInit();\
    GSPLCD_PowerOnBacklight(GSPLCD_SCREEN_BOTH);\
    gspLcdExit();
}

int main()
{
	aptInit();
	amInit();
	sdmcInit();
	romfsInit();
	srvInit();
	hidInit();

	// making nds folder if it doesn't exist
	mkdir("sdmc:/nds", 0777);
	mkdir("sdmc:/_nds/twloader/tmp", 0777);

	//if (bootstrapini.GetInt(settingsini_frontend, settingsini_frontend_twlappinstalled, 0) == 0) {
	//	ctr::app::install(ctr::fs::NAND, "sdmc:/_nds/twloader/cia/twlapp.cia", 0, 1287328);
	//	bootstrapini.SetInt(settingsini_frontend, settingsini_frontend_twlappinstalled, 1);
	//	settingsini.SaveIniFile( "sdmc:/_nds/twloader/settings.ini");
	//}

	// We might need a way to install the SRL CIA from romfs
	//	ctr::app::install(ctr::fs::NAND, "romfs:/twloader-twl.cia", 0, u64 size);

	std::string	bootstrapPath = "";
	
    sf2d_init();
    sf2d_set_clear_color(RGBA8(0x00, 0x00, 0x00, 0x00));
	sf2d_set_3D(1);
	
	// Font loading
	sftd_init();
	sftd_font *font = sftd_load_font_file("romfs:/font.ttf");

	sf2d_texture *toptex = sfil_load_PNG_file("romfs:/assets/top.png", SF2D_PLACE_RAM); // Top DSi-Menu border
	LoadColor();
	sf2d_texture *topbgtex = sfil_load_PNG_file(topbgloc, SF2D_PLACE_RAM); // Top background, behind the DSi-Menu border
	//sf2d_texture *vol0tex = sfil_load_PNG_file("romfs:/assets/volume0.png", SF2D_PLACE_RAM);
	//sf2d_texture *vol1tex = sfil_load_PNG_file("romfs:/assets/volume1.png", SF2D_PLACE_RAM);
	//sf2d_texture *vol2tex = sfil_load_PNG_file("romfs:/assets/volume2.png", SF2D_PLACE_RAM);
	//sf2d_texture *vol3tex = sfil_load_PNG_file("romfs:/assets/volume3.png", SF2D_PLACE_RAM);
	sf2d_texture *shoulderLtex = sfil_load_PNG_file("romfs:/assets/shoulder_L.png", SF2D_PLACE_RAM); // L shoulder
	sf2d_texture *shoulderRtex = sfil_load_PNG_file("romfs:/assets/shoulder_R.png", SF2D_PLACE_RAM); // R shoulder
	//sf2d_texture *batterychrgtex = sfil_load_PNG_file("romfs:/assets/battery_charging.png", SF2D_PLACE_RAM);
	//sf2d_texture *battery0tex = sfil_load_PNG_file("romfs:/assets/battery0.png", SF2D_PLACE_RAM);
	//sf2d_texture *battery1tex = sfil_load_PNG_file("romfs:/assets/battery1.png", SF2D_PLACE_RAM);
	//sf2d_texture *battery2tex = sfil_load_PNG_file("romfs:/assets/battery2.png", SF2D_PLACE_RAM);
	//sf2d_texture *battery3tex = sfil_load_PNG_file("romfs:/assets/battery3.png", SF2D_PLACE_RAM);
	//sf2d_texture *battery4tex = sfil_load_PNG_file("romfs:/assets/battery4.png", SF2D_PLACE_RAM);
	//sf2d_texture *battery5tex = sfil_load_PNG_file("romfs:/assets/battery5.png", SF2D_PLACE_RAM);
	sf2d_texture *bottomtex = sfil_load_PNG_file("romfs:/assets/bottom.png", SF2D_PLACE_RAM); // Bottom of menu
	sf2d_texture *homeicontex = sfil_load_PNG_file("romfs:/assets/homeicon.png", SF2D_PLACE_RAM); // HOME icon
	sf2d_texture *bottomlogotex = sfil_load_PNG_file("romfs:/assets/bottom_logo.png", SF2D_PLACE_RAM); // TWLoader logo on bottom screen
	sf2d_texture *bottomcovertex = sfil_load_PNG_file("romfs:/assets/bottom_cover.png", SF2D_PLACE_RAM); // Image to cover selected game/app
	sf2d_texture *startbordertex = sfil_load_PNG_file(startborderloc, SF2D_PLACE_RAM); // "START" border
	sf2d_texture *carttex = sfil_load_PNG_file("romfs:/assets/cart.png", SF2D_PLACE_RAM); // Cartridge on bottom screen
	sf2d_texture *boxfulltex = sfil_load_PNG_file("romfs:/assets/box_full.png", SF2D_PLACE_RAM); // (DSiWare) box on bottom screen
	sf2d_texture *bubbletex = sfil_load_PNG_file("romfs:/assets/bubble.png", SF2D_PLACE_RAM); // Text bubble
	sf2d_texture *bottomsettingstex = sfil_load_PNG_file("romfs:/assets/bottom_settings.png", SF2D_PLACE_RAM); // Bottom of settings screen

	LoadSettings();
	LoadTWLSettings();

	std::vector<std::string> files = {};
	std::vector<std::string> boxartfiles = {};
	
	std::string extension_UCnds = ".NDS";
	std::string extension_LCnds = ".nds";
	std::string extension_oddnds1 = ".Nds";
	std::string extension_oddnds2 = ".nDs";
	std::string extension_oddnds3 = ".ndS";
	std::string extension_oddnds4 = ".NDs";
	std::string extension_oddnds5 = ".nDS";
	std::string extension_oddnds6 = ".NdS";
	std::string extension_png = ".png";
	
	std::string fat = "fat:/nds/";
	
	DIR *dir;
	DIR *boxartdir;
	struct dirent *ent;
	
	if ((boxartdir = opendir ("sdmc:/_nds/twloader/boxart/")) != NULL) {
	/* print all the files and directories within directory */
		while ((ent = readdir (boxartdir)) != NULL) {
			std::string bafname = (ent->d_name);
			if(bafname.find(extension_png, (bafname.length() - extension_png.length())) != std::string::npos)
				boxartfiles.push_back(ent->d_name);
		}
		closedir (boxartdir);
	}

	boxartfile = boxartfiles.at(boxartnum).c_str();
	//char* boxartfile = "sdmc:/_nds/twloader/boxart/(Demo) Mario Kart DS (U).nds.png";
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Now loading box art...");
	sf2d_end_frame();
	sf2d_swapbuffers();

	LoadBoxArtFile();
	sf2d_texture *boxarttex1 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	LoadBoxArtFile();
	sf2d_texture *boxarttex2 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	LoadBoxArtFile();
	sf2d_texture *boxarttex3 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	LoadBoxArtFile();
	sf2d_texture *boxarttex4 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	LoadBoxArtFile();
	sf2d_texture *boxarttex5 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	LoadBoxArtFile();
	sf2d_texture *boxarttex6 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	LoadBoxArtFile();
	sf2d_texture *boxarttex7 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	LoadBoxArtFile();
	sf2d_texture *boxarttex8 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	LoadBoxArtFile();
	sf2d_texture *boxarttex9 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	LoadBoxArtFile();
	sf2d_texture *boxarttex10 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art

	boxartnum = 0;
	sf2d_texture* boxarttexnum = boxarttex1;
	
	if ((dir = opendir ("sdmc:/nds/")) != NULL) {
	/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			std::string fname = (ent->d_name);
			if(fname.find(extension_UCnds, (fname.length() - extension_UCnds.length())) != std::string::npos)
				files.push_back(ent->d_name);
			if(fname.find(extension_LCnds, (fname.length() - extension_LCnds.length())) != std::string::npos)
				files.push_back(ent->d_name);
			if(fname.find(extension_oddnds1, (fname.length() - extension_oddnds1.length())) != std::string::npos)
				files.push_back(ent->d_name);
			if(fname.find(extension_oddnds2, (fname.length() - extension_oddnds2.length())) != std::string::npos)
				files.push_back(ent->d_name);
			if(fname.find(extension_oddnds3, (fname.length() - extension_oddnds3.length())) != std::string::npos)
				files.push_back(ent->d_name);
			if(fname.find(extension_oddnds4, (fname.length() - extension_oddnds4.length())) != std::string::npos)
				files.push_back(ent->d_name);
			if(fname.find(extension_oddnds5, (fname.length() - extension_oddnds5.length())) != std::string::npos)
				files.push_back(ent->d_name);
			if(fname.find(extension_oddnds6, (fname.length() - extension_oddnds6.length())) != std::string::npos)
				files.push_back(ent->d_name);
		}
		closedir (dir);
	}
		
	//boxartfile_fullpath = malloc(256);
	//strcat(boxartfile_fullpath, "sdmc:/_nds/twloader/boxart/");
	//strcat(boxartfile_fullpath, boxartfile);

	int cursorPosition = 0, i = 0;
	int settingscursorPosition = 0, twlsettingscursorPosition = 0;
	
	bool cursorPositionset = false;
		
	char* rom = (char*)malloc(256);
		
	int fadealpha = 255;
	bool fadein = true;
		
	bool updatebotscreen = true;
	bool applaunchprep = false;
	bool applaunchon = false;
		
	float offset3d_topbg = 0.0f;
	float offset3d_boxart = 0.0f;

	//char* nickname = "Nickname";
			
	char* Lshouldertext;
	char* Rshouldertext;
			
	int LshoulderYpos = 220;
	int RshoulderYpos = 220;
			
	int filenameYpos;
	int cartXpos = 64;
	int titleboxXpos;
	int titleboxXmovepos = 0;
	int titleboxXmovetimer = 1; // Set to 1 for fade-in effect to run
	bool titleboxXmoveleft = false;
	bool titleboxXmoveright = false;
	int titleboxYmovepos = 120;
	
	int startbordermovepos;
	float startborderscalesize;
			
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
		
		u32 hDown = hidKeysDown();
		u32 hHeld = hidKeysHeld();
		
		offset3d_topbg = CONFIG_3D_SLIDERSTATE * 24.0f;
		offset3d_boxart = CONFIG_3D_SLIDERSTATE * 10.0f;
		
	if (boxartnum == -1) {
		boxartnum = 9;
		boxarttexnum = boxarttex10;
	} else if (boxartnum == 0) {
		boxarttexnum = boxarttex1;
	} else if (boxartnum == 1) {
		boxarttexnum = boxarttex2;
	} else if (boxartnum == 2) {
		boxarttexnum = boxarttex3;
	} else if (boxartnum == 3) {
		boxarttexnum = boxarttex4;
	} else if (boxartnum == 4) {
		boxarttexnum = boxarttex5;
	} else if (boxartnum == 5) {
		boxarttexnum = boxarttex6;
	} else if (boxartnum == 6) {
		boxarttexnum = boxarttex7;
	} else if (boxartnum == 7) {
		boxarttexnum = boxarttex8;
	} else if (boxartnum == 8) {
		boxarttexnum = boxarttex9;
	} else if (boxartnum == 9) {
		boxarttexnum = boxarttex10;
	} else if (boxartnum == 10) {
		boxartnum = 0;
		boxarttexnum = boxarttex1;
	}

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		//Draws a 100x100 yellow rectangle (255, 255, 00, 255) at (150, 70)
		//sf2d_draw_rectangle(150, 70, 100, 100, RGBA8(0xFF, 0xFF, 0x00, 0xFF));

		sf2d_draw_texture(topbgtex, 400/2 - topbgtex->width/2, 240/2 - topbgtex->height/2);
		sf2d_draw_texture(boxarttexnum, 400/2 - boxarttexnum->width/2, 240/2 - boxarttexnum->height/2); // Draw box art
		sf2d_draw_texture_scale_blend(boxarttexnum, 400/2 - boxarttexnum->width/2, 264, 1, -0.75, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 0xB0)); // Draw box art's reflection
		sf2d_draw_texture(toptex, 400/2 - toptex->width/2, 240/2 - toptex->height/2);
		//sftd_draw_textf(font, 24, 2, RGBA8(0, 0, 0, 255), 12, nickname);
		//sftd_draw_textf(font, 2, 2, RGBA8(0, 0, 0, 255), 12, boxartfile_fullpath+3); // Debug text
		sf2d_draw_texture(shoulderLtex, 0, LshoulderYpos);
		sf2d_draw_texture(shoulderRtex, 336, RshoulderYpos);
		sftd_draw_textf(font, 17, LshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Lshouldertext);
		sftd_draw_textf(font, 340, RshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Rshouldertext);

		sf2d_draw_rectangle(0, 0, 400, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect

		//if (ctr::battery::charging == true) {
		//	sf2d_draw_texture(batterychrgtex, 370, 2);
		//} else if (ctr::battery::level >= 0x80) {
		//	sf2d_draw_texture(battery4tex, 370, 2);
		//}
		sf2d_end_frame();
			
		sf2d_start_frame(GFX_TOP, GFX_RIGHT);
		sf2d_draw_texture(topbgtex, offset3d_topbg + 400/2 - topbgtex->width/2, 240/2 - topbgtex->height/2);
		sf2d_draw_texture(boxarttexnum, offset3d_boxart + 400/2 - boxarttexnum->width/2, 240/2 - boxarttexnum->height/2); // Draw box art
		sf2d_draw_texture_scale_blend(boxarttexnum, offset3d_boxart + 400/2 - boxarttexnum->width/2, 264, 1, -0.75, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 0xB0)); // Draw box art's reflection
		sf2d_draw_texture(toptex, 400/2 - toptex->width/2, 240/2 - toptex->height/2);
		//sftd_draw_textf(font, 24, 2, RGBA8(0, 0, 0, 255), 12, nickname);
		//sftd_draw_textf(font, 2, 2, RGBA8(0, 0, 0, 255), 12, boxartfile_fullpath+3); // Debug text
		sf2d_draw_texture(shoulderLtex, -1, LshoulderYpos);
		sf2d_draw_texture(shoulderRtex, 335, RshoulderYpos);
		sftd_draw_textf(font, 16, LshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Lshouldertext);
		sftd_draw_textf(font, 339, RshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Rshouldertext);
		
		sf2d_draw_rectangle(0, 0, 400, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect
		
		//if (ctr::battery::charging == true) {
		//	sf2d_draw_texture(batterychrgtex, 370, 2);
		//} else if (ctr::battery::level >= 0x80) {
		//	sf2d_draw_texture(battery4tex, 370, 2);
		//}
		sf2d_end_frame();
					
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
		
		if (fadein == true) {
			fadealpha -= 31;
			if (fadealpha < 0) {
				fadealpha = 0;
				fadein = false;
				titleboxXmovetimer = 0;
			}
		}

		if(titleboxXmoveleft == true) {
			titleboxXmovetimer += 1;
			if (titleboxXmovetimer == 10) {
				titleboxXmovetimer = 0;
				boxartnum--;
				titleboxXmoveleft = false;
			} else if (titleboxXmovetimer == 9) {
				// Delay a frame
			} else if (titleboxXmovetimer == 8) {
				titleboxXmovepos += 8;
				startbordermovepos = 1;
				startborderscalesize = 0.97;
				cursorPositionset = false;
			} else {
				if (cursorPositionset == false) {
					cursorPosition--;
					cursorPositionset = true;
				}
				titleboxXmovepos += 8;
			}
		} else if(titleboxXmoveright == true) {
			titleboxXmovetimer += 1;
			if (titleboxXmovetimer == 10) {
				titleboxXmovetimer = 0;
				boxartnum++;
				titleboxXmoveright = false;
			} else if (titleboxXmovetimer == 9) {
				// Delay a frame
			} else if (titleboxXmovetimer == 8) {
				titleboxXmovepos -= 8;
				startbordermovepos = 1;
				startborderscalesize = 0.97;
				cursorPositionset = false;
			} else {
				if (cursorPositionset == false) {
					cursorPosition++;
					cursorPositionset = true;
				}
				if (cursorPosition != i) {
					titleboxXmovepos -= 8;
				} else {
					titleboxXmovetimer = 0;
					titleboxXmoveright = false;
					cursorPositionset = false;
					cursorPosition--;
				}
			}
		}
		if(applaunchprep == true) {
			titleboxYmovepos -= 6;
			if (titleboxYmovepos == -240) {
				screenoff();
				SaveSettings();
				SaveTWLSettings();
				applaunchon = true;
			}
			fadealpha += 6;
			if (fadealpha > 255) {
				fadealpha = 255;
			}
		}

		if(updatebotscreen == true){
			if (screenmode == 0) {
				sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				sf2d_draw_texture(bottomtex, 320/2 - bottomtex->width/2, 240/2 - bottomtex->height/2);
				
				if (romselect_layout == 0) {
					filenameYpos = 0;
					if(files.size() >= 29) {
						for(i = 0; i < 30; i++){
							if(cursorPosition == i) {
								sftd_draw_textf(font, 10, filenameYpos-2*cursorPosition, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, files.at(i).c_str());
								filenameYpos += 12;
							} else {
								sftd_draw_textf(font, 10, filenameYpos-2*cursorPosition, RGBA8(0, 0, 0, 255), 12, files.at(i).c_str());
								filenameYpos += 12;
							}
						}
					} else {
						for(i = 0; i < files.size(); i++){
							if(cursorPosition == i) {
								sftd_draw_textf(font, 10, filenameYpos-2*cursorPosition, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, files.at(i).c_str());
								filenameYpos += 12;
							} else {
								sftd_draw_textf(font, 10, filenameYpos-2*cursorPosition, RGBA8(0, 0, 0, 255), 12, files.at(i).c_str());
								filenameYpos += 12;
							}
						}
					}
				} else {
					if (titleboxXmovetimer == 0) {
						sf2d_draw_texture(bubbletex, 0, 0);
					} else {
						sf2d_draw_texture(bottomlogotex, 320/2 - bottomlogotex->width/2, 40);
					}
					sf2d_draw_texture(homeicontex, 75, 220); // Draw HOME icon
					sftd_draw_textf(font, 92, 220, RGBA8(0, 0, 0, 255), 14, ": Return to HOME Menu");
					sf2d_draw_texture(carttex, cartXpos+titleboxXmovepos, 120);

					filenameYpos = 0;
					titleboxXpos = 128;
					if(files.size() >= 29) {
						for(i = 0; i < 30; i++){
							if (titleboxXmovetimer == 0) {
								if (cursorPosition == -1) {
									sftd_draw_textf(font, 10, 8, RGBA8(127, 127, 127, 255), 12, "Slot-1 cart (NTR carts only)");
								} else {
									sftd_draw_textf(font, 10, 8+filenameYpos-240*cursorPosition, RGBA8(127, 127, 127, 255), 12, files.at(i).c_str());
								}
							}
							sf2d_draw_texture(boxfulltex, titleboxXpos+titleboxXmovepos, 120);
							titleboxXpos += 64;
							filenameYpos += 240;
						}
					} else {
						for(i = 0; i < files.size(); i++){
							if (titleboxXmovetimer == 0) {
								if (cursorPosition == -1) {
									sftd_draw_textf(font, 10, 8, RGBA8(127, 127, 127, 255), 12, "Slot-1 cart (NTR carts only)");
								} else {
									sftd_draw_textf(font, 10, 8+filenameYpos-240*cursorPosition, RGBA8(127, 127, 127, 255), 12, files.at(i).c_str());
								}
							}
							sf2d_draw_texture(boxfulltex, titleboxXpos+titleboxXmovepos, 120);
							titleboxXpos += 64;
							filenameYpos += 240;
						}
					}
					if (applaunchprep == false) {
						if (titleboxXmovetimer == 0) {
							startbordermovepos = 0;
							startborderscalesize = 1.0;
						}
						sf2d_draw_texture_scale(startbordertex, 128+startbordermovepos, 116+startbordermovepos, startborderscalesize, startborderscalesize);
						sftd_draw_textf(font, 141, 176, RGBA8(255, 255, 255, 255), 12, "START");
					} else {
						sf2d_draw_texture(bottomcovertex, 128, 116);  // Cover selected game/app
						if (cursorPosition == -1) {
							sf2d_draw_texture(carttex, 128, titleboxYmovepos); // Draw selected Slot-1 game that moves up
						} else {
							sf2d_draw_texture(boxfulltex, 128, titleboxYmovepos); // Draw selected game/app that moves up
						}
					}
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
						sftd_draw_textf(font, 8, 200, RGBA8(255, 255, 255, 255), 13, "Set to TWL to get rid of lags in some games.");
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_cpuspeedtext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_cpuspeedvaluetext);
						settingsYpos += 12;
					}
					if(twlsettingscursorPosition == 1) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_bootscreentext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_bootscreenvaluetext);
						settingsYpos += 12;
						sftd_draw_textf(font, 8, 200, RGBA8(255, 255, 255, 255), 13, "Displays the DS/DSi boot animation");
						sftd_draw_textf(font, 8, 214, RGBA8(255, 255, 255, 255), 13, "before launched game.");
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_bootscreentext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_bootscreenvaluetext);
						settingsYpos += 12;
					}
					if(twlsettingscursorPosition == 2) {
						sftd_draw_textf(font, settingsXpos+16, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_healthsafetytext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_healthsafetyvaluetext);
						settingsYpos += 12;
						sftd_draw_textf(font, 8, 200, RGBA8(255, 255, 255, 255), 13, "Displays the Health and Safety");
						sftd_draw_textf(font, 8, 214, RGBA8(255, 255, 255, 255), 13, "message on the bottom screen.");
					} else {
						sftd_draw_textf(font, settingsXpos+16, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_healthsafetytext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_healthsafetyvaluetext);
					settingsYpos += 12;
					}
					if(twlsettingscursorPosition == 3) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_resetslot1text);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_resetslot1valuetext);
						settingsYpos += 12;
						sftd_draw_textf(font, 8, 200, RGBA8(255, 255, 255, 255), 13, "Enable this if Slot-1 carts are stuck");
						sftd_draw_textf(font, 8, 214, RGBA8(255, 255, 255, 255), 13, "on white screens.");
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_resetslot1text);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_resetslot1valuetext);
						settingsYpos += 12;
					}
					if(twlsettingscursorPosition == 4) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_consoletext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_consolevaluetext);
						settingsYpos += 12;
						sftd_draw_textf(font, 8, 200, RGBA8(255, 255, 255, 255), 13, "Displays some text before launched game.");
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_consoletext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_consolevaluetext);
						settingsYpos += 12;
					}
				}
			}
		sf2d_draw_rectangle(0, 0, 320, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect

		sf2d_end_frame();
		}
		
		sf2d_swapbuffers();
		
		if (titleboxXmovetimer == 0) {
			updatebotscreen = false;
		}
		if (screenmode == 0) {
			Lshouldertext = "???";
			if (romselect_layout == 0) {
				Rshouldertext = "Boxes";
				if(cursorPosition == -1) {
					titleboxXmovepos -= 64;
					cursorPosition = 0;
					updatebotscreen = true;
				}
				if(hDown & KEY_R) {
					romselect_layout = 1;
					settingsini.SetInt(settingsini_frontend, settingsini_frontend_botlayout, romselect_layout);
					settingsini.SaveIniFile( "sdmc:/_nds/twloader/settings.ini");
					updatebotscreen = true;
				} else if(hDown & KEY_A){
					twlsettings_launchslot1value = 0;
					screenoff();
					SaveSettings();
					SaveTWLSettings();
					rom = (char*)(files.at(cursorPosition)).c_str();
					applaunchon = true;
					updatebotscreen = true;
				} else if((hDown & KEY_DOWN) && cursorPosition != i){
					titleboxXmovepos -= 64;
					cursorPosition++;
					boxartnum++;
					if (cursorPosition == i) {
						titleboxXmovepos += 64;
						cursorPosition--;
						boxartnum--;
					}
					//if (boxarttex == boxarttex1) {
					//	boxarttex = boxarttex2;
					//} else if (boxarttex == boxarttex2) {
					//	boxarttex = boxarttex1;
					//}
					//boxartload = true;
					updatebotscreen = true;
				} else if((hDown & KEY_UP) && cursorPosition != 0){
					titleboxXmovepos += 64;
					cursorPosition--;
					boxartnum--;
					//if (boxarttex == boxarttex1) {
					//	boxarttex = boxarttex2;
					//} else if (boxarttex == boxarttex2) {
					//	boxarttex = boxarttex1;
					//}
					//boxartload = true;
					updatebotscreen = true;
				} else if(hDown & KEY_X) {
					twlsettings_launchslot1value = 1;
					screenoff();
					SaveSettings();
					SaveTWLSettings();
					applaunchon = true;
					updatebotscreen = true;
				} else if (hDown & KEY_SELECT) {
					screenmode = 1;
					updatebotscreen = true;
				}
			} else {
				Rshouldertext = "Filenames";
				startbordermovepos = 0;
				startborderscalesize = 1.0;
				if(applaunchprep == false) {
					if(hDown & KEY_R) {
						if (titleboxXmovetimer == 0) {
							romselect_layout = 0;
							settingsini.SetInt(settingsini_frontend, settingsini_frontend_botlayout, romselect_layout);
							settingsini.SaveIniFile( "sdmc:/_nds/twloader/settings.ini");
						}
						updatebotscreen = true;
					} else if(hDown & KEY_A){
						if (titleboxXmovetimer == 0) {
							if(cursorPosition == -1) {
								titleboxXmovetimer = 1;
								twlsettings_launchslot1value = 1;
								applaunchprep = true;
							} else {
								titleboxXmovetimer = 1;
								rom = (char*)(files.at(cursorPosition)).c_str();
								twlsettings_launchslot1value = 0;
								applaunchprep = true;
							}
						}
						updatebotscreen = true;
					} else if((hHeld & KEY_RIGHT) && cursorPosition != i){
						//titleboxXmovepos -= 64;
						if (titleboxXmoveleft == false) {
							titleboxXmoveright = true;
						}
						updatebotscreen = true;
					} else if((hHeld & KEY_LEFT) && cursorPosition != -1){
						//titleboxXmovepos += 64;
						if (titleboxXmoveright == false) {
							titleboxXmoveleft = true;
						}
						updatebotscreen = true;
					} else if (hDown & KEY_SELECT) {
						screenmode = 1;
						updatebotscreen = true;
					}
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
					updatebotscreen = true;
				} else if((hDown & KEY_DOWN) && twlsettingscursorPosition != 4){
					twlsettingscursorPosition++;
					updatebotscreen = true;
				} else if((hDown & KEY_UP) && twlsettingscursorPosition != 0){
					twlsettingscursorPosition--;
					updatebotscreen = true;
				} else if(hDown & KEY_L){
					settings_subscreenmode = 0;
					updatebotscreen = true;
				} else if(hDown & KEY_SELECT){
					screenmode = 0;
					updatebotscreen = true;
				}
			} else {
				if(hDown & KEY_A || hDown & KEY_RIGHT){
					if (settingscursorPosition == 0) {
						settings_colorvalue++; // Color
						if(settings_colorvalue == 18) {
							settings_colorvalue = 0;
						}
					} 
					updatebotscreen = true;
				} if(hDown & KEY_LEFT){
					if (settingscursorPosition == 0) {
						settings_colorvalue--; // Color
						if(settings_colorvalue == -1) {
							settings_colorvalue = 17;
						}
					} 
					updatebotscreen = true;
				} else if(hDown & KEY_R){
					settings_subscreenmode = 1;
					updatebotscreen = true;
				} else if(hDown & KEY_SELECT){
					screenmode = 0;
					updatebotscreen = true;
				}
			}
		}

		while(applaunchon){
			// Set ROM path if ROM is selected
			if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_launchslot1, 0) == 0) {
				bootstrapini.SetString(bootstrapini_ndsbootstrap, bootstrapini_ndspath,fat+rom);
				bootstrapini.SaveIniFile( "sdmc:/_nds/nds-bootstrap.ini");
			}
			// Prepare for the app launch
			APT_PrepareToDoApplicationJump(0, 0x0004800554574C44LL, 0); // TWL app's title ID
			// Tell APT to trigger the app launch and set the status of this app to exit
			APT_DoApplicationJump(param, sizeof(param), hmac);
		}
	}

	
	SaveSettings();
	SaveTWLSettings();
	hidExit();
	srvExit();
	romfsExit();
	sdmcExit();
	aptExit();
	sf2d_free_texture(topbgtex);
	sf2d_free_texture(toptex);
	sf2d_free_texture(shoulderLtex);
	sf2d_free_texture(shoulderRtex);
	//sf2d_free_texture(batterychrgtex);
	sf2d_free_texture(bottomtex);
	sf2d_free_texture(homeicontex);
	sf2d_free_texture(bottomlogotex);
	sf2d_free_texture(bottomcovertex);
	sf2d_free_texture(bubbletex);
	sf2d_free_texture(carttex);
	sf2d_free_texture(boxfulltex);
	sf2d_free_texture(startbordertex);
	sf2d_free_texture(bottomsettingstex);
	//sf2d_free_texture(boxartshadowtex);
	sf2d_free_texture(boxarttex1);
	sf2d_free_texture(boxarttex2);
	sf2d_free_texture(boxarttex3);
	sf2d_free_texture(boxarttex4);
	sf2d_free_texture(boxarttex5);
	sf2d_free_texture(boxarttex6);
	sf2d_free_texture(boxarttex7);
	sf2d_free_texture(boxarttex8);
	sf2d_free_texture(boxarttex9);
	sf2d_free_texture(boxarttex10);
    sf2d_fini();

    return 0;
}