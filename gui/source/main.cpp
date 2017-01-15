#include "download.h"

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

#include "sound.h"
#include "inifile.h"
#include "date.h"
#include "log.h"

#define CONFIG_3D_SLIDERSTATE (*(float *)0x1FF81080)


touchPosition touch;
u32 kUp;
u32 kDown;
u32 kHeld;

CIniFile settingsini( "sdmc:/_nds/twloader/settings.ini" );	
CIniFile bootstrapini( "sdmc:/_nds/nds-bootstrap.ini" );
#include "ndsheaderbanner.h"

int equals;
u8 language;

sftd_font *font;
sftd_font *font_b;
sf2d_texture *dialogboxtex; // Dialog box
sf2d_texture *settingstex; // Bottom of settings screen

enum ScreenMode {
	SCREEN_MODE_ROM_SELECT = 0,	// ROM Select
	SCREEN_MODE_SETTINGS = 1,	// Settings
};
ScreenMode screenmode = SCREEN_MODE_ROM_SELECT;
	
int settings_subscreenmode = 0;
// 0: Frontend settings
// 1: NTR/TWL-mode settings

static sf2d_texture *bnricontexnum = NULL;
static sf2d_texture *bnricontexlaunch = NULL;
static sf2d_texture *boxarttexnum = NULL;

// Banners and boxart. (formerly bannerandboxart.h)
// TODO: Some of this still needs reworking to fix
// memory leaks, but switching to arrays is a start.
static FILE* ndsFile[20] = { };
static char* bnriconpath[20] = { };
// bnricontex[]: 0-9 == regular; 10-19 == .nds icons only
static sf2d_texture *bnricontex[20] = { };
static char* boxartpath[20] = { };
static sf2d_texture *boxarttex[6] = { };

int bnriconnum = 0;
int bnriconframenum = 0;
int boxartnum = 0;
int pagenum = 0;
const char* temptext;
const char* bottomloc;
const char* musicpath = "romfs:/null.wav";

// Color settings.
// Use SET_ALPHA() to replace the alpha value.
#define SET_ALPHA(color, alpha) ((((alpha) & 0xFF) << 24) | ((color) & 0x00FFFFFF))
typedef struct _ColorData {
	const char *topbgloc;
	const char *dotcircleloc;
	const char *startborderloc;
	u32 color;
} ColorData;
static const ColorData *color_data = NULL;
static u32 menucolor;


static const char fcrompathini_flashcardrom[] = "FLASHCARD-ROM";
static const char fcrompathini_rompath[] = "NDS_PATH";
static const char fcrompathini_tid[] = "TID";
static const char fcrompathini_bnriconaniseq[] = "BNR_ICONANISEQ";
	

// Settings .ini file
static const char settingsini_frontend[] = "FRONTEND";
//static const char settingsini_frontend_twlappinstalled[] = "TWLAPP_INSTALLED";
static const char settingsini_frontend_name[] = "NAME";
static const char settingsini_frontend_color[] = "COLOR";
static const char settingsini_frontend_menucolor[] = "MENU_COLOR";
static const char settingsini_frontend_filename[] = "SHOW_FILENAME";
static const char settingsini_frontend_locswitch[] = "GAMELOC_SWITCH";
static const char settingsini_frontend_topborder[] = "TOP_BORDER";
static const char settingsini_frontend_toplayout[] = "TOP_LAYOUT";
static const char settingsini_frontend_custombot[] = "CUSTOM_BOTTOM";
static const char settingsini_frontend_counter[] = "COUNTER";
static const char settingsini_frontend_autoupdate[] = "AUTOUPDATE";
static const char settingsini_frontend_autodl[] = "AUTODOWNLOAD";
	
static const char settingsini_twlmode[] = "TWL-MODE";
static const char settingsini_twl_rainbowled[] = "RAINBOW_LED";
static const char settingsini_twl_clock[] = "TWL_CLOCK";
static const char settingsini_twl_vram[] = "TWL_VRAM";
static const char settingsini_twl_bootani[] = "BOOT_ANIMATION";
static const char settingsini_twl_hsmsg[] = "HEALTH&SAFETY_MSG";
static const char settingsini_twl_launchslot1[] = "LAUNCH_SLOT1";	// 0: Don't boot Slot-1, 1: Boot Slot-1, 2: Forward a ROM path to a Slot-1 flashcard.
static const char settingsini_twl_resetslot1[] = "RESET_SLOT1";
static const char settingsini_twl_keepsd[] = "SLOT1_KEEPSD";
static const char settingsini_twl_debug[] = "DEBUG";
static const char settingsini_twl_gbarunner[] = "GBARUNNER";
static const char settingsini_twl_forwarder[] = "FORWARDER";
static const char settingsini_twl_flashcard[] = "FLASHCARD";
// End



// Bootstrap .ini file
static const char bootstrapini_ndsbootstrap[] = "NDS-BOOTSTRAP";
static const char bootstrapini_ndspath[] = "NDS_PATH";
static const char bootstrapini_savpath[] = "SAV_PATH";
static const char bootstrapini_boostcpu[] = "BOOST_CPU";
static const char bootstrapini_debug[] = "DEBUG";
static const char bootstrapini_lockarm9scfgext[] = "LOCK_ARM9_SCFG_EXT";
// End

// Run
bool run = true;
// End

const char* text_returntohomemenu()
{
	static const char *const languages[] =
	{
		": Return to HOME Menu",		// Japanese
		": Return to HOME Menu",		// English
		": Retour au menu HOME",		// French
		": Zuruck zum HOME-Menu",		// German
		": Ritorna al Menu Home",		// Italian
		": Volver al menu HOME",		// Spanish
		": Return to HOME Menu",		// Simplified Chinese
		": Return to HOME Menu",		// Korean
		": Keer terug naar HOME-menu",	// Dutch
		": Retornar ao Menu HOME",		// Portugese
		": Return to HOME Menu",		// Russian
		": Return to HOME Menu"			// Traditional Chinese
	};
	
	if (language < 11) {
		return languages[language];
	} else {
		return languages[1];
	}
}

bool showdialogbox = false;

const char* Lshouldertext;
const char* Rshouldertext;

const char* noromtext1;
const char* noromtext2;

// Settings text
const char* settings_xbuttontext = "X: Update bootstrap (Official Release)";
const char* settings_ybuttontext = "Y: Update bootstrap (Unofficial build)";
const char* settings_startbuttontext = "START: Update TWLoader";

typedef struct {
	char text[13];
} sVerfile;

char settings_vertext[13];
char settings_latestvertext[13];

const char* settingstext_bot;

const char* settings_colortext = "Color";
const char* settings_menucolortext = "Menu color";
const char* settings_filenametext = "Show filename";
const char* settings_locswitchtext = "Game location switcher";
const char* settings_topbordertext = "Top border";
const char* settings_countertext = "Game counter";
const char* settings_custombottext = "Custom bottom image";
const char* settings_autoupdatetext = "Auto-update bootstrap";
const char* settings_autodltext = "Auto-update to latest TWLoader";

const char* settings_colorvaluetext;
const char* settings_menucolorvaluetext;
const char* settings_filenamevaluetext;
const char* settings_locswitchvaluetext;
const char* settings_topbordervaluetext;
const char* settings_countervaluetext;
const char* settings_custombotvaluetext;
const char* settings_autoupdatevaluetext;
const char* settings_autodlvaluetext;

const char* settings_lrpicktext = "Left/Right: Pick";
const char* settings_absavereturn = "A/B: Save and Return";
// End

int settings_colorvalue;
int settings_menucolorvalue;
int settings_filenamevalue;
int settings_locswitchvalue;
int settings_topbordervalue;
int settings_countervalue;
int settings_custombotvalue;
int settings_autoupdatevalue;
int settings_autodlvalue;

int romselect_toplayout;
//	0: Show box art
//	1: Hide box art

static bool applaunchprep = false;

static int fadealpha = 255;
static bool fadein = true;
static bool fadeout = false;

std::string name;

const char* romsel_filename;
vector<string> romsel_gameline;	// from banner (TODO: wstring?)

static const char* rom = "";		// Selected ROM image.
// TODO: Potential memory leaks for sav...
static char* sav = nullptr;		// Associated save file.
static const char* flashcardrom = "";

std::string sdmc = "sdmc:/";
std::string fat = "fat:/";
std::string slashchar = "/";
std::string woodfat = "fat0:/";
std::string dstwofat = "fat1:/";
std::string romfolder = "roms/nds/";
std::string flashcardfolder = "roms/flashcard/nds/";
const char* bnriconfolder = "sdmc:/_nds/twloader/bnricons/";
const char* fcbnriconfolder = "sdmc:/_nds/twloader/bnricons/flashcard/";
const char* boxartfolder = "sdmc:/_nds/twloader/boxart/";
const char* fcboxartfolder = "sdmc:/_nds/twloader/boxart/flashcard/";

// NTR/TWL-mode Settings text
const char* twlsettings_flashcardtext = "Flashcard(s) select";
const char* twlsettings_rainbowledtext = "Rainbow LED";
const char* twlsettings_cpuspeedtext = "ARM9 CPU Speed";
const char* twlsettings_extvramtext = "VRAM boost";
const char* twlsettings_bootscreentext = "DS/DSi Boot Screen";
const char* twlsettings_healthsafetytext = "Health and Safety message";
const char* twlsettings_resetslot1text = "Reset Slot-1";
const char* twlsettings_consoletext = "Console output";
const char* twlsettings_lockarm9scfgexttext = "Lock ARM9 SCFG_EXT";
	
const char* twlsettings_rainbowledvaluetext;
const char* twlsettings_cpuspeedvaluetext;
const char* twlsettings_extvramvaluetext;
const char* twlsettings_bootscreenvaluetext;
const char* twlsettings_healthsafetyvaluetext;
const char* twlsettings_resetslot1valuetext;
const char* twlsettings_consolevaluetext;
const char* twlsettings_lockarm9scfgextvaluetext;
// End
	
int twlsettings_rainbowledvalue;
int twlsettings_cpuspeedvalue;
int twlsettings_extvramvalue;
int twlsettings_forwardervalue;
int twlsettings_flashcardvalue;
/* Flashcard value
	0: DSTT/R4i Gold/R4i-SDHC/R4 SDHC Dual-Core/R4 SDHC Upgrade/SC DSONE
	1: R4DS (Original Non-SDHC version)/ M3 Simply
	2: R4iDSN/R4i Gold RTS
	3: Acekard 2(i)/M3DS Real/R4i-SDHC v1.4.x
	4: Acekard RPG
	5: Ace 3DS+/Gateway Blue Card/R4iTT
	6: SuperCard DSTWO
*/
int twlsettings_bootscreenvalue;
int twlsettings_healthsafetyvalue;
int twlsettings_launchslot1value;
int twlsettings_resetslot1value;
int twlsettings_consolevalue;
int twlsettings_lockarm9scfgextvalue;
int keepsdvalue = 0;
int gbarunnervalue = 0;


std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

/**
 * Replace a substring in a C string.
 * @param str Original string.
 * @param o_s Substring to search for.
 * @param n_s New substring.
 * @return Newly allocated string with the replaced substring. (Must be free()'d!)
 */
char *strrep(const char *str, const char *o_s, const char *n_s) 
{
	char *newstr = NULL;
	char *c = NULL;

	// Look for the substring.
	if ((c = strstr(str, o_s)) == NULL) {
		// Substring not found.
		return strdup(str);
	}

	newstr = (char*)malloc(strlen(str) - strlen(o_s) + strlen(n_s) + 1);
	if (!newstr) {
		printf("ERROR: unable to allocate memory\n");
		return NULL;
	}

	strncpy(newstr, str, c-str);  
	sprintf(newstr+(c-str), "%s%s", n_s, c+strlen(o_s));
	return newstr;
}


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


Handle ptmsysmHandle = 0;

Result ptmsysmInit()
{
    return srvGetServiceHandle(&ptmsysmHandle, "ptm:sysm");
}

Result ptmsysmExit()
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
void DialogBoxAppear(const char *text) {
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
void DialogBoxDisappear(const char *text) {
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
		sftd_draw_textf(font, 12, 16+i, RGBA8(0, 0, 0, 255), 12, dialog_text.c_str());
		sf2d_end_frame();
		sf2d_swapbuffers();
	}
	showdialogbox = false;
}

/**
 * Create a save file.
 * @param filename Filename.
 */
static void CreateGameSave(const char *filename) {
	DialogBoxAppear("Creating save file...");
	static const int BUFFER_SIZE = 4096;
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));

	FILE *pFile = fopen(filename, "wb");
	if (pFile) {
		for (int i = 524288; i > 0; i -= BUFFER_SIZE) {
			fwrite(buffer, 1, sizeof(buffer), pFile);
		}
		fclose(pFile);
	}

	DialogBoxDisappear("Done!");
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
	LogFM("Main.RainbowLED", "Rainbow LED is on");
	return 0;
}

