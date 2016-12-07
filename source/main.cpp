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

#include "sound.h"
#include "inifile.h"
#include "date.h"

#define CONFIG_3D_SLIDERSTATE (*(float *)0x1FF81080)


u32 kDown;
u32 kHeld;

CIniFile settingsini( "sdmc:/_nds/twloader/settings.ini" );	
CIniFile bootstrapini( "sdmc:/_nds/nds-bootstrap.ini" );

int color_Rvalue;
int color_Gvalue;
int color_Bvalue;
	
sf2d_texture *bnricontexnum;
sf2d_texture *boxarttexnum;
#include "bannerandboxart.h"
int bnriconnum = 0;
int boxartnum = 0;
const char* tempimagepath;
const char* bnriconfile;
const char* boxartfile;
const char* boxartfile_fullpath;
const char* topbgloc;
const char* startborderloc;


const char* fcrompathini_flashcardrom = "FLASHCARD-ROM";
const char* fcrompathini_rompath = "NDS_PATH";
const char* fcrompathini_bnrtext1 = "BNR_TEXT1";
const char* fcrompathini_bnrtext2 = "BNR_TEXT2";
const char* fcrompathini_bnrtext3 = "BNR_TEXT3";
	

// Settings .ini file
const char* settingsini_frontend = "FRONTEND";
//char* settingsini_frontend_twlappinstalled = "TWLAPP_INSTALLED";
const char* settingsini_frontend_color = "COLOR";
const char* settingsini_frontend_topborder = "TOP_BORDER";
const char* settingsini_frontend_toplayout = "TOP_LAYOUT";
//char* settingsini_frontend_botlayout = "BOTTOM_LAYOUT";
	
const char* settingsini_twlmode = "TWL-MODE";
const char* settingsini_twl_rainbowled = "RAINBOW_LED";
const char* settingsini_twl_clock = "TWL_CLOCK";
const char* settingsini_twl_bootani = "BOOT_ANIMATION";
const char* settingsini_twl_hsmsg = "HEALTH&SAFETY_MSG";
const char* settingsini_twl_launchslot1 = "LAUNCH_SLOT1";	// 0: Don't boot Slot-1, 1: Boot Slot-1, 2: Forward a ROM path to a Slot-1 flashcard.
const char* settingsini_twl_resetslot1 = "RESET_SLOT1";
const char* settingsini_twl_debug = "DEBUG";
const char* settingsini_twl_forwarder = "FORWARDER";
const char* settingsini_twl_flashcard = "FLASHCARD";
// End



// Bootstrap .ini file
const char* bootstrapini_ndsbootstrap = "NDS-BOOTSTRAP";
const char* bootstrapini_ndspath = "NDS_PATH";
const char* bootstrapini_boostcpu = "BOOST_CPU";
const char* bootstrapini_debug = "DEBUG";
const char* bootstrapini_lockarm9scfgext = "LOCK_ARM9_SCFG_EXT";
// End

const char* Lshouldertext;
const char* Rshouldertext;

const char* Lshouldertext_showboxart = "Show Box Art";
const char* Lshouldertext_hideboxart = "Hide Box Art";
const char* Rshouldertext_sdcard = "SD Card";
const char* Rshouldertext_flashcard = "Flashcard";

const char* noromtext1;
const char* noromtext2;

const char* batterytext;

// Settings text
const char* settings_vertext = "Ver. 1.5";

const char* settingstext_bot;

const char* settings_colortext = "Color";
const char* settings_topbordertext = "Top border";

const char* settings_colorvaluetext;
const char* settings_topbordervaluetext;

const char* settings_lrpicktext = "Left/Right: Pick";
const char* settings_absavereturn = "A/B: Save and Return";
// End

int settings_colorvalue;
int settings_topbordervalue;

int romselect_toplayout;
//	0: Show box art
//	1: Hide box art

std::string romsel_gameline1;
std::string romsel_gameline2;
std::string romsel_gameline3;
char *cstr1;
char *cstr2;
char *cstr3;

char* rom = (char*)malloc(256);
const char* flashcardrom;
std::string fat = "fat:/nds/";
std::string slashchar = "/";
std::string woodfat = "fat0:/";
std::string dstwofat = "fat1:/";
std::string flashcardfolder = "sdmc:/nds/flashcard/";
const char* fcbnriconfolder = "sdmc:/_nds/twloader/bnricons/flashcard/";
const char* boxartfolder = "sdmc:/_nds/twloader/boxart/";
const char* fcboxartfolder = "sdmc:/_nds/twloader/boxart/flashcard/";

// NTR/TWL-mode Settings text
const char* twlsettings_flashcardtext = "Flashcard(s) select";
const char* twlsettings_rainbowledtext = "Rainbow LED";
const char* twlsettings_cpuspeedtext = "ARM9 CPU Speed";
const char* twlsettings_bootscreentext = "DS/DSi Boot Screen";
const char* twlsettings_healthsafetytext = "Health and Safety message";
const char* twlsettings_resetslot1text = "Reset Slot-1";
const char* twlsettings_consoletext = "Console output";
const char* twlsettings_lockarm9scfgexttext = "Lock ARM9 SCFG_EXT";
	
const char* twlsettings_rainbowledvaluetext;
const char* twlsettings_cpuspeedvaluetext;
const char* twlsettings_flashcardvaluetext1;
const char* twlsettings_flashcardvaluetext2;
const char* twlsettings_flashcardvaluetext3;
const char* twlsettings_flashcardvaluetext4;
const char* twlsettings_flashcardvaluetext5;
const char* twlsettings_flashcardvaluetext6;
const char* twlsettings_bootscreenvaluetext;
const char* twlsettings_healthsafetyvaluetext;
const char* twlsettings_resetslot1valuetext;
const char* twlsettings_consolevaluetext;
const char* twlsettings_lockarm9scfgextvaluetext;
// End
	
int twlsettings_rainbowledvalue;
int twlsettings_cpuspeedvalue;
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

Result ptmsysmSetInfoLedPattern(RGBLedPattern pattern)
{
    u32* ipc = getThreadCommandBuffer();
    ipc[0] = 0x8010640;
    memcpy(&ipc[1], &pattern, 0x64);
    Result ret = svcSendSyncRequest(ptmsysmHandle);
    if(ret < 0) return ret;
    return ipc[1];
}

void RainbowLED() {
	RGBLedPattern pat;
    memset(&pat, 0, sizeof(pat));
    
    //marcus@Werkstaetiun:/media/marcus/WESTERNDIGI/dev_threedee/MCU_examples/RGB_rave$ lua graphics/colorgen.lua
    pat.r[0] = 128;
    pat.r[1] = 103;
    pat.r[2] = 79;
    pat.r[3] = 57;
    pat.r[4] = 38;
    pat.r[5] = 22;
    pat.r[6] = 11;
    pat.r[7] = 3;
    pat.r[8] = 1;
    pat.r[9] = 3;
    pat.r[10] = 11;
    pat.r[11] = 22;
    pat.r[12] = 38;
    pat.r[13] = 57;
    pat.r[14] = 79;
    pat.r[15] = 103;
    pat.r[16] = 128;
    pat.r[17] = 153;
    pat.r[18] = 177;
    pat.r[19] = 199;
    pat.r[20] = 218;
    pat.r[21] = 234;
    pat.r[22] = 245;
    pat.r[23] = 253;
    pat.r[24] = 255;
    pat.r[25] = 253;
    pat.r[26] = 245;
    pat.r[27] = 234;
    pat.r[28] = 218;
    pat.r[29] = 199;
    pat.r[30] = 177;
    pat.r[31] = 153;
    pat.g[0] = 238;
    pat.g[1] = 248;
    pat.g[2] = 254;
    pat.g[3] = 255;
    pat.g[4] = 251;
    pat.g[5] = 242;
    pat.g[6] = 229;
    pat.g[7] = 212;
    pat.g[8] = 192;
    pat.g[9] = 169;
    pat.g[10] = 145;
    pat.g[11] = 120;
    pat.g[12] = 95;
    pat.g[13] = 72;
    pat.g[14] = 51;
    pat.g[15] = 33;
    pat.g[16] = 18;
    pat.g[17] = 8;
    pat.g[18] = 2;
    pat.g[19] = 1;
    pat.g[20] = 5;
    pat.g[21] = 14;
    pat.g[22] = 27;
    pat.g[23] = 44;
    pat.g[24] = 65;
    pat.g[25] = 87;
    pat.g[26] = 111;
    pat.g[27] = 136;
    pat.g[28] = 161;
    pat.g[29] = 184;
    pat.g[30] = 205;
    pat.g[31] = 223;
    pat.b[0] = 18;
    pat.b[1] = 33;
    pat.b[2] = 51;
    pat.b[3] = 72;
    pat.b[4] = 95;
    pat.b[5] = 120;
    pat.b[6] = 145;
    pat.b[7] = 169;
    pat.b[8] = 192;
    pat.b[9] = 212;
    pat.b[10] = 229;
    pat.b[11] = 242;
    pat.b[12] = 251;
    pat.b[13] = 255;
    pat.b[14] = 254;
    pat.b[15] = 248;
    pat.b[16] = 238;
    pat.b[17] = 223;
    pat.b[18] = 205;
    pat.b[19] = 184;
    pat.b[20] = 161;
    pat.b[21] = 136;
    pat.b[22] = 111;
    pat.b[23] = 87;
    pat.b[24] = 64;
    pat.b[25] = 44;
    pat.b[26] = 27;
    pat.b[27] = 14;
    pat.b[28] = 5;
    pat.b[29] = 1;
    pat.b[30] = 2;
    pat.b[31] = 8;
    
    pat.ani = 0x20;
    int l;
    for(l = 0; l != 32; l++) {}
	
	if(ptmsysmInit() < 0) return 1;
    ptmsysmSetInfoLedPattern(pat);
    ptmsysmExit();
}

void LoadColor() {
	if (settings_colorvalue == 0) {
		topbgloc = "romfs:/graphics/topbg/0-gray.png";
		startborderloc = "romfs:/graphics/start_border/0-gray.png";
		color_Rvalue = 99;
		color_Gvalue = 127;
		color_Bvalue = 127;
	} else if (settings_colorvalue == 1) {
		topbgloc = "romfs:/graphics/topbg/1-brown.png";
		startborderloc = "romfs:/graphics/start_border/1-brown.png";
		color_Rvalue = 139;
		color_Gvalue = 99;
		color_Bvalue = 0;
	} else if (settings_colorvalue == 2) {
		topbgloc = "romfs:/graphics/topbg/2-red.png";
		startborderloc = "romfs:/graphics/start_border/2-red.png";
		color_Rvalue = 255;
		color_Gvalue = 0;
		color_Bvalue = 0;
	} else if (settings_colorvalue == 3) {
		topbgloc = "romfs:/graphics/topbg/3-pink.png";
		startborderloc = "romfs:/graphics/start_border/3-pink.png";
		color_Rvalue = 255;
		color_Gvalue = 127;
		color_Bvalue = 127;
	} else if (settings_colorvalue == 4) {
		topbgloc = "romfs:/graphics/topbg/4-orange.png";
		startborderloc = "romfs:/graphics/start_border/4-orange.png";
		color_Rvalue = 169;
		color_Gvalue = 31;
		color_Bvalue = 0;
	} else if (settings_colorvalue == 5) {
		topbgloc = "romfs:/graphics/topbg/5-yellow.png";
		startborderloc = "romfs:/graphics/start_border/5-yellow.png";
		color_Rvalue = 215;
		color_Gvalue = 215;
		color_Bvalue = 0;
	} else if (settings_colorvalue == 6) {
		topbgloc = "romfs:/graphics/topbg/6-yellowgreen.png";
		startborderloc = "romfs:/graphics/start_border/6-yellowgreen.png";
		color_Rvalue = 215;
		color_Gvalue = 255;
		color_Bvalue = 0;
	} else if (settings_colorvalue == 7) {
		topbgloc = "romfs:/graphics/topbg/7-green1.png";
		startborderloc = "romfs:/graphics/start_border/7-green1.png";
		color_Rvalue = 0;
		color_Gvalue = 255;
		color_Bvalue = 0;
	} else if (settings_colorvalue == 8) {
		topbgloc = "romfs:/graphics/topbg/8-green2.png";
		startborderloc = "romfs:/graphics/start_border/8-green2.png";
		color_Rvalue = 63;
		color_Gvalue = 255;
		color_Bvalue = 63;
	} else if (settings_colorvalue == 9) {
		topbgloc = "romfs:/graphics/topbg/9-lightgreen.png";
		startborderloc = "romfs:/graphics/start_border/9-lightgreen.png";
		color_Rvalue = 31;
		color_Gvalue = 231;
		color_Bvalue = 31;
	} else if (settings_colorvalue == 10) {
		topbgloc = "romfs:/graphics/topbg/10-skyblue.png";
		startborderloc = "romfs:/graphics/start_border/10-skyblue.png";
		color_Rvalue = 0;
		color_Gvalue = 63;
		color_Bvalue = 255;
	} else if (settings_colorvalue == 11) {
		topbgloc = "romfs:/graphics/topbg/11-lightblue.png";
		startborderloc = "romfs:/graphics/start_border/11-lightblue.png";
		color_Rvalue = 63;
		color_Gvalue = 63;
		color_Bvalue = 255;
	} else if (settings_colorvalue == 12) {
		topbgloc = "romfs:/graphics/topbg/12-blue.png";
		startborderloc = "romfs:/graphics/start_border/12-blue.png";
		color_Rvalue = 0;
		color_Gvalue = 0;
		color_Bvalue = 255;
	} else if (settings_colorvalue == 13) {
		topbgloc = "romfs:/graphics/topbg/13-violet.png";
		startborderloc = "romfs:/graphics/start_border/13-violet.png";
		color_Rvalue = 127;
		color_Gvalue = 0;
		color_Bvalue = 255;
	} else if (settings_colorvalue == 14) {
		topbgloc = "romfs:/graphics/topbg/14-purple.png";
		startborderloc = "romfs:/graphics/start_border/14-purple.png";
		color_Rvalue = 255;
		color_Gvalue = 0;
		color_Bvalue = 255;
	} else if (settings_colorvalue == 15) {
		topbgloc = "romfs:/graphics/topbg/15-fuschia.png";
		startborderloc = "romfs:/graphics/start_border/15-fuschia.png";
		color_Rvalue = 255;
		color_Gvalue = 0;
		color_Bvalue = 127;
	} else if (settings_colorvalue == 16) {
		topbgloc = "romfs:/graphics/topbg/16-red&blue.png";
		startborderloc = "romfs:/graphics/start_border/16-red&blue.png";
		color_Rvalue = 255;
		color_Gvalue = 0;
		color_Bvalue = 255;
	} else if (settings_colorvalue == 17) {
		topbgloc = "romfs:/graphics/topbg/17-green&yellow.png";
		startborderloc = "romfs:/graphics/start_border/17-green&yellow.png";
		color_Rvalue = 215;
		color_Gvalue = 215;
		color_Bvalue = 0;
	}
}

