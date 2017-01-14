#include "download.h"
#include "date.h"
#include "log.h"

#include <cstdio>
#include <malloc.h>

#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>

// Functions and variables defined in main.cpp.
extern void DialogueBoxAppear(void);
extern void DialogueBoxDisappear(void);
extern sftd_font *font;
extern sftd_font *font_b;
extern sf2d_texture *dialogueboxtex; // Dialogue box
extern sf2d_texture *settingstex; // Bottom of settings screen
extern bool showdialoguebox;
extern const char* dialoguetext;
extern bool run;	// Set to false to exit to the Home Menu.

extern int screenmode;
// 0: ROM select
// 1: Settings

typedef struct {
	char text[13];
} sVerfile;

extern char settings_vertext[13];
extern char settings_latestvertext[13];

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
	dialoguetext = "Now checking TWLoader version...";
	DialogueBoxAppear();
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	if (screenmode == 1) {
		sf2d_draw_texture(settingstex, 0, 0);
	}
	sf2d_draw_texture(dialogueboxtex, 0, 0);
	sftd_draw_textf(font, 12, 16, RGBA8(0, 0, 0, 255), 12, dialoguetext);
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
		sf2d_draw_texture(dialogueboxtex, 0, 0);
		if (updtequals == 0){
			LogFMA("checkUpdate", "Comparing...", "Are equals");
			LogFM("checkUpdate", "TWLoader is up-to-date!");
			dialoguetext = "TWLoader is up-to-date.";
			//DialogueBoxDisappear(); <-- this is causing a freeze only in this function.
			showdialoguebox = false;	// <-- so do this instead.
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
	sf2d_draw_texture(dialogueboxtex, 0, 0);
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
		sf2d_draw_texture(dialogueboxtex, 0, 0);
		if (res == 0) {
			sftd_draw_textf(font, 12, 16, RGBA8(0, 0, 0, 255), 12, "Now returning to HOME Menu...");
			sf2d_end_frame();
			sf2d_swapbuffers();
			run = false;
		} else {
			dialoguetext = "Download failed.";
			DialogueBoxDisappear();
		}
	} else {
		dialoguetext = "Update failed.";
		DialogueBoxDisappear();
	}
}

/**
 * Update nds-bootstrap to the latest unofficial build.
 */
void UpdateBootstrapUnofficial(void) {
	dialoguetext = "Now updating bootstrap (Unofficial)...";
	DialogueBoxAppear();
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	if (screenmode == 1) {
		sf2d_draw_texture(settingstex, 0, 0);
	}
	sf2d_draw_texture(dialogueboxtex, 0, 0);
	sftd_draw_textf(font, 12, 16, RGBA8(0, 0, 0, 255), 12, dialoguetext);
	sf2d_end_frame();
	sf2d_swapbuffers();
	remove("sdmc:/_nds/bootstrap.nds");
	downloadFile("https://www.dropbox.com/s/m3jmxhr4b5tn1yi/bootstrap.nds?dl=1","/_nds/bootstrap.nds", MEDIA_SD_FILE);
	dialoguetext = "Done!";
	if (screenmode == 1) {
		DialogueBoxDisappear();
	}
}

/**
 * Update nds-bootstrap to the latest release build.
 */
void UpdateBootstrapRelease(void) {
	dialoguetext = "Now updating bootstrap (Release)...";
	DialogueBoxAppear();
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	if (screenmode == 1) {
		sf2d_draw_texture(settingstex, 0, 0);
	}
	sftd_draw_textf(font, 12, 16, RGBA8(0, 0, 0, 255), 12, dialoguetext);
	sf2d_end_frame();
	sf2d_swapbuffers();
	remove("sdmc:/_nds/bootstrap.nds");
	downloadFile("https://www.dropbox.com/s/eb6e8nsa2eyjmb3/bootstrap.nds?dl=1","/_nds/bootstrap.nds", MEDIA_SD_FILE);
	dialoguetext = "Done!";
	if (screenmode == 1) {
		DialogueBoxDisappear();
	}
}
