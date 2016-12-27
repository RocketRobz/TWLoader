#include <algorithm>
#include <string>
#include <vector>

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#include <sftd.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <citrus/app.hpp>
//#include <citrus/battery.hpp>
//#include <citrus/core.hpp>
//#include <citrus/fs.hpp>

#include "sound.h"
#include "inifile.h"
#include "date.h"

#define CONFIG_3D_SLIDERSTATE (*(float *)0x1FF81080)


touchPosition touch;
u32 kUp;
u32 kDown;
u32 kHeld;

CIniFile settingsini( "sdmc:/_nds/twloader/settings.ini" );	
CIniFile bootstrapini( "sdmc:/_nds/nds-bootstrap.ini" );
#include "ndsheaderbanner.h"

int color_Rvalue;
int color_Gvalue;
int color_Bvalue;

sf2d_texture *bnricontexnum;
sf2d_texture *boxarttexnum;
#include "bannerandboxart.h"
int bnriconnum = 0;
int bnriconframenum = 0;
int boxartnum = 0;
int pagenum = 0;
const char* tempfile_fullpath;
const char* tempimagepath;
const char* bnriconfile;
const char* boxartfile;
const char* topbgloc;
const char* dotcircleloc;
const char* startborderloc;
const char* musicpath = "romfs:/null.wav";


const char* fcrompathini_flashcardrom = "FLASHCARD-ROM";
const char* fcrompathini_rompath = "NDS_PATH";
const char* fcrompathini_bnriconaniseq = "BNR_ICONANISEQ";
const char* fcrompathini_bnrtext1 = "BNR_TEXT1";
const char* fcrompathini_bnrtext2 = "BNR_TEXT2";
const char* fcrompathini_bnrtext3 = "BNR_TEXT3";
	

// Settings .ini file
const char* settingsini_frontend = "FRONTEND";
//char* settingsini_frontend_twlappinstalled = "TWLAPP_INSTALLED";
const char* settingsini_frontend_color = "COLOR";
const char* settingsini_frontend_locswitch = "GAMELOC_SWITCH";
const char* settingsini_frontend_topborder = "TOP_BORDER";
const char* settingsini_frontend_toplayout = "TOP_LAYOUT";
const char* settingsini_frontend_counter = "COUNTER";
const char* settingsini_frontend_autoupdate = "AUTOUPDATE";
const char* settingsini_frontend_autodl = "AUTODOWNLOAD";
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

const char* Lshouldertext_boxart = "Box Art";
const char* Lshouldertext_blank = "Blank";
const char* Rshouldertext_sdcard = "SD Card";
const char* Rshouldertext_flashcard = "Flashcard";

const char* noromtext1;
const char* noromtext2;

const char* batterytext;

// Settings text
const char* settings_vertext = "Ver. 2.0.1";

const char* settingstext_bot;

const char* settings_colortext = "Color";
const char* settings_locswitchtext = "Game location switcher";
const char* settings_topbordertext = "Top border";
const char* settings_countertext = "Game counter";
const char* settings_autoupdatetext = "Auto-update bootstrap";
const char* settings_autodltext = "Auto-download latest TWLoader";

const char* settings_colorvaluetext;
const char* settings_locswitchvaluetext;
const char* settings_topbordervaluetext;
const char* settings_countervaluetext;
const char* settings_autoupdatevaluetext;
const char* settings_autodlvaluetext;

const char* settings_lrpicktext = "Left/Right: Pick";
const char* settings_absavereturn = "A/B: Save and Return";
// End

int settings_colorvalue;
int settings_locswitchvalue;
int settings_topbordervalue;
int settings_countervalue;
int settings_autoupdatevalue;
int settings_autodlvalue;

int romselect_toplayout;
//	0: Show box art
//	1: Hide box art

const char* romsel_filename;
std::string romsel_gameline1;
std::string romsel_gameline2;
std::string romsel_gameline3;
char *cstr1;
char *cstr2;
char *cstr3;
const char* romsel_counter1;
const char* romsel_counter2fc;
const char* romsel_counter2sd;

char* rom = (char*)malloc(256);
const char* flashcardrom;
std::string fat = "fat:/roms/nds/";
std::string slashchar = "/";
std::string woodfat = "fat0:/";
std::string dstwofat = "fat1:/";
std::string romfolder = "sdmc:/roms/nds/";
std::string flashcardfolder = "sdmc:/roms/flashcard/nds/";
const char* bnriconfolder = "sdmc:/_nds/twloader/bnricons/";
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
void downloadfile(const char* url, const char* file){
	acInit();
	httpcInit(0x1000);
	u32 wifistatuts;
	ACU_GetWifiStatus(&wifistatuts);
	if(wifistatuts > 0 && wifistatuts < 3){ //Checks if wifi is on
	u8 method = 0;
	httpcContext context;
	u32 statuscode=0;
	HTTPC_RequestMethod useMethod = HTTPC_METHOD_GET;

	if(method <= 3 && method >= 1) useMethod = (HTTPC_RequestMethod)method;

	do {
		if (statuscode >= 301 && statuscode <= 308) {
			char newurl[4096];
			httpcGetResponseHeader(&context, (char*)"Location", &newurl[0], 4096);
			url = &newurl[0];

			httpcCloseContext(&context);
		}

		Result ret = httpcOpenContext(&context, useMethod, (char*)url, 0);
		httpcSetSSLOpt(&context, SSLCOPT_DisableVerify);

		if(ret==0){
			httpcBeginRequest(&context);
			u32 contentsize=0;
			httpcGetResponseStatusCode(&context, &statuscode);
			if (statuscode == 200){
				u32 readSize = 0;
				long int bytesWritten = 0;
				u8* buf = (u8*)malloc(0x1000);
				memset(buf, 0, 0x1000);

				Handle fileHandle;
				FS_Path filePath=fsMakePath(PATH_ASCII, file);
				FSUSER_OpenFileDirectly(&fileHandle, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""), filePath, FS_OPEN_CREATE | FS_OPEN_WRITE, 0x00000000);

				do {
					ret = httpcDownloadData(&context, buf, 0x1000, &readSize);
					FSFILE_Write(fileHandle, NULL, bytesWritten, buf, readSize, 0x10001);
					bytesWritten += readSize;
				} while (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING);

				FSFILE_Close(fileHandle);
				svcCloseHandle(fileHandle);
				free(buf);
			}
		}
	} while ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308));
	httpcCloseContext(&context);
		
	}
	httpcExit();
	acExit();
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
		dotcircleloc = "romfs:/graphics/dotcircle/0-gray.png";
		startborderloc = "romfs:/graphics/start_border/0-gray.png";
		color_Rvalue = 99;
		color_Gvalue = 127;
		color_Bvalue = 127;
	} else if (settings_colorvalue == 1) {
		topbgloc = "romfs:/graphics/topbg/1-brown.png";
		dotcircleloc = "romfs:/graphics/dotcircle/1-brown.png";
		startborderloc = "romfs:/graphics/start_border/1-brown.png";
		color_Rvalue = 139;
		color_Gvalue = 99;
		color_Bvalue = 0;
	} else if (settings_colorvalue == 2) {
		topbgloc = "romfs:/graphics/topbg/2-red.png";
		dotcircleloc = "romfs:/graphics/dotcircle/2-red.png";
		startborderloc = "romfs:/graphics/start_border/2-red.png";
		color_Rvalue = 255;
		color_Gvalue = 0;
		color_Bvalue = 0;
	} else if (settings_colorvalue == 3) {
		topbgloc = "romfs:/graphics/topbg/3-pink.png";
		dotcircleloc = "romfs:/graphics/dotcircle/3-pink.png";
		startborderloc = "romfs:/graphics/start_border/3-pink.png";
		color_Rvalue = 255;
		color_Gvalue = 127;
		color_Bvalue = 127;
	} else if (settings_colorvalue == 4) {
		topbgloc = "romfs:/graphics/topbg/4-orange.png";
		dotcircleloc = "romfs:/graphics/dotcircle/4-orange.png";
		startborderloc = "romfs:/graphics/start_border/4-orange.png";
		color_Rvalue = 169;
		color_Gvalue = 31;
		color_Bvalue = 0;
	} else if (settings_colorvalue == 5) {
		topbgloc = "romfs:/graphics/topbg/5-yellow.png";
		dotcircleloc = "romfs:/graphics/dotcircle/5-yellow.png";
		startborderloc = "romfs:/graphics/start_border/5-yellow.png";
		color_Rvalue = 215;
		color_Gvalue = 215;
		color_Bvalue = 0;
	} else if (settings_colorvalue == 6) {
		topbgloc = "romfs:/graphics/topbg/6-yellowgreen.png";
		dotcircleloc = "romfs:/graphics/dotcircle/6-yellowgreen.png";
		startborderloc = "romfs:/graphics/start_border/6-yellowgreen.png";
		color_Rvalue = 215;
		color_Gvalue = 255;
		color_Bvalue = 0;
	} else if (settings_colorvalue == 7) {
		topbgloc = "romfs:/graphics/topbg/7-green1.png";
		dotcircleloc = "romfs:/graphics/dotcircle/7-green1.png";
		startborderloc = "romfs:/graphics/start_border/7-green1.png";
		color_Rvalue = 0;
		color_Gvalue = 255;
		color_Bvalue = 0;
	} else if (settings_colorvalue == 8) {
		topbgloc = "romfs:/graphics/topbg/8-green2.png";
		dotcircleloc = "romfs:/graphics/dotcircle/8-green2.png";
		startborderloc = "romfs:/graphics/start_border/8-green2.png";
		color_Rvalue = 63;
		color_Gvalue = 255;
		color_Bvalue = 63;
	} else if (settings_colorvalue == 9) {
		topbgloc = "romfs:/graphics/topbg/9-lightgreen.png";
		dotcircleloc = "romfs:/graphics/dotcircle/9-lightgreen.png";
		startborderloc = "romfs:/graphics/start_border/9-lightgreen.png";
		color_Rvalue = 31;
		color_Gvalue = 231;
		color_Bvalue = 31;
	} else if (settings_colorvalue == 10) {
		topbgloc = "romfs:/graphics/topbg/10-skyblue.png";
		dotcircleloc = "romfs:/graphics/dotcircle/10-skyblue.png";
		startborderloc = "romfs:/graphics/start_border/10-skyblue.png";
		color_Rvalue = 0;
		color_Gvalue = 63;
		color_Bvalue = 255;
	} else if (settings_colorvalue == 11) {
		topbgloc = "romfs:/graphics/topbg/11-lightblue.png";
		dotcircleloc = "romfs:/graphics/dotcircle/11-lightblue.png";
		startborderloc = "romfs:/graphics/start_border/11-lightblue.png";
		color_Rvalue = 63;
		color_Gvalue = 63;
		color_Bvalue = 255;
	} else if (settings_colorvalue == 12) {
		topbgloc = "romfs:/graphics/topbg/12-blue.png";
		dotcircleloc = "romfs:/graphics/dotcircle/12-blue.png";
		startborderloc = "romfs:/graphics/start_border/12-blue.png";
		color_Rvalue = 0;
		color_Gvalue = 0;
		color_Bvalue = 255;
	} else if (settings_colorvalue == 13) {
		topbgloc = "romfs:/graphics/topbg/13-violet.png";
		dotcircleloc = "romfs:/graphics/dotcircle/13-violet.png";
		startborderloc = "romfs:/graphics/start_border/13-violet.png";
		color_Rvalue = 127;
		color_Gvalue = 0;
		color_Bvalue = 255;
	} else if (settings_colorvalue == 14) {
		topbgloc = "romfs:/graphics/topbg/14-purple.png";
		dotcircleloc = "romfs:/graphics/dotcircle/14-purple.png";
		startborderloc = "romfs:/graphics/start_border/14-purple.png";
		color_Rvalue = 255;
		color_Gvalue = 0;
		color_Bvalue = 255;
	} else if (settings_colorvalue == 15) {
		topbgloc = "romfs:/graphics/topbg/15-fuschia.png";
		dotcircleloc = "romfs:/graphics/dotcircle/15-fuschia.png";
		startborderloc = "romfs:/graphics/start_border/15-fuschia.png";
		color_Rvalue = 255;
		color_Gvalue = 0;
		color_Bvalue = 127;
	} else if (settings_colorvalue == 16) {
		topbgloc = "romfs:/graphics/topbg/16-red&blue.png";
		dotcircleloc = "romfs:/graphics/dotcircle/16-red&blue.png";
		startborderloc = "romfs:/graphics/start_border/16-red&blue.png";
		color_Rvalue = 255;
		color_Gvalue = 0;
		color_Bvalue = 255;
	} else if (settings_colorvalue == 17) {
		topbgloc = "romfs:/graphics/topbg/17-green&yellow.png";
		dotcircleloc = "romfs:/graphics/dotcircle/17-green&yellow.png";
		startborderloc = "romfs:/graphics/start_border/17-green&yellow.png";
		color_Rvalue = 215;
		color_Gvalue = 215;
		color_Bvalue = 0;
	} else if (settings_colorvalue == 18) {
		topbgloc = "romfs:/graphics/topbg/18-christmas.png";
		dotcircleloc = "romfs:/graphics/dotcircle/18-christmas.png";
		startborderloc = "romfs:/graphics/start_border/18-christmas.png";
		color_Rvalue = 255;
		color_Gvalue = 255;
		color_Bvalue = 0;
	}
}