void ChangeFCBNRIconNo() {
	if (bnriconnum == 0) {
		bnricontexnum = fcbnricontex1;
	} else if (bnriconnum == 1) {
		bnricontexnum = fcbnricontex2;
	} else if (bnriconnum == 2) {
		bnricontexnum = fcbnricontex3;
	} else if (bnriconnum == 3) {
		bnricontexnum = fcbnricontex4;
	} else if (bnriconnum == 4) {
		bnricontexnum = fcbnricontex5;
	} else if (bnriconnum == 5) {
		bnricontexnum = fcbnricontex6;
	} else if (bnriconnum == 6) {
		bnricontexnum = fcbnricontex7;
	} else if (bnriconnum == 7) {
		bnricontexnum = fcbnricontex8;
	} else if (bnriconnum == 8) {
		bnricontexnum = fcbnricontex9;
	} else if (bnriconnum == 9) {
		bnricontexnum = fcbnricontex10;
	} else if (bnriconnum == 10) {
		bnricontexnum = fcbnricontex11;
	} else if (bnriconnum == 11) {
		bnricontexnum = fcbnricontex12;
	} else if (bnriconnum == 12) {
		bnricontexnum = fcbnricontex13;
	} else if (bnriconnum == 13) {
		bnricontexnum = fcbnricontex14;
	} else if (bnriconnum == 14) {
		bnricontexnum = fcbnricontex15;
	} else if (bnriconnum == 15) {
		bnricontexnum = fcbnricontex16;
	} else if (bnriconnum == 16) {
		bnricontexnum = fcbnricontex17;
	} else if (bnriconnum == 17) {
		bnricontexnum = fcbnricontex18;
	} else if (bnriconnum == 18) {
		bnricontexnum = fcbnricontex19;
	} else if (bnriconnum == 19) {
		bnricontexnum = fcbnricontex20;
	} else if (bnriconnum == 20) {
		bnricontexnum = fcbnricontex21;
	} else if (bnriconnum == 21) {
		bnricontexnum = fcbnricontex22;
	} else if (bnriconnum == 22) {
		bnricontexnum = fcbnricontex23;
	} else if (bnriconnum == 23) {
		bnricontexnum = fcbnricontex24;
	} else if (bnriconnum == 24) {
		bnricontexnum = fcbnricontex25;
	} else if (bnriconnum == 25) {
		bnricontexnum = fcbnricontex26;
	} else if (bnriconnum == 26) {
		bnricontexnum = fcbnricontex27;
	} else if (bnriconnum == 27) {
		bnricontexnum = fcbnricontex28;
	} else if (bnriconnum == 28) {
		bnricontexnum = fcbnricontex29;
	} else if (bnriconnum == 29) {
		bnricontexnum = fcbnricontex30;
	} else if (bnriconnum == 30) {
		bnricontexnum = fcbnricontex31;
	} else if (bnriconnum == 31) {
		bnricontexnum = fcbnricontex32;
	} else if (bnriconnum == 32) {
		bnricontexnum = fcbnricontex33;
	} else if (bnriconnum == 33) {
		bnricontexnum = fcbnricontex34;
	} else if (bnriconnum == 34) {
		bnricontexnum = fcbnricontex35;
	} else if (bnriconnum == 35) {
		bnricontexnum = fcbnricontex36;
	} else if (bnriconnum == 36) {
		bnricontexnum = fcbnricontex37;
	} else if (bnriconnum == 37) {
		bnricontexnum = fcbnricontex38;
	} else if (bnriconnum == 38) {
		bnricontexnum = fcbnricontex39;
	} else if (bnriconnum == 39) {
		bnricontexnum = fcbnricontex40;
	} else if (bnriconnum == 40) {
		bnricontexnum = fcbnricontex41;
	} else if (bnriconnum == 41) {
		bnricontexnum = fcbnricontex42;
	} else if (bnriconnum == 42) {
		bnricontexnum = fcbnricontex43;
	} else if (bnriconnum == 43) {
		bnricontexnum = fcbnricontex44;
	} else if (bnriconnum == 44) {
		bnricontexnum = fcbnricontex45;
	} else if (bnriconnum == 45) {
		bnricontexnum = fcbnricontex46;
	} else if (bnriconnum == 46) {
		bnricontexnum = fcbnricontex47;
	} else if (bnriconnum == 47) {
		bnricontexnum = fcbnricontex48;
	} else if (bnriconnum == 48) {
		bnricontexnum = fcbnricontex49;
	} else if (bnriconnum == 49) {
		bnricontexnum = fcbnricontex50;
	}
}

void ChangeBoxArtNo() {
	if (boxartnum == 0) {
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
		boxarttexnum = boxarttex11;
	} else if (boxartnum == 11) {
		boxarttexnum = boxarttex12;
	} else if (boxartnum == 12) {
		boxarttexnum = boxarttex13;
	} else if (boxartnum == 13) {
		boxarttexnum = boxarttex14;
	} else if (boxartnum == 14) {
		boxarttexnum = boxarttex15;
	} else if (boxartnum == 15) {
		boxarttexnum = boxarttex16;
	} else if (boxartnum == 16) {
		boxarttexnum = boxarttex17;
	} else if (boxartnum == 17) {
		boxarttexnum = boxarttex18;
	} else if (boxartnum == 18) {
		boxarttexnum = boxarttex19;
	} else if (boxartnum == 19) {
		boxarttexnum = boxarttex20;
	} else if (boxartnum == 20) {
		boxarttexnum = boxarttex21;
	} else if (boxartnum == 21) {
		boxarttexnum = boxarttex22;
	} else if (boxartnum == 22) {
		boxarttexnum = boxarttex23;
	} else if (boxartnum == 23) {
		boxarttexnum = boxarttex24;
	} else if (boxartnum == 24) {
		boxarttexnum = boxarttex25;
	} else if (boxartnum == 25) {
		boxarttexnum = boxarttex26;
	} else if (boxartnum == 26) {
		boxarttexnum = boxarttex27;
	} else if (boxartnum == 27) {
		boxarttexnum = boxarttex28;
	} else if (boxartnum == 28) {
		boxarttexnum = boxarttex29;
	} else if (boxartnum == 29) {
		boxarttexnum = boxarttex30;
	} else if (boxartnum == 30) {
		boxarttexnum = boxarttex31;
	} else if (boxartnum == 31) {
		boxarttexnum = boxarttex32;
	} else if (boxartnum == 32) {
		boxarttexnum = boxarttex33;
	} else if (boxartnum == 33) {
		boxarttexnum = boxarttex34;
	} else if (boxartnum == 34) {
		boxarttexnum = boxarttex35;
	} else if (boxartnum == 35) {
		boxarttexnum = boxarttex36;
	} else if (boxartnum == 36) {
		boxarttexnum = boxarttex37;
	} else if (boxartnum == 37) {
		boxarttexnum = boxarttex38;
	} else if (boxartnum == 38) {
		boxarttexnum = boxarttex39;
	} else if (boxartnum == 39) {
		boxarttexnum = boxarttex40;
	} else if (boxartnum == 40) {
		boxarttexnum = boxarttex41;
	} else if (boxartnum == 41) {
		boxarttexnum = boxarttex42;
	} else if (boxartnum == 42) {
		boxarttexnum = boxarttex43;
	} else if (boxartnum == 43) {
		boxarttexnum = boxarttex44;
	} else if (boxartnum == 44) {
		boxarttexnum = boxarttex45;
	} else if (boxartnum == 45) {
		boxarttexnum = boxarttex46;
	} else if (boxartnum == 46) {
		boxarttexnum = boxarttex47;
	} else if (boxartnum == 47) {
		boxarttexnum = boxarttex48;
	} else if (boxartnum == 48) {
		boxarttexnum = boxarttex49;
	} else if (boxartnum == 49) {
		boxarttexnum = boxarttex50;
	}
}

void ChangeFCBoxArtNo() {
	if (boxartnum == 0) {
		boxarttexnum = fcboxarttex1;
	} else if (boxartnum == 1) {
		boxarttexnum = fcboxarttex2;
	} else if (boxartnum == 2) {
		boxarttexnum = fcboxarttex3;
	} else if (boxartnum == 3) {
		boxarttexnum = fcboxarttex4;
	} else if (boxartnum == 4) {
		boxarttexnum = fcboxarttex5;
	} else if (boxartnum == 5) {
		boxarttexnum = fcboxarttex6;
	} else if (boxartnum == 6) {
		boxarttexnum = fcboxarttex7;
	} else if (boxartnum == 7) {
		boxarttexnum = fcboxarttex8;
	} else if (boxartnum == 8) {
		boxarttexnum = fcboxarttex9;
	} else if (boxartnum == 9) {
		boxarttexnum = fcboxarttex10;
	} else if (boxartnum == 10) {
		boxarttexnum = fcboxarttex11;
	} else if (boxartnum == 11) {
		boxarttexnum = fcboxarttex12;
	} else if (boxartnum == 12) {
		boxarttexnum = fcboxarttex13;
	} else if (boxartnum == 13) {
		boxarttexnum = fcboxarttex14;
	} else if (boxartnum == 14) {
		boxarttexnum = fcboxarttex15;
	} else if (boxartnum == 15) {
		boxarttexnum = fcboxarttex16;
	} else if (boxartnum == 16) {
		boxarttexnum = fcboxarttex17;
	} else if (boxartnum == 17) {
		boxarttexnum = fcboxarttex18;
	} else if (boxartnum == 18) {
		boxarttexnum = fcboxarttex19;
	} else if (boxartnum == 19) {
		boxarttexnum = fcboxarttex20;
	} else if (boxartnum == 20) {
		boxarttexnum = fcboxarttex21;
	} else if (boxartnum == 21) {
		boxarttexnum = fcboxarttex22;
	} else if (boxartnum == 22) {
		boxarttexnum = fcboxarttex23;
	} else if (boxartnum == 23) {
		boxarttexnum = fcboxarttex24;
	} else if (boxartnum == 24) {
		boxarttexnum = fcboxarttex25;
	} else if (boxartnum == 25) {
		boxarttexnum = fcboxarttex26;
	} else if (boxartnum == 26) {
		boxarttexnum = fcboxarttex27;
	} else if (boxartnum == 27) {
		boxarttexnum = fcboxarttex28;
	} else if (boxartnum == 28) {
		boxarttexnum = fcboxarttex29;
	} else if (boxartnum == 29) {
		boxarttexnum = fcboxarttex30;
	} else if (boxartnum == 30) {
		boxarttexnum = fcboxarttex31;
	} else if (boxartnum == 31) {
		boxarttexnum = fcboxarttex32;
	} else if (boxartnum == 32) {
		boxarttexnum = fcboxarttex33;
	} else if (boxartnum == 33) {
		boxarttexnum = fcboxarttex34;
	} else if (boxartnum == 34) {
		boxarttexnum = fcboxarttex35;
	} else if (boxartnum == 35) {
		boxarttexnum = fcboxarttex36;
	} else if (boxartnum == 36) {
		boxarttexnum = fcboxarttex37;
	} else if (boxartnum == 37) {
		boxarttexnum = fcboxarttex38;
	} else if (boxartnum == 38) {
		boxarttexnum = fcboxarttex39;
	} else if (boxartnum == 39) {
		boxarttexnum = fcboxarttex40;
	} else if (boxartnum == 40) {
		boxarttexnum = fcboxarttex41;
	} else if (boxartnum == 41) {
		boxarttexnum = fcboxarttex42;
	} else if (boxartnum == 42) {
		boxarttexnum = fcboxarttex43;
	} else if (boxartnum == 43) {
		boxarttexnum = fcboxarttex44;
	} else if (boxartnum == 44) {
		boxarttexnum = fcboxarttex45;
	} else if (boxartnum == 45) {
		boxarttexnum = fcboxarttex46;
	} else if (boxartnum == 46) {
		boxarttexnum = fcboxarttex47;
	} else if (boxartnum == 47) {
		boxarttexnum = fcboxarttex48;
	} else if (boxartnum == 48) {
		boxarttexnum = fcboxarttex49;
	} else if (boxartnum == 49) {
		boxarttexnum = fcboxarttex50;
	}
}

