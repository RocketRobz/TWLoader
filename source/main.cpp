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
#include "date.h"

#define CONFIG_3D_SLIDERSTATE (*(float *)0x1FF81080)


u32 kDown;
u32 kHeld;

CIniFile settingsini( "sdmc:/_nds/twloader/settings.ini" );	
CIniFile bootstrapini( "sdmc:/_nds/nds-bootstrap.ini" );

int color_Rvalue;
int color_Gvalue;
int color_Bvalue;
	
sf2d_texture *boxarttexnum;
#include "boxart.h"
int boxartnum = 0;
const char* boxartpath;
const char* boxartfile;
const char* boxartfile_fullpath;
const char* topbgloc;
const char* startborderloc;


const char* fcrompathini_flashcardrom = "FLASHCARD-ROM";
const char* fcrompathini_rompath = "NDS_PATH";
	

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

const char* noromtext1;
const char* noromtext2;

const char* batterytext;

// Settings text
const char* settings_vertext = "Ver. 1.4.2";

const char* settingstext_bot;

const char* settings_colortext = "Color";
const char* settings_topbordertext = "Top border";

const char* settings_colorvaluetext;
const char* settings_topbordervaluetext;
// End

int settings_colorvalue;
int settings_topbordervalue;

int romselect_toplayout;
// int romselect_layout;
// 0: File browser (Text only)
// 1: DSi Menu