static void LoadColor(void) {
	static const ColorData colors[] = {
		{
			"romfs:/graphics/topbg/0-gray.png",
			"romfs:/graphics/dotcircle/0-gray.png",
			"romfs:/graphics/start_border/0-gray.png",
			(u32)RGBA8(99, 127, 127, 255)
		},
		{
			"romfs:/graphics/topbg/1-brown.png",
			"romfs:/graphics/dotcircle/1-brown.png",
			"romfs:/graphics/start_border/1-brown.png",
			(u32)RGBA8(139, 99, 0, 255)
		},
		{
			"romfs:/graphics/topbg/2-red.png",
			"romfs:/graphics/dotcircle/2-red.png",
			"romfs:/graphics/start_border/2-red.png",
			(u32)RGBA8(255, 0, 0, 255)
		},
		{
			"romfs:/graphics/topbg/3-pink.png",
			"romfs:/graphics/dotcircle/3-pink.png",
			"romfs:/graphics/start_border/3-pink.png",
			(u32)RGBA8(255, 127, 127, 255)
		},
		{
			"romfs:/graphics/topbg/4-orange.png",
			"romfs:/graphics/dotcircle/4-orange.png",
			"romfs:/graphics/start_border/4-orange.png",
			(u32)RGBA8(223, 63, 0, 255)
		},
		{
			"romfs:/graphics/topbg/5-yellow.png",
			"romfs:/graphics/dotcircle/5-yellow.png",
			"romfs:/graphics/start_border/5-yellow.png",
			(u32)RGBA8(215, 215, 0, 255)
		},
		{
			"romfs:/graphics/topbg/6-yellowgreen.png",
			"romfs:/graphics/dotcircle/6-yellowgreen.png",
			"romfs:/graphics/start_border/6-yellowgreen.png",
			(u32)RGBA8(215, 255, 0, 255)
		},
		{
			"romfs:/graphics/topbg/7-green1.png",
			"romfs:/graphics/dotcircle/7-green1.png",
			"romfs:/graphics/start_border/7-green1.png",
			(u32)RGBA8(0, 255, 0, 255)
		},
		{
			"romfs:/graphics/topbg/8-green2.png",
			"romfs:/graphics/dotcircle/8-green2.png",
			"romfs:/graphics/start_border/8-green2.png",
			(u32)RGBA8(63, 255, 63, 255)
		},
		{
			"romfs:/graphics/topbg/9-lightgreen.png",
			"romfs:/graphics/dotcircle/9-lightgreen.png",
			"romfs:/graphics/start_border/9-lightgreen.png",
			(u32)RGBA8(31, 231, 31, 255)
		},
		{
			"romfs:/graphics/topbg/10-skyblue.png",
			"romfs:/graphics/dotcircle/10-skyblue.png",
			"romfs:/graphics/start_border/10-skyblue.png",
			(u32)RGBA8(0, 63, 255, 255)
		},
		{
			"romfs:/graphics/topbg/11-lightblue.png",
			"romfs:/graphics/dotcircle/11-lightblue.png",
			"romfs:/graphics/start_border/11-lightblue.png",
			(u32)RGBA8(63, 63, 255, 255)
		},
		{
			"romfs:/graphics/topbg/12-blue.png",
			"romfs:/graphics/dotcircle/12-blue.png",
			"romfs:/graphics/start_border/12-blue.png",
			(u32)RGBA8(0, 0, 255, 255)
		},
		{
			"romfs:/graphics/topbg/13-violet.png",
			"romfs:/graphics/dotcircle/13-violet.png",
			"romfs:/graphics/start_border/13-violet.png",
			(u32)RGBA8(127, 0, 255, 255)
		},
		{
			"romfs:/graphics/topbg/14-purple.png",
			"romfs:/graphics/dotcircle/14-purple.png",
			"romfs:/graphics/start_border/14-purple.png",
			(u32)RGBA8(255, 0, 255, 255)
		},
		{
			"romfs:/graphics/topbg/15-fuchsia.png",
			"romfs:/graphics/dotcircle/15-fuchsia.png",
			"romfs:/graphics/start_border/15-fuchsia.png",
			(u32)RGBA8(255, 0, 127, 255)
		},
		{
			"romfs:/graphics/topbg/16-red&blue.png",
			"romfs:/graphics/dotcircle/16-red&blue.png",
			"romfs:/graphics/start_border/16-red&blue.png",
			(u32)RGBA8(255, 0, 255, 255)
		},
		{
			"romfs:/graphics/topbg/17-green&yellow.png",
			"romfs:/graphics/dotcircle/17-green&yellow.png",
			"romfs:/graphics/start_border/17-green&yellow.png",
			(u32)RGBA8(215, 215, 0, 255)
		},
		{
			"romfs:/graphics/topbg/18-christmas.png",
			"romfs:/graphics/dotcircle/18-christmas.png",
			"romfs:/graphics/start_border/18-christmas.png",
			(u32)RGBA8(255, 255, 0, 255)
		},
	};

	if (settings_colorvalue < 0 || settings_colorvalue > 18)
		settings_colorvalue = 0;
	color_data = &colors[settings_colorvalue];
	LogFM("LoadColor()", "Colors load successfully");
}

static void LoadMenuColor(void) {
	static const u32 menu_colors[] = {
		(u32)RGBA8(255, 255, 255, 255),		// White
		(u32)RGBA8(63, 63, 63, 195),		// Black
		(u32)RGBA8(139, 99, 0, 195),		// Brown
		(u32)RGBA8(255, 0, 0, 195),		// Red
		(u32)RGBA8(255, 163, 163, 195),		// Pink
		(u32)RGBA8(255, 127, 0, 223),		// Orange
		(u32)RGBA8(255, 255, 0, 223),		// Yellow
		(u32)RGBA8(215, 255, 0, 223),		// Yellow-Green
		(u32)RGBA8(0, 255, 0, 223),		// Green 1
		(u32)RGBA8(95, 223, 95, 193),		// Green 2
		(u32)RGBA8(127, 231, 127, 223),		// Light Green
		(u32)RGBA8(63, 127, 255, 223),		// Sky Blue
		(u32)RGBA8(127, 127, 255, 223),		// Light Blue
		(u32)RGBA8(0, 0, 255, 195),		// Blue
		(u32)RGBA8(127, 0, 255, 195),		// Violet
		(u32)RGBA8(255, 0, 255, 195),		// Purple
		(u32)RGBA8(255, 63, 127, 195),		// Fuchsia
	};

	if (settings_menucolorvalue < 0 || settings_menucolorvalue > 16)
		settings_menucolorvalue = 0;
	menucolor = menu_colors[settings_menucolorvalue];
	LogFM("LoadMenuColor()", "Menu color load successfully");
}

void LoadBottomImage() {
	bottomloc = "romfs:/graphics/bottom.png";
	
	if (settings_custombotvalue == 1) {
		if( access( "sdmc:/_nds/twloader/bottom.png", F_OK ) != -1 ) {
			bottomloc = "sdmc:/_nds/twloader/bottom.png";
			LogFM("LoadBottomImage()", "Using custom bottom image. Method load successfully");
		} else {
			bottomloc = "romfs:/graphics/bottom.png";
			LogFM("LoadBottomImage()", "Using default bottom image. Method load successfully");
		}
	}
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

static void OpenBNRIcon(void) {
	// Get the bnriconnum relative to the current page.
	const int idx = bnriconnum - (pagenum * 20);
	if (idx >= 0 && idx < 20) {
		// Selected banner icon is on the current page.
		if (ndsFile[idx]) {
			fclose(ndsFile[idx]);
		}
		ndsFile[idx] = fopen(bnriconpath[idx], "rb");
	}
}

/**
 * Store a banner icon path.
 * @param path Banner icon path. (will be strdup()'d)
 */
static void StoreBNRIconPath(const char *path) {
	// Get the bnriconnum relative to the current page.
	const int idx = bnriconnum - (pagenum * 20);
	if (idx >= 0 && idx < 20) {
		// Selected banner icon is on the current page.
		free(bnriconpath[idx]);
		bnriconpath[idx] = strdup(path);
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
	}
}

static void LoadBNRIcon(void) {
	// Get the bnriconnum relative to the current page.
	const int idx = bnriconnum - (pagenum * 20);
	if (idx >= 0 && idx < 20) {
		// Selected bnriconnum is on the current page.
		sf2d_free_texture(bnricontex[idx]);
		// LogFMA("Main.LoadBNRIcon", "Loading banner icon", bnriconpath[idx]);
		if (ndsFile[idx]) {
			bnricontex[idx] = grabIcon(ndsFile[idx]);
			fclose(ndsFile[idx]);
			ndsFile[idx] = NULL;
		} else {
			FILE *f_nobnr = fopen("romfs:/notextbanner", "rb");
			bnricontex[idx] = grabIcon(f_nobnr);
			fclose(f_nobnr);
		}
		// LogFMA("Main.LoadBNRIcon", "Banner icon loaded", bnriconpath[idx]);
	}
}

static void LoadBNRIconatLaunch(void) {
	// Get the bnriconnum relative to the current page.
	const int idx = bnriconnum - (pagenum * 20);
	if (idx >= 0 && idx < 20) {
		// Selected bnriconnum is on the current page.
		sf2d_free_texture(bnricontexlaunch);
		if (ndsFile[idx]) {
			bnricontexlaunch = grabIcon(ndsFile[idx]); // Banner icon
		} else {
			FILE *f_nobnr = fopen("romfs:/notextbanner", "rb");
			bnricontexlaunch = grabIcon(f_nobnr);
			fclose(f_nobnr);
		}
	}
}

static void LoadBoxArt(void) {
	// Get the boxartnum relative to the current page.
	const int idx = boxartnum - (pagenum * 20);
	if (idx >= 0 && idx < 20) {
		// Selected boxart is on the current page.
		// NOTE: Only 6 slots for boxart.
		sf2d_free_texture(boxarttex[idx % 6]);
		boxarttex[idx % 6] = sfil_load_PNG_file(boxartpath[idx], SF2D_PLACE_RAM); // Box art
	}
}

void LoadSettings() {
	name = settingsini.GetString(settingsini_frontend, settingsini_frontend_name, "");
	settings_colorvalue = settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0);
	settings_menucolorvalue = settingsini.GetInt(settingsini_frontend, settingsini_frontend_menucolor, 0);
	settings_filenamevalue = settingsini.GetInt(settingsini_frontend, settingsini_frontend_filename, 0);
	settings_locswitchvalue = settingsini.GetInt(settingsini_frontend, settingsini_frontend_locswitch, 0);
	settings_topbordervalue = settingsini.GetInt(settingsini_frontend, settingsini_frontend_topborder, 0);
	settings_countervalue = settingsini.GetInt(settingsini_frontend, settingsini_frontend_counter, 0);
	settings_custombotvalue = settingsini.GetInt(settingsini_frontend, settingsini_frontend_custombot, 0);
	romselect_toplayout = settingsini.GetInt(settingsini_frontend, settingsini_frontend_toplayout, 0);
	settings_autoupdatevalue = settingsini.GetInt(settingsini_frontend, settingsini_frontend_autoupdate, 0);
	settings_autodlvalue = settingsini.GetInt(settingsini_frontend, settingsini_frontend_autodl, 0);
	// romselect_layout = settingsini.GetInt(settingsini_frontend, settingsini_frontend_botlayout, 0);
	twlsettings_rainbowledvalue = settingsini.GetInt(settingsini_twlmode, settingsini_twl_rainbowled, 0);
	twlsettings_cpuspeedvalue = settingsini.GetInt(settingsini_twlmode, settingsini_twl_clock, 0);
	twlsettings_extvramvalue = settingsini.GetInt(settingsini_twlmode, settingsini_twl_vram, 0);
	twlsettings_bootscreenvalue = settingsini.GetInt(settingsini_twlmode, settingsini_twl_bootani, 0);
	twlsettings_healthsafetyvalue = settingsini.GetInt(settingsini_twlmode, settingsini_twl_hsmsg, 0);
	twlsettings_resetslot1value = settingsini.GetInt(settingsini_twlmode, settingsini_twl_resetslot1, 0);
	twlsettings_forwardervalue = settingsini.GetInt(settingsini_twlmode, settingsini_twl_forwarder, 0);
	twlsettings_flashcardvalue = settingsini.GetInt(settingsini_twlmode, settingsini_twl_flashcard, 0);
	if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_debug, 0) == 1) {
		twlsettings_consolevalue = 2;
	} else if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_debug, 0) == 0) {
		twlsettings_consolevalue = 1;
	} else if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_debug, 0) == -1) {
		twlsettings_consolevalue = 0;
	}
	if (bootstrapini.GetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, 0) == 1) {
		twlsettings_consolevalue = 2;
	} else if (bootstrapini.GetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, 0) == 0) {
		twlsettings_consolevalue = 1;
	} else if (bootstrapini.GetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, 0) == -1) {
		twlsettings_consolevalue = 0;
	}
	twlsettings_lockarm9scfgextvalue = bootstrapini.GetInt(bootstrapini_ndsbootstrap, bootstrapini_lockarm9scfgext, 0);
	LogFM("Main.LoadSettings", "Settings load successfully");
}

void SaveSettings() {
	settingsini.SetInt(settingsini_frontend, settingsini_frontend_color, settings_colorvalue);
	settingsini.SetInt(settingsini_frontend, settingsini_frontend_menucolor, settings_menucolorvalue);
	settingsini.SetInt(settingsini_frontend, settingsini_frontend_filename, settings_filenamevalue);
	settingsini.SetInt(settingsini_frontend, settingsini_frontend_locswitch, settings_locswitchvalue);
	settingsini.SetInt(settingsini_frontend, settingsini_frontend_topborder, settings_topbordervalue);
	settingsini.SetInt(settingsini_frontend, settingsini_frontend_counter, settings_countervalue);
	settingsini.SetInt(settingsini_frontend, settingsini_frontend_custombot, settings_custombotvalue);
	settingsini.SetInt(settingsini_frontend, settingsini_frontend_toplayout, romselect_toplayout);
	settingsini.SetInt(settingsini_frontend, settingsini_frontend_autoupdate, settings_autoupdatevalue);
	settingsini.SetInt(settingsini_frontend, settingsini_frontend_autodl, settings_autodlvalue);
	//settingsini.SetInt(settingsini_frontend, settingsini_frontend_botlayout, romselect_layout);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_rainbowled, twlsettings_rainbowledvalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_clock, twlsettings_cpuspeedvalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_vram, twlsettings_extvramvalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_bootani, twlsettings_bootscreenvalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_hsmsg, twlsettings_healthsafetyvalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_launchslot1, twlsettings_launchslot1value);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_resetslot1, twlsettings_resetslot1value);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_keepsd, keepsdvalue);
	if (twlsettings_consolevalue == 0) {
		settingsini.SetInt(settingsini_twlmode, settingsini_twl_debug, -1);
	} else if (twlsettings_consolevalue == 1) {
		settingsini.SetInt(settingsini_twlmode, settingsini_twl_debug, 0);
	} else if (twlsettings_consolevalue == 2) {
		settingsini.SetInt(settingsini_twlmode, settingsini_twl_debug, 1);
	}
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_forwarder, twlsettings_forwardervalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_flashcard, twlsettings_flashcardvalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_gbarunner, gbarunnervalue);
	settingsini.SaveIniFile("sdmc:/_nds/twloader/settings.ini");
	if (applaunchprep || fadeout) {
		// Set ROM path if ROM is selected
		if (twlsettings_launchslot1value == 0) {
			bootstrapini.SetString(bootstrapini_ndsbootstrap, bootstrapini_ndspath, fat+romfolder+rom);
			if (gbarunnervalue == 0) {
				bootstrapini.SetString(bootstrapini_ndsbootstrap, bootstrapini_savpath, fat+romfolder+sav);
				char path[256];
				snprintf(path, sizeof(path), "sdmc:/%s%s", romfolder.c_str(), sav);
				if (access(path, F_OK) == -1) {
					// Create a save file if it doesn't exist
					CreateGameSave(path);
				}
				free(sav);
			}
		}
	}
	bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_boostcpu, twlsettings_cpuspeedvalue);
	if (twlsettings_consolevalue == 0) {
		bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, -1);
	} else if (twlsettings_consolevalue == 1) {
		bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, 0);
	} else if (twlsettings_consolevalue == 2) {
		bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, 1);
	}
	bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_lockarm9scfgext, twlsettings_lockarm9scfgextvalue);
	bootstrapini.SaveIniFile("sdmc:/_nds/nds-bootstrap.ini");
}