void LoadFCBNRIcon() {
	if (bnriconnum == 0) {
		fcbnricontex1 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 1) {
		fcbnricontex2 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 2) {
		fcbnricontex3 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 3) {
		fcbnricontex4 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 4) {
		fcbnricontex5 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 5) {
		fcbnricontex6 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 6) {
		fcbnricontex7 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 7) {
		fcbnricontex8 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 8) {
		fcbnricontex9 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 9) {
		fcbnricontex10 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 10) {
		fcbnricontex11 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 11) {
		fcbnricontex12 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 12) {
		fcbnricontex13 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 13) {
		fcbnricontex14 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 14) {
		fcbnricontex15 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 15) {
		fcbnricontex16 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 16) {
		fcbnricontex17 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 17) {
		fcbnricontex18 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 18) {
		fcbnricontex19 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 19) {
		fcbnricontex20 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 20) {
		fcbnricontex21 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 21) {
		fcbnricontex22 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 22) {
		fcbnricontex23 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 23) {
		fcbnricontex24 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 24) {
		fcbnricontex25 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 25) {
		fcbnricontex26 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 26) {
		fcbnricontex27 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 27) {
		fcbnricontex28 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 28) {
		fcbnricontex29 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 29) {
		fcbnricontex30 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 30) {
		fcbnricontex31 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 31) {
		fcbnricontex32 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 32) {
		fcbnricontex33 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 33) {
		fcbnricontex34 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 34) {
		fcbnricontex35 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 35) {
		fcbnricontex36 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 36) {
		fcbnricontex37 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 37) {
		fcbnricontex38 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 38) {
		fcbnricontex39 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 39) {
		fcbnricontex40 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 40) {
		fcbnricontex41 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 41) {
		fcbnricontex42 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 42) {
		fcbnricontex43 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 43) {
		fcbnricontex44 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 44) {
		fcbnricontex45 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 45) {
		fcbnricontex46 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 46) {
		fcbnricontex47 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 47) {
		fcbnricontex48 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 48) {
		fcbnricontex49 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 49) {
		fcbnricontex50 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Banner icon
	}
}

void LoadBoxArt() {
	if (boxartnum == 0) {
		boxarttex1 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 1) {
		boxarttex2 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 2) {
		boxarttex3 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 3) {
		boxarttex4 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 4) {
		boxarttex5 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 5) {
		boxarttex6 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 6) {
		boxarttex7 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 7) {
		boxarttex8 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 8) {
		boxarttex9 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 9) {
		boxarttex10 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 10) {
		boxarttex11 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 11) {
		boxarttex12 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 12) {
		boxarttex13 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 13) {
		boxarttex14 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 14) {
		boxarttex15 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 15) {
		boxarttex16 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 16) {
		boxarttex17 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 17) {
		boxarttex18 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 18) {
		boxarttex19 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 19) {
		boxarttex20 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 20) {
		boxarttex21 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 21) {
		boxarttex22 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 22) {
		boxarttex23 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 23) {
		boxarttex24 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 24) {
		boxarttex25 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 25) {
		boxarttex26 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 26) {
		boxarttex27 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 27) {
		boxarttex28 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 28) {
		boxarttex29 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 29) {
		boxarttex30 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 30) {
		boxarttex31 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 31) {
		boxarttex32 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 32) {
		boxarttex33 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 33) {
		boxarttex34 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 34) {
		boxarttex35 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 35) {
		boxarttex36 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 36) {
		boxarttex37 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 37) {
		boxarttex38 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 38) {
		boxarttex39 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 39) {
		boxarttex40 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 40) {
		boxarttex41 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 41) {
		boxarttex42 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 42) {
		boxarttex43 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 43) {
		boxarttex44 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 44) {
		boxarttex45 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 45) {
		boxarttex46 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 46) {
		boxarttex47 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 47) {
		boxarttex48 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 48) {
		boxarttex49 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 49) {
		boxarttex50 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	}
}

void LoadFCBoxArt() {
	if (boxartnum == 0) {
		fcboxarttex1 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 1) {
		fcboxarttex2 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 2) {
		fcboxarttex3 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 3) {
		fcboxarttex4 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 4) {
		fcboxarttex5 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 5) {
		fcboxarttex6 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 6) {
		fcboxarttex7 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 7) {
		fcboxarttex8 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 8) {
		fcboxarttex9 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 9) {
		fcboxarttex10 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 10) {
		fcboxarttex11 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 11) {
		fcboxarttex12 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 12) {
		fcboxarttex13 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 13) {
		fcboxarttex14 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 14) {
		fcboxarttex15 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 15) {
		fcboxarttex16 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 16) {
		fcboxarttex17 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 17) {
		fcboxarttex18 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 18) {
		fcboxarttex19 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 19) {
		fcboxarttex20 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 20) {
		fcboxarttex21 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 21) {
		fcboxarttex22 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 22) {
		fcboxarttex23 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 23) {
		fcboxarttex24 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 24) {
		fcboxarttex25 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 25) {
		fcboxarttex26 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 26) {
		fcboxarttex27 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 27) {
		fcboxarttex28 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 28) {
		fcboxarttex29 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 29) {
		fcboxarttex30 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 30) {
		fcboxarttex31 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 31) {
		fcboxarttex32 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 32) {
		fcboxarttex33 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 33) {
		fcboxarttex34 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 34) {
		fcboxarttex35 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 35) {
		fcboxarttex36 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 36) {
		fcboxarttex37 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 37) {
		fcboxarttex38 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 38) {
		fcboxarttex39 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 39) {
		fcboxarttex40 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 40) {
		fcboxarttex41 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 41) {
		fcboxarttex42 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 42) {
		fcboxarttex43 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 43) {
		fcboxarttex44 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 44) {
		fcboxarttex45 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 45) {
		fcboxarttex46 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 46) {
		fcboxarttex47 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 47) {
		fcboxarttex48 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 48) {
		fcboxarttex49 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 49) {
		fcboxarttex50 = sfil_load_PNG_file(tempimagepath, SF2D_PLACE_RAM); // Box art
	}
}

void FreeBoxArtTextures() {
	sf2d_free_texture(fcbnricontex1);
	sf2d_free_texture(fcbnricontex2);
	sf2d_free_texture(fcbnricontex3);
	sf2d_free_texture(fcbnricontex4);
	sf2d_free_texture(fcbnricontex5);
	sf2d_free_texture(fcbnricontex6);
	sf2d_free_texture(fcbnricontex7);
	sf2d_free_texture(fcbnricontex8);
	sf2d_free_texture(fcbnricontex9);
	sf2d_free_texture(fcbnricontex10);
	sf2d_free_texture(fcbnricontex11);
	sf2d_free_texture(fcbnricontex12);
	sf2d_free_texture(fcbnricontex13);
	sf2d_free_texture(fcbnricontex14);
	sf2d_free_texture(fcbnricontex15);
	sf2d_free_texture(fcbnricontex16);
	sf2d_free_texture(fcbnricontex17);
	sf2d_free_texture(fcbnricontex18);
	sf2d_free_texture(fcbnricontex19);
	sf2d_free_texture(fcbnricontex20);
	sf2d_free_texture(fcbnricontex21);
	sf2d_free_texture(fcbnricontex22);
	sf2d_free_texture(fcbnricontex23);
	sf2d_free_texture(fcbnricontex24);
	sf2d_free_texture(fcbnricontex25);
	sf2d_free_texture(fcbnricontex26);
	sf2d_free_texture(fcbnricontex27);
	sf2d_free_texture(fcbnricontex28);
	sf2d_free_texture(fcbnricontex29);
	sf2d_free_texture(fcbnricontex30);
	sf2d_free_texture(fcbnricontex31);
	sf2d_free_texture(fcbnricontex32);
	sf2d_free_texture(fcbnricontex33);
	sf2d_free_texture(fcbnricontex34);
	sf2d_free_texture(fcbnricontex35);
	sf2d_free_texture(fcbnricontex36);
	sf2d_free_texture(fcbnricontex37);
	sf2d_free_texture(fcbnricontex38);
	sf2d_free_texture(fcbnricontex39);
	sf2d_free_texture(fcbnricontex40);
	sf2d_free_texture(fcbnricontex41);
	sf2d_free_texture(fcbnricontex42);
	sf2d_free_texture(fcbnricontex43);
	sf2d_free_texture(fcbnricontex44);
	sf2d_free_texture(fcbnricontex45);
	sf2d_free_texture(fcbnricontex46);
	sf2d_free_texture(fcbnricontex47);
	sf2d_free_texture(fcbnricontex48);
	sf2d_free_texture(fcbnricontex49);
	sf2d_free_texture(fcbnricontex50);

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
	sf2d_free_texture(boxarttex11);
	sf2d_free_texture(boxarttex12);
	sf2d_free_texture(boxarttex13);
	sf2d_free_texture(boxarttex14);
	sf2d_free_texture(boxarttex15);
	sf2d_free_texture(boxarttex16);
	sf2d_free_texture(boxarttex17);
	sf2d_free_texture(boxarttex18);
	sf2d_free_texture(boxarttex19);
	sf2d_free_texture(boxarttex20);
	sf2d_free_texture(boxarttex21);
	sf2d_free_texture(boxarttex22);
	sf2d_free_texture(boxarttex23);
	sf2d_free_texture(boxarttex24);
	sf2d_free_texture(boxarttex25);
	sf2d_free_texture(boxarttex26);
	sf2d_free_texture(boxarttex27);
	sf2d_free_texture(boxarttex28);
	sf2d_free_texture(boxarttex29);
	sf2d_free_texture(boxarttex30);
	sf2d_free_texture(boxarttex31);
	sf2d_free_texture(boxarttex32);
	sf2d_free_texture(boxarttex33);
	sf2d_free_texture(boxarttex34);
	sf2d_free_texture(boxarttex35);
	sf2d_free_texture(boxarttex36);
	sf2d_free_texture(boxarttex37);
	sf2d_free_texture(boxarttex38);
	sf2d_free_texture(boxarttex39);
	sf2d_free_texture(boxarttex40);
	sf2d_free_texture(boxarttex41);
	sf2d_free_texture(boxarttex42);
	sf2d_free_texture(boxarttex43);
	sf2d_free_texture(boxarttex44);
	sf2d_free_texture(boxarttex45);
	sf2d_free_texture(boxarttex46);
	sf2d_free_texture(boxarttex47);
	sf2d_free_texture(boxarttex48);
	sf2d_free_texture(boxarttex49);
	sf2d_free_texture(boxarttex50);
	
	sf2d_free_texture(fcboxarttex1);
	sf2d_free_texture(fcboxarttex2);
	sf2d_free_texture(fcboxarttex3);
	sf2d_free_texture(fcboxarttex4);
	sf2d_free_texture(fcboxarttex5);
	sf2d_free_texture(fcboxarttex6);
	sf2d_free_texture(fcboxarttex7);
	sf2d_free_texture(fcboxarttex8);
	sf2d_free_texture(fcboxarttex9);
	sf2d_free_texture(fcboxarttex10);
	sf2d_free_texture(fcboxarttex11);
	sf2d_free_texture(fcboxarttex12);
	sf2d_free_texture(fcboxarttex13);
	sf2d_free_texture(fcboxarttex14);
	sf2d_free_texture(fcboxarttex15);
	sf2d_free_texture(fcboxarttex16);
	sf2d_free_texture(fcboxarttex17);
	sf2d_free_texture(fcboxarttex18);
	sf2d_free_texture(fcboxarttex19);
	sf2d_free_texture(fcboxarttex20);
	sf2d_free_texture(fcboxarttex21);
	sf2d_free_texture(fcboxarttex22);
	sf2d_free_texture(fcboxarttex23);
	sf2d_free_texture(fcboxarttex24);
	sf2d_free_texture(fcboxarttex25);
	sf2d_free_texture(fcboxarttex26);
	sf2d_free_texture(fcboxarttex27);
	sf2d_free_texture(fcboxarttex28);
	sf2d_free_texture(fcboxarttex29);
	sf2d_free_texture(fcboxarttex30);
	sf2d_free_texture(fcboxarttex31);
	sf2d_free_texture(fcboxarttex32);
	sf2d_free_texture(fcboxarttex33);
	sf2d_free_texture(fcboxarttex34);
	sf2d_free_texture(fcboxarttex35);
	sf2d_free_texture(fcboxarttex36);
	sf2d_free_texture(fcboxarttex37);
	sf2d_free_texture(fcboxarttex38);
	sf2d_free_texture(fcboxarttex39);
	sf2d_free_texture(fcboxarttex40);
	sf2d_free_texture(fcboxarttex41);
	sf2d_free_texture(fcboxarttex42);
	sf2d_free_texture(fcboxarttex43);
	sf2d_free_texture(fcboxarttex44);
	sf2d_free_texture(fcboxarttex45);
	sf2d_free_texture(fcboxarttex46);
	sf2d_free_texture(fcboxarttex47);
	sf2d_free_texture(fcboxarttex48);
	sf2d_free_texture(fcboxarttex49);
	sf2d_free_texture(fcboxarttex50);
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
	if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_topborder, 0) == 1) {
		settings_topbordervalue = 1;
	} else {
		settings_topbordervalue = 0;
	}
	if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_toplayout, 0) == 1) {
		romselect_toplayout = 1;
	} else {
		romselect_toplayout = 0;
	}
	/* if (settingsini.GetInt(settingsini_frontend, settingsini_frontend_botlayout, 0) == 1) {
		romselect_layout = 1;
	} else {
		romselect_layout = 0;
	} */
	if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_rainbowled, 0) == 1) {
		twlsettings_rainbowledvalue = 1;
	} else {
		twlsettings_rainbowledvalue = 0;
	}
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
	if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_forwarder, 0) == 1) {
		twlsettings_forwardervalue = 1;
	} else {
		twlsettings_forwardervalue = 0;
	}
	if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_flashcard, 0) == 6) {
		twlsettings_flashcardvalue = 6;
	} else if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_flashcard, 0) == 5) {
		twlsettings_flashcardvalue = 5;
	} else if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_flashcard, 0) == 4) {
		twlsettings_flashcardvalue = 4;
	} else if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_flashcard, 0) == 3) {
		twlsettings_flashcardvalue = 3;
	} else if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_flashcard, 0) == 2) {
		twlsettings_flashcardvalue = 2;
	} else if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_flashcard, 0) == 1) {
		twlsettings_flashcardvalue = 1;
	} else {
		twlsettings_flashcardvalue = 0;
	}
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
	if (bootstrapini.GetInt(bootstrapini_ndsbootstrap, bootstrapini_lockarm9scfgext, 0) == 1) {
		twlsettings_lockarm9scfgextvalue = 1;
	} else {
		twlsettings_lockarm9scfgextvalue = 0;
	}
}

