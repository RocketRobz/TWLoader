#ifndef TWLOADER_MAIN_H
#define TWLOADER_MAIN_H

#include "graphic.h"
#include "pp2d/pp2d.h"

// Variables exported from main.cpp.
#include "sound.h"

#include <3ds/types.h>
#include <string>
#include <vector>

// Theme numbers
#define THEME_DSIMENU 0	// DSi Menu
#define THEME_3DSMENU 1	// 3DS Menu
#define THEME_R4 2		// R4
#define THEME_AKMENU 3	// Wood/akMenu

#define gamesPerPage 40


extern bool isDemo;	// Is the program running as demo version?
extern bool run;	// Set to false to exit to the Home Menu.
extern bool isNightly;


extern std::string homebrew_arg;

extern std::string fat;

// Dialog box
extern void DialogBoxAppear(int x, int y, const char *text);
extern void DialogBoxDisappear(int x, int y, const char *text);
extern bool showdialogbox;

// Shoulder button images.
extern const char* Lshouldertext;
extern const char* Rshouldertext;
extern int LshoulderYpos;
extern int RshoulderYpos;

// Status bar functions.
void draw_volume_slider(size_t texnum);
extern int batteryFrame;
void update_battery_level(size_t texnum);

extern bool showAnniversaryText;

// Screen effects.
extern int fadealpha;
extern bool fadein;
extern bool fadeout;
extern void screenoff(void);
extern void screenon(void);

extern int titleboxXmovetimer; // Set to 1 for fade-in effect to run

// Settings
extern std::string settings_releasebootstrapver;
extern std::string settings_unofficialbootstrapver;
extern std::string settings_SDK5releasebootstrapver;
extern std::string settings_SDK5unofficialbootstrapver;
extern char settings_vertext[13];

extern const u64 TWLNAND_TID;	// TWLNAND title ID.
extern const u64 NTRLAUNCHER_TID;	// TWLoader's NTR Launcher Title ID.
extern bool checkTWLNANDSide(void);
extern bool checkTWLNANDSide2(void);

// Current screen mode.
enum ScreenMode {
	SCREEN_MODE_ROM_SELECT = 0,	// ROM Select
	SCREEN_MODE_SETTINGS = 1,	// Settings
};
extern ScreenMode screenmode;

extern int TWLNANDnotfound_msg;

// Files
extern std::vector<std::string> files;
extern std::vector<std::string> fcfiles;

// Logging
extern bool logEnabled;

// Sound effects
extern bool dspfirmfound;
extern sound *sfx_launch;
extern sound *sfx_select;
extern sound *sfx_switch;
extern sound *sfx_wrong;
extern sound *sfx_back;

// Game start configuration.
extern bool keepsdvalue;
extern int gbarunnervalue;

#endif /* TWLOADER_MAIN_H */