void ChangeBNRIconNo() {
	if ( bnriconnum == 0+pagenum*20 ||
		bnriconnum == 10+pagenum*20 /* ||
		bnriconnum == 20 ||
		bnriconnum == 30 ||
		bnriconnum == 40 ||
		bnriconnum == 50 ||
		bnriconnum == 60 ||
		bnriconnum == 70 ||
		bnriconnum == 80 ||
		bnriconnum == 90 */ ) {
		bnricontexnum = bnricontex1;
	} else if ( bnriconnum == 1+pagenum*20 ||
				bnriconnum == 11+pagenum*20 /* ||
				bnriconnum == 21 ||
				bnriconnum == 31 ||
				bnriconnum == 41 ||
				bnriconnum == 51 ||
				bnriconnum == 61 ||
				bnriconnum == 71 ||
				bnriconnum == 81 ||
				bnriconnum == 91 */ ) {
		bnricontexnum = bnricontex2;
	} else if ( bnriconnum == 2+pagenum*20 ||
				bnriconnum == 12+pagenum*20 /* ||
				bnriconnum == 22 ||
				bnriconnum == 32 ||
				bnriconnum == 42 ||
				bnriconnum == 52 ||
				bnriconnum == 62 ||
				bnriconnum == 72 ||
				bnriconnum == 82 ||
				bnriconnum == 92 */ ) {
		bnricontexnum = bnricontex3;
	} else if ( bnriconnum == 3+pagenum*20 ||
				bnriconnum == 13+pagenum*20 /* ||
				bnriconnum == 23 ||
				bnriconnum == 33 ||
				bnriconnum == 43 ||
				bnriconnum == 53 ||
				bnriconnum == 63 ||
				bnriconnum == 73 ||
				bnriconnum == 83 ||
				bnriconnum == 93 */ ) {
		bnricontexnum = bnricontex4;
	} else if ( bnriconnum == 4+pagenum*20 ||
				bnriconnum == 14+pagenum*20 /* ||
				bnriconnum == 24 ||
				bnriconnum == 34 ||
				bnriconnum == 44 ||
				bnriconnum == 54 ||
				bnriconnum == 64 ||
				bnriconnum == 74 ||
				bnriconnum == 84 ||
				bnriconnum == 94 */ ) {
		bnricontexnum = bnricontex5;
	} else if ( bnriconnum == 5+pagenum*20 ||
				bnriconnum == 15+pagenum*20 /* ||
				bnriconnum == 25 ||
				bnriconnum == 35 ||
				bnriconnum == 45 ||
				bnriconnum == 55 ||
				bnriconnum == 65 ||
				bnriconnum == 75 ||
				bnriconnum == 85 ||
				bnriconnum == 95 */ ) {
		bnricontexnum = bnricontex6;
	} else if ( bnriconnum == 6+pagenum*20 ||
				bnriconnum == 16+pagenum*20 /* ||
				bnriconnum == 26 ||
				bnriconnum == 36 ||
				bnriconnum == 46 ||
				bnriconnum == 56 ||
				bnriconnum == 66 ||
				bnriconnum == 76 ||
				bnriconnum == 86 ||
				bnriconnum == 96 */ ) {
		bnricontexnum = bnricontex7;
	} else if ( bnriconnum == 7+pagenum*20 ||
				bnriconnum == 17+pagenum*20 /* ||
				bnriconnum == 27 ||
				bnriconnum == 37 ||
				bnriconnum == 47 ||
				bnriconnum == 57 ||
				bnriconnum == 67 ||
				bnriconnum == 77 ||
				bnriconnum == 87 ||
				bnriconnum == 97 */ ) {
		bnricontexnum = bnricontex8;
	} else if ( bnriconnum == 8+pagenum*20 ||
				bnriconnum == 18+pagenum*20 /* ||
				bnriconnum == 28 ||
				bnriconnum == 38 ||
				bnriconnum == 48 ||
				bnriconnum == 58 ||
				bnriconnum == 68 ||
				bnriconnum == 78 ||
				bnriconnum == 88 ||
				bnriconnum == 98 */ ) {
		bnricontexnum = bnricontex9;
	} else if ( bnriconnum == 9+pagenum*20 ||
				bnriconnum == 19+pagenum*20 /* ||
				bnriconnum == 29 ||
				bnriconnum == 39 ||
				bnriconnum == 49 ||
				bnriconnum == 59 ||
				bnriconnum == 69 ||
				bnriconnum == 79 ||
				bnriconnum == 89 ||
				bnriconnum == 99 */ ) {
		bnricontexnum = bnricontex10;
	}
}

void ChangeBoxArtNo() {
	if ( boxartnum == 0+pagenum*20 ) {
		boxarttexnum = boxarttex1;
	} else if ( boxartnum == 1+pagenum*20 ) {
		boxarttexnum = boxarttex2;
	} else if ( boxartnum == 2+pagenum*20 ) {
		boxarttexnum = boxarttex3;
	} else if ( boxartnum == 3+pagenum*20 ) {
		boxarttexnum = boxarttex4;
	} else if ( boxartnum == 4+pagenum*20 ) {
		boxarttexnum = boxarttex5;
	} else if ( boxartnum == 5+pagenum*20 ) {
		boxarttexnum = boxarttex6;
	} else if ( boxartnum == 6+pagenum*20 ) {
		boxarttexnum = boxarttex1;
	} else if ( boxartnum == 7+pagenum*20 ) {
		boxarttexnum = boxarttex2;
	} else if ( boxartnum == 8+pagenum*20 ) {
		boxarttexnum = boxarttex3;
	} else if ( boxartnum == 9+pagenum*20 ) {
		boxarttexnum = boxarttex4;
	} else if ( boxartnum == 10+pagenum*20 ) {
		boxarttexnum = boxarttex5;
	} else if ( boxartnum == 11+pagenum*20 ) {
		boxarttexnum = boxarttex6;
	} else if ( boxartnum == 12+pagenum*20 ) {
		boxarttexnum = boxarttex1;
	} else if ( boxartnum == 13+pagenum*20 ) {
		boxarttexnum = boxarttex2;
	} else if ( boxartnum == 14+pagenum*20 ) {
		boxarttexnum = boxarttex3;
	} else if ( boxartnum == 15+pagenum*20 ) {
		boxarttexnum = boxarttex4;
	} else if ( boxartnum == 16+pagenum*20 ) {
		boxarttexnum = boxarttex5;
	} else if ( boxartnum == 17+pagenum*20 ) {
		boxarttexnum = boxarttex6;
	} else if ( boxartnum == 18+pagenum*20 ) {
		boxarttexnum = boxarttex1;
	} else if ( boxartnum == 19+pagenum*20 ) {
		boxarttexnum = boxarttex2;
	} /* else if ( boxartnum == 20 ) {
		boxarttexnum = boxarttex3;
	} else if ( boxartnum == 21 ) {
		boxarttexnum = boxarttex4;
	} else if ( boxartnum == 22 ) {
		boxarttexnum = boxarttex5;
	} else if ( boxartnum == 23 ) {
		boxarttexnum = boxarttex6;
	} else if ( boxartnum == 24 ) {
		boxarttexnum = boxarttex1;
	} else if ( boxartnum == 25 ) {
		boxarttexnum = boxarttex2;
	} else if ( boxartnum == 26 ) {
		boxarttexnum = boxarttex3;
	} else if ( boxartnum == 27 ) {
		boxarttexnum = boxarttex4;
	} else if ( boxartnum == 28 ) {
		boxarttexnum = boxarttex5;
	} else if ( boxartnum == 29 ) {
		boxarttexnum = boxarttex6;
	} else if ( boxartnum == 30 ) {
		boxarttexnum = boxarttex1;
	} else if ( boxartnum == 31 ) {
		boxarttexnum = boxarttex2;
	} else if ( boxartnum == 32 ) {
		boxarttexnum = boxarttex3;
	} else if ( boxartnum == 33 ) {
		boxarttexnum = boxarttex4;
	} else if ( boxartnum == 34 ) {
		boxarttexnum = boxarttex5;
	} else if ( boxartnum == 35 ) {
		boxarttexnum = boxarttex6;
	} else if ( boxartnum == 36 ) {
		boxarttexnum = boxarttex1;
	} else if ( boxartnum == 37 ) {
		boxarttexnum = boxarttex2;
	} else if ( boxartnum == 38 ) {
		boxarttexnum = boxarttex3;
	} else if ( boxartnum == 39 ) {
		boxarttexnum = boxarttex4;
	} else if ( boxartnum == 40 ) {
		boxarttexnum = boxarttex5;
	} else if ( boxartnum == 41 ) {
		boxarttexnum = boxarttex6;
	} else if ( boxartnum == 42 ) {
		boxarttexnum = boxarttex1;
	} else if ( boxartnum == 43 ) {
		boxarttexnum = boxarttex2;
	} else if ( boxartnum == 44 ) {
		boxarttexnum = boxarttex3;
	} else if ( boxartnum == 45 ) {
		boxarttexnum = boxarttex4;
	} else if ( boxartnum == 46 ) {
		boxarttexnum = boxarttex5;
	} else if ( boxartnum == 47 ) {
		boxarttexnum = boxarttex6;
	} else if ( boxartnum == 48 ) {
		boxarttexnum = boxarttex1;
	} else if ( boxartnum == 49 ) {
		boxarttexnum = boxarttex2;
	} else if ( boxartnum == 50 ) {
		boxarttexnum = boxarttex3;
	} else if ( boxartnum == 51 ) {
		boxarttexnum = boxarttex4;
	} else if ( boxartnum == 52 ) {
		boxarttexnum = boxarttex5;
	} else if ( boxartnum == 53 ) {
		boxarttexnum = boxarttex6;
	} else if ( boxartnum == 54 ) {
		boxarttexnum = boxarttex1;
	} else if ( boxartnum == 55 ) {
		boxarttexnum = boxarttex2;
	} else if ( boxartnum == 56 ) {
		boxarttexnum = boxarttex3;
	} else if ( boxartnum == 57 ) {
		boxarttexnum = boxarttex4;
	} else if ( boxartnum == 58 ) {
		boxarttexnum = boxarttex5;
	} else if ( boxartnum == 59 ) {
		boxarttexnum = boxarttex6;
	} else if ( boxartnum == 60 ) {
		boxarttexnum = boxarttex1;
	} else if ( boxartnum == 61 ) {
		boxarttexnum = boxarttex2;
	} else if ( boxartnum == 62 ) {
		boxarttexnum = boxarttex3;
	} else if ( boxartnum == 63 ) {
		boxarttexnum = boxarttex4;
	} else if ( boxartnum == 64 ) {
		boxarttexnum = boxarttex5;
	} else if ( boxartnum == 65 ) {
		boxarttexnum = boxarttex6;
	} else if ( boxartnum == 66 ) {
		boxarttexnum = boxarttex1;
	} else if ( boxartnum == 67 ) {
		boxarttexnum = boxarttex2;
	} else if ( boxartnum == 68 ) {
		boxarttexnum = boxarttex3;
	} else if ( boxartnum == 69 ) {
		boxarttexnum = boxarttex4;
	} else if ( boxartnum == 70 ) {
		boxarttexnum = boxarttex5;
	} else if ( boxartnum == 71 ) {
		boxarttexnum = boxarttex6;
	} else if ( boxartnum == 72 ) {
		boxarttexnum = boxarttex1;
	} else if ( boxartnum == 73 ) {
		boxarttexnum = boxarttex2;
	} else if ( boxartnum == 74 ) {
		boxarttexnum = boxarttex3;
	} else if ( boxartnum == 75 ) {
		boxarttexnum = boxarttex4;
	} else if ( boxartnum == 76 ) {
		boxarttexnum = boxarttex5;
	} else if ( boxartnum == 77 ) {
		boxarttexnum = boxarttex6;
	} else if ( boxartnum == 78 ) {
		boxarttexnum = boxarttex1;
	} else if ( boxartnum == 79 ) {
		boxarttexnum = boxarttex2;
	} else if ( boxartnum == 80 ) {
		boxarttexnum = boxarttex3;
	} else if ( boxartnum == 81 ) {
		boxarttexnum = boxarttex4;
	} else if ( boxartnum == 82 ) {
		boxarttexnum = boxarttex5;
	} else if ( boxartnum == 83 ) {
		boxarttexnum = boxarttex6;
	} else if ( boxartnum == 84 ) {
		boxarttexnum = boxarttex1;
	} else if ( boxartnum == 85 ) {
		boxarttexnum = boxarttex2;
	} else if ( boxartnum == 86 ) {
		boxarttexnum = boxarttex3;
	} else if ( boxartnum == 87 ) {
		boxarttexnum = boxarttex4;
	} else if ( boxartnum == 88 ) {
		boxarttexnum = boxarttex5;
	} else if ( boxartnum == 89 ) {
		boxarttexnum = boxarttex6;
	} else if ( boxartnum == 90 ) {
		boxarttexnum = boxarttex1;
	} else if ( boxartnum == 91 ) {
		boxarttexnum = boxarttex2;
	} else if ( boxartnum == 92 ) {
		boxarttexnum = boxarttex3;
	} else if ( boxartnum == 93 ) {
		boxarttexnum = boxarttex4;
	} else if ( boxartnum == 94 ) {
		boxarttexnum = boxarttex5;
	} else if ( boxartnum == 95 ) {
		boxarttexnum = boxarttex6;
	} else if ( boxartnum == 96 ) {
		boxarttexnum = boxarttex1;
	} else if ( boxartnum == 97 ) {
		boxarttexnum = boxarttex2;
	} else if ( boxartnum == 98 ) {
		boxarttexnum = boxarttex3;
	} else if ( boxartnum == 99 ) {
		boxarttexnum = boxarttex4;
	} */
}

