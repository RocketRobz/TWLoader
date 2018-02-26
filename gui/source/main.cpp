// for strcasestr()
#define _GNU_SOURCE 1
#include "main.h"

#include "thread.h"
#include "dumpdsp.h"
#include "dsbootsplash.h"
#include "download.h"
#include "settings.h"
#include "textfns.h"
#include "language.h"
#include "gamecard.h"
#include "spi.h"
#include "archive.h"
#include "fsstream.h"
#include "title.h"
#include "stringutil.h"
#include "rmkdir.h"

#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <3ds.h>

#include "ptmu_x.h"

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

bool isDemo = COMPILE_3DSX;
bool isNightly = IS_NIGHTLY;

static touchPosition touch;

bool menuaction_nextpage = false;
bool menuaction_prevpage = false;
bool menuaction_launch = false;
bool menudboxaction_switchloc = false;
bool menudboxaction_changeromtype = false;
bool buttondeletegame = false;
bool addgametodeletequeue = false;

CIniFile bootstrapini( "sdmc:/_nds/nds-bootstrap.ini" );
#include "ndsheaderbanner.h"

int equals;

std::string	bootstrapPath;

// Current screen mode.
ScreenMode screenmode = SCREEN_MODE_ROM_SELECT;

int TWLNANDnotfound_msg = 2;

/* enum RomSelect_Mode {	// R4 Theme only
	ROMSEL_MODE_MENU = 0,	// Menu
	ROMSEL_MODE_ROMSEL = 1,	// ROM Select
};
RomSelect_Mode menudboxmode = DBOX_MODE_OPTIONS; */

enum MenuDBox_Mode {
	DBOX_MODE_OPTIONS = 0,			// Options
	DBOX_MODE_ROMTYPE = 1,			// Select ROM type
	DBOX_MODE_SETTINGS = 2,			// Game Settings
	DBOX_MODE_DELETE = 3,			// Delete confirmation
	DBOX_MODE_DELETED = 4,			// Title deleted message
	DBOX_MODE_OVERLAYS = 5,			// Overlays included message
	DBOX_MODE_DONOR_NOT_SET = 6,	// Donor ROM not set message
};
MenuDBox_Mode menudboxmode = DBOX_MODE_OPTIONS;

enum Menu_ControlSet {
	CTRL_SET_MENU = 0,		// Menu (R4 Theme only)
	CTRL_SET_GAMESEL = 1,	// ROM Select
	CTRL_SET_ROMTYPE = 2,	// ROM type select
	CTRL_SET_DBOX = 3,		// Dialog box mode
};
Menu_ControlSet menu_ctrlset = CTRL_SET_GAMESEL;


int bnriconnum = 0;
int loadbnriconnum = 0;
int boxartnum = 0;
int boxartpage = 0;
int loadboxartnum = 0;
const char* temptext;
const char* musicpath = "romfs:/null.wav";

const char* Lshouldertext = "";
const char* Rshouldertext = "";

u32 SDKVersion = 0;
const char* SDKnumbertext = "";

int LshoulderYpos = 220;
int RshoulderYpos = 220;

int text_width = 0;

// Sound effects.
sound *bgm_menu = NULL;
//sound *bgm_settings = NULL;
sound *sfx_launch = NULL;
sound *sfx_select = NULL;
sound *sfx_stop = NULL;
sound *sfx_switch = NULL;
sound *sfx_wrong = NULL;
sound *sfx_back = NULL;

//Banners and boxart. (formerly bannerandboxart.h)
// bnricontex[]: 0-19; 20 is for R4 theme only
// boxartpath[]: 0-19; 20 is for blank boxart only
static char* bnriconpath[gamesPerPage+1] = { };
static char* boxartpath[gamesPerPage+1] = { };

// Title box animation.
static int titleboxXpos = 0;
static int titleboxXmovepos = 0;
static float scrollwindowXmovepos = 0.0;
static bool titleboxXmoveleft = false;
static bool titleboxXmoveright = false;
static int titleboxYmovepos = 116;
int titleboxXmovetimer = 1; // Set to 1 for fade-in effect to run

static bool wood_uppressed = false;
static bool wood_downpressed = false;

static const char fcrompathini_flashcardrom[] = "FLASHCARD-ROM";
static const char fcrompathini_rompath[] = "NDS_PATH";
static const char fcrompathini_tid[] = "TID";
static const char fcrompathini_bnriconaniseq[] = "BNR_ICONANISEQ";	

// Bootstrap .ini file
static const char bootstrapini_ndspath[] = "NDS_PATH";
static const char bootstrapini_savpath[] = "SAV_PATH";
static const char bootstrapini_bootstrappath[] = "BOOTSTRAP_PATH";
static const char bootstrapini_arm7donorpath[] = "ARM7_DONOR_PATH";
static const char bootstrapini_loadingscreen[] = "LOADING_SCREEN";
static const char bootstrapini_boostcpu[] = "BOOST_CPU";
static const char bootstrapini_boostvram[] = "BOOST_VRAM";
static const char bootstrapini_bootsplash[] = "BOOTSPLASH";
static const char bootstrapini_debug[] = "DEBUG";
static const char bootstrapini_resetslot1[] = "RESETSLOT1";
static const char bootstrapini_lockarm9scfgext[] = "LOCK_ARM9_SCFG_EXT";
static const char bootstrapini_usearm7donor[] = "USE_ARM7_DONOR";
static const char bootstrapini_mpuregion[] = "PATCH_MPU_REGION";
static const char bootstrapini_mpusize[] = "PATCH_MPU_SIZE";
static const char bootstrapini_ndsbootstrap[] = "NDS-BOOTSTRAP";
// End

bool run = true;

bool showdialogbox = false;
bool showdialogbox_menu = false;	// for Game Select menu
int menudbox_movespeed = 22;
int menudbox_Ypos = -240;
int menudbox_bgalpha = 0;

const char* noromtext1;
const char* noromtext2;

int RGB[3]; // Pergame LED

bool usepergamesettings = true;

// Version numbers.
char settings_vertext[13];

std::string settings_releasebootstrapver;
std::string settings_unofficialbootstrapver;
std::string settings_SDK5releasebootstrapver;
std::string settings_SDK5unofficialbootstrapver;

static bool applaunchprep = false;
static bool launchCia = false;
static bool applaunchon = false;

bool showbubble = false;

int fadealpha = 255;
bool fadein = true;
bool fadeout = false;

static const char* romsel_filename;
static wstring romsel_filename_w;	// Unicode filename for display.
static vector<wstring> romsel_gameline;	// from banner

static const char* rom = "";		// Selected ROM image.
std::string sav;		// Associated save file.

std::string homebrew_arg = "";

// These are used by flashcard functions and must retain their trailing slash.
static const std::string sdmc = "sdmc:/";
std::string fat = "sd:/";
static const std::string slashchar = "/";
static const std::string woodfat = "fat0:/";
static const std::string dstwofat = "fat1:/";

static const char bnriconfolder[] = "sdmc:/_nds/twloader/bnricons";
static const char fcbnriconfolder[] = "sdmc:/_nds/twloader/bnricons/flashcard";
static const char boxartfolder[] = "sdmc:/_nds/twloader/boxart";
static const char fcboxartfolder[] = "sdmc:/_nds/twloader/boxart/flashcard";
static const char gbboxartfolder[] = "sdmc:/_nds/twloader/boxart/gb";
static const char nesboxartfolder[] = "sdmc:/_nds/twloader/boxart/nes";
static const char slot1boxartfolder[] = "sdmc:/_nds/twloader/boxart/slot1";
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


static bool screenoff_ran = false;
static bool screenon_ran = true;

void screenoff(void)
{
	screenon_ran = false;
	if(!screenoff_ran) {
		if (R_SUCCEEDED(gspLcdInit())) {
			GSPLCD_PowerOffBacklight(GSPLCD_SCREEN_BOTH);
			gspLcdExit();
		}
		screenoff_ran = true;
	}
}

void screenon(void)
{
 	screenoff_ran = false;
	if(!screenon_ran) {
		if (R_SUCCEEDED(gspLcdInit())) {
			GSPLCD_PowerOnBacklight(GSPLCD_SCREEN_BOTH);
			gspLcdExit();
		}
		screenon_ran = true;
	}
}

void botscreenoff(void)
{
	if (R_SUCCEEDED(gspLcdInit())) {
		GSPLCD_PowerOffBacklight(GSPLCD_SCREEN_BOTTOM);
		gspLcdExit();
	}
}

void botscreenon(void)
{
	if (R_SUCCEEDED(gspLcdInit())) {
		GSPLCD_PowerOnBacklight(GSPLCD_SCREEN_BOTTOM);
		gspLcdExit();
	}
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

bool checkDSiWareInstalled(u64 tid) {
    AM_TitleEntry entry;
    return R_SUCCEEDED(AM_GetTitleInfo(MEDIATYPE_NAND, 1, &tid, &entry));
}

u64 getTIDFromCIA(char* path) {
    AM_TitleEntry titleEntry;
    Handle handle;
    u64 res = 0;
    
    FS_Path ciaPath = fsMakePath(PATH_ASCII, path);
	FSUSER_OpenFileDirectly(&handle, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""), ciaPath, FS_OPEN_READ, 0);
    AM_GetCiaFileInfo(MEDIATYPE_NAND, &titleEntry, handle);
    
    res = titleEntry.titleID;
    
    FSFILE_Close(handle);
    
    return res;
}

/**
 * @brief Manage cia install or uninstall
 * @param install true is install, false to uninstall
 * @param mediaType, MEDIA_SD_CIA or MEDIA_NAND_CIA
 */
void TWLoader_cia_manager(bool install, MediaType mediaType, const char* filename)
{
    if (install) {
        if (logEnabled) LogFMA("TWLoader_cia_manager", "Installing", filename);
        Handle handle;
        long fileSize;
        char* buf;        

        switch (mediaType) {
            case MEDIA_SD_CIA:
                AM_QueryAvailableExternalTitleDatabase(NULL);
                AM_StartCiaInstall(MEDIATYPE_SD, &handle);
                break;
            case MEDIA_NAND_CIA:
            default:
                AM_StartCiaInstall(MEDIATYPE_NAND, &handle);
            break;
        }
        FILE* cia = fopen(filename, "r");
        fseek(cia, 0, SEEK_END);
        fileSize = ftell(cia);
        rewind(cia);
        buf = (char*)calloc(fileSize, sizeof(char));    
        fread(buf, sizeof(char), fileSize, cia);
        FSFILE_Write(handle, NULL, 0, buf, fileSize, 0);
        AM_FinishCiaInstall(handle);
        fclose(cia);
        free(buf);
    } else {
        // You'll need titleID
        //u64 titleID;
        //AM_DeleteTitle(mediaType, titleID);
        //AM_DeleteTicket(titleID);
    }    
}

static string dialog_text;

/**
 * Make the dialog box appear.
 * @param text Dialog box text.
 */
void DialogBoxAppear(int x, int y, const char *text) {
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
		pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
		if (screenmode == SCREEN_MODE_SETTINGS) {
			pp2d_draw_texture(settingstex, 0, 0);
		}
		pp2d_draw_texture(dialogboxtex, 0, i-240);			
		pp2d_draw_text(x, y+i-240, 0.5f, 0.5f, BLACK, dialog_text.c_str());
		pp2d_end_draw();
	}
	showdialogbox = true;
}

/**
 * Make the dialog box disappear.
 * @param text Dialog box text.
 */
void DialogBoxDisappear(int x, int y, const char *text) {
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
		pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
		if (screenmode == SCREEN_MODE_SETTINGS) {
			pp2d_draw_texture(settingstex, 0, 0);
		}
		pp2d_draw_texture(dialogboxtex, 0, i);
		pp2d_draw_text(x, y+i, 0.5f, 0.5f, BLACK, dialog_text.c_str());
		pp2d_end_draw();
	}
	showdialogbox = false;
}

/**
 * Install CIA file.
 * @param filename Filename.
 */
static void InstallCIA(const char *filename) {
    char nds_path[256];
    snprintf(nds_path, sizeof(nds_path), "sdmc:/%s/%s", settings.ui.romfolder.c_str() , rom);
    FILE *f_nds_file = fopen(nds_path, "rb");
    if (logEnabled) LogFMA("Install DSiWare", "Installing DSiWare cia...", filename);
    if (!f_nds_file) {
        DialogBoxAppear(12, 16, "fopen(nds_path) failed, skipping.");
        DialogBoxDisappear(12, 16, "fopen(nds_path) failed, skipping.");
    } else {
        DialogBoxAppear(12, 72, "Installing file...");
        TWLoader_cia_manager(true, MEDIA_NAND_CIA, nds_path);
        DialogBoxDisappear(12, 72, "Done!");
    }
    if (logEnabled) LogFMA("Install DSiWare", "Installing DSiWare cia...", "Done!");
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
		screenon();
		DialogBoxAppear(12, 16, "fopen(nds_path) failed, continuing anyway.");
		DialogBoxDisappear(12, 16, "fopen(nds_path) failed, continuing anyway.");
		screenoff();
		return -1;
	}

	char game_TID[5];
	grabTID(f_nds_file, game_TID, false);
	game_TID[4] = 0;
	game_TID[3] = 0;
	fclose(f_nds_file);
	
	if (strcmp(game_TID, "###") != 0) {	// Create save if game isn't homebrew
		screenon();
		DialogBoxAppear(12, 72, "Creating save file...");
		static const int BUFFER_SIZE = 4096;
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, sizeof(buffer));

		int savesize = 524288;	// 512KB (default size for most games)
		
		// Set save size to 8KB for the following games
		if (strcmp(game_TID, "ASC") == 0 )	// Sonic Rush
		{
			savesize = 8192;
		}

		// Set save size to 256KB for the following games
		if (strcmp(game_TID, "AMH") == 0 )	// Metroid Prime Hunters
		{
			savesize = 262144;
		}

		// Set save size to 1MB for the following games
		if (strcmp(game_TID, "AZL") == 0 )	// Wagamama Fashion: Girls Mode/Style Savvy/Nintendo presents: Style Boutique/Namanui Collection: Girls Style
		{
			savesize = 1048576;
		}

		// Set save size to 32MB for the following games
		if (strcmp(game_TID, "UOR") == 0 )	// WarioWare - D.I.Y. (Do It Yourself)
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

		DialogBoxDisappear(12, 72, "Done!");
		screenoff();
	}
	return 0;
}

/**
 * Write selected game's .sav to donor cart.
 * @param filename Filename.
 * @return 0 on success; non-zero on error.
 */
/*static int WriteGameSaveToDonor(std::u16string filename) {
	Result res = 0;

	Title title;

	char nds_path[256];
	snprintf(nds_path, sizeof(nds_path), "sdmc:/%s/%s", settings.ui.romfolder.c_str() , rom);
	FILE *f_nds_file = fopen(nds_path, "rb");
	if (!f_nds_file) {
		screenon();
		DialogBoxAppear(12, 16, "fopen(nds_path) failed, continuing anyway.");
		DialogBoxDisappear(12, 16, "fopen(nds_path) failed, continuing anyway.");
		screenoff();
		return -1;
	}
	
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

	if (gamecardGetType() == CARD_TYPE_NTR || gamecardGetType() == CARD_TYPE_TWL_ENH) {
		screenon();
		DialogBoxAppear(12, 72, "Writing save file to game cart...");
		pxiDevInit();

		CardType cardType = title.getSPICardType();
		u32 saveSize = SPIGetCapacity(cardType);
		u32 pageSize = SPIGetPageSize(cardType);

		u8* saveFile = new u8[saveSize];
		FSStream stream(getArchiveSDMC(), filename, FS_OPEN_READ);

		if (stream.getLoaded())
		{
			stream.read(saveFile, saveSize);
		}
		res = stream.getResult();
		stream.close();

		if (R_FAILED(res))
		{
			delete[] saveFile;
			DialogBoxDisappear(12, 72, "Failed to read .sav file.");
			screenoff();
			pxiDevExit();
			return -1;
		}

		for (u32 i = 0; i < saveSize/pageSize; ++i)
		{
			res = SPIWriteSaveData(cardType, pageSize*i, saveFile + pageSize*i, pageSize);
			if (R_FAILED(res))
			{
				break;
			}
		}

		if (R_FAILED(res))
		{
			delete[] saveFile;
			DialogBoxDisappear(12, 72, "Failed to write save to game cart.");
			screenoff();
			pxiDevExit();
			return -1;
		}		

		DialogBoxDisappear(12, 72, "Done!");
		screenoff();
		pxiDevExit();
	}
	return 0;
}*/

/**
 * Set homebrew version of nds-bootstrap for homebrew ROMs.
 */
void SetHomebrewBootstrap() {
	char nds_path[256];
	snprintf(nds_path, sizeof(nds_path), "sdmc:/%s/%s", settings.ui.romfolder.c_str() , rom);
	FILE *f_nds_file = fopen(nds_path, "rb");

	char game_TID[5];
	grabTID(f_nds_file, game_TID, false);
	game_TID[4] = 0;
	game_TID[3] = 0;
	fclose(f_nds_file);
	
	if (!memcmp(game_TID, "###", 3)) {
		bootstrapPath = "sd:/_nds/hb-bootstrap.nds";
	} else {
		if (settings.twl.bootstrapfile == 1) {
			bootstrapPath = "sd:/_nds/unofficial-bootstrap.nds";
		} else {
			bootstrapPath = "sd:/_nds/release-bootstrap.nds";
		}
	}

}

/**
 * Set donor SDK version for a specific game.
 */
void SetDonorSDK() {
	char nds_path[256];
	snprintf(nds_path, sizeof(nds_path), "sdmc:/%s/%s", settings.ui.romfolder.c_str() , rom);
	FILE *f_nds_file = fopen(nds_path, "rb");

	SDKVersion = 0;
	char game_TID[5];
	grabTID(f_nds_file, game_TID, false);
	game_TID[4] = 0;
	game_TID[3] = 0;
	if(strcmp(game_TID, "###") != 0) SDKVersion = getSDKVersion(f_nds_file, rom);
	fclose(f_nds_file);
	
	settings.twl.donorSdkVer = 0;

	// Check for ROM hacks that need an SDK version.
	static const char sdk2_list[][4] = {
		"AMQ",	// Mario vs. Donkey Kong 2 - March of the Minis
		"AMH",	// Metroid Prime Hunters
		"ASM",	// Super Mario 64 DS
		"SMS",	// Super Mario Star World, and Mario's Holiday
	};
	
	static const char sdk3_list[][4] = {
		"AMC",	// Mario Kart DS
		"EKD",	// Ermii Kart DS (Mario Kart DS hack)
		"A2D",	// New Super Mario Bros.
		"ADA",	// Pokemon Diamond
		"APA",	// Pokemon Pearl
		"ARZ",	// Rockman ZX/MegaMan ZX
		"YZX",	// Rockman ZX Advent/MegaMan ZX Advent
	};
	
	static const char sdk4_list[][4] = {
		"YKW",	// Kirby Super Star Ultra
		"A6C",	// MegaMan Star Force: Dragon
		"A6B",	// MegaMan Star Force: Leo Pegasus
		"A6A",	// MegaMan Star Force: Dragon
		"B6Z",	// Rockman Zero Collection/MegaMan Zero Collection
		"YT7",	// SEGA Superstars Tennis
		"AZL",	// Style Savvy
	};

	static const char sdk5_list[][4] = {
		"B2D",	// Doctor Who: Evacuation Earth
		"BH2",	// Super Scribblenauts
		"BSD",	// Lufia: Curse of the Sinistrals
		"BXS",	// Sonic Colo(u)rs
		"BOE",	// Inazuma Eleven 3: Sekai heno Chousen! The Ogre
		"BQ8",	// Crafting Mama
		"BK9",	// Kingdom Hearts: Re-Coded
		"BRJ",	// Radiant Historia
		"B3R",	// Pokemon Ranger: Guardian Signs
		"IRA",	// Pokemon Black Version
		"IRB",	// Pokemon White Version
		"VI2",	// Fire Emblem: Shin Monshou no Nazo Hikari to Kage no Eiyuu
		"BYY",	// Yu-Gi-Oh 5Ds World Championship 2011: Over The Nexus
		"UZP",	// Learn with Pokemon: Typing Adventure
		"B6F",	// LEGO Batman 2: DC Super Heroes
		"IRE",	// Pokemon Black Version 2
		"IRD",	// Pokemon White Version 2
	};

	// TODO: If the list gets large enough, switch to bsearch().
	for (unsigned int i = 0; i < sizeof(sdk2_list)/sizeof(sdk2_list[0]); i++) {
		if (!memcmp(game_TID, sdk2_list[i], 3)) {
			// Found a match.
			settings.twl.donorSdkVer = 2;
			break;
		}
	}
	
	// TODO: If the list gets large enough, switch to bsearch().
	for (unsigned int i = 0; i < sizeof(sdk3_list)/sizeof(sdk3_list[0]); i++) {
		if (!memcmp(game_TID, sdk3_list[i], 3)) {
			// Found a match.
			settings.twl.donorSdkVer = 3;
			break;
		}
	}

	// TODO: If the list gets large enough, switch to bsearch().
	for (unsigned int i = 0; i < sizeof(sdk4_list)/sizeof(sdk4_list[0]); i++) {
		if (!memcmp(game_TID, sdk4_list[i], 3)) {
			// Found a match.
			settings.twl.donorSdkVer = 4;
			break;
		}
	}

	if(SDKVersion > 0x5000000) {
		settings.twl.donorSdkVer = 5;

		if (settings.twl.bootstrapfile == 1) {
			bootstrapPath = "sd:/_nds/unofficial-bootstrap-sdk5.nds";
		} else {
			bootstrapPath = "sd:/_nds/release-bootstrap-sdk5.nds";
		}
	} else {
		// TODO: If the list gets large enough, switch to bsearch().
		for (unsigned int i = 0; i < sizeof(sdk5_list)/sizeof(sdk5_list[0]); i++) {
			if (!memcmp(game_TID, sdk5_list[i], 3)) {
				// Found a match.
				settings.twl.donorSdkVer = 5;

				if (settings.twl.bootstrapfile == 1) {
					bootstrapPath = "sd:/_nds/unofficial-bootstrap-sdk5.nds";
				} else {
					bootstrapPath = "sd:/_nds/release-bootstrap-sdk5.nds";
				}
				break;
			}
		}
	}
}

/**
 * Set compatibility check for a specific game.
 */
void SetCompatibilityCheck() {
	char nds_path[256];
	snprintf(nds_path, sizeof(nds_path), "sdmc:/%s/%s", settings.ui.romfolder.c_str() , rom);
	FILE *f_nds_file = fopen(nds_path, "rb");

	char game_TID[5];
	grabTID(f_nds_file, game_TID, false);
	game_TID[4] = 0;
	game_TID[3] = 0;
	fclose(f_nds_file);
	
	settings.twl.run_timeout = true;

	// Check for games that don't need compatibility checks.
	static const char list[][4] = {
		"###",	// Homebrew
		"NTR",	// Download Play ROMs
		"ADM",	// Animal Crossing: Wild World
		"AZD",	// The Legend of Zelda: Twilight Princess E3 Trailer
		"A2D",	// New Super Mario Bros.
	};
	
	// TODO: If the list gets large enough, switch to bsearch().
	for (unsigned int i = 0; i < sizeof(list)/sizeof(list[0]); i++) {
		if (!memcmp(game_TID, list[i], 3)) {
			// Found a match.
			settings.twl.run_timeout = false;
			break;
		}
	}
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
	grabTID(f_nds_file, game_TID, false);
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
		"A2D",	// New Super Mario Bros.
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
/**
 * Set green color for notification LED
 * @return 0 on success; non-zero on error.
 */
static int dsGreenLed(void) {
	RGBLedPattern pattern;
	pattern.ani = 32;	// Need to be 32 in order to be it constant

	// Set the color values to a single RGB value.
	memset(&pattern.r, (u8)0, sizeof(pattern.r));
	memset(&pattern.g, (u8)255, sizeof(pattern.g));
	memset(&pattern.b, (u8)0, sizeof(pattern.b));

	if (ptmsysmInit() < 0)
		return -1;
	ptmsysmSetInfoLedPattern(&pattern);
	ptmsysmExit();
	if (logEnabled)	LogFM("Main.dsGreenLed", "Notification LED is green");
	return 0;
}

static void ChangeBNRIconNo(void) {
	// Get the bnriconnum relative to the current page.
	const int idx = bnriconnum - (settings.ui.pagenum * gamesPerPage);
	if (idx >= 0 && idx < gamesPerPage) {
		// Selected banner icon is on the current page.
		bnricontexnum = bnricontex[(idx % 6)+bnriconPalLine[idx]*8];
	}
	if (settings.twl.romtype == 1) bnricontexnum = gbctex;
	else if (settings.twl.romtype == 2) bnricontexnum = nestex;
}

static void ChangeBoxArtNo(void) {
	// Get the boxartnum relative to the current page.
	const int idx = boxartnum - (settings.ui.pagenum * gamesPerPage);
	if (idx >= 0 && idx < gamesPerPage) {
		// Selected boxart is on the current page.
		// NOTE: Only 6 slots for boxart.
		boxarttexnum = boxarttex[idx % 6];
	}
}

/**
 * Store a banner icon path.
 * @param path Bnricon path. (will be strdup()'d)
 */
static void StoreBnrIconPath(const char *path) {
	// Get the bnriconnum relative to the current page.
	const int idx = loadbnriconnum;
	if (idx >= 0 && idx < gamesPerPage) {
		if (!path) {
			path = "romfs:/notextbanner";
		}
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
	const int idx = loadboxartnum - (settings.ui.pagenum * gamesPerPage);
	if (idx >= 0 && idx < gamesPerPage) {
		// Selected boxart is on the current page.
		free(boxartpath[idx]);
		boxartpath[idx] = strdup(path);
	} else {
		free(boxartpath[gamesPerPage]);
		boxartpath[gamesPerPage] = strdup("romfs:/graphics/blank_128x115.png");
	}
}

/**
 * Load a banner icon at the current bnriconnum.
 * @param filename Banner filename, or NULL for notextbanner.
 */
static void LoadBNRIcon(void) {
	// Get the bnriconnum relative to the current page.
	const int idx = loadbnriconnum;
	if (idx >= 0 && idx < 6) {
		for (int i = 0; i < 8; i++) {
			pp2d_free_texture(bnricontex[idx+i*8]);
		}
		// Selected bnriconnum is on the current page.
		FILE *f_bnr = fopen(bnriconpath[idx], "rb");
		fseek(f_bnr, 0, SEEK_END);
		off_t fsize = ftell(f_bnr);
		fseek(f_bnr, 0, SEEK_SET);

		u16 bannerVersion = grabBannerVersion(f_bnr);
		if(bannerVersion == NDS_BANNER_VER_DSi && fsize >= NDS_BANNER_SIZE_DSi) {
			for (int i = 0; i < 8; i++) {
				pp2d_load_texture_memory_RGBA5551(bnricontex[idx+i*8], grabIconDSi(f_bnr, i), 32, 256);
			}
		} else {
			pp2d_load_texture_memory_RGBA5551(bnricontex[idx], grabIcon(f_bnr), 32, 64);
		}
		fclose(f_bnr);
	}
}

/**
 * Load a banner icon at the current bnriconnum.
 * @param filename Banner filename, or NULL for notextbanner.
 */
static void LoadBNRIcon_Menu(int idx) {
	if (idx >= 0 && idx < gamesPerPage) {
		for (int i = 0; i < 8; i++) {
			pp2d_free_texture(bnricontex[(idx % 6)+i*8]);
		}
		// Selected bnriconnum is on the current page.
		FILE *f_bnr = fopen(bnriconpath[idx], "rb");
		fseek(f_bnr, 0, SEEK_END);
		off_t fsize = ftell(f_bnr);
		fseek(f_bnr, 0, SEEK_SET);

		u16 bannerVersion = grabBannerVersion(f_bnr);
		if(bannerVersion == NDS_BANNER_VER_DSi && fsize >= NDS_BANNER_SIZE_DSi) {
			for (int i = 0; i < 8; i++) {
				pp2d_load_texture_memory_RGBA5551(bnricontex[(idx % 6)+i*8], grabIconDSi(f_bnr, i), 32, 256);
			}
		} else {
			pp2d_load_texture_memory_RGBA5551(bnricontex[idx % 6], grabIcon(f_bnr), 32, 64);
		}
		fclose(f_bnr);
	}
}

/**
 * Load a banner icon at the current bnriconnum.
 * @param filename Banner filename, or NULL for notextbanner.
 */
static void LoadBNRSeq(void) {
	// Get the bnriconnum relative to the current page.
	const int idx = loadbnriconnum;
	if (idx >= 0 && idx < gamesPerPage) {
		// Selected bnriconnum is on the current page.
		FILE *f_bnr = fopen(bnriconpath[idx], "rb");
		fseek(f_bnr, 0, SEEK_END);
		off_t fsize = ftell(f_bnr);
		fseek(f_bnr, 0, SEEK_SET);

		u16 bannerVersion = grabBannerVersion(f_bnr);
		if(bannerVersion == NDS_BANNER_VER_DSi && fsize >= NDS_BANNER_SIZE_DSi) {
			grabBannerSequence(f_bnr, idx);
			bnriconisDSi[idx] = true;
		} else {
			bnriconisDSi[idx] = false;
		}
		fclose(f_bnr);
	}
}

/**
 * Load a banner icon at the current cursorPosition.
 * @param filename Banner filename, or NULL for notextbanner.
 */
static void LoadBNRIcon_R4Theme(const char *filename) {
	for (int i = 0; i < 8; i++) {
		pp2d_free_texture(bnricontex[6+i*8]);
	}
	if (!filename) {
		filename = "romfs:/notextbanner";
	}
	FILE *f_bnr = fopen(filename, "rb");
	if (!f_bnr) {
		filename = "romfs:/notextbanner";
		f_bnr = fopen(filename, "rb");
	}
	fseek(f_bnr, 0, SEEK_END);
	off_t fsize = ftell(f_bnr);
	fseek(f_bnr, 0, SEEK_SET);

	u16 bannerVersion = grabBannerVersion(f_bnr);
	if(bannerVersion == NDS_BANNER_VER_DSi && fsize >= NDS_BANNER_SIZE_DSi) {
		for (int i = 0; i < 8; i++) {
			pp2d_load_texture_memory_RGBA5551(bnricontex[6+i*8], grabIconDSi(f_bnr, i), 32, 256);
		}
		grabBannerSequence(f_bnr, gamesPerPage);
		bnriconisDSi[gamesPerPage] = true;
	} else {
		pp2d_load_texture_memory_RGBA5551(bnricontex[6], grabIcon(f_bnr), 32, 64);
		bnriconisDSi[gamesPerPage] = false;
	}
	fclose(f_bnr);
}

static void LoadBoxArt(void) {
	// Get the boxartnum relative to the current page.
	const int idx = boxartnum - (settings.ui.pagenum * gamesPerPage);
	if (idx >= 0 && idx < gamesPerPage+1) {
		// Selected boxart is on the current page.
		// NOTE: Only 6 slots for boxart.
		pp2d_free_texture(boxarttex[idx % 6]);
		const char *path = (boxartpath[idx] ? boxartpath[idx] : "romfs:/graphics/blank_128x115.png");
		pp2d_load_texture_png(boxarttex[idx % 6], path); // Box art
	}
}

static void LoadBoxArt_WoodTheme(void) {
	// Selected boxart is on the current page.
	pp2d_free_texture(boxarttex[6]);
	const char *path = (boxartpath[boxartnum] ? boxartpath[boxartnum] : "romfs:/graphics/blank_128x115.png");
	pp2d_load_texture_png(boxarttex[6], path); // Box art
}

/**
 * Load nds-bootstrap's configuration.
 */
static void LoadBootstrapConfig(void)
{
	settings.twl.loadingscreen = bootstrapini.GetInt(bootstrapini_ndsbootstrap, bootstrapini_loadingscreen, 1);
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
		if (!settings.twl.launchslot1 && gbarunnervalue == 0 && settings.twl.romtype == 0) {
			SetHomebrewBootstrap();
			SetDonorSDK();
			SetCompatibilityCheck();
			SetMPUSettings();
			bootstrapini.SetString(bootstrapini_ndsbootstrap, bootstrapini_ndspath, fat+settings.ui.romfolder+slashchar+rom);
			bootstrapini.SetInt(bootstrapini_ndsbootstrap, "DONOR_SDK_VER", settings.twl.donorSdkVer);
			bootstrapini.SetInt(bootstrapini_ndsbootstrap, "CHECK_COMPATIBILITY", settings.twl.run_timeout);
			bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_mpuregion, settings.twl.mpuregion);
			bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_mpusize, settings.twl.mpusize);
			bootstrapini.SetString(bootstrapini_ndsbootstrap, bootstrapini_savpath, fat+settings.ui.romfolder+slashchar+sav);
			char path[256];
			std::u16string u16_path;
			snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.romfolder.c_str(), sav.c_str());
			u16_path = u8tou16("/") + u8tou16(settings.ui.romfolder.c_str()) + u8tou16("/") + u8tou16(sav.c_str());
			if (logEnabled) LogFMA("Main.SaveBootstrapConfig", "Using u16_path:", u16tou8(u16_path).c_str());
			if (access(path, F_OK) == -1) {
				// Create a save file if it doesn't exist
				CreateGameSave(path);
			}
			//if(SDKVersion > 0x5000000) WriteGameSaveToDonor(u16_path);
		} else {
			bootstrapPath = "sd:/_nds/hb-bootstrap.nds";
			bootstrapini.SetString(bootstrapini_ndsbootstrap, bootstrapini_ndspath, "sd:/_nds/GBARunner2.nds");
		}
	}
	if (logEnabled) LogFMA("Main.SaveBootstrapConfig", "Using path:", bootstrapPath.c_str());
	bootstrapini.SetString(bootstrapini_ndsbootstrap, bootstrapini_bootstrappath, bootstrapPath);
	bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_loadingscreen, settings.twl.loadingscreen);
	bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_boostcpu, settings.twl.cpuspeed);
	bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_lockarm9scfgext, 0);
	
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
	if (!settings.twl.forwarder) {
		if (settings.twl.romtype == 0) {
			inifilename = ReplaceAll(rom, ".nds", ".ini");
			inifilename = ReplaceAll(rom, ".cia", ".ini");
		} else if (settings.twl.romtype == 1) {
			inifilename = ReplaceAll(rom, ".gb", ".ini");
			inifilename = ReplaceAll(rom, ".gbc", ".ini");
			inifilename = ReplaceAll(rom, ".sgb", ".ini");
		} else if (settings.twl.romtype == 2) {
			inifilename = ReplaceAll(rom, ".nes", ".ini");
			inifilename = ReplaceAll(rom, ".fds", ".ini");
		}
	} else {
		char path[256];
		snprintf(path, sizeof(path), "%s/%s", "flashcard", rom);
		inifilename = path;
	}
	char path[256];
	snprintf(path, sizeof(path), "sdmc:/_nds/twloader/gamesettings/%s", inifilename.c_str());
	CIniFile gamesettingsini(path);
	settings.pergame.cpuspeed = gamesettingsini.GetInt("GAME-SETTINGS", "TWL_CLOCK", -1);
	settings.pergame.usedonor = gamesettingsini.GetInt("GAME-SETTINGS", "USE_ARM7_DONOR", 1);
	if(settings.pergame.usedonor > 2) settings.pergame.usedonor = 1;
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
	if (!settings.twl.forwarder) {
		if (settings.twl.romtype == 0) {
			inifilename = ReplaceAll(rom, ".nds", ".ini");
			inifilename = ReplaceAll(rom, ".cia", ".ini");
		} else if (settings.twl.romtype == 1) {
			inifilename = ReplaceAll(rom, ".gb", ".ini");
			inifilename = ReplaceAll(rom, ".gbc", ".ini");
			inifilename = ReplaceAll(rom, ".sgb", ".ini");
		} else if (settings.twl.romtype == 2) {
			inifilename = ReplaceAll(rom, ".nes", ".ini");
			inifilename = ReplaceAll(rom, ".fds", ".ini");
		}
	} else {
		char path[256];
		snprintf(path, sizeof(path), "%s/%s", "flashcard", rom);
		inifilename = path;
	}
	char path[256];
	snprintf(path, sizeof(path), "sdmc:/_nds/twloader/gamesettings/%s", inifilename.c_str());
	CIniFile gamesettingsini(path);
	gamesettingsini.SetInt("GAME-SETTINGS", "TWL_CLOCK", settings.pergame.cpuspeed);
	gamesettingsini.SetInt("GAME-SETTINGS", "USE_ARM7_DONOR", settings.pergame.usedonor);
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
		if (settings.twl.romtype == 0) {
			inifilename = ReplaceAll(rom, ".nds", ".ini");
			inifilename = ReplaceAll(rom, ".cia", ".ini");
		} else if (settings.twl.romtype == 1) {
			inifilename = ReplaceAll(rom, ".gb", ".ini");
			inifilename = ReplaceAll(rom, ".gbc", ".ini");
			inifilename = ReplaceAll(rom, ".sgb", ".ini");
		} else if (settings.twl.romtype == 2) {
			inifilename = ReplaceAll(rom, ".nes", ".ini");
			inifilename = ReplaceAll(rom, ".fds", ".ini");
		}
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

