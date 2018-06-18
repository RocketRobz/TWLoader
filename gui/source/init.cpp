#include "main.h"
#include "thread.h"
#include "dumpdsp.h"
#include "download.h"
#include "gamecard.h"
#include "ndsheaderbanner.h"
#include "language.h"
#include "log.h"
#include "settings.h"
#include "textfns.h"

#include <cstdio>
#include <malloc.h>
#include <unistd.h>

#include <algorithm>
#include <string>
#include <unordered_set>
#include <vector>
#include <sys/stat.h>
using std::string;
using std::unordered_set;
using std::vector;
using std::wstring;

#include <3ds.h>
#include <3ds/types.h>
#include "graphic.h"
#include "pp2d/pp2d.h"

#define CONFIG_3D_SLIDERSTATE (*(float *)0x1FF81080)

const char* musicpath = "romfs:/null.wav";
const char* init_textOnScreen = " ";

bool showdialogbox_init = true;
int initdbox_waitTime = 0;
int initdbox_setWaitTime = 0;
int initdbox_movespeed = 22;
int initdbox_Ypos = -240;
int initdbox_bgalpha = 0;

Handle initThreadRequest;

bool initDone = false;

void initStuffThread() {
	Result res = 0;

	// Register a handler for "returned from HOME Menu".
	aptHook(&rfhm_cookie, rfhm_callback, &bannertextloaded);

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

	snprintf(romsel_counter2sd, sizeof(romsel_counter2sd), "%zu", files.size());
	if (logEnabled)	LogFMA("Main.ROM scanning", "Number of ROMs on the SD card detected", romsel_counter2sd);
	
	snprintf(romsel_counter2fc, sizeof(romsel_counter2fc), "%zu", fcfiles.size());
	if (logEnabled)	LogFMA("Main.ROM scanning", "Number of ROMs on the flashcard detected", romsel_counter2fc);
	
	snprintf(romsel_counter2gb, sizeof(romsel_counter2gb), "%zu", gbfiles.size());
	if (logEnabled)	LogFMA("Main.ROM scanning", "Number of GB ROMs on the SD card detected", romsel_counter2gb);
	
	snprintf(romsel_counter2nes, sizeof(romsel_counter2nes), "%zu", nesfiles.size());
	if (logEnabled)	LogFMA("Main.ROM scanning", "Number of NES ROMs on the SD card detected", romsel_counter2nes);
	
	init_textOnScreen =
	"Checking WiFi status...\n"
	"\n"
	"If you see this for more than 25 seconds,\n"
	"try rebooting, then after launching TWLoader,\n"
	"hold ? to skip downloading missing files.\n"
	"\n"
	"If the issue persists, reboot, then do the same,\n"
	"and also hold  to turn on quick start.";

	if(!(hHeld & KEY_Y)) {
		// Download missing files
		if (checkWifiStatus() && (DownloadMissingFiles() == 0)) {
			// Nothing
		}
	}

	if(!settings.ui.quickStart) {
		init_textOnScreen =
		"Checking WiFi status...\n"
		"\n"
		"If you see this for more than 25 seconds,\n"
		"try rebooting, then after launching TWLoader,\n"
		"hold  to turn on quick start.";

		// Download box art
		if (checkWifiStatus()) {
			downloadBoxArt();
		}
	
		init_textOnScreen = "Now checking banner data (SD Card)...";

		// Cache banner data for ROMs on the SD card.
		// TODO: Re-cache if it's 0 bytes?
		for (bnriconnum = 0; bnriconnum < (int)files.size(); bnriconnum++) {
			init_textOnScreen = "Now checking banner data (SD Card)...";
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
			
			if(cacheBanner(f_nds_file, tempfile, romsel_counter1, romsel_counter2sd, isCia) != 0) {
				if (logEnabled)	LogFMA("Main.Banner scanning", "Error reading banner from file", nds_path);
			}
			
			fclose(f_nds_file);
		}
	
		init_textOnScreen =
		"Checking WiFi status...\n"
		"\n"
		"If you see this for more than 25 seconds,\n"
		"try rebooting, then after launching TWLoader,\n"
		"hold  to turn on quick start.";

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

	showdialogbox = false;
	init_textOnScreen = " ";
	
	initDone = true;
}

/**
 * Play DS boot splash.
 */
void initStuff() {
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

	snprintf(settings_vertext, 13, "Ver. %d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);
	std::string version = settings_vertext;	
	int vertext_xPos;
	if (version.substr(version.find_first_not_of(' '), (version.find_last_not_of(' ') - version.find_first_not_of(' ') + 1)).size() > 8) {
		vertext_xPos = 324;
	}else{
		vertext_xPos = 336;
	}

	if (logEnabled)	LogFMA("Main.GUI version", "GUI version", settings_vertext);

	pp2d_load_texture_png(dsboottex, "romfs:/graphics/init/bg.png"); // Init background

	int aninumfadealpha = 0;
	int showAnniversaryTextYPos = 100;

	float initBG_pos = 0.0;

	if(isDemo)
		pp2d_load_texture_png(twloaderlogotex, "romfs:/graphics/logo/logo_demo.png"); // TWLoader (3DSX demo version) logo on top screen
	else
		pp2d_load_texture_png(twloaderlogotex, "romfs:/graphics/logo/logo.png"); // TWLoader logo on top screen

	// Dialog box textures.
	pp2d_load_texture_png(dialogboxtex, "romfs:/graphics/dialogbox.png"); // Dialog box
	pp2d_load_texture_png(dboxtex_iconbox, "romfs:/graphics/dbox/iconbox.png"); // Icon box
	pp2d_load_texture_png(dboxtex_button, "romfs:/graphics/dbox/button.png"); // Icon box
	pp2d_load_texture_png(dboxtex_buttonback, "romfs:/graphics/dbox/button_back.png"); // Icon box

	svcCreateEvent(&initThreadRequest,(ResetType)0);
	createThread((ThreadFunc)initStuffThread);

	while(aptMainLoop()) {
		if (showdialogbox_init) {
			if (initdbox_movespeed <= 1) {
				if (initdbox_Ypos >= 0) {
					initdbox_movespeed = 0;
					initdbox_Ypos = 0;
				} else
					initdbox_movespeed = 1;
			} else {
				initdbox_movespeed -= 0.2;
				initdbox_bgalpha += 5;
			}
			initdbox_Ypos += initdbox_movespeed;
		} else {
			if (initdbox_Ypos <= -240 || initdbox_Ypos >= 240) {
				initdbox_movespeed = 22;
				initdbox_Ypos = -240;
			} else {
				initdbox_movespeed += 1;
				initdbox_bgalpha -= 5;
				if (initdbox_bgalpha <= 0) {
					initdbox_bgalpha = 0;
				}
				initdbox_Ypos += initdbox_movespeed;
			}
		}

		pp2d_begin_draw(GFX_TOP, GFX_LEFT);
		for (int h = 0; h < 10; h++) {
			for (int v = 0; v < 6; v++) {
				pp2d_draw_texture(dsboottex, -48+initBG_pos+(h*48), -initBG_pos+(v*48));
			}
		}
		pp2d_draw_texture(twloaderlogotex, 400/2 - 256/2, 240/2 - 128/2); // 400/2 - height/2, 240/2 - width/2
		pp2d_draw_text(vertext_xPos, 222, 0.60, 0.60f, BLACK, settings_vertext);
		pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
		for (int h = 0; h < 9; h++) {
			for (int v = 0; v < 6; v++) {
				pp2d_draw_texture(dsboottex, -88+initBG_pos+(h*48), -initBG_pos+(v*48));
			}
		}
		if (initdbox_Ypos != -240) {
			// Draw the dialog box.
			pp2d_draw_rectangle(0, 0, 320, 240, RGBA8(0, 0, 0, initdbox_bgalpha)); // Fade in/out effect
			pp2d_draw_texture(dialogboxtex, 0, initdbox_Ypos);
			pp2d_draw_text(12, 16+initdbox_Ypos, 0.50, 0.50, BLACK, init_textOnScreen);
		}
		pp2d_end_draw();

		if(initdbox_setWaitTime != 0) {
			initdbox_waitTime++;
			if (initdbox_waitTime == initdbox_setWaitTime) {
				initdbox_setWaitTime = 0;
			}
		} else {
			initdbox_waitTime = 0;
		}

		initBG_pos += 0.50f;
		if(initBG_pos == 48.0) initBG_pos = 0.0;

		screenon();
		
		if (initDone) {
			showdialogbox_init = false;
			if (initdbox_Ypos == -240) break;
		}
	}

	/*pp2d_load_texture_png(anniversarytex, "romfs:/graphics/anniversary/text.png"); // TWLoader logo on top screen
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

	*/

}
