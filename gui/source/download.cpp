#include "download.h"
#include "date.h"
#include "inifile.h"
#include "log.h"
#include "ndsheaderbanner.h"
#include "settings.h"
#include "gamecard.h"

#include <cstdio>
#include <malloc.h>
#include <unistd.h>

#include <algorithm>
#include <string>
#include <unordered_set>
#include <vector>
using std::string;
using std::unordered_set;
using std::vector;

#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>

// Functions and variables defined in main.cpp.
extern void DialogBoxAppear(const char *text, int mode);
extern void DialogBoxDisappear(const char *text, int mode);
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
	u32 wifiStatus;
	bool res = false;

	if (R_SUCCEEDED(ACU_GetWifiStatus(&wifiStatus)) && wifiStatus) {
		LogFMA("WifiStatus", "Active internet connection found", RetTime().c_str());
		res = true;
	} else {
		LogFMA("WifiStatus", "No Internet connection found!", RetTime().c_str());
	}

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
	DialogBoxAppear(title, 0);
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	if (screenmode == 1) {
		sf2d_draw_texture(settingstex, 0, 0);
	}
	sf2d_draw_texture(dialogboxtex, 0, 0);
	sftd_draw_textf(font, 12, 16, RGBA8(0, 0, 0, 255), 12, title);
	sf2d_end_frame();
	sf2d_swapbuffers();

	int res = downloadFile("https://www.dropbox.com/s/v00qw6unyzntsgn/ver?dl=1", "/_nds/twloader/ver", MEDIA_SD_FILE);
	LogFM("checkUpdate", "downloadFile() end");
	if (res == 0) {
		sVerfile Verfile;

		bool isUnknown = false;
		FILE* VerFile = fopen("sdmc:/_nds/twloader/ver", "r");
		if (VerFile) {
			fread(&Verfile, 1, sizeof(Verfile), VerFile);
			strcpy(settings_latestvertext, Verfile.text);
			fclose(VerFile);
		} else {
			// Unable to open the version file.
			strcpy(settings_latestvertext, "Unknown");
			isUnknown = true;
		}
		LogFMA("checkUpdate", "Reading downloaded version:", settings_latestvertext);
		LogFMA("checkUpdate", "Reading ROMFS version:", settings_vertext);

		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		if (screenmode == 1) {
			sf2d_draw_texture(settingstex, 0, 0);
		}
		sf2d_draw_texture(dialogboxtex, 0, 0);
		if (!isUnknown && !strcmp(settings_latestvertext, settings_vertext)) {
			// Version is different.
			LogFMA("checkUpdate", "Comparing...", "Are equals");
		
			if (screenmode == 1) {				
				showdialogbox = false;
			}else{
				sf2d_end_frame();
				sf2d_swapbuffers();
			}
			
			LogFM("checkUpdate", "TWLoader is up-to-date!");			
			return -1;
		}
		LogFMA("checkUpdate", "Comparing...", "NO equals");
		showdialogbox = false;
		return 0;
	}
	LogFM("checkUpdate", "Problem downloading ver file!");
	showdialogbox = false;
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
		res = downloadFile("https://www.dropbox.com/s/jjb5u83pskrruij/TWLoader%20-%20TWLNAND%20side.cia?dl=1","/_nds/twloader/cia/TWLoader - TWLNAND side.cia", MEDIA_NAND_CIA);
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
			DialogBoxDisappear("Download failed.", 0);
		}
	} else {
		DialogBoxDisappear("Update failed.", 0);
	}
}

/**
 * Update nds-bootstrap to the latest unofficial build.
 */
void UpdateBootstrapUnofficial(void) {
	static const char title[] = "Now updating bootstrap (Unofficial)...";
	DialogBoxAppear(title, 0);
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
		DialogBoxDisappear("Done!", 0);
	}
}

/**
 * Update nds-bootstrap to the latest release build.
 */