void StoreBNRIconPath() {
	if (bnriconnum == 0+pagenum*20) {
		bnriconpath1 = tempimagepath;
	} else if (bnriconnum == 1+pagenum*20) {
		bnriconpath2 = tempimagepath;
	} else if (bnriconnum == 2+pagenum*20) {
		bnriconpath3 = tempimagepath;
	} else if (bnriconnum == 3+pagenum*20) {
		bnriconpath4 = tempimagepath;
	} else if (bnriconnum == 4+pagenum*20) {
		bnriconpath5 = tempimagepath;
	} else if (bnriconnum == 5+pagenum*20) {
		bnriconpath6 = tempimagepath;
	} else if (bnriconnum == 6+pagenum*20) {
		bnriconpath7 = tempimagepath;
	} else if (bnriconnum == 7+pagenum*20) {
		bnriconpath8 = tempimagepath;
	} else if (bnriconnum == 8+pagenum*20) {
		bnriconpath9 = tempimagepath;
	} else if (bnriconnum == 9+pagenum*20) {
		bnriconpath10 = tempimagepath;
	} else if (bnriconnum == 10+pagenum*20) {
		bnriconpath11 = tempimagepath;
	} else if (bnriconnum == 11+pagenum*20) {
		bnriconpath12 = tempimagepath;
	} else if (bnriconnum == 12+pagenum*20) {
		bnriconpath13 = tempimagepath;
	} else if (bnriconnum == 13+pagenum*20) {
		bnriconpath14 = tempimagepath;
	} else if (bnriconnum == 14+pagenum*20) {
		bnriconpath15 = tempimagepath;
	} else if (bnriconnum == 15+pagenum*20) {
		bnriconpath16 = tempimagepath;
	} else if (bnriconnum == 16+pagenum*20) {
		bnriconpath17 = tempimagepath;
	} else if (bnriconnum == 17+pagenum*20) {
		bnriconpath18 = tempimagepath;
	} else if (bnriconnum == 18+pagenum*20) {
		bnriconpath19 = tempimagepath;
	} else if (bnriconnum == 19+pagenum*20) {
		bnriconpath20 = tempimagepath;
	} /* else if (bnriconnum == 20) {
		bnriconpath21 = tempimagepath;
	} else if (bnriconnum == 21) {
		bnriconpath22 = tempimagepath;
	} else if (bnriconnum == 22) {
		bnriconpath23 = tempimagepath;
	} else if (bnriconnum == 23) {
		bnriconpath24 = tempimagepath;
	} else if (bnriconnum == 24) {
		bnriconpath25 = tempimagepath;
	} else if (bnriconnum == 25) {
		bnriconpath26 = tempimagepath;
	} else if (bnriconnum == 26) {
		bnriconpath27 = tempimagepath;
	} else if (bnriconnum == 27) {
		bnriconpath28 = tempimagepath;
	} else if (bnriconnum == 28) {
		bnriconpath29 = tempimagepath;
	} else if (bnriconnum == 29) {
		bnriconpath30 = tempimagepath;
	} else if (bnriconnum == 30) {
		bnriconpath31 = tempimagepath;
	} else if (bnriconnum == 31) {
		bnriconpath32 = tempimagepath;
	} else if (bnriconnum == 32) {
		bnriconpath33 = tempimagepath;
	} else if (bnriconnum == 33) {
		bnriconpath34 = tempimagepath;
	} else if (bnriconnum == 34) {
		bnriconpath35 = tempimagepath;
	} else if (bnriconnum == 35) {
		bnriconpath36 = tempimagepath;
	} else if (bnriconnum == 36) {
		bnriconpath37 = tempimagepath;
	} else if (bnriconnum == 37) {
		bnriconpath38 = tempimagepath;
	} else if (bnriconnum == 38) {
		bnriconpath39 = tempimagepath;
	} else if (bnriconnum == 39) {
		bnriconpath40 = tempimagepath;
	} else if (bnriconnum == 40) {
		bnriconpath41 = tempimagepath;
	} else if (bnriconnum == 41) {
		bnriconpath42 = tempimagepath;
	} else if (bnriconnum == 42) {
		bnriconpath43 = tempimagepath;
	} else if (bnriconnum == 43) {
		bnriconpath44 = tempimagepath;
	} else if (bnriconnum == 44) {
		bnriconpath45 = tempimagepath;
	} else if (bnriconnum == 45) {
		bnriconpath46 = tempimagepath;
	} else if (bnriconnum == 46) {
		bnriconpath47 = tempimagepath;
	} else if (bnriconnum == 47) {
		bnriconpath48 = tempimagepath;
	} else if (bnriconnum == 48) {
		bnriconpath49 = tempimagepath;
	} else if (bnriconnum == 49) {
		bnriconpath50 = tempimagepath;
	} else if (bnriconnum == 50) {
		bnriconpath51 = tempimagepath;
	} else if (bnriconnum == 51) {
		bnriconpath52 = tempimagepath;
	} else if (bnriconnum == 52) {
		bnriconpath53 = tempimagepath;
	} else if (bnriconnum == 53) {
		bnriconpath54 = tempimagepath;
	} else if (bnriconnum == 54) {
		bnriconpath55 = tempimagepath;
	} else if (bnriconnum == 55) {
		bnriconpath56 = tempimagepath;
	} else if (bnriconnum == 56) {
		bnriconpath57 = tempimagepath;
	} else if (bnriconnum == 57) {
		bnriconpath58 = tempimagepath;
	} else if (bnriconnum == 58) {
		bnriconpath59 = tempimagepath;
	} else if (bnriconnum == 59) {
		bnriconpath60 = tempimagepath;
	} else if (bnriconnum == 60) {
		bnriconpath61 = tempimagepath;
	} else if (bnriconnum == 61) {
		bnriconpath62 = tempimagepath;
	} else if (bnriconnum == 62) {
		bnriconpath63 = tempimagepath;
	} else if (bnriconnum == 63) {
		bnriconpath64 = tempimagepath;
	} else if (bnriconnum == 64) {
		bnriconpath65 = tempimagepath;
	} else if (bnriconnum == 65) {
		bnriconpath66 = tempimagepath;
	} else if (bnriconnum == 66) {
		bnriconpath67 = tempimagepath;
	} else if (bnriconnum == 67) {
		bnriconpath68 = tempimagepath;
	} else if (bnriconnum == 68) {
		bnriconpath69 = tempimagepath;
	} else if (bnriconnum == 69) {
		bnriconpath70 = tempimagepath;
	} else if (bnriconnum == 70) {
		bnriconpath71 = tempimagepath;
	} else if (bnriconnum == 71) {
		bnriconpath72 = tempimagepath;
	} else if (bnriconnum == 72) {
		bnriconpath73 = tempimagepath;
	} else if (bnriconnum == 73) {
		bnriconpath74 = tempimagepath;
	} else if (bnriconnum == 74) {
		bnriconpath75 = tempimagepath;
	} else if (bnriconnum == 75) {
		bnriconpath76 = tempimagepath;
	} else if (bnriconnum == 76) {
		bnriconpath77 = tempimagepath;
	} else if (bnriconnum == 77) {
		bnriconpath78 = tempimagepath;
	} else if (bnriconnum == 78) {
		bnriconpath79 = tempimagepath;
	} else if (bnriconnum == 79) {
		bnriconpath80 = tempimagepath;
	} else if (bnriconnum == 80) {
		bnriconpath81 = tempimagepath;
	} else if (bnriconnum == 81) {
		bnriconpath82 = tempimagepath;
	} else if (bnriconnum == 82) {
		bnriconpath83 = tempimagepath;
	} else if (bnriconnum == 83) {
		bnriconpath84 = tempimagepath;
	} else if (bnriconnum == 84) {
		bnriconpath85 = tempimagepath;
	} else if (bnriconnum == 85) {
		bnriconpath86 = tempimagepath;
	} else if (bnriconnum == 86) {
		bnriconpath87 = tempimagepath;
	} else if (bnriconnum == 87) {
		bnriconpath88 = tempimagepath;
	} else if (bnriconnum == 88) {
		bnriconpath89 = tempimagepath;
	} else if (bnriconnum == 89) {
		bnriconpath90 = tempimagepath;
	} else if (bnriconnum == 90) {
		bnriconpath91 = tempimagepath;
	} else if (bnriconnum == 91) {
		bnriconpath92 = tempimagepath;
	} else if (bnriconnum == 92) {
		bnriconpath93 = tempimagepath;
	} else if (bnriconnum == 93) {
		bnriconpath94 = tempimagepath;
	} else if (bnriconnum == 94) {
		bnriconpath95 = tempimagepath;
	} else if (bnriconnum == 95) {
		bnriconpath96 = tempimagepath;
	} else if (bnriconnum == 96) {
		bnriconpath97 = tempimagepath;
	} else if (bnriconnum == 97) {
		bnriconpath98 = tempimagepath;
	} else if (bnriconnum == 98) {
		bnriconpath99 = tempimagepath;
	} else if (bnriconnum == 99) {
		bnriconpath100 = tempimagepath;
	} */
}

void StoreBoxArtPath() {
	if (boxartnum == 0+pagenum*20) {
		boxartpath1 = tempimagepath;
	} else if (boxartnum == 1+pagenum*20) {
		boxartpath2 = tempimagepath;
	} else if (boxartnum == 2+pagenum*20) {
		boxartpath3 = tempimagepath;
	} else if (boxartnum == 3+pagenum*20) {
		boxartpath4 = tempimagepath;
	} else if (boxartnum == 4+pagenum*20) {
		boxartpath5 = tempimagepath;
	} else if (boxartnum == 5+pagenum*20) {
		boxartpath6 = tempimagepath;
	} else if (boxartnum == 6+pagenum*20) {
		boxartpath7 = tempimagepath;
	} else if (boxartnum == 7+pagenum*20) {
		boxartpath8 = tempimagepath;
	} else if (boxartnum == 8+pagenum*20) {
		boxartpath9 = tempimagepath;
	} else if (boxartnum == 9+pagenum*20) {
		boxartpath10 = tempimagepath;
	} else if (boxartnum == 10+pagenum*20) {
		boxartpath11 = tempimagepath;
	} else if (boxartnum == 11+pagenum*20) {
		boxartpath12 = tempimagepath;
	} else if (boxartnum == 12+pagenum*20) {
		boxartpath13 = tempimagepath;
	} else if (boxartnum == 13+pagenum*20) {
		boxartpath14 = tempimagepath;
	} else if (boxartnum == 14+pagenum*20) {
		boxartpath15 = tempimagepath;
	} else if (boxartnum == 15+pagenum*20) {
		boxartpath16 = tempimagepath;
	} else if (boxartnum == 16+pagenum*20) {
		boxartpath17 = tempimagepath;
	} else if (boxartnum == 17+pagenum*20) {
		boxartpath18 = tempimagepath;
	} else if (boxartnum == 18+pagenum*20) {
		boxartpath19 = tempimagepath;
	} else if (boxartnum == 19+pagenum*20) {
		boxartpath20 = tempimagepath;
	} /* else if (boxartnum == 20) {
		boxartpath21 = tempimagepath;
	} else if (boxartnum == 21) {
		boxartpath22 = tempimagepath;
	} else if (boxartnum == 22) {
		boxartpath23 = tempimagepath;
	} else if (boxartnum == 23) {
		boxartpath24 = tempimagepath;
	} else if (boxartnum == 24) {
		boxartpath25 = tempimagepath;
	} else if (boxartnum == 25) {
		boxartpath26 = tempimagepath;
	} else if (boxartnum == 26) {
		boxartpath27 = tempimagepath;
	} else if (boxartnum == 27) {
		boxartpath28 = tempimagepath;
	} else if (boxartnum == 28) {
		boxartpath29 = tempimagepath;
	} else if (boxartnum == 29) {
		boxartpath30 = tempimagepath;
	} else if (boxartnum == 30) {
		boxartpath31 = tempimagepath;
	} else if (boxartnum == 31) {
		boxartpath32 = tempimagepath;
	} else if (boxartnum == 32) {
		boxartpath33 = tempimagepath;
	} else if (boxartnum == 33) {
		boxartpath34 = tempimagepath;
	} else if (boxartnum == 34) {
		boxartpath35 = tempimagepath;
	} else if (boxartnum == 35) {
		boxartpath36 = tempimagepath;
	} else if (boxartnum == 36) {
		boxartpath37 = tempimagepath;
	} else if (boxartnum == 37) {
		boxartpath38 = tempimagepath;
	} else if (boxartnum == 38) {
		boxartpath39 = tempimagepath;
	} else if (boxartnum == 39) {
		boxartpath40 = tempimagepath;
	} else if (boxartnum == 40) {
		boxartpath41 = tempimagepath;
	} else if (boxartnum == 41) {
		boxartpath42 = tempimagepath;
	} else if (boxartnum == 42) {
		boxartpath43 = tempimagepath;
	} else if (boxartnum == 43) {
		boxartpath44 = tempimagepath;
	} else if (boxartnum == 44) {
		boxartpath45 = tempimagepath;
	} else if (boxartnum == 45) {
		boxartpath46 = tempimagepath;
	} else if (boxartnum == 46) {
		boxartpath47 = tempimagepath;
	} else if (boxartnum == 47) {
		boxartpath48 = tempimagepath;
	} else if (boxartnum == 48) {
		boxartpath49 = tempimagepath;
	} else if (boxartnum == 49) {
		boxartpath50 = tempimagepath;
	} else if (boxartnum == 50) {
		boxartpath51 = tempimagepath;
	} else if (boxartnum == 51) {
		boxartpath52 = tempimagepath;
	} else if (boxartnum == 52) {
		boxartpath53 = tempimagepath;
	} else if (boxartnum == 53) {
		boxartpath54 = tempimagepath;
	} else if (boxartnum == 54) {
		boxartpath55 = tempimagepath;
	} else if (boxartnum == 55) {
		boxartpath56 = tempimagepath;
	} else if (boxartnum == 56) {
		boxartpath57 = tempimagepath;
	} else if (boxartnum == 57) {
		boxartpath58 = tempimagepath;
	} else if (boxartnum == 58) {
		boxartpath59 = tempimagepath;
	} else if (boxartnum == 59) {
		boxartpath50 = tempimagepath;
	} else if (boxartnum == 60) {
		boxartpath61 = tempimagepath;
	} else if (boxartnum == 61) {
		boxartpath62 = tempimagepath;
	} else if (boxartnum == 62) {
		boxartpath63 = tempimagepath;
	} else if (boxartnum == 63) {
		boxartpath64 = tempimagepath;
	} else if (boxartnum == 64) {
		boxartpath65 = tempimagepath;
	} else if (boxartnum == 65) {
		boxartpath66 = tempimagepath;
	} else if (boxartnum == 66) {
		boxartpath67 = tempimagepath;
	} else if (boxartnum == 67) {
		boxartpath68 = tempimagepath;
	} else if (boxartnum == 68) {
		boxartpath69 = tempimagepath;
	} else if (boxartnum == 69) {
		boxartpath70 = tempimagepath;
	} else if (boxartnum == 70) {
		boxartpath71 = tempimagepath;
	} else if (boxartnum == 71) {
		boxartpath72 = tempimagepath;
	} else if (boxartnum == 72) {
		boxartpath73 = tempimagepath;
	} else if (boxartnum == 73) {
		boxartpath74 = tempimagepath;
	} else if (boxartnum == 74) {
		boxartpath75 = tempimagepath;
	} else if (boxartnum == 75) {
		boxartpath76 = tempimagepath;
	} else if (boxartnum == 76) {
		boxartpath77 = tempimagepath;
	} else if (boxartnum == 77) {
		boxartpath78 = tempimagepath;
	} else if (boxartnum == 78) {
		boxartpath79 = tempimagepath;
	} else if (boxartnum == 79) {
		boxartpath80 = tempimagepath;
	} else if (boxartnum == 80) {
		boxartpath81 = tempimagepath;
	} else if (boxartnum == 81) {
		boxartpath82 = tempimagepath;
	} else if (boxartnum == 82) {
		boxartpath83 = tempimagepath;
	} else if (boxartnum == 83) {
		boxartpath84 = tempimagepath;
	} else if (boxartnum == 84) {
		boxartpath85 = tempimagepath;
	} else if (boxartnum == 85) {
		boxartpath86 = tempimagepath;
	} else if (boxartnum == 86) {
		boxartpath87 = tempimagepath;
	} else if (boxartnum == 87) {
		boxartpath88 = tempimagepath;
	} else if (boxartnum == 88) {
		boxartpath89 = tempimagepath;
	} else if (boxartnum == 89) {
		boxartpath90 = tempimagepath;
	} else if (boxartnum == 90) {
		boxartpath91 = tempimagepath;
	} else if (boxartnum == 91) {
		boxartpath92 = tempimagepath;
	} else if (boxartnum == 92) {
		boxartpath93 = tempimagepath;
	} else if (boxartnum == 93) {
		boxartpath94 = tempimagepath;
	} else if (boxartnum == 94) {
		boxartpath95 = tempimagepath;
	} else if (boxartnum == 95) {
		boxartpath96 = tempimagepath;
	} else if (boxartnum == 96) {
		boxartpath97 = tempimagepath;
	} else if (boxartnum == 97) {
		boxartpath98 = tempimagepath;
	} else if (boxartnum == 98) {
		boxartpath99 = tempimagepath;
	} else if (boxartnum == 99) {
		boxartpath100 = tempimagepath;
	} */
}