char* rom = (char*)malloc(256);
std::string fat = "fat:/nds/";
std::string slashchar = "/";
std::string woodfat = "fat0:/";
std::string dstwofat = "fat1:/";
std::string flashcardfolder = "sdmc:/nds/flashcard/";
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
    
    //marcus@Werkstaetiun:/media/marcus/WESTERNDIGI/dev_threedee/MCU_examples/RGB_rave$ lua assets/colorgen.lua
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
		topbgloc = "romfs:/assets/topbg/0-gray.png";
		startborderloc = "romfs:/assets/start_border/0-gray.png";
		color_Rvalue = 99;
		color_Gvalue = 127;
		color_Bvalue = 127;
	} else if (settings_colorvalue == 1) {
		topbgloc = "romfs:/assets/topbg/1-brown.png";
		startborderloc = "romfs:/assets/start_border/1-brown.png";
		color_Rvalue = 139;
		color_Gvalue = 99;
		color_Bvalue = 0;
	} else if (settings_colorvalue == 2) {
		topbgloc = "romfs:/assets/topbg/2-red.png";
		startborderloc = "romfs:/assets/start_border/2-red.png";
		color_Rvalue = 255;
		color_Gvalue = 0;
		color_Bvalue = 0;
	} else if (settings_colorvalue == 3) {
		topbgloc = "romfs:/assets/topbg/3-pink.png";
		startborderloc = "romfs:/assets/start_border/3-pink.png";
		color_Rvalue = 255;
		color_Gvalue = 127;
		color_Bvalue = 127;
	} else if (settings_colorvalue == 4) {
		topbgloc = "romfs:/assets/topbg/4-orange.png";
		startborderloc = "romfs:/assets/start_border/4-orange.png";
		color_Rvalue = 169;
		color_Gvalue = 31;
		color_Bvalue = 0;
	} else if (settings_colorvalue == 5) {
		topbgloc = "romfs:/assets/topbg/5-yellow.png";
		startborderloc = "romfs:/assets/start_border/5-yellow.png";
		color_Rvalue = 215;
		color_Gvalue = 215;
		color_Bvalue = 0;
	} else if (settings_colorvalue == 6) {
		topbgloc = "romfs:/assets/topbg/6-yellowgreen.png";
		startborderloc = "romfs:/assets/start_border/6-yellowgreen.png";
		color_Rvalue = 215;
		color_Gvalue = 255;
		color_Bvalue = 0;
	} else if (settings_colorvalue == 7) {
		topbgloc = "romfs:/assets/topbg/7-green1.png";
		startborderloc = "romfs:/assets/start_border/7-green1.png";
		color_Rvalue = 0;
		color_Gvalue = 255;
		color_Bvalue = 0;
	} else if (settings_colorvalue == 8) {
		topbgloc = "romfs:/assets/topbg/8-green2.png";
		startborderloc = "romfs:/assets/start_border/8-green2.png";
		color_Rvalue = 63;
		color_Gvalue = 255;
		color_Bvalue = 63;
	} else if (settings_colorvalue == 9) {
		topbgloc = "romfs:/assets/topbg/9-lightgreen.png";
		startborderloc = "romfs:/assets/start_border/9-lightgreen.png";
		color_Rvalue = 31;
		color_Gvalue = 231;
		color_Bvalue = 31;
	} else if (settings_colorvalue == 10) {
		topbgloc = "romfs:/assets/topbg/10-skyblue.png";
		startborderloc = "romfs:/assets/start_border/10-skyblue.png";
		color_Rvalue = 0;
		color_Gvalue = 63;
		color_Bvalue = 255;
	} else if (settings_colorvalue == 11) {
		topbgloc = "romfs:/assets/topbg/11-lightblue.png";
		startborderloc = "romfs:/assets/start_border/11-lightblue.png";
		color_Rvalue = 63;
		color_Gvalue = 63;
		color_Bvalue = 255;
	} else if (settings_colorvalue == 12) {
		topbgloc = "romfs:/assets/topbg/12-blue.png";
		startborderloc = "romfs:/assets/start_border/12-blue.png";
		color_Rvalue = 0;
		color_Gvalue = 0;
		color_Bvalue = 255;
	} else if (settings_colorvalue == 13) {
		topbgloc = "romfs:/assets/topbg/13-violet.png";
		startborderloc = "romfs:/assets/start_border/13-violet.png";
		color_Rvalue = 127;
		color_Gvalue = 0;
		color_Bvalue = 255;
	} else if (settings_colorvalue == 14) {
		topbgloc = "romfs:/assets/topbg/14-purple.png";
		startborderloc = "romfs:/assets/start_border/14-purple.png";
		color_Rvalue = 255;
		color_Gvalue = 0;
		color_Bvalue = 255;
	} else if (settings_colorvalue == 15) {
		topbgloc = "romfs:/assets/topbg/15-fuschia.png";
		startborderloc = "romfs:/assets/start_border/15-fuschia.png";
		color_Rvalue = 255;
		color_Gvalue = 0;
		color_Bvalue = 127;
	} else if (settings_colorvalue == 16) {
		topbgloc = "romfs:/assets/topbg/16-red&blue.png";
		startborderloc = "romfs:/assets/start_border/16-red&blue.png";
		color_Rvalue = 255;
		color_Gvalue = 0;
		color_Bvalue = 255;
	} else if (settings_colorvalue == 17) {
		topbgloc = "romfs:/assets/topbg/17-green&yellow.png";
		startborderloc = "romfs:/assets/start_border/17-green&yellow.png";
		color_Rvalue = 215;
		color_Gvalue = 215;
		color_Bvalue = 0;
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

void LoadBoxArt() {
	if (boxartnum == 0) {
		boxarttex1 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 1) {
		boxarttex2 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 2) {
		boxarttex3 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 3) {
		boxarttex4 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 4) {
		boxarttex5 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 5) {
		boxarttex6 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 6) {
		boxarttex7 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 7) {
		boxarttex8 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 8) {
		boxarttex9 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 9) {
		boxarttex10 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 10) {
		boxarttex11 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 11) {
		boxarttex12 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 12) {
		boxarttex13 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 13) {
		boxarttex14 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 14) {
		boxarttex15 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 15) {
		boxarttex16 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 16) {
		boxarttex17 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 17) {
		boxarttex18 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 18) {
		boxarttex19 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 19) {
		boxarttex20 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 20) {
		boxarttex21 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 21) {
		boxarttex22 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 22) {
		boxarttex23 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 23) {
		boxarttex24 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 24) {
		boxarttex25 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 25) {
		boxarttex26 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 26) {
		boxarttex27 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 27) {
		boxarttex28 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 28) {
		boxarttex29 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 29) {
		boxarttex30 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 30) {
		boxarttex31 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 31) {
		boxarttex32 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 32) {
		boxarttex33 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 33) {
		boxarttex34 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 34) {
		boxarttex35 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 35) {
		boxarttex36 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 36) {
		boxarttex37 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 37) {
		boxarttex38 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 38) {
		boxarttex39 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 39) {
		boxarttex40 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 40) {
		boxarttex41 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 41) {
		boxarttex42 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 42) {
		boxarttex43 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 43) {
		boxarttex44 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 44) {
		boxarttex45 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 45) {
		boxarttex46 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 46) {
		boxarttex47 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 47) {
		boxarttex48 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 48) {
		boxarttex49 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 49) {
		boxarttex50 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	}
}

void LoadFlashcardBoxArt() {
	if (boxartnum == 0) {
		fcboxarttex1 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 1) {
		fcboxarttex2 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 2) {
		fcboxarttex3 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 3) {
		fcboxarttex4 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 4) {
		fcboxarttex5 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 5) {
		fcboxarttex6 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 6) {
		fcboxarttex7 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 7) {
		fcboxarttex8 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 8) {
		fcboxarttex9 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 9) {
		fcboxarttex10 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 10) {
		fcboxarttex11 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 11) {
		fcboxarttex12 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 12) {
		fcboxarttex13 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 13) {
		fcboxarttex14 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 14) {
		fcboxarttex15 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 15) {
		fcboxarttex16 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 16) {
		fcboxarttex17 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 17) {
		fcboxarttex18 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 18) {
		fcboxarttex19 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 19) {
		fcboxarttex20 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 20) {
		fcboxarttex21 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 21) {
		fcboxarttex22 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 22) {
		fcboxarttex23 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 23) {
		fcboxarttex24 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 24) {
		fcboxarttex25 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 25) {
		fcboxarttex26 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 26) {
		fcboxarttex27 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 27) {
		fcboxarttex28 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 28) {
		fcboxarttex29 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 29) {
		fcboxarttex30 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 30) {
		fcboxarttex31 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 31) {
		fcboxarttex32 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 32) {
		fcboxarttex33 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 33) {
		fcboxarttex34 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 34) {
		fcboxarttex35 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 35) {
		fcboxarttex36 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 36) {
		fcboxarttex37 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 37) {
		fcboxarttex38 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 38) {
		fcboxarttex39 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 39) {
		fcboxarttex40 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 40) {
		fcboxarttex41 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 41) {
		fcboxarttex42 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 42) {
		fcboxarttex43 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 43) {
		fcboxarttex44 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 44) {
		fcboxarttex45 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 45) {
		fcboxarttex46 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 46) {
		fcboxarttex47 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 47) {
		fcboxarttex48 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 48) {
		fcboxarttex49 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	} else if (boxartnum == 49) {
		fcboxarttex50 = sfil_load_PNG_file(boxartpath, SF2D_PLACE_RAM); // Box art
	}
}

void FreeBoxArtTextures() {
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
    sf2d_set_clear_color(RGBA8(0x00, 0x00, 0x00, 0x00));
	sf2d_set_3D(1);
	
	// Font loading
	sftd_init();
	sftd_font *font = sftd_load_font_file("romfs:/font.ttf");

	LoadSettings();

	sf2d_texture *toptex = sfil_load_PNG_file("romfs:/assets/top.png", SF2D_PLACE_RAM); // Top DSi-Menu border
	LoadColor();
	sf2d_texture *topbgtex = sfil_load_PNG_file(topbgloc, SF2D_PLACE_RAM); // Top background, behind the DSi-Menu border
	//sf2d_texture *vol0tex = sfil_load_PNG_file("romfs:/assets/volume0.png", SF2D_PLACE_RAM);
	//sf2d_texture *vol1tex = sfil_load_PNG_file("romfs:/assets/volume1.png", SF2D_PLACE_RAM);
	//sf2d_texture *vol2tex = sfil_load_PNG_file("romfs:/assets/volume2.png", SF2D_PLACE_RAM);
	//sf2d_texture *vol3tex = sfil_load_PNG_file("romfs:/assets/volume3.png", SF2D_PLACE_RAM);
	sf2d_texture *shoulderLtex = sfil_load_PNG_file("romfs:/assets/shoulder_L.png", SF2D_PLACE_RAM); // L shoulder
	sf2d_texture *shoulderRtex = sfil_load_PNG_file("romfs:/assets/shoulder_R.png", SF2D_PLACE_RAM); // R shoulder
	sf2d_texture *batterychrgtex = sfil_load_PNG_file("romfs:/assets/battery_charging.png", SF2D_PLACE_RAM);
	sf2d_texture *battery0tex = sfil_load_PNG_file("romfs:/assets/battery0.png", SF2D_PLACE_RAM);
	sf2d_texture *battery1tex = sfil_load_PNG_file("romfs:/assets/battery1.png", SF2D_PLACE_RAM);
	sf2d_texture *battery2tex = sfil_load_PNG_file("romfs:/assets/battery2.png", SF2D_PLACE_RAM);
	sf2d_texture *battery3tex = sfil_load_PNG_file("romfs:/assets/battery3.png", SF2D_PLACE_RAM);
	sf2d_texture *battery4tex = sfil_load_PNG_file("romfs:/assets/battery4.png", SF2D_PLACE_RAM);
	sf2d_texture *battery5tex = sfil_load_PNG_file("romfs:/assets/battery5.png", SF2D_PLACE_RAM);
	sf2d_texture *setbatterychrgtex = sfil_load_PNG_file("romfs:/assets/settings/battery_charging.png", SF2D_PLACE_RAM);
	sf2d_texture *setbattery0tex = sfil_load_PNG_file("romfs:/assets/settings/battery0.png", SF2D_PLACE_RAM);
	sf2d_texture *setbattery1tex = sfil_load_PNG_file("romfs:/assets/settings/battery1.png", SF2D_PLACE_RAM);
	sf2d_texture *setbattery2tex = sfil_load_PNG_file("romfs:/assets/settings/battery2.png", SF2D_PLACE_RAM);
	sf2d_texture *setbattery3tex = sfil_load_PNG_file("romfs:/assets/settings/battery3.png", SF2D_PLACE_RAM);
	sf2d_texture *setbattery4tex = sfil_load_PNG_file("romfs:/assets/settings/battery4.png", SF2D_PLACE_RAM);
	sf2d_texture *setbattery5tex = sfil_load_PNG_file("romfs:/assets/settings/battery5.png", SF2D_PLACE_RAM);
	sf2d_texture *bottomtex = sfil_load_PNG_file("romfs:/assets/bottom.png", SF2D_PLACE_RAM); // Bottom of menu
	sf2d_texture *iconunktex = sfil_load_PNG_file("romfs:/assets/icon_unknown.png", SF2D_PLACE_RAM); // Bottom of menu
	sf2d_texture *homeicontex = sfil_load_PNG_file("romfs:/assets/homeicon.png", SF2D_PLACE_RAM); // HOME icon
	sf2d_texture *whomeicontex = sfil_load_PNG_file("romfs:/assets/whomeicon.png", SF2D_PLACE_RAM); // HOME icon (Settings)
	sf2d_texture *bottomlogotex = sfil_load_PNG_file("romfs:/assets/bottom_logo.png", SF2D_PLACE_RAM); // TWLoader logo on bottom screen
	sf2d_texture *bottomcovertex = sfil_load_PNG_file("romfs:/assets/bottom_cover.png", SF2D_PLACE_RAM); // Image to cover selected game/app
	sf2d_texture *startbordertex = sfil_load_PNG_file(startborderloc, SF2D_PLACE_RAM); // "START" border
	sf2d_texture *settingsboxtex = sfil_load_PNG_file("romfs:/assets/settingsbox.png", SF2D_PLACE_RAM); // Settings box on bottom screen
	sf2d_texture *carttex = sfil_load_PNG_file("romfs:/assets/cart.png", SF2D_PLACE_RAM); // Cartridge on bottom screen
	sf2d_texture *boxfulltex = sfil_load_PNG_file("romfs:/assets/box_full.png", SF2D_PLACE_RAM); // (DSiWare) box on bottom screen
	sf2d_texture *bubbletex = sfil_load_PNG_file("romfs:/assets/bubble.png", SF2D_PLACE_RAM); // Text bubble
	sf2d_texture *settingstex = sfil_load_PNG_file("romfs:/assets/settings/screen.png", SF2D_PLACE_RAM); // Bottom of settings screen
	sf2d_texture *settingslogotex = sfil_load_PNG_file("romfs:/assets/settings/logo.png", SF2D_PLACE_RAM); // TWLoader logo on bottom screen
	sf2d_texture *dsboottex = sfil_load_PNG_file("romfs:/assets/settings/dsboot.png", SF2D_PLACE_RAM); // DS boot screen in settings
	sf2d_texture *dsiboottex = sfil_load_PNG_file("romfs:/assets/settings/dsiboot.png", SF2D_PLACE_RAM); // DSi boot screen in settings
	sf2d_texture *dshstex = sfil_load_PNG_file("romfs:/assets/settings/dshs.png", SF2D_PLACE_RAM); // DS H&S screen in settings
	sf2d_texture *dsihstex = sfil_load_PNG_file("romfs:/assets/settings/dsihs.png", SF2D_PLACE_RAM); // DSi H&S screen in settings
	sf2d_texture *whitescrtex = sfil_load_PNG_file("romfs:/assets/settings/whitescr.png", SF2D_PLACE_RAM); // White screen in settings
	sf2d_texture *disabledtex = sfil_load_PNG_file("romfs:/assets/settings/disable.png", SF2D_PLACE_RAM);

	std::vector<std::string> files = {};
	std::vector<std::string> fcfiles = {};
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
	DIR *boxartdir;
	DIR *fcboxartdir;
	struct dirent *ent;
	
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

	for(boxartnum = 0; boxartnum < boxartfiles.size(); boxartnum++) {
		boxartfile_fullpath = malloc(256);
		boxartfile = boxartfiles.at(boxartnum).c_str();
		strcat(boxartfile_fullpath, boxartfolder);
		strcat(boxartfile_fullpath, boxartfile);
		
		/* if (boxartnum == 3) {	// Fix box art displaying "?".
			if (fopen (boxartfile_fullpath, "rb") != NULL) {
				boxartpath = boxartfile_fullpath;
			} else {
				boxartpath = "romfs:/assets/boxart_unknown.png";	// Prevent crashing
			} fclose (fopen (boxartfile_fullpath, "rb"));
		} else { */
			if (fopen (boxartfile_fullpath+3, "rb") != NULL) {
				boxartpath = boxartfile_fullpath+3;
			} else {
				boxartpath = "romfs:/assets/boxart_unknown.png";	// Prevent crashing
			} fclose (fopen (boxartfile_fullpath+3, "rb"));
		//}
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

	for(boxartnum = 0; boxartnum < fcboxartfiles.size(); boxartnum++) {
		boxartfile_fullpath = malloc(256);
		boxartfile = fcboxartfiles.at(boxartnum).c_str();
		strcat(boxartfile_fullpath, fcboxartfolder);
		strcat(boxartfile_fullpath, boxartfile);

		/* if (boxartnum == 3) {	// Fix box art displaying "?".
			if (fopen (boxartfile_fullpath, "rb") != NULL) {
				boxartpath = boxartfile_fullpath;
			} else {
				boxartpath = "romfs:/assets/boxart_unknown.png";	// Prevent crashing
			} fclose (fopen (boxartfile_fullpath, "rb"));
		} else { */
			if (fopen (boxartfile_fullpath+3, "rb") != NULL) {
				boxartpath = boxartfile_fullpath+3;
			} else {
				boxartpath = "romfs:/assets/boxart_unknown.png";	// Prevent crashing
			} fclose (fopen (boxartfile_fullpath+3, "rb"));
		//}
		LoadBoxArt();
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
			
	const char* Lshouldertext;
	const char* Rshouldertext;
			
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
					boxartXpos = 136;
					if (twlsettings_forwardervalue == 1) {
						for(boxartnum = 0; boxartnum < fcboxartfiles.size(); boxartnum++) {
							ChangeBoxArtNo();
							sf2d_draw_texture(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
							sf2d_draw_texture_scale_blend(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 0xC0)); // Draw box art's reflection
							boxartXpos += 144;
						}
					} else {
						for(boxartnum = 0; boxartnum < boxartfiles.size(); boxartnum++) {
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
					boxartXpos = 136;
					if (twlsettings_forwardervalue == 1) {
						for(boxartnum = 0; boxartnum < fcboxartfiles.size(); boxartnum++) {
							ChangeBoxArtNo();
							sf2d_draw_texture(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
							sf2d_draw_texture_scale_blend(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 0xC0)); // Draw box art's reflection
							boxartXpos += 144;
						}
					} else {
						for(boxartnum = 0; boxartnum < boxartfiles.size(); boxartnum++) {
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
				screenmode = 0;
				fadeout = false;
				fadein = true;
			}
		}

		if(titleboxXmoveleft == true) {
			titleboxXmovetimer += 1;
			if (titleboxXmovetimer == 10) {
				titleboxXmovetimer = 0;
				titleboxXmoveleft = false;
			} else if (titleboxXmovetimer == 9) {
				// Delay a frame
			} else if (titleboxXmovetimer == 8) {
				titleboxXmovepos += 8;
				boxartXmovepos += 18;
				startbordermovepos = 1;
				startborderscalesize = 0.97;
				cursorPositionset = false;
			} else {
				if (cursorPositionset == false) {
					cursorPosition--;
					cursorPositionset = true;
				}
				titleboxXmovepos += 8;
				boxartXmovepos += 18;
			}
		} else if(titleboxXmoveright == true) {
			titleboxXmovetimer += 1;
			if (titleboxXmovetimer == 10) {
				titleboxXmovetimer = 0;
				titleboxXmoveright = false;
			} else if (titleboxXmovetimer == 9) {
				// Delay a frame
			} else if (titleboxXmovetimer == 8) {
				titleboxXmovepos -= 8;
				boxartXmovepos -= 18;
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
					boxartXmovepos -= 18;
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
			ndsiconYmovepos -= 6;
			if (titleboxYmovepos == -240) {
				if(screenmodeswitch == true) {
					screenmode = 1;
					titleboxYmovepos = 120;
					ndsiconYmovepos = 133;
					fadein = true;
					screenmodeswitch = false;
					applaunchprep = false;
				} else {
					screenoff();
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
					} else {
						sf2d_draw_texture(bottomlogotex, 320/2 - bottomlogotex->width/2, 40);
					}
					sf2d_draw_texture(homeicontex, 79, 220); // Draw HOME icon
					sftd_draw_textf(font, 96, 220, RGBA8(0, 0, 0, 255), 14, ": Return to HOME Menu");
					sf2d_draw_texture(settingsboxtex, setsboxXpos+titleboxXmovepos, 119);
					sf2d_draw_texture(carttex, cartXpos+titleboxXmovepos, 120);
					sf2d_draw_texture(iconunktex, 16+cartXpos+titleboxXmovepos, 133);

					titleboxXpos = 128;
					ndsiconXpos = 144;
					filenameYpos = 0;
					if (titleboxXmovetimer == 0) {
						if (cursorPosition == -2) {
							sftd_draw_textf(font, 10, 8, RGBA8(127, 127, 127, 255), 12, "Settings");
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
								sf2d_draw_texture(iconunktex, ndsiconXpos+titleboxXmovepos, 133);
								titleboxXpos += 64;
								ndsiconXpos += 64;
								filenameYpos += 240;
							}
						} else {
							for(i = 0; i < fcfiles.size(); i++){
								if (titleboxXmovetimer == 0) {
									sftd_draw_textf(font, 10, 8+filenameYpos-240*cursorPosition, RGBA8(127, 127, 127, 255), 12, fcfiles.at(i).c_str());
								}
								sf2d_draw_texture(boxfulltex, titleboxXpos+titleboxXmovepos, 120);
								sf2d_draw_texture(iconunktex, ndsiconXpos+titleboxXmovepos, 133);
								titleboxXpos += 64;
								ndsiconXpos += 64;
								filenameYpos += 240;
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
					if (applaunchprep == false) {
						if (titleboxXmovetimer == 0) {
							startbordermovepos = 0;
							startborderscalesize = 1.0;
						}
						sf2d_draw_texture_scale(startbordertex, 128+startbordermovepos, 116+startbordermovepos, startborderscalesize, startborderscalesize);
						sftd_draw_textf(font, 141, 176, RGBA8(255, 255, 255, 255), 12, "START");
					} else {
						sf2d_draw_texture(bottomcovertex, 128, 116);  // Cover selected game/app
						if (cursorPosition == -2) {
							sf2d_draw_texture(settingsboxtex, 128, titleboxYmovepos); // Draw settings box that moves up
						} else if (cursorPosition == -1) {
							sf2d_draw_texture(carttex, 128, titleboxYmovepos); // Draw selected Slot-1 game that moves up
							sf2d_draw_texture(iconunktex, 144, ndsiconYmovepos);
						} else {
							sf2d_draw_texture(boxfulltex, 128, titleboxYmovepos); // Draw selected game/app that moves up
							sf2d_draw_texture(iconunktex, 144, ndsiconYmovepos);
						}
					}
				// }
			} else if(screenmode == 1) {
				sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				sf2d_draw_texture(settingstex, 0, 0);
				sf2d_draw_texture(whomeicontex, 79, 220); // Draw HOME icon
				sftd_draw_textf(font, 96, 220, RGBA8(255, 255, 255, 255), 14, ": Return to HOME Menu");
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
						twlsettings_flashcardvaluetext3 = "R4i-SDHC v1.4.x (www.r4i-sdhc.com)";
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
					sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "Left/Right: Pick");
					sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "A/B: Save and Return");
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
			Lshouldertext = "Layout";
			if (twlsettings_forwardervalue == 1) {
				Rshouldertext = "SD Card";
			} else {
				Rshouldertext = "Flashcard";
			}
			/* if (i == 0) {	// If no ROMs are found
				romselect_layout = 1;
				updatebotscreen = true;
			}
			if(hDown & KEY_L) {
				if (romselect_toplayout == 1) {
					romselect_toplayout = 0;
				} else {
					romselect_toplayout = 1;
				}
			}
			if (romselect_layout == 0) {
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
						updatebotscreen = true;
					}
					if (noromsfound == false) {
						if (i == 0) {	// If no ROMs are found
							noromsfound = true;
							titleboxXmovepos = +64;
							cursorPosition = -1;
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
					} else if((hHeld & KEY_RIGHT) && cursorPosition != i){
						//titleboxXmovepos -= 64;
						if (titleboxXmoveleft == false) {
							titleboxXmoveright = true;
						}
						updatebotscreen = true;
					} else if((hHeld & KEY_LEFT) && cursorPosition != -2){
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
				} else if(hDown & KEY_RIGHT && twlsettings_flashcardvalue != 6){
					twlsettings_flashcardvalue++; // Flashcard
				} else if(hDown & KEY_A || hDown & KEY_B){
					settings_subscreenmode = 1;
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
				} else if((hDown & KEY_DOWN) && twlsettingscursorPosition != 7){
					twlsettingscursorPosition++;
				} else if((hDown & KEY_UP) && twlsettingscursorPosition != 0){
					twlsettingscursorPosition--;
				} else if(hDown & KEY_L){
					settings_subscreenmode = 0;
				} else if(hDown & KEY_B){
					titleboxXmovetimer = 1;
					fadeout = true;
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
				} if(hDown & KEY_LEFT){
					if (settingscursorPosition == 0) {
						settings_colorvalue--; // Color
						if(settings_colorvalue == -1) {
							settings_colorvalue = 17;
						}
						LoadColor();
					} 
				} else if((hDown & KEY_DOWN) && settingscursorPosition != 1){
					settingscursorPosition++;
				} else if((hDown & KEY_UP) && settingscursorPosition != 0){
					settingscursorPosition--;
				} else if(hDown & KEY_R){
					settings_subscreenmode = 1;
				} else if(hDown & KEY_B){
					titleboxXmovetimer = 1;
					fadeout = true;
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
    sf2d_fini();

    return 0;
}