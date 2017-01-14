#include "download.h"
#include "date.h"
#include "log.h"
#include "ndsheaderbanner.h"

#include <cstdio>
#include <malloc.h>
#include <unistd.h>

#include <string>
#include <vector>
using std::string;
using std::vector;

#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>

// Functions and variables defined in main.cpp.
extern void DialogBoxAppear(const char *text);
extern void DialogBoxDisappear(const char *text);
extern sftd_font *font;
extern sftd_font *font_b;
extern sf2d_texture *dialogboxtex; // Dialog box
extern sf2d_texture *settingstex; // Bottom of settings screen
extern bool showdialogbox;
extern bool run;	// Set to false to exit to the Home Menu.

extern int screenmode;
// 0: ROM select
// 1: Settings

typedef struct {
	char text[13];
} sVerfile;

extern char settings_vertext[13];
extern char settings_latestvertext[13];

// Language
extern u8 language;

// Files
extern vector<string> files;
extern vector<string> fcfiles;

/**
 * Check Wi-Fi status.
 * @return True if Wi-Fi is connected; false if not.
 */
bool checkWifiStatus(void) {
	acInit();
	u32 wifiStatus;
	ACU_GetWifiStatus(&wifiStatus);
	bool res = false;

	if (R_SUCCEEDED(ACU_GetWifiStatus(&wifiStatus)) && wifiStatus) {
		LogFMA("WifiStatus", "Internet connetion active found", RetTime().c_str());
		res = true;
	} else {
		LogFMA("WifiStatus", "No Internet connetion active found", RetTime().c_str());
	}

	acExit();
	return res;
}

/**
 * Download a file.
 * @param url URL of the file.
 * @param file Local filename.
 * @param mediaType How the file should be handled.
 * @return 0 on success; non-zero on error.
 */
int downloadFile(const char* url, const char* file, MediaType mediaType) {
	if (!checkWifiStatus())
		return -1;

	acInit();
	fsInit();
	httpcInit(0x1000);
	u8 method = 0;
	httpcContext context;
	u32 statuscode=0;
	HTTPC_RequestMethod useMethod = HTTPC_METHOD_GET;

	if (method <= 3 && method >= 1) {
		useMethod = (HTTPC_RequestMethod)method;
	}

	do {
		if (statuscode >= 301 && statuscode <= 308) {
			char newurl[4096];
			httpcGetResponseHeader(&context, (char*)"Location", &newurl[0], 4096);
			url = &newurl[0];

			httpcCloseContext(&context);
		}

		Result ret = httpcOpenContext(&context, useMethod, (char*)url, 0);
		httpcSetSSLOpt(&context, SSLCOPT_DisableVerify);

		if (ret==0) {
			httpcBeginRequest(&context);
			u32 contentsize=0;
			httpcGetResponseStatusCode(&context, &statuscode);
			if (statuscode == 200) {
				u32 readSize = 0;
				long int bytesWritten = 0;

				Handle fileHandle;
				FS_Path filePath=fsMakePath(PATH_ASCII, file);
				FSUSER_OpenFileDirectly(&fileHandle, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""), filePath, FS_OPEN_CREATE | FS_OPEN_WRITE, 0x00000000);

				httpcGetDownloadSizeState(&context, NULL, &contentsize);
				u8* buf = (u8*)malloc(contentsize);
				memset(buf, 0, contentsize);

				do {
					ret = httpcDownloadData(&context, buf, contentsize, &readSize);
					FSFILE_Write(fileHandle, NULL, bytesWritten, buf, readSize, 0x10001);
					bytesWritten += readSize;
				} while (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING);

				FSFILE_Close(fileHandle);
				svcCloseHandle(fileHandle);

				if (mediaType != MEDIA_SD_FILE) {
					// This is a CIA, so we should install it.
					amInit();
					Handle handle;
					// FIXME: Should check the return values.
					switch (mediaType) {
						case MEDIA_SD_CIA:
							AM_QueryAvailableExternalTitleDatabase(NULL);
							AM_StartCiaInstall(MEDIATYPE_SD, &handle);
							break;
						case MEDIA_NAND_CIA:
							AM_StartCiaInstall(MEDIATYPE_NAND, &handle);
							break;
						default:
							break;
					}
					FSFILE_Write(handle, NULL, 0, buf, contentsize, 0);
					AM_FinishCiaInstall(handle);
					amExit();
				}

				free(buf);
			}
		}
	} while ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308));
	httpcCloseContext(&context);

	httpcExit();
	acExit();
	fsExit();
	return 0;
}

/**
 * Check for a TWLoader update.
 * @return 0 if an update is available; non-zero if up to date or an error occurred.
 */
