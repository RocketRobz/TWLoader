// for strcasestr()
#define _GNU_SOURCE 1
#include "main.h"

#include "download.h"
#include "settings.h"
#include "textfns.h"
#include "language.h"
#include "gamecard.h"
#include "rmkdir.h"

#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#include "img/twpng.h"
#include <sftd.h>
#include "ptmu_x.h"

//#include <citrus/app.hpp>
//#include <citrus/battery.hpp>
//#include <citrus/core.hpp>
//#include <citrus/fs.hpp>

#include <algorithm>
#include <string>
#include <vector>
using std::string;
using std::vector;
using std::wstring;

#include "sound.h"
#include "inifile.h"
#include "date.h"
#include "log.h"
#include "keyboard.h"
#define CONFIG_3D_SLIDERSTATE (*(float *)0x1FF81080)

bool is3DSX = COMPILE_3DSX;

#include "logo_png.h"
#include "logo_demo_png.h"

static touchPosition touch;

bool menuaction_nextpage = false;
bool menuaction_prevpage = false;
bool menuaction_launch = false;
bool menudboxaction_switchloc = false;

CIniFile bootstrapini( "sdmc:/_nds/nds-bootstrap.ini" );
#include "ndsheaderbanner.h"

int equals;

sftd_font *font;
sftd_font *font_b;

// Dialog box textures.
sf2d_texture *dialogboxtex;	// Dialog box
static sf2d_texture *dboxtex_iconbox = NULL;
static sf2d_texture *dboxtex_button = NULL;
static sf2d_texture *dboxtex_buttonback = NULL;

sf2d_texture *settingslogotex;	// TWLoader logo.

static sf2d_texture *slot1boxarttex = NULL;

std::string	bootstrapPath;

// Current screen mode.
ScreenMode screenmode = SCREEN_MODE_ROM_SELECT;

/* enum RomSelect_Mode {	// R4 Theme only
	ROMSEL_MODE_MENU = 0,	// Menu
	ROMSEL_MODE_ROMSEL = 1,	// ROM Select
};
RomSelect_Mode menudboxmode = DBOX_MODE_OPTIONS; */

enum MenuDBox_Mode {
	DBOX_MODE_OPTIONS = 0,	// Options
	DBOX_MODE_SETTINGS = 1,	// Game Settings
};
MenuDBox_Mode menudboxmode = DBOX_MODE_OPTIONS;

enum Menu_ControlSet {
	CTRL_SET_MENU = 0,	// Menu (R4 Theme only)
	CTRL_SET_GAMESEL = 1,	// ROM Select
	CTRL_SET_DBOX = 2,	// Dialog box mode
};
Menu_ControlSet menu_ctrlset = CTRL_SET_GAMESEL;

static sf2d_texture *bnricontexnum = NULL;
static sf2d_texture *bnricontexlaunch = NULL;	// DO NOT FREE; points to bnricontex[]
static sf2d_texture *boxarttexnum = NULL;

// Banners and boxart. (formerly bannerandboxart.h)
// bnricontex[]: 0-19
static sf2d_texture *bnricontex[20] = { };
// boxartpath[]: 0-19; 20 is for blank boxart only
static char* boxartpath[21] = { };
static sf2d_texture *boxarttex[6] = { };

int bnriconnum = 0;
int bnriconframenum = 0;
int boxartnum = 0;
int pagenum = 0;
const char* temptext;
const char* musicpath = "romfs:/null.wav";


// Shoulder buttons.
sf2d_texture *shoulderLtex = NULL;
sf2d_texture *shoulderRtex = NULL;
// sf2d_texture *shoulderYtex = NULL;
// sf2d_texture *shoulderXtex = NULL;

const char* Lshouldertext = "";
const char* Rshouldertext = "";

int LshoulderYpos = 220;
int RshoulderYpos = 220;
// int YbuttonYpos = 220;
// int XbuttonYpos = 220;


// Sound effects.
sound *bgm_menu = NULL;
//sound *bgm_settings = NULL;
sound *sfx_launch = NULL;
sound *sfx_select = NULL;
sound *sfx_stop = NULL;
sound *sfx_switch = NULL;
sound *sfx_wrong = NULL;
sound *sfx_back = NULL;


// Title box animation.
static int titleboxXpos = 0;
static int titleboxXmovepos = 0;
static bool titleboxXmoveleft = false;
static bool titleboxXmoveright = false;
static int titleboxYmovepos = 120;
int titleboxXmovetimer = 1; // Set to 1 for fade-in effect to run

static bool wood_uppressed = false;
static bool wood_downpressed = false;


static const char fcrompathini_flashcardrom[] = "FLASHCARD-ROM";
static const char fcrompathini_rompath[] = "NDS_PATH";
static const char fcrompathini_tid[] = "TID";
static const char fcrompathini_bnriconaniseq[] = "BNR_ICONANISEQ";
	

// Bootstrap .ini file
static const char bootstrapini_ndsbootstrap[] = "NDS-BOOTSTRAP";
static const char bootstrapini_ndspath[] = "NDS_PATH";
static const char bootstrapini_savpath[] = "SAV_PATH";
static const char bootstrapini_mpuregion[] = "PATCH_MPU_REGION";
static const char bootstrapini_mpusize[] = "PATCH_MPU_SIZE";
static const char bootstrapini_boostcpu[] = "BOOST_CPU";
static const char bootstrapini_debug[] = "DEBUG";
static const char bootstrapini_lockarm9scfgext[] = "LOCK_ARM9_SCFG_EXT";
static const char bootstrapini_bootstrappath[] = "BOOTSTRAP_PATH";
// End

// Run
bool run = true;
// End

bool showdialogbox = false;
bool showdialogbox_menu = false;	// for Game Select menu
int menudbox_movespeed = 22;
int menudbox_Ypos = -240;
int menudbox_bgalpha = 0;

const char* noromtext1;
const char* noromtext2;

int RGB[3]; // Pergame LED

// Version numbers.

char settings_latestvertext[13];
char settings_vertext[13];

std::string settings_releasebootstrapver;
std::string settings_unofficialbootstrapver;

static bool applaunchprep = false;
static bool applaunchon = false;

int fadealpha = 255;
bool fadein = true;
bool fadeout = false;

static const char* romsel_filename;
static wstring romsel_filename_w;	// Unicode filename for display.
static vector<wstring> romsel_gameline;	// from banner

static const char* rom = "";		// Selected ROM image.
std::string sav;		// Associated save file.

// These are used by flashcard functions and must retain their trailing slash.
static const std::string sdmc = "sdmc:/";
static const std::string fat = "fat:/";
static const std::string slashchar = "/";
static const std::string woodfat = "fat0:/";
static const std::string dstwofat = "fat1:/";

static const char bnriconfolder[] = "sdmc:/_nds/twloader/bnricons";
static const char fcbnriconfolder[] = "sdmc:/_nds/twloader/bnricons/flashcard";
static const char boxartfolder[] = "sdmc:/_nds/twloader/boxart";
static const char fcboxartfolder[] = "sdmc:/_nds/twloader/boxart/flashcard";
// End
	
bool keepsdvalue = false;
int gbarunnervalue = 0;

bool logEnabled = false;

static std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}


static inline void screenoff(void)
{
    gspLcdInit();
    GSPLCD_PowerOffBacklight(GSPLCD_SCREEN_BOTH);
    gspLcdExit();
}

static inline void screenon(void)
{
    gspLcdInit();
    GSPLCD_PowerOnBacklight(GSPLCD_SCREEN_BOTH);
    gspLcdExit();
}


static Handle ptmsysmHandle = 0;

static inline Result ptmsysmInit(void)
{
    return srvGetServiceHandle(&ptmsysmHandle, "ptm:sysm");
}

static inline Result ptmsysmExit(void)
{
    return svcCloseHandle(ptmsysmHandle);
}

typedef struct
{
    u32 ani;
    u8 r[32];
    u8 g[32];
    u8 b[32];
} RGBLedPattern;

static Result ptmsysmSetInfoLedPattern(const RGBLedPattern* pattern)
{
    u32* ipc = getThreadCommandBuffer();
    ipc[0] = 0x8010640;
    memcpy(&ipc[1], pattern, 0x64);
    Result ret = svcSendSyncRequest(ptmsysmHandle);
    if(ret < 0) return ret;
    return ipc[1];
}

static string dialog_text;

/**
 * Make the dialog box appear.
 * @param text Dialog box text.
 */
void DialogBoxAppear(const char *text, int mode) {
	if (showdialogbox)
		return;

	// Save the dialog text so we can make
	// use if it if nullptr is specified.
	if (text) {
		dialog_text = text;
	}

	int movespeed = 22;
	for (int i = 0; i < 240; i += movespeed) {
		if (movespeed <= 1) {
			movespeed = 1;
		} else {
			movespeed -= 0.2;
		}
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		if (screenmode == SCREEN_MODE_SETTINGS) {
			sf2d_draw_texture(settingstex, 0, 0);
		}
		sf2d_draw_texture(dialogboxtex, 0, i-240);
		if (mode == 1) {
			sftd_draw_textf(font, 40, 72+i-240, RGBA8(0, 0, 0, 255), 16, dialog_text.c_str());
		} else
			sftd_draw_textf(font, 12, 16+i-240, RGBA8(0, 0, 0, 255), 12, dialog_text.c_str());
		sf2d_end_frame();
		sf2d_swapbuffers();
	}
	showdialogbox = true;
}

/**
 * Make the dialog box disappear.
 * @param text Dialog box text.
 */
void DialogBoxDisappear(const char *text, int mode) {
	if (!showdialogbox)
		return;

	// Save the dialog text so we can make
	// use if it if nullptr is specified.
	if (text) {
		dialog_text = text;
	}

	int movespeed = 1;
	for (int i = 0; i < 240; i += movespeed) {
		movespeed += 1;
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		if (screenmode == SCREEN_MODE_SETTINGS) {
			sf2d_draw_texture(settingstex, 0, 0);
		}
		sf2d_draw_texture(dialogboxtex, 0, i);
		if (mode == 1) {
			sftd_draw_textf(font, 40, 72+i, RGBA8(0, 0, 0, 255), 16, dialog_text.c_str());
		} else
			sftd_draw_textf(font, 12, 16+i, RGBA8(0, 0, 0, 255), 12, dialog_text.c_str());
		sf2d_end_frame();
		sf2d_swapbuffers();
	}
	showdialogbox = false;
}

/**
 * Create a save file.
 * @param filename Filename.
 * @return 0 on success; non-zero on error.
 */
static int CreateGameSave(const char *filename) {
	char nds_path[256];
	snprintf(nds_path, sizeof(nds_path), "sdmc:/%s/%s", settings.ui.romfolder.c_str() , rom);
	FILE *f_nds_file = fopen(nds_path, "rb");
	if (!f_nds_file) {
		DialogBoxAppear("fopen(nds_path) failed, continuing anyway.", 0);
		DialogBoxDisappear("fopen(nds_path) failed, continuing anyway.", 0);
		return -1;
	}

	char game_TID[5];
	grabTID(f_nds_file, game_TID);
	game_TID[4] = 0;
	fclose(f_nds_file);
	
	if (strcmp(game_TID, "####") != 0) {	// Create save if game isn't homebrew
		DialogBoxAppear("Creating save file...", 1);
		static const int BUFFER_SIZE = 4096;
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, sizeof(buffer));

		int savesize = 524288;	// 512KB (default size for most games)
		
		// Set save size to 1MB for the following games
		if (strcmp(game_TID, "AZLJ") == 0 ||	// Wagamama Fashion: Girls Mode
			strcmp(game_TID, "AZLE") == 0 ||	// Style Savvy
			strcmp(game_TID, "AZLP") == 0 ||	// Nintendo presents: Style Boutique
			strcmp(game_TID, "AZLK") == 0 )	// Namanui Collection: Girls Style
		{
			savesize = 1048576;
		}

		// Set save size to 32MB for the following games
		if (strcmp(game_TID, "UORE") == 0 ||	// WarioWare - D.I.Y.
			strcmp(game_TID, "UORP") == 0 )	// WarioWare - Do It Yourself
		{
			savesize = 1048576*32;
		}

		FILE *pFile = fopen(filename, "wb");
		if (pFile) {
			for (int i = savesize; i > 0; i -= BUFFER_SIZE) {
				fwrite(buffer, 1, sizeof(buffer), pFile);
			}
			fclose(pFile);
		}

		DialogBoxDisappear("Done!", 1);
	}
	return 0;
}

/**
 * Set MPU settings for a specific game.
 */
void SetMPUSettings() {
	const u32 hHeld = hidKeysHeld();

	char nds_path[256];
	snprintf(nds_path, sizeof(nds_path), "sdmc:/%s/%s", settings.ui.romfolder.c_str() , rom);
	FILE *f_nds_file = fopen(nds_path, "rb");

	char game_TID[5];
	grabTID(f_nds_file, game_TID);
	game_TID[4] = 0;
	game_TID[3] = 0;
	fclose(f_nds_file);
	
	if(hHeld & KEY_B){
		settings.twl.mpuregion = 1;
	} else if(hHeld & KEY_X){
		settings.twl.mpuregion = 2;
	} else if(hHeld & KEY_Y){
		settings.twl.mpuregion = 3;
	} else {
		settings.twl.mpuregion = 0;
	}
	if(hHeld & KEY_RIGHT){
		settings.twl.mpusize = 3145728;
	} else if(hHeld & KEY_LEFT){
		settings.twl.mpusize = 1;
	} else {
		settings.twl.mpusize = 0;
	}

	// Check for games that need an MPU size of 3 MB.
	static const char mpu_3MB_list[][4] = {
		"A7A",	// DS Download Station - Vol 1
		"A7B",	// DS Download Station - Vol 2
		"A7C",	// DS Download Station - Vol 3
		"A7D",	// DS Download Station - Vol 4
		"A7E",	// DS Download Station - Vol 5
		"A7F",	// DS Download Station - Vol 6 (EUR)
		"A7G",	// DS Download Station - Vol 6 (USA)
		"A7H",	// DS Download Station - Vol 7
		"A7I",	// DS Download Station - Vol 8
		"A7J",	// DS Download Station - Vol 9
		"A7K",	// DS Download Station - Vol 10
		"A7L",	// DS Download Station - Vol 11
		"A7M",	// DS Download Station - Vol 12
		"A7N",	// DS Download Station - Vol 13
		"A7O",	// DS Download Station - Vol 14
		"A7P",	// DS Download Station - Vol 15
		"A7Q",	// DS Download Station - Vol 16
		"A7R",	// DS Download Station - Vol 17
		"A7S",	// DS Download Station - Vol 18
		"A7T",	// DS Download Station - Vol 19
		"ARZ",	// Rockman ZX/MegaMan ZX
		"YZX",	// Rockman ZX Advent/MegaMan ZX Advent
		"B6Z",	// Rockman Zero Collection/MegaMan Zero Collection
	};

	// TODO: If the list gets large enough, switch to bsearch().
	for (unsigned int i = 0; i < sizeof(mpu_3MB_list)/sizeof(mpu_3MB_list[0]); i++) {
		if (!memcmp(game_TID, mpu_3MB_list[i], 3)) {
			// Found a match.
			settings.twl.mpuregion = 1;
			settings.twl.mpusize = 3145728;
			break;
		}
	}
}

/**
 * Set a rainbow cycle pattern on the notification LED.
 * @return 0 on success; non-zero on error.
 */
static int RainbowLED(void) {
	static const RGBLedPattern pat = {
		32,	// Number of valid entries.

		//marcus@Werkstaetiun:/media/marcus/WESTERNDIGI/dev_threedee/MCU_examples/RGB_rave$ lua graphics/colorgen.lua

		// Red
		{128, 103,  79,  57,  38,  22,  11,   3,   1,   3,  11,  22,  38,  57,  79, 103,
		 128, 153, 177, 199, 218, 234, 245, 253, 255, 253, 245, 234, 218, 199, 177, 153},

		// Green
		{238, 248, 254, 255, 251, 242, 229, 212, 192, 169, 145, 120,  95,  72,  51,  33,
		  18,   8,   2,   1,   5,  14,  27,  44,  65,  87, 111, 136, 161, 184, 205, 223},

		// Blue
		{ 18,  33,  51,  72,  95, 120, 145, 169, 192, 212, 229, 242, 251, 255, 254, 248,
		 238, 223, 205, 184, 161, 136, 111,  87,  64,  44,  27,  14,   5,   1,   2,   8},
	};

	if (ptmsysmInit() < 0)
		return -1;
	ptmsysmSetInfoLedPattern(&pat);
	ptmsysmExit();
	if (logEnabled)	LogFM("Main.RainbowLED", "Rainbow LED is on");
	return 0;
}

/**
 * Set a user led color for notification LED
 * @return 0 on success; non-zero on error.
 */
static int PergameLed(void) {
	RGBLedPattern pattern;
	pattern.ani = 32;	// Need to be 32 in order to be it constant

	// Set the color values to a single RGB value.
	memset(&pattern.r, (u8)RGB[0], sizeof(pattern.r));
	memset(&pattern.g, (u8)RGB[1], sizeof(pattern.g));
	memset(&pattern.b, (u8)RGB[2], sizeof(pattern.b));

	if (ptmsysmInit() < 0)
		return -1;
	ptmsysmSetInfoLedPattern(&pattern);
	ptmsysmExit();
	return 0;
}

static void ChangeBNRIconNo(void) {
	// Get the bnriconnum relative to the current page.
	const int idx = bnriconnum - (pagenum * 20);
	if (idx >= 0 && idx < 20) {
		// Selected banner icon is on the current page.
		bnricontexnum = bnricontex[idx];
	}
}

static void ChangeBoxArtNo(void) {
	// Get the boxartnum relative to the current page.
	const int idx = boxartnum - (pagenum * 20);
	if (idx >= 0 && idx < 20) {
		// Selected boxart is on the current page.
		// NOTE: Only 6 slots for boxart.
		boxarttexnum = boxarttex[idx % 6];
	}
}

/**
 * Store a boxart path.
 * @param path Boxart path. (will be strdup()'d)
 */
static void StoreBoxArtPath(const char *path) {
	// Get the boxartnum relative to the current page.
	const int idx = boxartnum - (pagenum * 20);
	if (idx >= 0 && idx < 20) {
		// Selected boxart is on the current page.
		free(boxartpath[idx]);
		boxartpath[idx] = strdup(path);
	} else {
		free(boxartpath[20]);
		boxartpath[20] = strdup("romfs:/graphics/blank_128x115.png");
	}
}

/**
 * Load a banner icon at the current bnriconnum.
 * @param filename Banner filename, or NULL for notextbanner.
 */
static void LoadBNRIcon(const char *filename) {
	// Get the bnriconnum relative to the current page.
	const int idx = bnriconnum - (pagenum * 20);
	if (idx >= 0 && idx < 20) {
		// Selected bnriconnum is on the current page.
		sf2d_free_texture(bnricontex[idx]);
		bnricontex[idx] = NULL;

		if (!filename) {
			filename = "romfs:/notextbanner";
		}
		FILE *f_bnr = fopen(filename, "rb");
		if (!f_bnr) {
			filename = "romfs:/notextbanner";
			f_bnr = fopen(filename, "rb");
		}

		bnricontex[idx] = grabIcon(f_bnr);
		fclose(f_bnr);
	}
}

/**
 * Load a banner icon at the current cursorPosition.
 * @param filename Banner filename, or NULL for notextbanner.
 */
static void LoadBNRIcon_R4Theme(const char *filename) {
	sf2d_free_texture(bnricontex[0]);
	bnricontex[0] = NULL;

	if (!filename) {
		filename = "romfs:/notextbanner";
	}
	FILE *f_bnr = fopen(filename, "rb");
	if (!f_bnr) {
		filename = "romfs:/notextbanner";
		f_bnr = fopen(filename, "rb");
	}

	bnricontex[0] = grabIcon(f_bnr);
	fclose(f_bnr);
}

static void LoadBoxArt(void) {
	// Get the boxartnum relative to the current page.
	const int idx = boxartnum - (pagenum * 20);
	if (idx >= 0 && idx < 21) {
		// Selected boxart is on the current page.
		// NOTE: Only 6 slots for boxart.
		sf2d_free_texture(boxarttex[idx % 6]);
		const char *path = (boxartpath[idx] ? boxartpath[idx] : "romfs:/graphics/blank_128x115.png");
		boxarttex[idx % 6] = sfil_load_PNG_file(path, SF2D_PLACE_RAM); // Box art
	}
}

static void LoadBoxArt_WoodTheme(const int idx) {
	// Selected boxart is on the current page.
	sf2d_free_texture(boxarttex[0]);
	const char *path = (boxartpath[idx] ? boxartpath[idx] : "romfs:/graphics/blank_128x115.png");
	boxarttex[0] = sfil_load_PNG_file(path, SF2D_PLACE_RAM); // Box art
}

/**
 * Load nds-bootstrap's configuration.
 */
static void LoadBootstrapConfig(void)
{
	switch (bootstrapini.GetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, -1)) {
		case 1:
			settings.twl.console = 2;
			break;
		case 0:
		default:
			settings.twl.console = 1;
			break;
		case -1:
			settings.twl.console = 0;
			break;
	}
	if (logEnabled)	LogFM("Main.LoadBootstrapConfig", "Bootstrap configuration loaded successfully");
}

/**
 * Update nds-bootstrap's configuration.
 */
static void SaveBootstrapConfig(void)
{
	if (applaunchon) {
		// Set ROM path if ROM is selected
		if (!settings.twl.launchslot1) {
			SetMPUSettings();
			bootstrapini.SetString(bootstrapini_ndsbootstrap, bootstrapini_ndspath, fat+settings.ui.romfolder+slashchar+rom);
			bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_mpuregion, settings.twl.mpuregion);
			bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_mpusize, settings.twl.mpusize);
			bootstrapini.SetString(bootstrapini_ndsbootstrap, bootstrapini_bootstrappath, bootstrapPath);
			if (gbarunnervalue == 0) {
				bootstrapini.SetString(bootstrapini_ndsbootstrap, bootstrapini_savpath, fat+settings.ui.romfolder+slashchar+sav);
				char path[256];
				snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.romfolder.c_str(), sav.c_str());
				if (access(path, F_OK) == -1) {
					// Create a save file if it doesn't exist
					CreateGameSave(path);
				}
			}
		}
	}
	bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_boostcpu, settings.twl.cpuspeed);

	// TODO: Change the default to 0?
	switch (settings.twl.console) {
		case 0:
			bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, -1);
			break;
		case 1:
		default:
			bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, 0);
			break;
		case 2:
			bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, 1);
			break;
	}

	bootstrapini.SaveIniFile("sdmc:/_nds/nds-bootstrap.ini");
}