void UpdateBootstrapRelease(void) {
	static const char title[] = "Now updating bootstrap (Release)...";
	DialogBoxAppear(title, 0);
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	if (screenmode == 1) {
		sf2d_draw_texture(settingstex, 0, 0);
	}
	sf2d_draw_texture(dialogboxtex, 0, 0);
	sftd_draw_textf(font, 12, 16, RGBA8(0, 0, 0, 255), 12, title);
	sf2d_end_frame();
	sf2d_swapbuffers();
	remove("sdmc:/_nds/bootstrap.nds");
	downloadFile("https://www.dropbox.com/s/eb6e8nsa2eyjmb3/bootstrap.nds?dl=1","/_nds/bootstrap.nds", MEDIA_SD_FILE);
	if (screenmode == 1) {
		DialogBoxDisappear("Done!", 0);
	}
}

/**
 * Look up a GameTDB region code.
 * @param tid_region Region character from the TID.
 * @param ba_region_fallback Pointer to const char* to store the fallback region.
 * @return GameTDB region code.
 */
static const char *getGameTDBRegion(u8 tid_region, const char **pFallback)
{
	// European boxart languages.
	static const char *const ba_langs_eur[] = {
		"EN",	// Japanese (English used in place)
		"EN",	// English
		"FR",	// French
		"DE",	// German
		"IT",	// Italian
		"ES",	// Spanish
		"ZHCN",	// Simplified Chinese
		"KO",	// Korean
		"NL",	// Dutch
		"PT",	// Portugese
		"RU",	// Russian
		"ZHTW"	// Traditional Chinese
	};

	const char *ba_region_fallback = NULL;
	const char *ba_region;

	// Get the system region.
	// This is needed in order to differentiate the 'O' region code
	// for USA and Europe.
	static u8 region = 0xFF;
	if (region == 0xFF) {
		int res = CFGU_SecureInfoGetRegion(&region);
		if (res != 0) {
			// Can't get the region. Assume USA.
			region = CFG_REGION_USA;
		}
	}

	switch (tid_region) {
		case 'E':
			ba_region = "US";	// USA
			break;
		case 'J':
			ba_region = "JA";	// Japanese
			break;
		case 'K':
			ba_region = "KO";	// Korean
			break;

		case 'O':			// USA/Europe
			if (region == CFG_REGION_USA) {
				// System is USA region.
				// Get the USA boxart if it's available.
				ba_region = "US";
				ba_region_fallback = "EN";
				break;
			}
			// fall-through
		case 'P':			// Europe
		default:
			// System is not USA region.
			// Get the European boxart that matches the system's language.
			// TODO: Check country code for Australia.
			// This requires parsing the Config savegame. (GetConfigInfoBlk2())
			// Reference: https://3dbrew.org/wiki/Config_Savegame
			ba_region = ba_langs_eur[language];
			if (strcmp(ba_region, "EN") != 0) {
				// Fallback to EN if the specified language isn't available.
				ba_region_fallback = "EN";
			}
			break;

		case 'U':
			// Alternate country code for Australia.
			ba_region = "AU";
			ba_region_fallback = "EN";
			break;

		// European country-specific localizations.
		case 'D':
			ba_region = "DE";	// German
			ba_region_fallback = "EN";
			break;
		case 'F':
			ba_region = "FR";	// French
			ba_region_fallback = "EN";
			break;
		case 'H':
			ba_region = "NL";	// Dutch
			ba_region_fallback = "EN";
			break;
		case 'I':
			ba_region = "IT";	// Italian
			ba_region_fallback = "EN";
			break;
		case 'R':
			ba_region = "RU";	// Russian
			ba_region_fallback = "EN";
			break;
		case 'S':
			ba_region = "ES";	// Spanish
			ba_region_fallback = "EN";
			break;
	}

	if (pFallback) {
		*pFallback = ba_region_fallback;
	}
	return ba_region;
}

/**
 * Download a single boxart from GameTDB.
 * @param ba_TID Game ID. (4 characters, NULL-terminated)
 * @return 0 on success; non-zero on error.
 */