static bool dspfirmfound = false;
static sf2d_texture *voltex[5] = { };		// Main screen.
static sf2d_texture *setvoltex[5] = { };	// Settings screen.

/**
 * Draw the volume slider.
 * @param texarray Texture array to use, (voltex or setvoltex)
 */
static void draw_volume_slider(sf2d_texture *texarray[])
{
	u8 volumeLevel = 0;
	if (!dspfirmfound) {
		// No DSP Firm.
		sf2d_draw_texture(texarray[4], 5, 2);
	} else if (R_SUCCEEDED(HIDUSER_GetSoundVolume(&volumeLevel))) {
		u8 voltex_id = 0;
		if (volumeLevel == 0) {
			voltex_id = 0;	// No slide = volume0 texture
		} else if (volumeLevel <= 21) {
			voltex_id = 1;	// 25% or less = volume1 texture
		} else if (volumeLevel <= 42) {
			voltex_id = 2;	// about 50% = volume2 texture
		} else if (volumeLevel >= 43) {
			voltex_id = 3;	// above 75% = volume3 texture
		}
		sf2d_draw_texture(texarray[voltex_id], 5, 2);
	}
}

static sf2d_texture *batteryIcon = NULL;	// Current battery level icon.
static sf2d_texture *batterychrgtex = NULL;	// Main screen. (fully charged)
static sf2d_texture *setbatterychrgtex = NULL;	// Settings screen. (fully charged)
static sf2d_texture *batterytex[6] = { };	// Main screen.
static sf2d_texture *setbatterytex[6] = { };	// Settings screen.

/**
 * Update the battery level icon.
 * @param texchrg Texture for "battery is charging". (batterychrgtex or setbatterychrgtex)
 * @param texarray Texture array for other levels. (batterytex or setbatterytex)
 * The global variable batteryIcon will be updated.
 */