/**
 * Load per-game settings.
 */
static void LoadPerGameSettings(void)
{
	std::string inifilename;
	if (!settings.twl.forwarder)
		inifilename = ReplaceAll(rom, ".nds", ".ini");
	else {
		char path[256];
		snprintf(path, sizeof(path), "%s/%s", "flashcard", rom);
		inifilename = path;
	}
	char path[256];
	snprintf(path, sizeof(path), "sdmc:/_nds/twloader/gamesettings/%s", inifilename.c_str());
	CIniFile gamesettingsini(path);
	settings.pergame.cpuspeed = gamesettingsini.GetInt("GAME-SETTINGS", "TWL_CLOCK", -1);
	settings.pergame.extvram = gamesettingsini.GetInt("GAME-SETTINGS", "TWL_VRAM", -1);
	settings.pergame.lockarm9scfgext = gamesettingsini.GetInt("GAME-SETTINGS", bootstrapini_lockarm9scfgext, -1);
	settings.pergame.red = gamesettingsini.GetInt("GAME-SETTINGS", "LED_RED", -1);
	settings.pergame.green = gamesettingsini.GetInt("GAME-SETTINGS", "LED_GREEN", -1);
	settings.pergame.blue = gamesettingsini.GetInt("GAME-SETTINGS", "LED_BLUE", -1);

	RGB[0] = settings.pergame.red;
	RGB[1] = settings.pergame.green;
	RGB[2] = settings.pergame.blue;

	// In case if the .ini was manually edited
	if(RGB[0] > 255) RGB[0] = 255;
	if(RGB[1] > 255) RGB[1] = 255;
	if(RGB[2] > 255) RGB[2] = 255;
	
	if (logEnabled)	LogFM("Main.LoadPerGameSettings", "Per-game settings loaded successfully");
}

/**
 * Update per-game settings.
 */
static void SavePerGameSettings(void)
{
	std::string inifilename;
	if (!settings.twl.forwarder)
		inifilename = ReplaceAll(rom, ".nds", ".ini");
	else {
		char path[256];
		snprintf(path, sizeof(path), "%s/%s", "flashcard", rom);
		inifilename = path;
	}
	char path[256];
	snprintf(path, sizeof(path), "sdmc:/_nds/twloader/gamesettings/%s", inifilename.c_str());
	CIniFile gamesettingsini(path);
	gamesettingsini.SetInt("GAME-SETTINGS", "TWL_CLOCK", settings.pergame.cpuspeed);
	gamesettingsini.SetInt("GAME-SETTINGS", "TWL_VRAM", settings.pergame.extvram);
	gamesettingsini.SetInt("GAME-SETTINGS", bootstrapini_lockarm9scfgext, settings.pergame.lockarm9scfgext);
	gamesettingsini.SetInt("GAME-SETTINGS", "LED_RED", settings.pergame.red);
	gamesettingsini.SetInt("GAME-SETTINGS", "LED_GREEN", settings.pergame.green);
	gamesettingsini.SetInt("GAME-SETTINGS", "LED_BLUE", settings.pergame.blue);
	gamesettingsini.SaveIniFile(path);
	if (logEnabled)	LogFM("Main.SavePerGameSettings", "Per-game settings saved successfully");
}

/**
 * Set per-game settings file.
 */
static void SetPerGameSettings(void)
{
	std::string inifilename = "sd:/_nds/twloader/gamesettings/null";
	if (!settings.twl.launchslot1) {
		inifilename = ReplaceAll(rom, ".nds", ".ini");
		char path[256];
		snprintf(path, sizeof(path), "%s/%s", "sd:/_nds/twloader/gamesettings", inifilename.c_str());
		inifilename = path;
	} else {
		if (settings.twl.forwarder) {
			char path[256];
			snprintf(path, sizeof(path), "%s/%s", "sd:/_nds/twloader/gamesettings/flashcard", rom);
			inifilename = path;
		}
	}
	CIniFile settingsini("sdmc:/_nds/twloader/settings.ini");
	settingsini.SetString("TWL-MODE", "GAMESETTINGS_PATH", inifilename);
	settingsini.SaveIniFile("sdmc:/_nds/twloader/settings.ini");
}
 
bool dspfirmfound = false;
static sf2d_texture *voltex[6] = { };

/**
 * Draw the volume slider.
 * @param texarray Texture array to use, (voltex or setvoltex)
 * The Dsi has 8 positions for volume and the 3ds has 64
 * Remap volume to simulate the 8 positions
 */
void draw_volume_slider(sf2d_texture *texarray[])
{
	u8 volumeLevel = 0;
	if (!dspfirmfound) {
		// No DSP Firm.
		sf2d_draw_texture(texarray[5], 5, 2);
	} else if (R_SUCCEEDED(HIDUSER_GetSoundVolume(&volumeLevel))) {
		u8 voltex_id = 0;
		if (volumeLevel == 0) {
			voltex_id = 0;	// 3ds 0, dsi 0 = volume0 texture
		} else if (volumeLevel <= 21) {
			voltex_id = 1;	// 3ds  1-21, dsi 1,2 = volume1 texture
		} else if (volumeLevel <= 42) {
			voltex_id = 2;	// 3ds 22-42, dsi 3,4 = volume2 texture
		} else if (volumeLevel <= 62) {
			voltex_id = 3;	// 3ds 43-62, dsi 5,6 = volume3 texture
		} else if (volumeLevel == 63) {
			voltex_id = 4;	// 3ds 63, dsi 8  = volume4 texture
		}
		sf2d_draw_texture(texarray[voltex_id], 5, 2);
	}
}

sf2d_texture *batteryIcon = NULL;		// Current battery level icon.
static sf2d_texture *batterychrgtex = NULL;	// Fully charged.
static sf2d_texture *batterytex[6] = { };	// Battery levels.

/**
 * Update the battery level icon.
 * @param texchrg Texture for "battery is charging". (batterychrgtex or setbatterychrgtex)
 * @param texarray Texture array for other levels. (batterytex or setbatterytex)
 * The global variable batteryIcon will be updated.
 */
void update_battery_level(sf2d_texture *texchrg, sf2d_texture *texarray[])
{
	u8 batteryChargeState = 0;
	u8 batteryLevel = 0;
	if (R_SUCCEEDED(PTMU_GetBatteryChargeState(&batteryChargeState)) && batteryChargeState) {
		batteryIcon = texchrg;
	} else if (R_SUCCEEDED(PTMU_GetBatteryLevel(&batteryLevel))) {
		switch (batteryLevel) {
			case 5: {
				// NOTE: PTMUX_GetAdapterState should be moved into
				// ctrulib without the 'X' prefix.
				u8 acAdapter = 0;
				if (R_SUCCEEDED(PTMUX_GetAdapterState(&acAdapter)) && acAdapter) {
					batteryIcon = texarray[5];
				} else {
					batteryIcon = texarray[4];
				}
				break;
			}
			case 4:
				batteryIcon = texarray[4];
				break;
			case 3:
				batteryIcon = texarray[3];
				break;
			case 2:
				batteryIcon = texarray[2];
				break;
			case 1:
			default:
				batteryIcon = texarray[1];
				break;
		}
	}

	if (!batteryIcon) {
		// No battery icon...
		batteryIcon = texarray[0];
	}
}

/**
 * Scan a directory for matching files.
 * @param path Directory path.
 * @param ext File extension, case-insensitive. (If nullptr, matches all files.)
 * @param files Vector to append files to.
 * @return Number of files matched. (-1 if the directory could not be opened.)
 */
static int scan_dir_for_files(const char *path, const char *ext, std::vector<std::string>& files)
{
	files.clear();

	DIR *dir = opendir(path);
	if (!dir) {
		// Unable to open the directory.
		return -1;
	}

	struct dirent *ent;
	const int extlen = (ext ? strlen(ext) : 0);
	while ((ent = readdir(dir)) != NULL) {
		std::string fname = (ent->d_name);
		if (extlen > 0) {
			// Check the file extension. (TODO needs verification)
			size_t lastdotpos = fname.find_last_of('.');
			if (lastdotpos == string::npos || lastdotpos + extlen > fname.size()) {
				// Invalid file extension.
				continue;
			}
			if (strcasecmp(&ent->d_name[lastdotpos], ext) != 0) {
				// Incorrect file extension.
				continue;
			}
		}

		// Append the file.
		files.push_back(fname);
	}
	closedir(dir);

	// Sort the vector and we're done.
	std::sort(files.begin(), files.end());
	return (int)files.size();
}

// Files
vector<string> files;
vector<string> fcfiles;

// Vector with found roms
vector<string> matching_files;

// APT hook for "returned from HOME menu".
static aptHookCookie rfhm_cookie;
static void rfhm_callback(APT_HookType hook, void *param)
{
	if (hook == APTHOOK_ONRESTORE) {
		// param == pointer to bannertextloaded
		// TODO: Only if cursorPosition == -1.
		*((bool*)param) = false;
		gamecardPoll(true);
	}
}

// Cartridge textures.
static sf2d_texture *cartnulltex = NULL;
static sf2d_texture *cartntrtex = NULL;
static sf2d_texture *carttwltex = NULL;
static sf2d_texture *cartctrtex = NULL;

/**
 * Determine the 3DS cartridge texture to use for Slot-1.
 * @return Cartridge texture.
 */
static inline sf2d_texture *carttex(void)
{
	// TODO: 3DS cartridges.
	switch (gamecardGetType()) {
		case CARD_TYPE_UNKNOWN:
		default:
			return cartnulltex;

		case CARD_TYPE_NTR:
		case CARD_TYPE_TWL_ENH:
			return cartntrtex;

		case CARD_TYPE_TWL_ONLY:
			return carttwltex;
			break;
			
		case CARD_TYPE_CTR:
			return cartctrtex;
			break;
	}
}

/**
 * Load the boxart for the Slot-1 cartridge.
 */
static void loadSlot1BoxArt(void)
{
	// Previously loaded boxart game ID.
	static u32 prev_gameID_u32 = 0;

	// Get the current game ID.
	u32 gameID_u32 = gamecardGetGameID_u32();
	if (gameID_u32 == prev_gameID_u32) {
		// No change in the game ID.
		if (!slot1boxarttex) {
			// No boxart loaded yet.
			// Load boxart_null.png.
			slot1boxarttex = sfil_load_PNG_file("romfs:/graphics/boxart_null.png", SF2D_PLACE_RAM);
		}
		return;
	}
	prev_gameID_u32 = gameID_u32;

	sf2d_texture *new_tex;
	const char *gameID = gamecardGetGameID();
	if (gameID) {
		if (checkWifiStatus()) {
			downloadSlot1BoxArt(gameID);
		}
		char path[256];
		// example: ASME.png
		if (logEnabled)	LogFMA("Main", "Loading Slot-1 box art", gameID);
		snprintf(path, sizeof(path), "%s/%.4s.png", boxartfolder, gameID);
		if (access(path, F_OK) != -1) {
			new_tex = sfil_load_PNG_file(path, SF2D_PLACE_RAM);
		} else {
			new_tex = sfil_load_PNG_file("romfs:/graphics/boxart_unknown.png", SF2D_PLACE_RAM);
		}
		if (logEnabled)	LogFMA("Main", "Done loading Slot-1 box art", gameID);
	} else {
		// No cartridge, or unrecognized cartridge.
		new_tex = sfil_load_PNG_file("romfs:/graphics/boxart_null.png", SF2D_PLACE_RAM);
	}

	// Replace slot1boxarttex with the new boxart.
	sf2d_texture *old_tex = slot1boxarttex;
	slot1boxarttex = new_tex;
	sf2d_free_texture(old_tex);
}

/**
 * Scan the ROM directories.
 */
static void scanRomDirectories(void)
{
	char path[256];

	// Use default directory if none is specified
	if (settings.ui.romfolder.empty()) {
		settings.ui.romfolder = "roms/nds";
	}
	snprintf(path, sizeof(path), "sdmc:/%s", settings.ui.romfolder.c_str());
	// Make sure the directory exists.
	rmkdir(path, 0777);

	// Scan the ROMs directory for ".nds" files.
	scan_dir_for_files(path, ".nds", files);
	
	// Use default directory if none is specified
	if (settings.ui.fcromfolder.empty()) {
		settings.ui.fcromfolder = "roms/flashcard/nds";
	}
	snprintf(path, sizeof(path), "sdmc:/%s", settings.ui.fcromfolder.c_str());
	// Make sure the directory exists.
	rmkdir(path, 0777);

	// Scan the flashcard directory for configuration files.
	scan_dir_for_files(path, ".ini", fcfiles);
}

// Cursor position.
static int cursorPosition = 0;
static int storedcursorPosition = 0;
static int r4menu_cursorPosition = 0;
static int woodmenu_cursorPosition = 0;
static int startmenu_cursorPosition = 0;
static int gamesettings_cursorPosition = 0;

/**
 * Draw the menu dialog box.
 */
static void drawMenuDialogBox(void)
{
	sf2d_draw_rectangle(0, 0, 320, 240, RGBA8(0, 0, 0, menudbox_bgalpha)); // Fade in/out effect
	sf2d_draw_texture(dialogboxtex, 0, menudbox_Ypos);
	sf2d_draw_texture(dboxtex_buttonback, 233, menudbox_Ypos+193);
	sftd_draw_wtext(font, 243, menudbox_Ypos+199, RGBA8(0, 0, 0, 255), 12, TR(STR_BACK));
	if (menudboxmode == DBOX_MODE_SETTINGS) {
		bnriconnum = cursorPosition;
		ChangeBNRIconNo();
		sf2d_draw_texture(dboxtex_iconbox, 23, menudbox_Ypos+23);
		sf2d_draw_texture_part(bnricontexnum, 28, menudbox_Ypos+28, bnriconframenum*32, 0, 32, 32);
		
		if (cursorPosition >= 0) {
			int y = 16, dy = 19;
			// Print the banner text, center-aligned.
			const size_t banner_lines = std::min(3U, romsel_gameline.size());
			for (size_t i = 0; i < banner_lines; i++, y += dy) {
				const int text_width = sftd_get_wtext_width(font_b, 16, romsel_gameline[i].c_str());
				sftd_draw_wtext(font_b, 48+(264-text_width)/2, y+menudbox_Ypos, RGBA8(0, 0, 0, 255), 16, romsel_gameline[i].c_str());
			}
			sftd_draw_wtext(font, 16, 72+menudbox_Ypos, RGBA8(127, 127, 127, 255), 12, romsel_filename_w.c_str());
		}
		
		const size_t file_count = (settings.twl.forwarder ? fcfiles.size() : files.size());

		char romsel_counter1[16];
		char romsel_counter2[16];
		snprintf(romsel_counter1, sizeof(romsel_counter1), "%d", storedcursorPosition+1);		
		if(matching_files.size() == 0){
			snprintf(romsel_counter2, sizeof(romsel_counter2), "%zu", file_count);
		}else{
			snprintf(romsel_counter2, sizeof(romsel_counter2), "%zu", matching_files.size());
		}
		
		if (file_count < 100) {
			sftd_draw_text(font, 16, 204+menudbox_Ypos, RGBA8(0, 0, 0, 255), 12, romsel_counter1);
			sftd_draw_text(font, 35, 204+menudbox_Ypos, RGBA8(0, 0, 0, 255), 12, "/");
			sftd_draw_text(font, 40, 204+menudbox_Ypos, RGBA8(0, 0, 0, 255), 12, romsel_counter2);
		} else {
			sftd_draw_text(font, 16, 204+menudbox_Ypos, RGBA8(0, 0, 0, 255), 12, romsel_counter1);
			sftd_draw_text(font, 43, 204+menudbox_Ypos, RGBA8(0, 0, 0, 255), 12, "/");
			sftd_draw_text(font, 48, 204+menudbox_Ypos, RGBA8(0, 0, 0, 255), 12, romsel_counter2);
		}
		
		const struct {
			int x;
			int y;
			const s8 *value;
			const wchar_t *title;
			const wchar_t *value_desc[2];	// 0 == off, 1 == on
		} buttons[] = {
			{ 23,  89, &settings.pergame.cpuspeed, TR(STR_START_ARM9_CPU_SPEED), {L"67 MHz (NTR)", L"133 MHz (TWL)"}},
			{161,  89, &settings.pergame.extvram, TR(STR_START_VRAM_BOOST), {L"Off", L"On"}},
			{ 23, 129, &settings.pergame.lockarm9scfgext, TR(STR_START_LOCK_ARM9_SCFG_EXT), {L"Off", L"On"}},
			{161, 129, &settings.pergame.donor, TR(STR_START_SET_DONOR), {NULL, NULL}},
			{ 23, 169, NULL, TR(STR_START_SET_LED), {NULL, NULL}},
		};
		
		for (int i = (int)(sizeof(buttons)/sizeof(buttons[0]))-1; i >= 0; i--) {
			if (gamesettings_cursorPosition == i) {
				// Button is highlighted.
				sf2d_draw_texture(dboxtex_button, buttons[i].x, menudbox_Ypos+buttons[i].y);
			} else {
				// Button is not highlighted. Darken the texture.
				sf2d_draw_texture_blend(dboxtex_button, buttons[i].x, menudbox_Ypos+buttons[i].y, RGBA8(127, 127, 127, 255));
			}

			const wchar_t *title = buttons[i].title;
			const wchar_t *value_desc = TR(STR_START_DEFAULT);
			if (i < 3) {
				switch (*(buttons[i].value)) {
					case -1:
					default:
						value_desc = TR(STR_START_DEFAULT);
						break;
					case 0:
						value_desc = buttons[i].value_desc[0];
						break;
					case 1:
						value_desc = buttons[i].value_desc[1];
						break;
				}
			}

			// Determine the text height.
			// NOTE: Button texture size is 132x34.
			const int h = 32;

			// Draw the title.
			int y = menudbox_Ypos + buttons[i].y + ((34 - h) / 2);
			int w = sftd_get_wtext_width(font, 12, title);
			int x = ((132 - w) / 2) + buttons[i].x;
			sftd_draw_wtext(font, x, y, RGBA8(0, 0, 0, 255), 12, title);
			y += 16;

			// Draw the value.
			if (i < 3) {
				w = sftd_get_wtext_width(font, 12, value_desc);
				x = ((132 - w) / 2) + buttons[i].x;
				sftd_draw_wtext(font, x, y, RGBA8(0, 0, 0, 255), 12, value_desc);
			} else if (i == 4) {
				// Show the RGB value.
				char rgb_str[32];
				snprintf(rgb_str, sizeof(rgb_str), "%d, %d, %d",
					settings.pergame.red,
					settings.pergame.green,
					settings.pergame.blue);
				w = sftd_get_text_width(font, 12, rgb_str);
				x = ((132 - w) / 2) + buttons[i].x;

				// Print the RGB value using its color.
				const u32 color = RGBA8(settings.pergame.red,
					settings.pergame.green,
					settings.pergame.blue, 255);
				sftd_draw_text(font, x, y, color, 12, rgb_str);
			}
		}
	} else {
		// UI options.
		const struct {
			int x;
			int y;
			const bool *value;
			const wchar_t *title;
			const wchar_t *value_desc[2];	// 0 == off, 1 == on
		} buttons[] = {
			{ 23,  31, &settings.twl.forwarder, TR(STR_START_GAMELOCATION), {TR(STR_START_SD_CARD), TR(STR_START_FLASHCARD)}},
			{161,  31, &settings.romselect.toplayout, NULL, {TR(STR_START_BOXART_ON), TR(STR_START_BOXART_OFF)}},
			{ 23,  71, &is3DSX, TR(STR_START_START_GBARUNNER2), {NULL, NULL}},
			{161,  71, &settings.ui.topborder, NULL, {TR(STR_START_TOP_BORDER_OFF), TR(STR_START_TOP_BORDER_ON)}},
			{ 23, 111, NULL, TR(STR_START_UNSET_DONOR), {NULL, NULL}},
			{161, 111, NULL, TR(STR_START_SEARCH), {NULL, NULL}},
		};

		for (int i = (int)(sizeof(buttons)/sizeof(buttons[0])) - 1; i >= 0; i--) {
			if (startmenu_cursorPosition == i) {
				// Button is highlighted.
				sf2d_draw_texture(dboxtex_button, buttons[i].x, menudbox_Ypos+buttons[i].y);
			} else {
				// Button is not highlighted. Darken the texture.
				sf2d_draw_texture_blend(dboxtex_button, buttons[i].x, menudbox_Ypos+buttons[i].y, RGBA8(127, 127, 127, 255));
			}

			const wchar_t *title = buttons[i].title;
			const wchar_t *value_desc = (buttons[i].value ? buttons[i].value_desc[!!*(buttons[i].value)] : NULL);

			// Determine width and height.
			const int h = (title && value_desc ? 32 : 16);

			// Draw the text.
			// NOTE: Button texture size is 132x34.
			int y = menudbox_Ypos + buttons[i].y + ((34 - h) / 2);
			if (title) {
				const int w = sftd_get_wtext_width(font, 12, title);
				const int x = ((132 - w) / 2) + buttons[i].x;
				sftd_draw_wtext(font, x, y, RGBA8(0, 0, 0, 255), 12, title);
				y += 16;
			}
			if (value_desc) {
				const int w = sftd_get_wtext_width(font, 12, value_desc);
				const int x = ((132 - w) / 2) + buttons[i].x;
				sftd_draw_wtext(font, x, y, RGBA8(0, 0, 0, 255), 12, value_desc);
			}
		}
	}
}

// TWLNAND side Title ID.
extern const u64 TWLNAND_TID;
const u64 TWLNAND_TID = 0x0004800554574C44ULL;

/**
* Check if the TWLNAND-side title is installed or not
* Title ID: 0x0004800554574C44ULL
* MediaType: MEDIATYPE_NAND
* @return: true if installed, false if not
*/
bool checkTWLNANDSide(void) {
	u64 tid = TWLNAND_TID;
	AM_TitleEntry entry;
	return R_SUCCEEDED(AM_GetTitleInfo(MEDIATYPE_NAND, 1, &tid, &entry));
}