void LoadBNRIcon() {
	if (bnriconnum == 0+pagenum*20) {
		sf2d_free_texture(bnricontex1);
		bnricontex1 = sfil_load_PNG_file(bnriconpath1, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 1+pagenum*20) {
		sf2d_free_texture(bnricontex2);
		bnricontex2 = sfil_load_PNG_file(bnriconpath2, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 2+pagenum*20) {
		sf2d_free_texture(bnricontex3);
		bnricontex3 = sfil_load_PNG_file(bnriconpath3, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 3+pagenum*20) {
		sf2d_free_texture(bnricontex4);
		bnricontex4 = sfil_load_PNG_file(bnriconpath4, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 4+pagenum*20) {
		sf2d_free_texture(bnricontex5);
		bnricontex5 = sfil_load_PNG_file(bnriconpath5, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 5+pagenum*20) {
		sf2d_free_texture(bnricontex6);
		bnricontex6 = sfil_load_PNG_file(bnriconpath6, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 6+pagenum*20) {
		sf2d_free_texture(bnricontex7);
		bnricontex7 = sfil_load_PNG_file(bnriconpath7, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 7+pagenum*20) {
		sf2d_free_texture(bnricontex8);
		bnricontex8 = sfil_load_PNG_file(bnriconpath8, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 8+pagenum*20) {
		sf2d_free_texture(bnricontex9);
		bnricontex9 = sfil_load_PNG_file(bnriconpath9, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 9+pagenum*20) {
		sf2d_free_texture(bnricontex10);
		bnricontex10 = sfil_load_PNG_file(bnriconpath10, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 10+pagenum*20) {
		sf2d_free_texture(bnricontex1);
		bnricontex1 = sfil_load_PNG_file(bnriconpath11, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 11+pagenum*20) {
		sf2d_free_texture(bnricontex2);
		bnricontex2 = sfil_load_PNG_file(bnriconpath12, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 12+pagenum*20) {
		sf2d_free_texture(bnricontex3);
		bnricontex3 = sfil_load_PNG_file(bnriconpath13, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 13+pagenum*20) {
		sf2d_free_texture(bnricontex4);
		bnricontex4 = sfil_load_PNG_file(bnriconpath14, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 14+pagenum*20) {
		sf2d_free_texture(bnricontex5);
		bnricontex5 = sfil_load_PNG_file(bnriconpath15, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 15+pagenum*20) {
		sf2d_free_texture(bnricontex6);
		bnricontex6 = sfil_load_PNG_file(bnriconpath16, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 16+pagenum*20) {
		sf2d_free_texture(bnricontex7);
		bnricontex7 = sfil_load_PNG_file(bnriconpath17, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 17+pagenum*20) {
		sf2d_free_texture(bnricontex8);
		bnricontex8 = sfil_load_PNG_file(bnriconpath18, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 18+pagenum*20) {
		sf2d_free_texture(bnricontex9);
		bnricontex9 = sfil_load_PNG_file(bnriconpath19, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 19+pagenum*20) {
		sf2d_free_texture(bnricontex10);
		bnricontex10 = sfil_load_PNG_file(bnriconpath20, SF2D_PLACE_RAM); // Banner icon
	} /* else if (bnriconnum == 20) {
		sf2d_free_texture(bnricontex1);
		bnricontex1 = sfil_load_PNG_file(bnriconpath21, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 21) {
		sf2d_free_texture(bnricontex2);
		bnricontex2 = sfil_load_PNG_file(bnriconpath22, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 22) {
		sf2d_free_texture(bnricontex3);
		bnricontex3 = sfil_load_PNG_file(bnriconpath23, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 23) {
		sf2d_free_texture(bnricontex4);
		bnricontex4 = sfil_load_PNG_file(bnriconpath24, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 24) {
		sf2d_free_texture(bnricontex5);
		bnricontex5 = sfil_load_PNG_file(bnriconpath25, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 25) {
		sf2d_free_texture(bnricontex6);
		bnricontex6 = sfil_load_PNG_file(bnriconpath26, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 26) {
		sf2d_free_texture(bnricontex7);
		bnricontex7 = sfil_load_PNG_file(bnriconpath27, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 27) {
		sf2d_free_texture(bnricontex8);
		bnricontex8 = sfil_load_PNG_file(bnriconpath28, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 28) {
		sf2d_free_texture(bnricontex9);
		bnricontex9 = sfil_load_PNG_file(bnriconpath29, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 29) {
		sf2d_free_texture(bnricontex10);
		bnricontex10 = sfil_load_PNG_file(bnriconpath30, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 30) {
		sf2d_free_texture(bnricontex1);
		bnricontex1 = sfil_load_PNG_file(bnriconpath31, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 31) {
		sf2d_free_texture(bnricontex2);
		bnricontex2 = sfil_load_PNG_file(bnriconpath32, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 32) {
		sf2d_free_texture(bnricontex3);
		bnricontex3 = sfil_load_PNG_file(bnriconpath33, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 33) {
		sf2d_free_texture(bnricontex4);
		bnricontex4 = sfil_load_PNG_file(bnriconpath34, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 34) {
		sf2d_free_texture(bnricontex5);
		bnricontex5 = sfil_load_PNG_file(bnriconpath35, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 35) {
		sf2d_free_texture(bnricontex6);
		bnricontex6 = sfil_load_PNG_file(bnriconpath36, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 36) {
		sf2d_free_texture(bnricontex7);
		bnricontex7 = sfil_load_PNG_file(bnriconpath37, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 37) {
		sf2d_free_texture(bnricontex8);
		bnricontex8 = sfil_load_PNG_file(bnriconpath38, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 38) {
		sf2d_free_texture(bnricontex9);
		bnricontex9 = sfil_load_PNG_file(bnriconpath39, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 39) {
		sf2d_free_texture(bnricontex10);
		bnricontex10 = sfil_load_PNG_file(bnriconpath40, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 40) {
		sf2d_free_texture(bnricontex1);
		bnricontex1 = sfil_load_PNG_file(bnriconpath41, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 41) {
		sf2d_free_texture(bnricontex2);
		bnricontex2 = sfil_load_PNG_file(bnriconpath42, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 42) {
		sf2d_free_texture(bnricontex3);
		bnricontex3 = sfil_load_PNG_file(bnriconpath43, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 43) {
		sf2d_free_texture(bnricontex4);
		bnricontex4 = sfil_load_PNG_file(bnriconpath44, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 44) {
		sf2d_free_texture(bnricontex5);
		bnricontex5 = sfil_load_PNG_file(bnriconpath45, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 45) {
		sf2d_free_texture(bnricontex6);
		bnricontex6 = sfil_load_PNG_file(bnriconpath46, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 46) {
		sf2d_free_texture(bnricontex7);
		bnricontex7 = sfil_load_PNG_file(bnriconpath47, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 47) {
		sf2d_free_texture(bnricontex8);
		bnricontex8 = sfil_load_PNG_file(bnriconpath48, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 48) {
		sf2d_free_texture(bnricontex9);
		bnricontex9 = sfil_load_PNG_file(bnriconpath49, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 49) {
		sf2d_free_texture(bnricontex10);
		bnricontex10 = sfil_load_PNG_file(bnriconpath50, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 50) {
		sf2d_free_texture(bnricontex1);
		bnricontex1 = sfil_load_PNG_file(bnriconpath51, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 51) {
		sf2d_free_texture(bnricontex2);
		bnricontex2 = sfil_load_PNG_file(bnriconpath52, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 52) {
		sf2d_free_texture(bnricontex3);
		bnricontex3 = sfil_load_PNG_file(bnriconpath53, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 53) {
		sf2d_free_texture(bnricontex4);
		bnricontex4 = sfil_load_PNG_file(bnriconpath54, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 54) {
		sf2d_free_texture(bnricontex5);
		bnricontex5 = sfil_load_PNG_file(bnriconpath55, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 55) {
		sf2d_free_texture(bnricontex6);
		bnricontex6 = sfil_load_PNG_file(bnriconpath56, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 56) {
		sf2d_free_texture(bnricontex7);
		bnricontex7 = sfil_load_PNG_file(bnriconpath57, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 57) {
		sf2d_free_texture(bnricontex8);
		bnricontex8 = sfil_load_PNG_file(bnriconpath58, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 58) {
		sf2d_free_texture(bnricontex9);
		bnricontex9 = sfil_load_PNG_file(bnriconpath59, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 59) {
		sf2d_free_texture(bnricontex10);
		bnricontex10 = sfil_load_PNG_file(bnriconpath60, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 60) {
		sf2d_free_texture(bnricontex1);
		bnricontex1 = sfil_load_PNG_file(bnriconpath61, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 61) {
		sf2d_free_texture(bnricontex2);
		bnricontex2 = sfil_load_PNG_file(bnriconpath62, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 62) {
		sf2d_free_texture(bnricontex3);
		bnricontex3 = sfil_load_PNG_file(bnriconpath63, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 63) {
		sf2d_free_texture(bnricontex4);
		bnricontex4 = sfil_load_PNG_file(bnriconpath64, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 64) {
		sf2d_free_texture(bnricontex5);
		bnricontex5 = sfil_load_PNG_file(bnriconpath65, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 65) {
		sf2d_free_texture(bnricontex6);
		bnricontex6 = sfil_load_PNG_file(bnriconpath66, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 66) {
		sf2d_free_texture(bnricontex7);
		bnricontex7 = sfil_load_PNG_file(bnriconpath67, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 67) {
		sf2d_free_texture(bnricontex8);
		bnricontex8 = sfil_load_PNG_file(bnriconpath68, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 68) {
		sf2d_free_texture(bnricontex9);
		bnricontex9 = sfil_load_PNG_file(bnriconpath69, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 69) {
		sf2d_free_texture(bnricontex10);
		bnricontex10 = sfil_load_PNG_file(bnriconpath70, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 70) {
		sf2d_free_texture(bnricontex1);
		bnricontex1 = sfil_load_PNG_file(bnriconpath71, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 71) {
		sf2d_free_texture(bnricontex2);
		bnricontex2 = sfil_load_PNG_file(bnriconpath72, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 72) {
		sf2d_free_texture(bnricontex3);
		bnricontex3 = sfil_load_PNG_file(bnriconpath73, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 73) {
		sf2d_free_texture(bnricontex4);
		bnricontex4 = sfil_load_PNG_file(bnriconpath74, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 74) {
		sf2d_free_texture(bnricontex5);
		bnricontex5 = sfil_load_PNG_file(bnriconpath75, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 75) {
		sf2d_free_texture(bnricontex6);
		bnricontex6 = sfil_load_PNG_file(bnriconpath76, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 76) {
		sf2d_free_texture(bnricontex7);
		bnricontex7 = sfil_load_PNG_file(bnriconpath77, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 77) {
		sf2d_free_texture(bnricontex8);
		bnricontex8 = sfil_load_PNG_file(bnriconpath78, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 78) {
		sf2d_free_texture(bnricontex9);
		bnricontex9 = sfil_load_PNG_file(bnriconpath79, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 79) {
		sf2d_free_texture(bnricontex10);
		bnricontex10 = sfil_load_PNG_file(bnriconpath80, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 80) {
		sf2d_free_texture(bnricontex1);
		bnricontex1 = sfil_load_PNG_file(bnriconpath81, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 81) {
		sf2d_free_texture(bnricontex2);
		bnricontex2 = sfil_load_PNG_file(bnriconpath82, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 82) {
		sf2d_free_texture(bnricontex3);
		bnricontex3 = sfil_load_PNG_file(bnriconpath83, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 83) {
		sf2d_free_texture(bnricontex4);
		bnricontex4 = sfil_load_PNG_file(bnriconpath84, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 84) {
		sf2d_free_texture(bnricontex5);
		bnricontex5 = sfil_load_PNG_file(bnriconpath85, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 85) {
		sf2d_free_texture(bnricontex6);
		bnricontex6 = sfil_load_PNG_file(bnriconpath86, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 86) {
		sf2d_free_texture(bnricontex7);
		bnricontex7 = sfil_load_PNG_file(bnriconpath87, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 87) {
		sf2d_free_texture(bnricontex8);
		bnricontex8 = sfil_load_PNG_file(bnriconpath88, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 88) {
		sf2d_free_texture(bnricontex9);
		bnricontex9 = sfil_load_PNG_file(bnriconpath89, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 89) {
		sf2d_free_texture(bnricontex10);
		bnricontex10 = sfil_load_PNG_file(bnriconpath90, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 90) {
		sf2d_free_texture(bnricontex1);
		bnricontex1 = sfil_load_PNG_file(bnriconpath91, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 91) {
		sf2d_free_texture(bnricontex2);
		bnricontex2 = sfil_load_PNG_file(bnriconpath92, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 92) {
		sf2d_free_texture(bnricontex3);
		bnricontex3 = sfil_load_PNG_file(bnriconpath93, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 93) {
		sf2d_free_texture(bnricontex4);
		bnricontex4 = sfil_load_PNG_file(bnriconpath94, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 94) {
		sf2d_free_texture(bnricontex5);
		bnricontex5 = sfil_load_PNG_file(bnriconpath95, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 95) {
		sf2d_free_texture(bnricontex6);
		bnricontex6 = sfil_load_PNG_file(bnriconpath96, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 96) {
		sf2d_free_texture(bnricontex7);
		bnricontex7 = sfil_load_PNG_file(bnriconpath97, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 97) {
		sf2d_free_texture(bnricontex8);
		bnricontex8 = sfil_load_PNG_file(bnriconpath98, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 98) {
		sf2d_free_texture(bnricontex9);
		bnricontex9 = sfil_load_PNG_file(bnriconpath99, SF2D_PLACE_RAM); // Banner icon
	} else if (bnriconnum == 99) {
		sf2d_free_texture(bnricontex10);
		bnricontex10 = sfil_load_PNG_file(bnriconpath100, SF2D_PLACE_RAM); // Banner icon
	} */
}

void LoadBoxArt() {
	if ( boxartnum == 0+pagenum*20 ) {
		sf2d_free_texture(boxarttex1);
		boxarttex1 = sfil_load_PNG_file(boxartpath1, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 1+pagenum*20 ) {
		sf2d_free_texture(boxarttex2);
		boxarttex2 = sfil_load_PNG_file(boxartpath2, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 2+pagenum*20 ) {
		sf2d_free_texture(boxarttex3);
		boxarttex3 = sfil_load_PNG_file(boxartpath3, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 3+pagenum*20 ) {
		sf2d_free_texture(boxarttex4);
		boxarttex4 = sfil_load_PNG_file(boxartpath4, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 4+pagenum*20 ) {
		sf2d_free_texture(boxarttex5);
		boxarttex5 = sfil_load_PNG_file(boxartpath5, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 5+pagenum*20 ) {
		sf2d_free_texture(boxarttex6);
		boxarttex6 = sfil_load_PNG_file(boxartpath6, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 6+pagenum*20 ) {
		sf2d_free_texture(boxarttex1);
		boxarttex1 = sfil_load_PNG_file(boxartpath7, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 7+pagenum*20 ) {
		sf2d_free_texture(boxarttex2);
		boxarttex2 = sfil_load_PNG_file(boxartpath8, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 8+pagenum*20 ) {
		sf2d_free_texture(boxarttex3);
		boxarttex3 = sfil_load_PNG_file(boxartpath9, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 9+pagenum*20 ) {
		sf2d_free_texture(boxarttex4);
		boxarttex4 = sfil_load_PNG_file(boxartpath10, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 10+pagenum*20 ) {
		sf2d_free_texture(boxarttex5);
		boxarttex5 = sfil_load_PNG_file(boxartpath11, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 11+pagenum*20 ) {
		sf2d_free_texture(boxarttex6);
		boxarttex6 = sfil_load_PNG_file(boxartpath12, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 12+pagenum*20 ) {
		sf2d_free_texture(boxarttex1);
		boxarttex1 = sfil_load_PNG_file(boxartpath13, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 13+pagenum*20 ) {
		sf2d_free_texture(boxarttex2);
		boxarttex2 = sfil_load_PNG_file(boxartpath14, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 14+pagenum*20 ) {
		sf2d_free_texture(boxarttex3);
		boxarttex3 = sfil_load_PNG_file(boxartpath15, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 15+pagenum*20 ) {
		sf2d_free_texture(boxarttex4);
		boxarttex4 = sfil_load_PNG_file(boxartpath16, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 16+pagenum*20 ) {
		sf2d_free_texture(boxarttex5);
		boxarttex5 = sfil_load_PNG_file(boxartpath17, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 17+pagenum*20 ) {
		sf2d_free_texture(boxarttex6);
		boxarttex6 = sfil_load_PNG_file(boxartpath18, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 18+pagenum*20 ) {
		sf2d_free_texture(boxarttex1);
		boxarttex1 = sfil_load_PNG_file(boxartpath19, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 19+pagenum*20 ) {
		sf2d_free_texture(boxarttex2);
		boxarttex2 = sfil_load_PNG_file(boxartpath20, SF2D_PLACE_RAM); // Box art
	} /* else if ( boxartnum == 20 ) {
		sf2d_free_texture(boxarttex3);
		boxarttex3 = sfil_load_PNG_file(boxartpath21, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 21 ) {
		sf2d_free_texture(boxarttex4);
		boxarttex4 = sfil_load_PNG_file(boxartpath22, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 22 ) {
		sf2d_free_texture(boxarttex5);
		boxarttex5 = sfil_load_PNG_file(boxartpath23, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 23 ) {
		sf2d_free_texture(boxarttex6);
		boxarttex6 = sfil_load_PNG_file(boxartpath24, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 24 ) {
		sf2d_free_texture(boxarttex1);
		boxarttex1 = sfil_load_PNG_file(boxartpath25, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 25 ) {
		sf2d_free_texture(boxarttex2);
		boxarttex2 = sfil_load_PNG_file(boxartpath26, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 26 ) {
		sf2d_free_texture(boxarttex3);
		boxarttex3 = sfil_load_PNG_file(boxartpath27, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 27 ) {
		sf2d_free_texture(boxarttex4);
		boxarttex4 = sfil_load_PNG_file(boxartpath28, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 28 ) {
		sf2d_free_texture(boxarttex5);
		boxarttex5 = sfil_load_PNG_file(boxartpath29, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 29 ) {
		sf2d_free_texture(boxarttex6);
		boxarttex6 = sfil_load_PNG_file(boxartpath30, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 30 ) {
		sf2d_free_texture(boxarttex1);
		boxarttex1 = sfil_load_PNG_file(boxartpath31, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 31 ) {
		sf2d_free_texture(boxarttex2);
		boxarttex2 = sfil_load_PNG_file(boxartpath32, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 32 ) {
		sf2d_free_texture(boxarttex3);
		boxarttex3 = sfil_load_PNG_file(boxartpath33, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 33 ) {
		sf2d_free_texture(boxarttex4);
		boxarttex4 = sfil_load_PNG_file(boxartpath34, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 34 ) {
		sf2d_free_texture(boxarttex5);
		boxarttex5 = sfil_load_PNG_file(boxartpath35, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 35 ) {
		sf2d_free_texture(boxarttex6);
		boxarttex6 = sfil_load_PNG_file(boxartpath36, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 36 ) {
		sf2d_free_texture(boxarttex1);
		boxarttex1 = sfil_load_PNG_file(boxartpath37, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 37 ) {
		sf2d_free_texture(boxarttex2);
		boxarttex2 = sfil_load_PNG_file(boxartpath38, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 38 ) {
		sf2d_free_texture(boxarttex3);
		boxarttex3 = sfil_load_PNG_file(boxartpath39, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 39 ) {
		sf2d_free_texture(boxarttex4);
		boxarttex4 = sfil_load_PNG_file(boxartpath40, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 40 ) {
		sf2d_free_texture(boxarttex5);
		boxarttex5 = sfil_load_PNG_file(boxartpath41, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 41 ) {
		sf2d_free_texture(boxarttex6);
		boxarttex6 = sfil_load_PNG_file(boxartpath42, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 42 ) {
		sf2d_free_texture(boxarttex1);
		boxarttex1 = sfil_load_PNG_file(boxartpath43, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 43 ) {
		sf2d_free_texture(boxarttex2);
		boxarttex2 = sfil_load_PNG_file(boxartpath44, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 44 ) {
		sf2d_free_texture(boxarttex3);
		boxarttex3 = sfil_load_PNG_file(boxartpath45, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 45 ) {
		sf2d_free_texture(boxarttex4);
		boxarttex4 = sfil_load_PNG_file(boxartpath46, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 46 ) {
		sf2d_free_texture(boxarttex5);
		boxarttex5 = sfil_load_PNG_file(boxartpath47, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 47 ) {
		sf2d_free_texture(boxarttex6);
		boxarttex6 = sfil_load_PNG_file(boxartpath48, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 48 ) {
		sf2d_free_texture(boxarttex1);
		boxarttex1 = sfil_load_PNG_file(boxartpath49, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 49 ) {
		sf2d_free_texture(boxarttex2);
		boxarttex2 = sfil_load_PNG_file(boxartpath50, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 50 ) {
		sf2d_free_texture(boxarttex3);
		boxarttex3 = sfil_load_PNG_file(boxartpath51, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 51 ) {
		sf2d_free_texture(boxarttex4);
		boxarttex4 = sfil_load_PNG_file(boxartpath52, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 52 ) {
		sf2d_free_texture(boxarttex5);
		boxarttex5 = sfil_load_PNG_file(boxartpath53, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 53 ) {
		sf2d_free_texture(boxarttex6);
		boxarttex6 = sfil_load_PNG_file(boxartpath54, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 54 ) {
		sf2d_free_texture(boxarttex1);
		boxarttex1 = sfil_load_PNG_file(boxartpath55, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 55 ) {
		sf2d_free_texture(boxarttex2);
		boxarttex2 = sfil_load_PNG_file(boxartpath56, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 56 ) {
		sf2d_free_texture(boxarttex3);
		boxarttex3 = sfil_load_PNG_file(boxartpath57, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 57 ) {
		sf2d_free_texture(boxarttex4);
		boxarttex4 = sfil_load_PNG_file(boxartpath58, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 58 ) {
		sf2d_free_texture(boxarttex5);
		boxarttex5 = sfil_load_PNG_file(boxartpath59, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 59 ) {
		sf2d_free_texture(boxarttex6);
		boxarttex6 = sfil_load_PNG_file(boxartpath60, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 60 ) {
		sf2d_free_texture(boxarttex1);
		boxarttex1 = sfil_load_PNG_file(boxartpath61, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 61 ) {
		sf2d_free_texture(boxarttex2);
		boxarttex2 = sfil_load_PNG_file(boxartpath62, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 62 ) {
		sf2d_free_texture(boxarttex3);
		boxarttex3 = sfil_load_PNG_file(boxartpath63, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 63 ) {
		sf2d_free_texture(boxarttex4);
		boxarttex4 = sfil_load_PNG_file(boxartpath64, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 64 ) {
		sf2d_free_texture(boxarttex5);
		boxarttex5 = sfil_load_PNG_file(boxartpath65, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 65 ) {
		sf2d_free_texture(boxarttex6);
		boxarttex6 = sfil_load_PNG_file(boxartpath66, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 66 ) {
		sf2d_free_texture(boxarttex1);
		boxarttex1 = sfil_load_PNG_file(boxartpath67, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 67 ) {
		sf2d_free_texture(boxarttex2);
		boxarttex2 = sfil_load_PNG_file(boxartpath68, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 68 ) {
		sf2d_free_texture(boxarttex3);
		boxarttex3 = sfil_load_PNG_file(boxartpath69, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 69 ) {
		sf2d_free_texture(boxarttex4);
		boxarttex4 = sfil_load_PNG_file(boxartpath70, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 70 ) {
		sf2d_free_texture(boxarttex5);
		boxarttex5 = sfil_load_PNG_file(boxartpath71, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 71 ) {
		sf2d_free_texture(boxarttex6);
		boxarttex6 = sfil_load_PNG_file(boxartpath72, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 72 ) {
		sf2d_free_texture(boxarttex1);
		boxarttex1 = sfil_load_PNG_file(boxartpath73, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 73 ) {
		sf2d_free_texture(boxarttex2);
		boxarttex2 = sfil_load_PNG_file(boxartpath74, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 74 ) {
		sf2d_free_texture(boxarttex3);
		boxarttex3 = sfil_load_PNG_file(boxartpath75, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 75 ) {
		sf2d_free_texture(boxarttex4);
		boxarttex4 = sfil_load_PNG_file(boxartpath76, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 76 ) {
		sf2d_free_texture(boxarttex5);
		boxarttex5 = sfil_load_PNG_file(boxartpath77, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 77 ) {
		sf2d_free_texture(boxarttex6);
		boxarttex6 = sfil_load_PNG_file(boxartpath78, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 78 ) {
		sf2d_free_texture(boxarttex1);
		boxarttex1 = sfil_load_PNG_file(boxartpath79, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 79 ) {
		sf2d_free_texture(boxarttex2);
		boxarttex2 = sfil_load_PNG_file(boxartpath80, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 80 ) {
		sf2d_free_texture(boxarttex3);
		boxarttex3 = sfil_load_PNG_file(boxartpath81, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 81 ) {
		sf2d_free_texture(boxarttex4);
		boxarttex4 = sfil_load_PNG_file(boxartpath82, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 82 ) {
		sf2d_free_texture(boxarttex5);
		boxarttex5 = sfil_load_PNG_file(boxartpath83, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 83 ) {
		sf2d_free_texture(boxarttex6);
		boxarttex6 = sfil_load_PNG_file(boxartpath84, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 84 ) {
		sf2d_free_texture(boxarttex1);
		boxarttex1 = sfil_load_PNG_file(boxartpath85, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 85 ) {
		sf2d_free_texture(boxarttex2);
		boxarttex2 = sfil_load_PNG_file(boxartpath86, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 86 ) {
		sf2d_free_texture(boxarttex3);
		boxarttex3 = sfil_load_PNG_file(boxartpath87, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 87 ) {
		sf2d_free_texture(boxarttex4);
		boxarttex4 = sfil_load_PNG_file(boxartpath88, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 88 ) {
		sf2d_free_texture(boxarttex5);
		boxarttex5 = sfil_load_PNG_file(boxartpath89, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 89 ) {
		sf2d_free_texture(boxarttex6);
		boxarttex6 = sfil_load_PNG_file(boxartpath90, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 90 ) {
		sf2d_free_texture(boxarttex1);
		boxarttex1 = sfil_load_PNG_file(boxartpath91, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 91 ) {
		sf2d_free_texture(boxarttex2);
		boxarttex2 = sfil_load_PNG_file(boxartpath92, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 92 ) {
		sf2d_free_texture(boxarttex3);
		boxarttex3 = sfil_load_PNG_file(boxartpath93, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 93 ) {
		sf2d_free_texture(boxarttex4);
		boxarttex4 = sfil_load_PNG_file(boxartpath94, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 94 ) {
		sf2d_free_texture(boxarttex5);
		boxarttex5 = sfil_load_PNG_file(boxartpath95, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 95 ) {
		sf2d_free_texture(boxarttex6);
		boxarttex6 = sfil_load_PNG_file(boxartpath96, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 96 ) {
		sf2d_free_texture(boxarttex1);
		boxarttex1 = sfil_load_PNG_file(boxartpath97, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 97 ) {
		sf2d_free_texture(boxarttex2);
		boxarttex2 = sfil_load_PNG_file(boxartpath98, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 98 ) {
		sf2d_free_texture(boxarttex3);
		boxarttex3 = sfil_load_PNG_file(boxartpath99, SF2D_PLACE_RAM); // Box art
	} else if ( boxartnum == 99 ) {
		sf2d_free_texture(boxarttex4);
		boxarttex4 = sfil_load_PNG_file(boxartpath100, SF2D_PLACE_RAM); // Box art
	} */
}


void LoadSettings() {
	settings_colorvalue = settingsini.GetInt(settingsini_frontend, settingsini_frontend_color, 0);
	settings_locswitchvalue = settingsini.GetInt(settingsini_frontend, settingsini_frontend_locswitch, 0);
	settings_topbordervalue = settingsini.GetInt(settingsini_frontend, settingsini_frontend_topborder, 0);
	settings_countervalue = settingsini.GetInt(settingsini_frontend, settingsini_frontend_counter, 0);
	romselect_toplayout = settingsini.GetInt(settingsini_frontend, settingsini_frontend_toplayout, 0);
	settings_autoupdatevalue = settingsini.GetInt(settingsini_frontend, settingsini_frontend_autoupdate, 0);
	settings_autodlvalue = settingsini.GetInt(settingsini_frontend, settingsini_frontend_autodl, 0);
	// romselect_layout = settingsini.GetInt(settingsini_frontend, settingsini_frontend_botlayout, 0);
	twlsettings_rainbowledvalue = settingsini.GetInt(settingsini_twlmode, settingsini_twl_rainbowled, 0);
	twlsettings_cpuspeedvalue = settingsini.GetInt(settingsini_twlmode, settingsini_twl_clock, 0);
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
}

void SaveSettings() {
	settingsini.SetInt(settingsini_frontend, settingsini_frontend_color, settings_colorvalue);
	settingsini.SetInt(settingsini_frontend, settingsini_frontend_locswitch, settings_locswitchvalue);
	settingsini.SetInt(settingsini_frontend, settingsini_frontend_topborder, settings_topbordervalue);
	settingsini.SetInt(settingsini_frontend, settingsini_frontend_counter, settings_countervalue);
	settingsini.SetInt(settingsini_frontend, settingsini_frontend_toplayout, romselect_toplayout);
	settingsini.SetInt(settingsini_frontend, settingsini_frontend_autoupdate, settings_autoupdatevalue);
	settingsini.SetInt(settingsini_frontend, settingsini_frontend_autodl, settings_autodlvalue);
	//settingsini.SetInt(settingsini_frontend, settingsini_frontend_botlayout, romselect_layout);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_rainbowled, twlsettings_rainbowledvalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_clock, twlsettings_cpuspeedvalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_bootani, twlsettings_bootscreenvalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_hsmsg, twlsettings_healthsafetyvalue);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_launchslot1, twlsettings_launchslot1value);
	settingsini.SetInt(settingsini_twlmode, settingsini_twl_resetslot1, twlsettings_resetslot1value);
	if (twlsettings_consolevalue == 0) {
		settingsini.SetInt(settingsini_twlmode, settingsini_twl_debug, -1);
	} else if (twlsettings_consolevalue == 1) {
		settingsini.SetInt(settingsini_twlmode, settingsini_twl_debug, 0);
	} else if (twlsettings_consolevalue == 2) {
		settingsini.SetInt(settingsini_twlmode, settingsini_twl_debug, 1);
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
	ptmuInit();	// For battery status
	sdmcInit();
	romfsInit();
	srvInit();
	hidInit();

	// make folders if they don't exist
	mkdir("sdmc:/roms/nds", 0777);
	mkdir("sdmc:/roms/flashcard/nds", 0777);
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
	sf2d_texture *shoulderYtex = sfil_load_PNG_file("romfs:/graphics/shoulder_Y.png", SF2D_PLACE_RAM); // Y button
	sf2d_texture *shoulderXtex = sfil_load_PNG_file("romfs:/graphics/shoulder_X.png", SF2D_PLACE_RAM); // X button
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
	sf2d_texture *dotcircletex = sfil_load_PNG_file(dotcircleloc, SF2D_PLACE_RAM); // Dots forming a circle
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

	bool dspfirmfound = false;
 	if( access( "sdmc:/3ds/dspfirm.cdc", F_OK ) != -1 ) {
		ndspInit();
		dspfirmfound = true;
	}

	bool musicbool = false;
	if( access( "sdmc:/_nds/twloader/music.wav", F_OK ) != -1 ) {
		musicpath = "sdmc:/_nds/twloader/music.wav";
	}

	sound bgm_menu(musicpath);
	//sound bgm_settings("sdmc:/_nds/twloader/music/settings.wav");
	sound sfx_launch("romfs:/sounds/launch.wav",2,false);
	sound sfx_select("romfs:/sounds/select.wav",2,false);
	sound sfx_stop("romfs:/sounds/stop.wav",2,false);
	sound sfx_switch("romfs:/sounds/switch.wav",2,false);
	sound sfx_wrong("romfs:/sounds/wrong.wav",2,false);
	sound sfx_back("romfs:/sounds/back.wav",2,false);

	const std::string& path = std::string();
	std::vector<std::string> files = {};
	std::vector<std::string> fcfiles = {};
	std::vector<std::string> bnriconfiles = {};
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
	DIR *bnricondir;
	DIR *fcbnricondir;
	DIR *boxartdir;
	DIR *fcboxartdir;
	struct dirent *namelist;

	if ((dir = opendir (path.empty() ? "sdmc:/roms/nds" : path.c_str())) != NULL) {
	/* print all the files and directories within directory */
		while ((namelist = readdir (dir)) != NULL) {
			std::string fname = (namelist->d_name);
			if(fname.find(extension_UCnds, (fname.length() - extension_UCnds.length())) != std::string::npos)
				files.push_back(namelist->d_name);
			if(fname.find(extension_LCnds, (fname.length() - extension_LCnds.length())) != std::string::npos)
				files.push_back(namelist->d_name);
			if(fname.find(extension_oddnds1, (fname.length() - extension_oddnds1.length())) != std::string::npos)
				files.push_back(namelist->d_name);
			if(fname.find(extension_oddnds2, (fname.length() - extension_oddnds2.length())) != std::string::npos)
				files.push_back(namelist->d_name);
			if(fname.find(extension_oddnds3, (fname.length() - extension_oddnds3.length())) != std::string::npos)
				files.push_back(namelist->d_name);
			if(fname.find(extension_oddnds4, (fname.length() - extension_oddnds4.length())) != std::string::npos)
				files.push_back(namelist->d_name);
			if(fname.find(extension_oddnds5, (fname.length() - extension_oddnds5.length())) != std::string::npos)
				files.push_back(namelist->d_name);
			if(fname.find(extension_oddnds6, (fname.length() - extension_oddnds6.length())) != std::string::npos)
				files.push_back(namelist->d_name);
		}
		closedir (dir);
		std::sort( files.begin(), files.end() );
	}
	
	if ((flashcarddir = opendir ("sdmc:/roms/flashcard/nds")) != NULL) {
	/* print all the files and directories within directory */
		while ((namelist = readdir (flashcarddir)) != NULL) {
			std::string fcfname = (namelist->d_name);
			if(fcfname.find(extension_UCini, (fcfname.length() - extension_UCini.length())) != std::string::npos)
				fcfiles.push_back(namelist->d_name);
			if(fcfname.find(extension_LCini, (fcfname.length() - extension_LCini.length())) != std::string::npos)
				fcfiles.push_back(namelist->d_name);
			if(fcfname.find(extension_oddini1, (fcfname.length() - extension_oddini1.length())) != std::string::npos)
				fcfiles.push_back(namelist->d_name);
			if(fcfname.find(extension_oddini2, (fcfname.length() - extension_oddini2.length())) != std::string::npos)
				fcfiles.push_back(namelist->d_name);
			if(fcfname.find(extension_oddini3, (fcfname.length() - extension_oddini3.length())) != std::string::npos)
				fcfiles.push_back(namelist->d_name);
			if(fcfname.find(extension_oddini4, (fcfname.length() - extension_oddini4.length())) != std::string::npos)
				fcfiles.push_back(namelist->d_name);
			if(fcfname.find(extension_oddini5, (fcfname.length() - extension_oddini5.length())) != std::string::npos)
				fcfiles.push_back(namelist->d_name);
			if(fcfname.find(extension_oddini6, (fcfname.length() - extension_oddini6.length())) != std::string::npos)
				fcfiles.push_back(namelist->d_name);
		}
		closedir (flashcarddir);
		std::sort( fcfiles.begin(), fcfiles.end() );
	}

	if ((fcbnricondir = opendir ("sdmc:/_nds/twloader/bnricons/")) != NULL) {
		while ((namelist = readdir (bnricondir)) != NULL) {
			std::string bifname = (namelist->d_name);
			if(bifname.find(extension_UCpng, (bifname.length() - extension_UCpng.length())) != std::string::npos)
				bnriconfiles.push_back(namelist->d_name);
			if(bifname.find(extension_LCpng, (bifname.length() - extension_LCpng.length())) != std::string::npos)
				bnriconfiles.push_back(namelist->d_name);
		}
		closedir (bnricondir);
		std::sort( bnriconfiles.begin(), bnriconfiles.end() );
	}

	if ((fcbnricondir = opendir ("sdmc:/_nds/twloader/bnricons/flashcard/")) != NULL) {
		while ((namelist = readdir (fcbnricondir)) != NULL) {
			std::string fcbifname = (namelist->d_name);
			if(fcbifname.find(extension_UCpng, (fcbifname.length() - extension_UCpng.length())) != std::string::npos)
				fcbnriconfiles.push_back(namelist->d_name);
			if(fcbifname.find(extension_LCpng, (fcbifname.length() - extension_LCpng.length())) != std::string::npos)
				fcbnriconfiles.push_back(namelist->d_name);
		}
		closedir (fcbnricondir);
		std::sort( fcbnriconfiles.begin(), fcbnriconfiles.end() );
	}

	if ((boxartdir = opendir ("sdmc:/_nds/twloader/boxart/")) != NULL) {
		while ((namelist = readdir (boxartdir)) != NULL) {
			std::string bafname = (namelist->d_name);
			if(bafname.find(extension_UCpng, (bafname.length() - extension_UCpng.length())) != std::string::npos)
				boxartfiles.push_back(namelist->d_name);
			if(bafname.find(extension_LCpng, (bafname.length() - extension_LCpng.length())) != std::string::npos)
				boxartfiles.push_back(namelist->d_name);
		}
		closedir (boxartdir);
		std::sort( boxartfiles.begin(), boxartfiles.end() );
	}

	if ((fcboxartdir = opendir ("sdmc:/_nds/twloader/boxart/flashcard/")) != NULL) {
		while ((namelist = readdir (fcboxartdir)) != NULL) {
			std::string fcbafname = (namelist->d_name);
			if(fcbafname.find(extension_UCpng, (fcbafname.length() - extension_UCpng.length())) != std::string::npos)
				fcboxartfiles.push_back(namelist->d_name);
			if(fcbafname.find(extension_LCpng, (fcbafname.length() - extension_LCpng.length())) != std::string::npos)
				fcboxartfiles.push_back(namelist->d_name);
		}
		closedir (fcboxartdir);
		std::sort( fcboxartfiles.begin(), fcboxartfiles.end() );
	}
	
	if(settings_autodlvalue == 1){
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Now downloading latest TWLoader version (GUI CIA)...");
		sf2d_end_frame();
		sf2d_swapbuffers();
		downloadfile("https://www.dropbox.com/s/01vifhf49lkailx/TWLoader.cia?dl=1","/_nds/twloader/cia/TWLoader.cia");
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Now downloading latest TWLoader version (TWLNAND side CIA)...");
		sf2d_end_frame();
		sf2d_swapbuffers();
		downloadfile("https://www.dropbox.com/s/jjb5u83pskrruij/TWLoader%20-%20TWLNAND%20side.cia?dl=1","/_nds/twloader/cia/TWLoader - TWLNAND side.cia");
	}
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	sf2d_end_frame();
	sf2d_swapbuffers();
	
	if(settings_autoupdatevalue == 2){
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Now updating bootstrap-card (Unofficial)...");
		sf2d_end_frame();
		sf2d_swapbuffers();
		downloadfile("https://www.dropbox.com/s/m3jmxhr4b5tn1yi/bootstrap-card.nds?dl=1","/_nds/bootstrap-card.nds");
	} else if(settings_autoupdatevalue == 1){
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Now updating bootstrap-card (Release)...");
		sf2d_end_frame();
		sf2d_swapbuffers();
		downloadfile("https://www.dropbox.com/s/eb6e8nsa2eyjmb3/bootstrap-card.nds?dl=1","/_nds/bootstrap-card.nds");
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Now updating bootstrap-dldi (Release)...");
		sf2d_end_frame();
		sf2d_swapbuffers();
		downloadfile("https://www.dropbox.com/s/prbs8b96fyb3zcb/bootstrap-dldi.nds?dl=1","/_nds/bootstrap-dldi.nds");
	}
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	sf2d_end_frame();
	sf2d_swapbuffers();
		
	char str2[20] = {0};
	std::sprintf(str2, "%d", fcfiles.size());
	romsel_counter2fc = str2;
	char str3[20] = {0};
	std::sprintf(str3, "%d", files.size());
	romsel_counter2sd = str3;

	int cursorPosition = 0, storedcursorPosition = 0, i = 0;
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
	bool bannertextloaded = false;
	bool boxarttexloaded = false;

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
	
	float rad = 0.0f;
	u16 touch_x = 320/2;
	u16 touch_y = 240/2;

	//char* nickname = "Nickname";
	
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
	
	bool run = true;

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

		u8 batteryChargeState = 0;
		u8 batteryLevel = 0;
		u32 batteryIcon = 0;
		
		if (storedcursorPosition < 0)
			storedcursorPosition = 0;
		
		if(screenmode == 0) {
			if (colortexloaded == false) {
				topbgtex = sfil_load_PNG_file(topbgloc, SF2D_PLACE_RAM); // Top background, behind the DSi-Menu border
				dotcircletex = sfil_load_PNG_file(dotcircleloc, SF2D_PLACE_RAM); // Dots forming a circle
				startbordertex = sfil_load_PNG_file(startborderloc, SF2D_PLACE_RAM); // "START" border
				colortexloaded = true;
			}
			if (boxarttexloaded == false) {
				if (twlsettings_forwardervalue == 0) {
					/* sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
					sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Now loading banner icons (SD Card)...");
					sf2d_end_frame();
					sf2d_swapbuffers(); */
					for(bnriconnum = pagenum*20; bnriconnum < 20+pagenum*20; bnriconnum++) {
						if (bnriconnum < files.size()) {
							bnriconfile = files.at(bnriconnum).c_str();
							tempfile_fullpath = malloc(256);
							strcpy(tempfile_fullpath, bnriconfolder);
							strcat(tempfile_fullpath, bnriconfile);
							strcat(tempfile_fullpath, ".png");

							if( access( tempfile_fullpath, F_OK ) != -1 ) {
								tempimagepath = tempfile_fullpath;
							} else {
								tempimagepath = "romfs:/graphics/icon_unknown.png";	// Prevent crashing
							}
						} else {
							tempimagepath = "romfs:/graphics/icon_unknown.png";	// Prevent crashing
						}
						StoreBNRIconPath();
					}

					/* sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
					sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Now storing box art filenames (SD Card)...");
					sf2d_end_frame();
					sf2d_swapbuffers(); */
					for(boxartnum = pagenum*20; boxartnum < 20+pagenum*20; boxartnum++) {
						if (boxartnum < files.size()) {
							boxartfile = files.at(boxartnum).c_str();
							tempfile_fullpath = malloc(256);
							strcpy(tempfile_fullpath, boxartfolder);
							strcat(tempfile_fullpath, boxartfile);
							strcat(tempfile_fullpath, ".png");
						
							if( access( tempfile_fullpath, F_OK ) != -1 ) {
								tempimagepath = tempfile_fullpath;
							} else {
								tempimagepath = "romfs:/graphics/boxart_unknown.png";
							}
						} else {
							tempimagepath = "romfs:/graphics/boxart_unknown.png";
						}
						StoreBoxArtPath();
					}
				} else {
					/* sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
					sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Now loading banner icons (Flashcard)...");
					sf2d_end_frame();
					sf2d_swapbuffers(); */
					for(bnriconnum = pagenum*20; bnriconnum < 20+pagenum*20; bnriconnum++) {
						if (bnriconnum < fcfiles.size()) {
							bnriconfile = fcfiles.at(bnriconnum).c_str();
							tempfile_fullpath = malloc(256);
							strcpy(tempfile_fullpath, fcbnriconfolder);
							strcat(tempfile_fullpath, bnriconfile);
							strcat(tempfile_fullpath, ".png");

							if( access( tempfile_fullpath, F_OK ) != -1 ) {
								tempimagepath = tempfile_fullpath;
							} else {
								tempimagepath = "romfs:/graphics/icon_unknown.png";	// Prevent crashing
							}
						} else {
							tempimagepath = "romfs:/graphics/icon_unknown.png";	// Prevent crashing
						}
						StoreBNRIconPath();
					}

					/* sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
					sftd_draw_textf(font, 2, 2, RGBA8(255, 255, 255, 255), 12, "Now storing box art filenames (Flashcard)...");
					sf2d_end_frame();
					sf2d_swapbuffers(); */
					for(boxartnum = pagenum*20; boxartnum < 20+pagenum*20; boxartnum++) {
						if (boxartnum < fcfiles.size()) {
							boxartfile = fcfiles.at(boxartnum).c_str();
							tempfile_fullpath = malloc(256);
							strcpy(tempfile_fullpath, fcboxartfolder);
							strcat(tempfile_fullpath, boxartfile);
							strcat(tempfile_fullpath, ".png");
						
							if( access( tempfile_fullpath, F_OK ) != -1 ) {
								tempimagepath = tempfile_fullpath;
							} else {
								tempimagepath = "romfs:/graphics/boxart_unknown.png";
							}
						} else {
							tempimagepath = "romfs:/graphics/boxart_unknown.png";
						}
						StoreBoxArtPath();
					}
				}
				
				bnriconnum = 0+pagenum*20;
				LoadBNRIcon();
				bnriconnum = 1+pagenum*20;
				LoadBNRIcon();
				bnriconnum = 2+pagenum*20;
				LoadBNRIcon();
				bnriconnum = 3+pagenum*20;
				LoadBNRIcon();
				bnriconnum = 4+pagenum*20;
				LoadBNRIcon();
				bnriconnum = 5+pagenum*20;
				LoadBNRIcon();
				bnriconnum = 6+pagenum*20;
				LoadBNRIcon();
				bnriconnum = 7+pagenum*20;
				LoadBNRIcon();
				bnriconnum = 8+pagenum*20;
				LoadBNRIcon();
				bnriconnum = 9+pagenum*20;
				LoadBNRIcon();
				
				boxartnum = 0+pagenum*20;
				LoadBoxArt();
				boxartnum = 1+pagenum*20;
				LoadBoxArt();
				boxartnum = 2+pagenum*20;
				LoadBoxArt();
				boxartnum = 3+pagenum*20;
				LoadBoxArt();
				boxartnum = 4+pagenum*20;
				LoadBoxArt();
				boxartnum = 5+pagenum*20;
				LoadBoxArt();
				
				boxarttexloaded = true;
				boxartnum = 0+pagenum*20;
			}

			if (!musicbool) {
				if (dspfirmfound) { bgm_menu.play(); }
				musicbool = true;
			}
			if (twlsettings_forwardervalue == 1) {
				noromtext1 = "No INIs found!";
				noromtext2 = "Put .ini files in 'sdmc:/roms/flashcard/nds'.";
			} else {
				noromtext1 = "No ROMs found!";
				noromtext2 = "Put .nds ROMs in 'sdmc:/roms/nds'.";
			}
			if(R_SUCCEEDED(PTMU_GetBatteryChargeState(&batteryChargeState)) && batteryChargeState) {
  				batteryIcon = batterychrgtex;
  			} else if(R_SUCCEEDED(PTMU_GetBatteryLevel(&batteryLevel))) {
				switch (batteryLevel){
					case 5:
						if (batteryChargeState){
							batteryIcon = battery5tex;
						}							
					case 4:
						batteryIcon = battery4tex;
						break;
					case 3:
						batteryIcon = battery3tex;
						break;
					case 2:
						batteryIcon = battery2tex;
						break;	
					case 1:
						batteryIcon = battery1tex;
						break;
				}
			}

			sf2d_start_frame(GFX_TOP, GFX_LEFT);
			sf2d_draw_texture_scale(topbgtex, offset3dl_topbg + -12, 0, 1.32, 1);
			if (i != 0) {	// If ROMs are found, then display box art
				if (romselect_toplayout == 0) {
					boxartXpos = 136;
					if (twlsettings_forwardervalue == 1) {
						if(fcfiles.size() >= 19+pagenum*20) {
							for(boxartnum = pagenum*20; boxartnum < 20+pagenum*20; boxartnum++) {
								ChangeBoxArtNo();
								sf2d_draw_texture(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
								sf2d_draw_texture_scale_blend(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 0xC0)); // Draw box art's reflection
								boxartXpos += 144;
							}
						} else {
							for(boxartnum = pagenum*20; boxartnum < fcfiles.size(); boxartnum++) {
								ChangeBoxArtNo();
								sf2d_draw_texture(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
								sf2d_draw_texture_scale_blend(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 0xC0)); // Draw box art's reflection
								boxartXpos += 144;
							}
						}
					} else {
						if(files.size() >= 19+pagenum*20) {
							for(boxartnum = pagenum*20; boxartnum < 20+pagenum*20; boxartnum++) {
								ChangeBoxArtNo();
								sf2d_draw_texture(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
								sf2d_draw_texture_scale_blend(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 0xC0)); // Draw box art's reflection
								boxartXpos += 144;
							}
						} else {
							for(boxartnum = pagenum*20; boxartnum < files.size(); boxartnum++) {
								ChangeBoxArtNo();
								sf2d_draw_texture(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
								sf2d_draw_texture_scale_blend(boxarttexnum, offset3dl_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 0xC0)); // Draw box art's reflection
								boxartXpos += 144;
							}
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
			//sftd_draw_textf(font, 2, 2, RGBA8(0, 0, 0, 255), 12, boxartpath1); // Debug text
			sf2d_draw_texture(shoulderLtex, 0, LshoulderYpos);
			sftd_draw_textf(font, 17, LshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Lshouldertext);
			if (settings_locswitchvalue == 1) {
				sf2d_draw_texture(shoulderRtex, 328, RshoulderYpos);
				sftd_draw_textf(font, 332, RshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Rshouldertext);
			}

			sf2d_draw_rectangle(0, 0, 400, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect

			sf2d_end_frame();
				
			sf2d_start_frame(GFX_TOP, GFX_RIGHT);
			sf2d_draw_texture_scale(topbgtex, offset3dr_topbg + -12, 0, 1.32, 1);
			if (i != 0) {	// If ROMs are found, then display box art
				if (romselect_toplayout == 0) {
					boxartXpos = 136;
					if (twlsettings_forwardervalue == 1) {
						if(fcfiles.size() >= 19+pagenum*20) {
							for(boxartnum = pagenum*20; boxartnum < 20+pagenum*20; boxartnum++) {
								ChangeBoxArtNo();
								sf2d_draw_texture(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
								sf2d_draw_texture_scale_blend(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 0xC0)); // Draw box art's reflection
								boxartXpos += 144;
							}
						} else {
							for(boxartnum = pagenum*20; boxartnum < fcfiles.size(); boxartnum++) {
								ChangeBoxArtNo();
								sf2d_draw_texture(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
								sf2d_draw_texture_scale_blend(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 0xC0)); // Draw box art's reflection
								boxartXpos += 144;
							}
						}
					} else {
						if(files.size() >= 19+pagenum*20) {
							for(boxartnum = pagenum*20; boxartnum < 20+pagenum*20; boxartnum++) {
								ChangeBoxArtNo();
								sf2d_draw_texture(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
								sf2d_draw_texture_scale_blend(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 0xC0)); // Draw box art's reflection
								boxartXpos += 144;
							}
						} else {
							for(boxartnum = pagenum*20; boxartnum < files.size(); boxartnum++) {
								ChangeBoxArtNo();
								sf2d_draw_texture(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 240/2 - boxarttexnum->height/2); // Draw box art
								sf2d_draw_texture_scale_blend(boxarttexnum, offset3dr_boxart+boxartXpos+boxartXmovepos, 264, 1, -0.75, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 0xC0)); // Draw box art's reflection
								boxartXpos += 144;
							}
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
			//sftd_draw_textf(font, 2, 2, RGBA8(0, 0, 0, 255), 12, boxartpath1); // Debug text
			sf2d_draw_texture(shoulderLtex, -1, LshoulderYpos);
			sftd_draw_textf(font, 16, LshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Lshouldertext);
			if (settings_locswitchvalue == 1) {
				sf2d_draw_texture(shoulderRtex, 327, RshoulderYpos);
				sftd_draw_textf(font, 331, RshoulderYpos+5, RGBA8(0, 0, 0, 255), 11, Rshouldertext);
			}
			
			sf2d_draw_rectangle(0, 0, 400, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect
			
			sf2d_end_frame();
		} else if(screenmode == 1) {
			/* if (!musicbool) {
				if (dspfirmfound) { bgm_settings.play(); }
				musicbool = true;
			} */
			if (colortexloaded == true) {
				sf2d_free_texture(topbgtex);
				sf2d_free_texture(dotcircletex);
				sf2d_free_texture(startbordertex);
				colortexloaded = false;
			}
			if(R_SUCCEEDED(PTMU_GetBatteryChargeState(&batteryChargeState)) && batteryChargeState) {
  				batteryIcon = setbatterychrgtex;
  			} else if(R_SUCCEEDED(PTMU_GetBatteryLevel(&batteryLevel))) {
				switch (batteryLevel){
					case 5:
						if (batteryChargeState){
							batteryIcon = setbattery5tex;
						}							
					case 4:
						batteryIcon = setbattery4tex;
						break;
					case 3:
						batteryIcon = setbattery3tex;
						break;
					case 2:
						batteryIcon = setbattery2tex;
						break;	
					case 1:
						batteryIcon = setbattery1tex;
						break;
				}
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
				musicbool = false;
				if(screenmode == 1) {
					screenmode = 0;
					fadeout = false;
					fadein = true;
				} else {
					// run = false;
					screenoff();
					SaveSettings();
					if (twlsettings_rainbowledvalue == 1) {
						RainbowLED(); }
					applaunchon = true;
				}
			}
		}
		
		if (playwrongsounddone == true) {
			if (hHeld & KEY_LEFT || hHeld & KEY_RIGHT) {} else {
				soundwaittimer += 1;
				if (soundwaittimer == 2) {
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
				bannertextloaded = false;			
				storedcursorPosition = cursorPosition;
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
				if (dspfirmfound) { sfx_select.play(); }
				// Load the previous banner icons
				if ( cursorPosition == 6+pagenum*20 ||
				cursorPosition == 11+pagenum*20 ||
				cursorPosition == 16+pagenum*20 ) {
					bnriconnum = cursorPosition-2;
					LoadBNRIcon();
					bnriconnum--;
					LoadBNRIcon();
					bnriconnum--;
					LoadBNRIcon();
					bnriconnum--;
					LoadBNRIcon();
					bnriconnum--;
					LoadBNRIcon();
				}
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
				if (cursorPositionset == false) {
					cursorPosition--;
					if (twlsettings_forwardervalue == 1) {
						if (cursorPosition == -1)
							cursorPosition--;
					}
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
		} else if(titleboxXmoveright == true) {
			titleboxXmovetimer += 1;
			if (titleboxXmovetimer == 10) {
				titleboxXmovetimer = 0;
				titleboxXmoveright = false;
			} else if (titleboxXmovetimer == 9) {
				// Delay a frame
				bannertextloaded = false;
				storedcursorPosition = cursorPosition;
				if (dspfirmfound) { sfx_stop.play(); }
				// Load the next banner icons
				if ( cursorPosition == 7+pagenum*20 ||
				cursorPosition == 12+pagenum*20 ||
				cursorPosition == 17+pagenum*20 ) {
					bnriconnum = cursorPosition+3;
					LoadBNRIcon();
					bnriconnum++;
					LoadBNRIcon();
					bnriconnum++;
					LoadBNRIcon();
					bnriconnum++;
					LoadBNRIcon();
					bnriconnum++;
					LoadBNRIcon();
				}
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
				if (dspfirmfound) { sfx_select.play(); }
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
						if (dspfirmfound) {
							sfx_wrong.stop();
							sfx_wrong.play();
						}
						playwrongsounddone = true;
					}
				}
			}
		}
		if(applaunchprep == true) {
			rad += 0.50f;
			titleboxYmovepos -= 6;
			ndsiconYmovepos -= 6;
			if (titleboxYmovepos == -240) {
				if(screenmodeswitch == true) {
					musicbool = false;
					screenmode = 1;
					rad == 0.0f;
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
						} else if (twlsettings_flashcardvalue == 1 || twlsettings_flashcardvalue == 3) {
							CIniFile fcrompathini( "sdmc:/_nds/YSMenu.ini" );
							std::string	rominini = setfcrompathini.GetString(fcrompathini_flashcardrom, fcrompathini_rompath, "");
							fcrompathini.SetString("YSMENU", "AUTO_BOOT", slashchar+rominini);
							fcrompathini.SaveIniFile( "sdmc:/_nds/YSMenu.ini" );
						} else if (twlsettings_flashcardvalue == 2 || twlsettings_flashcardvalue == 4 || twlsettings_flashcardvalue == 5) {
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

		// if(updatebotscreen == true){
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
					if (fadealpha == 0) {
						sf2d_draw_texture(bubbletex, 0, 0);
						// if (dspfirmfound) { sfx_menuselect.play(); }
						if (twlsettings_forwardervalue == 1) {
							if (cursorPosition == -2) {
								// sftd_draw_textf(font, 10, 8, RGBA8(127, 127, 127, 255), 12, "Settings");
								sftd_draw_textf(font, 132, 36, RGBA8(0, 0, 0, 255), 16, "Settings");
							} else if (cursorPosition == -1) {
								sftd_draw_textf(font, 10, 8, RGBA8(127, 127, 127, 255), 12, "Slot-1 cart (NTR carts only)");
							} else {
								if (bannertextloaded == false) {
									if (fcfiles.size() != 0)
										romsel_filename = fcfiles.at(storedcursorPosition).c_str();
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
									bannertextloaded = true;
								}
								sftd_draw_textf(font, 10, 8, RGBA8(127, 127, 127, 255), 12, romsel_filename);
								sftd_draw_textf(font, 10, 24, RGBA8(0, 0, 0, 255), 16, romsel_gameline1.c_str());
								sftd_draw_textf(font, 10, 44, RGBA8(0, 0, 0, 255), 16, romsel_gameline2.c_str());
								sftd_draw_textf(font, 10, 64, RGBA8(0, 0, 0, 255), 16, romsel_gameline3.c_str());
								if (settings_countervalue == 1) {
									char str[20] = {0};
									std::sprintf(str, "%d", storedcursorPosition+1);
									romsel_counter1 = str;
									sftd_draw_textf(font, 8, 96, RGBA8(0, 0, 0, 255), 12, romsel_counter1);
									sftd_draw_textf(font, 27, 96, RGBA8(0, 0, 0, 255), 12, "/");
									sftd_draw_textf(font, 32, 96, RGBA8(0, 0, 0, 255), 12, romsel_counter2fc);
								} else {
									bannertextloaded = true;
								}
							}
						} else {
							if (cursorPosition == -2) {
								// sftd_draw_textf(font, 10, 8, RGBA8(127, 127, 127, 255), 12, "Settings");
								sftd_draw_textf(font, 132, 36, RGBA8(0, 0, 0, 255), 16, "Settings");
							} else if (cursorPosition == -1) {
								sftd_draw_textf(font, 10, 8, RGBA8(127, 127, 127, 255), 12, "Slot-1 cart (NTR carts only)");
							} else {
								if (bannertextloaded == false) {
									if (files.size() != 0)
										romsel_filename = files.at(storedcursorPosition).c_str();
								}
								sftd_draw_textf(font, 10, 8, RGBA8(127, 127, 127, 255), 12, romsel_filename);
								if (settings_countervalue == 1) {
									char str[20] = {0};
									std::sprintf(str, "%d", storedcursorPosition+1);
									romsel_counter1 = str;
									sftd_draw_textf(font, 8, 96, RGBA8(0, 0, 0, 255), 12, romsel_counter1);
									sftd_draw_textf(font, 27, 96, RGBA8(0, 0, 0, 255), 12, "/");
									sftd_draw_textf(font, 32, 96, RGBA8(0, 0, 0, 255), 12, romsel_counter2sd);
								} else {
									bannertextloaded = true;
								}
							}
						}
					} else {
						sf2d_draw_texture(bottomlogotex, 320/2 - bottomlogotex->width/2, 40);
					}
					sf2d_draw_texture(homeicontex, 81, 220); // Draw HOME icon
					sftd_draw_textf(font, 98, 220, RGBA8(0, 0, 0, 255), 14, ": Return to HOME Menu");
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
							sf2d_draw_texture(bracetex, 32+titleboxXmovepos, 116);
							sf2d_draw_texture(settingsboxtex, cartXpos+titleboxXmovepos, 119);
						} else {
							sf2d_draw_texture(bracetex, 32+cartXpos+titleboxXmovepos, 116);
						}
					}

					titleboxXpos = 128;
					ndsiconXpos = 144;
					filenameYpos = 0;
					if (twlsettings_forwardervalue == 1) {
						if(fcfiles.size() >= 19+pagenum*20) {
							for(i = pagenum*20; i < 20+pagenum*20; i++){
								sf2d_draw_texture(boxfulltex, titleboxXpos+titleboxXmovepos, 120);
								titleboxXpos += 64;
							}
							for(bnriconnum = pagenum*20; bnriconnum < 20+pagenum*20; bnriconnum++) {
								ChangeBNRIconNo();
								sf2d_draw_texture_part(bnricontexnum, ndsiconXpos+titleboxXmovepos, 133, 0, bnriconframenum*32, 32, 32);
								ndsiconXpos += 64;
							}
						} else {
							for(i = pagenum*20; i < fcfiles.size(); i++){
								sf2d_draw_texture(boxfulltex, titleboxXpos+titleboxXmovepos, 120);
								titleboxXpos += 64;
							}
							for(bnriconnum = pagenum*20; bnriconnum < fcfiles.size(); bnriconnum++) {
								ChangeBNRIconNo();
								sf2d_draw_texture_part(bnricontexnum, ndsiconXpos+titleboxXmovepos, 133, 0, bnriconframenum*32, 32, 32);
								ndsiconXpos += 64;
							}
						}
					} else {
						if(files.size() >= 19+pagenum*20) {
							for(i = pagenum*20; i < 20+pagenum*20; i++){
								sf2d_draw_texture(boxfulltex, titleboxXpos+titleboxXmovepos, 120);
								titleboxXpos += 64;
							}
							for(bnriconnum = pagenum*20; bnriconnum < 20+pagenum*20; bnriconnum++) {
								ChangeBNRIconNo();
								sf2d_draw_texture_part(bnricontexnum, ndsiconXpos+titleboxXmovepos, 133, 0, bnriconframenum*32, 32, 32);
								ndsiconXpos += 64;
							}
						} else {
							for(i = pagenum*20; i < files.size(); i++){
								sf2d_draw_texture(boxfulltex, titleboxXpos+titleboxXmovepos, 120);
								titleboxXpos += 64;
							}
							for(bnriconnum = pagenum*20; bnriconnum < files.size(); bnriconnum++) {
								ChangeBNRIconNo();
								sf2d_draw_texture_part(bnricontexnum, ndsiconXpos+titleboxXmovepos, 133, 0, bnriconframenum*32, 32, 32);
								ndsiconXpos += 64;
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
							sf2d_draw_texture(settingsboxtex, 128, titleboxYmovepos-1); // Draw settings box that moves up
						} else if (cursorPosition == -1) {
							sf2d_draw_texture(carttex, 128, titleboxYmovepos); // Draw selected Slot-1 game that moves up
							sf2d_draw_texture(iconunktex, 144, ndsiconYmovepos);
						} else {
							sf2d_draw_texture(boxfulltex, 128, titleboxYmovepos); // Draw selected game/app that moves up
							bnriconnum = cursorPosition;
							ChangeBNRIconNo();
							sf2d_draw_texture_part(bnricontexnum, 144, ndsiconYmovepos, 0, bnriconframenum*32, 32, 32);
						}
						sf2d_draw_texture_rotate(dotcircletex, 160, 152, rad);  // Dots moving in circles
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
					} else if (settings_colorvalue == 18) {
						settings_colorvaluetext = "Christmas";
					}
					if (settings_locswitchvalue == 0) {
						settings_locswitchvaluetext = "Off";
					} else if (settings_locswitchvalue == 1) {
						settings_locswitchvaluetext = "On";
					}
					if (settings_topbordervalue == 0) {
						settings_topbordervaluetext = "Off";
					} else if (settings_topbordervalue == 1) {
						settings_topbordervaluetext = "On";
					}
					if (settings_countervalue == 0) {
						settings_countervaluetext = "Off";
					} else if (settings_topbordervalue == 1) {
						settings_countervaluetext = "On";
					}
					if (settings_autoupdatevalue == 2) {
						settings_autoupdatevaluetext = "Unofficial";
					} else if (settings_autoupdatevalue == 1) {
						settings_autoupdatevaluetext = "Release";
					} else if (settings_autoupdatevalue == 0){
						settings_autoupdatevaluetext = "Off";
					}
					if (settings_autodlvalue == 1) {
						settings_autodlvaluetext = "On";
					} else if (settings_autodlvalue == 0){
						settings_autodlvaluetext = "Off";
					}
					settingstext_bot = "Settings: GUI";
					settingsYpos = 40;
					if(settingscursorPosition == 0) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, settings_colortext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, settings_colorvaluetext);
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "The color of the top background");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "and the START border.");
						settingsYpos += 12;
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_colortext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_colorvaluetext);
						settingsYpos += 12;
					}
					if(settingscursorPosition == 1) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, settings_locswitchtext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, settings_locswitchvaluetext);
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "The R button switches the game location");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "between the SD Card and the flashcard.");
						settingsYpos += 12;
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_locswitchtext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_locswitchvaluetext);
						settingsYpos += 12;
					}
					if(settingscursorPosition == 2) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, settings_topbordertext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, settings_topbordervaluetext);
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "The border surrounding the top background.");
						settingsYpos += 12;
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_topbordertext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_topbordervaluetext);
						settingsYpos += 12;
					}
					if(settingscursorPosition == 3) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, settings_countertext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, settings_countervaluetext);
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "A number of selected game and listed games");
						sftd_draw_textf(font, 8, 198, RGBA8(255, 255, 255, 255), 13, "is shown below the text bubble.");
						settingsYpos += 12;
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_countertext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_countervaluetext);
						settingsYpos += 12;
					}
					if(settingscursorPosition == 4) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, settings_autoupdatetext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, settings_autoupdatevaluetext);
						sftd_draw_textf(font, 8, 184, RGBA8(255, 255, 255, 255), 13, "Auto-update nds-bootstrap at launch.");
						settingsYpos += 12;
					} else {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_autoupdatetext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(255, 255, 255, 255), 12, settings_autoupdatevaluetext);
						settingsYpos += 12;
					}
					if(settingscursorPosition == 5) {
						sftd_draw_textf(font, settingsXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, settings_autodltext);
						sftd_draw_textf(font, settingsvalueXpos, settingsYpos, RGBA8(color_Rvalue, color_Gvalue, color_Bvalue, 255), 12, settings_autodlvaluetext);
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
		// }
		
		sf2d_swapbuffers();


		if (titleboxXmovetimer == 0) {
			updatebotscreen = false;
		}
		if (screenmode == 0) {
			if (romselect_toplayout == 1) {
				Lshouldertext = Lshouldertext_boxart;
			} else {
				Lshouldertext = Lshouldertext_blank;
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
					if (settings_locswitchvalue == 1) {
						if(hDown & KEY_R) {
							pagenum = 0;
							bannertextloaded = false;
							cursorPosition = 0;
							storedcursorPosition = cursorPosition;
							titleboxXmovepos = 0;
							boxartXmovepos = 0;
							noromsfound = false;
							if (twlsettings_forwardervalue == 1) {
								twlsettings_forwardervalue = 0;
							} else {
								twlsettings_forwardervalue = 1;
							}
							boxarttexloaded = false;
							if (dspfirmfound) {
								sfx_switch.stop();	// Prevent freezing
								sfx_switch.play();
							}
							updatebotscreen = true;
						}
					}
					if (noromsfound == false) {
						if (twlsettings_forwardervalue == 1) {	// If no ROMs are found
							if (fcfiles.size() == 0) {	// If no ROMs are found
								cursorPosition = -2;
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
							if (dspfirmfound) {
								bgm_menu.stop();
								sfx_launch.play();
							}
						} else if (touch_x < 128 && touch_y >= 118 && touch_y <= 180) {
							//titleboxXmovepos -= 64;
							if (titleboxXmoveright == false) {
								titleboxXmoveleft = true;
							}
							updatebotscreen = true;
						} else if (touch_x > 192 && touch_y >= 118 && touch_y <= 180) {
							//titleboxXmovepos -= 64;
							if (titleboxXmoveleft == false) {
								if (twlsettings_forwardervalue == 1) {
									if (i == 0) {
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
						if (dspfirmfound) {
							bgm_menu.stop();
							sfx_launch.play();
						}
					} else if(hHeld & KEY_RIGHT){
						//titleboxXmovepos -= 64;
						if (titleboxXmoveleft == false) {
							if (twlsettings_forwardervalue == 1) {
								if (i == 0) {
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
						if (titleboxXmoveright == false) {
							titleboxXmoveleft = true;
						}
						updatebotscreen = true;
					} else if (hDown & KEY_SELECT) {
						titleboxXmovetimer = 1;
						twlsettings_launchslot1value = 2;
						fadeout = true;
						updatebotscreen = true;
						if (dspfirmfound) {
							bgm_menu.stop();
							sfx_launch.play();
						}
					}
				}
			//}
		} else if (screenmode == 1) {
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
					if (dspfirmfound) { sfx_select.play(); }
				} else if((hDown & KEY_DOWN) && twlsettingscursorPosition != 7){
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
						settings_locswitchvalue++; // Game location switcher
						if(settings_locswitchvalue == 2) {
							settings_locswitchvalue = 0;
						}
					} else if (settingscursorPosition == 2) {
						settings_topbordervalue++; // Top border
						if(settings_topbordervalue == 2) {
							settings_topbordervalue = 0;
						}
					} else if (settingscursorPosition == 3) {
						settings_countervalue++; // Game counter
						if(settings_countervalue == 2) {
							settings_countervalue = 0;
						}
					} else if (settingscursorPosition == 4) {
						settings_autoupdatevalue++; // Enable or disable autoupdate
						if(settings_autoupdatevalue == 3) {
							settings_autoupdatevalue = 0;
						}
					} else if (settingscursorPosition == 5) {
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
					} 
				} else if((hDown & KEY_DOWN) && settingscursorPosition != 5){
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
			APT_PrepareToDoApplicationJump(0, 0x0004800554574C44LL, 0); // TWL app's title ID
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
	if (colortexloaded == true) { sf2d_free_texture(dotcircletex); }
	if (colortexloaded == true) { sf2d_free_texture(startbordertex); }
	sf2d_free_texture(settingstex);
	sf2d_free_texture(dsboottex);
	sf2d_free_texture(dsiboottex);
	sf2d_free_texture(dshstex);
	sf2d_free_texture(dsihstex);
	sf2d_free_texture(whitescrtex);
	sf2d_free_texture(disabledtex);
	sf2d_free_texture(bnricontex1);
	sf2d_free_texture(bnricontex2);
	sf2d_free_texture(bnricontex3);
	sf2d_free_texture(bnricontex4);
	sf2d_free_texture(bnricontex5);
	sf2d_free_texture(bnricontex6);
	sf2d_free_texture(bnricontex7);
	sf2d_free_texture(bnricontex8);
	sf2d_free_texture(bnricontex9);
	sf2d_free_texture(bnricontex10);
	sf2d_free_texture(boxarttex1);
	sf2d_free_texture(boxarttex2);
	sf2d_free_texture(boxarttex3);
	sf2d_free_texture(boxarttex4);
	sf2d_free_texture(boxarttex5);
	sf2d_free_texture(boxarttex6);
	if (dspfirmfound) { ndspExit(); }
	sf2d_fini();

    return 0;
}