/**
 * Draw the volume slider.
 * @param texarray Texture array to use, (voltex or setvoltex)
 * The Dsi has 8 positions for volume and the 3ds has 64
 * Remap volume to simulate the 8 positions
 */
void draw_volume_slider(size_t texnum)
{
	u8 volumeLevel = 0;
	if (!dspfirmfound) {
		// No DSP Firm.
		pp2d_draw_texture_part(texnum, 5, 2, 0, 80, 32, 16);
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
		pp2d_draw_texture_part(texnum, 5, 2, 0, voltex_id*16, 32, 16);
	}
}

int batteryFrame = 0;

/**
 * Update the battery level icon.
 * @param texchrg Texture for "battery is charging". (batterychrgtex or setbatterychrgtex)
 * @param texarray Texture array for other levels. (batterytex or setbatterytex)
 * The global variable batteryIcon will be updated.
 */
void update_battery_level(size_t texnum)
{
	u8 batteryChargeState = 0;
	u8 batteryLevel = 0;
	batteryIcon = texnum;
	if (R_SUCCEEDED(PTMU_GetBatteryChargeState(&batteryChargeState)) && batteryChargeState) {
		batteryFrame = 6;
	} else if (R_SUCCEEDED(PTMU_GetBatteryLevel(&batteryLevel))) {
		switch (batteryLevel) {
			case 5: {
				// NOTE: PTMUX_GetAdapterState should be moved into
				// ctrulib without the 'X' prefix.
				u8 acAdapter = 0;
				if (R_SUCCEEDED(PTMUX_GetAdapterState(&acAdapter)) && acAdapter) {
					batteryFrame = 5;
				} else {
					batteryFrame = 4;
				}
				break;
			}
			case 4:
				batteryFrame = 4;
				break;
			case 3:
				batteryFrame = 3;
				break;
			case 2:
				batteryFrame = 2;
				break;
			case 1:
			default:
				batteryFrame = 1;
				break;
		}
	}

	if (!batteryIcon) {
		// No battery icon...
		batteryFrame = 0;
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

/**
 * Scan a directory for matching files.
 * @param path Directory path.
 * @param ext File extension, case-insensitive. (If nullptr, matches all files.)
 * @param ext2 File extension, case-insensitive. (If nullptr, matches all files.)
 * @param files Vector to append files to.
 * @return Number of files matched. (-1 if the directory could not be opened.)
 */
static int scan_dir_for_files2(const char *path, const char *ext, const char *ext2, std::vector<std::string>& files)
{
	files.clear();

	//ext
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

	//ext2
	dir = opendir(path);
	if (!dir) {
		// Unable to open the directory.
		return -1;
	}

	struct dirent *ent2;
	const int extlen2 = (ext2 ? strlen(ext2) : 0);
	while ((ent2 = readdir(dir)) != NULL) {
		std::string fname = (ent2->d_name);
		if (extlen2 > 0) {
			// Check the file extension. (TODO needs verification)
			size_t lastdotpos = fname.find_last_of('.');
			if (lastdotpos == string::npos || lastdotpos + extlen2 > fname.size()) {
				// Invalid file extension.
				continue;
			}
			if (strcasecmp(&ent2->d_name[lastdotpos], ext2) != 0) {
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

/**
 * Scan a directory for matching files.
 * @param path Directory path.
 * @param ext File extension, case-insensitive. (If nullptr, matches all files.)
 * @param ext2 File extension, case-insensitive. (If nullptr, matches all files.)
 * @param ext3 File extension, case-insensitive. (If nullptr, matches all files.)
 * @param files Vector to append files to.
 * @return Number of files matched. (-1 if the directory could not be opened.)
 */
static int scan_dir_for_files3(const char *path, const char *ext, const char *ext2, const char *ext3, std::vector<std::string>& files)
{
	files.clear();

	//ext
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

	//ext2
	dir = opendir(path);
	if (!dir) {
		// Unable to open the directory.
		return -1;
	}

	struct dirent *ent2;
	const int extlen2 = (ext2 ? strlen(ext2) : 0);
	while ((ent2 = readdir(dir)) != NULL) {
		std::string fname = (ent2->d_name);
		if (extlen2 > 0) {
			// Check the file extension. (TODO needs verification)
			size_t lastdotpos = fname.find_last_of('.');
			if (lastdotpos == string::npos || lastdotpos + extlen2 > fname.size()) {
				// Invalid file extension.
				continue;
			}
			if (strcasecmp(&ent2->d_name[lastdotpos], ext2) != 0) {
				// Incorrect file extension.
				continue;
			}
		}

		// Append the file.
		files.push_back(fname);
	}
	closedir(dir);

	//ext3
	dir = opendir(path);
	if (!dir) {
		// Unable to open the directory.
		return -1;
	}

	struct dirent *ent3;
	const int extlen3 = (ext3 ? strlen(ext3) : 0);
	while ((ent3 = readdir(dir)) != NULL) {
		std::string fname = (ent3->d_name);
		if (extlen3 > 0) {
			// Check the file extension. (TODO needs verification)
			size_t lastdotpos = fname.find_last_of('.');
			if (lastdotpos == string::npos || lastdotpos + extlen3 > fname.size()) {
				// Invalid file extension.
				continue;
			}
			if (strcasecmp(&ent3->d_name[lastdotpos], ext3) != 0) {
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
vector<string> gbfiles;
vector<string> nesfiles;

// Vector with found roms
vector<string> matching_files;

// Vector to select rom for deletion
vector<string> delete_queue;

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

/**
 * Determine the 3DS cartridge texture to use for Slot-1.
 * @return Cartridge texture.
 */
static inline size_t carttex(void)
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
			pp2d_load_texture_png(slot1boxarttex, "romfs:/graphics/boxart_null.png");
		}
		return;
	}
	prev_gameID_u32 = gameID_u32;

	const char *gameID = gamecardGetGameID();
	if (gameID) {
		if (checkWifiStatus()) {
			downloadSlot1BoxArt(gameID);
		}
		char path[256];
		// example: ASME.png
		if (logEnabled)	LogFMA("Main", "Loading Slot-1 box art", gameID);
		snprintf(path, sizeof(path), "%s/%.4s.png", slot1boxartfolder, gameID);
		if (access(path, F_OK) != -1) {
			pp2d_load_texture_png(new_tex, path);
		} else {
			pp2d_load_texture_png(new_tex, "romfs:/graphics/boxart_unknown.png");
		}
		if (logEnabled)	LogFMA("Main", "Done loading Slot-1 box art", gameID);
	} else {
		// No cartridge, or unrecognized cartridge.
		pp2d_load_texture_png(new_tex, "romfs:/graphics/boxart_null.png");
	}

	// Replace slot1boxarttex with the new boxart.
	slot1boxarttex = new_tex;
}

//static int gameType[] = 0;

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

	// Scan the ROMs directory for ".nds" and ".cia" files.
	scan_dir_for_files2(path, ".nds", ".cia", files);
	
	// Use default directory if none is specified
	if (settings.ui.fcromfolder.empty()) {
		settings.ui.fcromfolder = "roms/flashcard/nds";
	}
	snprintf(path, sizeof(path), "sdmc:/%s", settings.ui.fcromfolder.c_str());
	// Make sure the directory exists.
	rmkdir(path, 0777);

	// Scan the flashcard directory for configuration files.
	scan_dir_for_files(path, ".ini", fcfiles);

	// Use default directory if none is specified
	if (settings.ui.gbromfolder.empty()) {
		settings.ui.gbromfolder = "roms/gb";
	}
	snprintf(path, sizeof(path), "sdmc:/%s", settings.ui.gbromfolder.c_str());
	// Make sure the directory exists.
	rmkdir(path, 0777);

	// Scan the GB ROMs directory for ".gb", ".gbc", and ".sgb" files.
	scan_dir_for_files3(path, ".gb", ".gbc", ".sgb", gbfiles);

	// Use default directory if none is specified
	if (settings.ui.nesromfolder.empty()) {
		settings.ui.nesromfolder = "roms/nes";
	}
	snprintf(path, sizeof(path), "sdmc:/%s", settings.ui.nesromfolder.c_str());
	// Make sure the directory exists.
	rmkdir(path, 0777);

	// Scan the NES ROMs directory for ".nes" and ".fds" files.
	scan_dir_for_files2(path, ".nes", ".fds", nesfiles);
}

// Cursor position.
static int storedcursorPosition = 0;
static int r4menu_cursorPosition = 0;
static int woodmenu_cursorPosition = 0;
static int startmenu_cursorPosition = 0;
static int setromtype_cursorPosition = 0;
static int gamesettings_cursorPosition = 0;

static bool gamesettings_isCia = false;

/**
 * Draw the menu dialog box.
 */
static void drawMenuDialogBox(void)
{
	pp2d_draw_rectangle(0, 0, 320, 240, RGBA8(0, 0, 0, menudbox_bgalpha)); // Fade in/out effect
	pp2d_draw_texture(dialogboxtex, 0, menudbox_Ypos);
	pp2d_draw_texture(dboxtex_buttonback, 233, menudbox_Ypos+193);
	if (menudboxmode == DBOX_MODE_DONOR_NOT_SET) {
		pp2d_draw_text(244, menudbox_Ypos+199, 0.50, 0.50, BLACK, ": OK");

		bnriconnum = settings.ui.cursorPosition;
		ChangeBNRIconNo();
		pp2d_draw_texture(dboxtex_iconbox, 23, menudbox_Ypos+23);
		pp2d_draw_texture_part_flip(bnricontexnum, 28, menudbox_Ypos+28, 0, bnriconframenumY[bnriconnum-settings.ui.pagenum*gamesPerPage]*32, 32, 32, bannerFlip[bnriconnum-settings.ui.pagenum*gamesPerPage]);
		
		if (settings.ui.cursorPosition >= 0) {
			int y = 16, dy = 19;
			// Print the banner text, center-aligned.
			const size_t banner_lines = std::min(3U, romsel_gameline.size());
			for (size_t i = 0; i < banner_lines; i++, y += dy) {
				const int text_width = pp2d_get_wtext_width(romsel_gameline[i].c_str(), 0.60, 0.60);
				pp2d_draw_wtext(72+(240-text_width)/2, y+menudbox_Ypos, 0.60, 0.60, BLACK, romsel_gameline[i].c_str());
			}
			pp2d_draw_wtext(16, 72+menudbox_Ypos, 0.50, 0.50, GRAY, romsel_filename_w.c_str());
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
			pp2d_draw_text(16, 204+menudbox_Ypos, 0.50, 0.50, BLACK, romsel_counter1);
			pp2d_draw_text(35, 204+menudbox_Ypos, 0.50, 0.50, BLACK, "/");
			pp2d_draw_text(40, 204+menudbox_Ypos, 0.50, 0.50, BLACK, romsel_counter2);
		} else {
			pp2d_draw_text(16, 204+menudbox_Ypos, 0.50, 0.50, BLACK, romsel_counter1);
			pp2d_draw_text(43, 204+menudbox_Ypos, 0.50, 0.50, BLACK, "/");
			pp2d_draw_text(48, 204+menudbox_Ypos, 0.50, 0.50, BLACK, romsel_counter2);
		}

		pp2d_draw_text(32, 112+menudbox_Ypos, 0.50, 0.50, BLACK,
		"This game needs a donor ROM set.\n"
		"\n"
		"Please set Mario Kart DS as donor ROM,\n"
		"by moving to the ROM, press SELECT,\n"
		"then \"Set as donor ROM\".\n");
	} else if (menudboxmode == DBOX_MODE_OVERLAYS) {
		pp2d_draw_text(244, menudbox_Ypos+199, 0.50, 0.50, BLACK, ": OK");
		
		bnriconnum = settings.ui.cursorPosition;
		ChangeBNRIconNo();
		pp2d_draw_texture(dboxtex_iconbox, 23, menudbox_Ypos+23);
		pp2d_draw_texture_part_flip(bnricontexnum, 28, menudbox_Ypos+28, 0, bnriconframenumY[bnriconnum-settings.ui.pagenum*gamesPerPage]*32, 32, 32, bannerFlip[bnriconnum-settings.ui.pagenum*gamesPerPage]);
		
		if (settings.ui.cursorPosition >= 0) {
			int y = 16, dy = 19;
			// Print the banner text, center-aligned.
			const size_t banner_lines = std::min(3U, romsel_gameline.size());
			for (size_t i = 0; i < banner_lines; i++, y += dy) {
				const int text_width = pp2d_get_wtext_width(romsel_gameline[i].c_str(), 0.60, 0.60);
				pp2d_draw_wtext(72+(240-text_width)/2, y+menudbox_Ypos, 0.60, 0.60, BLACK, romsel_gameline[i].c_str());
			}
			pp2d_draw_wtext(16, 72+menudbox_Ypos, 0.50, 0.50, GRAY, romsel_filename_w.c_str());
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
			pp2d_draw_text(16, 204+menudbox_Ypos, 0.50, 0.50, BLACK, romsel_counter1);
			pp2d_draw_text(35, 204+menudbox_Ypos, 0.50, 0.50, BLACK, "/");
			pp2d_draw_text(40, 204+menudbox_Ypos, 0.50, 0.50, BLACK, romsel_counter2);
		} else {
			pp2d_draw_text(16, 204+menudbox_Ypos, 0.50, 0.50, BLACK, romsel_counter1);
			pp2d_draw_text(43, 204+menudbox_Ypos, 0.50, 0.50, BLACK, "/");
			pp2d_draw_text(48, 204+menudbox_Ypos, 0.50, 0.50, BLACK, romsel_counter2);
		}
		
		pp2d_draw_text(32, 128+menudbox_Ypos, 0.50, 0.50, BLACK,
		"This DSi-Enhanced game cannot be launched,\n"
		"due to overlays being included.");
	} else if (menudboxmode == DBOX_MODE_DELETED) {
		pp2d_draw_text(244, menudbox_Ypos+199, 0.50, 0.50, BLACK, ": OK");
		pp2d_draw_text(64, 112+menudbox_Ypos, 0.50, 0.50, BLACK, "Deleted.");
	} else if (menudboxmode == DBOX_MODE_DELETE) {
		pp2d_draw_text(244, menudbox_Ypos+199, 0.50, 0.50, BLACK, ": No");

		pp2d_draw_texture(dboxtex_buttonback, 143, menudbox_Ypos+193);
		pp2d_draw_text(152, menudbox_Ypos+199, 0.50, 0.50, BLACK, ": Yes");
		
		bnriconnum = settings.ui.cursorPosition;
		ChangeBNRIconNo();
		pp2d_draw_texture(dboxtex_iconbox, 23, menudbox_Ypos+23);
		pp2d_draw_texture_part_flip(bnricontexnum, 28, menudbox_Ypos+28, 0, bnriconframenumY[bnriconnum-settings.ui.pagenum*gamesPerPage]*32, 32, 32, bannerFlip[bnriconnum-settings.ui.pagenum*gamesPerPage]);
		
		if (settings.ui.cursorPosition >= 0) {
			if (settings.twl.romtype == 1) {
				romsel_gameline.clear();
				romsel_gameline.push_back(latin1_to_wstring(""));
				std::string std_romsel_filename = romsel_filename;
				if(std_romsel_filename.substr(std_romsel_filename.find_last_of(".") + 1) == "gb") {
					romsel_gameline.push_back(latin1_to_wstring("GameBoy ROM"));
				} else if(std_romsel_filename.substr(std_romsel_filename.find_last_of(".") + 1) == "gbc") {
					romsel_gameline.push_back(latin1_to_wstring("GameBoy Color ROM"));
				} else if(std_romsel_filename.substr(std_romsel_filename.find_last_of(".") + 1) == "sgb") {
					romsel_gameline.push_back(latin1_to_wstring("Super GameBoy ROM"));
				}
			} else if (settings.twl.romtype == 2) {
				romsel_gameline.clear();
				romsel_gameline.push_back(latin1_to_wstring(""));
				std::string std_romsel_filename = romsel_filename;
				if(std_romsel_filename.substr(std_romsel_filename.find_last_of(".") + 1) == "nes") {
					romsel_gameline.push_back(latin1_to_wstring("Nintendo Entertainment System ROM"));
				} else if(std_romsel_filename.substr(std_romsel_filename.find_last_of(".") + 1) == "fds") {
					romsel_gameline.push_back(latin1_to_wstring("Famicom Disk System ROM"));
				}
			}

			int y = 16, dy = 19;
			// Print the banner text, center-aligned.
			const size_t banner_lines = std::min(3U, romsel_gameline.size());
			for (size_t i = 0; i < banner_lines; i++, y += dy) {
				const int text_width = pp2d_get_wtext_width(romsel_gameline[i].c_str(), 0.60, 0.60);
				pp2d_draw_wtext(72+(240-text_width)/2, y+menudbox_Ypos, 0.60, 0.60, BLACK, romsel_gameline[i].c_str());
			}
			pp2d_draw_wtext(16, 72+menudbox_Ypos, 0.50, 0.50, GRAY, romsel_filename_w.c_str());
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
			pp2d_draw_text(16, 204+menudbox_Ypos, 0.50, 0.50, BLACK, romsel_counter1);
			pp2d_draw_text(35, 204+menudbox_Ypos, 0.50, 0.50, BLACK, "/");
			pp2d_draw_text(40, 204+menudbox_Ypos, 0.50, 0.50, BLACK, romsel_counter2);
		} else {
			pp2d_draw_text(16, 204+menudbox_Ypos, 0.50, 0.50, BLACK, romsel_counter1);
			pp2d_draw_text(43, 204+menudbox_Ypos, 0.50, 0.50, BLACK, "/");
			pp2d_draw_text(48, 204+menudbox_Ypos, 0.50, 0.50, BLACK, romsel_counter2);
		}
		
		if (settings.twl.forwarder) {
			pp2d_draw_text(32, 128+menudbox_Ypos, 0.50, 0.50, BLACK,
			"Are you sure you want to delete this title?\n"
			"(ROM and save data on the flashcard\n"
			"will be kept.)");
		} else {
			pp2d_draw_text(32, 128+menudbox_Ypos, 0.50, 0.50, BLACK,
			"Are you sure you want to delete this title?\n"
			"(Save data will be kept.)");
		}
	} else if (menudboxmode == DBOX_MODE_SETTINGS) {
		pp2d_draw_wtext(240, menudbox_Ypos+199, 0.50, 0.50, BLACK, TR(STR_BACK));

		bnriconnum = settings.ui.cursorPosition;
		ChangeBNRIconNo();
		pp2d_draw_texture(dboxtex_iconbox, 23, menudbox_Ypos+23);
		pp2d_draw_texture_part_flip(bnricontexnum, 28, menudbox_Ypos+28, 0, bnriconframenumY[bnriconnum-settings.ui.pagenum*gamesPerPage]*32, 32, 32, bannerFlip[bnriconnum-settings.ui.pagenum*gamesPerPage]);
		
		if (settings.ui.cursorPosition >= 0) {
			if (settings.twl.romtype == 1) {
				romsel_gameline.clear();
				romsel_gameline.push_back(latin1_to_wstring(""));
				std::string std_romsel_filename = romsel_filename;
				if(std_romsel_filename.substr(std_romsel_filename.find_last_of(".") + 1) == "gb") {
					romsel_gameline.push_back(latin1_to_wstring("GameBoy ROM"));
				} else if(std_romsel_filename.substr(std_romsel_filename.find_last_of(".") + 1) == "gbc") {
					romsel_gameline.push_back(latin1_to_wstring("GameBoy Color ROM"));
				} else if(std_romsel_filename.substr(std_romsel_filename.find_last_of(".") + 1) == "sgb") {
					romsel_gameline.push_back(latin1_to_wstring("Super GameBoy ROM"));
				}
			} else if (settings.twl.romtype == 2) {
				romsel_gameline.clear();
				romsel_gameline.push_back(latin1_to_wstring(""));
				std::string std_romsel_filename = romsel_filename;
				if(std_romsel_filename.substr(std_romsel_filename.find_last_of(".") + 1) == "nes") {
					romsel_gameline.push_back(latin1_to_wstring("Nintendo Entertainment System ROM"));
				} else if(std_romsel_filename.substr(std_romsel_filename.find_last_of(".") + 1) == "fds") {
					romsel_gameline.push_back(latin1_to_wstring("Famicom Disk System ROM"));
				}
			}

			int y = 16, dy = 19;
			// Print the banner text, center-aligned.
			const size_t banner_lines = std::min(3U, romsel_gameline.size());
			for (size_t i = 0; i < banner_lines; i++, y += dy) {
				const int text_width = pp2d_get_wtext_width(romsel_gameline[i].c_str(), 0.60, 0.60);
				pp2d_draw_wtext(72+(240-text_width)/2, y+menudbox_Ypos, 0.60, 0.60, BLACK, romsel_gameline[i].c_str());
			}
			pp2d_draw_wtext(16, 72+menudbox_Ypos, 0.50, 0.50, GRAY, romsel_filename_w.c_str());
		}

		size_t file_count = 0;
		if (settings.twl.romtype == 0) {
			file_count = (settings.twl.forwarder ? fcfiles.size() : files.size());
		} else if (settings.twl.romtype == 1) {
			file_count = (gbfiles.size());
		} else if (settings.twl.romtype == 2) {
			file_count = (nesfiles.size());
		}

		char romsel_counter1[16];
		char romsel_counter2[16];
		snprintf(romsel_counter1, sizeof(romsel_counter1), "%d", storedcursorPosition+1);		
		if(matching_files.size() == 0){
			snprintf(romsel_counter2, sizeof(romsel_counter2), "%zu", file_count);
		}else{
			snprintf(romsel_counter2, sizeof(romsel_counter2), "%zu", matching_files.size());
		}
		
		if (file_count < 100) {
			pp2d_draw_text(16, 204+menudbox_Ypos, 0.50, 0.50, BLACK, romsel_counter1);
			pp2d_draw_text(35, 204+menudbox_Ypos, 0.50, 0.50, BLACK, "/");
			pp2d_draw_text(40, 204+menudbox_Ypos, 0.50, 0.50, BLACK, romsel_counter2);
		} else {
			pp2d_draw_text(16, 204+menudbox_Ypos, 0.50, 0.50, BLACK, romsel_counter1);
			pp2d_draw_text(43, 204+menudbox_Ypos, 0.50, 0.50, BLACK, "/");
			pp2d_draw_text(48, 204+menudbox_Ypos, 0.50, 0.50, BLACK, romsel_counter2);
		}

		std::string std_romsel_filename = romsel_filename;
		if((std_romsel_filename.substr(std_romsel_filename.find_last_of(".") + 1) == "cia")
		|| (std_romsel_filename.substr(std_romsel_filename.find_last_of(".") + 1) == "gb")
		|| (std_romsel_filename.substr(std_romsel_filename.find_last_of(".") + 1) == "gbc")
		|| (std_romsel_filename.substr(std_romsel_filename.find_last_of(".") + 1) == "sgb"))
		{
			gamesettings_isCia = true;
			gamesettings_cursorPosition = 3;
		} else {
			gamesettings_isCia = false;
		}

		const struct {
			int x;
			int y;
			const s8 *value;
			const wchar_t *title;
			const wchar_t *value_desc[3];	// 0 == off, 1 == on, 2 == custom
		} buttons[] = {
			{ 23,  89, &settings.pergame.cpuspeed, TR(STR_START_ARM9_CPU_SPEED), {L"67 MHz (NTR)", L"133 MHz (TWL)", L""}},
			{ 23, 129, &settings.pergame.usedonor, TR(STR_START_USE_ARM7_DONOR), {L"Off", L"On", L"Force-use"}},
			{161, 129, &settings.pergame.donor, TR(STR_START_SET_DONOR), {L"", L"", L""}},
			{ 23, 169, NULL, TR(STR_START_SET_LED), {L"", L"", L""}},
		};

		if(gamesettings_isCia) {
			pp2d_draw_texture(dboxtex_button, buttons[3].x, menudbox_Ypos+129);

			const wchar_t *title = buttons[3].title;

			// Determine the text height.
			// NOTE: Button texture size is 132x34.
			const int h = 32;

			// Draw the title.
			int y = menudbox_Ypos + 129 + ((34 - h) / 2);
			int w = 0;
			int x = ((2 - w) / 2) + buttons[3].x;
			pp2d_draw_wtext(x, y, 0.50, 0.50, BLACK, title);
			y += 16;

			// Draw the value.
			// Show the RGB value.
			char rgb_str[32];
			snprintf(rgb_str, sizeof(rgb_str), "%d, %d, %d",
				settings.pergame.red,
				settings.pergame.green,
				settings.pergame.blue);
			w = 0;
			x = ((2 - w) / 2) + buttons[3].x;

			// Print the RGB value using its color.
			const u32 color = RGBA8(settings.pergame.red,
				settings.pergame.green,
				settings.pergame.blue, 255);
			pp2d_draw_text(x, y, 0.50, 0.50, color, rgb_str);
		} else if ((SDKVersion > 0x5000000 && SDKVersion < 0x6000000) || settings.twl.forwarder) {
			if(settings.ui.cursorPosition >= 0 && (SDKVersion > 0x5000000 && SDKVersion < 0x6000000)) {
				pp2d_draw_text(162, menudbox_Ypos + 169, 0.50, 0.50, BLACK, SDKnumbertext);
			}

			for (int i = (int)(sizeof(buttons)/sizeof(buttons[0]))-1; i >= 0; i -= 3) {
				if (gamesettings_cursorPosition == i) {
					// Button is highlighted.
					pp2d_draw_texture(dboxtex_button, buttons[i].x, menudbox_Ypos+buttons[i].y);
				} else {
					// Button is not highlighted. Darken the texture.
					pp2d_draw_texture_blend(dboxtex_button, buttons[i].x, menudbox_Ypos+buttons[i].y, RGBA8(127, 127, 127, 255));
				}

				const wchar_t *title = buttons[i].title;
				const wchar_t *value_desc = TR(STR_START_DEFAULT);
				if (i == 0 || i == 2) {
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
				} else if (i == 1) {
					switch (*(buttons[i].value)) {
						case 0:
						default:
							value_desc = buttons[i].value_desc[0];
							break;
						case 1:
							value_desc = buttons[i].value_desc[1];
							break;
						case 2:
							value_desc = buttons[i].value_desc[2];
							break;
					}
				}

				// Determine the text height.
				// NOTE: Button texture size is 132x34.
				const int h = 32;

				// Draw the title.
				int y = menudbox_Ypos + buttons[i].y + ((34 - h) / 2);
				int w = 0;
				int x = ((2 - w) / 2) + buttons[i].x;
				pp2d_draw_wtext(x, y, 0.50, 0.50, BLACK, title);
				y += 16;

				// Draw the value.
				if (i < 3) {
					w = 0;
					x = ((2 - w) / 2) + buttons[i].x;
					pp2d_draw_wtext(x, y, 0.50, 0.50, GRAY, value_desc);
				} else if (i == 3) {
					// Show the RGB value.
					char rgb_str[32];
					snprintf(rgb_str, sizeof(rgb_str), "%d, %d, %d",
						settings.pergame.red,
						settings.pergame.green,
						settings.pergame.blue);
					w = 0;
					x = ((2 - w) / 2) + buttons[i].x;

					// Print the RGB value using its color.
					const u32 color = RGBA8(settings.pergame.red,
						settings.pergame.green,
						settings.pergame.blue, 255);
					pp2d_draw_text(x, y, 0.50, 0.50, color, rgb_str);
				}
			}
		} else {
			if(settings.ui.cursorPosition >= 0) {
				pp2d_draw_text(162, menudbox_Ypos + 169, 0.50, 0.50, BLACK, SDKnumbertext);
			}
		
			for (int i = (int)(sizeof(buttons)/sizeof(buttons[0]))-1; i >= 0; i--) {
				if (gamesettings_cursorPosition == i) {
					// Button is highlighted.
					pp2d_draw_texture(dboxtex_button, buttons[i].x, menudbox_Ypos+buttons[i].y);
				} else {
					// Button is not highlighted. Darken the texture.
					pp2d_draw_texture_blend(dboxtex_button, buttons[i].x, menudbox_Ypos+buttons[i].y, RGBA8(127, 127, 127, 255));
				}

				const wchar_t *title = buttons[i].title;
				const wchar_t *value_desc = TR(STR_START_DEFAULT);
				if (i == 0 || i == 2) {
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
				} else if (i == 1) {
					switch (*(buttons[i].value)) {
						case 0:
						default:
							value_desc = buttons[i].value_desc[0];
							break;
						case 1:
							value_desc = buttons[i].value_desc[1];
							break;
						case 2:
							value_desc = buttons[i].value_desc[2];
							break;
					}
				}

				// Determine the text height.
				// NOTE: Button texture size is 132x34.
				const int h = 32;

				// Draw the title.
				int y = menudbox_Ypos + buttons[i].y + ((34 - h) / 2);
				int w = 0;
				int x = ((2 - w) / 2) + buttons[i].x;
				pp2d_draw_wtext(x, y, 0.50, 0.50, BLACK, title);
				y += 16;

				// Draw the value.
				if (i < 3) {
					w = 0;
					x = ((2 - w) / 2) + buttons[i].x;
					pp2d_draw_wtext(x, y, 0.50, 0.50, GRAY, value_desc);
				} else if (i == 3) {
					// Show the RGB value.
					char rgb_str[32];
					snprintf(rgb_str, sizeof(rgb_str), "%d, %d, %d",
						settings.pergame.red,
						settings.pergame.green,
						settings.pergame.blue);
					w = 0;
					x = ((2 - w) / 2) + buttons[i].x;

					// Print the RGB value using its color.
					const u32 color = RGBA8(settings.pergame.red,
						settings.pergame.green,
						settings.pergame.blue, 255);
					pp2d_draw_text(x, y, 0.50, 0.50, color, rgb_str);
				}
			}
		}
	} else if (menudboxmode == DBOX_MODE_ROMTYPE) {
		pp2d_draw_wtext(240, menudbox_Ypos+199, 0.50, 0.50, BLACK, TR(STR_BACK));

		// UI options.
		const struct {
			int x;
			int y;
			const char *title;
			const wchar_t *value_desc;
		} buttons[] = {
			{ 23,  31, "Nintendo DS/DSi", NULL},
			{161,  31, "GameBoy Advance", TR(STR_START_START_GBARUNNER2)},
			{ 23,  71, "GameBoy/SGB/GBC", NULL},
			{161,  71, "NES", NULL},
		};

		for (int i = (int)(sizeof(buttons)/sizeof(buttons[0])) - 1; i >= 0; i--) {
			if (setromtype_cursorPosition == i) {
				// Button is highlighted.
				pp2d_draw_texture(dboxtex_button, buttons[i].x, menudbox_Ypos+buttons[i].y);
			} else {
				// Button is not highlighted. Darken the texture.
				pp2d_draw_texture_blend(dboxtex_button, buttons[i].x, menudbox_Ypos+buttons[i].y, GRAY);
			}

			const char *title = buttons[i].title;
			const wchar_t *value_desc = buttons[i].value_desc;

			// Determine width and height.
			const int h = (title && value_desc ? 32 : 16);

			// Draw the text.
			// NOTE: Button texture size is 132x34.
			int y = menudbox_Ypos + buttons[i].y + ((34 - h) / 2);
			if (title) {
				const int w = 0;
				const int x = ((2 - w) / 2) + buttons[i].x;
				pp2d_draw_text(x, y, 0.50, 0.50, BLACK, title);
				y += 16;
			}
			if (value_desc) {
				const int w = 0;
				const int x = ((2 - w) / 2) + buttons[i].x;
				pp2d_draw_wtext(x, y, 0.50, 0.50, BLACK, value_desc);
			}
		}
	} else {
		pp2d_draw_wtext(240, menudbox_Ypos+199, 0.50, 0.50, BLACK, TR(STR_BACK));

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
			{ 23,  71, &isDemo, TR(STR_START_SELECT_ROMTYPE), {NULL, NULL}},
			{161,  71, &settings.ui.topborder, NULL, {TR(STR_START_TOP_BORDER_OFF), TR(STR_START_TOP_BORDER_ON)}},
			{ 23, 111, NULL, TR(STR_START_UNSET_DONOR), {NULL, NULL}},
			{161, 111, NULL, TR(STR_START_SEARCH), {NULL, NULL}},
			{ 23, 151, NULL, TR(STR_SETTINGS_TEXT), {NULL, NULL}},
		};

		for (int i = (int)(sizeof(buttons)/sizeof(buttons[0])) - 1; i >= 0; i--) {
			if (startmenu_cursorPosition == i) {
				// Button is highlighted.
				pp2d_draw_texture(dboxtex_button, buttons[i].x, menudbox_Ypos+buttons[i].y);
			} else {
				// Button is not highlighted. Darken the texture.
				pp2d_draw_texture_blend(dboxtex_button, buttons[i].x, menudbox_Ypos+buttons[i].y, GRAY);
			}

			const wchar_t *title = buttons[i].title;
			const wchar_t *value_desc = (buttons[i].value ? buttons[i].value_desc[!!*(buttons[i].value)] : NULL);

			// Determine width and height.
			const int h = (title && value_desc ? 32 : 16);

			// Draw the text.
			// NOTE: Button texture size is 132x34.
			int y = menudbox_Ypos + buttons[i].y + ((34 - h) / 2);
			if (title) {
				const int w = 0;
				const int x = ((2 - w) / 2) + buttons[i].x;
				pp2d_draw_wtext(x, y, 0.50, 0.50, BLACK, title);
				y += 16;
			}
			if (value_desc) {
				const int w = 0;
				const int x = ((2 - w) / 2) + buttons[i].x;
				pp2d_draw_wtext(x, y, 0.50, 0.50, BLACK, value_desc);
			}
		}
	}
}

// TWLNAND side Title ID.
extern const u64 TWLNAND_TID;
const u64 TWLNAND_TID = 0x0004800554574C44ULL;

// TWLNAND side (TWL touch mode) Title ID.
extern const u64 TWLNANDTWLTOUCH_TID;
const u64 TWLNANDTWLTOUCH_TID = 0x0004800554434C44ULL;

// TWLoader's NTR Launcher Title ID.
extern const u64 NTRLAUNCHER_TID;
const u64 NTRLAUNCHER_TID = 0x0004800554574C31ULL;

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

/**
* Check if the TWLNAND-side (part 1.1) title is installed or not
* Title ID: 0x0004800554434C44ULL
* MediaType: MEDIATYPE_NAND
* @return: true if installed, false if not
*/
bool checkTWLNANDSide1(void) {
	u64 tid = TWLNANDTWLTOUCH_TID;
	AM_TitleEntry entry;
	return R_SUCCEEDED(AM_GetTitleInfo(MEDIATYPE_NAND, 1, &tid, &entry));
}

/**
* Check if the TWLNAND-side (part 2) title is installed or not
* Title ID: 0x0004800554574C31ULL
* MediaType: MEDIATYPE_NAND
* @return: true if installed, false if not
*/
bool checkTWLNANDSide2(void) {
	u64 tid = NTRLAUNCHER_TID;
	AM_TitleEntry entry;
	return R_SUCCEEDED(AM_GetTitleInfo(MEDIATYPE_NAND, 1, &tid, &entry));
}

void deletemode_internal(RomLocation location, std::string del_rom) {
	char path[256];

	if (location == ROM_FLASHCARD) {
		if (logEnabled)	LogFM("Delete mode enabled (flashcard)", del_rom.c_str());
		snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.fcromfolder.c_str(), del_rom.c_str());
		fcfiles.erase(fcfiles.begin() + settings.ui.cursorPosition);
		
		CIniFile setfcrompathini(path);
		std::string ba_TIDini = setfcrompathini.GetString(fcrompathini_flashcardrom, fcrompathini_tid, "");

		char ba_TID[5];
		strcpy(ba_TID, ba_TIDini.c_str());
		ba_TID[4] = 0;
		
		remove(path); // Remove .ini after reading TID.
		if (logEnabled)	LogFMA("Delete mode", ".ini file deleted", path);

		snprintf(path, sizeof(path), "%s/%s.png", fcboxartfolder, del_rom.c_str());
		if (logEnabled)	LogFM("Trying to delete boxart by filename", path);
		if (access(path, F_OK) != -1) {
			remove(path); // Remove .png
			if (logEnabled)	LogFM("Delete mode", ".png file deleted (filename)");
		} else {
			snprintf(path, sizeof(path), "%s/%.4s.png", fcboxartfolder, ba_TID);
			if (logEnabled)	LogFM("Trying to delete boxart by TID instead", path);
			if (access(path, F_OK) != -1) {
				remove(path); // Remove .png
				if (logEnabled)	LogFM("Delete mode", ".png file deleted (TID)");
			} else {
				if (logEnabled)	LogFMA("Delete mode", "Boxart not found, skipping", del_rom.c_str());
			}
		}
		
		snprintf(path, sizeof(path), "%s/%s.bin", fcbnriconfolder, del_rom.c_str());
		if (logEnabled)	LogFM("Trying to delete banner data", path);
		remove(path); // Remove the .bin
		if (logEnabled)	LogFM("Delete mode", ".bin file deleted");
	} else if (location == ROM_SD) {
		if (logEnabled)	LogFM("Delete mode enabled (SD)", del_rom.c_str());
		bool isCia = false;
		if(del_rom.substr(del_rom.find_last_of(".") + 1) == "cia") isCia = true;
		snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.romfolder.c_str(), del_rom.c_str());
		files.erase(files.begin() + settings.ui.cursorPosition);
		
		FILE *f_nds_file = fopen(path, "rb");
		
		char ba_TID[5];
		grabTID(f_nds_file, ba_TID, isCia);
		ba_TID[4] = 0;
		fclose(f_nds_file);
		
		remove(path); // Remove .nds after reading TID.
		if (logEnabled)	LogFMA("Delete mode", ".nds file deleted", path);								
		snprintf(path, sizeof(path), "%s/%s.png",boxartfolder, del_rom.c_str());
		if (logEnabled)	LogFM("Trying to delete boxart by filename", path);
		if (access(path, F_OK) != -1) {
			remove(path); // Remove .png
			if (logEnabled)	LogFM("Delete mode", ".png file deleted (filename)");
		} else {
			snprintf(path, sizeof(path), "%s/%.4s.png",boxartfolder, ba_TID);
			if (logEnabled)	LogFM("Trying to delete boxart by TID instead", path);
			if (access(path, F_OK) != -1) {
				remove(path); // Remove .png
				if (logEnabled)	LogFM("Delete mode", ".png file deleted (TID)");
			} else {
				if (logEnabled)	LogFMA("Delete mode", "Boxart not found, skipping", del_rom.c_str());
			}
		}								
		snprintf(path, sizeof(path), "%s/%s.bin", bnriconfolder, del_rom.c_str());
		if (logEnabled)	LogFM("Trying to delete banner data", path);
		remove(path); // Remove the .bin
		if (logEnabled)	LogFM("Delete mode", ".bin file deleted");
	}
}

void dsiMenuTheme_loadingScreen() {
	pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
	if (settings.ui.custombot == 1)
		pp2d_draw_texture(bottomtex, 320/2 - 320/2, 240/2 - 240/2);
	else
		pp2d_draw_texture_blend(bottomtex, 320/2 - 320/2, 240/2 - 240/2, menucolor);

	pp2d_draw_texture(scrollbartex, 0, 240-28);
	pp2d_draw_texture_blend(buttonarrowtex, 0, 240-28, SET_ALPHA(color_data->color, 255));
	pp2d_draw_texture_rotate_flip_blend(buttonarrowtex, 320-25, 240-28, 180.0f, VERTICAL, SET_ALPHA(color_data->color, 255));

	pp2d_draw_texture(bubbletex, 0, 0);
	pp2d_draw_text(8, 38, 0.70, 0.70, BLACK, "Loading...");
	pp2d_end_draw();
}

static bool boxarttexloaded = false;
static int pagemax_ba = 0;

void menuLoadBoxArt() {
	if (!boxarttexloaded && !settings.romselect.toplayout && TWLNANDnotfound_msg == 2 && fadealpha == 0) {
		if (settings.twl.romtype == 0) {
			if (!settings.twl.forwarder) {
				char path[256];
				if(matching_files.size() == 0){
					if (loadboxartnum < pagemax_ba) {
						if (loadboxartnum < (int)files.size()) {
							bool isCia = false;
							const char *tempfile = files.at(loadboxartnum).c_str();
							std::string fn = tempfile;
							if(fn.substr(fn.find_last_of(".") + 1) == "cia") isCia = true;
							snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.romfolder.c_str(), tempfile);
							FILE *f_nds_file = fopen(path, "rb");
							if (!f_nds_file) {
								// Can't open the NDS file.
								if(logEnabled) LogFMA("main.loadBoxArts", "Error opening nds file", path);
								StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
							} else {
								char ba_TID[5];
								grabTID(f_nds_file, ba_TID, isCia);
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
							}
						} else {
							StoreBoxArtPath("romfs:/graphics/blank_128x115.png");
						}
					}
				}else{
					if (loadboxartnum < pagemax_ba) {
						if (loadboxartnum < (int)matching_files.size()) {
							bool isCia = false;
							const char *tempfile = matching_files.at(loadboxartnum).c_str();
							std::string fn = tempfile;
							if(fn.substr(fn.find_last_of(".") + 1) == "cia") isCia = true;
							snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.romfolder.c_str(), tempfile);
							FILE *f_nds_file = fopen(path, "rb");
							if (!f_nds_file) {
								// Can't open the NDS file.
								if(logEnabled) LogFMA("main.loadBoxArts", "Error opening nds file", path);
								StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
							} else {
								char ba_TID[5];
								grabTID(f_nds_file, ba_TID, isCia);
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
							}
						} else {
							StoreBoxArtPath("romfs:/graphics/blank_128x115.png");
						}
					}
				}
			} else {
				char path[256];
				if(matching_files.size() == 0){
					if (loadboxartnum < pagemax_ba) {
						if (loadboxartnum < (int)fcfiles.size()) {
							const char *tempfile = fcfiles.at(loadboxartnum).c_str();
							snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.fcromfolder.c_str(), tempfile);

							CIniFile setfcrompathini( path );
							std::string ba_TIDini = setfcrompathini.GetString(fcrompathini_flashcardrom, fcrompathini_tid, "");
							if (ba_TIDini.size() < 4) {
								// TID is too short.
								StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
							} else {
								char ba_TID[5];
								strcpy(ba_TID, ba_TIDini.c_str());
								ba_TID[4] = 0;

								// example: SuperMario64DS.nds.png
								snprintf(path, sizeof(path), "%s/%s.png", fcboxartfolder, tempfile);
								if (access(path, F_OK ) != -1 ) {
									StoreBoxArtPath(path);
								} else {
									// example: ASME.png
									// check first if boxart exist on fcboxartfolder by TID
									memset(path, 0, sizeof(path));
									snprintf(path, sizeof(path), "%s/%.4s.png", fcboxartfolder, ba_TID);
									if (access(path, F_OK ) != -1 ) {
										StoreBoxArtPath(path);
									} else {
										// Boxart doesn't exist in fxboxartfolder neither by name nor by TID
										// maybe on boxart (sd) folder?
										memset(path, 0, sizeof(path));
										snprintf(path, sizeof(path), "%s/%.4s.png", boxartfolder, ba_TID);
										if (access(path, F_OK) != -1) {
											StoreBoxArtPath(path);
										} else {
											StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
										}
									}
								}
							}
						} else {
							StoreBoxArtPath("romfs:/graphics/blank_128x115.png");
						}
					}
				}else{
					if (loadboxartnum < pagemax_ba) {
						if (loadboxartnum < (int)matching_files.size()) {
							const char *tempfile = matching_files.at(loadboxartnum).c_str();
							snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.fcromfolder.c_str(), tempfile);

							CIniFile setfcrompathini( path );
							std::string ba_TIDini = setfcrompathini.GetString(fcrompathini_flashcardrom, fcrompathini_tid, "");
							if (ba_TIDini.size() < 4) {
								// TID is too short.
								StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
							} else {
								char ba_TID[5];
								strcpy(ba_TID, ba_TIDini.c_str());
								ba_TID[4] = 0;

								// example: SuperMario64DS.nds.png
								snprintf(path, sizeof(path), "%s/%s.png", fcboxartfolder, tempfile);
								if (access(path, F_OK ) != -1 ) {
									StoreBoxArtPath(path);
								} else {
									// example: ASME.png
									// check first if boxart exist on fcboxartfolder by TID
									memset(path, 0, sizeof(path)); 
									snprintf(path, sizeof(path), "%s/%.4s.png", fcboxartfolder, ba_TID);
									if (access(path, F_OK ) != -1 ) {
										StoreBoxArtPath(path);
									} else {
										// Boxart doesn't exist in fxboxartfolder neither by name nor by TID
										// maybe on boxart (sd) folder?
										memset(path, 0, sizeof(path));
										snprintf(path, sizeof(path), "%s/%.4s.png", boxartfolder, ba_TID);
										if (access(path, F_OK) != -1) {
											StoreBoxArtPath(path);
										} else {
											StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
										}
									}
								}
							}
						} else {
							StoreBoxArtPath("romfs:/graphics/blank_128x115.png");
						}
					}
				}
			}
		} else if (settings.twl.romtype == 1) {
			char path[256];
			if(matching_files.size() == 0){
				if (loadboxartnum < pagemax_ba) {
					if (loadboxartnum < (int)gbfiles.size()) {
						const char *tempfile = gbfiles.at(loadboxartnum).c_str();
						snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.gbromfolder.c_str(), tempfile);

						// example: SuperMarioLand.gb.png
						snprintf(path, sizeof(path), "%s/%s.png", gbboxartfolder, tempfile);
						if (access(path, F_OK ) != -1 ) {
							StoreBoxArtPath(path);
						} else {
							StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
						}
					} else {
						StoreBoxArtPath("romfs:/graphics/blank_128x115.png");
					}
				}
			}else{
				if (loadboxartnum < pagemax_ba) {
					if (loadboxartnum < (int)matching_files.size()) {
						const char *tempfile = matching_files.at(loadboxartnum).c_str();
						snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.gbromfolder.c_str(), tempfile);

						// example: SuperMarioLand.gb.png
						snprintf(path, sizeof(path), "%s/%s.png", gbboxartfolder, tempfile);
						if (access(path, F_OK ) != -1 ) {
							StoreBoxArtPath(path);
						} else {
							StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
						}
					} else {
						StoreBoxArtPath("romfs:/graphics/blank_128x115.png");
					}
				}
			}
		} else if (settings.twl.romtype == 2) {
			char path[256];
			if(matching_files.size() == 0){
				if (loadboxartnum < pagemax_ba) {
					if (loadboxartnum < (int)nesfiles.size()) {
						const char *tempfile = nesfiles.at(loadboxartnum).c_str();
						snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.nesromfolder.c_str(), tempfile);

						// example: SuperMarioBros.nes.png
						snprintf(path, sizeof(path), "%s/%s.png", nesboxartfolder, tempfile);
						if (access(path, F_OK ) != -1 ) {
							StoreBoxArtPath(path);
						} else {
							StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
						}
					} else {
						StoreBoxArtPath("romfs:/graphics/blank_128x115.png");
					}
				}
			}else{
				if (loadboxartnum < pagemax_ba) {
					if (loadboxartnum < (int)matching_files.size()) {
						const char *tempfile = matching_files.at(loadboxartnum).c_str();
						snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.nesromfolder.c_str(), tempfile);

						// example: SuperMarioBros.nes.png
						snprintf(path, sizeof(path), "%s/%s.png", nesboxartfolder, tempfile);
						if (access(path, F_OK ) != -1 ) {
							StoreBoxArtPath(path);
						} else {
							StoreBoxArtPath("romfs:/graphics/boxart_unknown.png");
						}
					} else {
						StoreBoxArtPath("romfs:/graphics/blank_128x115.png");
					}
				}
			}
		}

		loadboxartnum++;
		if (loadboxartnum == pagemax_ba) {
			// Load up to 6 boxarts.
			for (int i = 0+settings.ui.pagenum*gamesPerPage; i < 6+settings.ui.pagenum*gamesPerPage; i++) {
				boxartnum = i+boxartpage*3;
				LoadBoxArt();
			}
			boxarttexloaded = true;
		}
		boxartnum = 0+settings.ui.pagenum*gamesPerPage;
	}
}