void SaveSettings() {
	settingsini.SetInt(settingsini_frontend, settingsini_frontend_color, settings_colorvalue);
	settingsini.SetInt(settingsini_frontend, settingsini_frontend_topborder, settings_topbordervalue);
	settingsini.SetInt(settingsini_frontend, settingsini_frontend_toplayout, romselect_toplayout);
	//settingsini.SetInt(settingsini_frontend, settingsini_frontend_botlayout, romselect_layout);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_rainbowled, twlsettings_rainbowledvalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_clock, twlsettings_cpuspeedvalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_bootani, twlsettings_bootscreenvalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_hsmsg, twlsettings_healthsafetyvalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_launchslot1, twlsettings_launchslot1value);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_resetslot1, twlsettings_resetslot1value);
	if (twlsettings_consolevalue == 0) {
		bootstrapini.SetInt(settingsini_twlmode, settingsini_twl_debug, -1);
	} else if (twlsettings_consolevalue == 1) {
		bootstrapini.SetInt(settingsini_twlmode, settingsini_twl_debug, 0);
	} else if (twlsettings_consolevalue == 2) {
		bootstrapini.SetInt(settingsini_twlmode, settingsini_twl_debug, 1);
	}
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_forwarder, twlsettings_forwardervalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_flashcard, twlsettings_flashcardvalue);
	// Set ROM path if ROM is selected
	if (settingsini.GetInt(settingsini_twlmode, settingsini_twl_launchslot1, 0) == 0) {
		bootstrapini.SetString(bootstrapini_ndsbootstrap, bootstrapini_ndspath,fat+rom);
	}
	settingsini.SaveIniFile( "sdmc:/_nds/twloader/settings.ini");
	bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_boostcpu, twlsettings_cpuspeedvalue);
	if (twlsettings_consolevalue == 0) {
		bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, -1);
	} else if (twlsettings_consolevalue == 1) {
		bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, 0);
	} else if (twlsettings_consolevalue == 2) {
		bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_debug, 1);
	}
	bootstrapini.SetInt(bootstrapini_ndsbootstrap, bootstrapini_lockarm9scfgext, twlsettings_lockarm9scfgextvalue);
	bootstrapini.SaveIniFile( "sdmc:/_nds/nds-bootstrap.ini");
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