static int downloadBoxArt_internal(const char *ba_TID)
{
	char path[256];
	char http_url[256];
	LogFMA("downloadBoxArt_internal", "Downloading boxart for TID", ba_TID);

	const char *ba_region_fallback = NULL;
	const char *ba_region = getGameTDBRegion(ba_TID[3], &ba_region_fallback);
	if (!ba_region)
		return -1;

	// NOTE: downloadFile() doesn't use devoptab,
	// so don't prefix the filename with sdmc:/.
	snprintf(path, sizeof(path), "/_nds/twloader/boxart/%.4s.png", ba_TID);
	snprintf(http_url, sizeof(http_url), "http://art.gametdb.com/ds/coverS/%s/%.4s.png",
		 ba_region, ba_TID);
	int res = downloadFile(http_url, path, MEDIA_SD_FILE);
	if (res != 0 && ba_region_fallback != NULL) {
		// Try the fallback region.
		snprintf(http_url, sizeof(http_url), "http://art.gametdb.com/ds/coverS/%s/%.4s.png",
			 ba_region_fallback, ba_TID);
		res = downloadFile(http_url, path, MEDIA_SD_FILE);
	}
	return res;
}

/**
 * Download Slot-1 boxart.
 */
void downloadSlot1BoxArt(const char* TID)
{
	LogFM("Main.downloadSlot1BoxArt", "Checking box art (Slot-1 card).");
	
	char ba_TID[5];
	snprintf(ba_TID, sizeof(ba_TID), "%.4s", TID);
	ba_TID[4] = 0;

	char path[256];
	snprintf(path, sizeof(path), "/_nds/twloader/boxart/flashcard/%.4s.png", ba_TID);
	if (!access(path, F_OK)) {
		// File already exists.
		return;
	}

	LogFM("Main.downloadSlot1BoxArt", "Downloading box art (Slot-1 card)");
	downloadBoxArt_internal(ba_TID);
}

/**
 * Download boxart.
 */