bool showAnniversaryText = false;

int main(){
	botscreenoff();

	pp2d_init();
	
	pp2d_set_screen_color(GFX_TOP, TRANSPARENT);
	pp2d_set_3D(0);
	
	Result res = 0;

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
	
	int aninumfadealpha = 0;
	int showAnniversaryTextYPos = 100;

	if(isDemo)
		pp2d_load_texture_png(twloaderlogotex, "romfs:/graphics/logo/logo_demo.png"); // TWLoader (3DSX demo version) logo on top screen
	else
		pp2d_load_texture_png(twloaderlogotex, "romfs:/graphics/logo/logo.png"); // TWLoader logo on top screen
	pp2d_load_texture_png(anniversarytex, "romfs:/graphics/anniversary/text.png"); // TWLoader logo on top screen
	if(showAnniversaryText) {
		for(int i = 0; i < 64; i++) {
			pp2d_begin_draw(GFX_TOP, GFX_LEFT);		
			pp2d_draw_texture_part_blend(anniversarytex, 0, 40, 0, 0, 160, 40, RGBA8(255, 255, 255, aninumfadealpha));
			pp2d_draw_texture_part(anniversarytex, 160, showAnniversaryTextYPos, 160, 0, 240, 40);
			pp2d_draw_texture(twloaderlogotex, 400/2 - 256/2, 240/2 - 128/2); // 400/2 - height/2, 240/2 - width/2
			pp2d_end_draw();
			aninumfadealpha += 4;
			if(aninumfadealpha > 255) aninumfadealpha = 255;
			showAnniversaryTextYPos--;
			if(showAnniversaryTextYPos < 40) showAnniversaryTextYPos = 40;
		}
	}

	pp2d_begin_draw(GFX_TOP, GFX_LEFT);		
	if(showAnniversaryText) {
		pp2d_draw_texture(anniversarytex, 0, 40);
		pp2d_draw_texture(twloaderlogotex, 400/2 - 256/2, 240/2 - 128/2); // 400/2 - height/2, 240/2 - width/2
	} else {
		pp2d_draw_texture(twloaderlogotex, 400/2 - 256/2, 240/2 - 128/2); // 400/2 - height/2, 240/2 - width/2
	}
	
	int filenum = 0;
	bool noromsfound = false;
	
	bool cursorPositionset = false;
	
	int soundwaittimer = 0;
	bool playwrongsounddone = false;

	bool colortexloaded = false;
	bool colortexloaded_bot = false;
	bool bnricontexloaded = false;

	bool updatetopscreen = true;
	bool screenmodeswitch = false;
	
	float rad = 0.0f;
	
	int boxartXpos;
	int boxartXmovepos = 0;

	int filenameYpos;
	int filenameYmovepos = 0;
	int setsboxXpos = 0;
	int cartXpos = 64;
	int boxartYmovepos = 63;
	int boxartreflYmovepos = 178;
	int ndsiconXpos;
	int ndsiconYmovepos = 129;
	int wood_ndsiconscaletimer = 0;
	int wood_ndsiconscalelag = 0;
	int wood_ndsiconscalemovepos = 0;
	float wood_ndsiconscalesize = 0.00f;
	bool wood_ndsiconscaledown = false;

	int startbordermovepos = 0;
	float startborderscalesize = 1.0f;

	bool bannertextloaded = false;	
	// Register a handler for "returned from HOME Menu".
	aptHook(&rfhm_cookie, rfhm_callback, &bannertextloaded);
	
	if(isNightly){
		logEnabled = true;
		char nightlyhash[16];
		snprintf(nightlyhash, 16, "%s", NIGHTLY);
		LogFMA("Welcome to nightly channel!", "Version:", nightlyhash);
		Log("********************************************************\n");	
	} else {
		/* Log file is dissabled by default. If _nds/twloader/log exist, we turn log file on, else, log is dissabled */
		struct stat logBuf;
		logEnabled = stat("sdmc:/_nds/twloader/log", &logBuf) == 0;
		/* Log configuration file end */
		if (logEnabled)	createLog();
	}		

	// make folders if they don't exist
	mkdir("sdmc:/3ds", 0777);	// For DSP dump
	mkdir("sdmc:/_nds", 0777);
	mkdir("sdmc:/_nds/twloader", 0777);
	mkdir("sdmc:/_nds/twloader/bnricons", 0777);
	mkdir("sdmc:/_nds/twloader/bnricons/flashcard", 0777);
	mkdir("sdmc:/_nds/twloader/boxart", 0777);
	mkdir("sdmc:/_nds/twloader/boxart/flashcard", 0777);
	mkdir("sdmc:/_nds/twloader/boxart/slot1", 0777);
	mkdir("sdmc:/_nds/twloader/gamesettings", 0777);
	mkdir("sdmc:/_nds/twloader/gamesettings/flashcard", 0777);
	mkdir("sdmc:/_nds/twloader/loadflashcard", 0777);
	
	snprintf(settings_vertext, 13, "Ver. %d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);
	std::string version = settings_vertext;	
	int vertext_xPos;
	if (version.substr(version.find_first_not_of(' '), (version.find_last_not_of(' ') - version.find_first_not_of(' ') + 1)).size() > 8) {
		vertext_xPos = 324;
	}else{
		vertext_xPos = 336;
	}
	
	pp2d_draw_text(vertext_xPos, 222, 0.60, 0.60f, WHITE, settings_vertext);
	pp2d_end_draw();
	
	if (logEnabled)	LogFMA("Main.GUI version", "GUI version", settings_vertext);
	
	aninumfadealpha = 255;
	bool botscreenon_ran = false;

	pp2d_load_texture_png(bottomlogotex, "romfs:/graphics/pseudoHDRlogo.png");

	for(int i = 0; i < 60*3; i++) {
		if(i <= 30) {
			aninumfadealpha -= 25;
			if(aninumfadealpha < 0) aninumfadealpha = 0;
		} else if(i >= 160) {
			aninumfadealpha += 25;
			if(aninumfadealpha > 255) aninumfadealpha = 255;
		}
		pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);	
		pp2d_draw_texture(bottomlogotex, 0, 0);
		pp2d_draw_text(32, 48, 0.60, 0.60f, WHITE, "Enhanced with");
		pp2d_draw_text(32, 192, 0.50, 0.50f, WHITE, "*Games not enhanced with pseudo-HDR");
		pp2d_draw_rectangle(0, 0, 320, 240, RGBA8(0, 0, 0, aninumfadealpha));
		pp2d_end_draw();
		if(!botscreenon_ran) {
			botscreenon();
			botscreenon_ran = true;
		}
	}
	botscreenoff();
	
	pp2d_free_texture(bottomlogotex);

	/** Speed up New 3DS only. **/
	bool isNew = 0;
	res = 0; // prev. result
	if(R_SUCCEEDED(res = APT_CheckNew3DS(&isNew))) {
		if (isNew) osSetSpeedupEnable(true);	// Enable speed-up for New 3DS users
	}
	/** Speedup set up correctly. **/
	
	LoadSettings();	
	if (settings.twl.bootstrapfile == 1) {
		bootstrapPath = "sd:/_nds/unofficial-bootstrap.nds";
	} else {
		bootstrapPath = "sd:/_nds/release-bootstrap.nds";
	}
	if (logEnabled) LogFMA("Main.bootstrapPath", "Using bootstrap:", bootstrapPath.c_str());
	LoadBootstrapConfig();

	// Store bootstrap version
	checkBootstrapVersion();
	
	// Initialize translations.
	langInit();	
	
	LoadColor();
	LoadMenuColor();
	LoadBottomImage();
	
	pp2d_set_texture_filter(GPU_LINEAR, GPU_NEAREST);

	/* pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
	pp2d_draw_text(12, 16, 0.5f, 0.5f, WHITE, "Loading textures...");
	pp2d_end_draw(); */
	
	if (logEnabled)	LogFM("Main.Textures", "Textures loading.");

	// Dialog box textures.
	pp2d_load_texture_png(dialogboxtex, "romfs:/graphics/dialogbox.png"); // Dialog box
	pp2d_load_texture_png(dboxtex_iconbox, "romfs:/graphics/dbox/iconbox.png"); // Icon box
	pp2d_load_texture_png(dboxtex_button, "romfs:/graphics/dbox/button.png"); // Icon box
	pp2d_load_texture_png(dboxtex_buttonback, "romfs:/graphics/dbox/button_back.png"); // Icon box

	pp2d_load_texture_png(r4loadingtex, "romfs:/graphics/r4/loading.png");		// R4 "Loading..." screen
	pp2d_load_texture_png(toptex, "romfs:/graphics/top.png"); // Top DSi-Menu border

	// Volume slider textures.
	pp2d_load_texture_png(voltex, "romfs:/graphics/volume.png");
	pp2d_load_texture_png(setvoltex, "romfs:/graphics/settings/volume.png");

	pp2d_load_texture_png(shouldertex, "romfs:/graphics/shoulder.png"); // Shoulder button
	pp2d_load_texture_png(_3dsshouldertex, "romfs:/graphics/3ds/shoulder.png"); // 3DS HOME Menu shoulder

	// Battery level textures.
	pp2d_load_texture_png(batterytex, "romfs:/graphics/battery.png");

	if(!isDemo) {
		pp2d_load_texture_png(settingslogotex, "romfs:/graphics/settings/logo.png"); // TWLoader logo in settings screen.
		pp2d_load_texture_png(settingslogotwltex, "romfs:/graphics/settings/logo_twl.png");
	} else {
		pp2d_load_texture_png(settingslogotex, "romfs:/graphics/settings/logo_demo.png"); // TWLoader logo in settings screen.
		pp2d_load_texture_png(settingslogotwltex, "romfs:/graphics/settings/logo_demo_twl.png");
		pp2d_load_texture_png(settingslogodemotex, "romfs:/graphics/settings/logo_demo_demo.png");
	}
	pp2d_load_texture_png(settingslogooadertex, "romfs:/graphics/settings/logo_oader.png");

	pp2d_load_texture_png(sdicontex, "romfs:/graphics/wood/sd.png");
	pp2d_load_texture_png(flashcardicontex, "romfs:/graphics/wood/flashcard.png");
	pp2d_load_texture_png(gbaicontex, "romfs:/graphics/wood/gba.png");
	pp2d_load_texture_png(smallsettingsicontex, "romfs:/graphics/wood/settings.png");
	pp2d_load_texture_png(iconnulltex, "romfs:/graphics/icon_null.png"); // Slot-1 cart icon if no cart is present
	pp2d_load_texture_png(homeicontex, "romfs:/graphics/homeicon.png"); // HOME icon
	pp2d_load_texture_png(bottomlogotex, "romfs:/graphics/bottom_logo.png"); // TWLoader logo on bottom screen
	pp2d_load_texture_png(scrollbartex, "romfs:/graphics/scrollbar.png"); // Scroll bar on bottom screen
	pp2d_load_texture_png(buttonarrowtex, "romfs:/graphics/button_arrow.png"); // Arrow button for scroll bar
	pp2d_load_texture_png(bipstex, "romfs:/graphics/bips.png"); // Little dots of scroll bar
	pp2d_load_texture_png(scrollwindowtex, "romfs:/graphics/scroll_window.png"); // Window behind dots of scroll bar
	pp2d_load_texture_png(scrollwindowfronttex, "romfs:/graphics/scroll_windowfront.png"); // Front of window for scroll bar
	pp2d_load_texture_png(settingsicontex, "romfs:/graphics/settingsbox.png"); // Settings box on bottom screen
	pp2d_load_texture_png(getfcgameboxtex, "romfs:/graphics/getfcgamebox.png");
	pp2d_load_texture_png(cartnulltex, "romfs:/graphics/cart_null.png"); // NTR cartridge
	pp2d_load_texture_png(cartntrtex, "romfs:/graphics/cart_ntr.png"); // NTR cartridge
	pp2d_load_texture_png(carttwltex, "romfs:/graphics/cart_twl.png"); // TWL cartridge
	pp2d_load_texture_png(cartctrtex, "romfs:/graphics/cart_ctr.png"); // CTR cartridge
	pp2d_load_texture_png(_3dsbottopbartex, "romfs:/graphics/3ds/bot_topbar.png");
	pp2d_load_texture_png(_3dsbotbotbartex, "romfs:/graphics/3ds/bot_botbar.png");
	pp2d_load_texture_png(_3dsbotbotbarbuttex, "romfs:/graphics/3ds/bot_botbarbut.png");
	pp2d_load_texture_png(bracetex, "romfs:/graphics/brace.png"); // Brace (C-shaped thingy)
	pp2d_load_texture_png(gbctex, "romfs:/graphics/icon_gbc.png"); // GBC icon (from SRLoader)
	pp2d_load_texture_png(nestex, "romfs:/graphics/icon_nes.png"); // NES icon (from SRLoader)

	if (logEnabled)	LogFM("Main.Textures", "Textures loaded.");

	dspfirmfound = false;
 	if( access( "sdmc:/3ds/dspfirm.cdc", F_OK ) != -1 ) {
		ndspInit();
		dspfirmfound = true;
		/* pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
		pp2d_draw_text(12, 16, 0.5f, 0.5f, WHITE, "DSP Firm found!");
		pp2d_end_draw(); */
		if (logEnabled)	LogFM("Main.dspfirm", "DSP Firm found!");
	}else{
		if (logEnabled)	LogFM("Main.dspfirm", "DSP Firm not found. Dumping DSP...");
		pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
		pp2d_draw_text(12, 16, 0.5f, 0.5f, WHITE, "Dumping DSP firm...");
		pp2d_end_draw();
		botscreenon();
		dumpDsp();
		if( access( "sdmc:/3ds/dspfirm.cdc", F_OK ) != -1 ) {
			ndspInit();
			dspfirmfound = true;
		} else {
			if (logEnabled)	LogFM("Main.dspfirm", "DSP Firm dumping failed.");
			settings.ui.showbootscreen = 0;
			
			for (int i = 0; i < 90; i++) {
				pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
				if (!isDemo) {
					pp2d_draw_text(12, 16, 0.5f, 0.5f, WHITE, "DSP firm dumping failed.\n"
						"Running without sound.\n"
						"(NTR/TWL mode will still have sound.)");
				} else {
					pp2d_draw_text(12, 16, 0.5f, 0.5f, WHITE, "DSP firm dumping failed.\n"
						"Running without sound.");
				}
				pp2d_end_draw();
			}
		}
		botscreenoff();
	}

	bool musicbool = false;
	if( access( "sdmc:/_nds/twloader/music.wav", F_OK ) != -1 ) {
		musicpath = "sdmc:/_nds/twloader/music.wav";
		/* pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
		pp2d_draw_text(12, 16, 0.5f, 0.5f, WHITE, "Custom music file found!");
		pp2d_end_draw(); */
		if (logEnabled)	LogFM("Main.music", "Custom music file found!");
	}else {
		if (logEnabled)	LogFM("Main.dspfirm", "No music file found.");
	}

	// Load the sound effects if DSP is available.
	if (dspfirmfound) {
		/* pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
		pp2d_draw_text(12, 16, 0.5f, 0.5f, WHITE, "Loading .wav files...");
		pp2d_end_draw(); */
		
		bgm_menu = new sound(musicpath);
		sfx_launch = new sound("romfs:/sounds/launch.wav", 2, false);
		sfx_select = new sound("romfs:/sounds/select.wav", 2, false);
		sfx_stop = new sound("romfs:/sounds/stop.wav", 2, false);
		sfx_switch = new sound("romfs:/sounds/switch.wav", 2, false);
		sfx_wrong = new sound("romfs:/sounds/wrong.wav", 2, false);
		sfx_back = new sound("romfs:/sounds/back.wav", 2, false);
	}

	// Scan hid shared memory for input events
	hidScanInput();

	const u32 hDown = hidKeysDown();
	const u32 hHeld = hidKeysHeld();

	if(hHeld & KEY_A) {
		settings.ui.quickStart = true;
	}

	/* pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
	pp2d_draw_text(12, 16, 0.5f, 0.5f, WHITE, "Scanning ROM directories...");
	pp2d_end_draw(); */
	
	// Scan the ROM directories.
	scanRomDirectories();

	char romsel_counter2sd[16];	// Number of ROMs on the SD card.
	snprintf(romsel_counter2sd, sizeof(romsel_counter2sd), "%zu", files.size());
	if (logEnabled)	LogFMA("Main.ROM scanning", "Number of ROMs on the SD card detected", romsel_counter2sd);
	
	char romsel_counter2fc[16];	// Number of ROMs on the flash card.
	snprintf(romsel_counter2fc, sizeof(romsel_counter2fc), "%zu", fcfiles.size());
	if (logEnabled)	LogFMA("Main.ROM scanning", "Number of ROMs on the flashcard detected", romsel_counter2fc);
	
	char romsel_counter2gb[16];	// Number of GB ROMs on the SD card.
	snprintf(romsel_counter2gb, sizeof(romsel_counter2gb), "%zu", gbfiles.size());
	if (logEnabled)	LogFMA("Main.ROM scanning", "Number of GB ROMs on the SD card detected", romsel_counter2gb);
	
	char romsel_counter2nes[16];	// Number of NES ROMs on the SD card.
	snprintf(romsel_counter2nes, sizeof(romsel_counter2nes), "%zu", nesfiles.size());
	if (logEnabled)	LogFMA("Main.ROM scanning", "Number of NES ROMs on the SD card detected", romsel_counter2nes);
	
	botscreenon();

	const char* wifiStuckMsg =
	"Checking WiFi status...\n"
	"\n"
	"If you see this for more than 25 seconds,\n"
	"try rebooting, then after launching TWLoader,\n"
	"hold  to skip downloading missing files.\n"
	"\n"
	"If the issue persists, reboot, then do the same,\n"
	"and also hold  to turn on quick start.";

	if(!(hHeld & KEY_Y)) {
		pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
		pp2d_draw_text(12, 16, 0.5f, 0.5f, WHITE, wifiStuckMsg);
		pp2d_end_draw();
			
		// Download missing files
		if (checkWifiStatus() && (DownloadMissingFiles() == 0)) {
			// Nothing
		}
	}

	if(!settings.ui.quickStart) {
		wifiStuckMsg =
		"Checking WiFi status...\n"
		"\n"
		"If you see this for more than 25 seconds,\n"
		"try rebooting, then after launching TWLoader,\n"
		"hold  to turn on quick start.";
	
		pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
		pp2d_draw_text(12, 16, 0.5f, 0.5f, WHITE, wifiStuckMsg);
		pp2d_end_draw();

		// Download box art
		if (checkWifiStatus()) {
			downloadBoxArt();
		}
	
		pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
		pp2d_draw_text(12, 16, 0.5f, 0.5f, WHITE, "Now checking banner data (SD Card)...");
		pp2d_end_draw();

		// Cache banner data for ROMs on the SD card.
		// TODO: Re-cache if it's 0 bytes?
		for (bnriconnum = 0; bnriconnum < (int)files.size(); bnriconnum++) {
			static const char title[] = "Now checking banner data (SD Card)...";
			char romsel_counter1[16];
			snprintf(romsel_counter1, sizeof(romsel_counter1), "%d", bnriconnum+1);
			bool isCia = false;
			const char *tempfile = files.at(bnriconnum).c_str();
			std::string fn = tempfile;
			if(fn.substr(fn.find_last_of(".") + 1) == "cia") isCia = true;

			wstring tempfile_w = utf8_to_wstring(tempfile);

			char nds_path[256];
			snprintf(nds_path, sizeof(nds_path), "sdmc:/%s/%s", settings.ui.romfolder.c_str(), tempfile);
			FILE *f_nds_file = fopen(nds_path, "rb");
			if (!f_nds_file)
				continue;
			
			if(cacheBanner(f_nds_file, tempfile, title, romsel_counter1, romsel_counter2sd, isCia) != 0) {
				if (logEnabled)	LogFMA("Main.Banner scanning", "Error reading banner from file", nds_path);
			}
			
			fclose(f_nds_file);
		}
	
		pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
		pp2d_draw_text(12, 16, 0.5f, 0.5f, WHITE, wifiStuckMsg);
		pp2d_end_draw();

		if (checkWifiStatus()) {
			if (settings.ui.autoupdate_twldr && (checkUpdate() == 0) && !isDemo) {
				DownloadTWLoaderCIAs();
			}

			switch (settings.ui.autoupdate) {
				case 0:
				default:
					break;
				case 1:
					UpdateBootstrapRelease();
					break;
				case 2:
					UpdateBootstrapUnofficial();
					break;
			}
		}
	}

	botscreenoff();
	showdialogbox = false;
	pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
	pp2d_draw_text(12, 16, 0.5f, 0.5f, WHITE, " ");
	pp2d_end_draw();
	
	if (settings.ui.theme >= THEME_R4) {
		menu_ctrlset = CTRL_SET_MENU;
		pp2d_set_3D(0);
	} else {
		pp2d_set_3D(1);
	}

	settings.ui.cursorPosition = 0+settings.ui.pagenum*gamesPerPage;
	storedcursorPosition = settings.ui.cursorPosition;
	
	std::string donorpath;

	if (settings.ui.showbootscreen == 1) {
		botscreenon();
		bootSplash();
		if (logEnabled)	LogFM("Main.bootSplash", "Boot splash played.");
		if (settings.ui.theme >= THEME_R4) {
		} else if(aptMainLoop()) {
			fade_whiteToBlack();
		}
	}

	if (settings.ui.theme >= THEME_R4) {
		if (aptMainLoop()) {
			// Top
			pp2d_begin_draw(GFX_TOP, GFX_LEFT);
			pp2d_draw_texture(r4loadingtex, 40, 0);

			// Clear bottom
			pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
			pp2d_draw_text(12, 16, 0.5f, 0.5f, WHITE, " ");
			pp2d_end_draw();
			botscreenon();
		}
	} else {
		botscreenoff();
	}
	
	loadboxartnum = settings.ui.pagenum*gamesPerPage;
	loadbnriconnum = settings.ui.pagenum*gamesPerPage;

	// Loop as long as the status is not exit
	const bool isTWLNANDInstalled = checkTWLNANDSide();
	const bool isTWLNAND1Installed = checkTWLNANDSide1();
	const bool isTWLNAND2Installed = checkTWLNANDSide2();
	// For testing in Citra
	//const bool isTWLNANDInstalled = true;
	//const bool isTWLNAND2Installed = true;
	// Save by default if the TWLNAND-side title is installed.
	// Otherwise, we don't want to save anything.
	bool saveOnExit = isTWLNANDInstalled && isTWLNAND2Installed;
	
	// Check if the TWLNAND-side title (both parts) are installed.
	if ((!isTWLNANDInstalled && !isDemo) || (!isTWLNAND2Installed && !isDemo)) {
		if (!isTWLNAND2Installed) {
			TWLNANDnotfound_msg = 2;
			if (logEnabled)	LogFM("Main.isTWLNAND2Installed", "TWLNAND side (part 2) not installed.");
		} else {
			TWLNANDnotfound_msg = 0;
			if (logEnabled)	LogFM("Main.isTWLNANDInstalled", "TWLNAND side (part 1) not installed.");
		}
		screenmode = SCREEN_MODE_SETTINGS;
		settingsResetSubScreenMode();
	} else if(!settings.ui.firstTimeMsgViewed) {
		screenmode = SCREEN_MODE_SETTINGS;
		settingsResetSubScreenMode();
	}

	//createThread((ThreadFunc)threadLoadBoxArt);

	if (logEnabled && aptMainLoop()) LogFM("Main.aptMainLoop", "TWLoader loaded.");
	while(run && aptMainLoop()) {
		pp2d_begin_draw(GFX_TOP, GFX_LEFT);
		
		// Scan hid shared memory for input events
		hidScanInput();

		const u32 hDown = hidKeysDown();
		const u32 hHeld = hidKeysHeld();
	
		offset3D[0].topbg = CONFIG_3D_SLIDERSTATE * -10.0f;
		offset3D[1].topbg = CONFIG_3D_SLIDERSTATE * 10.0f;
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

		size_t file_count = 0;
		if (settings.twl.romtype == 0) {
			file_count = (settings.twl.forwarder ? fcfiles.size() : files.size());
		} else if (settings.twl.romtype == 1) {
			file_count = (gbfiles.size());
		} else if (settings.twl.romtype == 2) {
			file_count = (nesfiles.size());
		}
		size_t sdfile_count = (files.size());
		size_t fcfile_count = (fcfiles.size());
		size_t gbfile_count = (gbfiles.size());
		size_t nesfile_count = (nesfiles.size());

		if(matching_files.size() != 0) {
			file_count = matching_files.size();
		}
		const int pagemax = std::min((gamesPerPage+settings.ui.pagenum*gamesPerPage), (int)file_count);
		pagemax_ba = (gamesPerPage+1)+settings.ui.pagenum*gamesPerPage;

		if(screenmode == SCREEN_MODE_ROM_SELECT) {
			if (!colortexloaded) {
				if (settings.ui.theme == THEME_AKMENU) {
					switch (settings.ui.subtheme) {
						case 0:
						default:
							pp2d_load_texture_png(topbgtex, "romfs:/graphics/wood/gbatemp/upper_screen.png"); // Top background
							break;
						case 1:
							pp2d_load_texture_png(topbgtex, "romfs:/graphics/wood/black/upper_screen.png"); // Top background
							break;
						case 2:
							pp2d_load_texture_png(topbgtex, "romfs:/graphics/wood/Adv.Evo/upper_screen.png"); // Top background
							break;
						case 3:
							pp2d_load_texture_png(topbgtex, "romfs:/graphics/wood/dstwo pink/upper_screen.png"); // Top background
							break;
					}
				} else if (settings.ui.theme == THEME_R4) {
					switch (settings.ui.subtheme) {
						case 0:
						default:
							pp2d_load_texture_png(toplogotex, "romfs:/graphics/r4/theme01/logo.png"); // Top logo
							pp2d_load_texture_png(topbgtex, "romfs:/graphics/r4/theme01/bckgrd_1.png"); // Top background
							break;
						case 1:
							pp2d_load_texture_png(toplogotex, "romfs:/graphics/r4/theme02/logo.png"); // Top logo
							pp2d_load_texture_png(topbgtex, "romfs:/graphics/r4/theme02/bckgrd_1.png"); // Top background
							break;
						case 2:
							pp2d_load_texture_png(toplogotex, "romfs:/graphics/r4/theme03/logo.png"); // Top logo
							pp2d_load_texture_png(topbgtex, "romfs:/graphics/r4/theme03/bckgrd_1.png"); // Top background
							break;
						case 3:
							pp2d_load_texture_png(toplogotex, "romfs:/graphics/r4/theme04/logo.png"); // Top logo
							pp2d_load_texture_png(topbgtex, "romfs:/graphics/r4/theme04/bckgrd_1.png"); // Top background
							break;
						case 4:
							pp2d_load_texture_png(toplogotex, "romfs:/graphics/r4/theme05/logo.png"); // Top logo
							pp2d_load_texture_png(topbgtex, "romfs:/graphics/r4/theme05/bckgrd_1.png"); // Top background
							break;
						case 5:
							pp2d_load_texture_png(toplogotex, "romfs:/graphics/r4/theme06/logo.png"); // Top logo
							pp2d_load_texture_png(topbgtex, "romfs:/graphics/r4/theme06/bckgrd_1.png"); // Top background
							break;
						case 6:
							pp2d_load_texture_png(toplogotex, "romfs:/graphics/r4/theme07/logo.png"); // Top logo
							pp2d_load_texture_png(topbgtex, "romfs:/graphics/r4/theme07/bckgrd_1.png"); // Top background
							break;
						case 7:
							pp2d_load_texture_png(toplogotex, "romfs:/graphics/r4/theme08/logo.png"); // Top logo
							pp2d_load_texture_png(topbgtex, "romfs:/graphics/r4/theme08/bckgrd_1.png"); // Top background
							break;
						case 8:
							pp2d_load_texture_png(toplogotex, "romfs:/graphics/r4/theme09/logo.png"); // Top logo
							pp2d_load_texture_png(topbgtex, "romfs:/graphics/r4/theme09/bckgrd_1.png"); // Top background
							break;
						case 9:
							pp2d_load_texture_png(toplogotex, "romfs:/graphics/r4/theme10/logo.png"); // Top logo
							pp2d_load_texture_png(topbgtex, "romfs:/graphics/r4/theme10/bckgrd_1.png"); // Top background
							break;
						case 10:
							pp2d_load_texture_png(toplogotex, "romfs:/graphics/r4/theme11/logo.png"); // Top logo
							pp2d_load_texture_png(topbgtex, "romfs:/graphics/r4/theme11/bckgrd_1.png"); // Top background
							break;
						case 11:
							pp2d_load_texture_png(toplogotex, "romfs:/graphics/r4/theme12/logo.png"); // Top logo
							pp2d_load_texture_png(topbgtex, "romfs:/graphics/r4/theme12/bckgrd_1.png"); // Top background
							break;
						case 12:
							pp2d_load_texture_png(toplogotex, "romfs:/graphics/r4/BlueMoon/c_file.png"); // Top logo
							pp2d_load_texture_png(topbgtex, "romfs:/graphics/r4/BlueMoon/c_file.png"); // Top background
							break;
					}
				} else if (settings.ui.theme == THEME_3DSMENU) {
					pp2d_load_texture_png(topbgtex, "romfs:/graphics/3ds/top.png"); // Top background, behind the DSi-Menu border
				} else {
					pp2d_load_texture_png(topbgtex, color_data->topbgloc); // Top background, behind the DSi-Menu border
				}
				settingsUnloadTextures();
				colortexloaded = true;
			}
			if (!bnricontexloaded && TWLNANDnotfound_msg == 2) {
				if (settings.ui.theme == THEME_DSIMENU && fadealpha == 0) {
					dsiMenuTheme_loadingScreen();
				}
				if (settings.twl.romtype == 0) {
					if (!settings.twl.forwarder) {
						char path[256];
						if(matching_files.size() == 0){
							for (loadbnriconnum = 0; loadbnriconnum < gamesPerPage; loadbnriconnum++) {						
								if (loadbnriconnum+settings.ui.pagenum*gamesPerPage < (int)files.size()) {
									const char *tempfile = files.at(loadbnriconnum+settings.ui.pagenum*gamesPerPage).c_str();
									snprintf(path, sizeof(path), "sdmc:/_nds/twloader/bnricons/%s.bin", tempfile);
									if (access(path, F_OK) != -1) {
										StoreBnrIconPath(path);
									} else {
										StoreBnrIconPath(NULL);
									}
									LoadBNRIcon();
									LoadBNRSeq();
								} else {
									StoreBnrIconPath(NULL);
									LoadBNRIcon();
									LoadBNRSeq();
								}
							}
						}else{
							for (loadbnriconnum = 0; loadbnriconnum < gamesPerPage; loadbnriconnum++) {						
								if (loadbnriconnum+settings.ui.pagenum*gamesPerPage < (int)matching_files.size()) {
									const char *tempfile = matching_files.at(loadbnriconnum+settings.ui.pagenum*gamesPerPage).c_str();
									snprintf(path, sizeof(path), "sdmc:/_nds/twloader/bnricons/%s.bin", tempfile);
									if (access(path, F_OK) != -1) {
										StoreBnrIconPath(path);
									} else {
										StoreBnrIconPath(NULL);
									}
									LoadBNRIcon();
									LoadBNRSeq();
								} else {
									StoreBnrIconPath(NULL);
									LoadBNRIcon();
									LoadBNRSeq();
								}
							}
						}
					} else {
						char path[256];
						if(matching_files.size() == 0){
							for (loadbnriconnum = 0; loadbnriconnum < gamesPerPage; loadbnriconnum++) {						
								if (loadbnriconnum+settings.ui.pagenum*gamesPerPage < (int)fcfiles.size()) {
									const char *tempfile = fcfiles.at(loadbnriconnum+settings.ui.pagenum*gamesPerPage).c_str();
									snprintf(path, sizeof(path), "%s/%s.bin", fcbnriconfolder, tempfile);
									if (access(path, F_OK) != -1) {
										StoreBnrIconPath(path);
									} else {
										StoreBnrIconPath(NULL);
									}
									LoadBNRIcon();
									LoadBNRSeq();
								} else {
									StoreBnrIconPath(NULL);
									LoadBNRIcon();
									LoadBNRSeq();
								}
							}
						}else{
							for (loadbnriconnum = 0; loadbnriconnum < gamesPerPage; loadbnriconnum++) {						
								if (loadbnriconnum+settings.ui.pagenum*gamesPerPage < (int)matching_files.size()) {
									const char *tempfile = matching_files.at(loadbnriconnum+settings.ui.pagenum*gamesPerPage).c_str();
									snprintf(path, sizeof(path), "%s/%s.bin", fcbnriconfolder, tempfile);
									if (access(path, F_OK) != -1) {
										StoreBnrIconPath(path);
									} else {
										StoreBnrIconPath(NULL);
									}
									LoadBNRIcon();
									LoadBNRSeq();
								} else {
									StoreBnrIconPath(NULL);
									LoadBNRIcon();
									LoadBNRSeq();
								}
							}
						}
					}
				} else {
					if(matching_files.size() == 0){
						for (loadbnriconnum = 0; loadbnriconnum < gamesPerPage; loadbnriconnum++) {						
							StoreBnrIconPath(NULL);
							LoadBNRIcon();
							LoadBNRSeq();
						}
					}else{
						for (loadbnriconnum = 0; loadbnriconnum < gamesPerPage; loadbnriconnum++) {						
							StoreBnrIconPath(NULL);
							LoadBNRIcon();
							LoadBNRSeq();
						}
					}
				}

				bnricontexloaded = true;
				bnriconnum = 0+settings.ui.pagenum*gamesPerPage;
			}
			menuLoadBoxArt();

			if (settings.ui.theme == THEME_AKMENU) {	// akMenu/Wood theme
//				for (int topfb = GFX_LEFT; topfb <= GFX_RIGHT; topfb++) {
					pp2d_draw_on(GFX_TOP, GFX_LEFT);
					pp2d_draw_texture(topbgtex, 40, 0);
					if (menu_ctrlset == CTRL_SET_MENU) {
						if ((woodmenu_cursorPosition == 1) && !isDemo) {
							switch (settings.ui.subtheme) {
								case 0:
								default:
									pp2d_draw_wtext(40+16, 192, 0.65f, 0.65f, WHITE, TR(STR_YBUTTON_ADD_GAMES));
									break;
								case 1:
									pp2d_draw_wtext(40+8, 64, 0.65f, 0.65f, WHITE, TR(STR_YBUTTON_ADD_GAMES));
									break;
								case 2:
									pp2d_draw_wtext(40+20, 142, 0.65f, 0.65f, WHITE, TR(STR_YBUTTON_ADD_GAMES));
									break;
								case 3:
									pp2d_draw_wtext(40+8, 126, 0.65f, 0.65f, WHITE, TR(STR_YBUTTON_ADD_GAMES));
									break;
							}
						}
						if (!settings.romselect.toplayout && woodmenu_cursorPosition == 2) {
							// Load the boxart for the Slot-1 cartridge if necessary.
							loadSlot1BoxArt();
							// Draw box art
							switch (settings.ui.subtheme) {
								case 0:
								default:
//									pp2d_draw_texture(slot1boxarttex, offset3D[topfb].boxart+40+14, 62);
									pp2d_draw_texture(slot1boxarttex, 0+40+14, 62);
									break;
								case 1:
								case 2:
//									pp2d_draw_texture(slot1boxarttex, offset3D[topfb].boxart+40+176, 113);
									pp2d_draw_texture(slot1boxarttex, 0+40+176, 113);
									break;
								case 3:
//									pp2d_draw_texture(slot1boxarttex, offset3D[topfb].boxart+40+164, 38);
									pp2d_draw_texture(slot1boxarttex, 0+40+164, 38);
									break;
							}
						}
					} else if (menu_ctrlset == CTRL_SET_GAMESEL) {
						if (settings.twl.forwarder && !isDemo) {
							switch (settings.ui.subtheme) {
								case 0:
								default:
									pp2d_draw_wtext(40+16, 192, 0.65f, 0.65f, WHITE, TR(STR_YBUTTON_ADD_GAMES));
									break;
								case 1:
									pp2d_draw_wtext(40+8, 64, 0.65f, 0.65f, WHITE, TR(STR_YBUTTON_ADD_GAMES));
									break;
								case 2:
									pp2d_draw_wtext(40+20, 142, 0.65f, 0.65f, WHITE, TR(STR_YBUTTON_ADD_GAMES));
									break;
								case 3:
									pp2d_draw_wtext(40+8, 126, 0.65f, 0.65f, WHITE, TR(STR_YBUTTON_ADD_GAMES));
									break;
							}
						}
						if (!settings.romselect.toplayout) {
							boxartnum = settings.ui.cursorPosition-settings.ui.pagenum*gamesPerPage;
							if (!bannertextloaded) {
								LoadBoxArt_WoodTheme();
								bannertextloaded = true;
							}
							// Draw box art
							switch (settings.ui.subtheme) {
								case 0:
								default:
//									pp2d_draw_texture(boxarttex[6], offset3D[topfb].boxart+40+14, 62);
									pp2d_draw_texture(boxarttex[6], 0+40+14, 62);
									break;
								case 1:
								case 2:
//									pp2d_draw_texture(boxarttex[6], offset3D[topfb].boxart+40+176, 113);
									pp2d_draw_texture(boxarttex[6], 0+40+176, 113);
									break;
								case 3:
//									pp2d_draw_texture(boxarttex[6], offset3D[topfb].boxart+40+164, 38);
									pp2d_draw_texture(boxarttex[6], 0+40+164, 38);
									break;
							}
						}
					}
					switch (settings.ui.subtheme) {
						case 0:
						default:
							pp2d_draw_text(40+200, 148, 0.82, 0.82, TIME, RetTime(true).c_str());
							DrawDateF(40+179, 198, 0.85, 0.85, TIME, FORMAT_MY);
							break;
						case 1:
							pp2d_draw_text(40+184, 8, 1.30, 1.30, WHITE, RetTime(true).c_str());
							DrawDateF(40+180, 78, 0.85, 0.85, WHITE, FORMAT_M);
							DrawDateF(40+232, 78, 0.85, 0.85, WHITE, FORMAT_Y);
							break;
						case 2:
							pp2d_draw_text(40+16, 78, 1.30, 1.30, WHITE, RetTime(true).c_str());
							DrawDateF(40+68, 202, 0.80, 0.80, WHITE, FORMAT_M);
							DrawDateF(40+108, 202, 0.75, 0.80, WHITE, FORMAT_Y);
							break;
						case 3:
							pp2d_draw_text(40+176, 172, 1.30, 1.30, WHITE, RetTime(true).c_str());
							break;
					}
					pp2d_draw_rectangle(0, 0, 40, 240, BLACK); // Left black bar
					pp2d_draw_rectangle(360, 0, 40, 240, BLACK); // Right black bar
//					pp2d_end_draw();
//				}
			} else if (settings.ui.theme == THEME_R4) {	// R4 theme
				if (updatetopscreen) {
//					pp2d_begin_draw(GFX_TOP);
					pp2d_draw_on(GFX_TOP, GFX_LEFT);
					if (menu_ctrlset != CTRL_SET_MENU) {
						pp2d_draw_texture(topbgtex, 40, 0);
						u32 text_color = BLACK;
						if (settings.ui.subtheme == 12) text_color = WHITE;
						filenameYpos = 15;
						if (settings.twl.romtype == 0) {
							file_count = (settings.twl.forwarder ? fcfiles.size() : files.size());
						} else if (settings.twl.romtype == 1) {
							file_count = (gbfiles.size());
						} else if (settings.twl.romtype == 2) {
							file_count = (nesfiles.size());
						}
						for (filenum = filenameYmovepos; filenum < (int)file_count; filenum++) {
							if (filenum < 15+filenameYmovepos) {
								u32 color;
								if (settings.ui.cursorPosition == filenum) {
									color = SET_ALPHA(color_data->color, 255);
								} else {
									color = text_color;
								}

								// Get the current filename and convert it to wstring.
								const char *filename = "";
								if (settings.twl.romtype == 0) {
									filename = (settings.twl.forwarder
											? fcfiles.at(filenum).c_str()
											: files.at(filenum).c_str());
								} else if (settings.twl.romtype == 1) {
									filename = (gbfiles.at(filenum).c_str());
								} else if (settings.twl.romtype == 2) {
									filename = (nesfiles.at(filenum).c_str());
								}
								wstring wstr = utf8_to_wstring(filename);
								pp2d_draw_wtext(42, filenameYpos, 0.50, 0.50, color, wstr.c_str());

								filenameYpos += 15;
							}
						}

						pp2d_draw_texture_part(topbgtex, 40, 0, 0, 0, 320, 15);
						const char *title = (settings.twl.forwarder
									? "Games (Flashcard)"
									: "Games (SD Card)");
						pp2d_draw_text(42, 0, 0.50, 0.50, text_color, title);
						
						char romsel_counter1[16];
						char romsel_counter2[16];
						snprintf(romsel_counter1, sizeof(romsel_counter1), "%d", settings.ui.cursorPosition+1);		
						snprintf(romsel_counter2, sizeof(romsel_counter2), "%d", file_count);
						
						if (settings.ui.counter) {
							if (file_count < 100) {
								pp2d_draw_text(40+276, 0, 0.50, 0.50, text_color, romsel_counter1);
								pp2d_draw_text(40+295, 0, 0.50, 0.50, text_color, "/");
								pp2d_draw_text(40+300, 0, 0.50, 0.50, text_color, romsel_counter2);
							} else {
								pp2d_draw_text(40+264, 0, 0.50, 0.50, text_color, romsel_counter1);
								pp2d_draw_text(40+289, 0, 0.50, 0.50, text_color, "/");
								pp2d_draw_text(40+294, 0, 0.50, 0.50, text_color, romsel_counter2);
							}
						}
					} else {
						pp2d_draw_texture(toplogotex, 40, 0);
					}
					pp2d_draw_rectangle(0, 0, 40, 240, RGBA8(0, 0, 0, 255)); // Left black bar
					pp2d_draw_rectangle(360, 0, 40, 240, RGBA8(0, 0, 0, 255)); // Right black bar
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

				update_battery_level(batterytex);
				for (int topfb = GFX_LEFT; topfb <= GFX_RIGHT; topfb++) {
//					pp2d_begin_draw(GFX_TOP);
					pp2d_draw_on(GFX_TOP, (gfx3dSide_t)topfb);
					if (settings.ui.theme == THEME_3DSMENU) {
						pp2d_draw_texture(topbgtex, offset3D[topfb].topbg-11, 0);
					} else {
						pp2d_draw_texture_scale(topbgtex, offset3D[topfb].topbg-12, 0, 1.32, 1);
					}
					if (filenum != 0) {	// If ROMs are found, then display box art
						if (!settings.romselect.toplayout) {
							if (loadboxartnum != pagemax_ba) {
								// Nothing
							} else {
								boxartXpos = 136;
								if (!settings.twl.forwarder && settings.ui.pagenum == 0) {
									if (settings.ui.cursorPosition < 2) {
										pp2d_draw_texture(slot1boxarttex, offset3D[topfb].boxart+boxartXpos-144+boxartXmovepos, 240/2 - 115/2); // Draw box art
										if (settings.ui.theme != THEME_3DSMENU) pp2d_draw_texture_flip_blend(slot1boxarttex, offset3D[topfb].boxart+boxartXpos-144+boxartXmovepos, 178, VERTICAL, SET_ALPHA(color_data->color, 255)); // Draw box art's reflection
									}
								}
								for (boxartnum = settings.ui.pagenum*gamesPerPage; boxartnum < pagemax; boxartnum++) {
									if (boxartnum < 9+settings.ui.pagenum*gamesPerPage) {
										ChangeBoxArtNo();
										// Draw box art
										pp2d_draw_texture(boxarttexnum, offset3D[topfb].boxart+boxartXpos+boxartXmovepos, 240/2 - 115/2);
										// Draw box art's reflection
										if (settings.ui.theme != THEME_3DSMENU) pp2d_draw_texture_flip_blend(boxarttexnum, offset3D[topfb].boxart+boxartXpos+boxartXmovepos, 178, VERTICAL, SET_ALPHA(color_data->color, 255));
										boxartXpos += 144;
									}
								}
								if (applaunchprep) {
									if (settings.ui.cursorPosition >= 0) {
										if (settings.ui.theme != THEME_3DSMENU) {
											boxartnum = settings.ui.cursorPosition;
											ChangeBoxArtNo();
											pp2d_draw_texture_part(topbgtex, offset3D[topfb].boxart+136, 63, offset3D[topfb].boxart+104, 63, 128, 115*2);
											// Draw moving box art
											pp2d_draw_texture(boxarttexnum, offset3D[topfb].boxart+136, boxartYmovepos);
											// Draw moving box art's reflection
											pp2d_draw_texture_flip_blend(boxarttexnum, offset3D[topfb].boxart+136, boxartreflYmovepos, VERTICAL, SET_ALPHA(color_data->color, 255));
										}
									} else if (!settings.twl.forwarder && settings.ui.cursorPosition == -1) {
										if (settings.ui.theme != THEME_3DSMENU) {
											pp2d_draw_texture_part(topbgtex, offset3D[topfb].boxart+136, 63, offset3D[topfb].boxart+104, 63, 128, 115*2);
											pp2d_draw_texture(slot1boxarttex, offset3D[topfb].boxart+136, boxartYmovepos); // Draw moving box art
											pp2d_draw_texture_flip_blend(slot1boxarttex, offset3D[topfb].boxart+136, boxartreflYmovepos, VERTICAL, SET_ALPHA(color_data->color, 255)); // Draw moving box art's reflection
										}
									}
								}
							}
						}
					} else {
						if (!settings.romselect.toplayout) {
							boxartXpos = 136;
							if (!settings.twl.forwarder && settings.ui.pagenum == 0) {
								if (settings.ui.cursorPosition < 2) {
									pp2d_draw_texture(slot1boxarttex, offset3D[topfb].boxart+boxartXpos+boxartXmovepos, 240/2 - 115/2); // Draw box art
									pp2d_draw_texture_flip_blend(slot1boxarttex, offset3D[topfb].boxart+boxartXpos+boxartXmovepos, 178, VERTICAL, SET_ALPHA(color_data->color, 255)); // Draw box art's reflection
								}
							}							
						}
					}
					if (settings.ui.theme != THEME_3DSMENU) {
						if (settings.ui.topborder) {
							pp2d_draw_texture_blend(toptex, 400/2 - 400/2, 240/2 - 240/2, menucolor);
							pp2d_draw_text(318, 1.0f, 0.58f, 0.58f, BLACK, RetTime(false).c_str());
							pp2d_draw_text(336, 1.0f, 0.58f, 0.58f, BLACK, RetTimeDot(true).c_str());
							DrawDate(264.0f, 1.0f, 0.58f, 0.58f, BLACK);
						} else {
							pp2d_draw_text(318, 1.0f, 0.58f, 0.58f, WHITE, RetTime(false).c_str());
							pp2d_draw_text(336, 1.0f, 0.58f, 0.58f, WHITE, RetTimeDot(true).c_str());
							DrawDate(264.0f, 1.0f, 0.58f, 0.58f, WHITE);
						}
						if (!settings.romselect.toplayout) {
							if (loadboxartnum != pagemax_ba) {
								if (fadealpha == 0) {
									pp2d_draw_text(112, 104, 0.50, 0.50, WHITE, "Storing box art paths...");
								}
							}
						}
					} else {
						pp2d_draw_texture_part(topbgtex, offset3D[topfb].topbg-11, 0, 0, 0, 140, 240);	// Cover left box art
						pp2d_draw_texture_part(topbgtex, 284+offset3D[topfb].topbg-11, 0, 284, 0, 140, 240);	// Cover right box art
						pp2d_draw_text(318, 1.0f, 0.58f, 0.58f, BLACK, RetTime(false).c_str());
						pp2d_draw_text(336, 1.0f, 0.58f, 0.58f, BLACK, RetTimeDot(true).c_str());
						DrawDate(264.0f, 1.0f, 0.58f, 0.58f, BLACK);
						if (!settings.romselect.toplayout) {
							if (loadboxartnum != pagemax_ba) {
								if (fadealpha == 0) {
									pp2d_draw_text(112, 104, 0.50, 0.50, BLACK, "Storing box art paths...");
								}
							}
						}
					}
					

					if (!settings.ui.topborder && settings.ui.theme == THEME_DSIMENU) {
						draw_volume_slider(setvoltex);
					} else {
						draw_volume_slider(voltex);
					}
					pp2d_draw_texture_part(batteryIcon, 371, 2, 0, batteryFrame*16, 27, 16);
					if (!settings.ui.name.empty()) {
						pp2d_draw_text(34.0f, 1.0f, 0.58, 0.58f, SET_ALPHA(color_data->color, 255), settings.ui.name.c_str());
					}
					if (settings.ui.theme != THEME_3DSMENU) {
						pp2d_draw_texture_part(shouldertex, 0, LshoulderYpos, 0, 0, 72, 20);
						pp2d_draw_texture_part(shouldertex, 328, RshoulderYpos, 0, 20, 73, 20);

						// Draw the "Previous" and "Next" text for L/R.
						u32 lr_color = (settings.ui.pagenum != 0 && file_count <= (size_t)-settings.ui.pagenum*gamesPerPage)
								? BLACK
								: GRAY;
						pp2d_draw_text(17, LshoulderYpos+4, 0.50, 0.50, lr_color, "Previous");

						lr_color = (file_count > (size_t)gamesPerPage+settings.ui.pagenum*gamesPerPage)
								? BLACK
								: GRAY;
						pp2d_draw_text(332, RshoulderYpos+4, 0.50, 0.50, lr_color, "Next");
					} else {
						pp2d_draw_texture(_3dsshouldertex, 0, LshoulderYpos-5);
						pp2d_draw_texture_flip(_3dsshouldertex, 312, RshoulderYpos-5, HORIZONTAL);

						// Draw the "Previous" and "Next" text for L/R.
						u32 lr_color = (settings.ui.pagenum != 0 && file_count <= (size_t)-settings.ui.pagenum*gamesPerPage)
								? BLACK
								: GRAY;
						pp2d_draw_text(4, LshoulderYpos+1, 0.55, 0.55, lr_color, " Previous");

						lr_color = (file_count > (size_t)gamesPerPage+settings.ui.pagenum*gamesPerPage)
								? BLACK
								: GRAY;
						pp2d_draw_text(346, RshoulderYpos+1, 0.55, 0.55, lr_color, "Next ");
					}

					if (fadealpha > 0) pp2d_draw_rectangle(0, 0, 400, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect
//					pp2d_end_draw();
				}
			}
		} else if (screenmode == SCREEN_MODE_SETTINGS) {			
			settingsDrawTopScreen();
		}

		if (screenmode == SCREEN_MODE_ROM_SELECT) {
			if (!colortexloaded_bot) {
				settingsUnloadTextures();
				pp2d_load_texture_png(dotcircletex, color_data->dotcircleloc); // Dots forming a circle
				if (settings.ui.theme != THEME_3DSMENU) pp2d_load_texture_png(startbordertex, color_data->startborderloc); // "START" border
				else pp2d_load_texture_png(startbordertex, "romfs:/graphics/3ds/cursor.png");	// HOME Menu cursor
				if (settings.ui.theme == THEME_AKMENU) {
					switch (settings.ui.subtheme) {
						case 0:
						default:
							pp2d_load_texture_png(bottomtex, "romfs:/graphics/wood/gbatemp/lower_screen.png"); // Bottom of menu
							break;
						case 1:
							pp2d_load_texture_png(bottomtex, "romfs:/graphics/wood/black/lower_screen.png"); // Bottom of menu
							break;
						case 2:
							pp2d_load_texture_png(bottomtex, "romfs:/graphics/wood/Adv.Evo/lower_screen.png"); // Bottom of menu
							break;
						case 3:
							pp2d_load_texture_png(bottomtex, "romfs:/graphics/wood/dstwo pink/lower_screen.png"); // Bottom of menu
							break;
					}
				} else if (settings.ui.theme == THEME_R4) {
					switch (settings.ui.subtheme) {
						case 0:
						default:
							pp2d_load_texture_png(iconstex, "romfs:/graphics/r4/theme01/icons.png"); // Bottom of menu
							pp2d_load_texture_png(bottomtex, "romfs:/graphics/r4/theme01/bckgrd_2.png"); // Bottom of rom select
							break;
						case 1:
							pp2d_load_texture_png(iconstex, "romfs:/graphics/r4/theme02/icons.png"); // Bottom of menu
							pp2d_load_texture_png(bottomtex, "romfs:/graphics/r4/theme02/bckgrd_2.png"); // Bottom of rom select
							break;
						case 2:
							pp2d_load_texture_png(iconstex, "romfs:/graphics/r4/theme03/icons.png"); // Bottom of menu
							pp2d_load_texture_png(bottomtex, "romfs:/graphics/r4/theme03/bckgrd_2.png"); // Bottom of rom select
							break;
						case 3:
							pp2d_load_texture_png(iconstex, "romfs:/graphics/r4/theme04/icons.png"); // Bottom of menu
							pp2d_load_texture_png(bottomtex, "romfs:/graphics/r4/theme04/bckgrd_2.png"); // Bottom of rom select
							break;
						case 4:
							pp2d_load_texture_png(iconstex, "romfs:/graphics/r4/theme05/icons.png"); // Bottom of menu
							pp2d_load_texture_png(bottomtex, "romfs:/graphics/r4/theme05/bckgrd_2.png"); // Bottom of rom select
							break;
						case 5:
							pp2d_load_texture_png(iconstex, "romfs:/graphics/r4/theme06/icons.png"); // Bottom of menu
							pp2d_load_texture_png(bottomtex, "romfs:/graphics/r4/theme06/bckgrd_2.png"); // Bottom of rom select
							break;
						case 6:
							pp2d_load_texture_png(iconstex, "romfs:/graphics/r4/theme07/icons.png"); // Bottom of menu
							pp2d_load_texture_png(bottomtex, "romfs:/graphics/r4/theme07/bckgrd_2.png"); // Bottom of rom select
							break;
						case 7:
							pp2d_load_texture_png(iconstex, "romfs:/graphics/r4/theme08/icons.png"); // Bottom of menu
							pp2d_load_texture_png(bottomtex, "romfs:/graphics/r4/theme08/bckgrd_2.png"); // Bottom of rom select
							break;
						case 8:
							pp2d_load_texture_png(iconstex, "romfs:/graphics/r4/theme09/icons.png"); // Bottom of menu
							pp2d_load_texture_png(bottomtex, "romfs:/graphics/r4/theme09/bckgrd_2.png"); // Bottom of rom select
							break;
						case 9:
							pp2d_load_texture_png(iconstex, "romfs:/graphics/r4/theme10/icons.png"); // Bottom of menu
							pp2d_load_texture_png(bottomtex, "romfs:/graphics/r4/theme10/bckgrd_2.png"); // Bottom of rom select
							break;
						case 10:
							pp2d_load_texture_png(iconstex, "romfs:/graphics/r4/theme11/icons.png"); // Bottom of menu
							pp2d_load_texture_png(bottomtex, "romfs:/graphics/r4/theme11/bckgrd_2.png"); // Bottom of rom select
							break;
						case 11:
							pp2d_load_texture_png(iconstex, "romfs:/graphics/r4/theme12/icons.png"); // Bottom of menu
							pp2d_load_texture_png(bottomtex, "romfs:/graphics/r4/theme12/bckgrd_2.png"); // Bottom of rom select
							break;
						case 12:
							pp2d_load_texture_png(iconstex, "romfs:/graphics/r4/BlueMoon/icons.png"); // Bottom of menu
							pp2d_load_texture_png(bottomtex, "romfs:/graphics/r4/BlueMoon/desktop.png"); // Bottom of rom select
							break;
					}
				} else if (settings.ui.theme == THEME_3DSMENU) {
					pp2d_load_texture_png(bottomtex, bottomloc); // Bottom of menu
					pp2d_load_texture_png(bubbletex, "romfs:/graphics/3ds/bubble.png"); // Text bubble
					pp2d_load_texture_png(boxtex, "romfs:/graphics/3ds/box.png"); // (DSiWare) box on bottom screen
				} else {
					pp2d_load_texture_png(bottomtex, bottomloc); // Bottom of menu
					pp2d_load_texture_png(bubbletex, "romfs:/graphics/bubble.png"); // Text bubble
					pp2d_load_texture_png(boxtex, "romfs:/graphics/box.png"); // (DSiWare) box on bottom screen
				}
				colortexloaded_bot = true;
			}

			pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
			if (settings.ui.theme == THEME_AKMENU) {
				if (settings.ui.woodIconScaleEffect) {
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
				} else {
					wood_ndsiconscaletimer = 60;
					wood_ndsiconscalemovepos = 0;
					wood_ndsiconscalesize = 0.00f;
					wood_ndsiconscaledown = false;
				}

				pp2d_draw_texture(bottomtex, 320/2 - 320/2, 240/2 - 240/2);
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
						if (settings.ui.cursorPosition == -1) {
							// Reload the banner text.
							bannertextloaded = false;
						}
					}
					size_t cardicontex = gamecardGetIcon();
					if (!gamecardIsInserted()) {
						cardicontex = iconnulltex;
					} else if (gamecardGetType() == CARD_TYPE_UNKNOWN || gamecardGetType() == CARD_TYPE_CTR) {
						cardicontex = iconnulltex;
					}

					int Ypos = 26;
					filenameYpos = 36;
					if (woodmenu_cursorPosition == 0) {
						pp2d_draw_rectangle(0, Ypos-4, 320, 40, SET_ALPHA(color_data->color, 127));
						if (sdfile_count != 0)
							pp2d_draw_texture_part_scale(sdicontex, 8-wood_ndsiconscalemovepos, -wood_ndsiconscalemovepos+Ypos, 0, 0, 32, 32, 1.00+wood_ndsiconscalesize, 1.00+wood_ndsiconscalesize);
						else
							pp2d_draw_texture_part_scale_blend(sdicontex, 8-wood_ndsiconscalemovepos, -wood_ndsiconscalemovepos+Ypos, 0, 0, 32, 32, 1.00+wood_ndsiconscalesize, 1.00+wood_ndsiconscalesize, RGBA8(255, 255, 255, 127));
					} else {
						if (sdfile_count != 0)
							pp2d_draw_texture_part(sdicontex, 8, Ypos, 0, 0, 32, 32);
						else
							pp2d_draw_texture_part_blend(sdicontex, 8, Ypos, 0, 0, 32, 32, (u32) RGBA8(255, 255, 255, 127));
					}
					pp2d_draw_text(46, filenameYpos, 0.45f, 0.45f, WHITE, "Games (SD Card)");
					Ypos += 39;
					filenameYpos += 39;
					if (woodmenu_cursorPosition == 1) {
						pp2d_draw_rectangle(0, Ypos-4, 320, 40, SET_ALPHA(color_data->color, 127));
						if (fcfile_count != 0 && settings.twl.romtype == 0)							
							pp2d_draw_texture_part_scale(flashcardicontex, 8-wood_ndsiconscalemovepos, -wood_ndsiconscalemovepos+Ypos, 0, 0, 32, 32, 1.00+wood_ndsiconscalesize, 1.00+wood_ndsiconscalesize);
						else
							pp2d_draw_texture_part_scale_blend(flashcardicontex, 8-wood_ndsiconscalemovepos, -wood_ndsiconscalemovepos+Ypos, 0, 0, 32, 32, 1.00+wood_ndsiconscalesize, 1.00+wood_ndsiconscalesize, RGBA8(255, 255, 255, 127));
					} else {
						if (fcfile_count != 0 && settings.twl.romtype == 0)
							pp2d_draw_texture_part(flashcardicontex, 8, Ypos, 0, 0, 32, 32);
						else
							pp2d_draw_texture_part_blend(flashcardicontex, 8, Ypos, 0, 0, 32, 32, (u32) RGBA8(255, 255, 255, 127));
					}
					pp2d_draw_text(46, filenameYpos, 0.45f, 0.45f, WHITE, "Games (Flashcard)");
					Ypos += 39;
					filenameYpos += 39;
					if (woodmenu_cursorPosition == 2) {
						pp2d_draw_rectangle(0, Ypos-4, 320, 40, SET_ALPHA(color_data->color, 127));
						pp2d_draw_texture_part_scale(cardicontex, 8-wood_ndsiconscalemovepos, -wood_ndsiconscalemovepos+Ypos, 0, bnriconframenumY[21]*32, 32, 32, 1.00+wood_ndsiconscalesize, 1.00+wood_ndsiconscalesize);
					} else
						pp2d_draw_texture_part(cardicontex, 8, Ypos, 0, bnriconframenumY[21]*32, 32, 32);
					pp2d_draw_text(46, filenameYpos, 0.45f, 0.45f, WHITE, "Launch Slot-1 card");
					Ypos += 39;
					filenameYpos += 39;
					if (woodmenu_cursorPosition == 3) {
						pp2d_draw_rectangle(0, Ypos-4, 320, 40, SET_ALPHA(color_data->color, 127));
						pp2d_draw_texture_part_scale(gbaicontex, 8-wood_ndsiconscalemovepos, -wood_ndsiconscalemovepos+Ypos, 0, 0, 32, 32, 1.00+wood_ndsiconscalesize, 1.00+wood_ndsiconscalesize);
					} else
						pp2d_draw_texture_part(gbaicontex, 8, Ypos, 0, 0, 32, 32);
					pp2d_draw_wtext(46, filenameYpos, 0.45f, 0.45f, WHITE, TR(STR_START_SELECT_ROMTYPE));
					Ypos += 39;
					filenameYpos += 39;
					if (woodmenu_cursorPosition == 4) {
						pp2d_draw_rectangle(0, Ypos-4, 320, 40, SET_ALPHA(color_data->color, 127));
						pp2d_draw_texture_part_scale(smallsettingsicontex, 8-wood_ndsiconscalemovepos, -wood_ndsiconscalemovepos+Ypos, 0, 0, 32, 32, 1.00+wood_ndsiconscalesize, 1.00+wood_ndsiconscalesize);
					} else
						pp2d_draw_texture_part(smallsettingsicontex, 8, Ypos, 0, 0, 32, 32);
					pp2d_draw_wtext(46, filenameYpos, 0.45f, 0.45f, WHITE, TR(STR_SETTINGS_TEXT));						
					pp2d_draw_text(2, 2, 0.50, 0.50, WHITE, "Menu");
				} else if (menu_ctrlset == CTRL_SET_ROMTYPE) {
					int Ypos = 26;
					filenameYpos = 36;
					if (setromtype_cursorPosition == 0) {
						pp2d_draw_rectangle(0, Ypos-4, 320, 40, SET_ALPHA(color_data->color, 127));
						pp2d_draw_texture_part_scale(flashcardicontex, 8-wood_ndsiconscalemovepos, -wood_ndsiconscalemovepos+Ypos, 0, 0, 32, 32, 1.00+wood_ndsiconscalesize, 1.00+wood_ndsiconscalesize);
					} else {
						pp2d_draw_texture_part(flashcardicontex, 8, Ypos, 0, 0, 32, 32);
					}
					pp2d_draw_text(46, filenameYpos, 0.45f, 0.45f, WHITE, "Nintendo DS/DSi");
					Ypos += 39;
					filenameYpos += 39;
					if (setromtype_cursorPosition == 1) {
						pp2d_draw_rectangle(0, Ypos-4, 320, 40, SET_ALPHA(color_data->color, 127));
						pp2d_draw_texture_part_scale(gbaicontex, 8-wood_ndsiconscalemovepos, -wood_ndsiconscalemovepos+Ypos, 0, 0, 32, 32, 1.00+wood_ndsiconscalesize, 1.00+wood_ndsiconscalesize);
					} else {
						pp2d_draw_texture_part(gbaicontex, 8, Ypos, 0, 0, 32, 32);
					}
					pp2d_draw_text(46, filenameYpos-8, 0.45f, 0.45f, WHITE, "GameBoy Advance");
					pp2d_draw_wtext(46, filenameYpos+8, 0.45f, 0.45f, WHITE, TR(STR_START_START_GBARUNNER2));
					Ypos += 39;
					filenameYpos += 39;
					if (setromtype_cursorPosition == 2) {
						pp2d_draw_rectangle(0, Ypos-4, 320, 40, SET_ALPHA(color_data->color, 127));
						pp2d_draw_texture_part_scale(gbctex, 8-wood_ndsiconscalemovepos, -wood_ndsiconscalemovepos+Ypos, 0, 0, 32, 32, 1.00+wood_ndsiconscalesize, 1.00+wood_ndsiconscalesize);
					} else
						pp2d_draw_texture_part(gbctex, 8, Ypos, 0, 0, 32, 32);
					pp2d_draw_text(46, filenameYpos, 0.45f, 0.45f, WHITE, "GameBoy/Super GB/GB Color");
					Ypos += 39;
					filenameYpos += 39;
					if (setromtype_cursorPosition == 3) {
						pp2d_draw_rectangle(0, Ypos-4, 320, 40, SET_ALPHA(color_data->color, 127));
						pp2d_draw_texture_part_scale(nestex, 8-wood_ndsiconscalemovepos, -wood_ndsiconscalemovepos+Ypos, 0, 0, 32, 32, 1.00+wood_ndsiconscalesize, 1.00+wood_ndsiconscalesize);
					} else
						pp2d_draw_texture_part(nestex, 8, Ypos, 0, 0, 32, 32);
					pp2d_draw_text(46, filenameYpos, 0.45f, 0.45f, WHITE, "Nintendo Entertainment System");
					pp2d_draw_text(2, 2, 0.50, 0.50, WHITE, "Select ROM type");
				} else {
					int Ypos = 26;
					for (filenum = (settings.ui.pagenum*gamesPerPage)+filenameYmovepos; filenum < pagemax; filenum++) {
						if (filenum < 5+(settings.ui.pagenum*gamesPerPage)+filenameYmovepos) {
							bnriconnum = filenum;
							ChangeBNRIconNo();
							if (settings.ui.cursorPosition == filenum) {
								pp2d_draw_rectangle(0, Ypos-4, 320, 40, SET_ALPHA(color_data->color, 127));
							}

							// Get the current filename and convert it to wstring.
							const char *filename = "";
							if (settings.twl.romtype == 0) {
								filename = (settings.twl.forwarder
										? fcfiles.at(filenum).c_str()
										: files.at(filenum).c_str());
							} else if (settings.twl.romtype == 1) {
								filename = (gbfiles.at(filenum).c_str());
							} else if (settings.twl.romtype == 2) {
								filename = (nesfiles.at(filenum).c_str());
							}
							wstring wstr = utf8_to_wstring(filename);
							pp2d_draw_wtext(46, Ypos+10, 0.45f, 0.45f, WHITE, wstr.c_str());

							if (settings.ui.cursorPosition == filenum)
								pp2d_draw_texture_part_scale_flip(bnricontexnum, 8-wood_ndsiconscalemovepos, -wood_ndsiconscalemovepos+Ypos, 0, bnriconframenumY[bnriconnum-settings.ui.pagenum*gamesPerPage]*32, 32, 32, 1.00+wood_ndsiconscalesize, 1.00+wood_ndsiconscalesize, bannerFlip[bnriconnum-settings.ui.pagenum*gamesPerPage]);
							else
								pp2d_draw_texture_part_flip(bnricontexnum, 8, Ypos, 0, bnriconframenumY[bnriconnum-settings.ui.pagenum*gamesPerPage]*32, 32, 32, bannerFlip[bnriconnum-settings.ui.pagenum*gamesPerPage]);
							Ypos += 39;
						}
					}
					pp2d_draw_texture_part(bottomtex, 0, 0, 0, 0, 320, 22);
					pp2d_draw_texture_part(bottomtex, 0, 217, 0, 217, 320, 23);
					if (settings.twl.forwarder)
						pp2d_draw_text(2, 2, 0.50, 0.50, WHITE, "Games (Flashcard)");
					else
						pp2d_draw_text(2, 2, 0.50, 0.50, WHITE, "Games (SD Card)");
					
					size_t file_count = 0;
					if (settings.twl.romtype == 0) {
						file_count = (settings.twl.forwarder ? fcfiles.size() : files.size());
					} else if (settings.twl.romtype == 1) {
						file_count = (gbfiles.size());
					} else if (settings.twl.romtype == 2) {
						file_count = (nesfiles.size());
					}

					char romsel_counter1[16];
					char romsel_counter2[16];
					snprintf(romsel_counter1, sizeof(romsel_counter1), "%d", settings.ui.cursorPosition+1);		
					snprintf(romsel_counter2, sizeof(romsel_counter2), "%zu", file_count);					

					if (settings.ui.counter) {
						if (file_count < 100) {
							pp2d_draw_text(276, 2, 0.50, 0.50, WHITE, romsel_counter1);
							pp2d_draw_text(295, 2, 0.50, 0.50, WHITE, "/");
							pp2d_draw_text(300, 2, 0.50, 0.50, WHITE, romsel_counter2);
						} else {
							pp2d_draw_text(264, 2, 0.50, 0.50, WHITE, romsel_counter1);
							pp2d_draw_text(289, 2, 0.50, 0.50, WHITE, "/");
							pp2d_draw_text(294, 2, 0.50, 0.50, WHITE, romsel_counter2);
						}
					}
				}
				// Playback animated icons
				for (int i = 0; i < gamesPerPage; i++) {
					if(bnriconisDSi[i]==true) {
						playBannerSequence(i);
					}
				}
				// Playback animated icon on Slot-1
				if(bnriconisDSi[gamesPerPage+1]==true) {
					playBannerSequence(gamesPerPage+1);
				} else {
					bnriconPalLine[gamesPerPage+1] = 0;
					bnriconframenumY[gamesPerPage+1] = 0;
					bannerFlip[gamesPerPage+1] = NONE;
				}
			} else if (settings.ui.theme == THEME_R4) {
				if (menu_ctrlset == CTRL_SET_MENU) {
					pp2d_draw_texture(iconstex, 0, 0);
					if (r4menu_cursorPosition == 0) {
						pp2d_draw_rectangle(12, 77, 92, 91, SET_ALPHA(color_data->color, 255));
						pp2d_draw_texture_part(iconstex, 14, 79, 14, 79, 88, 87);
						static const char selectiontext[] = "Games";

						pp2d_draw_text(128, 220, 0.60f, 0.60f, WHITE, selectiontext);
					} else 	if (r4menu_cursorPosition == 1) {
						pp2d_draw_rectangle(115, 77, 92, 91, SET_ALPHA(color_data->color, 255));
						pp2d_draw_texture_part(iconstex, 117, 79, 117, 79, 88, 87);
						static const char selectiontext[] = "Launch Slot-1 card";
						
						pp2d_draw_text(96, 220, 0.60f, 0.60f, WHITE, selectiontext);
					} else 	if (r4menu_cursorPosition == 2) {
						pp2d_draw_rectangle(217, 77, 92, 91, SET_ALPHA(color_data->color, 255));
						pp2d_draw_texture_part(iconstex, 219, 79, 219, 79, 88, 87);
						static const char selectiontext[] = "Start GBARunner2";
						
						pp2d_draw_text(96, 220, 0.60f, 0.60f, WHITE, selectiontext);
					}
					DrawDate(2, 220, 0.60f, 0.60f, false);
					pp2d_draw_text(274, 220, 0.60f, 0.60f, WHITE, RetTime(true).c_str());
				} else {
					pp2d_draw_texture(bottomtex, 0, 0);
					if (settings.twl.forwarder && !isDemo) {
						pp2d_draw_wtext(16, 192, 0.65f, 0.65f, WHITE, TR(STR_YBUTTON_ADD_GAMES));
					}
					if (!bannertextloaded && settings.twl.romtype == 0) {
						char path[256];
						bnriconnum = settings.ui.cursorPosition;
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
					pp2d_draw_rectangle(80, 31, 192, 42, RGBA8(255, 255, 255, 255));
					pp2d_draw_texture(dboxtex_iconbox, 47, 31);
					if (settings.twl.romtype == 2) {
						pp2d_draw_texture_part(nestex, 52, 36, 0, 0, 32, 32);
					} else if (settings.twl.romtype == 1) {
						pp2d_draw_texture_part(gbctex, 52, 36, 0, 0, 32, 32);
					} else {
						pp2d_draw_texture_part_flip(bnricontex[6+bnriconPalLine[gamesPerPage]*8], 52, 36, 0, bnriconframenumY[gamesPerPage]*32, 32, 32, bannerFlip[gamesPerPage]);
					}
					
					if (!bannertextloaded) {
						if (settings.twl.romtype == 0) {
							char path[256];
							if (settings.twl.forwarder) {
								if (fcfiles.size() != 0) {
									romsel_filename = fcfiles.at(settings.ui.cursorPosition).c_str();
									romsel_filename_w = utf8_to_wstring(romsel_filename);
								} else {
									romsel_filename = " ";
									romsel_filename_w = utf8_to_wstring(romsel_filename);
								}
							
								snprintf(path, sizeof(path), "%s/%s.bin", fcbnriconfolder, romsel_filename);
							} else {
								if (files.size() != 0) {
									romsel_filename = files.at(settings.ui.cursorPosition).c_str();
									romsel_filename_w = utf8_to_wstring(romsel_filename);
								} else {
									romsel_filename = " ";
									romsel_filename_w = utf8_to_wstring(romsel_filename);
								}
								snprintf(path, sizeof(path), "%s/%s.bin", bnriconfolder, romsel_filename);						
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
						} else if (settings.twl.romtype == 1) {
							romsel_filename = gbfiles.at(settings.ui.cursorPosition).c_str();

							romsel_gameline.clear();
							romsel_gameline.push_back(latin1_to_wstring(""));
							std::string std_romsel_filename = romsel_filename;
							if(std_romsel_filename.substr(std_romsel_filename.find_last_of(".") + 1) == "gb") {
								romsel_gameline.push_back(latin1_to_wstring("GameBoy ROM"));
							} else if(std_romsel_filename.substr(std_romsel_filename.find_last_of(".") + 1) == "gbc") {
								romsel_gameline.push_back(latin1_to_wstring("GameBoy Color ROM"));
							} else if(std_romsel_filename.substr(std_romsel_filename.find_last_of(".") + 1) == "sgb") {
								romsel_gameline.push_back(latin1_to_wstring("Super GameBoy ROM"));
							}
						} else if (settings.twl.romtype == 2) {
							romsel_filename = nesfiles.at(settings.ui.cursorPosition).c_str();

							romsel_gameline.clear();
							romsel_gameline.push_back(latin1_to_wstring(""));
							std::string std_romsel_filename = romsel_filename;
							if(std_romsel_filename.substr(std_romsel_filename.find_last_of(".") + 1) == "nes") {
								romsel_gameline.push_back(latin1_to_wstring("Nintendo Entertainment System ROM"));
							} else if(std_romsel_filename.substr(std_romsel_filename.find_last_of(".") + 1) == "fds") {
								romsel_gameline.push_back(latin1_to_wstring("Famicom Disk System ROM"));
							}
						}
						bannertextloaded = true;
					}
					if (settings.ui.cursorPosition >= 0) {
						int y = 32, dy = 12;
						// Print the banner text, center-aligned.
						const size_t banner_lines = std::min(3U, romsel_gameline.size());
						for (size_t i = 0; i < banner_lines; i++, y += dy) {
							const int text_width = pp2d_get_wtext_width(romsel_gameline[i].c_str(), 0.55, 0.55);
							pp2d_draw_wtext(84+(192-text_width)/2, y, 0.55, 0.55, BLACK, romsel_gameline[i].c_str());
						}
					}
				}
				// Playback animated icon
				if(bnriconisDSi[gamesPerPage]==true) {
					playBannerSequence(gamesPerPage);
				} else {
					bnriconPalLine[gamesPerPage] = 0;
					bnriconframenumY[gamesPerPage] = 0;
					bannerFlip[gamesPerPage] = NONE;
				}
			} else {
				if (settings.ui.custombot == 1) {
					pp2d_draw_texture(bottomtex, 320/2 - 320/2, 240/2 - 240/2);
				} else {
					if (settings.ui.theme == THEME_3DSMENU) {
						pp2d_draw_rectangle(0, 0, 320, 240, RGBA8(233, 219, 215, 255));
						pp2d_draw_texture(_3dsbottopbartex, 0, 0);
					} else {
						pp2d_draw_texture_blend(bottomtex, 320/2 - 320/2, 240/2 - 240/2, menucolor);
					}
				}

				if (settings.ui.theme != THEME_3DSMENU) pp2d_draw_texture(scrollbartex, 0, 240-28);
				
				if(!isDemo) {
					if (settings.ui.theme != THEME_3DSMENU) {
						if (!settings.ui.iconsize) {
							const wchar_t *home_text = TR(STR_RETURN_TO_HOME_MENU);
							const int home_width = pp2d_get_wtext_width(home_text, 0.50, 0.50) + 16;
							const int home_x = (320-home_width)/2;
							pp2d_draw_texture(homeicontex, home_x, 194); // Draw HOME icon
							pp2d_draw_wtext(home_x+20, 195, 0.50, 0.50, BLACK, home_text);
						}
					} else {
						pp2d_draw_texture(_3dsbotbotbartex, 0, 240-31);
						const wchar_t *home_text = TR(STR_RETURN_TO_HOME_MENU);
						const int home_width = pp2d_get_wtext_width(home_text, 0.50, 0.50) + 16;
						const int home_x = (320-home_width)/2;
						pp2d_draw_texture(homeicontex, home_x, 220); // Draw HOME icon
						pp2d_draw_wtext(home_x+20, 221, 0.50, 0.50, BLACK, home_text);
					}
				}

				if (settings.ui.pagenum == 0) {
					if (settings.ui.iconsize) {
						if (settings.ui.theme != THEME_3DSMENU) {
							pp2d_draw_texture_scale(bracetex, -74+titleboxXmovepos*1.25, 104, 1.25, 1.25);
							pp2d_draw_texture_scale(settingsicontex, -40+setsboxXpos+titleboxXmovepos*1.25, 107, 1.25, 1.25);
						} else {
							pp2d_draw_texture_scale(bracetex, -74+titleboxXmovepos*1.25, 112, 1.25, 1.25);
							pp2d_draw_texture_scale(settingsicontex, -40+setsboxXpos+titleboxXmovepos*1.25, 115, 1.25, 1.25);
						}
					} else {
						pp2d_draw_texture(bracetex, -32+titleboxXmovepos, 112);
						pp2d_draw_texture(settingsicontex, setsboxXpos+titleboxXmovepos, 115);
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
							if (settings.ui.cursorPosition == -1) {
								// Reload the banner text.
								bannertextloaded = false;
							}
						}

						if (settings.ui.iconsize) {
							if (settings.ui.theme != THEME_3DSMENU) {
								pp2d_draw_texture_scale(carttex(), -24+cartXpos+titleboxXmovepos*1.25, 107, 1.25, 1.25);
							} else {
								pp2d_draw_texture_scale(carttex(), -24+cartXpos+titleboxXmovepos*1.25, 115, 1.25, 1.25);
							}
						} else {
							pp2d_draw_texture(carttex(), cartXpos+titleboxXmovepos, 116);
						}
						size_t cardicontex = gamecardGetIcon();
						if (!gamecardIsInserted()) {
							cardicontex = iconnulltex;
						} else if (gamecardGetType() == CARD_TYPE_UNKNOWN || gamecardGetType() == CARD_TYPE_CTR) {
							cardicontex = iconnulltex;
						}
						if (settings.ui.iconsize) {
							if (settings.ui.theme != THEME_3DSMENU) {
								pp2d_draw_texture_part_scale_flip(cardicontex, -4+cartXpos+titleboxXmovepos*1.25, 123, 0, bnriconframenumY[gamesPerPage+1]*32, 32, 32, 1.25, 1.25, bannerFlip[gamesPerPage+1]);
							} else {
								pp2d_draw_texture_part_scale_flip(cardicontex, -4+cartXpos+titleboxXmovepos*1.25, 131, 0, bnriconframenumY[gamesPerPage+1]*32, 32, 32, 1.25, 1.25, bannerFlip[gamesPerPage+1]);
							}
						} else {
							pp2d_draw_texture_part_flip(cardicontex, 16+cartXpos+titleboxXmovepos, 129, 0, bnriconframenumY[gamesPerPage+1]*32, 32, 32, bannerFlip[gamesPerPage+1]);
						}
					} else {
						// Get flash cart games.
						if (settings.ui.iconsize) {
							if (settings.ui.theme != THEME_3DSMENU) {
								pp2d_draw_texture_scale(getfcgameboxtex, -24+cartXpos+titleboxXmovepos*1.25, 107, 1.25, 1.25);
							} else {
								pp2d_draw_texture_scale(getfcgameboxtex, -24+cartXpos+titleboxXmovepos*1.25, 115, 1.25, 1.25);
							}
						} else {
							pp2d_draw_texture(getfcgameboxtex, cartXpos+titleboxXmovepos, 115);
						}
					}
				} else {
					if (settings.ui.iconsize) {
						if (settings.ui.theme != THEME_3DSMENU) {
							pp2d_draw_texture_scale(bracetex, 15+cartXpos+titleboxXmovepos*1.25, 104, 1.25, 1.25);
						} else {
							pp2d_draw_texture_scale(bracetex, 15+cartXpos+titleboxXmovepos*1.25, 112, 1.25, 1.25);
						}
					} else {
						pp2d_draw_texture(bracetex, 32+cartXpos+titleboxXmovepos, 112);
					}
				}

				if (settings.ui.iconsize) {
					titleboxXpos = 120;
					ndsiconXpos = 140;
				} else {
					titleboxXpos = 128;
					ndsiconXpos = 144;
				}

				if (settings.ui.cursorPosition >= 0 && settings.ui.theme != THEME_3DSMENU) {
					pp2d_draw_texture(scrollwindowtex, 23+scrollwindowXmovepos, 240-28);
				}

				float bipxPos = 33.0;
				for (filenum = settings.ui.pagenum*gamesPerPage; filenum < gamesPerPage+settings.ui.pagenum*gamesPerPage; filenum++) {
					if (filenum < pagemax) {
						if (settings.ui.theme != THEME_3DSMENU) pp2d_draw_texture_part(bipstex, bipxPos, 222, 0, 0, 11, 11);
						bipxPos += 6.26;
						if (settings.ui.iconsize) {
							if (settings.ui.theme != THEME_3DSMENU) {
								pp2d_draw_texture_part_scale(boxtex, titleboxXpos+titleboxXmovepos*1.25, 108, 0, 0, 64, 64, 1.25, 1.25);
							} else {
								pp2d_draw_texture_part(boxtex, titleboxXpos+titleboxXmovepos*1.25, 116, 0, 0, 80, 80);
							}
							titleboxXpos += 80;

							bnriconnum = filenum;
							ChangeBNRIconNo();
							if (settings.ui.theme != THEME_3DSMENU) {
								pp2d_draw_texture_part_scale_flip(bnricontexnum, ndsiconXpos+titleboxXmovepos*1.25, 123, 0, bnriconframenumY[bnriconnum-settings.ui.pagenum*gamesPerPage]*32, 32, 32, 1.25, 1.25, bannerFlip[bnriconnum-settings.ui.pagenum*gamesPerPage]);
							} else {
								pp2d_draw_texture_part_scale_flip(bnricontexnum, -4+ndsiconXpos+titleboxXmovepos*1.25, 131, 0, bnriconframenumY[bnriconnum-settings.ui.pagenum*gamesPerPage]*32, 32, 32, 1.50, 1.50, bannerFlip[bnriconnum-settings.ui.pagenum*gamesPerPage]);
							}
							ndsiconXpos += 80;
						} else {
							if (settings.ui.theme != THEME_3DSMENU) {
								pp2d_draw_texture_part(boxtex, titleboxXpos+titleboxXmovepos, 116, 0, 0, 64, 64);
							} else {
								pp2d_draw_texture_part_scale(boxtex, 5+titleboxXpos+titleboxXmovepos, 119, 0, 0, 80, 80, 0.68, 0.67);
							}
							titleboxXpos += 64;

							bnriconnum = filenum;
							ChangeBNRIconNo();
							pp2d_draw_texture_part_flip(bnricontexnum, ndsiconXpos+titleboxXmovepos, 129, 0, bnriconframenumY[bnriconnum-settings.ui.pagenum*gamesPerPage]*32, 32, 32, bannerFlip[bnriconnum-settings.ui.pagenum*gamesPerPage]);
							ndsiconXpos += 64;
						}
					} else {
						if (settings.ui.theme != THEME_3DSMENU) pp2d_draw_texture_part(bipstex, bipxPos, 222, 0, 11, 11, 11);
						bipxPos += 6.26;
						if (settings.ui.iconsize) {
							if (settings.ui.theme != THEME_3DSMENU) {
								pp2d_draw_texture_part_scale(boxtex, titleboxXpos+titleboxXmovepos*1.25, 108, 0, 64, 64, 64, 1.25, 1.25);
							} else {
								pp2d_draw_texture_part_scale(boxtex, titleboxXpos+titleboxXmovepos*1.25, 116, 0, 80, 80, 80, 1.25, 1.25);
							}
							titleboxXpos += 80;
							ndsiconXpos += 80;
						} else {
							if (settings.ui.theme != THEME_3DSMENU) {
								pp2d_draw_texture_part(boxtex, titleboxXpos+titleboxXmovepos, 116, 0, 64, 64, 64);
							} else {
								pp2d_draw_texture_part_scale(boxtex, titleboxXpos+titleboxXmovepos, 116, 0, 80, 80, 80, 0.68, 0.67);
							}
							titleboxXpos += 64;
							ndsiconXpos += 64;
						}
					}
				}
				if (settings.ui.cursorPosition >= 0 && settings.ui.theme != THEME_3DSMENU) {
					pp2d_draw_texture_blend(scrollwindowfronttex, 23+scrollwindowXmovepos, 240-28, SET_ALPHA(color_data->color, 255));
				}

				if (settings.ui.theme != THEME_3DSMENU) {
					pp2d_draw_texture_blend(buttonarrowtex, 0, 240-28, SET_ALPHA(color_data->color, 255));
					pp2d_draw_texture_rotate_flip_blend(buttonarrowtex, 320-25, 240-28, 180.0f, VERTICAL, SET_ALPHA(color_data->color, 255));
				}

				if (settings.ui.iconsize) {
					if (settings.ui.theme != THEME_3DSMENU) {
						pp2d_draw_texture_scale_flip(bracetex, ndsiconXpos+titleboxXmovepos*1.25, 104, 1.25, 1.25, HORIZONTAL);
					} else {
						pp2d_draw_texture_scale_flip(bracetex, ndsiconXpos+titleboxXmovepos*1.25, 112, 1.25, 1.25, HORIZONTAL);
					}
				} else {
					pp2d_draw_texture_flip(bracetex, ndsiconXpos+titleboxXmovepos, 112, HORIZONTAL);
				}
				if (!applaunchprep) {
					if (titleboxXmovetimer == 0) {
						startbordermovepos = 0;
						startborderscalesize = 1.0;
					}
					if (!settings.twl.forwarder && settings.ui.cursorPosition == -1 && !gamecardIsInserted()) {
						// Slot-1 selected, but no cartridge is present.
						// Don't print "START" and the cursor border.
					} else {
						if (!isDemo || settings.ui.cursorPosition == -2) {
							if (showbubble) {
								// Print "START" and the cursor border.
								if (settings.ui.iconsize) {
									if (settings.ui.theme != THEME_3DSMENU) {
										pp2d_draw_texture_scale(startbordertex, 120+startbordermovepos, 104+startbordermovepos, startborderscalesize+0.25, startborderscalesize+0.25);
										const wchar_t *start_text = TR(STR_START);
										if(language==13) {
											text_width = pp2d_get_wtext_width(start_text, 0.50, 0.60);
											pp2d_draw_wtext(((320-text_width)/2), 180, 0.50, 0.60, WHITE, start_text);
										} else {
											text_width = pp2d_get_wtext_width(start_text, 0.60, 0.60);
											pp2d_draw_wtext(((320-text_width)/2), 180, 0.60, 0.60, WHITE, start_text);
										}
									} else {
										pp2d_draw_texture_scale(startbordertex, 120+startbordermovepos, 116+startbordermovepos, startborderscalesize, startborderscalesize);
									}
								} else {
									if (settings.ui.theme != THEME_3DSMENU) {
										pp2d_draw_texture_scale(startbordertex, 128+startbordermovepos, 112+startbordermovepos, startborderscalesize, startborderscalesize);
										const wchar_t *start_text = TR(STR_START);
										if(language==13) {
											text_width = pp2d_get_wtext_width(start_text, 0.40, 0.50);
											pp2d_draw_wtext(((320-text_width)/2), 173, 0.40, 0.50, WHITE, start_text);
										} else {
											text_width = pp2d_get_wtext_width(start_text, 0.50, 0.50);
											pp2d_draw_wtext(((320-text_width)/2), 173, 0.50, 0.50, WHITE, start_text);
										}
									} else {
										pp2d_draw_texture_scale(startbordertex, 130+startbordermovepos, 116+startbordermovepos, startborderscalesize-0.25, startborderscalesize-0.25);
									}
								}
							}
						}
					}
				} else if (settings.ui.theme != THEME_3DSMENU) {
					if (settings.ui.custombot)
						pp2d_draw_texture_part(bottomtex, 128, 112, 128, 112, 64, 80);
					else
						pp2d_draw_texture_part_blend(bottomtex, 128, 112, 128, 112, 64, 80, (u32) SET_ALPHA(menucolor, 255));  // Cover selected game/app
					if (settings.ui.cursorPosition == -2) {
						pp2d_draw_texture(settingsicontex, 128, titleboxYmovepos-1); // Draw settings box that moves up
					} else if (settings.ui.cursorPosition == -1) {
						if (settings.twl.forwarder)
							pp2d_draw_texture(getfcgameboxtex, 128, titleboxYmovepos-1);
						else {
							// Draw selected Slot-1 game that moves up
							pp2d_draw_texture(carttex(), 128, titleboxYmovepos);
							size_t cardicontex = gamecardGetIcon();
							if (cardicontex != bnricontex[8+bnriconPalLine[gamesPerPage+1]*8])
								cardicontex = iconnulltex;
							pp2d_draw_texture_part_flip(cardicontex, 144, ndsiconYmovepos, 0, bnriconframenumY[gamesPerPage+1]*32, 32, 32, bannerFlip[gamesPerPage+1]);
						}
					} else {
						pp2d_draw_texture_part(boxtex, 128, titleboxYmovepos, 0, 0, 64, 64); // Draw selected game/app that moves up
						bnriconnum = settings.ui.cursorPosition;
						ChangeBNRIconNo();
						pp2d_draw_texture_part_flip(bnricontexnum, 144, ndsiconYmovepos, 0, bnriconframenumY[bnriconnum-settings.ui.pagenum*gamesPerPage]*32, 32, 32, bannerFlip[bnriconnum-settings.ui.pagenum*gamesPerPage]);
					}
					pp2d_draw_texture_rotate(dotcircletex, 120, 104, rad);  // Dots moving in circles
				}
				if (showbubble) {
					pp2d_draw_texture(bubbletex, 0, 0);
					if (settings.ui.theme == THEME_3DSMENU) {
						pp2d_draw_texture(_3dsbotbotbarbuttex, 0, 240-28);
						const wchar_t *start_text = TR(STR_START);
						text_width = pp2d_get_wtext_width(start_text, 0.60, 0.60);
						pp2d_draw_wtext(((320-text_width)/2), 219.50, 0.60, 0.60, WHITE, start_text);
						pp2d_draw_wtext(((320-text_width)/2), 218, 0.60, 0.60, BLACK, start_text);
					}
					bool drawBannerText = true;
					if (settings.ui.cursorPosition == -2) {
						const wchar_t *curn2text = TR(STR_SETTINGS_TEXT);
						text_width = pp2d_get_wtext_width(curn2text, 0.70, 0.70);
						if (settings.ui.theme != THEME_3DSMENU) {
							pp2d_draw_wtext(((320-text_width)/2), 38, 0.70, 0.70, BLACK, curn2text);
						} else {
							pp2d_draw_wtext(((320-text_width)/2), 68, 0.70, 0.70, BLACK, curn2text);
						}
						drawBannerText = false;
					} else if (settings.ui.cursorPosition == -1) {
						if (settings.twl.forwarder) {
							static const char add_games_text[] = "Add Games";
							text_width = pp2d_get_text_width(add_games_text, 0.70, 0.70);
							if (settings.ui.theme != THEME_3DSMENU) {
								pp2d_draw_text(((320-text_width)/2), 38, 0.70, 0.70, BLACK, add_games_text);
							} else {
								pp2d_draw_text(((320-text_width)/2), 68, 0.70, 0.70, BLACK, add_games_text);
							}
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
								text_width = pp2d_get_wtext_width(msg, 0.70, 0.70);
								if (settings.ui.theme != THEME_3DSMENU) {
									pp2d_draw_wtext(((320-text_width)/2), 38, 0.70, 0.70, BLACK, msg);
								} else {
									pp2d_draw_wtext(((320-text_width)/2), 68, 0.70, 0.70, BLACK, msg);
								}
								drawBannerText = false;
							}
						}
					} else {
						if (!bannertextloaded) {
							char path[256];
							if (settings.twl.romtype == 0) {
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
										romsel_filename = matching_files.at(storedcursorPosition).c_str();
										romsel_filename_w = utf8_to_wstring(romsel_filename);
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
										romsel_filename = matching_files.at(storedcursorPosition).c_str();
										romsel_filename_w = utf8_to_wstring(romsel_filename);
										snprintf(path, sizeof(path), "%s/%s.bin", bnriconfolder, romsel_filename);
									}									
								}

								if (access(path, F_OK) == -1) {
									// Banner file is not available.
									strcpy(path, "romfs:/notextbanner");
								}
								bnriconnum = settings.ui.cursorPosition;
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
							} else {
								if(matching_files.size() == 0){
									if (settings.twl.romtype == 1) {
										if (gbfiles.size() != 0) {
											romsel_filename = gbfiles.at(storedcursorPosition).c_str();
											romsel_filename_w = utf8_to_wstring(romsel_filename);
										} else {
											romsel_filename = " ";
											romsel_filename_w = utf8_to_wstring(romsel_filename);
										}
									} else if (settings.twl.romtype == 2) {
										if (nesfiles.size() != 0) {
											romsel_filename = nesfiles.at(storedcursorPosition).c_str();
											romsel_filename_w = utf8_to_wstring(romsel_filename);
										} else {
											romsel_filename = " ";
											romsel_filename_w = utf8_to_wstring(romsel_filename);
										}
									}
									snprintf(path, sizeof(path), "%s/%s.bin", bnriconfolder, romsel_filename);
								}else {
									romsel_filename = matching_files.at(storedcursorPosition).c_str();
									romsel_filename_w = utf8_to_wstring(romsel_filename);
									snprintf(path, sizeof(path), "%s/%s.bin", bnriconfolder, romsel_filename);
								}									
							}
							bannertextloaded = true;
						}
					}

					if (drawBannerText) {
						int y, dy;
						//top dialog = 100px tall
						if (settings.twl.romtype != 0) {
							if (settings.ui.theme != THEME_3DSMENU) {
								pp2d_draw_wtext(10, 40, 0.75, 0.75, BLACK, romsel_filename_w.c_str());
							} else {
								pp2d_draw_wtext(36, 68, 0.65, 0.65, BLACK, romsel_filename_w.c_str());
							}
						} else {
							if (settings.ui.theme != THEME_3DSMENU) {
								if (settings.ui.filename) {
									pp2d_draw_wtext(10, 8, 0.50, 0.50, GRAY, romsel_filename_w.c_str());
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
									const int text_width = pp2d_get_wtext_width(romsel_gameline[i].c_str(), 0.75, 0.75);
									pp2d_draw_wtext((320-text_width)/2, y, 0.75, 0.75, BLACK, romsel_gameline[i].c_str());
								}
							} else {
								if (settings.ui.filename) {
									pp2d_draw_wtext(10, 196, 0.50, 0.50, GRAY, romsel_filename_w.c_str());
								}
								y = (150-(19*romsel_gameline.size()))/2 + 4;
								//y = 24; dy = 19;
								dy = 20;

								// Print the banner text, center-aligned.
								const size_t banner_lines = std::min(3U, romsel_gameline.size());
								for (size_t i = 0; i < banner_lines; i++, y += dy) {
									const int text_width = pp2d_get_wtext_width(romsel_gameline[i].c_str(), 0.65, 0.65);
									pp2d_draw_wtext((320-text_width)/2, y, 0.65, 0.65, BLACK, romsel_gameline[i].c_str());
								}
							}
						}

						if (settings.ui.cursorPosition >= 0 && settings.ui.counter) {
							char romsel_counter1[16];
							snprintf(romsel_counter1, sizeof(romsel_counter1), "%d", storedcursorPosition+1);
							const char *p_romsel_counter;
							if (settings.twl.romtype == 0) {
								if (settings.twl.forwarder) {
									p_romsel_counter = romsel_counter2fc;
								} else {
									p_romsel_counter = romsel_counter2sd;
								}
							} else if (settings.twl.romtype == 1) {
								p_romsel_counter = romsel_counter2gb;
							} else if (settings.twl.romtype == 2) {
								p_romsel_counter = romsel_counter2nes;
							}
							if (settings.ui.theme != THEME_3DSMENU) {
								if (file_count < 100) {
									pp2d_draw_text(8, 96, 0.50, 0.50, BLACK, romsel_counter1);
									pp2d_draw_text(27, 96, 0.50, 0.50, BLACK, "/");
									pp2d_draw_text(32, 96, 0.50, 0.50, BLACK, p_romsel_counter);
								} else {
									pp2d_draw_text(8, 96, 0.50, 0.50, BLACK, romsel_counter1);
									pp2d_draw_text(35, 96, 0.50, 0.50, BLACK, "/");
									pp2d_draw_text(40, 96, 0.50, 0.50, BLACK, p_romsel_counter);
								}
							} else {
								if (file_count < 100) {
									pp2d_draw_text(8, 220, 0.50, 0.50, WHITE, romsel_counter1);
									pp2d_draw_text(27, 220, 0.50, 0.50, WHITE, "/");
									pp2d_draw_text(32, 220, 0.50, 0.50, WHITE, p_romsel_counter);
									pp2d_draw_text(8, 219, 0.50, 0.50, BLACK, romsel_counter1);
									pp2d_draw_text(27, 219, 0.50, 0.50, BLACK, "/");
									pp2d_draw_text(32, 219, 0.50, 0.50, BLACK, p_romsel_counter);
								} else {
									pp2d_draw_text(8, 220, 0.50, 0.50, WHITE, romsel_counter1);
									pp2d_draw_text(35, 220, 0.50, 0.50, WHITE, "/");
									pp2d_draw_text(40, 220, 0.50, 0.50, WHITE, p_romsel_counter);
									pp2d_draw_text(8, 219, 0.50, 0.50, BLACK, romsel_counter1);
									pp2d_draw_text(35, 219, 0.50, 0.50, BLACK, "/");
									pp2d_draw_text(40, 219, 0.50, 0.50, BLACK, p_romsel_counter);
								}
							}
						}
					}
				} else {
					if (settings.ui.theme == THEME_DSIMENU) pp2d_draw_texture(bottomlogotex, 320/2 - 189/2, 40);
				}
				if (menudbox_Ypos != -240) {
					// Draw the menu dialog box.
					drawMenuDialogBox();
				}
				if (fadealpha > 0) {
					pp2d_draw_rectangle(0, 0, 320, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect
					if (fadealpha == 255) screenoff();
				} else {
					screenon();
				}

				// Playback animated icons on SD/flash card
				for (int i = 0; i < gamesPerPage; i++) {
					if(bnriconisDSi[i]==true) {
						playBannerSequence(i);
					}
				}
				// Playback animated icon on Slot-1
				if(bnriconisDSi[gamesPerPage+1]==true) {
					playBannerSequence(gamesPerPage+1);
				} else {
					bnriconPalLine[gamesPerPage+1] = 0;
					bnriconframenumY[gamesPerPage+1] = 0;
					bannerFlip[gamesPerPage+1] = NONE;
				}
			}
		} else if (screenmode == SCREEN_MODE_SETTINGS) {
			settingsDrawBottomScreen();
			if (fadealpha > 0) {
				pp2d_draw_rectangle(0, 0, 320, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect
				if (fadealpha == 255) screenoff();
			} else {
				screenon();
			}
		}
		pp2d_end_draw();

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
		
		if (fadein) {
			showbubble = false;
			fadealpha -= 31;
			if (fadealpha < 0) {
				fadealpha = 0;
				fadein = false;
				titleboxXmovetimer = 0;
				if (settings.ui.cursorPosition >= 0) {
					if (settings.ui.cursorPosition >= (int)file_count)
						showbubble = false;
					else
						showbubble = true;
				}
			}
		}
		
		if (fadeout) {
			showbubble = false;
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
					
					// Clear delete queue
					if (delete_queue.size() != 0) {
						delete_queue.clear();
					}
					
					// Clear matching_files vector
					if(matching_files.size() != 0) {
						matching_files.clear(); // Clear filter
						snprintf(romsel_counter2sd, sizeof(romsel_counter2sd), "%zu", files.size()); // Reload counter
						snprintf(romsel_counter2fc, sizeof(romsel_counter2fc), "%zu", fcfiles.size()); // Reload counter for FlashCard
						snprintf(romsel_counter2gb, sizeof(romsel_counter2gb), "%zu", gbfiles.size()); // Reload counter for GB ROMs
						boxarttexloaded = false; // Reload boxarts
						bnricontexloaded = false; // Reload banner icons
						pp2d_set_screen_color(GFX_TOP, TRANSPARENT);
					}
					if (settings.ui.theme == THEME_AKMENU) {
						pp2d_set_3D(0);
						pp2d_set_screen_color(GFX_TOP, TRANSPARENT);
						menu_ctrlset = CTRL_SET_MENU;
						woodmenu_cursorPosition = 4;
						if (settings.ui.cursorPosition < 0)
							settings.ui.cursorPosition = 0;
					} else if (settings.ui.theme == THEME_R4) {
						pp2d_set_3D(0);
						pp2d_set_screen_color(GFX_TOP, TRANSPARENT);
						menu_ctrlset = CTRL_SET_MENU;
						titleboxXmovepos = 0;
						boxartXmovepos = 0;
						if (settings.ui.cursorPosition < 0)
							settings.ui.cursorPosition = 0;
					} else {
						pp2d_set_3D(1);
						titleboxXmovepos = 0;
						settings.ui.cursorPosition = 0 + settings.ui.pagenum * gamesPerPage; // This is to reset cursor position after switching from R4 theme.
						if (settings.twl.forwarder) {
							if (fcfiles.size() <= 0) {
								startbordermovepos = 0;
								startborderscalesize = 1.0;
								// No ROMs were found.
								settings.ui.cursorPosition = -1;
								titleboxXmovepos = +64;
								noromsfound = true;
							}
						} else {
							if (files.size() <= 0) {
								startbordermovepos = 0;
								startborderscalesize = 1.0;
								// No ROMs were found.
								settings.ui.cursorPosition = -1;
								titleboxXmovepos = +64;
								noromsfound = true;
							}
						}							
						storedcursorPosition = settings.ui.cursorPosition; // This is to reset cursor position after switching from R4 theme.
						boxartXmovepos = 0;
						loadboxartnum = settings.ui.pagenum*gamesPerPage;
						boxarttexloaded = false;
						bnricontexloaded = false;
						menu_ctrlset = CTRL_SET_GAMESEL;
					}
					colortexloaded = false; // Reload top textures
					colortexloaded_bot = false; // Reload bottom textures
				} else if (gbarunnervalue == 1) {
					if (logEnabled)	LogFM("Main", "Loading GBARunner.");
					applaunchon = true;
				} else if (screenmodeswitch) {
					screenmode = SCREEN_MODE_SETTINGS;
					settingsResetSubScreenMode();
					rad = 0.0f;
					boxartYmovepos = 63;
					boxartreflYmovepos = 178;
					scrollwindowXmovepos = 0;
					titleboxYmovepos = 116;
					ndsiconYmovepos = 129;
					fadeout = false;
					fadein = true;
					screenmodeswitch = false;
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
				bannertextloaded = false;
				if (settings.ui.cursorPosition >= 0) {
					if (settings.ui.cursorPosition >= (int)file_count)
						showbubble = false;
					else {
						showbubble = true;
						if (dspfirmfound) {
							sfx_stop->stop();
							sfx_stop->play();
						}
					}
				} else {
					showbubble = true;
					if (dspfirmfound) {
						sfx_stop->stop();
						sfx_stop->play();
					}
				}
				storedcursorPosition = settings.ui.cursorPosition;
			} else if (titleboxXmovetimer == 9) {
				startbordermovepos = 0;
				startborderscalesize = 1.0;
			} else if (titleboxXmovetimer == 8) {
				scrollwindowXmovepos -= 0.75;
				titleboxXmovepos += 8;
				boxartXmovepos += 18;
				startbordermovepos = 1;
				if(settings.ui.theme == THEME_3DSMENU) startbordermovepos++;
				startborderscalesize = 0.97;
				cursorPositionset = false;
			} else if (titleboxXmovetimer == 2) {
				if (dspfirmfound) {
					sfx_select->stop();
					sfx_select->play();
				}
				scrollwindowXmovepos -= 0.75;
				titleboxXmovepos += 8;
				boxartXmovepos += 18;
				// Load the previous box art
				if ( settings.ui.cursorPosition >= 1+settings.ui.pagenum*gamesPerPage
				&& settings.ui.cursorPosition <= 36+settings.ui.pagenum*gamesPerPage ) {
					boxartpage--;
					boxartnum = settings.ui.cursorPosition-1;
					LoadBoxArt();
					if (settings.twl.romtype == 0) LoadBNRIcon_Menu((settings.ui.cursorPosition-2)-settings.ui.pagenum*gamesPerPage);
				}
				if ( settings.ui.cursorPosition == 6+settings.ui.pagenum*gamesPerPage ||
				settings.ui.cursorPosition == 12+settings.ui.pagenum*gamesPerPage ||
				settings.ui.cursorPosition == 18+settings.ui.pagenum*gamesPerPage ||
				settings.ui.cursorPosition == 24+settings.ui.pagenum*gamesPerPage ||
				settings.ui.cursorPosition == 30+settings.ui.pagenum*gamesPerPage ||
				settings.ui.cursorPosition == 36+settings.ui.pagenum*gamesPerPage ) {
					boxartXmovepos = -144*7;
					boxartXmovepos += 18*2;
				}
			} else {
				if (!cursorPositionset) {
					settings.ui.cursorPosition--;
					cursorPositionset = true;
				}
				if (settings.ui.pagenum == 0) {
					if (settings.ui.cursorPosition != -3) {
						scrollwindowXmovepos -= 0.75;
						titleboxXmovepos += 8;
						boxartXmovepos += 18;
					} else {
						titleboxXmovetimer = 0;
						titleboxXmoveleft = false;
						cursorPositionset = false;
						settings.ui.cursorPosition++;
						if (!playwrongsounddone) {
							if (dspfirmfound) {
								sfx_wrong->stop();
								sfx_wrong->play();
							}
							playwrongsounddone = true;
						}
					}
				} else {
					if (settings.ui.cursorPosition != -1+settings.ui.pagenum*gamesPerPage) {
						scrollwindowXmovepos -= 0.75;
						titleboxXmovepos += 8;
						boxartXmovepos += 18;
					} else {
						titleboxXmovetimer = 0;
						titleboxXmoveleft = false;
						cursorPositionset = false;
						settings.ui.cursorPosition++;
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
				bannertextloaded = false;
				if (settings.ui.cursorPosition >= 0) {
					if (settings.ui.cursorPosition >= (int)file_count)
						showbubble = false;
					else {
						showbubble = true;
						if (dspfirmfound) {
							sfx_stop->stop();
							sfx_stop->play();
						}
					}
				} else {
					showbubble = true;
					if (dspfirmfound) {
						sfx_stop->stop();
						sfx_stop->play();
					}
				}
				storedcursorPosition = settings.ui.cursorPosition;
				// Load the next box art
				if ( settings.ui.cursorPosition >= 3+settings.ui.pagenum*gamesPerPage
				&& settings.ui.cursorPosition <= 39+settings.ui.pagenum*gamesPerPage ) {
					if (settings.ui.cursorPosition != 3) {
						boxartpage++;
						boxartnum = settings.ui.cursorPosition+2;
						LoadBoxArt();
					}
					if (settings.twl.romtype == 0) LoadBNRIcon_Menu((settings.ui.cursorPosition+3)-settings.ui.pagenum*gamesPerPage);
				}
				if ( settings.ui.cursorPosition == 7+settings.ui.pagenum*gamesPerPage ||
				settings.ui.cursorPosition == 13+settings.ui.pagenum*gamesPerPage ||
				settings.ui.cursorPosition == 19+settings.ui.pagenum*gamesPerPage ||
				settings.ui.cursorPosition == 25+settings.ui.pagenum*gamesPerPage ||
				settings.ui.cursorPosition == 31+settings.ui.pagenum*gamesPerPage ||
				settings.ui.cursorPosition == 37+settings.ui.pagenum*gamesPerPage ) {
					boxartXmovepos = -144;
				}
			} else if (titleboxXmovetimer == 9) {
				startbordermovepos = 0;
				startborderscalesize = 1.0;
			} else if (titleboxXmovetimer == 8) {
				scrollwindowXmovepos += 0.75;
				titleboxXmovepos -= 8;
				boxartXmovepos -= 18;
				startbordermovepos = 1;
				if(settings.ui.theme == THEME_3DSMENU) startbordermovepos++;
				startborderscalesize = 0.97;
				cursorPositionset = false;
			} else if (titleboxXmovetimer == 2) {
				if (dspfirmfound) {
					sfx_select->stop();
					sfx_select->play();
				}
				scrollwindowXmovepos += 0.75;
				titleboxXmovepos -= 8;
				boxartXmovepos -= 18;
			} else {
				if (!cursorPositionset) {
					settings.ui.cursorPosition++;
					cursorPositionset = true;
				}
				if (settings.ui.cursorPosition != filenum) {
					scrollwindowXmovepos += 0.75;
					titleboxXmovepos -= 8;
					boxartXmovepos -= 18;
				} else {
					titleboxXmovetimer = 0;
					titleboxXmoveright = false;
					cursorPositionset = false;
					settings.ui.cursorPosition--;
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
			if (titleboxYmovepos < -240) {
				if (screenmodeswitch) {
					musicbool = false;
					screenmode = SCREEN_MODE_SETTINGS;
					settingsResetSubScreenMode();
					rad = 0.0f;
					boxartYmovepos = 63;
					boxartreflYmovepos = 178;
					scrollwindowXmovepos = 0;
					titleboxYmovepos = 116;
					ndsiconYmovepos = 129;
					fadein = true;
					screenmodeswitch = false;
					applaunchprep = false;
				} else {
					if (logEnabled)	LogFM("Main.applaunchprep", "Switching to NTR/TWL-mode");
					applaunchon = true;
				}
			}
			if (settings.ui.theme == THEME_3DSMENU) {
				fadealpha += 14;
			} else {
				fadealpha += 6;
			}
			if (fadealpha > 255) {
				showbubble = false;
				fadealpha = 255;
			}
		}

		// Controls
		if (screenmode == SCREEN_MODE_ROM_SELECT) {
			if (settings.ui.theme == THEME_AKMENU) {
				if (menu_ctrlset == CTRL_SET_MENU) {
					if (hDown & KEY_A) {
						switch (woodmenu_cursorPosition) {
							case 0:
							default:
								if (sdfile_count != 0 || gbfile_count != 0 || nesfile_count != 0) {
									menu_ctrlset = CTRL_SET_GAMESEL;
									if (settings.twl.forwarder) {
										settings.twl.forwarder = false;
										settings.ui.cursorPosition = 0;
										settings.ui.pagenum = 0;
										boxarttexloaded = false; // Reload boxarts
										bnricontexloaded = false; // Reload banner icons
										colortexloaded = false; // Reload top textures
										colortexloaded_bot = false; // Reload bottom textures
										boxartpage = 0;
										loadboxartnum = 0+settings.ui.pagenum*gamesPerPage;
										loadbnriconnum = 0+settings.ui.pagenum*gamesPerPage;
										for (int i = 0; i < gamesPerPage; i++) {
											// Reset banner icon frames
											bnriconPalLine[i] = 0;
											bnriconframenumY[i] = 0;
											bannerFlip[i] = NONE;
											clearBannerSequence(i);
										}
										bannertextloaded = false;
									}
								}
								break;
							case 1:
								if (fcfile_count != 0 && settings.twl.romtype == 0) {
									menu_ctrlset = CTRL_SET_GAMESEL;
									if (!settings.twl.forwarder) {
										settings.twl.forwarder = true;
										settings.ui.cursorPosition = 0;
										settings.ui.pagenum = 0;
										boxarttexloaded = false; // Reload boxarts
										bnricontexloaded = false; // Reload banner icons
										colortexloaded = false; // Reload top textures
										colortexloaded_bot = false; // Reload bottom textures
										boxartpage = 0;
										loadboxartnum = 0+settings.ui.pagenum*gamesPerPage;
										loadbnriconnum = 0+settings.ui.pagenum*gamesPerPage;
										for (int i = 0; i < gamesPerPage; i++) {
											// Reset banner icon frames
											bnriconPalLine[i] = 0;
											bnriconframenumY[i] = 0;
											bannerFlip[i] = NONE;
											clearBannerSequence(i);
										}
										bannertextloaded = false;
									}
								}
								break;
							case 2:
								if (!isDemo) {
									settings.twl.launchslot1 = true;
									settings.twl.forwarder = false;
									if (logEnabled)	LogFM("Main", "Switching to NTR/TWL-mode");
									applaunchon = true;
								}
								break;
							case 3:
								menu_ctrlset = CTRL_SET_ROMTYPE;
								setromtype_cursorPosition = settings.twl.romtype;
								if(settings.twl.romtype == 1) setromtype_cursorPosition = 2;
								else if(settings.twl.romtype == 2) setromtype_cursorPosition = 3;
								break;
							case 4:
								pp2d_set_3D(1);
								screenmode = SCREEN_MODE_SETTINGS;
								settingsResetSubScreenMode();
								break;
						}
						wood_ndsiconscaletimer = 0;
					} else if((hDown & KEY_Y) && (woodmenu_cursorPosition == 1) && !isDemo && settings.twl.romtype == 0){
						settings.twl.forwarder = true;
						settings.twl.launchslot1 = true;
						keepsdvalue = true;
						rom = "_nds/twloader.nds";
						usepergamesettings = false;
						if (logEnabled)	LogFM("Main", "Switching to NTR/TWL-mode");
						applaunchon = true;
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
					} else if(hDown & KEY_SELECT){
						settings.ui.woodIconScaleEffect = !settings.ui.woodIconScaleEffect;
					}
				} else if (menu_ctrlset == CTRL_SET_ROMTYPE) {
					if (hDown & KEY_A) {
						switch (setromtype_cursorPosition) {
							case 0:
							default:
								if (sdfile_count != 0 || fcfile_count != 0) {
									settings.twl.romtype = 0;
									menu_ctrlset = CTRL_SET_MENU;
									if (sdfile_count != 0) settings.twl.forwarder = false;
									else settings.twl.forwarder = true;
									settings.ui.cursorPosition = 0;
									settings.ui.pagenum = 0;
									boxarttexloaded = false; // Reload boxarts
									bnricontexloaded = false; // Reload banner icons
									colortexloaded = false; // Reload top textures
									colortexloaded_bot = false; // Reload bottom textures
									boxartpage = 0;
									loadboxartnum = 0+settings.ui.pagenum*gamesPerPage;
									loadbnriconnum = 0+settings.ui.pagenum*gamesPerPage;
									for (int i = 0; i < gamesPerPage; i++) {
										// Reset banner icon frames
										bnriconPalLine[i] = 0;
										bnriconframenumY[i] = 0;
										bannerFlip[i] = NONE;
										clearBannerSequence(i);
									}
									bannertextloaded = false;
								}
								break;
							case 1:
								if (!isDemo) {
									gbarunnervalue = 1;
									if (settings.twl.forwarder)
										settings.twl.launchslot1 = true;
									else
										settings.twl.launchslot1 = false;
									if (logEnabled)	LogFM("Main", "Switching to NTR/TWL-mode");
									applaunchon = true;
								}
								break;
							case 2:
								if (gbfile_count != 0) {
									settings.twl.romtype = 1;
									menu_ctrlset = CTRL_SET_MENU;
									settings.twl.forwarder = false;
									settings.ui.cursorPosition = 0;
									settings.ui.pagenum = 0;
									boxarttexloaded = false; // Reload boxarts
									bnricontexloaded = false; // Reload banner icons
									colortexloaded = false; // Reload top textures
									colortexloaded_bot = false; // Reload bottom textures
									boxartpage = 0;
									loadboxartnum = 0+settings.ui.pagenum*gamesPerPage;
									loadbnriconnum = 0+settings.ui.pagenum*gamesPerPage;
									for (int i = 0; i < gamesPerPage; i++) {
										// Reset banner icon frames
										bnriconPalLine[i] = 0;
										bnriconframenumY[i] = 0;
										bannerFlip[i] = NONE;
										clearBannerSequence(i);
									}
									bannertextloaded = false;
								}
								break;
							case 3:
								if (nesfile_count != 0) {
									settings.twl.romtype = 2;
									menu_ctrlset = CTRL_SET_MENU;
									settings.twl.forwarder = false;
									settings.ui.cursorPosition = 0;
									settings.ui.pagenum = 0;
									boxarttexloaded = false; // Reload boxarts
									bnricontexloaded = false; // Reload banner icons
									colortexloaded = false; // Reload top textures
									colortexloaded_bot = false; // Reload bottom textures
									boxartpage = 0;
									loadboxartnum = 0+settings.ui.pagenum*gamesPerPage;
									loadbnriconnum = 0+settings.ui.pagenum*gamesPerPage;
									for (int i = 0; i < gamesPerPage; i++) {
										// Reset banner icon frames
										bnriconPalLine[i] = 0;
										bnriconframenumY[i] = 0;
										bannerFlip[i] = NONE;
										clearBannerSequence(i);
									}
									bannertextloaded = false;
								}
								break;
						}
						wood_ndsiconscaletimer = 0;
					} else if(hDown & KEY_DOWN){
						setromtype_cursorPosition++;
						if (setromtype_cursorPosition > 3) {
							setromtype_cursorPosition = 3;
						}
						wood_ndsiconscaletimer = 0;
					} else if(hDown & KEY_UP){
						setromtype_cursorPosition--;
						if (setromtype_cursorPosition < 0) {
							setromtype_cursorPosition = 0;
						}
						wood_ndsiconscaletimer = 0;
					} else if (hDown & KEY_B) {
						menu_ctrlset = CTRL_SET_MENU;
						wood_ndsiconscaletimer = 0;
					} else if(hDown & KEY_SELECT){
						settings.ui.woodIconScaleEffect = !settings.ui.woodIconScaleEffect;
					}
				} else {
					if(settings.ui.cursorPosition < 0) {
						filenameYmovepos = 0;
						titleboxXmovepos -= 64;
						boxartXmovepos -= 18*8;
						settings.ui.cursorPosition = 0;
					}
					if(hDown & KEY_A){
						if (!isDemo) {
							if (settings.twl.forwarder) {
								settings.twl.launchslot1 = true;
								rom = fcfiles.at(settings.ui.cursorPosition).c_str();
								if (logEnabled)	LogFM("Main", "Switching to NTR/TWL-mode");
								applaunchon = true;
							} else {
								settings.twl.launchslot1 = false;
								bool isCia = false;
								bool overlaysIncluded = false;
								bool donorFound = true;
								if (settings.twl.romtype == 0) {
									rom = files.at(settings.ui.cursorPosition).c_str();
									if(settings.ui.cursorPosition >= 0) {
										char path[256];
										snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.romfolder.c_str(), rom);
										std::string fn = rom;
										if(fn.substr(fn.find_last_of(".") + 1) == "cia") isCia = true;

										if(isCia) {
											FILE *f_nds_file = fopen(path, "rb");
											if (f_nds_file) {
												overlaysIncluded = getOverlaySize(f_nds_file, rom, isCia);
												fclose(f_nds_file);
											}
										} else {
											donorpath = bootstrapini.GetString(bootstrapini_ndsbootstrap, bootstrapini_arm7donorpath, "");
											// Show "Donor ROM not set" message, if game is SDK3-4, but not MKDS, and donor path is blank
											if (donorpath.compare("") == 0) {
												FILE *f_nds_file = fopen(path, "rb");
												char game_TID[5];
												grabTID(f_nds_file, game_TID, false);
												game_TID[4] = 0;
												game_TID[3] = 0;
												SDKVersion = 0;
												if(strcmp(game_TID, "###") != 0) {
													SDKVersion = getSDKVersion(f_nds_file, rom);
													if((SDKVersion > 0x3000000) && (SDKVersion < 0x5000000) && (strcmp(game_TID, "AMC") != 0)) {
														donorFound = false;
													}
												}
												fclose(f_nds_file);
											}
										}
									}
								} else if (settings.twl.romtype == 1) {
									homebrew_arg = gbfiles.at(settings.ui.cursorPosition).c_str();
								} else if (settings.twl.romtype == 2) {
									homebrew_arg = nesfiles.at(settings.ui.cursorPosition).c_str();
								}

								if(!overlaysIncluded && donorFound) {
									if (settings.twl.romtype == 0) sav = ReplaceAll(rom, ".nds", ".sav");
									if (logEnabled)	LogFM("Main", "Switching to NTR/TWL-mode");
									applaunchon = true;
								}
							}
						}
						wood_ndsiconscaletimer = 0;
					} else if((hDown & KEY_Y) && settings.twl.forwarder && !isDemo){
						settings.twl.launchslot1 = true;
						keepsdvalue = true;
						rom = "_nds/twloader.nds";
						usepergamesettings = false;
						if (logEnabled)	LogFM("Main", "Switching to NTR/TWL-mode");
						applaunchon = true;
					} else if(hDown & KEY_L){
						if ((size_t)settings.ui.pagenum != 0 && file_count <= (size_t)0-settings.ui.pagenum*gamesPerPage) {
							settings.ui.pagenum--;
							bannertextloaded = false;
							settings.ui.cursorPosition = 0+settings.ui.pagenum*gamesPerPage;
							bnricontexloaded = false;
							boxarttexloaded = false;
							boxartpage = 0;
							loadboxartnum = 0+settings.ui.pagenum*gamesPerPage;
							loadbnriconnum = 0+settings.ui.pagenum*gamesPerPage;
							for (int i = 0; i < gamesPerPage; i++) {
								// Reset banner icon frames
								bnriconPalLine[i] = 0;
								bnriconframenumY[i] = 0;
								bannerFlip[i] = NONE;
								clearBannerSequence(i);
							}
						}
					} else if(hDown & KEY_R){
						if (file_count > (size_t)pagemax) {
							settings.ui.pagenum++;
							bannertextloaded = false;
							settings.ui.cursorPosition = 0+settings.ui.pagenum*gamesPerPage;
							bnricontexloaded = false;
							boxarttexloaded = false;
							boxartpage = 0;
							loadboxartnum = 0+settings.ui.pagenum*gamesPerPage;
							loadbnriconnum = 0+settings.ui.pagenum*gamesPerPage;
							for (int i = 0; i < gamesPerPage; i++) {
								// Reset banner icon frames
								bnriconPalLine[i] = 0;
								bnriconframenumY[i] = 0;
								bannerFlip[i] = NONE;
								clearBannerSequence(i);
							}
						}
					} else if(hDown & KEY_DOWN){
						settings.ui.cursorPosition++;
						if (settings.ui.cursorPosition >= pagemax) {
							settings.ui.cursorPosition = 0+settings.ui.pagenum*gamesPerPage;
							for (int i = 0; i < 6; i++) {
								if (settings.twl.romtype == 0) LoadBNRIcon_Menu(i);
							}
						} else if ( settings.ui.cursorPosition >= 3+settings.ui.pagenum*gamesPerPage
								&& settings.ui.cursorPosition <= 36+settings.ui.pagenum*gamesPerPage )
						{
							if (settings.twl.romtype == 0) LoadBNRIcon_Menu((settings.ui.cursorPosition+3)-settings.ui.pagenum*gamesPerPage);
						}
						wood_downpressed = true;
						wood_ndsiconscaletimer = 0;
						bannertextloaded = false;
					} else if((hDown & KEY_UP) && (filenum > 1)){
						settings.ui.cursorPosition--;
						if (settings.ui.cursorPosition < 0+settings.ui.pagenum*gamesPerPage) {
							settings.ui.cursorPosition = pagemax-1;
							for (int i = settings.ui.cursorPosition; i < settings.ui.cursorPosition+6; i++) {
								if (settings.twl.romtype == 0) LoadBNRIcon_Menu((i-5)-settings.ui.pagenum*gamesPerPage);
							}
						} else if ( settings.ui.cursorPosition >= 2+settings.ui.pagenum*gamesPerPage
								&& settings.ui.cursorPosition <= 35+settings.ui.pagenum*gamesPerPage )
						{
							if (settings.twl.romtype == 0) LoadBNRIcon_Menu((settings.ui.cursorPosition-2)-settings.ui.pagenum*gamesPerPage);
						}
						wood_uppressed = true;
						wood_ndsiconscaletimer = 0;
						bannertextloaded = false;
					} else if (hDown & KEY_B) {
						menu_ctrlset = CTRL_SET_MENU;
						wood_ndsiconscaletimer = 0;
					} else if(hDown & KEY_SELECT){
						settings.ui.woodIconScaleEffect = !settings.ui.woodIconScaleEffect;
					}
					if (filenum+settings.ui.pagenum*gamesPerPage > 4) {
						if (settings.ui.cursorPosition-settings.ui.pagenum*gamesPerPage > 2)
							filenameYmovepos = settings.ui.cursorPosition-(settings.ui.pagenum*gamesPerPage)-2;
						else
							filenameYmovepos = 0;
					}
				}
			} else if (settings.ui.theme == THEME_R4) {
				if (menu_ctrlset == CTRL_SET_MENU) {
					if (hDown & KEY_A) {
						switch (r4menu_cursorPosition) {
							case 0: {
							default:
								if (file_count == 0) settings.twl.forwarder = !settings.twl.forwarder;
								size_t file_count2 = (settings.twl.forwarder ? fcfiles.size() : files.size());
								if (file_count2 != 0) menu_ctrlset = CTRL_SET_GAMESEL;
								updatetopscreen = true;
								break;
							} case 1:
								if (!isDemo) {
									settings.twl.launchslot1 = true;
									settings.twl.forwarder = false;
									if (logEnabled)	LogFM("Main", "Switching to NTR/TWL-mode");
									applaunchon = true;
								}
								break;
							case 2:
								if (!isDemo) {
									gbarunnervalue = 1;
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
					} else if (hDown & KEY_START) {
						pp2d_set_3D(1);
						screenmode = SCREEN_MODE_SETTINGS;
						settingsResetSubScreenMode();
						updatetopscreen = true;
					}
				} else {
					if(settings.ui.cursorPosition < 0) {
						filenameYmovepos = 0;
						titleboxXmovepos -= 64;
						boxartXmovepos -= 18*8;
						settings.ui.cursorPosition = 0;
						updatetopscreen = true;
					}
					if((hDown & KEY_L) && settings.twl.romtype == 0) {
						settings.twl.forwarder = !settings.twl.forwarder;
						settings.ui.cursorPosition = 0;
						settings.ui.pagenum = 0;
						boxarttexloaded = false; // Reload boxarts
						bnricontexloaded = false; // Reload banner icons
						colortexloaded = false; // Reload top textures
						colortexloaded_bot = false; // Reload bottom textures
						boxartpage = 0;
						loadboxartnum = 0+settings.ui.pagenum*gamesPerPage;
						loadbnriconnum = 0+settings.ui.pagenum*gamesPerPage;
						for (int i = 0; i < gamesPerPage; i++) {
							// Reset banner icon frames
							bnriconPalLine[i] = 0;
							bnriconframenumY[i] = 0;
							bannerFlip[i] = NONE;
							clearBannerSequence(i);
						}
						bannertextloaded = false;
						updatetopscreen = true;
					}
					if(hDown & KEY_A){
						if (!isDemo) {
							if (settings.twl.forwarder) {
								settings.twl.launchslot1 = true;
								rom = fcfiles.at(settings.ui.cursorPosition).c_str();
								if (logEnabled)	LogFM("Main", "Switching to NTR/TWL-mode");
								applaunchon = true;
							} else {
								settings.twl.launchslot1 = false;
								bool isCia = false;
								bool overlaysIncluded = false;
								bool donorFound = true;
								rom = files.at(settings.ui.cursorPosition).c_str();
								if (settings.twl.romtype == 0) {
									if(settings.ui.cursorPosition >= 0) {
										char path[256];
										snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.romfolder.c_str(), rom);
										std::string fn = rom;
										if(fn.substr(fn.find_last_of(".") + 1) == "cia") isCia = true;

										if(isCia) {
											FILE *f_nds_file = fopen(path, "rb");
											if (f_nds_file) {
												overlaysIncluded = getOverlaySize(f_nds_file, rom, isCia);
												fclose(f_nds_file);
											}
										} else {
											donorpath = bootstrapini.GetString(bootstrapini_ndsbootstrap, bootstrapini_arm7donorpath, "");
											// Show "Donor ROM not set" message, if game is SDK3-4, but not MKDS, and donor path is blank
											if (donorpath.compare("") == 0) {
												FILE *f_nds_file = fopen(path, "rb");
												char game_TID[5];
												grabTID(f_nds_file, game_TID, false);
												game_TID[4] = 0;
												game_TID[3] = 0;
												SDKVersion = 0;
												if(strcmp(game_TID, "###") != 0) {
													SDKVersion = getSDKVersion(f_nds_file, rom);
													if((SDKVersion > 0x3000000) && (SDKVersion < 0x5000000) && (strcmp(game_TID, "AMC") != 0)) {
														donorFound = false;
													}
												}
												fclose(f_nds_file);
											}
										}
									}
								} else if (settings.twl.romtype == 1) {
									homebrew_arg = gbfiles.at(settings.ui.cursorPosition).c_str();
								} else if (settings.twl.romtype == 2) {
									homebrew_arg = nesfiles.at(settings.ui.cursorPosition).c_str();
								}

								if(!overlaysIncluded && donorFound) {
									if (settings.twl.romtype == 0) sav = ReplaceAll(rom, ".nds", ".sav");
									if (logEnabled)	LogFM("Main", "Switching to NTR/TWL-mode");
									applaunchon = true;
								}
							}
						}
					} else if((hDown & KEY_Y) && settings.twl.forwarder && !isDemo){
						settings.twl.launchslot1 = true;
						keepsdvalue = true;
						rom = "_nds/twloader.nds";
						usepergamesettings = false;
						if (logEnabled)	LogFM("Main", "Switching to NTR/TWL-mode");
						applaunchon = true;
					} else if(hDown & KEY_DOWN){
						settings.ui.cursorPosition++;
						if (settings.ui.cursorPosition == filenum) {
							filenameYmovepos = 0;
							settings.ui.cursorPosition = 0;
						}
						bannertextloaded = false;
						updatetopscreen = true;
					} else if((hDown & KEY_UP) && (filenum > 1)){
						if (settings.ui.cursorPosition == 0) {
							settings.ui.cursorPosition = filenum;
						}
						settings.ui.cursorPosition--;
						bannertextloaded = false;
						updatetopscreen = true;
					} else if ((hDown & KEY_LEFT) && (filenum > 10)) {
						settings.ui.cursorPosition -= 10;
						if (settings.ui.cursorPosition < 0) {
							settings.ui.cursorPosition = 0;
						}
						bannertextloaded = false;			
						updatetopscreen = true;
					} else if ((hDown & KEY_RIGHT) && (filenum > 10)) {
						settings.ui.cursorPosition += 10;
						if (settings.ui.cursorPosition >= filenum) {
							settings.ui.cursorPosition = filenum-1;
						}
						bannertextloaded = false;			
						updatetopscreen = true;
					} else if (hDown & KEY_B) {
						menu_ctrlset = CTRL_SET_MENU;
						updatetopscreen = true;
					}
					if (filenum > 15) {
						if (settings.ui.cursorPosition > filenum-8)
							filenameYmovepos = filenum-15;
						else if (settings.ui.cursorPosition > 7)
							filenameYmovepos = settings.ui.cursorPosition-7;
						else
							filenameYmovepos = 0;
					}
				}
			} else {
				if (!noromsfound && file_count == 0) {
					// No ROMs were found.
					settings.ui.cursorPosition = -1;
					storedcursorPosition = settings.ui.cursorPosition;
					titleboxXmovepos = +64;
					boxartXmovepos = 0;
					noromsfound = true;
				}
				if(titleboxXmovetimer == 0 && menu_ctrlset == CTRL_SET_GAMESEL) {
					if(hDown & KEY_R) {
						menuaction_nextpage = true;
					} else if(hDown & KEY_L) {
						menuaction_prevpage = true;
					} else if((hidKeysUp() & KEY_UP) && settings.ui.cursorPosition >= 0 && showbubble) {
						if ((hHeld & KEY_X) | (hDown & KEY_X) | (hidKeysUp() & KEY_X)) {
							addgametodeletequeue = true;							
						}
					} else if((hDown & KEY_X) && settings.ui.cursorPosition >= 0 && showbubble) {
						// Show delete confirmation.
						menudboxmode = DBOX_MODE_DELETE;
						if (!showdialogbox_menu) {
							if (menudbox_Ypos == -240) {
								showdialogbox_menu = true;
								menu_ctrlset = CTRL_SET_DBOX;
								// Reset the cursor positions.
								startmenu_cursorPosition = 0;
								gamesettings_cursorPosition = 0;
							}
						}
					}
					if(hDown & KEY_Y) {
						if (dspfirmfound) {
							sfx_switch->stop();	// Prevent freezing
							sfx_switch->play();
						}
						settings.ui.iconsize = !settings.ui.iconsize;
					}
					if(addgametodeletequeue) {
						// Add game to delete queue.
						addgametodeletequeue = false; // Prevent adding the same game twice.
						
						if (settings.twl.forwarder) {
							if(matching_files.size() == 0){
								rom = fcfiles.at(settings.ui.cursorPosition).c_str();
							} else {
								rom = matching_files.at(settings.ui.cursorPosition).c_str();
							}
						} else {
							if (settings.twl.romtype == 0) {
								if(matching_files.size() == 0){
									rom = files.at(settings.ui.cursorPosition).c_str();
								} else {
									rom = matching_files.at(settings.ui.cursorPosition).c_str();
								}
							} else if (settings.twl.romtype == 1) {
								if(matching_files.size() == 0){
									rom = gbfiles.at(settings.ui.cursorPosition).c_str();
								} else {
									rom = matching_files.at(settings.ui.cursorPosition).c_str();
								}
							} else if (settings.twl.romtype == 2) {
								if(matching_files.size() == 0){
									rom = nesfiles.at(settings.ui.cursorPosition).c_str();
								} else {
									rom = matching_files.at(settings.ui.cursorPosition).c_str();
								}
							}
						}
						
						// If it was marked, unmark it.
						auto it = std::find(delete_queue.begin(), delete_queue.end(), rom);
						if(it != delete_queue.end()) {
							delete_queue.erase(it);
							// TODO unmark texture.	
							if (logEnabled)	LogFMA("Multi delete mode enabled", "Rom remove from queue", rom);							
						} else {
							// Not found, so we can add it to list.
							delete_queue.push_back(rom);
							// TODO mark texture.
							if (logEnabled)	LogFMA("Multi delete mode enabled", "Rom added to queue", rom);
						}					
						std::sort(delete_queue.begin(), delete_queue.end());
					}
					
					hidTouchRead(&touch);
					if(hDown & KEY_TOUCH){
						if (touch.px >= 128 && touch.px <= 192 && touch.py >= 112 && touch.py <= 192) {
							if (showbubble) menuaction_launch = true;
						} else if ((touch.px < 128 && touch.py >= 118 && touch.py <= 180 && menudbox_Ypos == -240)
								|| (touch.px <= 25 && touch.py >= 212 && menudbox_Ypos == -240)) {
							if (!titleboxXmoveright) {
								titleboxXmoveleft = true;
							}
						} else if ((touch.px > 192 && touch.py >= 118 && touch.py <= 180 && menudbox_Ypos == -240)
								|| (touch.px >= 295 && touch.py >= 212 && menudbox_Ypos == -240)) {
							if (!titleboxXmoveleft) {
								if (settings.ui.cursorPosition == -1) {
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
						}
					} else if((hDown & KEY_A) && showbubble){
						menuaction_launch = true;
					} else if(hHeld & KEY_RIGHT && menudbox_Ypos == -240) {
						if (!titleboxXmoveleft) {
							if (settings.ui.cursorPosition == -1) {
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
					} else if(hHeld & KEY_LEFT && menudbox_Ypos == -240){
						if (!titleboxXmoveright) {
							titleboxXmoveleft = true;
						}
					/* } else if((hDown & KEY_DOWN) && (bnriconisDSi[settings.ui.cursorPosition-settings.ui.pagenum*gamesPerPage]==true)) {
						bnriconframenum[settings.ui.cursorPosition-settings.ui.pagenum*gamesPerPage]++;
						if(bnriconframenum[settings.ui.cursorPosition-settings.ui.pagenum*gamesPerPage] == 8) {
							bnriconframenum[settings.ui.cursorPosition-settings.ui.pagenum*gamesPerPage] = 0;
						} */
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
					} else if ((hDown & KEY_SELECT) && showbubble) {
						// Switch to per-game settings.
						menudboxmode = DBOX_MODE_SETTINGS;
						if(!settings.twl.forwarder && settings.ui.cursorPosition >= 0 && settings.twl.romtype==0) {
							// Get SDK version to show
							bool isCia = false;
							char path[256];
							const char *rom_filename;
							if(matching_files.size() == 0){
								if (files.size() != 0) {
									rom_filename = files.at(settings.ui.cursorPosition).c_str();
								} else {
									rom_filename = " ";
								}
								snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.romfolder.c_str(), rom_filename);
							} else {
								rom_filename = matching_files.at(settings.ui.cursorPosition).c_str();
								snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.romfolder.c_str(), rom_filename);
							}									
							std::string fn = rom_filename;
							if(fn.substr(fn.find_last_of(".") + 1) == "cia") isCia = true;

							if(isCia) {
								SDKnumbertext = "";	// Clear number
							} else {
								FILE *f_nds_file = fopen(path, "rb");
								char game_TID[5];
								grabTID(f_nds_file, game_TID, false);
								game_TID[4] = 0;
								game_TID[3] = 0;
								SDKVersion = 0;
								if(strcmp(game_TID, "###") != 0) {
									SDKVersion = getSDKVersion(f_nds_file, rom_filename);
									if((SDKVersion > 0x1000000) && (SDKVersion < 0x2000000)) {
										SDKnumbertext = "SDK ver: 1";
									} else if((SDKVersion > 0x2000000) && (SDKVersion < 0x3000000)) {
										SDKnumbertext = "SDK ver: 2";
									} else if((SDKVersion > 0x3000000) && (SDKVersion < 0x4000000)) {
										SDKnumbertext = "SDK ver: 3";
									} else if((SDKVersion > 0x4000000) && (SDKVersion < 0x5000000)) {
										SDKnumbertext = "SDK ver: 4";
									} else if((SDKVersion > 0x5000000) && (SDKVersion < 0x6000000)) {
										SDKnumbertext = "SDK ver: 5 (TWLSDK)";
									} else {
										SDKnumbertext = "SDK ver: ?";
									}
								} else {
									SDKnumbertext = "";	// Clear number
								}
								fclose(f_nds_file);
							}
						} else {
							SDKVersion = 0;
						}
						if (!showdialogbox_menu) {
							if (settings.ui.cursorPosition >= 0 && menudbox_Ypos == -240) {
								if (settings.twl.forwarder) {
									if(matching_files.size() == 0){
										rom = fcfiles.at(settings.ui.cursorPosition).c_str();
									} else {
										rom = matching_files.at(settings.ui.cursorPosition).c_str();
									}
								} else {
									if (settings.twl.romtype == 0) {
										if(matching_files.size() == 0){
											rom = files.at(settings.ui.cursorPosition).c_str();
										} else {
											rom = matching_files.at(settings.ui.cursorPosition).c_str();
										}
									} else if (settings.twl.romtype == 1) {
										if(matching_files.size() == 0){
											rom = gbfiles.at(settings.ui.cursorPosition).c_str();
										} else {
											rom = matching_files.at(settings.ui.cursorPosition).c_str();
										}
									} else if (settings.twl.romtype == 2) {
										if(matching_files.size() == 0){
											rom = nesfiles.at(settings.ui.cursorPosition).c_str();
										} else {
											rom = matching_files.at(settings.ui.cursorPosition).c_str();
										}
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
							settings.ui.pagenum++;
							bannertextloaded = false;
							settings.ui.cursorPosition = 0+settings.ui.pagenum*gamesPerPage;
							storedcursorPosition = settings.ui.cursorPosition;
							scrollwindowXmovepos = 0;
							titleboxXmovepos = 0;
							boxartXmovepos = 0;
							bnricontexloaded = false;
							boxarttexloaded = false;
							boxartpage = 0;
							loadboxartnum = 0+settings.ui.pagenum*gamesPerPage;
							loadbnriconnum = 0+settings.ui.pagenum*gamesPerPage;
							for (int i = 0; i < gamesPerPage; i++) {
								// Reset banner icon frames
								bnriconPalLine[i] = 0;
								bnriconframenumY[i] = 0;
								bannerFlip[i] = NONE;
								clearBannerSequence(i);
							}
							if (dspfirmfound) {
								sfx_switch->stop();	// Prevent freezing
								sfx_switch->play();
							}
						}
					} else if (menuaction_prevpage) {
						// Don't run the action again 'til L is pressed again
						menuaction_prevpage = false;
						if ((size_t)settings.ui.pagenum != 0 && file_count <= (size_t)0-settings.ui.pagenum*gamesPerPage) {
							settings.ui.pagenum--;
							bannertextloaded = false;
							settings.ui.cursorPosition = 0+settings.ui.pagenum*gamesPerPage;
							storedcursorPosition = settings.ui.cursorPosition;
							scrollwindowXmovepos = 0;
							titleboxXmovepos = 0;
							boxartXmovepos = 0;
							// noromsfound = false;
							bnricontexloaded = false;
							boxarttexloaded = false;
							boxartpage = 0;
							loadboxartnum = 0+settings.ui.pagenum*gamesPerPage;
							loadbnriconnum = 0+settings.ui.pagenum*gamesPerPage;
							for (int i = 0; i < gamesPerPage; i++) {
								// Reset banner icon frames
								bnriconPalLine[i] = 0;
								bnriconframenumY[i] = 0;
								bannerFlip[i] = NONE;
								clearBannerSequence(i);
							}
							if (dspfirmfound) {
								sfx_switch->stop();	// Prevent freezing
								sfx_switch->play();
							}
						}
					} else if (menuaction_launch) { menuaction_launch = false;	// Don't run the action again 'til A is pressed again
						bool isCia = false;
						bool overlaysIncluded = false;
						bool donorFound = true;
						if(!settings.twl.forwarder && settings.ui.cursorPosition >= 0 && settings.twl.romtype == 0) {
							char path[256];
							const char *rom_filename;
							if(matching_files.size() == 0){
								if (files.size() != 0) {
									rom_filename = files.at(settings.ui.cursorPosition).c_str();
								} else {
									rom_filename = " ";
								}
								snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.romfolder.c_str(), rom_filename);
							} else {
								rom_filename = matching_files.at(settings.ui.cursorPosition).c_str();
								snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.romfolder.c_str(), rom_filename);
							}									
							std::string fn = rom_filename;
							if(fn.substr(fn.find_last_of(".") + 1) == "cia") isCia = true;

							if(isCia) {
								FILE *f_nds_file = fopen(path, "rb");
								if (f_nds_file) {
									overlaysIncluded = getOverlaySize(f_nds_file, rom_filename, isCia);
									fclose(f_nds_file);
								}
							} else {
								donorpath = bootstrapini.GetString(bootstrapini_ndsbootstrap, bootstrapini_arm7donorpath, "");
								// Show "Donor ROM not set" message, if game is SDK3-4, but not MKDS, and donor path is blank
								if (donorpath.compare("") == 0) {
									FILE *f_nds_file = fopen(path, "rb");
									char game_TID[5];
									grabTID(f_nds_file, game_TID, false);
									game_TID[4] = 0;
									game_TID[3] = 0;
									SDKVersion = 0;
									if(strcmp(game_TID, "###") != 0) {
										SDKVersion = getSDKVersion(f_nds_file, rom_filename);
										if((SDKVersion > 0x3000000) && (SDKVersion < 0x5000000) && (strcmp(game_TID, "AMC") != 0)) {
											donorFound = false;
										}
									}
									fclose(f_nds_file);
								}
							}
						} else {
							SDKVersion = 0;
						}

						if(!donorFound) {
							if (!playwrongsounddone) {
								if (dspfirmfound) {
									sfx_wrong->stop();
									sfx_wrong->play();
								}
								playwrongsounddone = true;
							}
							showdialogbox_menu = true;
							menu_ctrlset = CTRL_SET_DBOX;
							menudboxmode = DBOX_MODE_DONOR_NOT_SET;
						} else if(!overlaysIncluded) {
							if (settings.ui.theme != THEME_3DSMENU) showbubble = false;
							if (!isDemo || settings.ui.cursorPosition == -2) {
								bool playlaunchsound = true;
								if (titleboxXmovetimer == 0) {
									if(settings.ui.cursorPosition == -2) {
										titleboxXmovetimer = 1;
										screenmodeswitch = true;
										applaunchprep = true;
									} else if(settings.ui.cursorPosition == -1) {
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
												rom = fcfiles.at(settings.ui.cursorPosition).c_str();
											} else {
												rom = matching_files.at(settings.ui.cursorPosition).c_str();
											}
										} else {
											settings.twl.launchslot1 = false;
											if (settings.twl.romtype == 0) {
												if(matching_files.size() == 0){
													rom = files.at(settings.ui.cursorPosition).c_str();
												} else {
													rom = matching_files.at(settings.ui.cursorPosition).c_str();
												}
												sav = ReplaceAll(rom, ".nds", ".sav");
											} else if (settings.twl.romtype == 1) {
												if(matching_files.size() == 0){
													homebrew_arg = gbfiles.at(settings.ui.cursorPosition).c_str();
												} else {
													homebrew_arg = matching_files.at(settings.ui.cursorPosition).c_str();
												}
											} else if (settings.twl.romtype == 2) {
												if(matching_files.size() == 0){
													homebrew_arg = nesfiles.at(settings.ui.cursorPosition).c_str();
												} else {
													homebrew_arg = matching_files.at(settings.ui.cursorPosition).c_str();
												}
											}
										}
										applaunchprep = true;
										if(isCia) launchCia = true;
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
						} else {
							if (!playwrongsounddone) {
								if (dspfirmfound) {
									sfx_wrong->stop();
									sfx_wrong->play();
								}
								playwrongsounddone = true;
							}
							showdialogbox_menu = true;
							menu_ctrlset = CTRL_SET_DBOX;
							menudboxmode = DBOX_MODE_OVERLAYS;
						}
					}
					
				} else if(menu_ctrlset == CTRL_SET_DBOX) {
					hidTouchRead(&touch);
					
					if (menudboxmode == DBOX_MODE_DELETED
					|| menudboxmode == DBOX_MODE_OVERLAYS
					|| menudboxmode == DBOX_MODE_DONOR_NOT_SET) {
						if (hDown & (KEY_A | KEY_B)) {
							showdialogbox_menu = false;
							menudbox_movespeed = 1;
							menu_ctrlset = CTRL_SET_GAMESEL;
						}
					} else if (menudboxmode == DBOX_MODE_DELETE) {
						if (hDown & KEY_A) {
							buttondeletegame = true;
							menudboxmode = DBOX_MODE_DELETED;
						} else if (hDown & KEY_B) {
							showdialogbox_menu = false;
							menudbox_movespeed = 1;
							menu_ctrlset = CTRL_SET_GAMESEL;
						}
					} else if (menudboxmode == DBOX_MODE_OPTIONS) {
						if ((hDown & KEY_SELECT) && showbubble && settings.ui.cursorPosition >= 0) {
							// Switch to game-specific settings.
							menudboxmode = DBOX_MODE_SETTINGS;
							bool isCia = false;
							if(!settings.twl.forwarder && settings.twl.romtype==0) {
								// Get SDK version to show
								char path[256];
								const char *rom_filename;
								if(matching_files.size() == 0){
									if (files.size() != 0) {
										rom_filename = files.at(settings.ui.cursorPosition).c_str();
									} else {
										rom_filename = " ";
									}
									snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.romfolder.c_str(), rom_filename);
								} else {
									rom_filename = matching_files.at(settings.ui.cursorPosition).c_str();
									snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.romfolder.c_str(), rom_filename);
								}									
								std::string fn = rom_filename;
								if(fn.substr(fn.find_last_of(".") + 1) == "cia") isCia = true;

								if(isCia) {
									SDKnumbertext = "";	// Clear number
								} else {
									FILE *f_nds_file = fopen(path, "rb");
									char game_TID[5];
									grabTID(f_nds_file, game_TID, false);
									game_TID[4] = 0;
									game_TID[3] = 0;
									SDKVersion = 0;
									if(strcmp(game_TID, "###") != 0) {
										SDKVersion = getSDKVersion(f_nds_file, rom_filename);
										if((SDKVersion > 0x1000000) && (SDKVersion < 0x2000000)) {
											SDKnumbertext = "SDK ver: 1";
										} else if((SDKVersion > 0x2000000) && (SDKVersion < 0x3000000)) {
											SDKnumbertext = "SDK ver: 2";
										} else if((SDKVersion > 0x3000000) && (SDKVersion < 0x4000000)) {
											SDKnumbertext = "SDK ver: 3";
										} else if((SDKVersion > 0x4000000) && (SDKVersion < 0x5000000)) {
											SDKnumbertext = "SDK ver: 4";
										} else if((SDKVersion > 0x5000000) && (SDKVersion < 0x6000000)) {
											SDKnumbertext = "SDK ver: 5 (TWLSDK)";
										} else {
											SDKnumbertext = "SDK ver: ?";
										}
									} else {
										SDKnumbertext = "";	// Clear number
									}
									fclose(f_nds_file);
								}
							} else {
								SDKVersion = 0;
							}
						} else if (hDown & KEY_RIGHT) {
							if (startmenu_cursorPosition % 2 != 1 &&
							    startmenu_cursorPosition != 6)
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
							if (startmenu_cursorPosition < 5) {
								startmenu_cursorPosition += 2;
							}
	
						} else if (hDown & KEY_UP) {
							if (startmenu_cursorPosition > 1) {
								startmenu_cursorPosition -= 2;
							}
						} else if(hDown & KEY_TOUCH){
							if (touch.px >= 23 && touch.px <= 155 && touch.py >= (menudbox_Ypos + 31) && touch.py <= (menudbox_Ypos + 65)) { // Game location button
								startmenu_cursorPosition = 0;
								if (settings.twl.romtype == 0) {
									menudboxaction_switchloc = true;
								} else {
									if (!playwrongsounddone) {
										if (dspfirmfound) {
											sfx_wrong->stop();
											sfx_wrong->play();
										}
										playwrongsounddone = true;
									}
								}
							}else if (touch.px >= 161 && touch.px <= 293 && touch.py >= (menudbox_Ypos + 31) && touch.py <= (menudbox_Ypos + 65)){ // Box art button
								startmenu_cursorPosition = 1;
								settings.romselect.toplayout = !settings.romselect.toplayout;
								if (dspfirmfound) {
									sfx_switch->stop();	// Prevent freezing
									sfx_switch->play();
								}
							}else if (touch.px >= 23 && touch.px <= 155 && touch.py >= (menudbox_Ypos + 71) && touch.py <= (menudbox_Ypos + 105)){ // Start GBARunner2 button
								startmenu_cursorPosition = 2;
								menudboxmode = DBOX_MODE_ROMTYPE;
								setromtype_cursorPosition = settings.twl.romtype;
								if(settings.twl.romtype == 1) setromtype_cursorPosition = 2;
								if(settings.twl.romtype == 2) setromtype_cursorPosition = 3;
							}else if (touch.px >= 161 && touch.px <= 293 && touch.py >= (menudbox_Ypos + 71) && touch.py <= (menudbox_Ypos + 105)){ // Top border button
								startmenu_cursorPosition = 3;
								settings.ui.topborder = !settings.ui.topborder;
							}else if (touch.px >= 23 && touch.px <= 155 && touch.py >= (menudbox_Ypos + 111) && touch.py <= (menudbox_Ypos + 145)){ // Unset donor ROM button
								startmenu_cursorPosition = 4;
								bootstrapini.SetString(bootstrapini_ndsbootstrap, bootstrapini_arm7donorpath, "");
								bootstrapini.SaveIniFile("sdmc:/_nds/nds-bootstrap.ini");
								showdialogbox_menu = false;
								menudbox_movespeed = 1;
								menu_ctrlset = CTRL_SET_GAMESEL;
							}else if (touch.px >= 161 && touch.px <= 293 && touch.py >= (menudbox_Ypos + 111) && touch.py <= (menudbox_Ypos + 145)){ // Search button
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
									// Prepare some stuff to show correctly the filtered roms
									settings.ui.pagenum = 0; // Go to page 0
									settings.ui.cursorPosition = 0; // Move the cursor to 0
									storedcursorPosition = settings.ui.cursorPosition; // Move the cursor to 0
									scrollwindowXmovepos = 0; // Move the cursor to 0
									titleboxXmovepos = 0; // Move the cursor to 0
									boxartXmovepos = 0; // Move the cursor to 0
									snprintf(romsel_counter2sd, sizeof(romsel_counter2sd), "%zu", matching_files.size()); // Reload counter
									boxarttexloaded = false; // Reload boxarts
									bnricontexloaded = false; // Reload banner icons
									colortexloaded = false; // Reload top textures
									colortexloaded_bot = false; // Reload bottom textures
									boxartpage = 0;
									loadboxartnum = 0+settings.ui.pagenum*gamesPerPage;
									loadbnriconnum = 0+settings.ui.pagenum*gamesPerPage;
								}
								pp2d_draw_texture(dboxtex_button, 161, menudbox_Ypos + 111); // Light the button to print it always
								showdialogbox_menu = false;
								menudbox_movespeed = 1;
								menu_ctrlset = CTRL_SET_GAMESEL;
							}else if (touch.px >= 23 && touch.px <= 155 && touch.py >= (menudbox_Ypos + 151) && touch.py <= (menudbox_Ypos + 185)){ // Settings button
								startmenu_cursorPosition = 6; // Only this is making sometimes to not show the light texture
								pp2d_draw_texture(dboxtex_button, 23, menudbox_Ypos + 151); // Light the button to print it always
								fadeout = true;
								screenmodeswitch = true;
								if (dspfirmfound) {
									bgm_menu->stop();
									sfx_launch->play();
								}
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
									if (settings.twl.romtype == 0) {
										menudboxaction_switchloc = true;
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
								case 1:
									settings.romselect.toplayout = !settings.romselect.toplayout;
									if (dspfirmfound) {
										sfx_switch->stop();	// Prevent freezing
										sfx_switch->play();
									}
									break;
								case 2:
									menudboxmode = DBOX_MODE_ROMTYPE;
									setromtype_cursorPosition = settings.twl.romtype;
									if(settings.twl.romtype == 1) setromtype_cursorPosition = 2;
									if(settings.twl.romtype == 2) setromtype_cursorPosition = 3;
									break;
								case 3:
									settings.ui.topborder = !settings.ui.topborder;
									break;
								case 4:
									// Unset donor ROM path
									bootstrapini.SetString(bootstrapini_ndsbootstrap, bootstrapini_arm7donorpath, "");
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
										// Prepare some stuff to show correctly the filtered roms
										settings.ui.pagenum = 0; // Go to page 0
										settings.ui.cursorPosition = 0; // Move the cursor to 0
										storedcursorPosition = settings.ui.cursorPosition; // Move the cursor to 0
										scrollwindowXmovepos = 0; // Move the cursor to 0
										titleboxXmovepos = 0; // Move the cursor to 0
										boxartXmovepos = 0; // Move the cursor to 0
										snprintf(romsel_counter2sd, sizeof(romsel_counter2sd), "%zu", matching_files.size()); // Reload counter
										boxarttexloaded = false; // Reload boxarts
										bnricontexloaded = false; // Reload banner icons
										colortexloaded = false; // Reload top textures
										colortexloaded_bot = false; // Reload bottom textures
										boxartpage = 0;
										loadboxartnum = 0+settings.ui.pagenum*gamesPerPage;
										loadbnriconnum = 0+settings.ui.pagenum*gamesPerPage;
									}
									showdialogbox_menu = false;
									menudbox_movespeed = 1;
									menu_ctrlset = CTRL_SET_GAMESEL;
									break;
								}
								case 6:
									fadeout = true;
									screenmodeswitch = true;
									if (dspfirmfound) {
										bgm_menu->stop();
										sfx_launch->play();
									}
									showdialogbox_menu = false;
									menudbox_movespeed = 1;
									menu_ctrlset = CTRL_SET_GAMESEL;
									break;
							}
						} else if (hDown & (KEY_B | KEY_START)) {
							showdialogbox_menu = false;
							menudbox_movespeed = 1;
							menu_ctrlset = CTRL_SET_GAMESEL;
						}
					} else if (menudboxmode == DBOX_MODE_ROMTYPE) {
						if (hDown & KEY_RIGHT) {
							if (setromtype_cursorPosition % 2 != 1 &&
							    setromtype_cursorPosition != 3)
							{
								// Move right.
								setromtype_cursorPosition++;
							}
						} else if (hDown & KEY_LEFT) {
							if (setromtype_cursorPosition % 2 != 0) {
								// Move left.
								setromtype_cursorPosition--;
							}
						} else if (hDown & KEY_DOWN) {
							if (setromtype_cursorPosition < 2) {
								setromtype_cursorPosition += 2;
							}
	
						} else if (hDown & KEY_UP) {
							if (setromtype_cursorPosition > 1) {
								setromtype_cursorPosition -= 2;
							}
						} else if(hDown & KEY_TOUCH){
							if (touch.px >= 23 && touch.px <= 155 && touch.py >= (menudbox_Ypos + 31) && touch.py <= (menudbox_Ypos + 65)) { // Nintendo DSi/DSi button
								setromtype_cursorPosition = 0;
								settings.twl.romtype = 0;
								menudboxaction_changeromtype = true;
								showdialogbox_menu = false;
								menudbox_movespeed = 1;
								menu_ctrlset = CTRL_SET_GAMESEL;
							}else if (touch.px >= 161 && touch.px <= 293 && touch.py >= (menudbox_Ypos + 31) && touch.py <= (menudbox_Ypos + 65)){ // GameBoy Advance button
								setromtype_cursorPosition = 1;
								if (!isDemo) {
									gbarunnervalue = 1;
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
								showdialogbox_menu = false;
								menudbox_movespeed = 1;
								menu_ctrlset = CTRL_SET_GAMESEL;
							}else if (touch.px >= 23 && touch.px <= 155 && touch.py >= (menudbox_Ypos + 71) && touch.py <= (menudbox_Ypos + 105)){ // GameBoy/Color button
								setromtype_cursorPosition = 2;
								settings.twl.romtype = 1;
								menudboxaction_changeromtype = true;
								showdialogbox_menu = false;
								menudbox_movespeed = 1;
								menu_ctrlset = CTRL_SET_GAMESEL;
							}else if (touch.px >= 161 && touch.px <= 293 && touch.py >= (menudbox_Ypos + 71) && touch.py <= (menudbox_Ypos + 105)){ // NES button
								setromtype_cursorPosition = 3;
								settings.twl.romtype = 2;
								menudboxaction_changeromtype = true;
								showdialogbox_menu = false;
								menudbox_movespeed = 1;
								menu_ctrlset = CTRL_SET_GAMESEL;
							}else if (touch.px >= 233 && touch.px <= 299 && touch.py >= (menudbox_Ypos + 191) && touch.py <= (menudbox_Ypos + 217)){ // Back button
								menudboxmode = DBOX_MODE_OPTIONS;
							}
						} else if (hDown & KEY_A) {
							switch (setromtype_cursorPosition) {
								case 0:
								default:
									settings.twl.romtype = 0;
									menudboxaction_changeromtype = true;
									break;
								case 1:
									if (!isDemo) {
										gbarunnervalue = 1;
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
								case 2:
									settings.twl.romtype = 1;
									menudboxaction_changeromtype = true;
									break;
								case 3:
									settings.twl.romtype = 2;
									menudboxaction_changeromtype = true;
									break;
							}
							showdialogbox_menu = false;
							menudbox_movespeed = 1;
							menu_ctrlset = CTRL_SET_GAMESEL;
						} else if (hDown & (KEY_B | KEY_START)) {
							menudboxmode = DBOX_MODE_OPTIONS;
						}
					} else if (menudboxmode == DBOX_MODE_SETTINGS) {
						hidTouchRead(&touch);
						
						if (hDown & KEY_START) {
							// Switch to the "Start" menu.
							if (settings.twl.forwarder) {
								if(matching_files.size() == 0){
									rom = fcfiles.at(settings.ui.cursorPosition).c_str();
								}else{
									rom = matching_files.at(settings.ui.cursorPosition).c_str();
								}
							} else if (settings.twl.romtype == 0) {
								if(matching_files.size() == 0){
									rom = files.at(settings.ui.cursorPosition).c_str();
								}else{
									rom = matching_files.at(settings.ui.cursorPosition).c_str();
								}
							} else if (settings.twl.romtype == 1) {
								if(matching_files.size() == 0){
									rom = gbfiles.at(settings.ui.cursorPosition).c_str();
								}else{
									rom = matching_files.at(settings.ui.cursorPosition).c_str();
								}
							} else if (settings.twl.romtype == 2) {
								if(matching_files.size() == 0){
									rom = nesfiles.at(settings.ui.cursorPosition).c_str();
								}else{
									rom = matching_files.at(settings.ui.cursorPosition).c_str();
								}
							}
							SavePerGameSettings();
							menudboxmode = DBOX_MODE_OPTIONS;
						} else if (hDown & KEY_RIGHT) {
							if (gamesettings_cursorPosition == 1) {
								gamesettings_cursorPosition = 2;
							}
						} else if (hDown & KEY_LEFT) {
							if (gamesettings_cursorPosition == 2) {
								gamesettings_cursorPosition = 1;
							}
						} else if (hDown & KEY_DOWN) {
							if((SDKVersion > 0x5000000 && SDKVersion < 0x6000000) || settings.twl.forwarder) {
								gamesettings_cursorPosition = 3;
							} else {
								if (gamesettings_cursorPosition == 0) {
									gamesettings_cursorPosition = 1;
								} else if (gamesettings_cursorPosition == 1 || gamesettings_cursorPosition == 2) {
									gamesettings_cursorPosition = 3;
								} else if (gamesettings_cursorPosition == 3) {
									gamesettings_cursorPosition = 0;
								}
							}
						} else if (hDown & KEY_UP) {
							if((SDKVersion > 0x5000000 && SDKVersion < 0x6000000) || settings.twl.forwarder) {
								gamesettings_cursorPosition = 0;
							} else {
								if (gamesettings_cursorPosition == 0) {
									gamesettings_cursorPosition = 3;
								} else if (gamesettings_cursorPosition == 1 || gamesettings_cursorPosition == 2) {
									gamesettings_cursorPosition = 0;
								} else if (gamesettings_cursorPosition == 3) {
									gamesettings_cursorPosition = 1;
								}
							}
						} else if(hDown & KEY_TOUCH){
							if(gamesettings_isCia) {
								if (touch.px >= 23 && touch.px <= 155 && touch.py >= (menudbox_Ypos + 129) && touch.py <= (menudbox_Ypos + 163)){ // Set LED Color
									gamesettings_cursorPosition = 3;								

									RGB[0] = keyboardInputInt("Red color: max is 255");
									RGB[1] = keyboardInputInt("Green color: max is 255");
									RGB[2] = keyboardInputInt("Blue color: max is 255");
									
									settings.pergame.red = RGB[0];
									settings.pergame.green = RGB[1];
									settings.pergame.blue = RGB[2];
								}
							} else {
								if (touch.px >= 23 && touch.px <= 155 && touch.py >= (menudbox_Ypos + 89) && touch.py <= (menudbox_Ypos + 123)) { // ARM9 CPU Speed
									gamesettings_cursorPosition = 0;
									settings.pergame.cpuspeed++;
									if(settings.pergame.cpuspeed == 2)
										settings.pergame.cpuspeed = -1;
									if (dspfirmfound) {
										sfx_select->stop();	// Prevent freezing
										sfx_select->play();
									}
								}
								if((SDKVersion > 0x5000000 && SDKVersion < 0x6000000) || settings.twl.forwarder) {} else {
									if (touch.px >= 23 && touch.px <= 155 && touch.py >= (menudbox_Ypos + 129) && touch.py <= (menudbox_Ypos + 163)){ // Use set donor ROM
										gamesettings_cursorPosition = 1;
										settings.pergame.usedonor++;
										if(settings.pergame.usedonor == 3)
											settings.pergame.usedonor = 0;
										if (dspfirmfound) {
											sfx_select->stop();	// Prevent freezing
											sfx_select->play();
										}
									}else if (touch.px >= 161 && touch.px <= 293 && touch.py >= (menudbox_Ypos + 129) && touch.py <= (menudbox_Ypos + 163)){ // Set as donor ROM
										gamesettings_cursorPosition = 2;
										if(matching_files.size() == 0){
											rom = files.at(settings.ui.cursorPosition).c_str();
										}else{
											rom = matching_files.at(settings.ui.cursorPosition).c_str();
										}
										bootstrapini.SetString(bootstrapini_ndsbootstrap, bootstrapini_arm7donorpath, fat+settings.ui.romfolder+slashchar+rom);
										bootstrapini.SaveIniFile("sdmc:/_nds/nds-bootstrap.ini");
										showdialogbox_menu = false;
										menudbox_movespeed = 1;
										menu_ctrlset = CTRL_SET_GAMESEL;
									}
								}
								if (touch.px >= 23 && touch.px <= 155 && touch.py >= (menudbox_Ypos + 169) && touch.py <= (menudbox_Ypos + 203)){ // Set LED Color
									gamesettings_cursorPosition = 3;								

									RGB[0] = keyboardInputInt("Red color: max is 255");
									RGB[1] = keyboardInputInt("Green color: max is 255");
									RGB[2] = keyboardInputInt("Blue color: max is 255");
									
									settings.pergame.red = RGB[0];
									settings.pergame.green = RGB[1];
									settings.pergame.blue = RGB[2];
								}
							}
							if (touch.px >= 233 && touch.px <= 299 && touch.py >= (menudbox_Ypos + 191) && touch.py <= (menudbox_Ypos + 217)){ // Back button
								if (settings.twl.forwarder) {
									if(matching_files.size() == 0){
										rom = fcfiles.at(settings.ui.cursorPosition).c_str();
									} else {
										rom = matching_files.at(settings.ui.cursorPosition).c_str();
									}
								} else {
									if(matching_files.size() == 0){
										rom = files.at(settings.ui.cursorPosition).c_str();
									}else{
										rom = matching_files.at(settings.ui.cursorPosition).c_str();
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
									settings.pergame.usedonor++;
									if(settings.pergame.usedonor == 3)
										settings.pergame.usedonor = 0;
									if (dspfirmfound) {
										sfx_select->stop();	// Prevent freezing
										sfx_select->play();
									}
									break;
								case 2:
									if (settings.twl.forwarder) {
										if(matching_files.size() == 0){
											rom = fcfiles.at(settings.ui.cursorPosition).c_str();
										}else{
											rom = matching_files.at(settings.ui.cursorPosition).c_str();
										}										
									} else {
										if(matching_files.size() == 0){
											rom = files.at(settings.ui.cursorPosition).c_str();
										}else{
											rom = matching_files.at(settings.ui.cursorPosition).c_str();
										}
										bootstrapini.SetString(bootstrapini_ndsbootstrap, bootstrapini_arm7donorpath, fat+settings.ui.romfolder+slashchar+rom);
										bootstrapini.SaveIniFile("sdmc:/_nds/nds-bootstrap.ini");
									}
									showdialogbox_menu = false;
									menudbox_movespeed = 1;
									menu_ctrlset = CTRL_SET_GAMESEL;
									break;
								case 3:
									RGB[0] = keyboardInputInt("Red color: max is 255");
									RGB[1] = keyboardInputInt("Green color: max is 255");
									RGB[2] = keyboardInputInt("Blue color: max is 255");
									
									settings.pergame.red = RGB[0];
									settings.pergame.green = RGB[1];
									settings.pergame.blue = RGB[2];

									if (dspfirmfound) {
										sfx_wrong->stop();	// Prevent freezing
										sfx_wrong->play();
									}
									break;
							}
						} else if (hDown & (KEY_B | KEY_SELECT)) {
							if (settings.twl.forwarder) {
								if(matching_files.size() == 0){
									rom = fcfiles.at(settings.ui.cursorPosition).c_str();
								}else{
									rom = matching_files.at(settings.ui.cursorPosition).c_str();
								}
							} else if (settings.twl.romtype == 0) {
								if(matching_files.size() == 0){
									rom = files.at(settings.ui.cursorPosition).c_str();
								}else{
									rom = matching_files.at(settings.ui.cursorPosition).c_str();
								}
							} else if (settings.twl.romtype == 1) {
								if(matching_files.size() == 0){
									rom = gbfiles.at(settings.ui.cursorPosition).c_str();
								}else{
									rom = matching_files.at(settings.ui.cursorPosition).c_str();
								}
							} else if (settings.twl.romtype == 2) {
								if(matching_files.size() == 0){
									rom = nesfiles.at(settings.ui.cursorPosition).c_str();
								}else{
									rom = matching_files.at(settings.ui.cursorPosition).c_str();
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
						if (matching_files.size() != 0) matching_files.clear();
						if (delete_queue.size() != 0) delete_queue.clear();
						settings.ui.pagenum = 0;
						bannertextloaded = false;
						settings.ui.cursorPosition = 0;
						storedcursorPosition = settings.ui.cursorPosition;
						scrollwindowXmovepos = 0;
						titleboxXmovepos = 0;
						boxartXmovepos = 0;
						noromsfound = false;
						settings.twl.forwarder = !settings.twl.forwarder;
						bnricontexloaded = false;
						boxarttexloaded = false;
						boxartpage = 0;
						loadboxartnum = 0+settings.ui.pagenum*gamesPerPage;
						loadbnriconnum = 0+settings.ui.pagenum*gamesPerPage;
						for (int i = 0; i < gamesPerPage; i++) {
							// Reset banner icon frames
							bnriconPalLine[i] = 0;
							bnriconframenumY[i] = 0;
							bannerFlip[i] = NONE;
							clearBannerSequence(i);
						}
						if (dspfirmfound) {
							sfx_switch->stop();	// Prevent freezing
							sfx_switch->play();
						}
						
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

					if (menudboxaction_changeromtype) { menudboxaction_changeromtype = false;
						if (dspfirmfound) {
							sfx_switch->stop();	// Prevent freezing
							sfx_switch->play();
						}
						if (matching_files.size() != 0) matching_files.clear();
						if (delete_queue.size() != 0) delete_queue.clear();
						settings.ui.pagenum = 0;
						bannertextloaded = false;
						settings.ui.cursorPosition = 0;
						storedcursorPosition = settings.ui.cursorPosition;
						scrollwindowXmovepos = 0;
						titleboxXmovepos = 0;
						boxartXmovepos = 0;
						noromsfound = false;
						settings.twl.forwarder = false;
						bnricontexloaded = false;
						boxarttexloaded = false;
						boxartpage = 0;
						loadboxartnum = 0+settings.ui.pagenum*gamesPerPage;
						loadbnriconnum = 0+settings.ui.pagenum*gamesPerPage;
						for (int i = 0; i < gamesPerPage; i++) {
							// Reset banner icon frames
							bnriconPalLine[i] = 0;
							bnriconframenumY[i] = 0;
							bannerFlip[i] = NONE;
							clearBannerSequence(i);
						}
						if (dspfirmfound) {
							sfx_switch->stop();	// Prevent freezing
							sfx_switch->play();
						}
						
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

					if(buttondeletegame) {
						// Delete game mode.
						buttondeletegame = false; // Prevent deleting by accident.
						
						if(delete_queue.size() != 0) {
							vector<string> delete_queue_copy = delete_queue; // Copy the vector 'cause we cannot delete while iterating it.
							
							std::string rom_iter;
							if (settings.twl.forwarder) {
								for (auto iter = delete_queue_copy.cbegin(); iter != delete_queue_copy.cend(); ++iter) {
									
									if(matching_files.size() == 0){
										rom_iter = fcfiles.at(settings.ui.cursorPosition);
									} else {
										rom_iter = matching_files.at(settings.ui.cursorPosition);
									}
									
									if (strcasestr(iter->c_str(), rom_iter.c_str()) != NULL) {
										// ROM found.
										deletemode_internal(ROM_FLASHCARD, rom_iter);										
										delete_queue.erase(iter);
										if (logEnabled)	LogFMA("Delete mode", "Rom deleted from queue", rom_iter.c_str());
									}
								}

							} else {				
								for (auto iter = delete_queue_copy.cbegin(); iter != delete_queue_copy.cend(); ++iter) {
									
									if(matching_files.size() == 0){
										rom_iter = files.at(settings.ui.cursorPosition);
									} else {
										rom_iter = matching_files.at(settings.ui.cursorPosition);
									}
									
									if (strcasestr(iter->c_str(), rom_iter.c_str()) != NULL) {
										// ROM found.										
										deletemode_internal(ROM_SD, rom_iter);										
										delete_queue.erase(iter);
										if (logEnabled)	LogFMA("Delete mode", "Rom deleted from queue", rom_iter.c_str());
									}
								}
							}

							if(delete_queue.size() != 0) {
								if (logEnabled)	LogFM("Delete mode", "Error detected. Cannot clear vector");
							} else {
								delete_queue.clear();
								delete_queue_copy.clear();
							}

						} else {
							// We want to delete only one game
							
							std::string del_rom;
							if (settings.twl.forwarder) {
								if(matching_files.size() == 0){
									del_rom = fcfiles.at(settings.ui.cursorPosition).c_str();
								} else {
									del_rom = matching_files.at(settings.ui.cursorPosition).c_str();
								}
								deletemode_internal(ROM_FLASHCARD, del_rom);
							} else {
								if(matching_files.size() == 0){
									del_rom = files.at(settings.ui.cursorPosition);
								} else {
									del_rom = matching_files.at(settings.ui.cursorPosition);
								}
								deletemode_internal(ROM_SD, del_rom);
							}
						}
						snprintf(romsel_counter2fc, sizeof(romsel_counter2fc), "%zu", fcfiles.size());
						snprintf(romsel_counter2sd, sizeof(romsel_counter2sd), "%zu", files.size());

						settings.ui.pagenum = 0; // Go to page 0
						settings.ui.cursorPosition = 0; // Move the cursor to 0
						storedcursorPosition = settings.ui.cursorPosition; // Move the cursor to 0
						scrollwindowXmovepos = 0; // Move the cursor to 0
						titleboxXmovepos = 0; // Move the cursor to 0
						boxartXmovepos = 0; // Move the cursor to 0
						boxarttexloaded = false; // Reload boxarts
						bnricontexloaded = false; // Reload banner icons
						colortexloaded = false; // Reload top textures
						colortexloaded_bot = false; // Reload bottom textures
						boxartpage = 0;
						loadboxartnum = 0+settings.ui.pagenum*gamesPerPage;
						loadbnriconnum = 0+settings.ui.pagenum*gamesPerPage;
						for (int i = 0; i < gamesPerPage; i++) {
							// Reset banner icon frames
							bnriconPalLine[i] = 0;
							bnriconframenumY[i] = 0;
							bannerFlip[i] = NONE;
							clearBannerSequence(i);
						}
						bannertextloaded = false; // Reload banner text after deletion
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
							fcrompathini.SetString("YSMENU", "DEFAULT_RESET", "true");
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
				rom = "GBARunner2.nds";
				if (settings.twl.forwarder) {
					switch (settings.twl.flashcard) {
						case 0:
						case 1:
						case 3:
						default: {
							CIniFile fcrompathini("sdmc:/_nds/YSMenu.ini");
							fcrompathini.SetString("YSMENU", "AUTO_BOOT", slashchar+rom);
							fcrompathini.SetString("YSMENU", "DEFAULT_DMA", "true");
							fcrompathini.SetString("YSMENU", "DEFAULT_RESET", "true");
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
			u64 tid;
			if (settings.twl.romtype > 0 && isTWLNAND1Installed) {
				tid = TWLNANDTWLTOUCH_TID;
			} else {
				tid = TWLNAND_TID;
			}
			if (settings.twl.forwarder) {
				CIniFile settingsini("sdmc:/_nds/twloader/settings.ini");
				if(settingsini.GetInt("TWL-MODE","FLASHCARD",0) == 0
				|| settingsini.GetInt("TWL-MODE","FLASHCARD",0) == 1
				|| settingsini.GetInt("TWL-MODE","FLASHCARD",0) == 4) {
				} else {
					tid = NTRLAUNCHER_TID;
				}
			} else if (settings.ui.cursorPosition == -1) {
				tid = NTRLAUNCHER_TID;
			}

			FS_MediaType mediaType = MEDIATYPE_NAND;
			bool switchToTwl = true;	

			if(launchCia) {
				char path[256];
				const char *rom_filename;
				if(matching_files.size() == 0){
					if (files.size() != 0) {
						rom_filename = files.at(settings.ui.cursorPosition).c_str();
					} else {
						rom_filename = " ";
					}
					snprintf(path, sizeof(path), "/%s/%s", settings.ui.romfolder.c_str(), rom_filename);
				} else {
					rom_filename = matching_files.at(settings.ui.cursorPosition).c_str();
					snprintf(path, sizeof(path), "/%s/%s", settings.ui.romfolder.c_str(), rom_filename);
				}

                tid = getTIDFromCIA(path);
				if (!checkDSiWareInstalled(tid)) InstallCIA(rom_filename);
			}

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
				if (usepergamesettings) {
					if (settings.ui.cursorPosition >= 0 && gbarunnervalue == 0) {
						if (settings.twl.romtype == 0) {
							if (settings.twl.forwarder) {
								if(matching_files.size() == 0){
									rom = fcfiles.at(settings.ui.cursorPosition).c_str();
								}else{
									rom = matching_files.at(settings.ui.cursorPosition).c_str();
								}
							} else {
								if(matching_files.size() == 0){
									rom = files.at(settings.ui.cursorPosition).c_str();
								} else {
									rom = matching_files.at(settings.ui.cursorPosition).c_str();
								}
							}
						} else if (settings.twl.romtype == 1) {
							if(matching_files.size() == 0){
								rom = gbfiles.at(settings.ui.cursorPosition).c_str();
							} else {
								rom = matching_files.at(settings.ui.cursorPosition).c_str();
							}
						} else if (settings.twl.romtype == 2) {
							if(matching_files.size() == 0){
								rom = nesfiles.at(settings.ui.cursorPosition).c_str();
							} else {
								rom = matching_files.at(settings.ui.cursorPosition).c_str();
							}
						}
						LoadPerGameSettings();
					}
				}
				if((RGB[0] <= 0) && (RGB[1] <= 0) && (RGB[2] <= 0)){
					// If RGB in pergame is 0 or less, use standard green or rainbowled patern
					if (settings.twl.rainbowled==1) {
						dsGreenLed();
					} else if (settings.twl.rainbowled==2) {
						RainbowLED();
					}
				}else{
					// Use pergame led
					PergameLed();
				}
			}

			if (settings.ui.showbootscreen == 2 && settings.twl.romtype == 0 && gbarunnervalue == 0 && switchToTwl) {
				bootSplash();
				if (logEnabled)	LogFM("Main.bootSplash", "Boot splash played");
				if(aptMainLoop()) fade_whiteToBlack();
			}

			if(aptMainLoop()) {
				while(1) {
					// Buffers for APT_DoApplicationJump().
					u8 param[0x300];
					u8 hmac[0x20];
					// Clear both buffers
					memset(param, 0, sizeof(param));
					memset(hmac, 0, sizeof(hmac));

					APT_PrepareToDoApplicationJump(0, tid, mediaType);
					// Tell APT to trigger the app launch and set the status of this app to exit
					APT_DoApplicationJump(param, sizeof(param), hmac);
				}
			}
		}
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

	if (settings.ui.theme <= THEME_3DSMENU) gamecardClearCache();

	// Free the arrays.
	if (boxarttexloaded) {
		for (int i = 0; i < gamesPerPage; i++) {
			free(boxartpath[i]);
		}
	}
	if (bnricontexloaded) {
		for (int i = 0; i < gamesPerPage; i++) {
			free(bnriconpath[i]);
		}
	}

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

	if (logEnabled) LogFM("Main", "Stoping pp2d...");
	pp2d_exit();
	
	screenon();
	if (logEnabled) LogFM("Main", "All services are closing and returning to HOME Menu");

	destroyThreads();
	acExit();
	hidExit();
	srvExit();
	romfsExit();
	aptExit();
	ptmuxExit();
	ptmuExit();
	amExit();
	cfguExit();
	sdmcExit();

	return 0;
}