int main()
{
	aptInit();
	amInit();
	sdmcInit();
	romfsInit();
	srvInit();
	hidInit();

	// make folders if they don't exist
	mkdir("sdmc:/nds", 0777);
	mkdir("sdmc:/_dsttfwd", 0777);
	mkdir("sdmc:/_dstwofwd", 0777);
	mkdir("sdmc:/_nds/twloader", 0777);
	mkdir("sdmc:/_nds/twloader/bnricons", 0777);
	mkdir("sdmc:/_nds/twloader/bnricons/flashcard", 0777);
	mkdir("sdmc:/_nds/twloader/boxart", 0777);
	mkdir("sdmc:/_nds/twloader/boxart/flashcard", 0777);
	//mkdir("sdmc:/_nds/twloader/tmp", 0777);

	//if (bootstrapini.GetInt(settingsini_frontend, settingsini_frontend_twlappinstalled, 0) == 0) {
	//	ctr::app::install(ctr::fs::NAND, "sdmc:/_nds/twloader/cia/twlapp.cia", 0, 1287328);
	//	bootstrapini.SetInt(settingsini_frontend, settingsini_frontend_twlappinstalled, 1);
	//	settingsini.SaveIniFile( "sdmc:/_nds/twloader/settings.ini");
	//}

	// We might need a way to install the SRL CIA from romfs
	//	ctr::app::install(ctr::fs::NAND, "romfs:/twloader-twl.cia", 0, u64 size);

	std::string	bootstrapPath = "";
	
    sf2d_init();
 	ndspInit();
	sf2d_set_clear_color(RGBA8(0x00, 0x00, 0x00, 0x00));
	sf2d_set_3D(1);

	// Font loading
	sftd_init();
	sftd_font *font = sftd_load_font_file("romfs:/font.ttf");
	sftd_draw_textf(font, 0, 0, RGBA8(255, 0, 0, 255), 20, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890:-.'!?()\"end"); //Hack to avoid blurry text!

	LoadSettings();

	LoadColor();
	sf2d_texture *toptex = sfil_load_PNG_file("romfs:/graphics/top.png", SF2D_PLACE_RAM); // Top DSi-Menu border
	sf2d_texture *topbgtex = sfil_load_PNG_file(topbgloc, SF2D_PLACE_RAM); // Top background, behind the DSi-Menu border
	//sf2d_texture *vol0tex = sfil_load_PNG_file("romfs:/graphics/volume0.png", SF2D_PLACE_RAM);
	//sf2d_texture *vol1tex = sfil_load_PNG_file("romfs:/graphics/volume1.png", SF2D_PLACE_RAM);
	//sf2d_texture *vol2tex = sfil_load_PNG_file("romfs:/graphics/volume2.png", SF2D_PLACE_RAM);
	//sf2d_texture *vol3tex = sfil_load_PNG_file("romfs:/graphics/volume3.png", SF2D_PLACE_RAM);
	sf2d_texture *shoulderLtex = sfil_load_PNG_file("romfs:/graphics/shoulder_L.png", SF2D_PLACE_RAM); // L shoulder
	sf2d_texture *shoulderRtex = sfil_load_PNG_file("romfs:/graphics/shoulder_R.png", SF2D_PLACE_RAM); // R shoulder
	sf2d_texture *batterychrgtex = sfil_load_PNG_file("romfs:/graphics/battery_charging.png", SF2D_PLACE_RAM);
	sf2d_texture *battery0tex = sfil_load_PNG_file("romfs:/graphics/battery0.png", SF2D_PLACE_RAM);
	sf2d_texture *battery1tex = sfil_load_PNG_file("romfs:/graphics/battery1.png", SF2D_PLACE_RAM);
	sf2d_texture *battery2tex = sfil_load_PNG_file("romfs:/graphics/battery2.png", SF2D_PLACE_RAM);
	sf2d_texture *battery3tex = sfil_load_PNG_file("romfs:/graphics/battery3.png", SF2D_PLACE_RAM);
	sf2d_texture *battery4tex = sfil_load_PNG_file("romfs:/graphics/battery4.png", SF2D_PLACE_RAM);
	sf2d_texture *battery5tex = sfil_load_PNG_file("romfs:/graphics/battery5.png", SF2D_PLACE_RAM);
	sf2d_texture *setbatterychrgtex = sfil_load_PNG_file("romfs:/graphics/settings/battery_charging.png", SF2D_PLACE_RAM);
	sf2d_texture *setbattery0tex = sfil_load_PNG_file("romfs:/graphics/settings/battery0.png", SF2D_PLACE_RAM);
	sf2d_texture *setbattery1tex = sfil_load_PNG_file("romfs:/graphics/settings/battery1.png", SF2D_PLACE_RAM);
	sf2d_texture *setbattery2tex = sfil_load_PNG_file("romfs:/graphics/settings/battery2.png", SF2D_PLACE_RAM);
	sf2d_texture *setbattery3tex = sfil_load_PNG_file("romfs:/graphics/settings/battery3.png", SF2D_PLACE_RAM);
	sf2d_texture *setbattery4tex = sfil_load_PNG_file("romfs:/graphics/settings/battery4.png", SF2D_PLACE_RAM);
	sf2d_texture *setbattery5tex = sfil_load_PNG_file("romfs:/graphics/settings/battery5.png", SF2D_PLACE_RAM);
	sf2d_texture *bottomtex = sfil_load_PNG_file("romfs:/graphics/bottom.png", SF2D_PLACE_RAM); // Bottom of menu
	sf2d_texture *iconunktex = sfil_load_PNG_file("romfs:/graphics/icon_placeholder.png", SF2D_PLACE_RAM); // Icon placeholder at bottom of menu
	sf2d_texture *homeicontex = sfil_load_PNG_file("romfs:/graphics/homeicon.png", SF2D_PLACE_RAM); // HOME icon
	sf2d_texture *whomeicontex = sfil_load_PNG_file("romfs:/graphics/whomeicon.png", SF2D_PLACE_RAM); // HOME icon (Settings)
	sf2d_texture *bottomlogotex = sfil_load_PNG_file("romfs:/graphics/bottom_logo.png", SF2D_PLACE_RAM); // TWLoader logo on bottom screen
	sf2d_texture *bottomcovertex = sfil_load_PNG_file("romfs:/graphics/bottom_cover.png", SF2D_PLACE_RAM); // Image to cover selected game/app
	sf2d_texture *startbordertex = sfil_load_PNG_file(startborderloc, SF2D_PLACE_RAM); // "START" border
	sf2d_texture *settingsboxtex = sfil_load_PNG_file("romfs:/graphics/settingsbox.png", SF2D_PLACE_RAM); // Settings box on bottom screen
	sf2d_texture *carttex = sfil_load_PNG_file("romfs:/graphics/cart.png", SF2D_PLACE_RAM); // Cartridge on bottom screen
	sf2d_texture *boxfulltex = sfil_load_PNG_file("romfs:/graphics/box_full.png", SF2D_PLACE_RAM); // (DSiWare) box on bottom screen
	sf2d_texture *bracetex = sfil_load_PNG_file("romfs:/graphics/brace.png", SF2D_PLACE_RAM); // Brace (C-shaped thingy)
	sf2d_texture *bubbletex = sfil_load_PNG_file("romfs:/graphics/bubble.png", SF2D_PLACE_RAM); // Text bubble
	sf2d_texture *settingstex = sfil_load_PNG_file("romfs:/graphics/settings/screen.png", SF2D_PLACE_RAM); // Bottom of settings screen
	sf2d_texture *settingslogotex = sfil_load_PNG_file("romfs:/graphics/settings/logo.png", SF2D_PLACE_RAM); // TWLoader logo on bottom screen
	sf2d_texture *dsboottex = sfil_load_PNG_file("romfs:/graphics/settings/dsboot.png", SF2D_PLACE_RAM); // DS boot screen in settings
	sf2d_texture *dsiboottex = sfil_load_PNG_file("romfs:/graphics/settings/dsiboot.png", SF2D_PLACE_RAM); // DSi boot screen in settings
	sf2d_texture *dshstex = sfil_load_PNG_file("romfs:/graphics/settings/dshs.png", SF2D_PLACE_RAM); // DS H&S screen in settings
	sf2d_texture *dsihstex = sfil_load_PNG_file("romfs:/graphics/settings/dsihs.png", SF2D_PLACE_RAM); // DSi H&S screen in settings
	sf2d_texture *whitescrtex = sfil_load_PNG_file("romfs:/graphics/settings/whitescr.png", SF2D_PLACE_RAM); // White screen in settings
	sf2d_texture *disabledtex = sfil_load_PNG_file("romfs:/graphics/settings/disable.png", SF2D_PLACE_RAM);
	//bool musicbool = false;
	/* sound bgm_menu("romfs:/music/menu.wav");
	sound bgm_settings("romfs:/music/settings.wav"); */
	sound sfx_launch("romfs:/sounds/launch.wav",2,false);
	sound sfx_select("romfs:/sounds/select.wav",2,false);
	sound sfx_stop("romfs:/sounds/stop.wav",2,false);
	sound sfx_switch("romfs:/sounds/switch.wav",2,false);
	sound sfx_wrong("romfs:/sounds/wrong.wav",2,false);
	sound sfx_back("romfs:/sounds/back.wav",2,false);

	std::vector<std::string> files = {};
	std::vector<std::string> fcfiles = {};
	std::vector<std::string> fcbnriconfiles = {};
	std::vector<std::string> boxartfiles = {};
	std::vector<std::string> fcboxartfiles = {};
	
	std::string extension_UCnds = ".NDS";
	std::string extension_LCnds = ".nds";
	std::string extension_oddnds1 = ".Nds";
	std::string extension_oddnds2 = ".nDs";
	std::string extension_oddnds3 = ".ndS";
	std::string extension_oddnds4 = ".NDs";
	std::string extension_oddnds5 = ".nDS";
	std::string extension_oddnds6 = ".NdS";
	std::string extension_UCini = ".INI";
	std::string extension_LCini = ".ini";
	std::string extension_oddini1 = ".Ini";
	std::string extension_oddini2 = ".iNi";
	std::string extension_oddini3 = ".inI";
	std::string extension_oddini4 = ".INi";
	std::string extension_oddini5 = ".iNi";
	std::string extension_oddini6 = ".InI";
	std::string extension_UCpng = ".PNG";
	std::string extension_LCpng = ".png";
		
	DIR *dir;
	DIR *flashcarddir;
	DIR *fcbnricondir;
	DIR *boxartdir;
	DIR *fcboxartdir;
	struct dirent *ent;

	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Now loading banner icons (Flashcard)...");
	sf2d_end_frame();
	sf2d_swapbuffers();

	if ((fcbnricondir = opendir ("sdmc:/_nds/twloader/bnricons/flashcard/")) != NULL) {
		while ((ent = readdir (fcbnricondir)) != NULL) {
			std::string fcbifname = (ent->d_name);
			if(fcbifname.find(extension_UCpng, (fcbifname.length() - extension_UCpng.length())) != std::string::npos)
				fcbnriconfiles.push_back(ent->d_name);
			if(fcbifname.find(extension_LCpng, (fcbifname.length() - extension_LCpng.length())) != std::string::npos)
				fcbnriconfiles.push_back(ent->d_name);
		}
		closedir (fcbnricondir);
	}

	for(bnriconnum = 0; bnriconnum < 50; bnriconnum++) {
		if (bnriconnum < fcbnriconfiles.size()) {
			boxartfile_fullpath = malloc(256);
			bnriconfile = fcbnriconfiles.at(bnriconnum).c_str();
			strcat(boxartfile_fullpath, fcbnriconfolder);
			strcat(boxartfile_fullpath, bnriconfile);

			if (fopen (boxartfile_fullpath+3, "rb") != NULL) {
				tempimagepath = boxartfile_fullpath+3;
			} fclose (fopen (boxartfile_fullpath+3, "rb"));
		} else {
			tempimagepath = "romfs:/graphics/icon_unknown.png";	// Prevent crashing
		}
		LoadFCBNRIcon();
	}

	boxartnum = 0;

	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Now loading box art (SD Card)...");
	sf2d_end_frame();
	sf2d_swapbuffers();

	if ((boxartdir = opendir ("sdmc:/_nds/twloader/boxart/")) != NULL) {
		while ((ent = readdir (boxartdir)) != NULL) {
			std::string bafname = (ent->d_name);
			if(bafname.find(extension_UCpng, (bafname.length() - extension_UCpng.length())) != std::string::npos)
				boxartfiles.push_back(ent->d_name);
			if(bafname.find(extension_LCpng, (bafname.length() - extension_LCpng.length())) != std::string::npos)
				boxartfiles.push_back(ent->d_name);
		}
		closedir (boxartdir);
	}

	for(boxartnum = 0; boxartnum < 50; boxartnum++) {
		if (boxartnum < boxartfiles.size()) {
			boxartfile_fullpath = malloc(256);
			boxartfile = boxartfiles.at(boxartnum).c_str();
			strcat(boxartfile_fullpath, boxartfolder);
			strcat(boxartfile_fullpath, boxartfile);
		
			if (fopen (boxartfile_fullpath+3, "rb") != NULL) {
				tempimagepath = boxartfile_fullpath+3;
			} fclose (fopen (boxartfile_fullpath+3, "rb"));
		} else {
			tempimagepath = "romfs:/graphics/boxart_unknown.png";	// Prevent crashing
		}
		LoadBoxArt();
	}

	boxartnum = 0;
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Now loading box art (Flashcard)...");
	sf2d_end_frame();
	sf2d_swapbuffers();

	if ((fcboxartdir = opendir ("sdmc:/_nds/twloader/boxart/flashcard/")) != NULL) {
		while ((ent = readdir (fcboxartdir)) != NULL) {
			std::string fcbafname = (ent->d_name);
			if(fcbafname.find(extension_UCpng, (fcbafname.length() - extension_UCpng.length())) != std::string::npos)
				fcboxartfiles.push_back(ent->d_name);
			if(fcbafname.find(extension_LCpng, (fcbafname.length() - extension_LCpng.length())) != std::string::npos)
				fcboxartfiles.push_back(ent->d_name);
		}
		closedir (fcboxartdir);
	}

	for(boxartnum = 0; boxartnum < 50; boxartnum++) {
		if (boxartnum < fcboxartfiles.size()) {
			boxartfile_fullpath = malloc(256);
			boxartfile = fcboxartfiles.at(boxartnum).c_str();
			strcat(boxartfile_fullpath, fcboxartfolder);
			strcat(boxartfile_fullpath, boxartfile);

			if (fopen (boxartfile_fullpath+3, "rb") != NULL) {
				tempimagepath = boxartfile_fullpath+3;
			} fclose (fopen (boxartfile_fullpath+3, "rb"));
		} else {
			tempimagepath = "romfs:/graphics/boxart_unknown.png";	// Prevent crashing
		}
		LoadFCBoxArt();
	}

	boxartnum = 0;
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	// No text
	sf2d_end_frame();
	sf2d_swapbuffers();

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
	
	if ((flashcarddir = opendir ("sdmc:/nds/flashcard/")) != NULL) {
	/* print all the files and directories within directory */
		while ((ent = readdir (flashcarddir)) != NULL) {
			std::string fcfname = (ent->d_name);
			if(fcfname.find(extension_UCini, (fcfname.length() - extension_UCini.length())) != std::string::npos)
				fcfiles.push_back(ent->d_name);
			if(fcfname.find(extension_LCini, (fcfname.length() - extension_LCini.length())) != std::string::npos)
				fcfiles.push_back(ent->d_name);
			if(fcfname.find(extension_oddini1, (fcfname.length() - extension_oddini1.length())) != std::string::npos)
				fcfiles.push_back(ent->d_name);
			if(fcfname.find(extension_oddini2, (fcfname.length() - extension_oddini2.length())) != std::string::npos)
				fcfiles.push_back(ent->d_name);
			if(fcfname.find(extension_oddini3, (fcfname.length() - extension_oddini3.length())) != std::string::npos)
				fcfiles.push_back(ent->d_name);
			if(fcfname.find(extension_oddini4, (fcfname.length() - extension_oddini4.length())) != std::string::npos)
				fcfiles.push_back(ent->d_name);
			if(fcfname.find(extension_oddini5, (fcfname.length() - extension_oddini5.length())) != std::string::npos)
				fcfiles.push_back(ent->d_name);
			if(fcfname.find(extension_oddini6, (fcfname.length() - extension_oddini6.length())) != std::string::npos)
				fcfiles.push_back(ent->d_name);
		}
		closedir (flashcarddir);
	}
		
	int cursorPosition = 0, i = 0;
	bool noromsfound = false;
	int settingscursorPosition = 0, twlsettingscursorPosition = 0;
	
	bool cursorPositionset = false;
	
	int soundwaittimer = 0;
	bool playwrongsounddone = false;

	sf2d_texture *batteryIcon;
	
	int fadealpha = 255;
	bool fadein = true;
	bool fadeout = false;
		
	bool colortexloaded = true;

	bool updatebotscreen = true;
	bool screenmodeswitch = false;
	bool applaunchprep = false;
	bool applaunchon = false;
		
	float offset3dl_topbg = 0.0f;
	float offset3dr_topbg = 0.0f;
	float offset3dl_boxart = 0.0f;
	float offset3dr_boxart = 0.0f;
	float offset3dl_disabled = 0.0f;
	float offset3dr_disabled = 0.0f;

	//char* nickname = "Nickname";
	
	int boxartXpos;
	int boxartXmovepos = 0;
	
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
	int ndsiconXpos;
	int ndsiconYmovepos = 133;
	
	int startbordermovepos;
	float startborderscalesize;
			
	int screenmode = 0;
	// 0: ROM select
	// 1: Settings
	
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
		
		offset3dl_topbg = CONFIG_3D_SLIDERSTATE * -12.0f;
		offset3dr_topbg = CONFIG_3D_SLIDERSTATE * 12.0f;
		offset3dl_boxart = CONFIG_3D_SLIDERSTATE * -5.0f;
		offset3dr_boxart = CONFIG_3D_SLIDERSTATE * 5.0f;
		offset3dl_disabled = CONFIG_3D_SLIDERSTATE * -3.0f;
		offset3dr_disabled = CONFIG_3D_SLIDERSTATE * 3.0f;

		u8 batteryChargeState = 0;
		u8 batteryLevel = 0;
		
		if(screenmode == 0) {
			/* if (!musicbool) {
				bgm_menu.play();
				musicbool = true;
			} */
			if (twlsettings_forwardervalue == 1) {
				noromtext1 = "No INIs found!";
				noromtext2 = "Put .ini files in 'sdmc:/nds/flashcard'.";
			} else {
				noromtext1 = "No ROMs found!";
				noromtext2 = "Put .nds ROMs in 'sdmc:/nds'.";
			}
			if (colortexloaded == false) {
				topbgtex = sfil_load_PNG_file(topbgloc, SF2D_PLACE_RAM); // Top background, behind the DSi-Menu border
				startbordertex = sfil_load_PNG_file(startborderloc, SF2D_PLACE_RAM); // "START" border
				colortexloaded = true;
			}
			if(R_SUCCEEDED(PTMU_GetBatteryChargeState(&batteryChargeState)) && batteryChargeState) {
				batteryIcon = batterychrgtex;
			} else if(R_SUCCEEDED(PTMU_GetBatteryLevel(&batteryLevel))) {
				batteryIcon = battery5tex;
			} else {
				batteryIcon = battery0tex;
			}
			sf2d_start_frame(GFX_TOP, GFX_LEFT);
			sf2d_draw_texture_scale(topbgtex, offset3dl_topbg + -12, 0, 1.32, 1);
			if (i != 0) {	// If ROMs are found, then display box art
				if (romselect_toplayout == 0) {
					if (twlsettings_forwardervalue == 1) {
						boxartXpos = 136;
						for(boxartnum = 0; boxartnum < fcfiles.size(); boxartnum++) {
							ChangeFCBoxArtNo();
							sf2d_draw_texture(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
							sf2d_draw_texture_scale_blend(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 0xC0)); // Draw box art's reflection
							boxartXpos += 144;
						}
					} else {
						boxartXpos = 136;
						for(boxartnum = 0; boxartnum < files.size(); boxartnum++) {
							ChangeBoxArtNo();
							sf2d_draw_texture(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
							sf2d_draw_texture_scale_blend(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 0xC0)); // Draw box art's reflection
							boxartXpos += 144;
						}
					}
				}
			} else {
				sftd_draw_textf(font, offset3dl_boxart+152, 96, RGBA8(255, 255, 255, 255), 12, noromtext1);
				sftd_draw_textf(font, offset3dl_boxart+124, 112, RGBA8(255, 255, 255, 255), 12, noromtext2);
			}
			if (settings_topbordervalue == 1) {
				sf2d_draw_texture(toptex, 400/2 - toptex->width/2, 240/2 - toptex->height/2);
				sftd_draw_text(font, 328, 2, RGBA8(0, 0, 0, 255), 13, RetTime().c_str());
			} else {
				sftd_draw_text(font, 328, 2, RGBA8(255, 255, 255, 255), 13, RetTime().c_str());
			}
			sf2d_draw_texture(batteryIcon, 371, 2);
			//sftd_draw_textf(font, 24, 2, RGBA8(0, 0, 0, 255), 12, nickname);
			//sftd_draw_textf(font, 2, 2, RGBA8(0, 0, 0, 255), 12, boxartfile_fullpath); // Debug text
			sf2d_draw_texture(shoulderLtex, 0, LshoulderYpos);
			sf2d_draw_texture(shoulderRtex, 328, RshoulderYpos);
			sftd_draw_textf(font, 17, LshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Lshouldertext);
			sftd_draw_textf(font, 332, RshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Rshouldertext);

			sf2d_draw_rectangle(0, 0, 400, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect

			sf2d_end_frame();
				
			sf2d_start_frame(GFX_TOP, GFX_RIGHT);
			sf2d_draw_texture_scale(topbgtex, offset3dr_topbg + -12, 0, 1.32, 1);
			if (i != 0) {	// If ROMs are found, then display box art
				if (romselect_toplayout == 0) {
					if (twlsettings_forwardervalue == 1) {
						boxartXpos = 136;
						for(boxartnum = 0; boxartnum < fcfiles.size(); boxartnum++) {
							ChangeFCBoxArtNo();
							sf2d_draw_texture(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
							sf2d_draw_texture_scale_blend(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 0xC0)); // Draw box art's reflection
							boxartXpos += 144;
						}
					} else {
						boxartXpos = 136;
						for(boxartnum = 0; boxartnum < files.size(); boxartnum++) {
							ChangeBoxArtNo();
							sf2d_draw_texture(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
							sf2d_draw_texture_scale_blend(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 0xC0)); // Draw box art's reflection
							boxartXpos += 144;
						}
					}
				}
			} else {
				sftd_draw_textf(font, offset3dr_boxart+152, 96, RGBA8(255, 255, 255, 255), 12, noromtext1);
				sftd_draw_textf(font, offset3dr_boxart+124, 112, RGBA8(255, 255, 255, 255), 12, noromtext2);
			}
			if (settings_topbordervalue == 1) {
				sf2d_draw_texture(toptex, 400/2 - toptex->width/2, 240/2 - toptex->height/2);
				sftd_draw_text(font, 328, 2, RGBA8(0, 0, 0, 255), 13, RetTime().c_str());
			} else {
				sftd_draw_text(font, 328, 2, RGBA8(255, 255, 255, 255), 13, RetTime().c_str());
			}
			sf2d_draw_texture(batteryIcon, 371, 2);
			//sftd_draw_textf(font, 24, 2, RGBA8(0, 0, 0, 255), 12, nickname);
			//sftd_draw_textf(font, 2, 2, RGBA8(0, 0, 0, 255), 12, boxartfile_fullpath); // Debug text
			sf2d_draw_texture(shoulderLtex, -1, LshoulderYpos);
			sf2d_draw_texture(shoulderRtex, 327, RshoulderYpos);
			sftd_draw_textf(font, 16, LshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Lshouldertext);
			sftd_draw_textf(font, 331, RshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Rshouldertext);
			
			sf2d_draw_rectangle(0, 0, 400, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect
			
			sf2d_end_frame();
		} else if(screenmode == 1) {
			/* if (!musicbool) {
				bgm_settings.play();
				musicbool = true;
			} */
			if (colortexloaded == true) {
				sf2d_free_texture(topbgtex);
				sf2d_free_texture(startbordertex);
				colortexloaded = false;
			}
			if(R_SUCCEEDED(PTMU_GetBatteryChargeState(&batteryChargeState)) && batteryChargeState) {
				batteryIcon = setbatterychrgtex;
			} else if(R_SUCCEEDED(PTMU_GetBatteryLevel(&batteryLevel))) {
				batteryIcon = setbattery5tex;
			} else {
				batteryIcon = setbattery0tex;
			}
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
					sf2d_draw_texture(whitescrtex, offset3dl_boxart+136, 124); // Draw H&S screen
				}
				if (twlsettings_bootscreenvalue == 0) {
					sf2d_draw_texture(disabledtex, offset3dl_disabled+136, 20); // Draw disabled texture
					sf2d_draw_texture(disabledtex, offset3dl_disabled+136, 124); // Draw disabled texture
				}
			} else {
				sf2d_draw_texture(settingslogotex, offset3dl_boxart+400/2 - settingslogotex->width/2, 240/2 - settingslogotex->height/2);
			}
			sftd_draw_text(font, 328, 2, RGBA8(255, 255, 255, 255), 13, RetTime().c_str());
			sftd_draw_textf(font, 336, 218, RGBA8(255, 255, 255, 255), 14, settings_vertext);
			sf2d_draw_texture(batteryIcon, 371, 2);
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
					sf2d_draw_texture(whitescrtex, offset3dr_boxart+136, 124); // Draw H&S screen
				}
				if (twlsettings_bootscreenvalue == 0) {
					sf2d_draw_texture(disabledtex, offset3dr_disabled+136, 20); // Draw disabled texture
					sf2d_draw_texture(disabledtex, offset3dr_disabled+136, 124); // Draw disabled texture
				}
			} else {
				sf2d_draw_texture(settingslogotex, offset3dr_boxart+400/2 - settingslogotex->width/2, 240/2 - settingslogotex->height/2);
			}
			sftd_draw_text(font, 328, 2, RGBA8(255, 255, 255, 255), 13, RetTime().c_str());
			sftd_draw_textf(font, 336, 218, RGBA8(255, 255, 255, 255), 14, settings_vertext);
			sf2d_draw_texture(batteryIcon, 371, 2);
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
		
		if (fadein == true) {
			fadealpha -= 31;
			if (fadealpha < 0) {
				fadealpha = 0;
				fadein = false;
				titleboxXmovetimer = 0;
			}
		}
		
		if (fadeout == true) {
			fadealpha += 31;
			if (fadealpha > 255) {
				fadealpha = 255;
				//musicbool = false;
				screenmode = 0;
				fadeout = false;
				fadein = true;
			}
		}
		
		if (playwrongsounddone == true) {
			if (hHeld & KEY_LEFT || hHeld & KEY_RIGHT) {} else {
				soundwaittimer += 1;
				if (soundwaittimer == 3) {
					soundwaittimer = 0;
					playwrongsounddone = false;
				}
			}
		}

		if(titleboxXmoveleft == true) {
			titleboxXmovetimer += 1;
			if (titleboxXmovetimer == 10) {
				titleboxXmovetimer = 0;
				titleboxXmoveleft = false;
			} else if (titleboxXmovetimer == 9) {
				// Delay a frame
				sfx_stop.play();
			} else if (titleboxXmovetimer == 8) {
				titleboxXmovepos += 8;
				boxartXmovepos += 18;
				startbordermovepos = 1;
				startborderscalesize = 0.97;
				cursorPositionset = false;
			} else if (titleboxXmovetimer == 2) {
				titleboxXmovepos += 8;
				boxartXmovepos += 18;
				sfx_select.play();
			} else {
				if (cursorPositionset == false) {
					cursorPosition--;
					if (twlsettings_forwardervalue == 1) {
						if (cursorPosition == -1)
							cursorPosition--;
					}
					cursorPositionset = true;
				}
				if (cursorPosition != -3) {
					titleboxXmovepos += 8;
					boxartXmovepos += 18;
				} else {
					titleboxXmovetimer = 0;
					titleboxXmoveleft = false;
					cursorPositionset = false;
					cursorPosition++;
					if (!playwrongsounddone) {
						sfx_wrong.stop();
						sfx_wrong.play();
						playwrongsounddone = true;
					}
				}
			}
		} else if(titleboxXmoveright == true) {
			titleboxXmovetimer += 1;
			if (titleboxXmovetimer == 10) {
				titleboxXmovetimer = 0;
				titleboxXmoveright = false;
			} else if (titleboxXmovetimer == 9) {
				// Delay a frame
				sfx_stop.play();
			} else if (titleboxXmovetimer == 8) {
				titleboxXmovepos -= 8;
				boxartXmovepos -= 18;
				startbordermovepos = 1;
				startborderscalesize = 0.97;
				cursorPositionset = false;
			} else if (titleboxXmovetimer == 2) {
				titleboxXmovepos -= 8;
				boxartXmovepos -= 18;
				sfx_select.play();
			} else {
				if (cursorPositionset == false) {
					cursorPosition++;
					if (twlsettings_forwardervalue == 1) {
						if (cursorPosition == -1)
							cursorPosition++;
					}
					cursorPositionset = true;
				}
				if (cursorPosition != i) {
					titleboxXmovepos -= 8;
					boxartXmovepos -= 18;
				} else {
					titleboxXmovetimer = 0;
					titleboxXmoveright = false;
					cursorPositionset = false;
					cursorPosition--;
					if (!playwrongsounddone) {
						sfx_wrong.stop();
						sfx_wrong.play();
						playwrongsounddone = true;
					}
				}
			}
		}
		if(applaunchprep == true) {
			titleboxYmovepos -= 6;
			ndsiconYmovepos -= 6;
			if (titleboxYmovepos == -240) {
				if(screenmodeswitch == true) {
					//musicbool = false;
					screenmode = 1;
					titleboxYmovepos = 120;
					ndsiconYmovepos = 133;
					fadein = true;
					screenmodeswitch = false;
					applaunchprep = false;
				} else {
					screenoff();
					if (twlsettings_forwardervalue == 1) {
						CIniFile setfcrompathini( flashcardfolder+rom );
						if (twlsettings_flashcardvalue == 0) {
							CIniFile fcrompathini( "sdmc:/_dsttfwd/YSMenu.ini" );
							std::string	rominini = setfcrompathini.GetString(fcrompathini_flashcardrom, fcrompathini_rompath, "");
							fcrompathini.SetString("YSMENU", "AUTO_BOOT", slashchar+rominini);
							fcrompathini.SaveIniFile( "sdmc:/_dsttfwd/YSMenu.ini" );
						} else if (twlsettings_flashcardvalue == 3) {
							CIniFile fcrompathini( "sdmc:/_nds/YSMenu.ini" );
							std::string	rominini = setfcrompathini.GetString(fcrompathini_flashcardrom, fcrompathini_rompath, "");
							fcrompathini.SetString("YSMENU", "AUTO_BOOT", slashchar+rominini);
							fcrompathini.SaveIniFile( "sdmc:/_nds/YSMenu.ini" );
						} else if (twlsettings_flashcardvalue == 5) {
							CIniFile fcrompathini( "sdmc:/_nds/lastsave.ini" );
							std::string	rominini = setfcrompathini.GetString(fcrompathini_flashcardrom, fcrompathini_rompath, "");
							fcrompathini.SetString("Save Info", "lastLoaded", woodfat+rominini);
							fcrompathini.SaveIniFile( "sdmc:/_nds/lastsave.ini" );
						} else if (twlsettings_flashcardvalue == 6) {
							CIniFile fcrompathini( "sdmc:/_dstwofwd/autoboot.ini" );
							std::string	rominini = setfcrompathini.GetString(fcrompathini_flashcardrom, fcrompathini_rompath, "");
							fcrompathini.SetString("Dir Info", "fullName", dstwofat+rominini);
							fcrompathini.SaveIniFile( "sdmc:/_dstwofwd/autoboot.ini" );
						}
					}
					SaveSettings();
					if (twlsettings_rainbowledvalue == 1) {
						RainbowLED(); }
					applaunchon = true;
				}
			}
			fadealpha += 6;
			if (fadealpha > 255) {
				fadealpha = 255;
			}
		}

		if(updatebotscreen == true){
			if (screenmode == 0) {
				i = 0;
				sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				sf2d_draw_texture(bottomtex, 320/2 - bottomtex->width/2, 240/2 - bottomtex->height/2);
				
				/* if (romselect_layout == 0) {
					filenameYpos = 0;
					if(files.size() >= 49) {
						for(i = 0; i < 50; i++){
							if(cursorPosition == i) {
								sftd_draw_textf(font, 10, filenameYpos+filenameYmovepos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, files.at(i).c_str());
								filenameYpos += 12;
							} else {
								sftd_draw_textf(font, 10, filenameYpos+filenameYmovepos, RGBA8(0, 0, 0, 255), 12, files.at(i).c_str());
								filenameYpos += 12;
							}
						}
					} else {
						for(i = 0; i < files.size(); i++){
							if(cursorPosition == i) {
								sftd_draw_textf(font, 10, filenameYpos+filenameYmovepos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, files.at(i).c_str());
								filenameYpos += 12;
							} else {
								sftd_draw_textf(font, 10, filenameYpos+filenameYmovepos, RGBA8(0, 0, 0, 255), 12, files.at(i).c_str());
								filenameYpos += 12;
							}
						}
					}
				} else { */
					if (titleboxXmovetimer == 0) {
						sf2d_draw_texture(bubbletex, 0, 0);
						// sfx_menuselect.play();
						if (twlsettings_forwardervalue == 1) {
							if (cursorPosition == -2 || cursorPosition == -1) {
							} else {
								flashcardrom = fcfiles.at(cursorPosition).c_str();
								CIniFile setfcrompathini( flashcardfolder+flashcardrom );
								romsel_gameline1 = setfcrompathini.GetString(fcrompathini_flashcardrom, fcrompathini_bnrtext1, "");
								romsel_gameline2 = setfcrompathini.GetString(fcrompathini_flashcardrom, fcrompathini_bnrtext2, "");
								romsel_gameline3 = setfcrompathini.GetString(fcrompathini_flashcardrom, fcrompathini_bnrtext3, "");
								char *cstr1 = new char[romsel_gameline1.length() + 1];
								strcpy(cstr1, romsel_gameline1.c_str());
								char *cstr2 = new char[romsel_gameline2.length() + 1];
								strcpy(cstr2, romsel_gameline2.c_str());
								char *cstr3 = new char[romsel_gameline3.length() + 1];
								strcpy(cstr3, romsel_gameline3.c_str());
								sftd_draw_textf(font, 10, 24, RGBA8(0, 0, 0, 255), 16, romsel_gameline1.c_str());
								sftd_draw_textf(font, 10, 44, RGBA8(0, 0, 0, 255), 16, romsel_gameline2.c_str());
								sftd_draw_textf(font, 10, 64, RGBA8(0, 0, 0, 255), 16, romsel_gameline3.c_str());
							}
						}
					} else {
						sf2d_draw_texture(bottomlogotex, 320/2 - bottomlogotex->width/2, 40);
					}
					sf2d_draw_texture(homeicontex, 81, 220); // Draw HOME icon
					sftd_draw_textf(font, 98, 220, RGBA8(0, 0, 0, 255), 14, ": Return to HOME Menu");
					if (twlsettings_forwardervalue == 0) {
						sf2d_draw_texture(bracetex, -32+titleboxXmovepos, 116);
						sf2d_draw_texture(settingsboxtex, setsboxXpos+titleboxXmovepos, 119);
						sf2d_draw_texture(carttex, cartXpos+titleboxXmovepos, 120);
						sf2d_draw_texture(iconunktex, 16+cartXpos+titleboxXmovepos, 133);
					} else {
						sf2d_draw_texture(bracetex, 32+titleboxXmovepos, 116);
						sf2d_draw_texture(settingsboxtex, cartXpos+titleboxXmovepos, 119);
					}

					titleboxXpos = 128;
					ndsiconXpos = 144;
					filenameYpos = 0;
					if (titleboxXmovetimer == 0) {
						if (cursorPosition == -2) {
							// sftd_draw_textf(font, 10, 8, RGBA8(127, 127, 127, 255), 12, "Settings");
							sftd_draw_textf(font, 132, 34, RGBA8(0, 0, 0, 255), 16, "Settings");
						} else if (cursorPosition == -1) {
							sftd_draw_textf(font, 10, 8, RGBA8(127, 127, 127, 255), 12, "Slot-1 cart (NTR carts only)");
						}
					}
					if (twlsettings_forwardervalue == 1) {
						if(fcfiles.size() >= 49) {
							for(i = 0; i < 50; i++){
								if (titleboxXmovetimer == 0) {
									sftd_draw_textf(font, 10, 8+filenameYpos-240*cursorPosition, RGBA8(127, 127, 127, 255), 12, fcfiles.at(i).c_str());
								}
								sf2d_draw_texture(boxfulltex, titleboxXpos+titleboxXmovepos, 120);
								titleboxXpos += 64;
								filenameYpos += 240;
							}
							for(bnriconnum = 0; bnriconnum < 50; bnriconnum++) {
								ChangeFCBNRIconNo();
								sf2d_draw_texture(bnricontexnum, ndsiconXpos+titleboxXmovepos, 133);
								ndsiconXpos += 64;
							}
						} else {
							for(i = 0; i < fcfiles.size(); i++){
								if (titleboxXmovetimer == 0) {
									sftd_draw_textf(font, 10, 8+filenameYpos-240*cursorPosition, RGBA8(127, 127, 127, 255), 12, fcfiles.at(i).c_str());
								}
								sf2d_draw_texture(boxfulltex, titleboxXpos+titleboxXmovepos, 120);
								titleboxXpos += 64;
								filenameYpos += 240;
							}
							for(bnriconnum = 0; bnriconnum < fcfiles.size(); bnriconnum++) {
								ChangeFCBNRIconNo();
								sf2d_draw_texture(bnricontexnum, ndsiconXpos+titleboxXmovepos, 133);
								ndsiconXpos += 64;
							}
						}
					} else {
						if(files.size() >= 49) {
							for(i = 0; i < 50; i++){
								if (titleboxXmovetimer == 0) {
									sftd_draw_textf(font, 10, 8+filenameYpos-240*cursorPosition, RGBA8(127, 127, 127, 255), 12, files.at(i).c_str());
								}
								sf2d_draw_texture(boxfulltex, titleboxXpos+titleboxXmovepos, 120);
								sf2d_draw_texture(iconunktex, ndsiconXpos+titleboxXmovepos, 133);
								titleboxXpos += 64;
								ndsiconXpos += 64;
								filenameYpos += 240;
							}
						} else {
							for(i = 0; i < files.size(); i++){
								if (titleboxXmovetimer == 0) {
									sftd_draw_textf(font, 10, 8+filenameYpos-240*cursorPosition, RGBA8(127, 127, 127, 255), 12, files.at(i).c_str());
								}
								sf2d_draw_texture(boxfulltex, titleboxXpos+titleboxXmovepos, 120);
								sf2d_draw_texture(iconunktex, ndsiconXpos+titleboxXmovepos, 133);
								titleboxXpos += 64;
								ndsiconXpos += 64;
								filenameYpos += 240;
							}
						}
					}
					sf2d_draw_texture_scale(bracetex, 15+ndsiconXpos+titleboxXmovepos, 116, -1, 1);
					if (applaunchprep == false) {
						if (titleboxXmovetimer == 0) {
							startbordermovepos = 0;
							startborderscalesize = 1.0;
						}
						sf2d_draw_texture_scale(startbordertex, 128+startbordermovepos, 116+startbordermovepos, startborderscalesize, startborderscalesize);
						sftd_draw_textf(font, 140, 176, RGBA8(255, 255, 255, 255), 13, "START");
					} else {
						sf2d_draw_texture(bottomcovertex, 128, 116);  // Cover selected game/app
						if (cursorPosition == -2) {
							sf2d_draw_texture(settingsboxtex, 128, titleboxYmovepos); // Draw settings box that moves up
						} else if (cursorPosition == -1) {
							sf2d_draw_texture(carttex, 128, titleboxYmovepos); // Draw selected Slot-1 game that moves up
							sf2d_draw_texture(iconunktex, 144, ndsiconYmovepos);
						} else {
							sf2d_draw_texture(boxfulltex, 128, titleboxYmovepos); // Draw selected game/app that moves up
							if (twlsettings_forwardervalue == 1) {
								bnriconnum = cursorPosition;
								ChangeFCBNRIconNo();
								sf2d_draw_texture(bnricontexnum, 144, ndsiconYmovepos);
							} else {
								sf2d_draw_texture(iconunktex, 144, ndsiconYmovepos);
							}
						}
					}
				// }
			} else if(screenmode == 1) {
				sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				sf2d_draw_texture(settingstex, 0, 0);
				sf2d_draw_texture(whomeicontex, 81, 220); // Draw HOME icon
				sftd_draw_textf(font, 98, 220, RGBA8(255, 255, 255, 255), 14, ": Return to HOME Menu");
				if (settings_subscreenmode == 0) {
					sf2d_draw_texture(shoulderLtex, 0, LshoulderYpos);
					sf2d_draw_texture(shoulderRtex, 248, RshoulderYpos);
					sftd_draw_textf(font, 17, LshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Lshouldertext);
					sftd_draw_textf(font, 252, RshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Rshouldertext);
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
					if (settings_topbordervalue == 0) {
						settings_topbordervaluetext = "Off";
					} else if (settings_topbordervalue == 1) {
						settings_topbordervaluetext = "On";
					}
						
					settingstext_bot = "Settings: GUI";
					settingsYpos = 40;
					if(settingscursorPosition == 0) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, settings_colortext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, settings_colorvaluetext);
						settingsYpos += 12;
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_colortext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_colorvaluetext);
						settingsYpos += 12;
					}
					if(settingscursorPosition == 1) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, settings_topbordertext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, settings_topbordervaluetext);
						settingsYpos += 12;
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_topbordertext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_topbordervaluetext);
						settingsYpos += 12;
					}
				} else if (settings_subscreenmode == 1) {
					sf2d_draw_texture(shoulderLtex, 0, LshoulderYpos);
					sf2d_draw_texture(shoulderRtex, 248, RshoulderYpos);
					sftd_draw_textf(font, 17, LshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Lshouldertext);
					sftd_draw_textf(font, 252, RshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Rshouldertext);
					if (twlsettings_rainbowledvalue == 1) {
						twlsettings_rainbowledvaluetext = "On";
					} else {
						twlsettings_rainbowledvaluetext = "Off";
					}
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
					if (twlsettings_lockarm9scfgextvalue == 1) {
						twlsettings_lockarm9scfgextvaluetext = "On";
					} else {
						twlsettings_lockarm9scfgextvaluetext = "Off";
					}
					settingstext_bot = "Settings: NTR/TWL-mode";
					settingsYpos = 40;
					if(twlsettingscursorPosition == 0) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_flashcardtext);
						settingsYpos += 12;
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "Pick a flashcard to use to");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "run ROMs from it.");
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_flashcardtext);
						settingsYpos += 12;
					}
					if(twlsettingscursorPosition == 1) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_rainbowledtext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_rainbowledvaluetext);
						settingsYpos += 12;
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "See rainbow colors glowing in");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "the Notification LED.");
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_rainbowledtext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_rainbowledvaluetext);
						settingsYpos += 12;
					}
					if(twlsettingscursorPosition == 2) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_cpuspeedtext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_cpuspeedvaluetext);
						settingsYpos += 12;
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "Set to TWL to get rid of lags in some games.");
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_cpuspeedtext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_cpuspeedvaluetext);
						settingsYpos += 12;
					}
					if(twlsettingscursorPosition == 3) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_bootscreentext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_bootscreenvaluetext);
						settingsYpos += 12;
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "Displays the DS/DSi boot animation");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "before launched game.");
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_bootscreentext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_bootscreenvaluetext);
						settingsYpos += 12;
					}
					if(twlsettingscursorPosition == 4) {
						sftd_draw_textf(font, settingsXpos+16, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_healthsafetytext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_healthsafetyvaluetext);
						settingsYpos += 12;
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "Displays the Health and Safety");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "message on the bottom screen.");
					} else {
						sftd_draw_textf(font, settingsXpos+16, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_healthsafetytext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_healthsafetyvaluetext);
						settingsYpos += 12;
					}
					if(twlsettingscursorPosition == 5) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_resetslot1text);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_resetslot1valuetext);
						settingsYpos += 12;
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "Enable this if Slot-1 carts are stuck");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "on white screens.");
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_resetslot1text);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_resetslot1valuetext);
						settingsYpos += 12;
					}
					if(twlsettingscursorPosition == 6) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_consoletext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_consolevaluetext);
						settingsYpos += 12;
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "Displays some text before launched game.");
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_consoletext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_consolevaluetext);
						settingsYpos += 12;
					}
					if(twlsettingscursorPosition == 7) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_lockarm9scfgexttext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_lockarm9scfgextvaluetext);
						settingsYpos += 12;
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "Locks the ARM9 SCFG_EXT,");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "avoiding conflict with recent libnds.");
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_lockarm9scfgexttext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, twlsettings_lockarm9scfgextvaluetext);
						settingsYpos += 12;
					}
				} else if (settings_subscreenmode == 2) {
					if (twlsettings_flashcardvalue == 0) {
						twlsettings_flashcardvaluetext1 = "DSTT";
						twlsettings_flashcardvaluetext2 = "R4i Gold";
						twlsettings_flashcardvaluetext3 = "R4i-SDHC (Non-v1.4.x version) (www.r4i-sdhc.com)";
						twlsettings_flashcardvaluetext4 = "R4 SDHC Dual-Core";
						twlsettings_flashcardvaluetext5 = "R4 SDHC Upgrade";
						twlsettings_flashcardvaluetext6 = "SuperCard DSONE";
					} else if (twlsettings_flashcardvalue == 1) {
						twlsettings_flashcardvaluetext1 = "Original R4";
						twlsettings_flashcardvaluetext2 = "M3 Simply";
						twlsettings_flashcardvaluetext3 = " ";
						twlsettings_flashcardvaluetext4 = " ";
						twlsettings_flashcardvaluetext5 = " ";
						twlsettings_flashcardvaluetext6 = " ";
					} else if (twlsettings_flashcardvalue == 2) {
						twlsettings_flashcardvaluetext1 = "R4iDSN";
						twlsettings_flashcardvaluetext2 = "R4i Gold RTS";
						twlsettings_flashcardvaluetext3 = " ";
						twlsettings_flashcardvaluetext4 = " ";
						twlsettings_flashcardvaluetext5 = " ";
						twlsettings_flashcardvaluetext6 = " ";
					} else if (twlsettings_flashcardvalue == 3) {
						twlsettings_flashcardvaluetext1 = "Acekard 2(i)";
						twlsettings_flashcardvaluetext2 = "M3DS Real";
						twlsettings_flashcardvaluetext3 = " ";
						twlsettings_flashcardvaluetext4 = " ";
						twlsettings_flashcardvaluetext5 = " ";
						twlsettings_flashcardvaluetext6 = " ";
					} else if (twlsettings_flashcardvalue == 4) {
						twlsettings_flashcardvaluetext1 = "Acekard RPG";
						twlsettings_flashcardvaluetext2 = " ";
						twlsettings_flashcardvaluetext3 = " ";
						twlsettings_flashcardvaluetext4 = " ";
						twlsettings_flashcardvaluetext5 = " ";
						twlsettings_flashcardvaluetext6 = " ";
					} else if (twlsettings_flashcardvalue == 5) {
						twlsettings_flashcardvaluetext1 = "Ace 3DS+";
						twlsettings_flashcardvaluetext2 = "Gateway Blue Card";
						twlsettings_flashcardvaluetext3 = "R4iTT";
						twlsettings_flashcardvaluetext4 = " ";
						twlsettings_flashcardvaluetext5 = " ";
						twlsettings_flashcardvaluetext6 = " ";
					} else if (twlsettings_flashcardvalue == 6) {
						twlsettings_flashcardvaluetext1 = "SuperCard DSTWO";
						twlsettings_flashcardvaluetext2 = " ";
						twlsettings_flashcardvaluetext3 = " ";
						twlsettings_flashcardvaluetext4 = " ";
						twlsettings_flashcardvaluetext5 = " ";
						twlsettings_flashcardvaluetext6 = " ";
					}
					settingstext_bot = twlsettings_flashcardtext;
					settingsYpos = 40;
					sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_flashcardvaluetext1);
					settingsYpos += 12;
					sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_flashcardvaluetext2);
					settingsYpos += 12;
					sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_flashcardvaluetext3);
					settingsYpos += 12;
					sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_flashcardvaluetext4);
					settingsYpos += 12;
					sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_flashcardvaluetext5);
					settingsYpos += 12;
					sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, twlsettings_flashcardvaluetext6);
					sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, settings_lrpicktext);
					sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, settings_absavereturn);
				}
				sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 16, settingstext_bot);
			}
		sf2d_draw_rectangle(0, 0, 320, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect

		sf2d_end_frame();
		}
		
		sf2d_swapbuffers();


		if (titleboxXmovetimer == 0) {
			updatebotscreen = false;
		}
		if (screenmode == 0) {
			if (romselect_toplayout == 1) {
				Lshouldertext = Lshouldertext_showboxart;
			} else {
				Lshouldertext = Lshouldertext_hideboxart;
			}
			if (twlsettings_forwardervalue == 1) {
				Rshouldertext = Rshouldertext_sdcard;
			} else {
				Rshouldertext = Rshouldertext_flashcard;
			}
			/* if (i == 0) {	// If no ROMs are found
				romselect_layout = 1;
				updatebotscreen = true;
			} */
			if(hDown & KEY_L) {
				if (romselect_toplayout == 1) {
					romselect_toplayout = 0;
				} else {
					romselect_toplayout = 1;
				}
				sfx_switch.stop();	// Prevent freezing
				sfx_switch.play();
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
					rom = (char*)(files.at(cursorPosition)).c_str();
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
					screenmode = 1;
					updatebotscreen = true;
				}
			} else { */
				startbordermovepos = 0;
				startborderscalesize = 1.0;
				if(applaunchprep == false) {
					if(hDown & KEY_R) {
						cursorPosition = 0;
						titleboxXmovepos = 0;
						boxartXmovepos = 0;
						noromsfound = false;
						if (twlsettings_forwardervalue == 1) {
							twlsettings_forwardervalue = 0;
						} else {
							twlsettings_forwardervalue = 1;
						}
						sfx_switch.stop();	// Prevent freezing
						sfx_switch.play();
						updatebotscreen = true;
					}
					if (noromsfound == false) {
						noromsfound = true;
						if (i == 0) {	// If no ROMs are found
							cursorPosition = -1;
							titleboxXmovepos = +64;
							boxartXmovepos = 0;
							updatebotscreen = true;
						}
					}
					if(hDown & KEY_A){
						if (titleboxXmovetimer == 0) {
							if(cursorPosition == -2) {
								titleboxXmovetimer = 1;
								screenmodeswitch = true;
								applaunchprep = true;
							} else if(cursorPosition == -1) {
								titleboxXmovetimer = 1;
								twlsettings_launchslot1value = 1;
								applaunchprep = true;
							} else {
								titleboxXmovetimer = 1;
								if (twlsettings_forwardervalue == 1) {
									twlsettings_launchslot1value = 1;
									rom = (char*)(fcfiles.at(cursorPosition)).c_str();
								} else {
									twlsettings_launchslot1value = 0;
									rom = (char*)(files.at(cursorPosition)).c_str();
								}
								applaunchprep = true;
							}
						}
						updatebotscreen = true;
						//bgm_menu.stop();
						sfx_launch.play();
					} else if((hHeld & KEY_RIGHT) && cursorPosition != i && i != 0 ){
						//titleboxXmovepos -= 64;
						if (titleboxXmoveleft == false) {
							titleboxXmoveright = true;
						}
						updatebotscreen = true;
					} else if(hHeld & KEY_LEFT){
						//titleboxXmovepos += 64;
						if (titleboxXmoveright == false) {
							titleboxXmoveleft = true;
						}
						updatebotscreen = true;
					} /* else if (hDown & KEY_SELECT) {
						screenmode = 1;
						updatebotscreen = true;
					} */
				}
			//}
		} else if (screenmode == 1) {
			Lshouldertext = "GUI";
			Rshouldertext = "NTR/TWL";
			updatebotscreen = true;
			if (settings_subscreenmode == 2) {
				if(hDown & KEY_LEFT && twlsettings_flashcardvalue != 0){
					twlsettings_flashcardvalue--; // Flashcard
					sfx_select.play();
				} else if(hDown & KEY_RIGHT && twlsettings_flashcardvalue != 6){
					twlsettings_flashcardvalue++; // Flashcard
					sfx_select.play();
				} else if(hDown & KEY_A || hDown & KEY_B){
					settings_subscreenmode = 1;
					sfx_select.play();
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
						twlsettings_bootscreenvalue++; // Boot screen
						if(twlsettings_bootscreenvalue == 2) {
							twlsettings_bootscreenvalue = 0;
						}
					} else if (twlsettingscursorPosition == 4) {
						twlsettings_healthsafetyvalue++; // H&S message
						if(twlsettings_healthsafetyvalue == 2) {
							twlsettings_healthsafetyvalue = 0;
						}
					} else if (twlsettingscursorPosition == 5) {
						twlsettings_resetslot1value++; // Reset Slot-1
						if(twlsettings_resetslot1value == 2) {
							twlsettings_resetslot1value = 0;
						}
					} else if (twlsettingscursorPosition == 6) {
						twlsettings_consolevalue++; // Console output
						if(twlsettings_consolevalue == 3) {
							twlsettings_consolevalue = 0;
						}
					} else if (twlsettingscursorPosition == 7) {
						twlsettings_lockarm9scfgextvalue++; // Lock ARM9 SCFG_EXT
						if(twlsettings_lockarm9scfgextvalue == 2) {
							twlsettings_lockarm9scfgextvalue = 0;
						}
					}
					sfx_select.play();
				} else if((hDown & KEY_DOWN) && twlsettingscursorPosition != 7){
					twlsettingscursorPosition++;
					sfx_select.play();
				} else if((hDown & KEY_UP) && twlsettingscursorPosition != 0){
					twlsettingscursorPosition--;
					sfx_select.play();
				} else if(hDown & KEY_L){
					settings_subscreenmode = 0;
					sfx_switch.stop();	// Prevent freezing
					sfx_switch.play();
				} else if(hDown & KEY_B){
					titleboxXmovetimer = 1;
					fadeout = true;
					//bgm_settings.stop();
					sfx_back.play();
				}
			} else {
				if(hDown & KEY_A || hDown & KEY_RIGHT){
					if (settingscursorPosition == 0) {
						settings_colorvalue++; // Color
						if(settings_colorvalue == 18) {
							settings_colorvalue = 0;
						}
						LoadColor();
					} else if (settingscursorPosition == 1) {
						settings_topbordervalue++; // Top border
						if(settings_topbordervalue == 2) {
							settings_topbordervalue = 0;
						}
					}
					sfx_select.play();
				} if(hDown & KEY_LEFT){
					if (settingscursorPosition == 0) {
						settings_colorvalue--; // Color
						if(settings_colorvalue == -1) {
							settings_colorvalue = 17;
						}
						LoadColor();
						sfx_select.play();
					} 
				} else if((hDown & KEY_DOWN) && settingscursorPosition != 1){
					settingscursorPosition++;
					sfx_select.play();
				} else if((hDown & KEY_UP) && settingscursorPosition != 0){
					settingscursorPosition--;
					sfx_select.play();
				} else if(hDown & KEY_R){
					settings_subscreenmode = 1;
					sfx_switch.stop();	// Prevent freezing
					sfx_switch.play();
				} else if(hDown & KEY_B){
					titleboxXmovetimer = 1;
					fadeout = true;
					//bgm_settings.stop();
					sfx_back.play();
				}
			}
		}

		while(applaunchon){
			// Prepare for the app launch
			APT_PrepareToDoApplicationJump(0, 0x0004800554574C44LL, 0); // TWL app's title ID
			// Tell APT to trigger the app launch and set the status of this app to exit
			APT_DoApplicationJump(param, sizeof(param), hmac);
		}
	}

	
	SaveSettings();
	hidExit();
	srvExit();
	romfsExit();
	sdmcExit();
	aptExit();
	if (colortexloaded == true) { sf2d_free_texture(topbgtex); }
	sf2d_free_texture(toptex);
	sf2d_free_texture(shoulderLtex);
	sf2d_free_texture(shoulderRtex);
	sf2d_free_texture(batterychrgtex);
	sf2d_free_texture(battery0tex);
	sf2d_free_texture(battery1tex);
	sf2d_free_texture(battery2tex);
	sf2d_free_texture(battery3tex);
	sf2d_free_texture(battery4tex);
	sf2d_free_texture(battery5tex);
	sf2d_free_texture(setbatterychrgtex);
	sf2d_free_texture(setbattery0tex);
	sf2d_free_texture(setbattery1tex);
	sf2d_free_texture(setbattery2tex);
	sf2d_free_texture(setbattery3tex);
	sf2d_free_texture(setbattery4tex);
	sf2d_free_texture(setbattery5tex);
	sf2d_free_texture(bottomtex);
	sf2d_free_texture(iconunktex);
	sf2d_free_texture(homeicontex);
	sf2d_free_texture(whomeicontex);
	sf2d_free_texture(bottomlogotex);
	sf2d_free_texture(bottomcovertex);
	sf2d_free_texture(bubbletex);
	sf2d_free_texture(settingsboxtex);
	sf2d_free_texture(carttex);
	sf2d_free_texture(boxfulltex);
	if (colortexloaded == true) { sf2d_free_texture(startbordertex); }
	sf2d_free_texture(settingstex);
	sf2d_free_texture(dsboottex);
	sf2d_free_texture(dsiboottex);
	sf2d_free_texture(dshstex);
	sf2d_free_texture(dsihstex);
	sf2d_free_texture(whitescrtex);
	sf2d_free_texture(disabledtex);
	FreeBoxArtTextures();
	ndspExit();
	sf2d_fini();

    return 0;
}