void downloadBoxArt(void)
{
	char path[256];

	// First, check if we're missing any boxart on the SD card.
	unordered_set<u32> boxart_all_tids;	// Title IDs of all ROM files.
	vector<u32> boxart_dl_tids;	// Title IDs of boxart to download.
	boxart_all_tids.reserve(files.size() + fcfiles.size());
	boxart_dl_tids.reserve(files.size() + fcfiles.size());
	LogFM("Download.downloadBoxArt", "Checking missing boxart on SD card...");

	// Check if we're missing any boxart for ROMs on the SD card.
	for (size_t boxartnum = 0; boxartnum < files.size(); boxartnum++) {
		// Get the title ID from the ROM image.
		const char *tempfile = files.at(boxartnum).c_str();
		snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.romfolder.c_str(), tempfile);
		FILE *f_nds_file = fopen(path, "rb");
		if (!f_nds_file)
			continue;
		
		LogFMA("DownloadBoxArt.SD CARD", "Path: ", path);
		char ba_TID[5];
		grabTID(f_nds_file, ba_TID);
		ba_TID[4] = 0;
		fclose(f_nds_file);

		// Did we already check for this boxart?
		// NOTE: Storing byteswapped in order to sort correctly.
		u32 tid;
		memcpy(&tid, ba_TID, sizeof(tid));
		tid = __builtin_bswap32(tid);
		if (tid == 0x4E54524A) {
			// NTRJ: Generic game code used for
			// prototypes and some DL Play games.
			// No boxart is available.
			continue;
		}
		if (boxart_all_tids.find(tid) != boxart_all_tids.end()) {
			// Already checked for this boxart.
			continue;
		}
		boxart_all_tids.insert(tid);

		// Does this boxart file already exist?
		snprintf(path, sizeof(path), "sdmc:/_nds/twloader/boxart/%.4s.png", ba_TID);
		if (!access(path, F_OK)) {
			// Boxart file exists.
			continue;
		}

		// Boxart file does not exist. Download it.
		boxart_dl_tids.push_back(tid);
	}
	
	// Check if we're missing any boxart for ROMs on the flashcard.
	for (size_t boxartnum = 0; boxartnum < fcfiles.size(); boxartnum++) {
		// Get the title ID from the INI file.
		const char *tempfile = fcfiles.at(boxartnum).c_str();
		CIniFile setfcrompathini(tempfile);
		std::string ba_TID = setfcrompathini.GetString("FLASHCARD-ROM", "TID", "");
		if (ba_TID.size() < 4) {
			// Invalid TID.
			continue;
		}
		
		LogFMA("DownloadBoxArt.FLASHCARD", "TID: ", ba_TID.c_str());		
		// Did we already check for this boxart?
		// NOTE: Storing byteswapped in order to sort correctly.
		u32 tid;
		memcpy(&tid, ba_TID.c_str(), sizeof(tid));
		tid = __builtin_bswap32(tid);
		if (tid == 0x4E54524A) {
			// NTRJ: Generic game code used for
			// prototypes and some DL Play games.
			// No boxart is available.
			continue;
		}
		if (boxart_all_tids.find(tid) != boxart_all_tids.end()) {
			// Already checked for this boxart.
			continue;
		}
		boxart_all_tids.insert(tid);

		// Does this boxart file already exist?
		snprintf(path, sizeof(path), "sdmc:/_nds/twloader/boxart/%.4s.png", ba_TID.c_str());
		if (!access(path, F_OK)) {
			// Boxart file exists.
			continue;
		}

		// Boxart file does not exist. Download it.
		boxart_dl_tids.push_back(tid);
	}

	// Check if we're missing boxart for the Slot-1 cartridge.
	gamecardPoll(true);
	const char *const card_gameID = gamecardGetGameID();
	if (card_gameID) {
		// Did we already check for this card?
		// NOTE: Storing byteswapped in order to sort correctly.
		u32 tid;
		memcpy(&tid, card_gameID, sizeof(tid));
		tid = __builtin_bswap32(tid);
		if (boxart_all_tids.find(tid) == boxart_all_tids.end()) {
			// Boxart wasn't checked yet.
			boxart_all_tids.insert(tid);

			// Does this boxart file already exist?
			snprintf(path, sizeof(path), "sdmc:/_nds/twloader/boxart/%.4s.png", card_gameID);
			if (access(path, F_OK) != 0) {
				// Boxart file does not exist. Download it.
				boxart_dl_tids.push_back(tid);
			}
		}
	}
	
	LogFM("DownloadBoxArt.Read_BoxArt", "Boxart read correctly.");
	if (boxart_dl_tids.empty()) {
		// No boxart to download.
		LogFM("Download.downloadBoxArt", "No boxart to download.");
		return;
	}

	// Sort the TIDs for convenience purposes.
	std::sort(boxart_dl_tids.begin(), boxart_dl_tids.end());

	// Download the boxart.
	char s_boxart_total[12];
	snprintf(s_boxart_total, sizeof(s_boxart_total), "%zu", boxart_dl_tids.size());
	LogFM("DownloadBoxArt.downloading_process", "Downloading missing boxart");
	for (size_t boxartnum = 0; boxartnum < boxart_dl_tids.size(); boxartnum++) {
		static const char title[] = "Downloading missing boxart...";

		// Convert the TID back to char.
		char ba_TID[5];
		u32 tid = __builtin_bswap32(boxart_dl_tids[boxartnum]);
		memcpy(ba_TID, &tid, 4);
		ba_TID[4] = 0;
		
		// Show the dialog.
		DialogBoxAppear(title, 0);
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(dialogboxtex, 0, 0);
		sftd_draw_text(font, 12, 16, RGBA8(0, 0, 0, 255), 12, title);
		sftd_draw_textf(font, 12, 32, RGBA8(0, 0, 0, 255), 12, "%zu", boxartnum);
		sftd_draw_text(font, 39, 32, RGBA8(0, 0, 0, 255), 12, "/");
		sftd_draw_text(font, 44, 32, RGBA8(0, 0, 0, 255), 12, s_boxart_total);
		sftd_draw_textf(font, 12, 64, RGBA8(0, 0, 0, 255), 12, "Downloading: %.4s", ba_TID);
		sf2d_end_frame();
		sf2d_swapbuffers();

		downloadBoxArt_internal(ba_TID);
	}
}