int checkUpdate(void) {
	LogFM("checkUpdate", "Checking updates...");
	static const char title[] = "Now checking TWLoader version...";
	DialogBoxAppear(title);
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	if (screenmode == 1) {
		sf2d_draw_texture(settingstex, 0, 0);
	}
	sf2d_draw_texture(dialogboxtex, 0, 0);
	sftd_draw_textf(font, 12, 16, RGBA8(0, 0, 0, 255), 12, title);
	sf2d_end_frame();
	sf2d_swapbuffers();

	int res = downloadFile("https://www.dropbox.com/s/v00qw6unyzntsgn/ver?dl=1", "/_nds/twloader/ver", MEDIA_SD_FILE);
	if (res == 0) {
		sVerfile Verfile;

		FILE* VerFile = fopen("sdmc:/_nds/twloader/ver", "r");
		fread(&Verfile, 1, sizeof(Verfile), VerFile);
		strncpy(settings_latestvertext, Verfile.text, sizeof(settings_latestvertext));
		settings_latestvertext[sizeof(settings_latestvertext)-1] = 0;
		fclose(VerFile);
		LogFMA("checkUpdate", "Reading downloaded version:", settings_latestvertext);
		LogFMA("checkUpdate", "Reading ROMFS version:", settings_vertext);

		int updtequals = strcmp(settings_latestvertext, settings_vertext);

		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		if (screenmode == 1) {
			sf2d_draw_texture(settingstex, 0, 0);
		}
		sf2d_draw_texture(dialogboxtex, 0, 0);
		if (updtequals == 0){
			LogFMA("checkUpdate", "Comparing...", "Are equals");
			LogFM("checkUpdate", "TWLoader is up-to-date!");
			// FIXME: DialogBoxDiappear is freezing here,
			// so just hide the dialog.
			//DialogBoxDisappear("TWLoader is up-to-date.");
			showdialogbox = false;	// <-- so do this instead.
			return -1;
		}
		LogFMA("checkUpdate", "Comparing...", "NO equals");
		return 0;
	}

	LogFM("checkUpdate", "Problem downloading ver file!");
	return -1;
}

/**
 * Download the TWLoader CIAs.
 */
void DownloadTWLoaderCIAs(void) {
	sftd_draw_textf(font, 12, 16, RGBA8(0, 0, 0, 255), 12, "Now updating TWLoader to latest version...");
	sftd_draw_textf(font, 12, 30, RGBA8(0, 0, 0, 255), 12, "(GUI)");
	sf2d_end_frame();
	sf2d_swapbuffers();

	int res = downloadFile("https://www.dropbox.com/s/01vifhf49lkailx/TWLoader.cia?dl=1","/_nds/twloader/cia/TWLoader.cia", MEDIA_SD_CIA);
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	if (screenmode == 1) {
		sf2d_draw_texture(settingstex, 0, 0);
	}
	sf2d_draw_texture(dialogboxtex, 0, 0);
	if (res == 0) {
		sftd_draw_textf(font, 12, 16, RGBA8(0, 0, 0, 255), 12, "Now downloading latest TWLoader version...");
		sftd_draw_textf(font, 12, 30, RGBA8(0, 0, 0, 255), 12, "(TWLNAND side CIA)");
		sf2d_end_frame();
		sf2d_swapbuffers();
		// TODO: Use MEDIA_NAND_CIA.
		res = downloadFile("https://www.dropbox.com/s/jjb5u83pskrruij/TWLoader%20-%20TWLNAND%20side.cia?dl=1","/_nds/twloader/cia/TWLoader - TWLNAND side.cia", MEDIA_SD_FILE);
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		if (screenmode == 1) {
			sf2d_draw_texture(settingstex, 0, 0);
		}
		sf2d_draw_texture(dialogboxtex, 0, 0);
		if (res == 0) {
			sftd_draw_textf(font, 12, 16, RGBA8(0, 0, 0, 255), 12, "Now returning to HOME Menu...");
			sf2d_end_frame();
			sf2d_swapbuffers();
			run = false;
		} else {
			DialogBoxDisappear("Download failed.");
		}
	} else {
		DialogBoxDisappear("Update failed.");
	}
}

/**
 * Update nds-bootstrap to the latest unofficial build.
 */
void UpdateBootstrapUnofficial(void) {
	static const char title[] = "Now updating bootstrap (Unofficial)...";
	DialogBoxAppear(title);
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	if (screenmode == 1) {
		sf2d_draw_texture(settingstex, 0, 0);
	}
	sf2d_draw_texture(dialogboxtex, 0, 0);
	sftd_draw_textf(font, 12, 16, RGBA8(0, 0, 0, 255), 12, title);
	sf2d_end_frame();
	sf2d_swapbuffers();
	remove("sdmc:/_nds/bootstrap.nds");
	downloadFile("https://www.dropbox.com/s/m3jmxhr4b5tn1yi/bootstrap.nds?dl=1","/_nds/bootstrap.nds", MEDIA_SD_FILE);
	if (screenmode == 1) {
		DialogBoxDisappear("Done!");
	}
}