int main()
{
	sf2d_init();
	sf2d_set_clear_color(RGBA8(0x00, 0x00, 0x00, 0x00));
	sf2d_set_3D(0);

	if(is3DSX)
		settingslogotex = sfil_load_PNG_buffer(logo_demo_png, SF2D_PLACE_RAM); // TWLoader (3DSX demo version) logo on top screen
	else
		settingslogotex = sfil_load_PNG_buffer(logo_png, SF2D_PLACE_RAM); // TWLoader logo on top screen

	sf2d_start_frame(GFX_TOP, GFX_LEFT);
	sf2d_draw_texture(settingslogotex, 400/2 - settingslogotex->width/2, 240/2 - settingslogotex->height/2);
	sf2d_end_frame();
	sf2d_start_frame(GFX_TOP, GFX_RIGHT);
	sf2d_draw_texture(settingslogotex, 400/2 - settingslogotex->width/2, 240/2 - settingslogotex->height/2);
	sf2d_end_frame();
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	sf2d_end_frame();
	sf2d_swapbuffers();


	aptInit();
	cfguInit();
	amInit();
	ptmuInit();	// For battery status
	ptmuxInit();	// For AC adapter status
	sdmcInit();
	romfsInit();
	srvInit();
	hidInit();
	acInit();
		
	bool bannertextloaded = false;	
	// Register a handler for "returned from HOME Menu".
	aptHook(&rfhm_cookie, rfhm_callback, &bannertextloaded);
	
	/* Log file is dissabled by default. If _nds/twloader/log exist, we turn log file on, else, log is dissabled */
	struct stat logBuf;
	logEnabled = stat("sdmc:/_nds/twloader/log", &logBuf) == 0;
	/* Log configuration file end */
	
	if (logEnabled)	createLog();

	// make folders if they don't exist
	mkdir("sdmc:/_nds", 0777);
	mkdir("sdmc:/_nds/twloader", 0777);
	mkdir("sdmc:/_nds/twloader/bnricons", 0777);
	mkdir("sdmc:/_nds/twloader/bnricons/flashcard", 0777);
	mkdir("sdmc:/_nds/twloader/boxart", 0777);
	mkdir("sdmc:/_nds/twloader/gamesettings", 0777);
	mkdir("sdmc:/_nds/twloader/gamesettings/flashcard", 0777);
	mkdir("sdmc:/_nds/twloader/loadflashcard", 0777);
	//mkdir("sdmc:/_nds/twloader/tmp", 0777);
	
	// Font loading
	sftd_init();
	font = sftd_load_font_file("romfs:/fonts/FOT-RodinBokutoh Pro M.otf");
	font_b = sftd_load_font_file("romfs:/fonts/FOT-RodinBokutoh Pro DB.otf");
	sftd_draw_text(font, 0, 0, RGBA8(0, 0, 0, 255), 16, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890&:-.'!?()\"end"); //Hack to avoid blurry text!
	sftd_draw_text(font_b, 0, 0, RGBA8(0, 0, 0, 255), 24, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890&:-.'!?()\"end"); //Hack to avoid blurry text!	
	if (logEnabled)	LogFM("Main.Font loading", "Fonts load correctly");
	
    snprintf(settings_vertext, 14, "Ver. %d.%d.%d   ", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);
	if (logEnabled)	LogFMA("Main.GUI version", "Successful reading version", settings_vertext);

	LoadSettings();	
	bootstrapPath = settings.twl.bootstrapfile ? "fat:/_nds/release-bootstrap.nds" : "fat:/_nds/unofficial-bootstrap.nds";
	if (logEnabled) LogFMA("Main.bootstrapPath", "Using path:", bootstrapPath.c_str());
	LoadBootstrapConfig();

	// Store bootstrap version
	checkBootstrapVersion();
	
	// Initialize translations.
	langInit();	
	
	LoadColor();
	LoadMenuColor();
	LoadBottomImage();

	// Dialog box textures.
	dialogboxtex = sfil_load_PNG_file("romfs:/graphics/dialogbox.png", SF2D_PLACE_RAM); // Dialog box
	dboxtex_iconbox = sfil_load_PNG_file("romfs:/graphics/dbox/iconbox.png", SF2D_PLACE_RAM); // Icon box
	dboxtex_button = sfil_load_PNG_file("romfs:/graphics/dbox/button.png", SF2D_PLACE_RAM); // Icon box
	dboxtex_buttonback = sfil_load_PNG_file("romfs:/graphics/dbox/button_back.png", SF2D_PLACE_RAM); // Icon box

	sf2d_texture *toplogotex = NULL;		// Top of R4 menu
	sf2d_texture *toptex = sfil_load_PNG_file("romfs:/graphics/top.png", SF2D_PLACE_RAM); // Top DSi-Menu border
	sf2d_texture *topbgtex; // Top background, behind the DSi-Menu border

	// Volume slider textures.
	voltex[0] = sfil_load_PNG_file("romfs:/graphics/volume0.png", SF2D_PLACE_RAM); // Show no volume
	voltex[1] = sfil_load_PNG_file("romfs:/graphics/volume1.png", SF2D_PLACE_RAM); // Volume low above 0
	voltex[2] = sfil_load_PNG_file("romfs:/graphics/volume2.png", SF2D_PLACE_RAM); // Volume medium
	voltex[3] = sfil_load_PNG_file("romfs:/graphics/volume3.png", SF2D_PLACE_RAM); // Hight volume
	voltex[4] = sfil_load_PNG_file("romfs:/graphics/volume4.png", SF2D_PLACE_RAM); // 100%
	voltex[5] = sfil_load_PNG_file("romfs:/graphics/volume5.png", SF2D_PLACE_RAM); // No DSP firm found

	shoulderLtex = sfil_load_PNG_file("romfs:/graphics/shoulder_L.png", SF2D_PLACE_RAM); // L shoulder
	shoulderRtex = sfil_load_PNG_file("romfs:/graphics/shoulder_R.png", SF2D_PLACE_RAM); // R shoulder

	// Battery level textures.
	batterychrgtex = sfil_load_PNG_file("romfs:/graphics/battery_charging.png", SF2D_PLACE_RAM);
	batterytex[0] = sfil_load_PNG_file("romfs:/graphics/battery0.png", SF2D_PLACE_RAM);
	batterytex[1] = sfil_load_PNG_file("romfs:/graphics/battery1.png", SF2D_PLACE_RAM);
	batterytex[2] = sfil_load_PNG_file("romfs:/graphics/battery2.png", SF2D_PLACE_RAM);
	batterytex[3] = sfil_load_PNG_file("romfs:/graphics/battery3.png", SF2D_PLACE_RAM);
	batterytex[4] = sfil_load_PNG_file("romfs:/graphics/battery4.png", SF2D_PLACE_RAM);
	batterytex[5] = sfil_load_PNG_file("romfs:/graphics/battery5.png", SF2D_PLACE_RAM);

	sf2d_texture *iconstex = NULL;		// Bottom of menu (3 icons)
	sf2d_texture *bottomtex = NULL;		// Bottom of menu
	sf2d_texture *sdicontex = sfil_load_PNG_file("romfs:/graphics/wood/sd.png", SF2D_PLACE_RAM);
	sf2d_texture *flashcardicontex = sfil_load_PNG_file("romfs:/graphics/wood/flashcard.png", SF2D_PLACE_RAM);
	sf2d_texture *gbaicontex = sfil_load_PNG_file("romfs:/graphics/wood/gba.png", SF2D_PLACE_RAM);
	sf2d_texture *smallsettingsicontex = sfil_load_PNG_file("romfs:/graphics/wood/settings.png", SF2D_PLACE_RAM);
	sf2d_texture *iconnulltex = sfil_load_PNG_file("romfs:/graphics/icon_null.png", SF2D_PLACE_RAM); // Slot-1 cart icon if no cart is present
	sf2d_texture *homeicontex = sfil_load_PNG_file("romfs:/graphics/homeicon.png", SF2D_PLACE_RAM); // HOME icon
	sf2d_texture *bottomlogotex = sfil_load_PNG_file("romfs:/graphics/bottom_logo.png", SF2D_PLACE_RAM); // TWLoader logo on bottom screen
	sf2d_texture *dotcircletex = NULL;	// Dots forming a circle
	sf2d_texture *startbordertex = NULL;	// "START" border
	sf2d_texture *settingsicontex = sfil_load_PNG_file("romfs:/graphics/settingsbox.png", SF2D_PLACE_RAM); // Settings box on bottom screen
	sf2d_texture *getfcgameboxtex = sfil_load_PNG_file("romfs:/graphics/getfcgamebox.png", SF2D_PLACE_RAM);
	cartnulltex = sfil_load_PNG_file("romfs:/graphics/cart_null.png", SF2D_PLACE_RAM); // NTR cartridge
	cartntrtex = sfil_load_PNG_file("romfs:/graphics/cart_ntr.png", SF2D_PLACE_RAM); // NTR cartridge
	carttwltex = sfil_load_PNG_file("romfs:/graphics/cart_twl.png", SF2D_PLACE_RAM); // TWL cartridge
	cartctrtex = sfil_load_PNG_file("romfs:/graphics/cart_ctr.png", SF2D_PLACE_RAM); // CTR cartridge
	sf2d_texture *boxfulltex = sfil_load_PNG_file("romfs:/graphics/box_full.png", SF2D_PLACE_RAM); // (DSiWare) box on bottom screen
	sf2d_texture *bracetex = sfil_load_PNG_file("romfs:/graphics/brace.png", SF2D_PLACE_RAM); // Brace (C-shaped thingy)
	sf2d_texture *bubbletex = sfil_load_PNG_file("romfs:/graphics/bubble.png", SF2D_PLACE_RAM); // Text bubble

	if (logEnabled)	LogFM("Main.sf2d_textures", "Textures load successfully");

	dspfirmfound = false;
 	if( access( "sdmc:/3ds/dspfirm.cdc", F_OK ) != -1 ) {
		ndspInit();
		dspfirmfound = true;
		if (logEnabled)	LogFM("Main.dspfirm", "DSP Firm found!");
	}else{
		if (logEnabled)	LogFM("Main.dspfirm", "DSP Firm not found");
	}

	bool musicbool = false;
	if( access( "sdmc:/_nds/twloader/music.wav", F_OK ) != -1 ) {
		musicpath = "sdmc:/_nds/twloader/music.wav";
		if (logEnabled)	LogFM("Main.music", "Custom music file found!");
	}else {
		if (logEnabled)	LogFM("Main.dspfirm", "No music file found");
	}

	// Load the sound effects if DSP is available.
	if (dspfirmfound) {
		bgm_menu = new sound(musicpath);
		//bgm_settings = new sound("sdmc:/_nds/twloader/music/settings.wav");
		sfx_launch = new sound("romfs:/sounds/launch.wav", 2, false);
		sfx_select = new sound("romfs:/sounds/select.wav", 2, false);
		sfx_stop = new sound("romfs:/sounds/stop.wav", 2, false);
		sfx_switch = new sound("romfs:/sounds/switch.wav", 2, false);
		sfx_wrong = new sound("romfs:/sounds/wrong.wav", 2, false);
		sfx_back = new sound("romfs:/sounds/back.wav", 2, false);
	}

	// Scan the ROM directories.
	scanRomDirectories();

	char romsel_counter2sd[16];	// Number of ROMs on the SD card.
	snprintf(romsel_counter2sd, sizeof(romsel_counter2sd), "%zu", files.size());
	if (logEnabled)	LogFMA("Main. ROM scanning", "Number of ROMs on the SD card detected", romsel_counter2sd);
	
	char romsel_counter2fc[16];	// Number of ROMs on the flash card.
	snprintf(romsel_counter2fc, sizeof(romsel_counter2fc), "%zu", fcfiles.size());
	if (logEnabled)	LogFMA("Main. ROM scanning", "Number of ROMs on the flashcard detected", romsel_counter2fc);
	
	// Download box art
	if (checkWifiStatus()) {
		downloadBoxArt();
	}

	// Cache banner data for ROMs on the SD card.
	// TODO: Re-cache if it's 0 bytes?
	if (logEnabled)	Log("********************************************************\n");
	for (bnriconnum = 0; bnriconnum < (int)files.size(); bnriconnum++) {
		static const char title[] = "Now checking banner data (SD Card)...";
		char romsel_counter1[16];
		snprintf(romsel_counter1, sizeof(romsel_counter1), "%d", bnriconnum+1);
		const char *tempfile = files.at(bnriconnum).c_str();

		wstring tempfile_w = utf8_to_wstring(tempfile);
		sftd_draw_wtext(font, 12, 64, RGBA8(0, 0, 0, 255), 12, tempfile_w.c_str());

		char nds_path[256];
		snprintf(nds_path, sizeof(nds_path), "sdmc:/%s/%s", settings.ui.romfolder.c_str(), tempfile);
		FILE *f_nds_file = fopen(nds_path, "rb");
		if (!f_nds_file)
			continue;
		if (logEnabled)	LogFMA("Main. Banner scanning", "Trying to read banner from file", nds_path);
		
		if(cacheBanner(f_nds_file, tempfile, font, dialogboxtex, title, romsel_counter1, romsel_counter2sd) == 0) {
			if (logEnabled)	LogFM("Main. Banner scanning", "Done!");
		}else {
			if (logEnabled)	LogFM("Main. Banner scanning", "Error!");
		}
		
		fclose(f_nds_file);
	}

	if (checkWifiStatus()) {
		if (settings.ui.autodl && (checkUpdate() == 0) && !is3DSX) {
			DownloadTWLoaderCIAs();
		}

		switch (settings.ui.autoupdate) {
			case 2:
				UpdateBootstrapUnofficial();
				break;
			case 1:
				UpdateBootstrapRelease();
				break;
			default:
				break;
		}
	}

	DialogBoxDisappear(" ", 0);
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	sf2d_end_frame();
	sf2d_swapbuffers();

	int filenum = 0;
	bool noromsfound = false;
	
	bool cursorPositionset = false;
	
	int soundwaittimer = 0;
	bool playwrongsounddone = false;

	bool colortexloaded = false;
	bool colortexloaded_bot = false;
	bool bnricontexloaded = false;
	bool boxarttexloaded = false;

	bool updatetopscreen = true;
	bool screenmodeswitch = false;
	bool applaunchicon = false;
	
	float rad = 0.0f;
	
	int boxartXpos;
	int boxartXmovepos = 0;

	int filenameYpos;
	int filenameYmovepos = 0;
	int setsboxXpos = 0;
	int cartXpos = 64;
	int boxartYmovepos = 63;
	int boxartreflYmovepos = 264;
	int ndsiconXpos;
	int ndsiconYmovepos = 133;
	int wood_ndsiconscaletimer = 0;
	int wood_ndsiconscalelag = 0;
	int wood_ndsiconscalemovepos = 0;
	float wood_ndsiconscalesize = 0.00f;
	bool wood_ndsiconscaledown = false;

	int startbordermovepos = 0;
	float startborderscalesize = 1.0f;
	
	if (settings.ui.theme >= 1)
		menu_ctrlset = CTRL_SET_MENU;
	sf2d_set_3D(1);

	// Loop as long as the status is not exit
	const bool isTWLNANDInstalled = checkTWLNANDSide();
	// Save by default if the TWLNAND-side title is installed.
	// Otherwise, we don't want to save anything.
	bool saveOnExit = isTWLNANDInstalled;
	while(run && aptMainLoop()) {
	//while(run) {
		// Scan hid shared memory for input events
		hidScanInput();

		const u32 hDown = hidKeysDown();
		const u32 hHeld = hidKeysHeld();

		// Check if the TWLNAND-side title is installed.
		if (!isTWLNANDInstalled) {
			static const char twlnand_msg[] =
				"The TWLNAND-side title has not been installed.\n"
				"Please install the TWLNAND-side CIA:\n"
				"\n"
				"/_nds/twloader/cias/TWLoader - TWLNAND side.cia\n"
				"\n"
				"\n"
				"\n"
				"\n"
				"\n"
				"\n"
				"\n"
				"\n"
				"\n"
				"\n"
				"\n"
				"\n"
				"                 Press the HOME button to exit.";
			DialogBoxAppear(twlnand_msg, 0);
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_draw_texture(dialogboxtex, 0, 0);
			sftd_draw_text(font, 12, 16, RGBA8(0, 0, 0, 255), 12, twlnand_msg);
			sf2d_end_frame();
			sf2d_swapbuffers();
			continue;
		}

		offset3D[0].topbg = CONFIG_3D_SLIDERSTATE * -12.0f;
		offset3D[1].topbg = CONFIG_3D_SLIDERSTATE * 12.0f;
		offset3D[0].boxart = CONFIG_3D_SLIDERSTATE * -5.0f;
		offset3D[1].boxart = CONFIG_3D_SLIDERSTATE * 5.0f;
		offset3D[0].disabled = CONFIG_3D_SLIDERSTATE * -3.0f;
		offset3D[1].disabled = CONFIG_3D_SLIDERSTATE * 3.0f;
		
		if (showdialogbox_menu) {
			if (menudbox_movespeed <= 1) {
				if (menudbox_Ypos >= 0) {
					menudbox_movespeed = 0;
					menudbox_Ypos = 0;
				} else
					menudbox_movespeed = 1;
			} else {
				menudbox_movespeed -= 0.2;
				menudbox_bgalpha += 5;
			}
			menudbox_Ypos += menudbox_movespeed;
		} else {
			if (menudbox_Ypos <= -240 || menudbox_Ypos >= 240) {
				menudbox_movespeed = 22;
				menudbox_Ypos = -240;
			} else {
				menudbox_movespeed += 1;
				menudbox_bgalpha -= 5;
				if (menudbox_bgalpha <= 0) {
					menudbox_bgalpha = 0;
				}
				menudbox_Ypos += menudbox_movespeed;
			}
		}


		if (storedcursorPosition < 0)
			storedcursorPosition = 0;

		size_t file_count = (settings.twl.forwarder ? fcfiles.size() : files.size());

		if(matching_files.size() != 0) {
			file_count = matching_files.size();
		}
		const int pagemax = std::min((20+pagenum*20), (int)file_count);
		const int pagemax_ba = std::min((21+pagenum*20), (int)file_count);

		if(screenmode == SCREEN_MODE_ROM_SELECT) {
			if (!colortexloaded) {
				if (settings.ui.theme == 2) {
					switch (settings.ui.subtheme) {
						case 0:
						default:
							topbgtex = sfil_load_JPEG_file("romfs:/graphics/wood/gbatemp/upper_screen.jpg", SF2D_PLACE_RAM); // Top background
							break;
						case 1:
							topbgtex = sfil_load_PNG_file("romfs:/graphics/wood/black/upper_screen.png", SF2D_PLACE_RAM); // Top background
							break;
						case 2:
							topbgtex = sfil_load_JPEG_file("romfs:/graphics/wood/Adv.Evo/upper_screen.jpg", SF2D_PLACE_RAM); // Top background
							break;
						case 3:
							topbgtex = sfil_load_JPEG_file("romfs:/graphics/wood/dstwo pink/upper_screen.jpg", SF2D_PLACE_RAM); // Top background
							break;
					}
				} else if (settings.ui.theme == 1) {
					switch (settings.ui.subtheme) {
						case 0:
						default:
							toplogotex = sfil_load_JPEG_file("romfs:/graphics/r4/theme01/logo.jpg", SF2D_PLACE_RAM); // Top logo
							topbgtex = sfil_load_JPEG_file("romfs:/graphics/r4/theme01/bckgrd_1.jpg", SF2D_PLACE_RAM); // Top background
							break;
						case 1:
							toplogotex = sfil_load_JPEG_file("romfs:/graphics/r4/theme02/logo.jpg", SF2D_PLACE_RAM); // Top logo
							topbgtex = sfil_load_JPEG_file("romfs:/graphics/r4/theme02/bckgrd_1.jpg", SF2D_PLACE_RAM); // Top background
							break;
						case 2:
							toplogotex = sfil_load_JPEG_file("romfs:/graphics/r4/theme03/logo.jpg", SF2D_PLACE_RAM); // Top logo
							topbgtex = sfil_load_JPEG_file("romfs:/graphics/r4/theme03/bckgrd_1.jpg", SF2D_PLACE_RAM); // Top background
							break;
						case 3:
							toplogotex = sfil_load_JPEG_file("romfs:/graphics/r4/theme04/logo.jpg", SF2D_PLACE_RAM); // Top logo
							topbgtex = sfil_load_JPEG_file("romfs:/graphics/r4/theme04/bckgrd_1.jpg", SF2D_PLACE_RAM); // Top background
							break;
						case 4:
							toplogotex = sfil_load_JPEG_file("romfs:/graphics/r4/theme05/logo.jpg", SF2D_PLACE_RAM); // Top logo
							topbgtex = sfil_load_JPEG_file("romfs:/graphics/r4/theme05/bckgrd_1.jpg", SF2D_PLACE_RAM); // Top background
							break;
						case 5:
							toplogotex = sfil_load_JPEG_file("romfs:/graphics/r4/theme06/logo.jpg", SF2D_PLACE_RAM); // Top logo
							topbgtex = sfil_load_JPEG_file("romfs:/graphics/r4/theme06/bckgrd_1.jpg", SF2D_PLACE_RAM); // Top background
							break;
						case 6:
							toplogotex = sfil_load_JPEG_file("romfs:/graphics/r4/theme07/logo.jpg", SF2D_PLACE_RAM); // Top logo
							topbgtex = sfil_load_JPEG_file("romfs:/graphics/r4/theme07/bckgrd_1.jpg", SF2D_PLACE_RAM); // Top background
							break;
						case 7:
							toplogotex = sfil_load_JPEG_file("romfs:/graphics/r4/theme08/logo.jpg", SF2D_PLACE_RAM); // Top logo
							topbgtex = sfil_load_JPEG_file("romfs:/graphics/r4/theme08/bckgrd_1.jpg", SF2D_PLACE_RAM); // Top background
							break;
						case 8:
							toplogotex = sfil_load_JPEG_file("romfs:/graphics/r4/theme09/logo.jpg", SF2D_PLACE_RAM); // Top logo
							topbgtex = sfil_load_JPEG_file("romfs:/graphics/r4/theme09/bckgrd_1.jpg", SF2D_PLACE_RAM); // Top background
							break;
						case 9:
							toplogotex = sfil_load_JPEG_file("romfs:/graphics/r4/theme10/logo.jpg", SF2D_PLACE_RAM); // Top logo
							topbgtex = sfil_load_JPEG_file("romfs:/graphics/r4/theme10/bckgrd_1.jpg", SF2D_PLACE_RAM); // Top background
							break;
						case 10:
							toplogotex = sfil_load_JPEG_file("romfs:/graphics/r4/theme11/logo.jpg", SF2D_PLACE_RAM); // Top logo
							topbgtex = sfil_load_JPEG_file("romfs:/graphics/r4/theme11/bckgrd_1.jpg", SF2D_PLACE_RAM); // Top background
							break;
						case 11:
							toplogotex = sfil_load_PNG_file("romfs:/graphics/r4/theme12/logo.png", SF2D_PLACE_RAM); // Top logo
							topbgtex = sfil_load_PNG_file("romfs:/graphics/r4/theme12/bckgrd_1.png", SF2D_PLACE_RAM); // Top background
							break;
					}
				} else
					topbgtex = sfil_load_PNG_file(color_data->topbgloc, SF2D_PLACE_RAM); // Top background, behind the DSi-Menu border
				settingsUnloadTextures();
				colortexloaded = true;
			}
			if (!bnricontexloaded) {
				if (!settings.twl.forwarder) {
					/* sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
					sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Now loading banner icons (SD Card)...");
					sf2d_end_frame();
					sf2d_swapbuffers(); */
					char path[256];
					if(matching_files.size() == 0){
						for (bnriconnum = pagenum*20; bnriconnum < pagemax; bnriconnum++) {						
							if (bnriconnum < (int)files.size()) {
								const char *tempfile = files.at(bnriconnum).c_str();
								snprintf(path, sizeof(path), "sdmc:/_nds/twloader/bnricons/%s.bin", tempfile);
								LoadBNRIcon(path);
							} else {
								LoadBNRIcon(NULL);
							}
						}
					}else{
						for (bnriconnum = pagenum*20; bnriconnum < pagemax; bnriconnum++) {						
							if (bnriconnum < (int)matching_files.size()) {
								const char *tempfile = matching_files.at(bnriconnum).c_str();
								snprintf(path, sizeof(path), "sdmc:/_nds/twloader/bnricons/%s.bin", tempfile);
								LoadBNRIcon(path);
							} else {
								LoadBNRIcon(NULL);
							}
						}
					}
				} else {
					/* sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
					sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Now loading banner icons (Flashcard)...");
					sf2d_end_frame();
					sf2d_swapbuffers(); */
					char path[256];
					if(matching_files.size() == 0){
						for (bnriconnum = pagenum*20; bnriconnum < pagemax; bnriconnum++) {
							if (bnriconnum < (int)fcfiles.size()) {
								const char *tempfile = fcfiles.at(bnriconnum).c_str();
								snprintf(path, sizeof(path), "%s/%s.bin", fcbnriconfolder, tempfile);
								if (access(path, F_OK) != -1) {
									LoadBNRIcon(path);
								} else {
									LoadBNRIcon(NULL);
								}
							} else {
								LoadBNRIcon(NULL);
							}
						}
					}else{
						for (bnriconnum = pagenum*20; bnriconnum < pagemax; bnriconnum++) {
							if (bnriconnum < (int)matching_files.size()) {
								const char *tempfile = matching_files.at(bnriconnum).c_str();
								snprintf(path, sizeof(path), "%s/%s.bin", fcbnriconfolder, tempfile);
								if (access(path, F_OK) != -1) {
									LoadBNRIcon(path);
								} else {
									LoadBNRIcon(NULL);
								}
							} else {
								LoadBNRIcon(NULL);
							}
						}
					}
				}

				bnricontexloaded = true;
				bnriconnum = 0+pagenum*20;
			}			
			if (!boxarttexloaded) {
				if (!settings.twl.forwarder) {
					/* sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
					sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Now storing box art filenames (SD Card)...");
					sf2d_end_frame();
					sf2d_swapbuffers(); */
					char path[256];
					if(matching_files.size() == 0){
						for(boxartnum = pagenum*20; boxartnum < pagemax_ba; boxartnum++) {
							if (boxartnum < (int)files.size()) {
								const char *tempfile = files.at(boxartnum).c_str();
								snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.romfolder.c_str(), tempfile);
								FILE *f_nds_file = fopen(path, "rb");
								if (!f_nds_file) {
									// Can't open the NDS file.
									StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
									continue;
								}

								char ba_TID[5];
								grabTID(f_nds_file, ba_TID);
								ba_TID[4] = 0;
								fclose(f_nds_file);

								// example: SuperMario64DS.nds.png
								snprintf(path, sizeof(path), "%s/%s.png", boxartfolder, tempfile);
								if (access(path, F_OK ) != -1 ) {
									StoreBoxArtPath(path);
								} else {
									// example: ASME.png
									snprintf(path, sizeof(path), "%s/%.4s.png", boxartfolder, ba_TID);
									if (access(path, F_OK) != -1) {
										StoreBoxArtPath(path);
									} else {
										StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
									}
								}
							} else {
								StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
							}
						}
					}else{
						for(boxartnum = pagenum*20; boxartnum < pagemax_ba; boxartnum++) {
							if (boxartnum < (int)matching_files.size()) {
								const char *tempfile = matching_files.at(boxartnum).c_str();
								snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.romfolder.c_str(), tempfile);
								FILE *f_nds_file = fopen(path, "rb");
								if (!f_nds_file) {
									// Can't open the NDS file.
										StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
									continue;
								}

								char ba_TID[5];
								grabTID(f_nds_file, ba_TID);
								ba_TID[4] = 0;
								fclose(f_nds_file);

								// example: SuperMario64DS.nds.png
								snprintf(path, sizeof(path), "%s/%s.png", boxartfolder, tempfile);
								if (access(path, F_OK ) != -1 ) {
									StoreBoxArtPath(path);
								} else {
									// example: ASME.png
									snprintf(path, sizeof(path), "%s/%.4s.png", boxartfolder, ba_TID);
									if (access(path, F_OK) != -1) {
										StoreBoxArtPath(path);
									} else {
										StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
									}
								}
							} else {
								StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
							}
						}
					}
				} else {
					/* sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
					sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Now storing box art filenames (Flashcard)...");
					sf2d_end_frame();
					sf2d_swapbuffers(); */
					char path[256];
					if(matching_files.size() == 0){
						for(boxartnum = pagenum*20; boxartnum < pagemax; boxartnum++) {
							if (boxartnum < (int)fcfiles.size()) {
								const char *tempfile = fcfiles.at(boxartnum).c_str();
								snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.fcromfolder.c_str(), tempfile);

								CIniFile setfcrompathini( path );
								std::string ba_TIDini = setfcrompathini.GetString(fcrompathini_flashcardrom, fcrompathini_tid, "");
								if (ba_TIDini.size() < 4) {
									// TID is too short.
									StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
									continue;
								}

								char ba_TID[5];
								strcpy(ba_TID, ba_TIDini.c_str());
								ba_TID[4] = 0;

								// example: SuperMario64DS.nds.png
								snprintf(path, sizeof(path), "%s/%s.png", fcboxartfolder, tempfile);
								if (access(path, F_OK ) != -1 ) {
									StoreBoxArtPath(path);
								} else {
									// example: ASME.png
									snprintf(path, sizeof(path), "%s/%.4s.png", boxartfolder, ba_TID);
									if (access(path, F_OK) != -1) {
										StoreBoxArtPath(path);
									} else {
										StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
									}
								}
							} else {
								StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
							}
						}
					}else{
						for(boxartnum = pagenum*20; boxartnum < pagemax; boxartnum++) {
							if (boxartnum < (int)matching_files.size()) {
								const char *tempfile = matching_files.at(boxartnum).c_str();
								snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.fcromfolder.c_str(), tempfile);

								CIniFile setfcrompathini( path );
								std::string ba_TIDini = setfcrompathini.GetString(fcrompathini_flashcardrom, fcrompathini_tid, "");
								if (ba_TIDini.size() < 4) {
									// TID is too short.
									StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
									continue;
								}

								char ba_TID[5];
								strcpy(ba_TID, ba_TIDini.c_str());
								ba_TID[4] = 0;

								// example: SuperMario64DS.nds.png
								snprintf(path, sizeof(path), "%s/%s.png", fcboxartfolder, tempfile);
								if (access(path, F_OK ) != -1 ) {
									StoreBoxArtPath(path);
								} else {
									// example: ASME.png
									snprintf(path, sizeof(path), "%s/%.4s.png", boxartfolder, ba_TID);
									if (access(path, F_OK) != -1) {
										StoreBoxArtPath(path);
									} else {
										StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
									}
								}
							} else {
								StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
							}
						}
					}
				}
				
				// Load up to 6 boxarts.
				for (boxartnum = pagenum*20; boxartnum < 6+pagenum*20; boxartnum++) {
					LoadBoxArt();
				}
				boxarttexloaded = true;
				boxartnum = 0+pagenum*20;
			}
			
			if (settings.ui.theme == 2) {	// akMenu/Wood theme
				for (int topfb = GFX_LEFT; topfb <= GFX_RIGHT; topfb++) {
					sf2d_start_frame(GFX_TOP, (gfx3dSide_t)topfb);	
					sf2d_draw_texture(topbgtex, 40, 0);
					if (menu_ctrlset == CTRL_SET_MENU) {
						if (!settings.romselect.toplayout && woodmenu_cursorPosition == 2) {
							// Load the boxart for the Slot-1 cartridge if necessary.
							loadSlot1BoxArt();
							// Draw box art
							switch (settings.ui.subtheme) {
								case 0:
								default:
									sf2d_draw_texture(slot1boxarttex, offset3D[topfb].boxart+40+14, 62);
									break;
								case 1:
								case 2:
									sf2d_draw_texture(slot1boxarttex, offset3D[topfb].boxart+40+176, 113);
									break;
								case 3:
									sf2d_draw_texture(slot1boxarttex, offset3D[topfb].boxart+40+164, 38);
									break;
							}
						}
					} else {
						if (!settings.romselect.toplayout) {
							if (!bannertextloaded) {
								LoadBoxArt_WoodTheme(cursorPosition-pagenum*20);
								bannertextloaded = true;
							}
							boxartnum = 0;
							ChangeBoxArtNo();
							// Draw box art
							switch (settings.ui.subtheme) {
								case 0:
								default:
									sf2d_draw_texture(boxarttexnum, offset3D[topfb].boxart+40+14, 62);
									break;
								case 1:
								case 2:
									sf2d_draw_texture(boxarttexnum, offset3D[topfb].boxart+40+176, 113);
									break;
								case 3:
									sf2d_draw_texture(boxarttexnum, offset3D[topfb].boxart+40+164, 38);
									break;
							}
						}
					}
					switch (settings.ui.subtheme) {
						case 0:
						default:
							sftd_draw_text(font_b, 40+200, 148, RGBA8(16, 0, 0, 223), 22, RetTime(true).c_str());
							DrawDateF(22+197, 198, FORMAT_MY, RGBA8(16, 0, 0, 223), 22);
							break;
						case 1:
							sftd_draw_text(font_b, 40+184, 8, RGBA8(255, 255, 255, 255), 33, RetTime(true).c_str());
							DrawDateF(40+182, 78, FORMAT_MY, RGBA8(255, 255, 255, 255), 22);
							break;
						case 2:
							sftd_draw_text(font_b, 40+16, 76, RGBA8(255, 255, 255, 255), 33, RetTime(true).c_str());
							DrawDateF(40+69, 204, FORMAT_MY, RGBA8(255, 255, 255, 255), 19);
							break;
						case 3:
							sftd_draw_text(font_b, 40+176, 172, RGBA8(255, 255, 255, 255), 33, RetTime(true).c_str());
							break;
					}
					sf2d_draw_rectangle(0, 0, 40, 240, RGBA8(0, 0, 0, 255)); // Left black bar
					sf2d_draw_rectangle(360, 0, 40, 240, RGBA8(0, 0, 0, 255)); // Right black bar
					sf2d_end_frame();
				}
			} else if (settings.ui.theme == 1) {	// R4 theme
				if (updatetopscreen) {
					sf2d_start_frame(GFX_TOP, GFX_LEFT);	
					if (menu_ctrlset != CTRL_SET_MENU) {
						sf2d_draw_texture(topbgtex, 40, 0);
						filenameYpos = 15;
						const int file_count = (settings.twl.forwarder ? fcfiles.size() : files.size());
						for (filenum = 0; filenum < file_count; filenum++) {
							u32 color;
							if (cursorPosition == filenum) {
								color = SET_ALPHA(color_data->color, 255);
							} else {
								color = RGBA8(255, 255, 255, 255);
							}

							// Get the current filename and convert it to wstring.
							const char *filename = (settings.twl.forwarder
									? fcfiles.at(filenum).c_str()
									: files.at(filenum).c_str());
							wstring wstr = utf8_to_wstring(filename);
							sftd_draw_wtext(font, 42, filenameYpos+filenameYmovepos*15, color, 12, wstr.c_str());

							filenameYpos += 15;
						}

						sf2d_draw_texture_part(topbgtex, 40, 0, 0, 0, 320, 15);
						const char *title = (settings.twl.forwarder
									? "Games (Flashcard)"
									: "Games (SD Card)");
						sftd_draw_textf(font, 42, 0, RGBA8(0, 0, 0, 255), 12, title);
						
						char romsel_counter1[16];
						char romsel_counter2[16];
						snprintf(romsel_counter1, sizeof(romsel_counter1), "%d", cursorPosition+1);		
						// if(matching_files.size() == 0){
							snprintf(romsel_counter2, sizeof(romsel_counter2), "%d", file_count);
						// }else{
						// 	snprintf(romsel_counter2, sizeof(romsel_counter2), "%zu", matching_files.size());
						// }
						
						if (settings.ui.counter) {
							if (file_count < 100) {
								sftd_draw_text(font, 40+276, 0, RGBA8(0, 0, 0, 255), 12, romsel_counter1);
								sftd_draw_text(font, 40+295, 0, RGBA8(0, 0, 0, 255), 12, "/");
								sftd_draw_text(font, 40+300, 0, RGBA8(0, 0, 0, 255), 12, romsel_counter2);
							} else {
								sftd_draw_text(font, 40+276, 0, RGBA8(0, 0, 0, 255), 12, romsel_counter1);
								sftd_draw_text(font, 40+303, 0, RGBA8(0, 0, 0, 255), 12, "/");
								sftd_draw_text(font, 40+308, 0, RGBA8(0, 0, 0, 255), 12, romsel_counter2);
							}
						}
					} else {
						sf2d_draw_texture(toplogotex, 40, 0);
					}
					sf2d_draw_rectangle(0, 0, 40, 240, RGBA8(0, 0, 0, 255)); // Left black bar
					sf2d_draw_rectangle(360, 0, 40, 240, RGBA8(0, 0, 0, 255)); // Right black bar
					sf2d_end_frame();
					updatetopscreen = false;
				}
			} else {	// DSi-Menu theme
				if (!settings.twl.forwarder) {
					// Load the boxart for the Slot-1 cartridge if necessary.
					loadSlot1BoxArt();
				}

				if (!musicbool) {
					if (dspfirmfound) { bgm_menu->play(); }
					musicbool = true;
				}
				if (settings.twl.forwarder) {
					noromtext1 = "No games found!";
					noromtext2 = "Select \"Add Games\" to get started.";
				} else {
					noromtext1 = "No games found!";
					noromtext2 = " ";
				}

				update_battery_level(batterychrgtex, batterytex);
				for (int topfb = GFX_LEFT; topfb <= GFX_RIGHT; topfb++) {
					sf2d_start_frame(GFX_TOP, (gfx3dSide_t)topfb);	
					sf2d_draw_texture_scale(topbgtex, offset3D[topfb].topbg-12, 0, 1.32, 1);
					if (filenum != 0) {	// If ROMs are found, then display box art
						if (!settings.romselect.toplayout) {
							boxartXpos = 136;
							if (!settings.twl.forwarder && pagenum == 0) {
								if (cursorPosition < 2) {
									sf2d_draw_texture(slot1boxarttex, offset3D[topfb].boxart+boxartXpos-144+boxartXmovepos, 240/2 - slot1boxarttex->height/2); // Draw box art
									sf2d_draw_texture_scale_blend(slot1boxarttex, offset3D[topfb].boxart+boxartXpos-144+boxartXmovepos, 264, 1, -0.75, SET_ALPHA(color_data->color, 0xC0)); // Draw box art's reflection
								}
							}
							for (boxartnum = pagenum*20; boxartnum < pagemax; boxartnum++) {
								if (boxartnum < 9+pagenum*20) {
									ChangeBoxArtNo();
									// Draw box art
									sf2d_draw_texture(boxarttexnum, offset3D[topfb].boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2);
									// Draw box art's reflection
									sf2d_draw_texture_scale_blend(boxarttexnum, offset3D[topfb].boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, SET_ALPHA(color_data->color, 0xC0));
									boxartXpos += 144;
								}
							}
							if (applaunchprep) {
								if (cursorPosition >= 0) {
									boxartnum = cursorPosition;
									ChangeBoxArtNo();
									sf2d_draw_texture_part(topbgtex, offset3D[topfb].boxart+136, 63, offset3D[topfb].boxart+104, 63, 128, 115*2);
									// Draw moving box art
									sf2d_draw_texture(boxarttexnum, offset3D[topfb].boxart+136, boxartYmovepos);
									// Draw moving box art's reflection
									sf2d_draw_texture_scale_blend(boxarttexnum, offset3D[topfb].boxart+136, boxartreflYmovepos, 1, -0.75, SET_ALPHA(color_data->color, 0xC0));
								} else if (!settings.twl.forwarder && cursorPosition == -1) {
									sf2d_draw_texture_part(topbgtex, offset3D[topfb].boxart+136, 63, offset3D[topfb].boxart+104, 63, 128, 115*2);
									sf2d_draw_texture(slot1boxarttex, offset3D[topfb].boxart+136, boxartYmovepos); // Draw moving box art
									sf2d_draw_texture_scale_blend(slot1boxarttex, offset3D[topfb].boxart+136, boxartreflYmovepos, 1, -0.75, SET_ALPHA(color_data->color, 0xC0)); // Draw moving box art's reflection
								}
							}
						}
					} else {
						if (!settings.romselect.toplayout) {
							boxartXpos = 136;
							if (!settings.twl.forwarder && pagenum == 0) {
								if (cursorPosition < 2) {
									sf2d_draw_texture(slot1boxarttex, offset3D[topfb].boxart+boxartXpos+boxartXmovepos, 240/2 - slot1boxarttex->height/2); // Draw box art
									sf2d_draw_texture_scale_blend(slot1boxarttex, offset3D[topfb].boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, SET_ALPHA(color_data->color, 0xC0)); // Draw box art's reflection
								}
							} else {
								int text_width = sftd_get_text_width(font, 12, noromtext1);
								sftd_draw_textf(font, offset3D[topfb].boxart+((400-text_width)/2), 96, RGBA8(255, 255, 255, 255), 12, noromtext1);
								text_width = sftd_get_text_width(font, 12, noromtext2);
								sftd_draw_textf(font, offset3D[topfb].boxart+((400-text_width)/2), 112, RGBA8(255, 255, 255, 255), 12, noromtext2);
							}
						} else {
							if (settings.twl.forwarder && pagenum == 0) {
								int text_width = sftd_get_text_width(font, 12, noromtext1);
								sftd_draw_textf(font, offset3D[topfb].boxart+((400-text_width)/2), 96, RGBA8(255, 255, 255, 255), 12, noromtext1);
								text_width = sftd_get_text_width(font, 12, noromtext2);
								sftd_draw_textf(font, offset3D[topfb].boxart+((400-text_width)/2), 112, RGBA8(255, 255, 255, 255), 12, noromtext2);
							}
						}
					}
					if (settings.ui.topborder) {
						sf2d_draw_texture_blend(toptex, 400/2 - toptex->width/2, 240/2 - toptex->height/2, menucolor);
						sftd_draw_text(font, 328, 3, RGBA8(0, 0, 0, 255), 12, RetTime(false).c_str());
						DrawDate(282, 3, RGBA8(0, 0, 0, 255), 12);
					} else {
						sftd_draw_text(font, 328, 3, RGBA8(255, 255, 255, 255), 12, RetTime(false).c_str());
						DrawDate(282, 3, RGBA8(255, 255, 255, 255), 12);
					}

					draw_volume_slider(voltex);
					sf2d_draw_texture(batteryIcon, 371, 2);
					if (!settings.ui.name.empty()) {
						sftd_draw_textf(font, 34, 3, SET_ALPHA(color_data->color, 255), 12, settings.ui.name.c_str());
					}
					// sftd_draw_textf(font, 2, 2, RGBA8(0, 0, 0, 255), 12, temptext); // Debug text
					sf2d_draw_texture(shoulderLtex, 0, LshoulderYpos);
					// sftd_draw_textf(font, 17, LshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Lshouldertext);
					sf2d_draw_texture(shoulderRtex, 328, RshoulderYpos);
					// sftd_draw_textf(font, 332, RshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Rshouldertext);
					// Draw the "Prev" and "Next" text for X/Y.
					u32 lr_color = (pagenum != 0 && file_count <= (size_t)-pagenum*20)
							? RGBA8(0, 0, 0, 255)
							: RGBA8(127, 127, 127, 255);
					sftd_draw_text(font, 17, LshoulderYpos+5, lr_color, 11, "Prev. Page");

					lr_color = (file_count > (size_t)20+pagenum*20)
							? RGBA8(0, 0, 0, 255)
							: RGBA8(127, 127, 127, 255);
					sftd_draw_text(font, 332, RshoulderYpos+5, lr_color, 11, "Next Page");

					sf2d_draw_rectangle(0, 0, 400, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect
					sf2d_end_frame();
				}
			}
		} else if (screenmode == SCREEN_MODE_SETTINGS) {
			if (colortexloaded) {
				sf2d_free_texture(topbgtex);
				topbgtex = NULL;
				colortexloaded = false;
			}
			settingsDrawTopScreen();
		}
					
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
		
		/* if(hHeld & KEY_Y){
			if (YbuttonYpos != 223)
			{YbuttonYpos += 1;}
		} else {
			if (YbuttonYpos != 220)
			{YbuttonYpos -= 1;}
		}
		if(hHeld & KEY_X){
			if (XbuttonYpos != 223)
			{XbuttonYpos += 1;}
		} else {
			if (XbuttonYpos != 220)
			{XbuttonYpos -= 1;}
		} */
		
		if (fadein) {
			fadealpha -= 31;
			if (fadealpha < 0) {
				fadealpha = 0;
				fadein = false;
				titleboxXmovetimer = 0;
			}
		}
		
		if (fadeout) {
			fadealpha += 31;
			if (fadealpha > 255) {
				fadealpha = 255;
				musicbool = false;
				if(screenmode == SCREEN_MODE_SETTINGS) {
					screenmode = SCREEN_MODE_ROM_SELECT;
					fadeout = false;
					fadein = true;

					// Poll for Slot-1 changes.
					gamecardPoll(true);

					// Force banner text reload in case
					// the Slot-1 cartridge was changed
					// or the UI language was changed.
					bannertextloaded = false;
					
					// Reload language
					langInit();
					
					// Clear matching_files vector
					if(matching_files.size() != 0) {
						matching_files.clear(); // Clear filter
						snprintf(romsel_counter2sd, sizeof(romsel_counter2sd), "%zu", files.size()); // Reload counter
						snprintf(romsel_counter2fc, sizeof(romsel_counter2fc), "%zu", fcfiles.size()); // Reload counter for FlashCard
						boxarttexloaded = false; // Reload boxarts
						bnricontexloaded = false; // Reload banner icons
					
						/** This is better than a glitched screen */
						sf2d_start_frame(GFX_TOP, GFX_LEFT);
						sf2d_end_frame();
						sf2d_swapbuffers();
					}
					if (settings.ui.theme == 2) {
						/** This is better than a glitched screen */
						sf2d_start_frame(GFX_TOP, GFX_LEFT);
						sf2d_end_frame();
						sf2d_swapbuffers();
						
						menu_ctrlset = CTRL_SET_MENU;
						woodmenu_cursorPosition = 4;
						char path[256];
						if (cursorPosition < 0)
							cursorPosition = 0;
						// Reload 1st icon
						if (settings.twl.forwarder) {
							const char *tempfile = fcfiles.at(0).c_str();
							snprintf(path, sizeof(path), "sdmc:/_nds/twloader/bnricons/flashcard/%s.bin", tempfile);
						} else {
							const char *tempfile = files.at(0).c_str();
							snprintf(path, sizeof(path), "sdmc:/_nds/twloader/bnricons/%s.bin", tempfile);
						}
						if (access(path, F_OK) != -1) {
							LoadBNRIcon_R4Theme(path);
						} else {
							LoadBNRIcon_R4Theme(NULL);
						}
					} else if (settings.ui.theme == 1) {
						/** This is better than a glitched screen */
						sf2d_start_frame(GFX_TOP, GFX_LEFT);
						sf2d_end_frame();
						sf2d_swapbuffers();
						
						menu_ctrlset = CTRL_SET_MENU;
						titleboxXmovepos = 0;
						boxartXmovepos = 0;
						if (cursorPosition < 0)
							cursorPosition = 0;
					} else {
						cursorPosition = 0+pagenum*20; // This is to reset cursor position after switching from R4 theme.
						storedcursorPosition = cursorPosition; // This is to reset cursor position after switching from R4 theme.
						titleboxXmovepos = 0;
						boxartXmovepos = 0;
						char path[256];
						// Reload 1st icon
						if (settings.twl.forwarder) {
							const char *tempfile = fcfiles.at(0+pagenum*20).c_str();
							snprintf(path, sizeof(path), "sdmc:/_nds/twloader/bnricons/flashcard/%s.bin", tempfile);
						} else {
							const char *tempfile = files.at(0+pagenum*20).c_str();
							snprintf(path, sizeof(path), "sdmc:/_nds/twloader/bnricons/%s.bin", tempfile);
						}
						if (access(path, F_OK) != -1) {
							LoadBNRIcon_R4Theme(path);
						} else {
							LoadBNRIcon_R4Theme(NULL);
						}
						// Reload 1st box art
						LoadBoxArt_WoodTheme(0-pagenum*20);
						menu_ctrlset = CTRL_SET_GAMESEL;
					}
				} else if (gbarunnervalue == 1) {
					if (logEnabled)	LogFM("Main", "Switching to NTR/TWL-mode");
					applaunchon = true;
				}
			}
		}
		
		if (playwrongsounddone) {
			if (hHeld & KEY_LEFT || hHeld & KEY_RIGHT) {} else {
				soundwaittimer += 1;
				if (soundwaittimer == 2) {
					soundwaittimer = 0;
					playwrongsounddone = false;
				}
			}
		}

		if (titleboxXmoveleft) {
			titleboxXmovetimer += 1;
			if (titleboxXmovetimer == 10) {
				titleboxXmovetimer = 0;
				titleboxXmoveleft = false;
			} else if (titleboxXmovetimer == 9) {
				// Delay a frame
				bannertextloaded = false;			
				storedcursorPosition = cursorPosition;
				if (dspfirmfound) { sfx_stop->stop(); }
				if (dspfirmfound) { sfx_stop->play(); }
			} else if (titleboxXmovetimer == 8) {
				titleboxXmovepos += 8;
				boxartXmovepos += 18;
				startbordermovepos = 1;
				startborderscalesize = 0.97;
				cursorPositionset = false;
			} else if (titleboxXmovetimer == 2) {
				titleboxXmovepos += 8;
				boxartXmovepos += 18;
				if (dspfirmfound) { sfx_select->stop(); }
				if (dspfirmfound) { sfx_select->play(); }
				// Load the previous box art
				if ( cursorPosition == 3+pagenum*20 ||
				cursorPosition == 6+pagenum*20 ||
				cursorPosition == 9+pagenum*20 ||
				cursorPosition == 12+pagenum*20 ||
				cursorPosition == 15+pagenum*20 ||
				cursorPosition == 18+pagenum*20 ) {
					boxartnum = cursorPosition-1;
					LoadBoxArt();
					boxartnum--;
					LoadBoxArt();
					boxartnum--;
					LoadBoxArt();
				}
				if ( cursorPosition == 6+pagenum*20 ||
				cursorPosition == 12+pagenum*20 ||
				cursorPosition == 18+pagenum*20 ) {
					boxartXmovepos = -144*7;
					boxartXmovepos += 18*2;
				}
			} else {
				if (!cursorPositionset) {
					cursorPosition--;
					cursorPositionset = true;
				}
				if (pagenum == 0) {
					if (cursorPosition != -3) {
						titleboxXmovepos += 8;
						boxartXmovepos += 18;
					} else {
						titleboxXmovetimer = 0;
						titleboxXmoveleft = false;
						cursorPositionset = false;
						cursorPosition++;
						if (!playwrongsounddone) {
							if (dspfirmfound) {
								sfx_wrong->stop();
								sfx_wrong->play();
							}
							playwrongsounddone = true;
						}
					}
				} else {
					if (cursorPosition != -1+pagenum*20) {
						titleboxXmovepos += 8;
						boxartXmovepos += 18;
					} else {
						titleboxXmovetimer = 0;
						titleboxXmoveleft = false;
						cursorPositionset = false;
						cursorPosition++;
						if (!playwrongsounddone) {
							if (dspfirmfound) {
								sfx_wrong->stop();
								sfx_wrong->play();
							}
							playwrongsounddone = true;
						}
					}
				}
			}
		} else if(titleboxXmoveright) {
			titleboxXmovetimer += 1;
			if (titleboxXmovetimer == 10) {
				titleboxXmovetimer = 0;
				titleboxXmoveright = false;
			} else if (titleboxXmovetimer == 9) {
				// Delay a frame
				bannertextloaded = false;
				storedcursorPosition = cursorPosition;
				if (dspfirmfound) { sfx_stop->stop(); }
				if (dspfirmfound) { sfx_stop->play(); }
				// Load the next box art
				if ( cursorPosition == 4+pagenum*20 ||
				cursorPosition == 7+pagenum*20 ||
				cursorPosition == 10+pagenum*20 ||
				cursorPosition == 13+pagenum*20 ||
				cursorPosition == 16+pagenum*20 ||
				cursorPosition == 19+pagenum*20 ) {
					boxartnum = cursorPosition+2;
					LoadBoxArt();
					boxartnum++;
					LoadBoxArt();
					boxartnum++;
					LoadBoxArt();
				}
				if ( cursorPosition == 7+pagenum*20 ||
				cursorPosition == 13+pagenum*20 ||
				cursorPosition == 19+pagenum*20 ) {
					boxartXmovepos = -144;
				}
			} else if (titleboxXmovetimer == 8) {
				titleboxXmovepos -= 8;
				boxartXmovepos -= 18;
				startbordermovepos = 1;
				startborderscalesize = 0.97;
				cursorPositionset = false;
			} else if (titleboxXmovetimer == 2) {
				titleboxXmovepos -= 8;
				boxartXmovepos -= 18;
				if (dspfirmfound) { sfx_select->stop(); }
				if (dspfirmfound) { sfx_select->play(); }
			} else {
				if (!cursorPositionset) {
					cursorPosition++;
					cursorPositionset = true;
				}
				if (cursorPosition != filenum) {
					titleboxXmovepos -= 8;
					boxartXmovepos -= 18;
				} else {
					titleboxXmovetimer = 0;
					titleboxXmoveright = false;
					cursorPositionset = false;
					cursorPosition--;
					if (!playwrongsounddone) {
						if (dspfirmfound) {
							sfx_wrong->stop();
							sfx_wrong->play();
						}
						playwrongsounddone = true;
					}
				}
			}
		}
		if (applaunchprep) {
			rad += 0.50f;
			boxartYmovepos -= 6;
			boxartreflYmovepos += 2;
			titleboxYmovepos -= 6;
			ndsiconYmovepos -= 6;
			if (titleboxYmovepos == -240) {
				if (screenmodeswitch) {
					musicbool = false;
					screenmode = SCREEN_MODE_SETTINGS;
					settingsResetSubScreenMode();
					rad = 0.0f;
					boxartYmovepos = 63;
					boxartreflYmovepos = 264;
					titleboxYmovepos = 120;
					ndsiconYmovepos = 133;
					fadein = true;
					screenmodeswitch = false;
					applaunchprep = false;
				} else {
					if (logEnabled)	LogFM("Main.applaunchprep", "Switching to NTR/TWL-mode");
					applaunchon = true;
				}
			}
			fadealpha += 6;
			if (fadealpha > 255) {
				fadealpha = 255;
			}
		}

		//if (updatebotscreen) {
			if (screenmode == SCREEN_MODE_ROM_SELECT) {
				if (!colortexloaded_bot) {
					settingsUnloadTextures();
					dotcircletex = sfil_load_PNG_file(color_data->dotcircleloc, SF2D_PLACE_RAM); // Dots forming a circle
					startbordertex = sfil_load_PNG_file(color_data->startborderloc, SF2D_PLACE_RAM); // "START" border
					if (settings.ui.theme == 2) {
						switch (settings.ui.subtheme) {
							case 0:
							default:
								bottomtex = sfil_load_JPEG_file("romfs:/graphics/wood/gbatemp/lower_screen.jpg", SF2D_PLACE_RAM); // Bottom of menu
								break;
							case 1:
								bottomtex = sfil_load_PNG_file("romfs:/graphics/wood/black/lower_screen.png", SF2D_PLACE_RAM); // Bottom of menu
								break;
							case 2:
								bottomtex = sfil_load_PNG_file("romfs:/graphics/wood/Adv.Evo/lower_screen.png", SF2D_PLACE_RAM); // Bottom of menu
								break;
							case 3:
								bottomtex = sfil_load_JPEG_file("romfs:/graphics/wood/dstwo pink/lower_screen.jpg", SF2D_PLACE_RAM); // Bottom of menu
								break;
						}
					} else if (settings.ui.theme == 1) {
						switch (settings.ui.subtheme) {
							case 0:
							default:
								iconstex = sfil_load_JPEG_file("romfs:/graphics/r4/theme01/icons.jpg", SF2D_PLACE_RAM); // Bottom of menu
								bottomtex = sfil_load_PNG_file("romfs:/graphics/r4/theme01/bckgrd_2.png", SF2D_PLACE_RAM); // Bottom of rom select
								break;
							case 1:
								iconstex = sfil_load_JPEG_file("romfs:/graphics/r4/theme02/icons.jpg", SF2D_PLACE_RAM); // Bottom of menu
								bottomtex = sfil_load_PNG_file("romfs:/graphics/r4/theme02/bckgrd_2.png", SF2D_PLACE_RAM); // Bottom of rom select
								break;
							case 2:
								iconstex = sfil_load_JPEG_file("romfs:/graphics/r4/theme03/icons.jpg", SF2D_PLACE_RAM); // Bottom of menu
								bottomtex = sfil_load_PNG_file("romfs:/graphics/r4/theme03/bckgrd_2.png", SF2D_PLACE_RAM); // Bottom of rom select
								break;
							case 3:
								iconstex = sfil_load_JPEG_file("romfs:/graphics/r4/theme04/icons.jpg", SF2D_PLACE_RAM); // Bottom of menu
								bottomtex = sfil_load_PNG_file("romfs:/graphics/r4/theme04/bckgrd_2.png", SF2D_PLACE_RAM); // Bottom of rom select
								break;
							case 4:
								iconstex = sfil_load_JPEG_file("romfs:/graphics/r4/theme05/icons.jpg", SF2D_PLACE_RAM); // Bottom of menu
								bottomtex = sfil_load_PNG_file("romfs:/graphics/r4/theme05/bckgrd_2.png", SF2D_PLACE_RAM); // Bottom of rom select
								break;
							case 5:
								iconstex = sfil_load_JPEG_file("romfs:/graphics/r4/theme06/icons.jpg", SF2D_PLACE_RAM); // Bottom of menu
								bottomtex = sfil_load_PNG_file("romfs:/graphics/r4/theme06/bckgrd_2.png", SF2D_PLACE_RAM); // Bottom of rom select
								break;
							case 6:
								iconstex = sfil_load_JPEG_file("romfs:/graphics/r4/theme07/icons.jpg", SF2D_PLACE_RAM); // Bottom of menu
								bottomtex = sfil_load_PNG_file("romfs:/graphics/r4/theme07/bckgrd_2.png", SF2D_PLACE_RAM); // Bottom of rom select
								break;
							case 7:
								iconstex = sfil_load_JPEG_file("romfs:/graphics/r4/theme08/icons.jpg", SF2D_PLACE_RAM); // Bottom of menu
								bottomtex = sfil_load_PNG_file("romfs:/graphics/r4/theme08/bckgrd_2.png", SF2D_PLACE_RAM); // Bottom of rom select
								break;
							case 8:
								iconstex = sfil_load_JPEG_file("romfs:/graphics/r4/theme09/icons.jpg", SF2D_PLACE_RAM); // Bottom of menu
								bottomtex = sfil_load_PNG_file("romfs:/graphics/r4/theme09/bckgrd_2.png", SF2D_PLACE_RAM); // Bottom of rom select
								break;
							case 9:
								iconstex = sfil_load_JPEG_file("romfs:/graphics/r4/theme10/icons.jpg", SF2D_PLACE_RAM); // Bottom of menu
								bottomtex = sfil_load_PNG_file("romfs:/graphics/r4/theme10/bckgrd_2.png", SF2D_PLACE_RAM); // Bottom of rom select
								break;
							case 10:
								iconstex = sfil_load_JPEG_file("romfs:/graphics/r4/theme11/icons.jpg", SF2D_PLACE_RAM); // Bottom of menu
								bottomtex = sfil_load_PNG_file("romfs:/graphics/r4/theme11/bckgrd_2.png", SF2D_PLACE_RAM); // Bottom of rom select
								break;
							case 11:
								iconstex = sfil_load_PNG_file("romfs:/graphics/r4/theme12/icons.png", SF2D_PLACE_RAM); // Bottom of menu
								bottomtex = sfil_load_PNG_file("romfs:/graphics/r4/theme12/bckgrd_2.png", SF2D_PLACE_RAM); // Bottom of rom select
								break;
						}
					} else {
						bottomtex = sfil_load_PNG_file(bottomloc, SF2D_PLACE_RAM); // Bottom of menu
					}
					colortexloaded_bot = true;
				}
				sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				
				if (settings.ui.theme == 2) {
					if (wood_ndsiconscaletimer == 60) {
						// Scale icon at 30fps
						if (wood_ndsiconscalelag == 1) {
							if (wood_ndsiconscaledown) {
								wood_ndsiconscalemovepos -= 1;
								wood_ndsiconscalesize -= 0.06f;
								if (wood_ndsiconscalemovepos == 0)
									wood_ndsiconscaledown = false;
							} else {
								wood_ndsiconscalemovepos += 1;
								wood_ndsiconscalesize += 0.06f;
								if (wood_ndsiconscalemovepos == 4)
									wood_ndsiconscaledown = true;
							}
							wood_ndsiconscalelag = 0;
						} else {
							wood_ndsiconscalelag = 1;
						}
					} else {
						wood_ndsiconscaletimer += 1;
						wood_ndsiconscalemovepos = 0;
						wood_ndsiconscalesize = 0.00f;
						wood_ndsiconscaledown = false;
					}
				
					sf2d_draw_texture(bottomtex, 320/2 - bottomtex->width/2, 240/2 - bottomtex->height/2);
					if (menu_ctrlset == CTRL_SET_MENU) {
						// Poll for Slot-1 changes.
						bool forcePoll = false;
						bool doSlot1Update = false;
						if (gamecardIsInserted() && gamecardGetType() == CARD_TYPE_UNKNOWN) {
							// Card is inserted, but we don't know its type.
							// Force an update.
							forcePoll = true;
						}
						bool s1chg = gamecardPoll(forcePoll);
						if (s1chg) {
							// Update Slot-1 if:
							// - forcePoll is false
							// - forcePoll is true, and card is no longer unknown.
							doSlot1Update = (!forcePoll || gamecardGetType() != CARD_TYPE_UNKNOWN);
						}
						if (doSlot1Update) {
							// Slot-1 card has changed.
							if (cursorPosition == -1) {
								// Reload the banner text.
								bannertextloaded = false;
							}
						}
						sf2d_texture *cardicontex = gamecardGetIcon();
						if (!cardicontex)
							cardicontex = iconnulltex;

						int Ypos = 26;
						filenameYpos = 36;
						if (woodmenu_cursorPosition == 0) {
							sf2d_draw_rectangle(0, Ypos-4, 320, 40, SET_ALPHA(color_data->color, 127));
							sf2d_draw_texture_part_scale(sdicontex, 8-wood_ndsiconscalemovepos, -wood_ndsiconscalemovepos+Ypos, bnriconframenum*32, 0, 32, 32, 1.00+wood_ndsiconscalesize, 1.00+wood_ndsiconscalesize);
						} else
							sf2d_draw_texture_part(sdicontex, 8, Ypos, bnriconframenum*32, 0, 32, 32);
						sftd_draw_textf(font, 46, filenameYpos, RGBA8(255, 255, 255, 255), 12, "Games (SD Card)");
						Ypos += 39;
						filenameYpos += 39;
						if (woodmenu_cursorPosition == 1) {
							sf2d_draw_rectangle(0, Ypos-4, 320, 40, SET_ALPHA(color_data->color, 127));
							sf2d_draw_texture_part_scale(flashcardicontex, 8-wood_ndsiconscalemovepos, -wood_ndsiconscalemovepos+Ypos, bnriconframenum*32, 0, 32, 32, 1.00+wood_ndsiconscalesize, 1.00+wood_ndsiconscalesize);
						} else
							sf2d_draw_texture_part(flashcardicontex, 8, Ypos, bnriconframenum*32, 0, 32, 32);
						sftd_draw_textf(font, 46, filenameYpos, RGBA8(255, 255, 255, 255), 12, "Games (Flashcard)");
						Ypos += 39;
						filenameYpos += 39;
						if (woodmenu_cursorPosition == 2) {
							sf2d_draw_rectangle(0, Ypos-4, 320, 40, SET_ALPHA(color_data->color, 127));
							sf2d_draw_texture_part_scale(cardicontex, 8-wood_ndsiconscalemovepos, -wood_ndsiconscalemovepos+Ypos, bnriconframenum*32, 0, 32, 32, 1.00+wood_ndsiconscalesize, 1.00+wood_ndsiconscalesize);
						} else
							sf2d_draw_texture_part(cardicontex, 8, Ypos, bnriconframenum*32, 0, 32, 32);
						sftd_draw_textf(font, 46, filenameYpos, RGBA8(255, 255, 255, 255), 12, "Launch Slot-1 card");
						Ypos += 39;
						filenameYpos += 39;
						if (woodmenu_cursorPosition == 3) {
							sf2d_draw_rectangle(0, Ypos-4, 320, 40, SET_ALPHA(color_data->color, 127));
							sf2d_draw_texture_part_scale(gbaicontex, 8-wood_ndsiconscalemovepos, -wood_ndsiconscalemovepos+Ypos, bnriconframenum*32, 0, 32, 32, 1.00+wood_ndsiconscalesize, 1.00+wood_ndsiconscalesize);
						} else
							sf2d_draw_texture_part(gbaicontex, 8, Ypos, bnriconframenum*32, 0, 32, 32);
						sftd_draw_textf(font, 46, filenameYpos, RGBA8(255, 255, 255, 255), 12, "Start GBARunner2");
						Ypos += 39;
						filenameYpos += 39;
						if (woodmenu_cursorPosition == 4) {
							sf2d_draw_rectangle(0, Ypos-4, 320, 40, SET_ALPHA(color_data->color, 127));
							sf2d_draw_texture_part_scale(smallsettingsicontex, 8-wood_ndsiconscalemovepos, -wood_ndsiconscalemovepos+Ypos, bnriconframenum*32, 0, 32, 32, 1.00+wood_ndsiconscalesize, 1.00+wood_ndsiconscalesize);
						} else
							sf2d_draw_texture_part(smallsettingsicontex, 8, Ypos, bnriconframenum*32, 0, 32, 32);
						sftd_draw_wtextf(font, 46, filenameYpos, RGBA8(255, 255, 255, 255), 12, TR(STR_SETTINGS_TEXT));
					} else {
						int Ypos = 26;
						filenameYpos = 36;
						for (filenum = pagenum*20; filenum < pagemax; filenum++) {
							bnriconnum = filenum;
							ChangeBNRIconNo();
							if (cursorPosition == filenum) {
								sf2d_draw_rectangle(0, Ypos-4+filenameYmovepos*39, 320, 40, SET_ALPHA(color_data->color, 127));
							}

							// Get the current filename and convert it to wstring.
							const char *filename = (settings.twl.forwarder
									? fcfiles.at(filenum).c_str()
									: files.at(filenum).c_str());
							wstring wstr = utf8_to_wstring(filename);
							sftd_draw_wtext(font, 46, filenameYpos+filenameYmovepos*39, RGBA8(255, 255, 255, 255), 12, wstr.c_str());

							if (cursorPosition == filenum)
								sf2d_draw_texture_part_scale(bnricontexnum, 8-wood_ndsiconscalemovepos, -wood_ndsiconscalemovepos+Ypos+filenameYmovepos*39, bnriconframenum*32, 0, 32, 32, 1.00+wood_ndsiconscalesize, 1.00+wood_ndsiconscalesize);
							else
								sf2d_draw_texture_part(bnricontexnum, 8, Ypos+filenameYmovepos*39, bnriconframenum*32, 0, 32, 32);
							Ypos += 39;
							filenameYpos += 39;
						}
						sf2d_draw_texture_part(bottomtex, 0, 0, 0, 0, 320, 22);
						sf2d_draw_texture_part(bottomtex, 0, 217, 0, 217, 320, 23);
						if (settings.twl.forwarder)
							sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Games (Flashcard)");
						else
							sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Games (SD Card)");
						
						const size_t file_count = (settings.twl.forwarder ? fcfiles.size() : files.size());
						
						char romsel_counter1[16];
						char romsel_counter2[16];
						snprintf(romsel_counter1, sizeof(romsel_counter1), "%d", cursorPosition+1);		
						// if(matching_files.size() == 0){
							snprintf(romsel_counter2, sizeof(romsel_counter2), "%zu", file_count);
						// }else{
						// 	snprintf(romsel_counter2, sizeof(romsel_counter2), "%zu", matching_files.size());
						// }
						
						if (settings.ui.counter) {
							if (file_count < 100) {
								sftd_draw_text(font, 276, 2, RGBA8(255, 255, 255, 255), 12, romsel_counter1);
								sftd_draw_text(font, 295, 2, RGBA8(255, 255, 255, 255), 12, "/");
								sftd_draw_text(font, 300, 2, RGBA8(255, 255, 255, 255), 12, romsel_counter2);
							} else {
								sftd_draw_text(font, 276, 2, RGBA8(255, 255, 255, 255), 12, romsel_counter1);
								sftd_draw_text(font, 303, 2, RGBA8(255, 255, 255, 255), 12, "/");
								sftd_draw_text(font, 308, 2, RGBA8(255, 255, 255, 255), 12, romsel_counter2);
							}
						}
					}
				} else if (settings.ui.theme == 1) {
					if (menu_ctrlset == CTRL_SET_MENU) {
						sf2d_draw_texture(iconstex, 320/2 - iconstex->width/2, 240/2 - iconstex->height/2);
						if (r4menu_cursorPosition == 0) {
							sf2d_draw_rectangle(12, 77, 92, 91, SET_ALPHA(color_data->color, 255));
							sf2d_draw_texture_part(iconstex, 14, 79, 14, 79, 88, 87);
							static const char selectiontext[] = "Games";
							const int text_width = sftd_get_text_width(font, 14, selectiontext);
							sftd_draw_textf(font, (320-text_width)/2, 220, RGBA8(255, 255, 255, 255), 14, selectiontext);
						} else 	if (r4menu_cursorPosition == 1) {
							sf2d_draw_rectangle(115, 77, 92, 91, SET_ALPHA(color_data->color, 255));
							sf2d_draw_texture_part(iconstex, 117, 79, 117, 79, 88, 87);
							static const char selectiontext[] = "Launch Slot-1 card";
							const int text_width = sftd_get_text_width(font, 14, selectiontext);
							sftd_draw_textf(font, (320-text_width)/2, 220, RGBA8(255, 255, 255, 255), 14, selectiontext);
						} else 	if (r4menu_cursorPosition == 2) {
							sf2d_draw_rectangle(217, 77, 92, 91, SET_ALPHA(color_data->color, 255));
							sf2d_draw_texture_part(iconstex, 219, 79, 219, 79, 88, 87);
							static const char selectiontext[] = "Start GBARunner2";
							const int text_width = sftd_get_text_width(font, 14, selectiontext);
							sftd_draw_textf(font, (320-text_width)/2, 220, RGBA8(255, 255, 255, 255), 14, selectiontext);
						}
						DrawDate(2, 220, RGBA8(255, 255, 255, 255), 14);
						sftd_draw_text(font, 276, 220, RGBA8(255, 255, 255, 255), 14, RetTime(true).c_str());
					} else {
						sf2d_draw_texture(bottomtex, 320/2 - bottomtex->width/2, 240/2 - bottomtex->height/2);
						if (!bannertextloaded) {
							char path[256];
							bnriconnum = cursorPosition;
							if (settings.twl.forwarder) {
								const char *tempfile = fcfiles.at(bnriconnum).c_str();
								snprintf(path, sizeof(path), "sdmc:/_nds/twloader/bnricons/flashcard/%s.bin", tempfile);
							} else {
								const char *tempfile = files.at(bnriconnum).c_str();
								snprintf(path, sizeof(path), "sdmc:/_nds/twloader/bnricons/%s.bin", tempfile);
							}
							if (access(path, F_OK) != -1) {
								LoadBNRIcon_R4Theme(path);
							} else {
								LoadBNRIcon_R4Theme(NULL);
							}
						}
						sf2d_draw_rectangle(80, 31, 192, 42, RGBA8(255, 255, 255, 255));
						sf2d_draw_texture(dboxtex_iconbox, 47, 31);
						sf2d_draw_texture_part(bnricontex[0], 52, 36, bnriconframenum*32, 0, 32, 32);
						
						if (!bannertextloaded) {
							char path[256];
							if (settings.twl.forwarder) {
								// if(matching_files.size() == 0){
									if (fcfiles.size() != 0) {
										romsel_filename = fcfiles.at(cursorPosition).c_str();
										romsel_filename_w = utf8_to_wstring(romsel_filename);
									} else {
										romsel_filename = " ";
										romsel_filename_w = utf8_to_wstring(romsel_filename);
									}
								/* }else{
									if (matching_files.size() != 0) {
										romsel_filename = matching_files.at(storedcursorPosition).c_str();
										romsel_filename_w = utf8_to_wstring(romsel_filename);
									} else {
										romsel_filename = " ";
										romsel_filename_w = utf8_to_wstring(romsel_filename);
									}
								} */
									snprintf(path, sizeof(path), "%s/%s.bin", fcbnriconfolder, romsel_filename);
								} else {
								// if(matching_files.size() == 0){
									if (files.size() != 0) {
										romsel_filename = files.at(cursorPosition).c_str();
										romsel_filename_w = utf8_to_wstring(romsel_filename);
									} else {
										romsel_filename = " ";
										romsel_filename_w = utf8_to_wstring(romsel_filename);
									}
									snprintf(path, sizeof(path), "%s/%s.bin", bnriconfolder, romsel_filename);
								/* }else {
									if (matching_files.size() != 0) {
										romsel_filename = matching_files.at(storedcursorPosition).c_str();
										romsel_filename_w = utf8_to_wstring(romsel_filename);
									} else {
										romsel_filename = " ";
										romsel_filename_w = utf8_to_wstring(romsel_filename);
									}
									snprintf(path, sizeof(path), "%s/%s.bin", bnriconfolder, romsel_filename); */
								}

							if (access(path, F_OK) == -1) {
								// Banner file is not available.
								strcpy(path, "romfs:/notextbanner");
							}
							FILE *f_bnr = fopen(path, "rb");
							if (f_bnr) {
								romsel_gameline = grabText(f_bnr, language);
								fclose(f_bnr);
							} else {
								// Unable to open the banner file.
								romsel_gameline.clear();
								romsel_gameline.push_back(latin1_to_wstring("ERROR:"));
								romsel_gameline.push_back(latin1_to_wstring("Unable to open the cached banner."));
							}
							bannertextloaded = true;
						}
						if (cursorPosition >= 0) {
							int y = 32, dy = 12;
							// Print the banner text, center-aligned.
							const size_t banner_lines = std::min(3U, romsel_gameline.size());
							for (size_t i = 0; i < banner_lines; i++, y += dy) {
								const int text_width = sftd_get_wtext_width(font_b, 12, romsel_gameline[i].c_str());
								sftd_draw_wtext(font_b, 84+(192-text_width)/2, y, RGBA8(0, 0, 0, 255), 12, romsel_gameline[i].c_str());
							}
						}
					}
				} else {
					if (settings.ui.custombot == 1)
						sf2d_draw_texture(bottomtex, 320/2 - bottomtex->width/2, 240/2 - bottomtex->height/2);
					else
						sf2d_draw_texture_blend(bottomtex, 320/2 - bottomtex->width/2, 240/2 - bottomtex->height/2, menucolor);

					if (fadealpha == 0) {
						sf2d_draw_texture(bubbletex, 0, 0);
						// if (dspfirmfound) { sfx_menuselect->play(); }
						bool drawBannerText = true;
						if (cursorPosition == -2) {
							const wchar_t *curn2text = TR(STR_SETTINGS_TEXT);
							const int text_width = sftd_get_wtext_width(font_b, 18, curn2text);
							sftd_draw_wtextf(font_b, (320-text_width)/2, 38, RGBA8(0, 0, 0, 255), 18, curn2text);
							drawBannerText = false;
						} else if (cursorPosition == -1) {
							if (settings.twl.forwarder) {
								static const char add_games_text[] = "Add Games";
								const int text_width = sftd_get_text_width(font_b, 18, add_games_text);
								sftd_draw_text(font_b, (320-text_width)/2, 38, RGBA8(0, 0, 0, 255), 18, add_games_text);
								drawBannerText = false;
							} else {
								// Get the text from the Slot 1 cartridge.
								if (!bannertextloaded) {
									romsel_gameline = gamecardGetText();
									const char *productCode = gamecardGetProductCode();
									if (!romsel_gameline.empty() && productCode) {
										// Display the product code and revision.
										// FIXME: Figure out a way to get the revision for CTR cards.
										char buf[48];
										if (gamecardGetType() != CARD_TYPE_CTR) {
											snprintf(buf, sizeof(buf), "Slot-1: %s, Rev.%02u", productCode, gamecardGetRevision());
										} else {
											snprintf(buf, sizeof(buf), "Slot-1: %s", productCode);
										}
										romsel_filename_w = latin1_to_wstring(buf);
									} else {
										romsel_filename_w.clear();
									}
								}

								if (romsel_gameline.empty()) {
									const wchar_t *msg;
									if (gamecardIsInserted()) {
										// Game card is inserted, but unrecognized.
										// It may be a blocked flashcard.
										msg = TR(STR_UNKOWN);
									} else {
										// No game card is inserted.
										msg = TR(STR_NO_CARTRIDGE);
									}
									const int text_width = sftd_get_wtext_width(font_b, 18, msg);
									sftd_draw_wtext(font_b, (320-text_width)/2, 38, RGBA8(0, 0, 0, 255), 18, msg);
									drawBannerText = false;
								}
							}
						} else {
							if (!bannertextloaded) {
								char path[256];
								if (settings.twl.forwarder) {
									if(matching_files.size() == 0){
										if (fcfiles.size() != 0) {
											romsel_filename = fcfiles.at(storedcursorPosition).c_str();
											romsel_filename_w = utf8_to_wstring(romsel_filename);
										} else {
											romsel_filename = " ";
											romsel_filename_w = utf8_to_wstring(romsel_filename);
										}
									}else{
										if (matching_files.size() != 0) {
											romsel_filename = matching_files.at(storedcursorPosition).c_str();
											romsel_filename_w = utf8_to_wstring(romsel_filename);
										} else {
											romsel_filename = " ";
											romsel_filename_w = utf8_to_wstring(romsel_filename);
										}
									}
									snprintf(path, sizeof(path), "%s/%s.bin", fcbnriconfolder, romsel_filename);
								} else {
									if(matching_files.size() == 0){
										if (files.size() != 0) {
											romsel_filename = files.at(storedcursorPosition).c_str();
											romsel_filename_w = utf8_to_wstring(romsel_filename);
										} else {
											romsel_filename = " ";
											romsel_filename_w = utf8_to_wstring(romsel_filename);
										}
										snprintf(path, sizeof(path), "%s/%s.bin", bnriconfolder, romsel_filename);
									}else {
										if (matching_files.size() != 0) {
											romsel_filename = matching_files.at(storedcursorPosition).c_str();
											romsel_filename_w = utf8_to_wstring(romsel_filename);
										} else {
											romsel_filename = " ";
											romsel_filename_w = utf8_to_wstring(romsel_filename);
										}
										snprintf(path, sizeof(path), "%s/%s.bin", bnriconfolder, romsel_filename);
									}									
								}

								if (access(path, F_OK) == -1) {
									// Banner file is not available.
									strcpy(path, "romfs:/notextbanner");
								}
								bnriconnum = cursorPosition;
								FILE *f_bnr = fopen(path, "rb");
								if (f_bnr) {
									romsel_gameline = grabText(f_bnr, language);
									fclose(f_bnr);
								} else {
									// Unable to open the banner file.
									romsel_gameline.clear();
									romsel_gameline.push_back(latin1_to_wstring("ERROR:"));
									romsel_gameline.push_back(latin1_to_wstring("Unable to open the cached banner."));
								}
								bannertextloaded = true;
							}
						}

						if (drawBannerText) {
							int y, dy;
							//top dialog = 100px tall
							if (settings.ui.filename) {
								sftd_draw_wtext(font, 10, 8, RGBA8(127, 127, 127, 255), 12, romsel_filename_w.c_str());
								y = (100-(19*romsel_gameline.size()))/2 + 4;
								//y = 24; dy = 19;
								dy = 19;
							} else {
								y = (100-(22*romsel_gameline.size()))/2;
								//y = 16; dy = 22;
								dy = 22;
							}

							// Print the banner text, center-aligned.
							const size_t banner_lines = std::min(3U, romsel_gameline.size());
							for (size_t i = 0; i < banner_lines; i++, y += dy) {
								const int text_width = sftd_get_wtext_width(font_b, 16, romsel_gameline[i].c_str());
								sftd_draw_wtext(font_b, (320-text_width)/2, y, RGBA8(0, 0, 0, 255), 16, romsel_gameline[i].c_str());
							}

							if (cursorPosition >= 0 && settings.ui.counter) {
								char romsel_counter1[16];
								snprintf(romsel_counter1, sizeof(romsel_counter1), "%d", storedcursorPosition+1);
								const char *p_romsel_counter;
								if (settings.twl.forwarder) {
									p_romsel_counter = romsel_counter2fc;
								} else {
									p_romsel_counter = romsel_counter2sd;
								}
								if (file_count < 100) {
									sftd_draw_textf(font, 8, 96, RGBA8(0, 0, 0, 255), 12, romsel_counter1);
									sftd_draw_textf(font, 27, 96, RGBA8(0, 0, 0, 255), 12, "/");
									sftd_draw_textf(font, 32, 96, RGBA8(0, 0, 0, 255), 12, p_romsel_counter);
								} else {
									sftd_draw_textf(font, 8, 96, RGBA8(0, 0, 0, 255), 12, romsel_counter1);
									sftd_draw_textf(font, 35, 96, RGBA8(0, 0, 0, 255), 12, "/");
									sftd_draw_textf(font, 40, 96, RGBA8(0, 0, 0, 255), 12, p_romsel_counter);
								}
							}
						}
					} else {
						sf2d_draw_texture(bottomlogotex, 320/2 - bottomlogotex->width/2, 40);
					}

					if(!is3DSX) {
						const wchar_t *home_text = TR(STR_RETURN_TO_HOME_MENU);
						const int home_width = sftd_get_wtext_width(font, 13, home_text) + 16;
						const int home_x = (320-home_width)/2;
						sf2d_draw_texture(homeicontex, home_x, 220); // Draw HOME icon
						sftd_draw_wtext(font, home_x+16, 221, RGBA8(0, 0, 0, 255), 13, home_text);
					}

					if (pagenum == 0) {
						if (settings.ui.iconsize) {
							sf2d_draw_texture_scale(bracetex, -74+titleboxXmovepos*1.25, 108, 1.25, 1.25);
							sf2d_draw_texture_scale(settingsicontex, -40+setsboxXpos+titleboxXmovepos*1.25, 111, 1.25, 1.25);
						} else {
							sf2d_draw_texture(bracetex, -32+titleboxXmovepos, 116);
							sf2d_draw_texture(settingsicontex, setsboxXpos+titleboxXmovepos, 119);
						}

						if (!settings.twl.forwarder) {
							// Poll for Slot-1 changes.
							bool forcePoll = false;
							bool doSlot1Update = false;
							if (gamecardIsInserted() && gamecardGetType() == CARD_TYPE_UNKNOWN) {
								// Card is inserted, but we don't know its type.
								// Force an update.
								forcePoll = true;
							}
							bool s1chg = gamecardPoll(forcePoll);
							if (s1chg) {
								// Update Slot-1 if:
								// - forcePoll is false
								// - forcePoll is true, and card is no longer unknown.
								doSlot1Update = (!forcePoll || gamecardGetType() != CARD_TYPE_UNKNOWN);
							}
							if (doSlot1Update) {
								// Slot-1 card has changed.
								if (cursorPosition == -1) {
									// Reload the banner text.
									bannertextloaded = false;
								}
							}

							if (settings.ui.iconsize)
								sf2d_draw_texture_scale(carttex(), -24+cartXpos+titleboxXmovepos*1.25, 111, 1.25, 1.25);
							else
								sf2d_draw_texture(carttex(), cartXpos+titleboxXmovepos, 120);
							sf2d_texture *cardicontex = gamecardGetIcon();
							if (!cardicontex)
								cardicontex = iconnulltex;
							if (settings.ui.iconsize)
								sf2d_draw_texture_part_scale(cardicontex, -4+cartXpos+titleboxXmovepos*1.25, 127, bnriconframenum*32, 0, 32, 32, 1.25, 1.25);
							else
								sf2d_draw_texture_part(cardicontex, 16+cartXpos+titleboxXmovepos, 133, bnriconframenum*32, 0, 32, 32);
						} else {
							// Get flash cart games.
							if (settings.ui.iconsize)
								sf2d_draw_texture_scale(getfcgameboxtex, -24+cartXpos+titleboxXmovepos*1.25, 111, 1.25, 1.25);
							else
								sf2d_draw_texture(getfcgameboxtex, cartXpos+titleboxXmovepos, 119);
						}
					} else {
						sf2d_draw_texture(bracetex, 32+cartXpos+titleboxXmovepos, 116);
					}

					if (settings.ui.iconsize) {
						titleboxXpos = 120;
						ndsiconXpos = 140;
					} else {
						titleboxXpos = 128;
						ndsiconXpos = 144;
					}
					//filenameYpos = 0;
					for (filenum = pagenum*20; filenum < pagemax; filenum++) {
						if (settings.ui.iconsize) {
							sf2d_draw_texture_scale(boxfulltex, titleboxXpos+titleboxXmovepos*1.25, 112, 1.25, 1.25);
							titleboxXpos += 80;

							bnriconnum = filenum;
							ChangeBNRIconNo();
							sf2d_draw_texture_part_scale(bnricontexnum, ndsiconXpos+titleboxXmovepos*1.25, 127, bnriconframenum*32, 0, 32, 32, 1.25, 1.25);
							ndsiconXpos += 80;
						} else {
							sf2d_draw_texture(boxfulltex, titleboxXpos+titleboxXmovepos, 120);
							titleboxXpos += 64;

							bnriconnum = filenum;
							ChangeBNRIconNo();
							sf2d_draw_texture_part(bnricontexnum, ndsiconXpos+titleboxXmovepos, 133, bnriconframenum*32, 0, 32, 32);
							ndsiconXpos += 64;
						}
					}

					if (settings.ui.iconsize) {
						sf2d_draw_texture_scale(bracetex, 15+ndsiconXpos+titleboxXmovepos*1.25, 108, -1.25, 1.25);
					} else {
						sf2d_draw_texture_scale(bracetex, 15+ndsiconXpos+titleboxXmovepos, 116, -1, 1);
					}
					if (!applaunchprep) {
						if (titleboxXmovetimer == 0) {
							startbordermovepos = 0;
							startborderscalesize = 1.0;
						}
						if (!settings.twl.forwarder && cursorPosition == -1 && !gamecardIsInserted()) {
							// Slot-1 selected, but no cartridge is present.
							// Don't print "START" and the cursor border.
						} else {
							if (!is3DSX || cursorPosition == -2) {
								// Print "START" and the cursor border.
								if (settings.ui.iconsize) {
									sf2d_draw_texture_scale(startbordertex, 120+startbordermovepos, 108+startbordermovepos, startborderscalesize+0.25, startborderscalesize+0.25);
									const wchar_t *start_text = TR(STR_START);
									const int start_width = sftd_get_wtext_width(font_b, 16, start_text);
									sftd_draw_wtext(font_b, (320-start_width)/2, 183, RGBA8(255, 255, 255, 255), 16, start_text);
								} else {
									sf2d_draw_texture_scale(startbordertex, 128+startbordermovepos, 116+startbordermovepos, startborderscalesize, startborderscalesize);
									const wchar_t *start_text = TR(STR_START);
									const int start_width = sftd_get_wtext_width(font_b, 12, start_text);
									sftd_draw_wtext(font_b, (320-start_width)/2, 177, RGBA8(255, 255, 255, 255), 12, start_text);
								}
							}
						}
					} else {
						if (settings.ui.custombot)
							sf2d_draw_texture_part(bottomtex, 128, 116, 128, 116, 64, 80);
						else
							sf2d_draw_texture_part_blend(bottomtex, 128, 116, 128, 116, 64, 80, SET_ALPHA(menucolor, 255));  // Cover selected game/app
						if (cursorPosition == -2) {
							sf2d_draw_texture(settingsicontex, 128, titleboxYmovepos-1); // Draw settings box that moves up
						} else if (cursorPosition == -1) {
							if (settings.twl.forwarder)
								sf2d_draw_texture(getfcgameboxtex, 128, titleboxYmovepos-1);
							else {
								// Draw selected Slot-1 game that moves up
								sf2d_draw_texture(carttex(), 128, titleboxYmovepos);
								sf2d_texture *cardicontex = gamecardGetIcon();
								if (!cardicontex)
									cardicontex = iconnulltex;
								sf2d_draw_texture(cardicontex, 144, ndsiconYmovepos);
							}
						} else {
							sf2d_draw_texture(boxfulltex, 128, titleboxYmovepos); // Draw selected game/app that moves up
							if (!applaunchicon) {
								bnriconnum = cursorPosition;
								ChangeBNRIconNo();
								bnricontexlaunch = bnricontexnum;
								applaunchicon = true;
							}
							sf2d_draw_texture_part(bnricontexlaunch, 144, ndsiconYmovepos, bnriconframenum*32, 0, 32, 32);
						}
						sf2d_draw_texture_rotate(dotcircletex, 160, 152, rad);  // Dots moving in circles
					}
					if (menudbox_Ypos != -240) {
						// Draw the menu dialog box.
						drawMenuDialogBox();
					}
				}
			} else if (screenmode == SCREEN_MODE_SETTINGS) {
				if (colortexloaded_bot) {
					sf2d_free_texture(dotcircletex);
					dotcircletex = NULL;
					sf2d_free_texture(startbordertex);
					startbordertex = NULL;
					sf2d_free_texture(bottomtex);
					bottomtex = NULL;
					colortexloaded_bot = false;
				}
				settingsDrawBottomScreen();
			}
		if (settings.ui.theme == 0)
			sf2d_draw_rectangle(0, 0, 320, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect

		sf2d_end_frame();
		// }
		
		sf2d_swapbuffers();


		/* if (titleboxXmovetimer == 0) {
			updatebotscreen = false;
		} */
		if (screenmode == SCREEN_MODE_ROM_SELECT) {
			/* if (filenum == 0) {	// If no ROMs are found
				romselect_layout = 1;
				updatebotscreen = true;
			} */
			if (settings.ui.theme == 2) {
				if (menu_ctrlset == CTRL_SET_MENU) {
					if (hDown & KEY_A) {
						switch (woodmenu_cursorPosition) {
							case 0:
							default:
								menu_ctrlset = CTRL_SET_GAMESEL;
								if (settings.twl.forwarder) {
									settings.twl.forwarder = false;
									cursorPosition = 0;
									pagenum = 0;
									boxarttexloaded = false; // Reload boxarts
									bnricontexloaded = false; // Reload banner icons
									bannertextloaded = false;
								}
								break;
							case 1:
								menu_ctrlset = CTRL_SET_GAMESEL;
								if (!settings.twl.forwarder) {
									settings.twl.forwarder = true;
									cursorPosition = 0;
									pagenum = 0;
									boxarttexloaded = false; // Reload boxarts
									bnricontexloaded = false; // Reload banner icons
									bannertextloaded = false;
								}
								break;
							case 2:
								if (!is3DSX) {
									settings.twl.launchslot1 = true;
									settings.twl.forwarder = false;
									if (logEnabled)	LogFM("Main", "Switching to NTR/TWL-mode");
									applaunchon = true;
								}
								break;
							case 3:
								if (!is3DSX) {
									gbarunnervalue = 1;
									settings.ui.romfolder = "_nds";									
									rom = "GBARunner2.nds";
									if (settings.twl.forwarder)
										settings.twl.launchslot1 = true;
									else
										settings.twl.launchslot1 = false;
									if (logEnabled)	LogFM("Main", "Switching to NTR/TWL-mode");
									applaunchon = true;
								}
								break;
							case 4:
								sf2d_set_3D(1);
								screenmode = SCREEN_MODE_SETTINGS;
								settingsResetSubScreenMode();
								break;
						}
						wood_ndsiconscaletimer = 0;
					} else if(hDown & KEY_DOWN){
						woodmenu_cursorPosition++;
						if (woodmenu_cursorPosition > 4) {
							woodmenu_cursorPosition = 4;
						}
						wood_ndsiconscaletimer = 0;
					} else if(hDown & KEY_UP){
						woodmenu_cursorPosition--;
						if (woodmenu_cursorPosition < 0) {
							woodmenu_cursorPosition = 0;
						}
						wood_ndsiconscaletimer = 0;
					}
				} else {
					if(cursorPosition < 0) {
						filenameYmovepos = 0;
						titleboxXmovepos -= 64;
						boxartXmovepos -= 18*8;
						cursorPosition = 0;
						// updatebotscreen = true;
					}
					if(hDown & KEY_A){
						if (!is3DSX) {
							if (settings.twl.forwarder) {
								settings.twl.launchslot1 = true;
								// if(matching_files.size() == 0){
									rom = fcfiles.at(cursorPosition).c_str();
								// }else {
							// 	rom = matching_files.at(cursorPosition).c_str();
							// }
							} else {
								settings.twl.launchslot1 = false;
								// if(matching_files.size() == 0){
									rom = files.at(cursorPosition).c_str();
								// }else {
								// 	rom = matching_files.at(cursorPosition).c_str();
								// }
								sav = ReplaceAll(rom, ".nds", ".sav");
							}
							if (logEnabled)	LogFM("Main", "Switching to NTR/TWL-mode");
							applaunchon = true;
						}
						wood_ndsiconscaletimer = 0;
					} else if(hDown & KEY_L){
						if ((size_t)pagenum != 0 && file_count <= (size_t)0-pagenum*20) {
							pagenum--;
							bannertextloaded = false;
							cursorPosition = 0+pagenum*20;
							bnricontexloaded = false;
							boxarttexloaded = false;
						}
					} else if(hDown & KEY_R){
						if (file_count > (size_t)pagemax) {
							pagenum++;
							bannertextloaded = false;
							cursorPosition = 0+pagenum*20;
							bnricontexloaded = false;
							boxarttexloaded = false;
						}
					} else if(hDown & KEY_DOWN){
						cursorPosition++;
						if (cursorPosition >= pagemax) {
							cursorPosition = 0+pagenum*20;
						}
						wood_downpressed = true;
						wood_ndsiconscaletimer = 0;
						bannertextloaded = false;
					} else if((hDown & KEY_UP) && (filenum > 1)){
						cursorPosition--;
						if (cursorPosition < 0+pagenum*20) {
							cursorPosition = pagemax-1;
						}
						wood_uppressed = true;
						wood_ndsiconscaletimer = 0;
						bannertextloaded = false;
					} else if (hDown & KEY_B) {
						menu_ctrlset = CTRL_SET_MENU;
						wood_ndsiconscaletimer = 0;
					}
					if (filenum > 4) {
						if (cursorPosition > 2+pagenum*20)
							filenameYmovepos = -cursorPosition+2+pagenum*20;
						else
							filenameYmovepos = 0;
					}
				}
			} else if (settings.ui.theme == 1) {
				if (menu_ctrlset == CTRL_SET_MENU) {
					if (hDown & KEY_A) {
						switch (r4menu_cursorPosition) {
							case 0:
							default:
								menu_ctrlset = CTRL_SET_GAMESEL;
								updatetopscreen = true;
								break;
							case 1:
								if (!is3DSX) {
									settings.twl.launchslot1 = true;
									settings.twl.forwarder = false;
									if (logEnabled)	LogFM("Main", "Switching to NTR/TWL-mode");
									applaunchon = true;
								}
								break;
							case 2:
								if (!is3DSX) {
									gbarunnervalue = 1;
									settings.ui.romfolder = "_nds";									
									rom = "GBARunner2.nds";
									if (settings.twl.forwarder)
										settings.twl.launchslot1 = true;
									else
										settings.twl.launchslot1 = false;
									if (logEnabled)	LogFM("Main", "Switching to NTR/TWL-mode");
									applaunchon = true;
								}
								break;
						}
					} else if (hDown & KEY_RIGHT) {
						r4menu_cursorPosition++;
						if (r4menu_cursorPosition > 2) {
							r4menu_cursorPosition = 2;
						}
					} else if (hDown & KEY_LEFT) {
						r4menu_cursorPosition--;
						if (r4menu_cursorPosition < 0) {
							r4menu_cursorPosition = 0;
						}
					} else if (hDown & KEY_SELECT) {
						sf2d_set_3D(1);
						screenmode = SCREEN_MODE_SETTINGS;
						settingsResetSubScreenMode();
						updatetopscreen = true;
					}
				} else {
					if(cursorPosition < 0) {
						filenameYmovepos = 0;
						titleboxXmovepos -= 64;
						boxartXmovepos -= 18*8;
						cursorPosition = 0;
						updatetopscreen = true;
					}
					if(hDown & KEY_L) {
						settings.twl.forwarder = !settings.twl.forwarder;
						cursorPosition = 0;
						pagenum = 0;
						boxarttexloaded = false; // Reload boxarts
						bnricontexloaded = false; // Reload banner icons
						bannertextloaded = false;
						updatetopscreen = true;
					}
					if(hDown & KEY_A){
						if (!is3DSX) {
							if (settings.twl.forwarder) {
								settings.twl.launchslot1 = true;
								// if(matching_files.size() == 0){
									rom = fcfiles.at(cursorPosition).c_str();
								// }else {
							// 	rom = matching_files.at(cursorPosition).c_str();
							// }
							} else {
								settings.twl.launchslot1 = false;
								// if(matching_files.size() == 0){
									rom = files.at(cursorPosition).c_str();
								// }else {
								// 	rom = matching_files.at(cursorPosition).c_str();
								// }
								sav = ReplaceAll(rom, ".nds", ".sav");
							}
							if (logEnabled)	LogFM("Main", "Switching to NTR/TWL-mode");
							applaunchon = true;
						}
					} else if(hDown & KEY_DOWN){
						cursorPosition++;
						if (cursorPosition == filenum) {
							filenameYmovepos = 0;
							cursorPosition = 0;
						}
						bannertextloaded = false;
						updatetopscreen = true;
					} else if((hDown & KEY_UP) && (filenum > 1)){
						if (cursorPosition == 0) {
							cursorPosition = filenum;
						}
						cursorPosition--;
						bannertextloaded = false;
						updatetopscreen = true;
					} else if ((hDown & KEY_LEFT) && (filenum > 10)) {
						cursorPosition -= 10;
						if (cursorPosition < 0) {
							cursorPosition = 0;
						}
						bannertextloaded = false;			
						updatetopscreen = true;
					} else if ((hDown & KEY_RIGHT) && (filenum > 10)) {
						cursorPosition += 10;
						if (cursorPosition >= filenum) {
							cursorPosition = filenum-1;
						}
						bannertextloaded = false;			
						updatetopscreen = true;
					} else if (hDown & KEY_B) {
						menu_ctrlset = CTRL_SET_MENU;
						updatetopscreen = true;
					}
					if (filenum > 15) {
						/* if (cursorPosition > filenum-8) {}
						else */ if (cursorPosition > 7)
							filenameYmovepos = -cursorPosition+7;
						else
							filenameYmovepos = 0;
					}
				}
			} else {
				startbordermovepos = 0;
				startborderscalesize = 1.0;
				if (!noromsfound && file_count == 0) {
					// No ROMs were found.
					cursorPosition = -1;
					storedcursorPosition = cursorPosition;
					titleboxXmovepos = +64;
					boxartXmovepos = 0;
					// updatebotscreen = true;
					noromsfound = true;
				}
				if(titleboxXmovetimer == 0 && menu_ctrlset == CTRL_SET_GAMESEL) {
					if(hDown & KEY_R) {
						menuaction_nextpage = true;
					} else if(hDown & KEY_L) {
						menuaction_prevpage = true;
					}
					if(hDown & KEY_Y) {
						if (dspfirmfound) {
							sfx_switch->stop();	// Prevent freezing
							sfx_switch->play();
						}
						settings.ui.iconsize = !settings.ui.iconsize;
					}
					hidTouchRead(&touch);
					if(hDown & KEY_TOUCH){
						if (touch.px >= 128 && touch.px <= 192 && touch.py >= 112 && touch.py <= 192) {
							menuaction_launch = true;
						} else if (touch.px < 128 && touch.py >= 118 && touch.py <= 180  && menudbox_Ypos == -240) {
							//titleboxXmovepos -= 64;
							if (!titleboxXmoveright) {
								titleboxXmoveleft = true;
							}
							// updatebotscreen = true;
						} else if (touch.px > 192 && touch.py >= 118 && touch.py <= 180  && menudbox_Ypos == -240) {
							//titleboxXmovepos -= 64;
							if (!titleboxXmoveleft) {
								if (cursorPosition == -1) {
									if (filenum == 0) {
										if (!playwrongsounddone) {
											if (dspfirmfound) {
												sfx_wrong->stop();
												sfx_wrong->play();
											}
											playwrongsounddone = true;
										}
									} else {
										titleboxXmoveright = true;
									}
								} else {
									titleboxXmoveright = true;
								}
							}
							// updatebotscreen = true;
						}
					} else if(hDown & KEY_A){
						menuaction_launch = true;
					} else if(hHeld & KEY_RIGHT && menudbox_Ypos == -240){
						//titleboxXmovepos -= 64;
						if (!titleboxXmoveleft) {
							if (cursorPosition == -1) {
								if (filenum == 0) {
									if (!playwrongsounddone) {
										if (dspfirmfound) {
											sfx_wrong->stop();
											sfx_wrong->play();
										}
										playwrongsounddone = true;
									}
								} else {
									titleboxXmoveright = true;
								}
							} else {
								titleboxXmoveright = true;
							}
						}
						// updatebotscreen = true;
					} else if(hHeld & KEY_LEFT && menudbox_Ypos == -240){
						//titleboxXmovepos += 64;
						if (!titleboxXmoveright) {
							titleboxXmoveleft = true;
						}
						// updatebotscreen = true;
					} else if (hDown & KEY_START) {
						// Switch to the "Start" menu.
						menudboxmode = DBOX_MODE_OPTIONS;
						if (!showdialogbox_menu) {
							if (menudbox_Ypos == -240) {
								showdialogbox_menu = true;
								menu_ctrlset = CTRL_SET_DBOX;
								// Reset the cursor positions.
								startmenu_cursorPosition = 0;
								gamesettings_cursorPosition = 0;
							}
						}
					} else if (hDown & KEY_SELECT) {
						// Switch to per-game settings.
						menudboxmode = DBOX_MODE_SETTINGS;
						if (!showdialogbox_menu) {
							if (cursorPosition >= 0 && menudbox_Ypos == -240) {
								if (settings.twl.forwarder) {
									if(matching_files.size() == 0){
										rom = fcfiles.at(cursorPosition).c_str();
									} else {
										rom = matching_files.at(cursorPosition).c_str();
									}
								} else {
									if(matching_files.size() == 0){
										rom = files.at(cursorPosition).c_str();
									} else {
										rom = matching_files.at(cursorPosition).c_str();
									}
								}
								LoadPerGameSettings();
								showdialogbox_menu = true;
								menu_ctrlset = CTRL_SET_DBOX;
								// Reset the cursor positions.
								startmenu_cursorPosition = 0;
								gamesettings_cursorPosition = 0;
							}
						}
					}
					
					if (menuaction_nextpage) {
						// Don't run the action again 'til R is pressed again
						menuaction_nextpage = false;
						if (file_count > (size_t)pagemax) {
							pagenum++;
							bannertextloaded = false;
							cursorPosition = 0+pagenum*20;
							storedcursorPosition = cursorPosition;
							titleboxXmovepos = 0;
							boxartXmovepos = 0;
							// noromsfound = false;
							bnricontexloaded = false;
							boxarttexloaded = false;
							if (dspfirmfound) {
								sfx_switch->stop();	// Prevent freezing
								sfx_switch->play();
							}
							// updatebotscreen = true;
						}
					} else if (menuaction_prevpage) {
						// Don't run the action again 'til L is pressed again
						menuaction_prevpage = false;
						if ((size_t)pagenum != 0 && file_count <= (size_t)0-pagenum*20) {
							pagenum--;
							bannertextloaded = false;
							cursorPosition = 0+pagenum*20;
							storedcursorPosition = cursorPosition;
							titleboxXmovepos = 0;
							boxartXmovepos = 0;
							// noromsfound = false;
							bnricontexloaded = false;
							boxarttexloaded = false;
							if (dspfirmfound) {
								sfx_switch->stop();	// Prevent freezing
								sfx_switch->play();
							}
							// updatebotscreen = true;
						}
					} else if (menuaction_launch) { menuaction_launch = false;	// Don't run the action again 'til A is pressed again
						if (!is3DSX || cursorPosition == -2) {
							bool playlaunchsound = true;
							if (titleboxXmovetimer == 0) {
								if(cursorPosition == -2) {
									titleboxXmovetimer = 1;
									screenmodeswitch = true;
									applaunchprep = true;
								} else if(cursorPosition == -1) {
									if (!settings.twl.forwarder && !gamecardIsInserted()) {
										// Slot-1 is selected, but no
										// cartridge is present.
										if (!playwrongsounddone) {
											if (dspfirmfound) {
												sfx_wrong->stop();
												sfx_wrong->play();
											}
											playwrongsounddone = true;
										}
										playlaunchsound = false;
									} else {
										titleboxXmovetimer = 1;
										settings.twl.launchslot1 = true;
										if (settings.twl.forwarder) {
											keepsdvalue = true;
											rom = "_nds/twloader.nds";
										}
										applaunchprep = true;
									}
								} else {
									titleboxXmovetimer = 1;
									if (settings.twl.forwarder) {
										settings.twl.launchslot1 = true;
										if(matching_files.size() == 0){
											rom = fcfiles.at(cursorPosition).c_str();
										}else {
											rom = matching_files.at(cursorPosition).c_str();
										}
									} else {
										settings.twl.launchslot1 = false;
										if(matching_files.size() == 0){
											rom = files.at(cursorPosition).c_str();
										}else {
											rom = matching_files.at(cursorPosition).c_str();
										}
										sav = ReplaceAll(rom, ".nds", ".sav");
									}
									applaunchprep = true;
								}
							}
							if (playlaunchsound && dspfirmfound) {
								bgm_menu->stop();
								sfx_launch->play();
							}
						} else {
							if (!playwrongsounddone) {
								if (dspfirmfound) {
									sfx_wrong->stop();
									sfx_wrong->play();
								}
								playwrongsounddone = true;
							}
						}
					}
					
				} else if(menu_ctrlset == CTRL_SET_DBOX) {
					hidTouchRead(&touch);
					
					if (menudboxmode == DBOX_MODE_OPTIONS) {
						if (hDown & KEY_SELECT) {
							if (cursorPosition >= 0) {
								// Switch to game-specific settings.
								menudboxmode = DBOX_MODE_SETTINGS;
							}
						} else if (hDown & KEY_RIGHT) {
							if (startmenu_cursorPosition % 2 != 1 &&
							    startmenu_cursorPosition != 5)
							{
								// Move right.
								startmenu_cursorPosition++;
							}
						} else if (hDown & KEY_LEFT) {
							if (startmenu_cursorPosition % 2 != 0) {
								// Move left.
								startmenu_cursorPosition--;
							}
						} else if (hDown & KEY_DOWN) {
							if (startmenu_cursorPosition < 4) {
								startmenu_cursorPosition += 2;
							}
	
						} else if (hDown & KEY_UP) {
							if (startmenu_cursorPosition > 1) {
								startmenu_cursorPosition -= 2;
							}
						} else if(hDown & KEY_TOUCH){
							if (touch.px >= 23 && touch.px <= 155 && touch.py >= 31 && touch.py <= 65) { // Game location button
								startmenu_cursorPosition = 0;
								menudboxaction_switchloc = true;
							}else if (touch.px >= 161 && touch.px <= 293 && touch.py >= 31 && touch.py <= 65){ // Box art button
								startmenu_cursorPosition = 1;
								settings.romselect.toplayout = !settings.romselect.toplayout;
								if (dspfirmfound) {
									sfx_switch->stop();	// Prevent freezing
									sfx_switch->play();
								}
							}else if (touch.px >= 23 && touch.px <= 155 && touch.py >= 71 && touch.py <= 105){ // Start GBARunner2 button
								startmenu_cursorPosition = 2;
								if (!is3DSX) {
									gbarunnervalue = 1;
									settings.ui.romfolder = "_nds";
									rom = "GBARunner2.nds";
									if (settings.twl.forwarder) {
										settings.twl.launchslot1 = true;
									} else {
										settings.twl.launchslot1 = false;
									}
									fadeout = true;
									if (dspfirmfound) {
										bgm_menu->stop();
										sfx_launch->play();
									}
								} else {
									if (!playwrongsounddone) {
										if (dspfirmfound) {
											sfx_wrong->stop();
											sfx_wrong->play();
										}
										playwrongsounddone = true;
									}
								}
							}else if (touch.px >= 161 && touch.px <= 293 && touch.py >= 71 && touch.py <= 105){ // Top border button
								startmenu_cursorPosition = 3;
								settings.ui.topborder = !settings.ui.topborder;
							}else if (touch.px >= 23 && touch.px <= 155 && touch.py >= 111 && touch.py <= 145){ // Unset donor ROM button
								startmenu_cursorPosition = 4;
								bootstrapini.SetString(bootstrapini_ndsbootstrap, "ARM7_DONOR_PATH", "");
								bootstrapini.SaveIniFile("sdmc:/_nds/nds-bootstrap.ini");
								showdialogbox_menu = false;
								menudbox_movespeed = 1;
								menu_ctrlset = CTRL_SET_GAMESEL;
							}else if (touch.px >= 161 && touch.px <= 293 && touch.py >= 111 && touch.py <= 145){ // Search button
								startmenu_cursorPosition = 5; // Only this is making sometimes to not show the light texture								
								if(matching_files.size() != 0){
									matching_files.clear();
									snprintf(romsel_counter2sd, sizeof(romsel_counter2sd), "%zu", files.size());
									snprintf(romsel_counter2fc, sizeof(romsel_counter2fc), "%zu", fcfiles.size());
								}
								
								std::string gameName = keyboardInput(TR(STR_START_SEARCH_HINT));
								if (logEnabled)	LogFMA("Main.search","Text written", gameName.c_str());
								
								if(!settings.twl.forwarder){
									// SD CARD
									for (auto iter = files.cbegin(); iter != files.cend(); ++iter) {
										if (iter->size() < gameName.size()) {
											// Filename we're checking is shorter than the search term,
											// so it can't match.
											continue;
										}
										// Use C string comparison for case-insensitive checks.
										if (strcasestr(iter->c_str(), gameName.c_str()) != NULL) {
											// String matches.
											matching_files.push_back(*iter);
										}
									}
								}else{
									// FLASHCARD
									for (auto iter = fcfiles.cbegin(); iter != fcfiles.cend(); ++iter) {
										if (iter->size() < gameName.size()) {
											// Filename we're checking is shorter than the search term,
											// so it can't match.
											continue;
										}
										// Use C string comparison for case-insensitive checks.
										if (strcasestr(iter->c_str(), gameName.c_str()) != NULL) {
											// String matches.
											matching_files.push_back(*iter);
										}
									}
								}							

								if (matching_files.size() != 0) {
									/** Prepare some stuff to show correctly the filtered roms */
									pagenum = 0; // Go to page 0
									cursorPosition = 0; // Move the cursor to 0
									storedcursorPosition = cursorPosition; // Move the cursor to 0
									titleboxXmovepos = 0; // Move the cursor to 0
									boxartXmovepos = 0; // Move the cursor to 0
									snprintf(romsel_counter2sd, sizeof(romsel_counter2sd), "%zu", matching_files.size()); // Reload counter
									boxarttexloaded = false; // Reload boxarts
									bnricontexloaded = false; // Reload banner icons
								}
								sf2d_draw_texture(dboxtex_button, 161, menudbox_Ypos + 111); // Light the button to print it always
								showdialogbox_menu = false;
								menudbox_movespeed = 1;
								menu_ctrlset = CTRL_SET_GAMESEL;			
							}else if (touch.px >= 233 && touch.px <= 299 && touch.py >= (menudbox_Ypos + 191) && touch.py <= (menudbox_Ypos + 217)){ // Back button
								showdialogbox_menu = false;
								menudbox_movespeed = 1;
								menu_ctrlset = CTRL_SET_GAMESEL;
							}
						} else if (hDown & KEY_A) {
							switch (startmenu_cursorPosition) {
								case 0:
								default:
									menudboxaction_switchloc = true;
									break;
								case 1:
									settings.romselect.toplayout = !settings.romselect.toplayout;
									if (dspfirmfound) {
										sfx_switch->stop();	// Prevent freezing
										sfx_switch->play();
									}
									break;
								case 2:
									if (!is3DSX) {
										gbarunnervalue = 1;
										settings.ui.romfolder = "_nds";
										rom = "GBARunner2.nds";
										if (settings.twl.forwarder) {
											settings.twl.launchslot1 = true;
										} else {
											settings.twl.launchslot1 = false;
										}
										fadeout = true;
										if (dspfirmfound) {
											bgm_menu->stop();
											sfx_launch->play();
										}
									} else {
										if (!playwrongsounddone) {
											if (dspfirmfound) {
												sfx_wrong->stop();
												sfx_wrong->play();
											}
											playwrongsounddone = true;
										}
									}
									break;
								case 3:
									settings.ui.topborder = !settings.ui.topborder;
									break;
								case 4:
									// Unset donor ROM path
									bootstrapini.SetString(bootstrapini_ndsbootstrap, "ARM7_DONOR_PATH", "");
									bootstrapini.SaveIniFile("sdmc:/_nds/nds-bootstrap.ini");
									showdialogbox_menu = false;
									menudbox_movespeed = 1;
									menu_ctrlset = CTRL_SET_GAMESEL;
									break;
								case 5: {
									// Search
									if(matching_files.size() != 0){
										matching_files.clear();
										snprintf(romsel_counter2sd, sizeof(romsel_counter2sd), "%zu", files.size());
									}
									
									std::string gameName = keyboardInput(TR(STR_START_SEARCH_HINT));
									if (logEnabled)	LogFMA("Main.search","Text written", gameName.c_str());
									
									for (auto iter = files.cbegin(); iter != files.cend(); ++iter) {
										if (iter->size() < gameName.size()) {
											// Filename we're checking is shorter than the search term,
											// so it can't match.
											continue;
										}

										// Check if the game name contains the search term.
										// (Case-insensitive search.)
										if (strcasestr(iter->c_str(), gameName.c_str()) != NULL) {
											// String matches.
											matching_files.push_back(*iter);
										}
									}

									if (matching_files.size() != 0) {
										/** Prepare some stuff to show correctly the filtered roms */
										pagenum = 0; // Go to page 0
										cursorPosition = 0; // Move the cursor to 0
										storedcursorPosition = cursorPosition; // Move the cursor to 0
										titleboxXmovepos = 0; // Move the cursor to 0
										boxartXmovepos = 0; // Move the cursor to 0
										snprintf(romsel_counter2sd, sizeof(romsel_counter2sd), "%zu", matching_files.size()); // Reload counter
										boxarttexloaded = false; // Reload boxarts
										bnricontexloaded = false; // Reload banner icons
									}
									showdialogbox_menu = false;
									menudbox_movespeed = 1;
									menu_ctrlset = CTRL_SET_GAMESEL;
									break;
								}
							}
						} else if (hDown & (KEY_B | KEY_START)) {
							showdialogbox_menu = false;
							menudbox_movespeed = 1;
							menu_ctrlset = CTRL_SET_GAMESEL;
						}
					} else if (menudboxmode == DBOX_MODE_SETTINGS) {
						hidTouchRead(&touch);
						
						if (hDown & KEY_START) {
							// Switch to the "Start" menu.
							if (settings.twl.forwarder) {
								if(matching_files.size() == 0){
									rom = fcfiles.at(cursorPosition).c_str();
								} else {
									rom = matching_files.at(cursorPosition).c_str();
								}
							} else {
								if (matching_files.size() == 0) {
									rom = files.at(cursorPosition).c_str();
								}else{
									rom = matching_files.at(cursorPosition).c_str();
								}
								
							}
							SavePerGameSettings();
							menudboxmode = DBOX_MODE_OPTIONS;
						} else if (hDown & KEY_RIGHT) {
							if (gamesettings_cursorPosition == 0) {
								gamesettings_cursorPosition = 1;
							} else if (gamesettings_cursorPosition == 2) {
								gamesettings_cursorPosition = 3;
							}
						} else if (hDown & KEY_LEFT) {
							if (gamesettings_cursorPosition == 1) {
								gamesettings_cursorPosition = 0;
							} else if (gamesettings_cursorPosition == 3) {
								gamesettings_cursorPosition = 2;
							}
						} else if (hDown & KEY_DOWN) {
							if (gamesettings_cursorPosition == 0) {
								gamesettings_cursorPosition = 2;
							} else if (gamesettings_cursorPosition == 1) {
								gamesettings_cursorPosition = 3;
							} else if (gamesettings_cursorPosition == 2) {
								gamesettings_cursorPosition = 4;
							}
						} else if (hDown & KEY_UP) {
							if (gamesettings_cursorPosition == 2) {
								gamesettings_cursorPosition = 0;
							} else if (gamesettings_cursorPosition == 3) {
								gamesettings_cursorPosition = 1;
							} else if (gamesettings_cursorPosition == 4) {
								gamesettings_cursorPosition = 2;
							}
						} else if(hDown & KEY_TOUCH){
							if (touch.px >= 23 && touch.px <= 155 && touch.py >= 89 && touch.py <= 123) { // ARM9 CPU Speed
								gamesettings_cursorPosition = 0;
								settings.pergame.cpuspeed++;
								if(settings.pergame.cpuspeed == 2)
									settings.pergame.cpuspeed = -1;
								if (dspfirmfound) {
									sfx_select->stop();	// Prevent freezing
									sfx_select->play();
								}
							}else if (touch.px >= 161 && touch.px <= 293 && touch.py >= 89 && touch.py <= 123){ // VRAM boost
								gamesettings_cursorPosition = 1;
								settings.pergame.extvram++;
								if(settings.pergame.extvram == 2)
									settings.pergame.extvram = -1;
								if (dspfirmfound) {
									sfx_select->stop();	// Prevent freezing
									sfx_select->play();
								}
							}else if (touch.px >= 23 && touch.px <= 155 && touch.py >= 129 && touch.py <= 163){ // Lock ARM9 SCFG_EXT
								gamesettings_cursorPosition = 2;
								settings.pergame.lockarm9scfgext++;
								if(settings.pergame.lockarm9scfgext == 2)
									settings.pergame.lockarm9scfgext = -1;
								if (dspfirmfound) {
									sfx_select->stop();	// Prevent freezing
									sfx_select->play();
								}
							}else if (touch.px >= 161 && touch.px <= 293 && touch.py >= 129 && touch.py <= 163){ // Set as donor ROM
								gamesettings_cursorPosition = 3;
								if (settings.twl.forwarder) {
									if(matching_files.size() == 0){
										rom = fcfiles.at(cursorPosition).c_str();
									} else {
										rom = matching_files.at(cursorPosition).c_str();
									}
								} else {
									if(matching_files.size() == 0){
										rom = files.at(cursorPosition).c_str();
									}else{
										rom = matching_files.at(cursorPosition).c_str();
									}
									bootstrapini.SetString(bootstrapini_ndsbootstrap, "ARM7_DONOR_PATH", fat+settings.ui.romfolder+slashchar+rom);
									bootstrapini.SaveIniFile("sdmc:/_nds/nds-bootstrap.ini");
								}
								showdialogbox_menu = false;
								menudbox_movespeed = 1;
								menu_ctrlset = CTRL_SET_GAMESEL;
							}else if (touch.px >= 23 && touch.px <= 155 && touch.py >= 169 && touch.py <= 203){ // Set LED Color
								gamesettings_cursorPosition = 4;								

								RGB[0] = keyboardInputInt("Red color: max is 255");
								RGB[1] = keyboardInputInt("Green color: max is 255");
								RGB[2] = keyboardInputInt("Blue color: max is 255");
								
								settings.pergame.red = RGB[0];
								settings.pergame.green = RGB[1];
								settings.pergame.blue = RGB[2];
								
							}else if (touch.px >= 233 && touch.px <= 299 && touch.py >= (menudbox_Ypos + 191) && touch.py <= (menudbox_Ypos + 217)){ // Back button
								if (settings.twl.forwarder) {
									if(matching_files.size() == 0){
										rom = fcfiles.at(cursorPosition).c_str();
									} else {
										rom = matching_files.at(cursorPosition).c_str();
									}
								} else {
									if(matching_files.size() == 0){
										rom = files.at(cursorPosition).c_str();
									}else{
										rom = matching_files.at(cursorPosition).c_str();
									}
								}
								SavePerGameSettings();
								showdialogbox_menu = false;
								menudbox_movespeed = 1;
								menu_ctrlset = CTRL_SET_GAMESEL;
							}
						}else if (hDown & KEY_A) {
							switch (gamesettings_cursorPosition) {
								case 0:
								default:
									settings.pergame.cpuspeed++;
									if(settings.pergame.cpuspeed == 2)
										settings.pergame.cpuspeed = -1;
									if (dspfirmfound) {
										sfx_select->stop();	// Prevent freezing
										sfx_select->play();
									}
									break;
								case 1:
									settings.pergame.extvram++;
									if(settings.pergame.extvram == 2)
										settings.pergame.extvram = -1;
									if (dspfirmfound) {
										sfx_select->stop();	// Prevent freezing
										sfx_select->play();
									}
									break;
								case 2:
									settings.pergame.lockarm9scfgext++;
									if(settings.pergame.lockarm9scfgext == 2)
										settings.pergame.lockarm9scfgext = -1;
									if (dspfirmfound) {
										sfx_select->stop();	// Prevent freezing
										sfx_select->play();
									}
									break;
								case 3:
									if (settings.twl.forwarder) {
										if(matching_files.size() == 0){
											rom = fcfiles.at(cursorPosition).c_str();
										}else{
											rom = matching_files.at(cursorPosition).c_str();
										}
									} else {
										if(matching_files.size() == 0){
											rom = files.at(cursorPosition).c_str();
										}else{
											rom = matching_files.at(cursorPosition).c_str();
										}
										bootstrapini.SetString(bootstrapini_ndsbootstrap, "ARM7_DONOR_PATH", fat+settings.ui.romfolder+slashchar+rom);
										bootstrapini.SaveIniFile("sdmc:/_nds/nds-bootstrap.ini");
									}
									showdialogbox_menu = false;
									menudbox_movespeed = 1;
									menu_ctrlset = CTRL_SET_GAMESEL;
									break;
								case 4:
									RGB[0] = keyboardInputInt("Red color: max is 255");
									RGB[1] = keyboardInputInt("Green color: max is 255");
									RGB[2] = keyboardInputInt("Blue color: max is 255");
									
									settings.pergame.red = RGB[0];
									settings.pergame.green = RGB[1];
									settings.pergame.blue = RGB[2];
									break;
							}
						} else if (hDown & (KEY_B | KEY_SELECT)) {
							if (settings.twl.forwarder) {
								if(matching_files.size() == 0){
									rom = fcfiles.at(cursorPosition).c_str();
								}else{
									rom = matching_files.at(cursorPosition).c_str();
								}
							} else {
								if(matching_files.size() == 0){
									rom = files.at(cursorPosition).c_str();
								}else{
									rom = matching_files.at(cursorPosition).c_str();
								}
							}
							SavePerGameSettings();
							showdialogbox_menu = false;
							menudbox_movespeed = 1;
							menu_ctrlset = CTRL_SET_GAMESEL;
						}
					}
					
					if (menudboxaction_switchloc) { menudboxaction_switchloc = false;
						if (dspfirmfound) {
							sfx_switch->stop();	// Prevent freezing
							sfx_switch->play();
						}
						matching_files.clear();
						pagenum = 0;
						bannertextloaded = false;
						cursorPosition = 0;
						storedcursorPosition = cursorPosition;
						titleboxXmovepos = 0;
						boxartXmovepos = 0;
						noromsfound = false;
						settings.twl.forwarder = !settings.twl.forwarder;
						bnricontexloaded = false;
						boxarttexloaded = false;
						if (dspfirmfound) {
							sfx_switch->stop();	// Prevent freezing
							sfx_switch->play();
						}
						// updatebotscreen = true;
						
						if (!settings.twl.forwarder) {
							// Poll for Slot-1 changes.
							gamecardPoll(true);

							// Load the Slot-1 boxart.
							// NOTE: This is needed here; otherwise, the
							// boxart won't be visible for a few frames
							// when switching from flashcard to SD.
							loadSlot1BoxArt();
						}
					}
					
				}
			}
		} else if (screenmode == SCREEN_MODE_SETTINGS) {
			settingsMoveCursor(hDown);
		}
		
		if (applaunchon) {
			if (settings.twl.forwarder && gbarunnervalue == 0) {
				CIniFile setfcrompathini(sdmc+settings.ui.fcromfolder+slashchar+rom);
				std::string rominini = setfcrompathini.GetString(fcrompathini_flashcardrom, fcrompathini_rompath, "");
				// TODO: Enum values for flash card type.
				if (keepsdvalue) {
					switch (settings.twl.flashcard) {
						case 0:
						case 1:
						case 3:
						default: {
							CIniFile fcrompathini("sdmc:/_nds/YSMenu.ini");
							fcrompathini.SetString("YSMENU", "AUTO_BOOT", slashchar+rom);
							fcrompathini.SaveIniFile("sdmc:/_nds/YSMenu.ini");
							break;
						}

						case 2:
						case 4:
						case 5: {
							CIniFile fcrompathini("sdmc:/_nds/lastsave.ini");
							fcrompathini.SetString("Save Info", "lastLoaded", woodfat+rom);
							fcrompathini.SaveIniFile("sdmc:/_nds/lastsave.ini");
							break;
						}

						case 6: {
							CIniFile fcrompathini("sdmc:/_nds/dstwoautoboot.ini");
							fcrompathini.SetString("Dir Info", "fullName", dstwofat+rom);
							fcrompathini.SaveIniFile("sdmc:/_nds/dstwoautoboot.ini");
							break;
						}
					}
				} else {
					CIniFile setfcrompathini(sdmc+settings.ui.fcromfolder+rom);
					switch (settings.twl.flashcard) {
						case 0:
						case 1:
						case 3:
						default: {
							CIniFile fcrompathini("sdmc:/_nds/YSMenu.ini");
							fcrompathini.SetString("YSMENU", "AUTO_BOOT", slashchar+rominini);
							fcrompathini.SetString("YSMENU", "DEFAULT_DMA", "true");
							fcrompathini.SetString("YSMENU", "DEFAULT_RESET", "false");
							fcrompathini.SaveIniFile("sdmc:/_nds/YSMenu.ini");
							break;
						}

						case 2:
						case 4:
						case 5: {
							CIniFile fcrompathini("sdmc:/_nds/lastsave.ini");
							fcrompathini.SetString("Save Info", "lastLoaded", woodfat+rominini);
							fcrompathini.SaveIniFile("sdmc:/_nds/lastsave.ini");
							break;
						}

						case 6: {
							CIniFile fcrompathini("sdmc:/_nds/dstwoautoboot.ini");
							fcrompathini.SetString("Dir Info", "fullName", dstwofat+rominini);
							fcrompathini.SaveIniFile("sdmc:/_nds/dstwoautoboot.ini");
							break;
						}
					}
				}
			} else if (settings.twl.forwarder && gbarunnervalue == 1) {
				if (settings.twl.forwarder) {
					switch (settings.twl.flashcard) {
						case 0:
						case 1:
						case 3:
						default: {
							CIniFile fcrompathini("sdmc:/_nds/YSMenu.ini");
							fcrompathini.SetString("YSMENU", "AUTO_BOOT", slashchar+rom);
							fcrompathini.SetString("YSMENU", "DEFAULT_DMA", "true");
							fcrompathini.SetString("YSMENU", "DEFAULT_RESET", "false");
							fcrompathini.SaveIniFile("sdmc:/_nds/YSMenu.ini");
							break;
						}

						case 2:
						case 4:
						case 5: {
							CIniFile fcrompathini("sdmc:/_nds/lastsave.ini");
							fcrompathini.SetString("Save Info", "lastLoaded", woodfat+rom);
							fcrompathini.SaveIniFile("sdmc:/_nds/lastsave.ini");
							break;
						}

						case 6: {
							CIniFile fcrompathini("sdmc:/_nds/dstwoautoboot.ini");
							fcrompathini.SetString("Dir Info", "fullName", dstwofat+rom);
							fcrompathini.SaveIniFile("sdmc:/_nds/dstwoautoboot.ini");
							break;
						}
					}
				}
			}

			// Save settings.
			saveOnExit = false;
			SaveSettings();
			SetPerGameSettings();
			SaveBootstrapConfig();

			// Prepare for the app launch.
			u64 tid = TWLNAND_TID;
			FS_MediaType mediaType = MEDIATYPE_NAND;
			bool switchToTwl = true;	
			
			if (!settings.twl.forwarder && settings.twl.launchslot1) {
				// CTR cards should be launched directly.
				// TODO: TWL cards should also be launched directly,
				// but APT_PrepareToDoApplicationJump() ends up
				// rebooting to the home screen for NTR/TWL...
				if (gamecardGetType() == CARD_TYPE_CTR) {
					u64 ctr_tid = gamecardGetTitleID();
					if (ctr_tid != 0) {
						tid = ctr_tid;
						mediaType = MEDIATYPE_GAME_CARD;
						switchToTwl = false;
					}
				}
			}

			if (switchToTwl) {
				// Switching to TWL.
				// Activate the rainbow LED and shut off the screen.
				// TODO: Allow rainbow LED even in CTR? (It'll stay
				// cycling as long as no event causes it to change.)
				if (cursorPosition >= 0 && gbarunnervalue == 0) {
					if (settings.twl.forwarder) {
						if(matching_files.size() == 0){
							rom = fcfiles.at(cursorPosition).c_str();
						}else{
							rom = matching_files.at(cursorPosition).c_str();
						}
					} else {
						if(matching_files.size() == 0){
							rom = files.at(cursorPosition).c_str();
						} else {
							rom = matching_files.at(cursorPosition).c_str();
						}
					}
					LoadPerGameSettings();
				}
				if((RGB[0] < 0) && (RGB[1] < 0) && (RGB[2] < 0)){
					// If RGB in pergame is less than 0, use standard rainbowled patern
					if (settings.twl.rainbowled) {
						RainbowLED();
					}
				}else{
					// Use pergame led
					PergameLed();
				}
				screenoff();
			}

			// Buffers for APT_DoApplicationJump().
			u8 param[0x300];
			u8 hmac[0x20];
			// Clear both buffers
			memset(param, 0, sizeof(param));
			memset(hmac, 0, sizeof(hmac));

			APT_PrepareToDoApplicationJump(0, tid, mediaType);
			// Tell APT to trigger the app launch and set the status of this app to exit
			APT_DoApplicationJump(param, sizeof(param), hmac);
			break;
		}
	//}	// run
	}	// aptMainLoop

	// Unregister the "returned from HOME Menu" handler.
	aptUnhook(&rfhm_cookie);
	if (logEnabled) LogFM("Main", "returned from HOME Menu handler unregistered");

	if (saveOnExit) {
		// Save settings.
		SaveSettings();
		SetPerGameSettings();
		SaveBootstrapConfig();
	}
	if (logEnabled) LogFM("Main.saveOnExit", "Settings are saved");

	// Unload settings screen textures.
	settingsUnloadTextures();
	if (logEnabled) LogFM("Main.settingsUnloadTextures", "Settings textures unloaded");

	if (colortexloaded) sf2d_free_texture(topbgtex);
	if (logEnabled) LogFM("Main", "topbgtex freed");
	sf2d_free_texture(toptex);
	for (int i = 0; i < 6; i++) {
		sf2d_free_texture(voltex[i]);
	}

	sf2d_free_texture(shoulderLtex);
	sf2d_free_texture(shoulderRtex);
	if (logEnabled) LogFM("Main", "Shoulder textures freed");

	if (settings.ui.theme == 0) {
		sf2d_free_texture(batterychrgtex);
		for (int i = 0; i < 6; i++) {
			sf2d_free_texture(batterytex[i]);
		}
		if (logEnabled) LogFM("Main", "DSi Menu battery textures freed");
	}
	if (settings.ui.theme == 1) sf2d_free_texture(iconstex);
	if (colortexloaded) sf2d_free_texture(bottomtex);
	sf2d_free_texture(sdicontex);
	sf2d_free_texture(flashcardicontex);
	sf2d_free_texture(gbaicontex);
	sf2d_free_texture(smallsettingsicontex);
	sf2d_free_texture(iconnulltex);
	sf2d_free_texture(homeicontex);
	sf2d_free_texture(bottomlogotex);
	sf2d_free_texture(bubbletex);
	sf2d_free_texture(settingsicontex);
	sf2d_free_texture(getfcgameboxtex);
	sf2d_free_texture(cartnulltex);
	sf2d_free_texture(cartntrtex);
	sf2d_free_texture(carttwltex);
	if (settings.ui.theme == 0) gamecardClearCache();
	sf2d_free_texture(boxfulltex);
	if (colortexloaded && settings.ui.theme == 0) {
		sf2d_free_texture(dotcircletex);
		sf2d_free_texture(startbordertex);
	}

	// Free the arrays.
	if (bnricontexloaded) {
		for (int i = 0; i < 20; i++) {
			sf2d_free_texture(bnricontex[i]);
			free(boxartpath[i]);
		}
	}
	if (boxarttexloaded) {
		sf2d_free_texture(slot1boxarttex);
		for (int i = 0; i < 6; i++) {
			sf2d_free_texture(boxarttex[i]);
		}
	}

	// Remaining common textures.
	sf2d_free_texture(dboxtex_iconbox);
	sf2d_free_texture(dboxtex_button);
	sf2d_free_texture(dboxtex_buttonback);
	sf2d_free_texture(dialogboxtex);
	sf2d_free_texture(settingslogotex);

	// Clear the translations cache.
	langClear();

	// Shut down audio.
	delete bgm_menu;
	//delete bgm_settings;
	delete sfx_launch;
	delete sfx_select;
	delete sfx_stop;
	delete sfx_switch;
	delete sfx_wrong;
	delete sfx_back;
	if (dspfirmfound) {
		ndspExit();
	}

	sftd_free_font(font);
	sftd_free_font(font_b);
	if (logEnabled) LogFM("Main", "Fonts freed");
	sftd_fini();
	sf2d_fini();

	acExit();
	hidExit();
	srvExit();
	romfsExit();
	sdmcExit();
	ptmuxExit();
	ptmuExit();
	amExit();
	cfguExit();
	aptExit();
	if (logEnabled) LogFM("Main", "All services are closed and returned to HOME Menu");
	return 0;
}