static void update_battery_level(sf2d_texture *texchrg, sf2d_texture *texarray[])
{
	u8 batteryChargeState = 0;
	u8 batteryLevel = 0;
	if (R_SUCCEEDED(PTMU_GetBatteryChargeState(&batteryChargeState)) && batteryChargeState) {
		batteryIcon = batterychrgtex;
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

	struct dirent *namelist;
	const int extlen = (ext ? strlen(ext) : 0);
	while ((namelist = readdir(dir)) != NULL) {
		std::string fname = (namelist->d_name);
		if (extlen > 0) {
			// Check the file extension. (TODO needs verification)
			size_t lastdotpos = fname.find_last_of('.');
			if (lastdotpos == string::npos || lastdotpos + extlen > fname.size()) {
				// Invalid file extension.
				continue;
			}
			if (strcasecmp(&namelist->d_name[lastdotpos], ext) != 0) {
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

int main()
{
	aptInit();
	cfguInit();
	amInit();
	ptmuInit();	// For battery status
	ptmuxInit();	// For AC adapter status
	sdmcInit();
	romfsInit();
	srvInit();
	hidInit();
	
	sf2d_init();
	sf2d_set_clear_color(RGBA8(0x00, 0x00, 0x00, 0x00));
	sf2d_set_3D(0);

	sf2d_texture *settingslogotex = sfil_load_PNG_file("romfs:/graphics/settings/logo.png", SF2D_PLACE_RAM); // TWLoader logo on top screen

	sf2d_start_frame(GFX_TOP, GFX_LEFT);
	sf2d_draw_texture(settingslogotex, 400/2 - settingslogotex->width/2, 240/2 - settingslogotex->height/2);
	sf2d_end_frame();
	sf2d_start_frame(GFX_TOP, GFX_RIGHT);
	sf2d_draw_texture(settingslogotex, 400/2 - settingslogotex->width/2, 240/2 - settingslogotex->height/2);
	sf2d_end_frame();
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	sf2d_end_frame();
	sf2d_swapbuffers();
	
	createLog();

	// make folders if they don't exist
	mkdir("sdmc:/roms/nds", 0777);
	mkdir("sdmc:/roms/flashcard/nds", 0777);
	mkdir("sdmc:/_nds/twloader", 0777);
	mkdir("sdmc:/_nds/twloader/bnricons", 0777);
	mkdir("sdmc:/_nds/twloader/bnricons/flashcard", 0777);
	mkdir("sdmc:/_nds/twloader/boxart", 0777);
	mkdir("sdmc:/_nds/twloader/boxart/flashcard", 0777);
	//mkdir("sdmc:/_nds/twloader/tmp", 0777);

	std::string	bootstrapPath = "";
	
	// Font loading
	sftd_init();
	font = sftd_load_font_file("romfs:/fonts/FOT-RodinBokutoh Pro M.otf");
	font_b = sftd_load_font_file("romfs:/fonts/FOT-RodinBokutoh Pro DB.otf");
	sftd_draw_textf(font, 0, 0, RGBA8(0, 0, 0, 255), 16, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890&:-.'!?()\"end"); //Hack to avoid blurry text!
	sftd_draw_textf(font_b, 0, 0, RGBA8(0, 0, 0, 255), 24, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890&:-.'!?()\"end"); //Hack to avoid blurry text!	
	LogFM("Main.Font loading", "Fonts load correctly");
	
	sVerfile Verfile;
	
	FILE* VerFile = fopen("romfs:/ver", "r");
	fread(&Verfile,1,sizeof(Verfile),VerFile);
	strcpy(settings_vertext, Verfile.text);
	fclose(VerFile);
	LogFMA("Main.Verfile (ROMFS)", "Successful reading ver from ROMFS",Verfile.text);
	
	CFGU_GetSystemLanguage(&language);

	LoadSettings();

	LoadColor();
	LoadMenuColor();
	LoadBottomImage();
	dialogboxtex = sfil_load_PNG_file("romfs:/graphics/dialogbox.png", SF2D_PLACE_RAM); // Dialog box
	sf2d_texture *toptex = sfil_load_PNG_file("romfs:/graphics/top.png", SF2D_PLACE_RAM); // Top DSi-Menu border
	sf2d_texture *topbgtex; // Top background, behind the DSi-Menu border

	// Volume slider textures.
	voltex[0] = sfil_load_PNG_file("romfs:/graphics/volume0.png", SF2D_PLACE_RAM); // Show no volume
	voltex[1] = sfil_load_PNG_file("romfs:/graphics/volume1.png", SF2D_PLACE_RAM); // Volume low above 0
	voltex[2] = sfil_load_PNG_file("romfs:/graphics/volume2.png", SF2D_PLACE_RAM); // Volume medium
	voltex[3] = sfil_load_PNG_file("romfs:/graphics/volume3.png", SF2D_PLACE_RAM); // Hight volume
	voltex[4] = sfil_load_PNG_file("romfs:/graphics/volume4.png", SF2D_PLACE_RAM); // No DSP firm found

	sf2d_texture *shoulderLtex = sfil_load_PNG_file("romfs:/graphics/shoulder_L.png", SF2D_PLACE_RAM); // L shoulder
	sf2d_texture *shoulderRtex = sfil_load_PNG_file("romfs:/graphics/shoulder_R.png", SF2D_PLACE_RAM); // R shoulder
	sf2d_texture *shoulderYtex = sfil_load_PNG_file("romfs:/graphics/shoulder_Y.png", SF2D_PLACE_RAM); // Y button
	sf2d_texture *shoulderXtex = sfil_load_PNG_file("romfs:/graphics/shoulder_X.png", SF2D_PLACE_RAM); // X button

	// Battery level textures.
	batterychrgtex = sfil_load_PNG_file("romfs:/graphics/battery_charging.png", SF2D_PLACE_RAM);
	batterytex[0] = sfil_load_PNG_file("romfs:/graphics/battery0.png", SF2D_PLACE_RAM);
	batterytex[1] = sfil_load_PNG_file("romfs:/graphics/battery1.png", SF2D_PLACE_RAM);
	batterytex[2] = sfil_load_PNG_file("romfs:/graphics/battery2.png", SF2D_PLACE_RAM);
	batterytex[3] = sfil_load_PNG_file("romfs:/graphics/battery3.png", SF2D_PLACE_RAM);
	batterytex[4] = sfil_load_PNG_file("romfs:/graphics/battery4.png", SF2D_PLACE_RAM);
	batterytex[5] = sfil_load_PNG_file("romfs:/graphics/battery5.png", SF2D_PLACE_RAM);

	sf2d_texture *bottomtex; // Bottom of menu
	sf2d_texture *iconunktex = sfil_load_PNG_file("romfs:/graphics/icon_placeholder.png", SF2D_PLACE_RAM); // Icon placeholder at bottom of menu
	sf2d_texture *homeicontex = sfil_load_PNG_file("romfs:/graphics/homeicon.png", SF2D_PLACE_RAM); // HOME icon
	sf2d_texture *whomeicontex = sfil_load_PNG_file("romfs:/graphics/whomeicon.png", SF2D_PLACE_RAM); // HOME icon (Settings)
	sf2d_texture *bottomlogotex = sfil_load_PNG_file("romfs:/graphics/bottom_logo.png", SF2D_PLACE_RAM); // TWLoader logo on bottom screen
	sf2d_texture *dotcircletex; // Dots forming a circle
	sf2d_texture *startbordertex; // "START" border
	sf2d_texture *settingsboxtex = sfil_load_PNG_file("romfs:/graphics/settingsbox.png", SF2D_PLACE_RAM); // Settings box on bottom screen
	sf2d_texture *getfcgameboxtex = sfil_load_PNG_file("romfs:/graphics/getfcgamebox.png", SF2D_PLACE_RAM);
	sf2d_texture *carttex = sfil_load_PNG_file("romfs:/graphics/cart.png", SF2D_PLACE_RAM); // Cartridge on bottom screen
	sf2d_texture *boxfulltex = sfil_load_PNG_file("romfs:/graphics/box_full.png", SF2D_PLACE_RAM); // (DSiWare) box on bottom screen
	sf2d_texture *bracetex = sfil_load_PNG_file("romfs:/graphics/brace.png", SF2D_PLACE_RAM); // Brace (C-shaped thingy)
	sf2d_texture *bubbletex = sfil_load_PNG_file("romfs:/graphics/bubble.png", SF2D_PLACE_RAM); // Text bubble
	sf2d_texture *dsboottex = NULL;		// DS boot screen in settings
	sf2d_texture *dsiboottex = NULL;	// DSi boot screen in settings
	sf2d_texture *dshstex = NULL;		// DS H&S screen in settings
	sf2d_texture *dsihstex = NULL;		// DSi H&S screen in settings
	sf2d_texture *disabledtex = NULL;	// Red circle with line

	LogFM("Main.sf2d_textures", "Textures load successfully");

	dspfirmfound = false;
 	if( access( "sdmc:/3ds/dspfirm.cdc", F_OK ) != -1 ) {
		ndspInit();
		dspfirmfound = true;
		LogFM("Main.dspfirm", "DSP Firm found!");
	}else{
		LogFM("Main.dspfirm", "DSP Firm not found");
	}

	bool musicbool = false;
	if( access( "sdmc:/_nds/twloader/music.wav", F_OK ) != -1 ) {
		musicpath = "sdmc:/_nds/twloader/music.wav";
		LogFM("Main.music", "Custom music file found!");
	}else {
		LogFM("Main.dspfirm", "No music file found");
	}

	sound bgm_menu(musicpath);
	//sound bgm_settings("sdmc:/_nds/twloader/music/settings.wav");
	sound sfx_launch("romfs:/sounds/launch.wav",2,false);
	sound sfx_select("romfs:/sounds/select.wav",2,false);
	sound sfx_stop("romfs:/sounds/stop.wav",2,false);
	sound sfx_switch("romfs:/sounds/switch.wav",2,false);
	sound sfx_wrong("romfs:/sounds/wrong.wav",2,false);
	sound sfx_back("romfs:/sounds/back.wav",2,false);

	// Scan the ROMs directory for ".nds" files.
	scan_dir_for_files("sdmc:/roms/nds", ".nds", files);
	
	// Scan the flashcard directory for configuration files.
	scan_dir_for_files("sdmc:/roms/flashcard/nds", ".ini", fcfiles);

	char romsel_counter2sd[16];	// Number of ROMs on the SD card.
	snprintf(romsel_counter2sd, sizeof(romsel_counter2sd), "%d", files.size());
	
	char romsel_counter2fc[16];	// Number of ROMs on the flash card.
	snprintf(romsel_counter2fc, sizeof(romsel_counter2fc), "%d", fcfiles.size());

	// Download box art
	if (checkWifiStatus()) {
		downloadBoxArt();
	}

	// Cache banner data
	for (bnriconnum = 0; bnriconnum < files.size(); bnriconnum++) {
		static const char title[] = "Now checking if banner data exists (SD Card)...";
		char romsel_counter1[16];
		snprintf(romsel_counter1, sizeof(romsel_counter1), "%d", bnriconnum+1);
		const char *tempfile = files.at(bnriconnum).c_str();
		DialogBoxAppear(title);
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(dialogboxtex, 0, 0);
		sftd_draw_textf(font, 12, 16, RGBA8(0, 0, 0, 255), 12, title);
		sftd_draw_textf(font, 12, 48, RGBA8(0, 0, 0, 255), 12, romsel_counter1);
		sftd_draw_textf(font, 31, 48, RGBA8(0, 0, 0, 255), 12, "/");
		sftd_draw_textf(font, 36, 48, RGBA8(0, 0, 0, 255), 12, romsel_counter2sd);
		sftd_draw_textf(font, 12, 64, RGBA8(0, 0, 0, 255), 12, tempfile);

		char nds_path[256];
		snprintf(nds_path, sizeof(nds_path), "sdmc:/roms/nds/%s", tempfile);
		FILE *f_nds_file = fopen(nds_path, "rb");
		cacheBanner(f_nds_file, tempfile, font);
		fclose(f_nds_file);
	}

	if(settings_autodlvalue == 1 && checkWifiStatus() && (checkUpdate() == 0)){
		DownloadTWLoaderCIAs();
	}
	
	if(settings_autoupdatevalue == 2 && checkWifiStatus()){
		UpdateBootstrapUnofficial();
	} else if(settings_autoupdatevalue == 1 && checkWifiStatus()){
		UpdateBootstrapRelease();
	}
	DialogBoxDisappear(nullptr);
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	sf2d_end_frame();
	sf2d_swapbuffers();

	int cursorPosition = 0, storedcursorPosition = 0, filenum = 0;
	bool noromsfound = false;
	int settingscursorPosition = 0, twlsettingscursorPosition = 0;
	
	bool cursorPositionset = false;
	
	int soundwaittimer = 0;
	bool playwrongsounddone = false;

	bool colortexloaded = false;
	bool colortexloaded_bot = false;
	bool bannertextloaded = false;
	bool bnricontexloaded = false;
	bool boxarttexloaded = false;

	bool updatebotscreen = true;
	bool screenmodeswitch = false;
	bool applaunchicon = false;
	bool applaunchon = false;
		
	float offset3dl_topbg = 0.0f;
	float offset3dr_topbg = 0.0f;
	float offset3dl_boxart = 0.0f;
	float offset3dr_boxart = 0.0f;
	float offset3dl_disabled = 0.0f;
	float offset3dr_disabled = 0.0f;
	
	float rad = 0.0f;
	u16 touch_x = 320/2;
	u16 touch_y = 240/2;
	
	int textWidth = 0;
	int textHeight = 0;
	
	int boxartXpos;
	int boxartXmovepos = 0;
	
	int YbuttonYpos = 220;
	int XbuttonYpos = 220;
	
	int LshoulderYpos = 220;
	int RshoulderYpos = 220;

	int filenameYpos;
	int filenameYmovepos = 0;
	int setsboxXpos = 0;
	int cartXpos = 64;
	int titleboxXpos;
	int titleboxXmovepos = 0;
	int titleboxXmovetimer = 1; // Set to 1 for fade-in effect to run
	bool titleboxXmoveleft = false;
	bool titleboxXmoveright = false;
	int titleboxYmovepos = 120;
	int boxartYmovepos = 63;
	int boxartreflYmovepos = 264;
	int ndsiconXpos;
	int ndsiconYmovepos = 133;
	
	int startbordermovepos;
	float startborderscalesize;
	
	int settingsXpos = 24;
	int settingsvalueXpos = 236;
	int settingsYpos;
	
	sf2d_set_3D(1);
	
	// We need these 2 buffers for APT_DoAppJump() later. They can be smaller too
	u8 param[0x300];
	u8 hmac[0x20];
	// Clear both buffers
	memset(param, 0, sizeof(param));
	memset(hmac, 0, sizeof(hmac));
	// Loop as long as the status is not exit
	while(run && aptMainLoop()) {
	//while(run) {
		// Scan hid shared memory for input events
		hidScanInput();
		
		u32 hUp = hidKeysUp();
		u32 hDown = hidKeysDown();
		u32 hHeld = hidKeysHeld();
		
		offset3dl_topbg = CONFIG_3D_SLIDERSTATE * -12.0f;
		offset3dr_topbg = CONFIG_3D_SLIDERSTATE * 12.0f;
		offset3dl_boxart = CONFIG_3D_SLIDERSTATE * -5.0f;
		offset3dr_boxart = CONFIG_3D_SLIDERSTATE * 5.0f;
		offset3dl_disabled = CONFIG_3D_SLIDERSTATE * -3.0f;
		offset3dr_disabled = CONFIG_3D_SLIDERSTATE * 3.0f;

		if (storedcursorPosition < 0)
			storedcursorPosition = 0;

		if(screenmode == SCREEN_MODE_ROM_SELECT) {
			if (!colortexloaded) {
				topbgtex = sfil_load_PNG_file(color_data->topbgloc, SF2D_PLACE_RAM); // Top background, behind the DSi-Menu border
				for (int i = 0; i < 5; i++) {
					sf2d_free_texture(setvoltex[i]);
					setvoltex[i] = NULL;
				}
				sf2d_free_texture(setbatterychrgtex);
				for (int i = 0; i < 6; i++) {
					sf2d_free_texture(setbatterytex[i]);
					setbatterytex[i] = NULL;
				}
				sf2d_free_texture(dsboottex);
				sf2d_free_texture(dsiboottex);
				sf2d_free_texture(dshstex);
				sf2d_free_texture(dsihstex);
				sf2d_free_texture(disabledtex);
				colortexloaded = true;
			}
			if (!bnricontexloaded) {
				if (twlsettings_forwardervalue == 0) {
					/* sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
					sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Now loading banner icons (SD Card)...");
					sf2d_end_frame();
					sf2d_swapbuffers(); */
					char path[256];
					for (bnriconnum = pagenum*20; bnriconnum < 20+pagenum*20; bnriconnum++) {
						if (bnriconnum < files.size()) {
							const char *tempfile = files.at(bnriconnum).c_str();
							snprintf(path, sizeof(path), "sdmc:/_nds/twloader/bnricons/%s.bin", tempfile);
							StoreBNRIconPath(path);
						} else {
							StoreBNRIconPath("romfs:/notextbanner");
						}
						OpenBNRIcon();
						LoadBNRIcon();
					}
				} else {
					/* sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
					sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Now loading banner icons (Flashcard)...");
					sf2d_end_frame();
					sf2d_swapbuffers(); */
					char path[256];
					for (bnriconnum = pagenum*20; bnriconnum < 20+pagenum*20; bnriconnum++) {
						if (bnriconnum < fcfiles.size()) {
							const char *tempfile = fcfiles.at(bnriconnum).c_str();
							snprintf(path, sizeof(path), "%s%s.bin", fcbnriconfolder, tempfile);
							if (access(path, F_OK) != -1) {
								StoreBNRIconPath(path);
							} else {
								StoreBNRIconPath("romfs:/notextbanner");
							}
						} else {
							StoreBNRIconPath("romfs:/notextbanner");
						}
						OpenBNRIcon();
						LoadBNRIcon();
					}
				}

				bnricontexloaded = true;
				bnriconnum = 0+pagenum*20;
			}
			if (!boxarttexloaded) {
				if (twlsettings_forwardervalue == 0) {
					/* sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
					sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Now storing box art filenames (SD Card)...");
					sf2d_end_frame();
					sf2d_swapbuffers(); */
					char path[256];
					for(boxartnum = pagenum*20; boxartnum < 20+pagenum*20; boxartnum++) {
						if (boxartnum < files.size()) {
							const char *tempfile = files.at(boxartnum).c_str();
							snprintf(path, sizeof(path), "sdmc:/roms/nds/%s", tempfile);
							FILE *f_nds_file = fopen(path, "rb");

							char ba_TID[5];
							grabTID(f_nds_file, ba_TID);
							ba_TID[4] = 0;
							fclose(f_nds_file);

							// example: SuperMario64DS.nds.png
							snprintf(path, sizeof(path), "%s%s.png", boxartfolder, tempfile);
							if (access(path, F_OK ) != -1 ) {
								StoreBoxArtPath(path);
							} else {
								// example: ASME.png
								snprintf(path, sizeof(path), "%s%.4s.png", boxartfolder, ba_TID);
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
				} else {
					/* sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
					sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Now storing box art filenames (Flashcard)...");
					sf2d_end_frame();
					sf2d_swapbuffers(); */
					char path[256];
					for(boxartnum = pagenum*20; boxartnum < 20+pagenum*20; boxartnum++) {
						if (boxartnum < fcfiles.size()) {
							const char *tempfile = fcfiles.at(boxartnum).c_str();
							snprintf(path, sizeof(path), "sdmc:/roms/flashcard/nds/%s", tempfile);

							CIniFile setfcrompathini( path );
							std::string	ba_TIDini = setfcrompathini.GetString(fcrompathini_flashcardrom, fcrompathini_tid, "");
							char ba_TID[5];
							strcpy(ba_TID, ba_TIDini.c_str());
							ba_TID[4] = 0;

							// example: SuperMario64DS.nds.png
							snprintf(path, sizeof(path), "%s%s.png", boxartfolder, tempfile);
							if (access(path, F_OK ) != -1 ) {
								StoreBoxArtPath(path);
							} else {
								// example: ASME.png
								snprintf(path, sizeof(path), "%s%.4s.png", boxartfolder, ba_TID);
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
				
				// Load up to 6 boxarts.
				for (boxartnum = pagenum*20; boxartnum < 6+pagenum*20; boxartnum++) {
					LoadBoxArt();
				}
				boxarttexloaded = true;
				boxartnum = 0+pagenum*20;
			}

			if (!musicbool) {
				if (dspfirmfound) { bgm_menu.play(); }
				musicbool = true;
			}
			if (twlsettings_forwardervalue == 1) {
				noromtext1 = "No games found!";
				noromtext2 = "Select 'Add games' to get started.";
			} else {
				noromtext1 = "No games found!";
				noromtext2 = "Put .nds ROMs in 'sdmc:/roms/nds'.";
			}

			update_battery_level(batterychrgtex, batterytex);
			sf2d_start_frame(GFX_TOP, GFX_LEFT);			
			sf2d_draw_texture_scale(topbgtex, offset3dl_topbg-12, 0, 1.32, 1);
			if (filenum != 0) {	// If ROMs are found, then display box art
				if (romselect_toplayout == 0) {
					boxartXpos = 136;
					if (twlsettings_forwardervalue == 1) {
						if(fcfiles.size() >= 19+pagenum*20) {
							for(boxartnum = pagenum*20; boxartnum < 20+pagenum*20; boxartnum++) {
								ChangeBoxArtNo();
								sf2d_draw_texture(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
								sf2d_draw_texture_scale_blend(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, SET_ALPHA(color_data->color, 0xC0)); // Draw box art's reflection
								boxartXpos += 144;
							}
						} else {
							for(boxartnum = pagenum*20; boxartnum < fcfiles.size(); boxartnum++) {
								ChangeBoxArtNo();
								sf2d_draw_texture(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
								sf2d_draw_texture_scale_blend(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, SET_ALPHA(color_data->color, 0xC0)); // Draw box art's reflection
								boxartXpos += 144;
							}
						}
					} else {
						if(files.size() >= 19+pagenum*20) {
							for(boxartnum = pagenum*20; boxartnum < 20+pagenum*20; boxartnum++) {
								ChangeBoxArtNo();
								sf2d_draw_texture(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
								sf2d_draw_texture_scale_blend(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, SET_ALPHA(color_data->color, 0xC0)); // Draw box art's reflection
								boxartXpos += 144;
							}
						} else {
							for(boxartnum = pagenum*20; boxartnum < files.size(); boxartnum++) {
								ChangeBoxArtNo();
								sf2d_draw_texture(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
								sf2d_draw_texture_scale_blend(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, SET_ALPHA(color_data->color, 0xC0)); // Draw box art's reflection
								boxartXpos += 144;
							}
						}
					}
					if (applaunchprep) {
						if (cursorPosition >= 0) {
							boxartnum = cursorPosition;
							ChangeBoxArtNo();
							sf2d_draw_texture_part(topbgtex, offset3dl_boxart+136, 63, offset3dl_boxart+104, 63, 128, 115*2);
							sf2d_draw_texture(boxarttexnum, offset3dl_boxart+136, boxartYmovepos); // Draw moving box art
							sf2d_draw_texture_scale_blend(boxarttexnum, offset3dl_boxart+136, boxartreflYmovepos, 1, -0.75, SET_ALPHA(color_data->color, 0xC0)); // Draw moving box art's reflection
						}
					}
				}
			} else {
				sftd_draw_textf(font, offset3dl_boxart+152, 96, RGBA8(255, 255, 255, 255), 12, noromtext1);
				sftd_draw_textf(font, offset3dl_boxart+124, 112, RGBA8(255, 255, 255, 255), 12, noromtext2);
			}
			if (settings_topbordervalue == 1) {
				sf2d_draw_texture_blend(toptex, 400/2 - toptex->width/2, 240/2 - toptex->height/2, menucolor);
				sftd_draw_text(font, 328, 3, RGBA8(0, 0, 0, 255), 12, RetTime().c_str());
			} else {
				sftd_draw_text(font, 328, 3, RGBA8(255, 255, 255, 255), 12, RetTime().c_str());
			}

			draw_volume_slider(voltex);
			sf2d_draw_texture(batteryIcon, 371, 2);
			sftd_draw_textf(font, 32, 2, SET_ALPHA(color_data->color, 255), 12, name.c_str());
			// sftd_draw_textf(font, 2, 2, RGBA8(0, 0, 0, 255), 12, temptext); // Debug text
			sf2d_draw_texture(shoulderLtex, 0, LshoulderYpos);
			sftd_draw_textf(font, 17, LshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Lshouldertext);
			if (settings_locswitchvalue == 1) {
				sf2d_draw_texture(shoulderRtex, 328, RshoulderYpos);
				sftd_draw_textf(font, 332, RshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Rshouldertext);
			}

			sf2d_draw_rectangle(0, 0, 400, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect

			sf2d_end_frame();
				
			sf2d_start_frame(GFX_TOP, GFX_RIGHT);
			sf2d_draw_texture_scale(topbgtex, offset3dr_topbg-12, 0, 1.32, 1);
			if (filenum != 0) {	// If ROMs are found, then display box art
				if (romselect_toplayout == 0) {
					boxartXpos = 136;
					if (twlsettings_forwardervalue == 1) {
						if(fcfiles.size() >= 19+pagenum*20) {
							for(boxartnum = pagenum*20; boxartnum < 20+pagenum*20; boxartnum++) {
								ChangeBoxArtNo();
								sf2d_draw_texture(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
								sf2d_draw_texture_scale_blend(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, SET_ALPHA(color_data->color, 0xC0)); // Draw box art's reflection
								boxartXpos += 144;
							}
						} else {
							for(boxartnum = pagenum*20; boxartnum < fcfiles.size(); boxartnum++) {
								ChangeBoxArtNo();
								sf2d_draw_texture(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
								sf2d_draw_texture_scale_blend(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, SET_ALPHA(color_data->color, 0xC0)); // Draw box art's reflection
								boxartXpos += 144;
							}
						}
					} else {
						if(files.size() >= 19+pagenum*20) {
							for(boxartnum = pagenum*20; boxartnum < 20+pagenum*20; boxartnum++) {
								ChangeBoxArtNo();
								sf2d_draw_texture(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
								sf2d_draw_texture_scale_blend(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, SET_ALPHA(color_data->color, 0xC0)); // Draw box art's reflection
								boxartXpos += 144;
							}
						} else {
							for(boxartnum = pagenum*20; boxartnum < files.size(); boxartnum++) {
								ChangeBoxArtNo();
								sf2d_draw_texture(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
								sf2d_draw_texture_scale_blend(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, SET_ALPHA(color_data->color, 0xC0)); // Draw box art's reflection
								boxartXpos += 144;
							}
						}
					}
					if (applaunchprep) {
						if (cursorPosition >= 0) {
							boxartnum = cursorPosition;
							ChangeBoxArtNo();
							sf2d_draw_texture_part(topbgtex, offset3dr_boxart+136, 63, offset3dr_boxart+104, 63, 128, 115*2);
							sf2d_draw_texture(boxarttexnum, offset3dr_boxart+136, boxartYmovepos); // Draw moving box art
							sf2d_draw_texture_scale_blend(boxarttexnum, offset3dr_boxart+136, boxartreflYmovepos, 1, -0.75, SET_ALPHA(color_data->color, 0xC0)); // Draw moving box art's reflection
						}
					}
				}
			} else {
				sftd_draw_textf(font, offset3dr_boxart+152, 96, RGBA8(255, 255, 255, 255), 12, noromtext1);
				sftd_draw_textf(font, offset3dr_boxart+124, 112, RGBA8(255, 255, 255, 255), 12, noromtext2);
			}
			if (settings_topbordervalue == 1) {
				sf2d_draw_texture_blend(toptex, 400/2 - toptex->width/2, 240/2 - toptex->height/2, menucolor);
				sftd_draw_text(font, 328, 3, RGBA8(0, 0, 0, 255), 12, RetTime().c_str());
			} else {
				sftd_draw_text(font, 328, 3, RGBA8(255, 255, 255, 255), 12, RetTime().c_str());
			}

			draw_volume_slider(voltex);
			sf2d_draw_texture(batteryIcon, 371, 2);
			sftd_draw_textf(font, 32, 2, SET_ALPHA(color_data->color, 255), 12, name.c_str());
			// sftd_draw_textf(font, 2, 2, RGBA8(0, 0, 0, 255), 12, temptext); // Debug text
			sf2d_draw_texture(shoulderLtex, -1, LshoulderYpos);
			sftd_draw_textf(font, 16, LshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Lshouldertext);
			if (settings_locswitchvalue == 1) {
				sf2d_draw_texture(shoulderRtex, 327, RshoulderYpos);
				sftd_draw_textf(font, 331, RshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Rshouldertext);
			}
			
			sf2d_draw_rectangle(0, 0, 400, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect
			
			sf2d_end_frame();
		} else if(screenmode == SCREEN_MODE_SETTINGS) {
			/* if (!musicbool) {
				if (dspfirmfound) { bgm_settings.play(); }
				musicbool = true;
			} */
			if (colortexloaded) {
				sf2d_free_texture(topbgtex);
				setvoltex[0] = sfil_load_PNG_file("romfs:/graphics/settings/volume0.png", SF2D_PLACE_RAM); // Show no volume (settings)
				setvoltex[1] = sfil_load_PNG_file("romfs:/graphics/settings/volume1.png", SF2D_PLACE_RAM); // Volume low above 0 (settings)
				setvoltex[2] = sfil_load_PNG_file("romfs:/graphics/settings/volume2.png", SF2D_PLACE_RAM); // Volume medium (settings)
				setvoltex[3] = sfil_load_PNG_file("romfs:/graphics/settings/volume3.png", SF2D_PLACE_RAM); // Hight volume (settings)
				setvoltex[4] = sfil_load_PNG_file("romfs:/graphics/settings/volume4.png", SF2D_PLACE_RAM); // No DSP firm found (settings)
				setbatterychrgtex = sfil_load_PNG_file("romfs:/graphics/settings/battery_charging.png", SF2D_PLACE_RAM);
				setbatterytex[0] = sfil_load_PNG_file("romfs:/graphics/settings/battery0.png", SF2D_PLACE_RAM);
				setbatterytex[1] = sfil_load_PNG_file("romfs:/graphics/settings/battery1.png", SF2D_PLACE_RAM);
				setbatterytex[2] = sfil_load_PNG_file("romfs:/graphics/settings/battery2.png", SF2D_PLACE_RAM);
				setbatterytex[3] = sfil_load_PNG_file("romfs:/graphics/settings/battery3.png", SF2D_PLACE_RAM);
				setbatterytex[4] = sfil_load_PNG_file("romfs:/graphics/settings/battery4.png", SF2D_PLACE_RAM);
				setbatterytex[5] = sfil_load_PNG_file("romfs:/graphics/settings/battery5.png", SF2D_PLACE_RAM);
				dsboottex = sfil_load_PNG_file("romfs:/graphics/settings/dsboot.png", SF2D_PLACE_RAM); // DS boot screen in settings
				dsiboottex = sfil_load_PNG_file("romfs:/graphics/settings/dsiboot.png", SF2D_PLACE_RAM); // DSi boot screen in settings
				dshstex = sfil_load_PNG_file("romfs:/graphics/settings/dshs.png", SF2D_PLACE_RAM); // DS H&S screen in settings
				dsihstex = sfil_load_PNG_file("romfs:/graphics/settings/dsihs.png", SF2D_PLACE_RAM); // DSi H&S screen in settings
				disabledtex = sfil_load_PNG_file("romfs:/graphics/settings/disable.png", SF2D_PLACE_RAM); // Red circle with line
				colortexloaded = false;
			}

			update_battery_level(setbatterychrgtex, setbatterytex);
			sf2d_start_frame(GFX_TOP, GFX_LEFT);
			sf2d_draw_texture_scale(settingstex, 0, 0, 1.32, 1);
			if (settings_subscreenmode == 1) {
				if (twlsettings_cpuspeedvalue == 1) {
					sf2d_draw_texture(dsiboottex, offset3dl_boxart+136, 20); // Draw boot screen
				} else {
					sf2d_draw_texture(dsboottex, offset3dl_boxart+136, 20); // Draw boot screen
				}
				if (twlsettings_healthsafetyvalue == 1) {
					if (twlsettings_cpuspeedvalue == 1) {
						sf2d_draw_texture(dsihstex, offset3dl_boxart+136, 124); // Draw H&S screen
					} else {
						sf2d_draw_texture(dshstex, offset3dl_boxart+136, 124); // Draw H&S screen
					}
				} else {
					// Draw a white screen in place of the H&S screen.
					sf2d_draw_rectangle(offset3dl_boxart+136, 124, 128, 96, RGBA8(255, 255, 255, 255));
				}
				if (twlsettings_bootscreenvalue == 0) {
					sf2d_draw_texture(disabledtex, offset3dl_disabled+136, 20); // Draw disabled texture
					sf2d_draw_texture(disabledtex, offset3dl_disabled+136, 124); // Draw disabled texture
				}
			} else {
				sf2d_draw_texture(settingslogotex, offset3dl_boxart+400/2 - settingslogotex->width/2, 240/2 - settingslogotex->height/2);
				if (settings_subscreenmode == 0) {
					sftd_draw_textf(font, offset3dl_disabled+72, 166, RGBA8(0, 0, 255, 255), 14, settings_xbuttontext);
					sftd_draw_textf(font, offset3dl_disabled+72, 180, RGBA8(0, 255, 0, 255), 14, settings_ybuttontext);
					sftd_draw_textf(font, offset3dl_disabled+72, 194, RGBA8(255, 255, 255, 255), 14, settings_startbuttontext);
				}
			}
			sftd_draw_text(font, 328, 3, RGBA8(255, 255, 255, 255), 12, RetTime().c_str());
			sftd_draw_textf(font, 334, 222, RGBA8(255, 255, 255, 255), 14, settings_vertext);

			draw_volume_slider(setvoltex);
			sf2d_draw_texture(batteryIcon, 371, 2);
			sftd_draw_textf(font, 32, 2, SET_ALPHA(color_data->color, 255), 12, name.c_str());
			sf2d_draw_rectangle(0, 0, 400, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect
			sf2d_end_frame();
			
			sf2d_start_frame(GFX_TOP, GFX_RIGHT);
			sf2d_draw_texture_scale(settingstex, 0, 0, 1.32, 1);
			if (settings_subscreenmode == 1) {
				if (twlsettings_cpuspeedvalue == 1) {
					sf2d_draw_texture(dsiboottex, offset3dr_boxart+136, 20); // Draw boot screen
				} else {
					sf2d_draw_texture(dsboottex, offset3dr_boxart+136, 20); // Draw boot screen
				}
				if (twlsettings_healthsafetyvalue == 1) {
					if (twlsettings_cpuspeedvalue == 1) {
						sf2d_draw_texture(dsihstex, offset3dr_boxart+136, 124); // Draw H&S screen
					} else {
						sf2d_draw_texture(dshstex, offset3dr_boxart+136, 124); // Draw H&S screen
					}
				} else {
					// Draw a white screen in place of the H&S screen.
					sf2d_draw_rectangle(offset3dr_boxart+136, 124, 128, 96, RGBA8(255, 255, 255, 255));
				}
				if (twlsettings_bootscreenvalue == 0) {
					sf2d_draw_texture(disabledtex, offset3dr_disabled+136, 20); // Draw disabled texture
					sf2d_draw_texture(disabledtex, offset3dr_disabled+136, 124); // Draw disabled texture
				}
			} else {
				sf2d_draw_texture(settingslogotex, offset3dr_boxart+400/2 - settingslogotex->width/2, 240/2 - settingslogotex->height/2);
				if (settings_subscreenmode == 0) {
					sftd_draw_textf(font, offset3dr_disabled+72, 166, RGBA8(0, 0, 255, 255), 14, settings_xbuttontext);
					sftd_draw_textf(font, offset3dr_disabled+72, 180, RGBA8(0, 255, 0, 255), 14, settings_ybuttontext);
					sftd_draw_textf(font, offset3dr_disabled+72, 194, RGBA8(255, 255, 255, 255), 14, settings_startbuttontext);
				}
			}
			sftd_draw_text(font, 328, 3, RGBA8(255, 255, 255, 255), 12, RetTime().c_str());
			sftd_draw_textf(font, 334, 222, RGBA8(255, 255, 255, 255), 14, settings_vertext);
			draw_volume_slider(setvoltex);
			sf2d_draw_texture(batteryIcon, 371, 2);
			sftd_draw_textf(font, 32, 2, SET_ALPHA(color_data->color, 255), 12, name.c_str());
			sf2d_draw_rectangle(0, 0, 400, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect
			sf2d_end_frame();
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
		
		if(hHeld & KEY_Y){
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
		}
		
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
				} else {
					// run = false;
					if (twlsettings_forwardervalue == 1) {
						switch (twlsettings_flashcardvalue) {
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
					}
					gbarunnervalue = 1;
					SaveSettings();
					screenoff();
					if (twlsettings_rainbowledvalue == 1)
						RainbowLED();
					LogFM("Main.applaunchprep", "Switching to NTR/TWL-mode");
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
				if (dspfirmfound) { sfx_stop.stop(); }
				if (dspfirmfound) { sfx_stop.play(); }
			} else if (titleboxXmovetimer == 8) {
				titleboxXmovepos += 8;
				boxartXmovepos += 18;
				startbordermovepos = 1;
				startborderscalesize = 0.97;
				cursorPositionset = false;
			} else if (titleboxXmovetimer == 2) {
				titleboxXmovepos += 8;
				boxartXmovepos += 18;
				if (dspfirmfound) { sfx_select.stop(); }
				if (dspfirmfound) { sfx_select.play(); }
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
								sfx_wrong.stop();
								sfx_wrong.play();
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
								sfx_wrong.stop();
								sfx_wrong.play();
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
				if (dspfirmfound) { sfx_stop.stop(); }
				if (dspfirmfound) { sfx_stop.play(); }
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
			} else if (titleboxXmovetimer == 8) {
				titleboxXmovepos -= 8;
				boxartXmovepos -= 18;
				startbordermovepos = 1;
				startborderscalesize = 0.97;
				cursorPositionset = false;
			} else if (titleboxXmovetimer == 2) {
				titleboxXmovepos -= 8;
				boxartXmovepos -= 18;
				if (dspfirmfound) { sfx_select.stop(); }
				if (dspfirmfound) { sfx_select.play(); }
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
							sfx_wrong.stop();
							sfx_wrong.play();
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
					rad == 0.0f;
					boxartYmovepos = 63;
					boxartreflYmovepos = 264;
					titleboxYmovepos = 120;
					ndsiconYmovepos = 133;
					fadein = true;
					screenmodeswitch = false;
					applaunchprep = false;
				} else {
					if (twlsettings_forwardervalue == 1) {
						CIniFile setfcrompathini( sdmc+flashcardfolder+rom );
						std::string rominini = setfcrompathini.GetString(fcrompathini_flashcardrom, fcrompathini_rompath, "");
						// TODO: Enum values for flash card type.
						if (keepsdvalue == 1) {
							switch (twlsettings_flashcardvalue) {
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
							CIniFile setfcrompathini(sdmc+flashcardfolder+rom);
							switch (twlsettings_flashcardvalue) {
								case 0:
								case 1:
								case 3:
								default: {
									CIniFile fcrompathini("sdmc:/_nds/YSMenu.ini");
									fcrompathini.SetString("YSMENU", "AUTO_BOOT", slashchar+rominini);
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
					}
					SaveSettings();
					screenoff();
					if (twlsettings_rainbowledvalue == 1)
						RainbowLED();
					LogFM("Main.applaunchprep", "Switching to NTR/TWL-mode");
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
					sf2d_free_texture(settingstex);
					dotcircletex = sfil_load_PNG_file(color_data->dotcircleloc, SF2D_PLACE_RAM); // Dots forming a circle
					startbordertex = sfil_load_PNG_file(color_data->startborderloc, SF2D_PLACE_RAM); // "START" border
					bottomtex = sfil_load_PNG_file(bottomloc, SF2D_PLACE_RAM); // Bottom of menu
					colortexloaded_bot = true;
				}
				sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				if (settings_custombotvalue == 1)
					sf2d_draw_texture(bottomtex, 320/2 - bottomtex->width/2, 240/2 - bottomtex->height/2);
				else
					sf2d_draw_texture_blend(bottomtex, 320/2 - bottomtex->width/2, 240/2 - bottomtex->height/2, menucolor);
				
				/* if (romselect_layout == 0) {
					filenameYpos = 0;
					if(files.size() >= 49) {
						for(filenum = 0; filenum < 50; filenum++){
							if(cursorPosition == i) {
								sftd_draw_textf(font, 10, filenameYpos+filenameYmovepos, SET_ALPHA(color_data->color, 255), 12, files.at(i).c_str());
								filenameYpos += 12;
							} else {
								sftd_draw_textf(font, 10, filenameYpos+filenameYmovepos, RGBA8(0, 0, 0, 255), 12, files.at(i).c_str());
								filenameYpos += 12;
							}
						}
					} else {
						for(filenum = 0; filenum < files.size(); filenum++){
							if(cursorPosition == i) {
								sftd_draw_textf(font, 10, filenameYpos+filenameYmovepos, SET_ALPHA(color_data->color, 255), 12, files.at(i).c_str());
								filenameYpos += 12;
							} else {
								sftd_draw_textf(font, 10, filenameYpos+filenameYmovepos, RGBA8(0, 0, 0, 255), 12, files.at(i).c_str());
								filenameYpos += 12;
							}
						}
					}
				} else { */
					if (fadealpha == 0) {
						sf2d_draw_texture(bubbletex, 0, 0);
						// if (dspfirmfound) { sfx_menuselect.play(); }
						if (cursorPosition == -2) {
							// sftd_draw_textf(font, 10, 8, RGBA8(127, 127, 127, 255), 12, "Settings");
							sftd_draw_textf(font_b, 124, 38, RGBA8(0, 0, 0, 255), 18, "Settings");
						} else if (cursorPosition == -1) {
							if (twlsettings_forwardervalue == 1) {
								sftd_draw_textf(font_b, 112, 38, RGBA8(0, 0, 0, 255), 18, "Add games");
							}
						} else {
							if (!bannertextloaded) {
								char path[256];
								if (twlsettings_forwardervalue == 1) {
									if (fcfiles.size() != 0) {
										romsel_filename = fcfiles.at(storedcursorPosition).c_str();
									}
									snprintf(path, sizeof(path), "%s%s.bin", fcbnriconfolder, romsel_filename);
								} else {
									if (files.size() != 0) {
										romsel_filename = files.at(storedcursorPosition).c_str();
									}
									const char *tempfile = files.at(cursorPosition).c_str();
									snprintf(path, sizeof(path), "sdmc:/_nds/twloader/bnricons/%s.bin", tempfile);
								}

								if (access(path, F_OK) == -1) {
									// Banner file is not available.
									strcpy(path, "romfs:/notextbanner");
								}
								bnriconnum = cursorPosition;
								FILE *f_bnr = fopen(path, "rb");
								romsel_gameline = grabText(f_bnr, language);
								fclose(f_bnr);
								bannertextloaded = true;
							}

							int y, dy;
							if (settings_filenamevalue == 1) {
								// TODO: Convert romsel_filename from UTF-8?
								sftd_draw_textf(font, 10, 8, RGBA8(127, 127, 127, 255), 12, romsel_filename);
								y = 24; dy = 19;
							} else {
								y = 16; dy = 22;
							}

							// Print the banner text, center-aligned.
							const size_t banner_lines = std::min(3U, romsel_gameline.size());
							for (size_t i = 0; i < banner_lines; i++, y += dy) {
								int text_width = sftd_get_text_width(font_b, 16, romsel_gameline[i].c_str());
								sftd_draw_textf(font_b, (320-text_width)/2, y, RGBA8(0, 0, 0, 255), 16, romsel_gameline[i].c_str());
							}

							if (settings_countervalue == 1) {
								char romsel_counter1[16];
								snprintf(romsel_counter1, sizeof(romsel_counter1), "%d", storedcursorPosition+1);
								size_t file_count;
								const char *p_romsel_counter;
								if (twlsettings_forwardervalue == 1) {
									file_count = fcfiles.size();
									p_romsel_counter = romsel_counter2fc;
								} else {
									file_count = files.size();
									p_romsel_counter = romsel_counter2sd;
								}
								if (file_count < 100) {
									sftd_draw_textf(font, 8, 96, RGBA8(0, 0, 0, 255), 12, romsel_counter1);
									sftd_draw_textf(font, 27, 96, RGBA8(0, 0, 0, 255), 12, "/");
									sftd_draw_textf(font, 32, 96, RGBA8(0, 0, 0, 255), 12, p_romsel_counter);
								} else {
									sftd_draw_textf(font, 8, 96, RGBA8(0, 0, 0, 255), 12, romsel_counter1);
									sftd_draw_textf(font, 30, 96, RGBA8(0, 0, 0, 255), 12, "/");
									sftd_draw_textf(font, 36, 96, RGBA8(0, 0, 0, 255), 12, p_romsel_counter);
								}
							} else {
								bannertextloaded = true;
							}
						}
					} else {
						sf2d_draw_texture(bottomlogotex, 320/2 - bottomlogotex->width/2, 40);
					}
					sf2d_draw_texture(homeicontex, 81, 220); // Draw HOME icon
					sftd_draw_textf(font, 98, 221, RGBA8(0, 0, 0, 255), 13, text_returntohomemenu());
					sf2d_draw_texture(shoulderYtex, 0, YbuttonYpos);
					sf2d_draw_texture(shoulderXtex, 248, XbuttonYpos);
					if (twlsettings_forwardervalue == 0) {
						if (pagenum != 0) {
							if(files.size() <= 0-pagenum*20) {
								sftd_draw_textf(font, 17, YbuttonYpos+5, RGBA8(0, 0, 0, 255), 11, "Prev");
							} else {
								sftd_draw_textf(font, 17, YbuttonYpos+5, RGBA8(127, 127, 127, 255), 11, "Prev");
							}
						} else {
							sftd_draw_textf(font, 17, YbuttonYpos+5, RGBA8(127, 127, 127, 255), 11, "Prev");
						}
						if(files.size() > 20+pagenum*20) {
							sftd_draw_textf(font, 252, XbuttonYpos+5, RGBA8(0, 0, 0, 255), 11, "Next");
						} else {
							sftd_draw_textf(font, 252, XbuttonYpos+5, RGBA8(127, 127, 127, 255), 11, "Next");
						}
					} else {
						if (pagenum != 0) {
							if(fcfiles.size() <= 0-pagenum*20) {
								sftd_draw_textf(font, 17, YbuttonYpos+5, RGBA8(0, 0, 0, 255), 11, "Prev");
							} else {
								sftd_draw_textf(font, 17, YbuttonYpos+5, RGBA8(127, 127, 127, 255), 11, "Prev");
							}
						} else {
							sftd_draw_textf(font, 17, YbuttonYpos+5, RGBA8(127, 127, 127, 255), 11, "Prev");
						}
						if(fcfiles.size() > 20+pagenum*20) {
							sftd_draw_textf(font, 252, XbuttonYpos+5, RGBA8(0, 0, 0, 255), 11, "Next");
						} else {
							sftd_draw_textf(font, 252, XbuttonYpos+5, RGBA8(127, 127, 127, 255), 11, "Next");
						}
					}
					if (twlsettings_forwardervalue == 0) {
						if (pagenum == 0) {
							sf2d_draw_texture(bracetex, -32+titleboxXmovepos, 116);
							sf2d_draw_texture(settingsboxtex, setsboxXpos+titleboxXmovepos, 119);
							sf2d_draw_texture(carttex, cartXpos+titleboxXmovepos, 120);
							sf2d_draw_texture(iconunktex, 16+cartXpos+titleboxXmovepos, 133);
						} else {
							sf2d_draw_texture(bracetex, 32+cartXpos+titleboxXmovepos, 116);
						}
					} else {
						if (pagenum == 0) {
							sf2d_draw_texture(bracetex, -32+titleboxXmovepos, 116);
							sf2d_draw_texture(settingsboxtex, setsboxXpos+titleboxXmovepos, 119);
							sf2d_draw_texture(getfcgameboxtex, cartXpos+titleboxXmovepos, 119);
						} else {
							sf2d_draw_texture(bracetex, 32+cartXpos+titleboxXmovepos, 116);
						}
					}

					titleboxXpos = 128;
					ndsiconXpos = 144;
					filenameYpos = 0;
					if (twlsettings_forwardervalue == 1) {
						if(fcfiles.size() >= 19+pagenum*20) {
							for (filenum = pagenum*20; filenum < 20+pagenum*20; filenum++) {
								sf2d_draw_texture(boxfulltex, titleboxXpos+titleboxXmovepos, 120);
								titleboxXpos += 64;

								bnriconnum = filenum;
								ChangeBNRIconNo();
								sf2d_draw_texture_part(bnricontexnum, ndsiconXpos+titleboxXmovepos, 133, bnriconframenum*32, 0, 32, 32);
								ndsiconXpos += 64;
							}
						} else {
							for (filenum = pagenum*20; filenum < fcfiles.size(); filenum++){
								sf2d_draw_texture(boxfulltex, titleboxXpos+titleboxXmovepos, 120);
								titleboxXpos += 64;

								bnriconnum = filenum;
								ChangeBNRIconNo();
								sf2d_draw_texture_part(bnricontexnum, ndsiconXpos+titleboxXmovepos, 133, bnriconframenum*32, 0, 32, 32);
								ndsiconXpos += 64;
							}
						}
					} else {
						if(files.size() >= 19+pagenum*20) {
							for (filenum = pagenum*20; filenum < 20+pagenum*20; filenum++) {
								sf2d_draw_texture(boxfulltex, titleboxXpos+titleboxXmovepos, 120);
								titleboxXpos += 64;

								bnriconnum = filenum;
								ChangeBNRIconNo();
								sf2d_draw_texture_part(bnricontexnum, ndsiconXpos+titleboxXmovepos, 133, bnriconframenum*32, 0, 32, 32);
								ndsiconXpos += 64;
							}
						} else {
							for (filenum = pagenum*20; filenum < files.size(); filenum++) {
								sf2d_draw_texture(boxfulltex, titleboxXpos+titleboxXmovepos, 120);
								titleboxXpos += 64;

								bnriconnum = filenum;
								ChangeBNRIconNo();
								sf2d_draw_texture_part(bnricontexnum, ndsiconXpos+titleboxXmovepos, 133, bnriconframenum*32, 0, 32, 32);
								ndsiconXpos += 64;
							}
						} 
					}
					sf2d_draw_texture_scale(bracetex, 15+ndsiconXpos+titleboxXmovepos, 116, -1, 1);
					if (!applaunchprep) {
						if (titleboxXmovetimer == 0) {
							startbordermovepos = 0;
							startborderscalesize = 1.0;
						}
						sf2d_draw_texture_scale(startbordertex, 128+startbordermovepos, 116+startbordermovepos, startborderscalesize, startborderscalesize);
						sftd_draw_textf(font_b, 140, 177, RGBA8(255, 255, 255, 255), 12, "START");
					} else {
						if (settings_custombotvalue == 1)
							sf2d_draw_texture_part(bottomtex, 128, 116, 128, 116, 64, 80);
						else
							sf2d_draw_texture_part_blend(bottomtex, 128, 116, 128, 116, 64, 80, SET_ALPHA(menucolor, 255));  // Cover selected game/app
						if (cursorPosition == -2) {
							sf2d_draw_texture(settingsboxtex, 128, titleboxYmovepos-1); // Draw settings box that moves up
						} else if (cursorPosition == -1) {
							if (twlsettings_forwardervalue == 1)
								sf2d_draw_texture(getfcgameboxtex, 128, titleboxYmovepos-1);
							else {
								sf2d_draw_texture(carttex, 128, titleboxYmovepos); // Draw selected Slot-1 game that moves up
								sf2d_draw_texture(iconunktex, 144, ndsiconYmovepos);
							}
						} else {
							sf2d_draw_texture(boxfulltex, 128, titleboxYmovepos); // Draw selected game/app that moves up
							if (!applaunchicon) {
								bnriconnum = cursorPosition;
								if (twlsettings_forwardervalue == 0) {
									OpenBNRIcon();
									LoadBNRIconatLaunch();
								} else {
									ChangeBNRIconNo();
									bnricontexlaunch = bnricontexnum;
								}
								applaunchicon = true;
							}
							sf2d_draw_texture_part(bnricontexlaunch, 144, ndsiconYmovepos, bnriconframenum*32, 0, 32, 32);
						}
						sf2d_draw_texture_rotate(dotcircletex, 160, 152, rad);  // Dots moving in circles
					}
				// }
			} else if(screenmode == SCREEN_MODE_SETTINGS) {
				if (colortexloaded_bot) {
					sf2d_free_texture(dotcircletex);
					sf2d_free_texture(startbordertex);
					sf2d_free_texture(bottomtex);
					settingstex = sfil_load_PNG_file("romfs:/graphics/settings/screen.png", SF2D_PLACE_RAM); // Bottom of settings screen
					colortexloaded_bot = false;
				}
				sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				sf2d_draw_texture(settingstex, 0, 0);
				sf2d_draw_texture(whomeicontex, 81, 220); // Draw HOME icon
				sftd_draw_textf(font, 98, 221, RGBA8(255, 255, 255, 255), 13, text_returntohomemenu());
				if (settings_subscreenmode == 0) {
					sf2d_draw_texture(shoulderLtex, 0, LshoulderYpos);
					sf2d_draw_texture(shoulderRtex, 248, RshoulderYpos);
					sftd_draw_textf(font, 17, LshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Lshouldertext);
					sftd_draw_textf(font, 252, RshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Rshouldertext);

					// Color text.
					static const char *const color_text[] = {
						"Gray", "Brown", "Red", "Pink",
						"Orange", "Yellow", "Yellow-Green", "Green 1",
						"Green 2", "Light Green", "Sky Blue", "Light Blue",
						"Blue", "Violet", "Purple", "Fuchsia",
						"Red & Blue", "Green & Yellow", "Christmas"
					};
					if (settings_colorvalue < 0 || settings_colorvalue > 18)
						settings_colorvalue = 0;
					settings_colorvaluetext = color_text[settings_colorvalue];

					// Menu color text.
					static const char *const menu_color_text[] = {
						"White", "Black", "Brown", "Red",
						"Pink", "Orange", "Yellow", "Yellow-Green",
						"Green 1", "Green 2", "Light Green", "Sky Blue",
						"Light Blue", "Blue", "Violet", "Purple",
						"Fuchsia"
					};
					if (settings_menucolorvalue < 0 || settings_menucolorvalue > 18)
						settings_menucolorvalue = 0;
					settings_menucolorvaluetext = menu_color_text[settings_menucolorvalue];

					settings_filenamevaluetext = (settings_filenamevalue ? "On" : "Off");
					settings_locswitchvaluetext = (settings_locswitchvalue ? "On" : "Off");
					settings_topbordervaluetext = (settings_topbordervalue ? "On" : "Off");
					settings_countervaluetext = (settings_countervalue ? "On" : "Off");
					settings_custombotvaluetext = (settings_custombotvalue ? "On" : "Off");

					switch (settings_autoupdatevalue) {
						case 0:
						default:
							settings_autoupdatevaluetext = "Off";
							break;
						case 1:
							settings_autoupdatevaluetext = "Release";
							break;
						case 2:
							settings_autoupdatevaluetext = "Unofficial";
							break;
					}
					settings_autodlvaluetext = (settings_autodlvalue ? "On" : "Off");

					settingstext_bot = "Settings: GUI";
					settingsYpos = 40;
					if(settingscursorPosition == 0) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, settings_colortext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, settings_colorvaluetext);
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "The color of the top background,");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "the START border, and the circling dots.");
						settingsYpos += 12;
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_colortext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_colorvaluetext);
						settingsYpos += 12;
					}
					if(settingscursorPosition == 1) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, settings_menucolortext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, settings_menucolorvaluetext);
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "The color of the top border,");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "and the bottom background.");
						settingsYpos += 12;
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_menucolortext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_menucolorvaluetext);
						settingsYpos += 12;
					}
					if(settingscursorPosition == 2) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, settings_filenametext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, settings_filenamevaluetext);
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "Shows game filename at the top of the bubble.");
						settingsYpos += 12;
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_filenametext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_filenamevaluetext);
						settingsYpos += 12;
					}
					if(settingscursorPosition == 3) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, settings_locswitchtext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, settings_locswitchvaluetext);
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "The R button switches the game location");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "between the SD Card and the flashcard.");
						settingsYpos += 12;
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_locswitchtext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_locswitchvaluetext);
						settingsYpos += 12;
					}
					if(settingscursorPosition == 4) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, settings_topbordertext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, settings_topbordervaluetext);
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "The border surrounding the top background.");
						settingsYpos += 12;
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_topbordertext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_topbordervaluetext);
						settingsYpos += 12;
					}
					if(settingscursorPosition == 5) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, settings_countertext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, settings_countervaluetext);
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "A number of selected game and listed games");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "is shown below the text bubble.");
						settingsYpos += 12;
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_countertext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_countervaluetext);
						settingsYpos += 12;
					}
					if(settingscursorPosition == 6) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, settings_custombottext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, settings_custombotvaluetext);
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "Loads a custom bottom screen image");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "for the game menu.");
						settingsYpos += 12;
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_custombottext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_custombotvaluetext);
						settingsYpos += 12;
					}
					if(settingscursorPosition == 7) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, settings_autoupdatetext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, settings_autoupdatevaluetext);
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "Auto-update nds-bootstrap at launch.");
						settingsYpos += 12;
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_autoupdatetext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_autoupdatevaluetext);
						settingsYpos += 12;
					}
					if(settingscursorPosition == 8) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, settings_autodltext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, settings_autodlvaluetext);
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "Auto-download the CIA of the latest");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "TWLoader version at launch.");
						settingsYpos += 12;
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_autodltext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_autodlvaluetext);
						settingsYpos += 12;
					}
				} else if (settings_subscreenmode == 1) {
					sf2d_draw_texture(shoulderLtex, 0, LshoulderYpos);
					sf2d_draw_texture(shoulderRtex, 248, RshoulderYpos);
					sftd_draw_textf(font, 17, LshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Lshouldertext);
					sftd_draw_textf(font, 252, RshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Rshouldertext);

					twlsettings_rainbowledvaluetext = (twlsettings_rainbowledvalue ? "On" : "Off");
					twlsettings_cpuspeedvaluetext = (twlsettings_cpuspeedvalue ? "133mhz (TWL)" : "67mhz (NTR)");
					twlsettings_extvramvaluetext = (twlsettings_extvramvalue ? "On" : "Off");
					twlsettings_bootscreenvaluetext = (twlsettings_bootscreenvalue ? "On" : "Off");
					twlsettings_healthsafetyvaluetext = (twlsettings_healthsafetyvalue ? "On" : "Off");
					twlsettings_resetslot1valuetext = (twlsettings_resetslot1value ? "On" : "Off");

					switch (twlsettings_consolevalue) {
						case 0:
						default:
							twlsettings_consolevaluetext = "Off";
							break;
						case 1:
							twlsettings_consolevaluetext = "On";
							break;
						case 2:
							twlsettings_consolevaluetext = "On (Debug)";
							break;
					}

					twlsettings_lockarm9scfgextvaluetext = (twlsettings_lockarm9scfgextvalue ? "On" : "Off");

					settingstext_bot = "Settings: NTR/TWL-mode";
					settingsYpos = 40;
					if(twlsettingscursorPosition == 0) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, twlsettings_flashcardtext);
						settingsYpos += 12;
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "Pick a flashcard to use to");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "run ROMs from it.");
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_flashcardtext);
						settingsYpos += 12;
					}
					if(twlsettingscursorPosition == 1) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, twlsettings_rainbowledtext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, twlsettings_rainbowledvaluetext);
						settingsYpos += 12;
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "See rainbow colors glowing in");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "the Notification LED.");
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_rainbowledtext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_rainbowledvaluetext);
						settingsYpos += 12;
					}
					if(twlsettingscursorPosition == 2) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, twlsettings_cpuspeedtext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, twlsettings_cpuspeedvaluetext);
						settingsYpos += 12;
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "Set to TWL to get rid of lags in some games.");
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_cpuspeedtext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_cpuspeedvaluetext);
						settingsYpos += 12;
					}
					if(twlsettingscursorPosition == 3) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, twlsettings_extvramtext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, twlsettings_extvramvaluetext);
						settingsYpos += 12;
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "Allows 8 bit VRAM writes");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "and expands the bus to 32 bit.");
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_extvramtext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_extvramvaluetext);
						settingsYpos += 12;
					}
					if(twlsettingscursorPosition == 4) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, twlsettings_bootscreentext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, twlsettings_bootscreenvaluetext);
						settingsYpos += 12;
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "Displays the DS/DSi boot animation");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "before launched game.");
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_bootscreentext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_bootscreenvaluetext);
						settingsYpos += 12;
					}
					if(twlsettingscursorPosition == 5) {
						sftd_draw_textf(font, settingsXpos+16, settingsYpos, SET_ALPHA(color_data->color, 255), 12, twlsettings_healthsafetytext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, twlsettings_healthsafetyvaluetext);
						settingsYpos += 12;
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "Displays the Health and Safety");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "message on the bottom screen.");
					} else {
						sftd_draw_textf(font, settingsXpos+16, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_healthsafetytext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_healthsafetyvaluetext);
						settingsYpos += 12;
					}
					if(twlsettingscursorPosition == 6) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, twlsettings_resetslot1text);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, twlsettings_resetslot1valuetext);
						settingsYpos += 12;
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "Enable this if Slot-1 carts are stuck");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "on white screens.");
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_resetslot1text);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_resetslot1valuetext);
						settingsYpos += 12;
					}
					if(twlsettingscursorPosition == 7) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, twlsettings_consoletext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, twlsettings_consolevaluetext);
						settingsYpos += 12;
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "Displays some text before launched game.");
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_consoletext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_consolevaluetext);
						settingsYpos += 12;
					}
					if(twlsettingscursorPosition == 8) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, twlsettings_lockarm9scfgexttext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, SET_ALPHA(color_data->color, 255), 12, twlsettings_lockarm9scfgextvaluetext);
						settingsYpos += 12;
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "Locks the ARM9 SCFG_EXT,");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "avoiding conflict with recent libnds.");
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_lockarm9scfgexttext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_lockarm9scfgextvaluetext);
						settingsYpos += 12;
					}
				} else if (settings_subscreenmode == 2) {
					// Flash card options.
					static const char *const flash_card_options[][6] = {
						{"DSTT", "R4i Gold", "R4i-SDHC (Non-v1.4.x version) (www.r4i-sdhc.com)",
						 "R4 SDHC Dual-Core", "R4 SDHC Upgrade", "SuperCard DSONE"},
						{"Original R4", "M3 Simply", " ", " ", " ", " "},
						{"R4iDSN", "R4i Gold RTS", " ", " ", " ", " "},
						{"Acekard 2(i)", "M3DS Real", " ", " ", " ", " "},
						{"Acekard RPG", " ", " ", " ", " ", " "},
						{"Ace 3DS+", "Gateway Blue Card", "R4iTT", " ", " ", " "},
						{"SuperCard DSTWO", " ", " ", " ", " ", " "},
					};

					if (twlsettings_flashcardvalue < 0 || twlsettings_flashcardvalue > 6) {
						twlsettings_flashcardvalue = 0;
					}
					const char *const *fctext = flash_card_options[twlsettings_flashcardvalue];
					settingstext_bot = twlsettings_flashcardtext;
					settingsYpos = 40;
					for (int i = 0; i < 6; i++, settingsYpos += 12) {
						sftd_draw_textf(font, settingsXpos, settingsYpos,
							SET_ALPHA(color_data->color, 255), 12,
							flash_card_options[twlsettings_flashcardvalue][i]);
					}
					sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, settings_lrpicktext);
					sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, settings_absavereturn);
				}
				sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 16, settingstext_bot);
			}
		sf2d_draw_rectangle(0, 0, 320, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect

		sf2d_end_frame();
		// }
		
		sf2d_swapbuffers();


		if (titleboxXmovetimer == 0) {
			updatebotscreen = false;
		}
		if (screenmode == SCREEN_MODE_ROM_SELECT) {
			Lshouldertext = (romselect_toplayout ? "Box Art" : "Blank");
			Rshouldertext = (twlsettings_forwardervalue ? "SD Card" : "Flashcard");
			/* if (filenum == 0) {	// If no ROMs are found
				romselect_layout = 1;
				updatebotscreen = true;
			} */
			if(hDown & KEY_L) {
				romselect_toplayout = !romselect_toplayout;
				if (dspfirmfound) {
					sfx_switch.stop();	// Prevent freezing
					sfx_switch.play();
				}
			}
			/* if (romselect_layout == 0) {
				Rshouldertext = "DSi-Menu";
				if(cursorPosition == -1) {
					filenameYmovepos = 0;
					titleboxXmovepos -= 64;
					boxartXmovepos -= 18*8;
					cursorPosition = 0;
					updatebotscreen = true;
				}
				if(hDown & KEY_R) {
					romselect_layout = 1;
					updatebotscreen = true;
				} else if(hDown & KEY_A){
					twlsettings_launchslot1value = 0;
					screenoff();
					rom = files.at(cursorPosition).c_str();
					SaveSettings();
					applaunchon = true;
					updatebotscreen = true;
				} else if(hDown & KEY_DOWN){
					if (cursorPosition > 7) {
						filenameYmovepos -= 12;
					}
					titleboxXmovepos -= 64;
					boxartXmovepos -= 18*8;
					cursorPosition++;
					if (cursorPosition == i) {
						titleboxXmovepos = 0;
						boxartXmovepos = 0;
						filenameYmovepos = 0;
						cursorPosition = 0;
					}
					updatebotscreen = true;
				} else if((hDown & KEY_UP) && (i > 1)){
					if (cursorPosition > 8) {
						filenameYmovepos += 12;
					}
					titleboxXmovepos += 64;
					boxartXmovepos += 18*8;
					if (cursorPosition == 0) {
						titleboxXmovepos = 0;
						boxartXmovepos -= 64*i-64;
						filenameYmovepos -= 12*i-12*9;
						cursorPosition = i;
					}
						cursorPosition--;
					updatebotscreen = true;
				} else if(hDown & KEY_X) {
					twlsettings_launchslot1value = 1;
					screenoff();
					SaveSettings();
					applaunchon = true;
					updatebotscreen = true;
				} else if (hDown & KEY_SELECT) {
					screenmode = SCREEN_MODE_SETTINGS;
					updatebotscreen = true;
				}
			} else { */
				startbordermovepos = 0;
				startborderscalesize = 1.0;
				if(!applaunchprep || fadealpha == 255) {
					if (settings_locswitchvalue == 1) {
						if(hDown & KEY_R) {
							pagenum = 0;
							bannertextloaded = false;
							cursorPosition = 0;
							storedcursorPosition = cursorPosition;
							titleboxXmovepos = 0;
							boxartXmovepos = 0;
							noromsfound = false;
							twlsettings_forwardervalue = !twlsettings_forwardervalue;
							bnricontexloaded = false;
							boxarttexloaded = false;
							if (dspfirmfound) {
								sfx_switch.stop();	// Prevent freezing
								sfx_switch.play();
							}
							updatebotscreen = true;
						}
					}
					if (!noromsfound) {
						if (twlsettings_forwardervalue == 1) {	// If no ROMs are found
							if (fcfiles.size() == 0) {	// If no ROMs are found
								cursorPosition = -1;
								storedcursorPosition = cursorPosition;
								titleboxXmovepos = +64;
								boxartXmovepos = 0;
							}
						} else {
							if (files.size() == 0) {	// If no ROMs are found
								cursorPosition = -1;
								storedcursorPosition = cursorPosition;
								titleboxXmovepos = +64;
								boxartXmovepos = 0;
							}
						}
						updatebotscreen = true;
						noromsfound = true;
					}
					if(hDown & KEY_X) {
						if (twlsettings_forwardervalue == 1) {
							if(fcfiles.size() > 20+pagenum*20) {
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
									sfx_switch.stop();	// Prevent freezing
									sfx_switch.play();
								}
								updatebotscreen = true;
							}
						} else {
							if(files.size() > 20+pagenum*20) {
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
									sfx_switch.stop();	// Prevent freezing
									sfx_switch.play();
								}
								updatebotscreen = true;
							}
						}
					} else if(hDown & KEY_Y) {
						if (pagenum != 0) {
							if (twlsettings_forwardervalue == 1) {
								if(fcfiles.size() <= 0-pagenum*20) {
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
										sfx_switch.stop();	// Prevent freezing
										sfx_switch.play();
									}
									updatebotscreen = true;
								}
							} else {
								if(files.size() <= 0-pagenum*20) {
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
										sfx_switch.stop();	// Prevent freezing
										sfx_switch.play();
									}
									updatebotscreen = true;
								}
							}
						}
					}
					if(hDown & KEY_TOUCH){
						hidTouchRead(&touch);
						touch_x = touch.px;
						touch_y = touch.py;
						if (touch_x <= 72 && touch_y >= YbuttonYpos) {		// Also for Y button
							if (pagenum != 0) {
								if (twlsettings_forwardervalue == 1) {
									if(fcfiles.size() <= 0-pagenum*20) {
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
											sfx_switch.stop();	// Prevent freezing
											sfx_switch.play();
										}
										updatebotscreen = true;
									}
								} else {
									if(files.size() <= 0-pagenum*20) {
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
											sfx_switch.stop();	// Prevent freezing
											sfx_switch.play();
										}
										updatebotscreen = true;
									}
								}
							}
						} else if (touch_x >= 248 && touch_y >= XbuttonYpos) {
							if (twlsettings_forwardervalue == 1) {
								if(fcfiles.size() > 20+pagenum*20) {
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
										sfx_switch.stop();	// Prevent freezing
										sfx_switch.play();
									}
									updatebotscreen = true;
								}
							} else {
								if(files.size() > 20+pagenum*20) {
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
										sfx_switch.stop();	// Prevent freezing
										sfx_switch.play();
									}
									updatebotscreen = true;
								}
							}
						} else if (touch_x >= 128 && touch_x <= 192 && touch_y >= 112 && touch_y <= 192) {
							if (titleboxXmovetimer == 0) {
								if(cursorPosition == -2) {
									titleboxXmovetimer = 1;
									screenmodeswitch = true;
									applaunchprep = true;
								} else if(cursorPosition == -1) {
									titleboxXmovetimer = 1;
									if (twlsettings_forwardervalue == 1) {
										keepsdvalue = 1;
										rom = "_nds/twloader.nds";
									} else
										twlsettings_launchslot1value = 1;
									applaunchprep = true;
								} else {
									titleboxXmovetimer = 1;
									if (twlsettings_forwardervalue == 1) {
										twlsettings_launchslot1value = 1;
										rom = fcfiles.at(cursorPosition).c_str();
									} else {
										twlsettings_launchslot1value = 0;
										rom = files.at(cursorPosition).c_str();
										// FIXME: Potential memory leak.
										sav = strrep(rom, ".nds", ".sav");
									}
									applaunchprep = true;
								}
							}
							updatebotscreen = true;
							if (dspfirmfound) {
								bgm_menu.stop();
								sfx_launch.play();
							}
						} else if (touch_x < 128 && touch_y >= 118 && touch_y <= 180) {
							//titleboxXmovepos -= 64;
							if (!titleboxXmoveright) {
								titleboxXmoveleft = true;
							}
							updatebotscreen = true;
						} else if (touch_x > 192 && touch_y >= 118 && touch_y <= 180) {
							//titleboxXmovepos -= 64;
							if (!titleboxXmoveleft) {
								if (cursorPosition == -1) {
									if (filenum == 0) {
										if (!playwrongsounddone) {
											if (dspfirmfound) {
												sfx_wrong.stop();
												sfx_wrong.play();
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
							updatebotscreen = true;
						}
					} else if(hDown & KEY_A){
						if (titleboxXmovetimer == 0) {
							if(cursorPosition == -2) {
								titleboxXmovetimer = 1;
								screenmodeswitch = true;
								applaunchprep = true;
							} else if(cursorPosition == -1) {
								titleboxXmovetimer = 1;
								if (twlsettings_forwardervalue == 1) {
									keepsdvalue = 1;
									rom = "_nds/twloader.nds";
								} else
									twlsettings_launchslot1value = 1;
								applaunchprep = true;
							} else {
								titleboxXmovetimer = 1;
								if (twlsettings_forwardervalue == 1) {
									twlsettings_launchslot1value = 1;
									rom = fcfiles.at(cursorPosition).c_str();
								} else {
									twlsettings_launchslot1value = 0;
									rom = files.at(cursorPosition).c_str();
									// FIXME: Potential memory leak.
									sav = strrep(rom, ".nds", ".sav");
								}
								applaunchprep = true;
							}
						}
						updatebotscreen = true;
						if (dspfirmfound) {
							bgm_menu.stop();
							sfx_launch.play();
						}
					} else if(hHeld & KEY_RIGHT){
						//titleboxXmovepos -= 64;
						if (!titleboxXmoveleft) {
							if (cursorPosition == -1) {
								if (filenum == 0) {
									if (!playwrongsounddone) {
										if (dspfirmfound) {
											sfx_wrong.stop();
											sfx_wrong.play();
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
						updatebotscreen = true;
					} else if(hHeld & KEY_LEFT){
						//titleboxXmovepos += 64;
						if (!titleboxXmoveright) {
							titleboxXmoveleft = true;
						}
						updatebotscreen = true;
					} else if (hDown & KEY_SELECT) {
						if (titleboxXmovetimer == 0) {
							titleboxXmovetimer = 1;
							romfolder = "_nds/";
							rom = "GBARunner2.nds";
							if (twlsettings_forwardervalue == 1) {
								twlsettings_launchslot1value = 1;
							} else {
								twlsettings_launchslot1value = 0;
							}
							fadeout = true;
							updatebotscreen = true;
							if (dspfirmfound) {
								bgm_menu.stop();
								sfx_launch.play();
							}
						}
						
					}
				}
			//}
		} else if (screenmode == SCREEN_MODE_SETTINGS) {
			Lshouldertext = "GUI";
			Rshouldertext = "NTR/TWL";
			updatebotscreen = true;
			if (settings_subscreenmode == 2) {
				if(hDown & KEY_LEFT && twlsettings_flashcardvalue != 0){
					twlsettings_flashcardvalue--; // Flashcard
					if (dspfirmfound) { sfx_select.play(); }
				} else if(hDown & KEY_RIGHT && twlsettings_flashcardvalue != 6){
					twlsettings_flashcardvalue++; // Flashcard
					if (dspfirmfound) { sfx_select.play(); }
				} else if(hDown & KEY_A || hDown & KEY_B){
					settings_subscreenmode = 1;
					if (dspfirmfound) { sfx_select.play(); }
				}
			} else if (settings_subscreenmode == 1) {
				if(hDown & KEY_A){
					if (twlsettingscursorPosition == 0) {
						settings_subscreenmode = 2;
					} else if (twlsettingscursorPosition == 1) {
						twlsettings_rainbowledvalue++; // Rainbow LED
						if(twlsettings_rainbowledvalue == 2) {
							twlsettings_rainbowledvalue = 0;
						}
					} else if (twlsettingscursorPosition == 2) {
						twlsettings_cpuspeedvalue++; // CPU speed
						if(twlsettings_cpuspeedvalue == 2) {
							twlsettings_cpuspeedvalue = 0;
						}
					} else if (twlsettingscursorPosition == 3) {
						twlsettings_extvramvalue++; // VRAM boost
						if(twlsettings_extvramvalue == 2) {
							twlsettings_extvramvalue = 0;
						}
					} else if (twlsettingscursorPosition == 4) {
						twlsettings_bootscreenvalue++; // Boot screen
						if(twlsettings_bootscreenvalue == 2) {
							twlsettings_bootscreenvalue = 0;
						}
					} else if (twlsettingscursorPosition == 5) {
						twlsettings_healthsafetyvalue++; // H&S message
						if(twlsettings_healthsafetyvalue == 2) {
							twlsettings_healthsafetyvalue = 0;
						}
					} else if (twlsettingscursorPosition == 6) {
						twlsettings_resetslot1value++; // Reset Slot-1
						if(twlsettings_resetslot1value == 2) {
							twlsettings_resetslot1value = 0;
						}
					} else if (twlsettingscursorPosition == 7) {
						twlsettings_consolevalue++; // Console output
						if(twlsettings_consolevalue == 3) {
							twlsettings_consolevalue = 0;
						}
					} else if (twlsettingscursorPosition == 8) {
						twlsettings_lockarm9scfgextvalue++; // Lock ARM9 SCFG_EXT
						if(twlsettings_lockarm9scfgextvalue == 2) {
							twlsettings_lockarm9scfgextvalue = 0;
						}
					}
					if (dspfirmfound) { sfx_select.play(); }
				} else if((hDown & KEY_DOWN) && twlsettingscursorPosition != 8){
					twlsettingscursorPosition++;
					if (dspfirmfound) { sfx_select.play(); }
				} else if((hDown & KEY_UP) && twlsettingscursorPosition != 0){
					twlsettingscursorPosition--;
					if (dspfirmfound) { sfx_select.play(); }
				} else if(hDown & KEY_L){
					settings_subscreenmode = 0;
					if (dspfirmfound) {
						sfx_switch.stop();	// Prevent freezing
						sfx_switch.play();
					}
				} else if(hDown & KEY_B){
					titleboxXmovetimer = 1;
					fadeout = true;
					if (dspfirmfound) {
						// bgm_settings.stop();
						sfx_back.play();
					}
				}
			} else {
				if(hDown & KEY_A || hDown & KEY_RIGHT){
					if (settingscursorPosition == 0) {
						settings_colorvalue++; // Color
						if(settings_colorvalue == 19) {
							settings_colorvalue = 0;
						}
						LoadColor();
					} else if (settingscursorPosition == 1) {
						settings_menucolorvalue++; // Menu color
						if(settings_menucolorvalue == 17) {
							settings_menucolorvalue = 0;
						}
						LoadMenuColor();
					} else if (settingscursorPosition == 2) {
						settings_filenamevalue++; // Show filename
						if(settings_filenamevalue == 2) {
							settings_filenamevalue = 0;
						}
					} else if (settingscursorPosition == 3) {
						settings_locswitchvalue++; // Game location switcher
						if(settings_locswitchvalue == 2) {
							settings_locswitchvalue = 0;
						}
					} else if (settingscursorPosition == 4) {
						settings_topbordervalue++; // Top border
						if(settings_topbordervalue == 2) {
							settings_topbordervalue = 0;
						}
					} else if (settingscursorPosition == 5) {
						settings_countervalue++; // Game counter
						if(settings_countervalue == 2) {
							settings_countervalue = 0;
						}					
					} else if (settingscursorPosition == 6) {
						settings_custombotvalue++; // Custom bottom image
						if(settings_custombotvalue == 2) {
							settings_custombotvalue = 0;
						}
						LoadBottomImage();
					} else if (settingscursorPosition == 7) {
						settings_autoupdatevalue++; // Enable or disable autoupdate
						if(settings_autoupdatevalue == 3) {
							settings_autoupdatevalue = 0;
						}
					} else if (settingscursorPosition == 8) {
						settings_autodlvalue++; // Enable or disable autodownload
						if(settings_autodlvalue == 2) {
							settings_autodlvalue = 0;
						}
					}
					if (dspfirmfound) { sfx_select.play(); }
				} if(hDown & KEY_LEFT){
					if (settingscursorPosition == 0) {
						settings_colorvalue--; // Color
						if(settings_colorvalue == -1) {
							settings_colorvalue = 18;
						}
						LoadColor();
						if (dspfirmfound) { sfx_select.play(); }
					} else if (settingscursorPosition == 1) {
						settings_menucolorvalue--; // Menu color
						if(settings_menucolorvalue == -1) {
							settings_menucolorvalue = 16;
						}
						LoadMenuColor();
					}
				} else if((hDown & KEY_DOWN) && settingscursorPosition != 8){
					settingscursorPosition++;
					if (dspfirmfound) { sfx_select.play(); }
				} else if((hDown & KEY_UP) && settingscursorPosition != 0){
					settingscursorPosition--;
					if (dspfirmfound) { sfx_select.play(); }
				} else if(hDown & KEY_R){
					settings_subscreenmode = 1;
					if (dspfirmfound) {
						sfx_switch.stop();	// Prevent freezing
						sfx_switch.play();
					}
				} else if(hDown & KEY_X && checkWifiStatus()){
					UpdateBootstrapRelease();
				} else if(hDown & KEY_Y && checkWifiStatus()){
					UpdateBootstrapUnofficial();
				} else if(hDown & KEY_START && checkWifiStatus() && (checkUpdate() == 0)){
					DownloadTWLoaderCIAs();
				} else if(hDown & KEY_B){
					titleboxXmovetimer = 1;
					fadeout = true;
					if (dspfirmfound) {
						// bgm_settings.stop();
						sfx_back.play();
					}
				}
			}
		}

		while(applaunchon){
			// Prepare for the app launch
			APT_PrepareToDoApplicationJump(0, 0x0004800554574C44LL, 0); // TWLNAND side's title ID
			// Tell APT to trigger the app launch and set the status of this app to exit
			APT_DoApplicationJump(param, sizeof(param), hmac);
		}
	//}	// run
	}	// aptMainLoop

	SaveSettings();

	hidExit();
	srvExit();
	romfsExit();
	sdmcExit();
	ptmuxExit();
	ptmuExit();
	amExit();
	cfguExit();
	aptExit();

	if (colortexloaded) { sf2d_free_texture(topbgtex); }
	sf2d_free_texture(toptex);
	for (int i = 0; i < 5; i++) {
		sf2d_free_texture(voltex[i]);
	}
	if (screenmode == SCREEN_MODE_SETTINGS) {
		for (int i = 0; i < 5; i++) {
			sf2d_free_texture(setvoltex[i]);
		}
	}
	sf2d_free_texture(shoulderLtex);
	sf2d_free_texture(shoulderRtex);
	sf2d_free_texture(batterychrgtex);
	for (int i = 0; i < 6; i++) {
		sf2d_free_texture(batterytex[i]);
	}
	if (screenmode == SCREEN_MODE_SETTINGS) {
		sf2d_free_texture(setbatterychrgtex);
		for (int i = 0; i < 6; i++) {
			sf2d_free_texture(setbatterytex[i]);
		}
	}
	if (colortexloaded) { sf2d_free_texture(bottomtex); }
	sf2d_free_texture(iconunktex);
	sf2d_free_texture(homeicontex);
	sf2d_free_texture(whomeicontex);
	sf2d_free_texture(bottomlogotex);
	sf2d_free_texture(bubbletex);
	sf2d_free_texture(settingsboxtex);
	sf2d_free_texture(carttex);
	sf2d_free_texture(boxfulltex);
	if (colortexloaded) { sf2d_free_texture(dotcircletex); }
	if (colortexloaded) { sf2d_free_texture(startbordertex); }
	if (screenmode == SCREEN_MODE_SETTINGS) {
		sf2d_free_texture(settingstex);
		sf2d_free_texture(settingslogotex);
		sf2d_free_texture(dsboottex);
		sf2d_free_texture(dsiboottex);
		sf2d_free_texture(dshstex);
		sf2d_free_texture(dsihstex);
		sf2d_free_texture(disabledtex);
	}

	// Launch banner.
	sf2d_free_texture(bnricontexlaunch);

	// Free the arrays.
	for (int i = 0; i < 20; i++) {
		if (ndsFile[i]) {
			fclose(ndsFile[i]);
		}
		free(bnriconpath[i]);
		sf2d_free_texture(bnricontex[i]);
		free(boxartpath[i]);
	}
	for (int i = 0; i < 6; i++) {
		sf2d_free_texture(boxarttex[i]);
	}

	if (dspfirmfound) { ndspExit(); }
	sftd_free_font(font);
	sftd_free_font(font_b);
	sf2d_fini();

	return 0;
}