/**
 * Update nds-bootstrap to the latest release build.
 */
void UpdateBootstrapRelease(void) {
	static const char title[] = "Now updating bootstrap (Release)...";
	DialogBoxAppear(title);
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	if (screenmode == 1) {
		sf2d_draw_texture(settingstex, 0, 0);
	}
	sftd_draw_textf(font, 12, 16, RGBA8(0, 0, 0, 255), 12, title);
	sf2d_end_frame();
	sf2d_swapbuffers();
	remove("sdmc:/_nds/bootstrap.nds");
	downloadFile("https://www.dropbox.com/s/eb6e8nsa2eyjmb3/bootstrap.nds?dl=1","/_nds/bootstrap.nds", MEDIA_SD_FILE);
	if (screenmode == 1) {
		DialogBoxDisappear("Done!");
	}
}

/**
 * Download boxart.
 */
void downloadBoxArt(void)
{
	// European boxart languages.
	static const char ba_langs_eur[][4] = {
		"EN",	// Japanese (English used in place)
		"EN",	// English
		"FR",	// French
		"DE",	// German
		"IT",	// Italian
		"ES",	// Spanish
		"ZH",	// Simplified Chinese
		"KO",	// Korean
		"NL",	// Dutch
		"PT",	// Portugese
		"RU",	// Russian
		"TW"	// Traditional Chinese
	};

	char romsel_counter2sd[16];	// Number of ROMs on the SD card.
	snprintf(romsel_counter2sd, sizeof(romsel_counter2sd), "%d", files.size());

	LogFM("Main.downloadBoxArt", "Checking box art.");
	for (size_t boxartnum = 0; boxartnum < files.size(); boxartnum++) {
		static const char title[] = "Now checking box art if exists (SD Card)...";
		char romsel_counter1[16];
		snprintf(romsel_counter1, sizeof(romsel_counter1), "%d", boxartnum+1);
		DialogBoxAppear(title);
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(dialogboxtex, 0, 0);
		sftd_draw_textf(font, 12, 16, RGBA8(0, 0, 0, 255), 12, title);
		sftd_draw_textf(font, 12, 32, RGBA8(0, 0, 0, 255), 12, romsel_counter1);
		sftd_draw_textf(font, 31, 32, RGBA8(0, 0, 0, 255), 12, "/");
		sftd_draw_textf(font, 36, 32, RGBA8(0, 0, 0, 255), 12, romsel_counter2sd);
		sf2d_end_frame();
		sf2d_swapbuffers();

		const char *tempfile = files.at(boxartnum).c_str();
		char path[256];
		snprintf(path, sizeof(path), "sdmc:/roms/nds/%s", tempfile);
		FILE *f_nds_file = fopen(path, "rb");

		char ba_TID[5];
		grabTID(f_nds_file, ba_TID);
		ba_TID[4] = 0;
		fclose(f_nds_file);

		const char *ba_region;
		switch (ba_TID[3]) {
			case 'E':
			default:
				ba_region = "US";	// USA (default)
				break;
			case 'O':			// USA/Europe
			case 'P':			// Europe
				ba_region = ba_langs_eur[language];
				break;
			case 'D':
				ba_region = "DE";	// German
				break;
			case 'F':
				ba_region = "FR";	// French
				break;
			case 'I':
				ba_region = "IT";	// Italian
				break;
			case 'J':
				ba_region = "JA";	// Japanese
				break;
			case 'K':
				ba_region = "KO";	// Korean
				break;
			case 'S':
				ba_region = "ES";	// Spanish
				break;
		}

		char http_url[256];
		snprintf(http_url, sizeof(http_url), "http://art.gametdb.com/ds/coverS/%s/%.4s.png", ba_region, ba_TID);
		snprintf(path, sizeof(path), "/_nds/twloader/boxart/%.4s.png", ba_TID);
		if (access(path, F_OK) == -1) {
			LogFMA("Main.downloadBoxArt", "Downloading box art:", romsel_counter1);
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_draw_texture(dialogboxtex, 0, 0);
			sftd_draw_textf(font, 12, 16, RGBA8(0, 0, 0, 255), 12, "Now downloading box art (SD Card)...");
			sftd_draw_textf(font, 12, 32, RGBA8(0, 0, 0, 255), 12, romsel_counter1);
			sftd_draw_textf(font, 31, 32, RGBA8(0, 0, 0, 255), 12, "/");
			sftd_draw_textf(font, 36, 32, RGBA8(0, 0, 0, 255), 12, romsel_counter2sd);
			sf2d_end_frame();
			sf2d_swapbuffers();
			downloadFile(http_url, path, MEDIA_SD_FILE);
		}
	}

	LogFM("Main.downloadBoxArt", "Box arts downloaded correctly");
}
