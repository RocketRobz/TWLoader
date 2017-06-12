#include "download.h"
#include "main.h"
#include "language.h"
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
#include <sys/stat.h>
using std::string;
using std::unordered_set;
using std::vector;

#include <3ds.h>
#include <sf2d.h>
#include "citrostuff.h"
#include "json/json.h"

const char* JSON_URL = "https://raw.githubusercontent.com/Jolty95/TWLoader-update/master/update.json";
bool updateGUI = false;
bool updateNAND = false;
bool updateNAND_part2 = false;
bool updateNAND_STG2 = false;
bool updateACE_RPG = false;
bool updateGBARUNNER_2 = false;
bool updateLOADCARD_DSTT = false;
bool updateR4 = false;
//bool doCleanInstall = true;

std::string gui_url;
std::string gui_3dsx_url;
std::string gui_smdh_url;
std::string demo_gui_3dsx_url;
std::string demo_gui_smdh_url;
std::string nand_url;
std::string nand_part2_url;
std::string nand_twld_url;
std::string ace_rpg_url;
std::string gbarunner2_url;
std::string loadcard_dstt_url;
std::string r4_url;
std::string release_BS_ver;
std::string unofficial_BS_ver;
std::string release_BS_url;
std::string unofficial_BS_url;

/**
 * Check Wi-Fi status.
 * @return True if Wi-Fi is connected; false if not.
 */
bool checkWifiStatus(void) {
	u32 wifiStatus;
	bool res = false;

	if (R_SUCCEEDED(ACU_GetWifiStatus(&wifiStatus)) && wifiStatus) {
		if (logEnabled)	LogFMA("WifiStatus", "Active internet connection found", RetTime(true).c_str());
		res = true;
	} else {
		if (logEnabled)	LogFMA("WifiStatus", "No Internet connection found!", RetTime(true).c_str());
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
	httpcContext context;
	u32 statuscode = 0;
	HTTPC_RequestMethod useMethod = HTTPC_METHOD_GET;

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
			if(R_SUCCEEDED(httpcBeginRequest(&context))){
				u32 contentsize=0;
				if(R_FAILED(httpcGetResponseStatusCode(&context, &statuscode))){
					if (logEnabled) LogFM("downloadFile.error", "An error has ocurred trying to get response status code.");
					httpcCloseContext(&context);
					httpcExit();
					fsExit();
					return -1;
				}
				if (statuscode == 200) {
					u32 readSize = 0;
					long int bytesWritten = 0;

					Handle fileHandle;
					FS_Path filePath=fsMakePath(PATH_ASCII, file);
					FSUSER_OpenFileDirectly(&fileHandle, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""), filePath, FS_OPEN_CREATE | FS_OPEN_WRITE, 0x00000000);

					if(R_FAILED(httpcGetDownloadSizeState(&context, NULL, &contentsize))){
						if (logEnabled) LogFM("downloadFile.error", "An error has ocurred trying to get download size.");
						httpcCloseContext(&context);
						httpcExit();
						fsExit();
						return -1;
					}
					u8* buf = (u8*)malloc(contentsize);
					memset(buf, 0, contentsize);

					do {
						if(R_FAILED(ret = httpcDownloadData(&context, buf, contentsize, &readSize))){
							// In case there is an error
							if (logEnabled) LogFM("downloadFile.error", "An error has ocurred while downloading data.");
							free(buf);
							httpcCloseContext(&context);
							httpcExit();
							fsExit();
							return -1;
						}
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
				}  else if ( ((statuscode >= 400) && (statuscode <= 451)) || ((statuscode >= 500) && (statuscode <= 512)) ) {
					// 4XX client error.
					// 5XX server error.
					httpcCloseContext(&context);
					char errorcode_s[4];
					snprintf(errorcode_s, sizeof(errorcode_s), "%lu", statuscode);
					if (logEnabled) LogFMA("downloadFile.error", "Error accessing resource.", errorcode_s);
					httpcExit();
					fsExit();
					return -1;
				}
			}else{
				// There was an error begining the request
				if (logEnabled) LogFM("downloadFile.error", "An error has ocurred trying to request server.");
				httpcCloseContext(&context);
				httpcExit();
				fsExit();
				return -1;
			}
		}else{
			// There was a problem opening HTTP context
			if (logEnabled) LogFMA("downloadFile.error", "An error has ocurred trying to open HTTP context.", url);
			httpcCloseContext(&context);
			httpcExit();
			fsExit();
			return -1;
		}
	} while ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308));
	httpcCloseContext(&context);

	httpcExit();
	fsExit();
	return 0;
}

Result http_read_internal(httpcContext* context, u32* bytesRead, void* buffer, u32 size) {
    if(context == NULL || buffer == NULL) {
        return -1;
    }

    Result res = httpcDownloadData(context, (u8*) buffer, size, bytesRead);
    return res != (int) HTTPC_RESULTCODE_DOWNLOADPENDING ? res : 0;
}

struct ReadJson {
    std::string strvalue1;
    std::string strvalue2;
    std::string strvalue3;
    std::string strvalue4;
    std::string strvalue5;
    std::string strvalue6;
    std::string strvalue7;
    std::string strvalue8;
};

ReadJson internal_json_reader(json_value* json, json_value* val,
								std::string str1,
								std::string str2,
								std::string str3,
								std::string str4,
								std::string str5,
								std::string str6,
								std::string str7,
								std::string str8)
{
	ReadJson rj;
	for(u32 i = 0; i < (json->u.object.length -1); i++) {
		// Create two variables that will store the values and it size
		char* name = val->u.object.values[i].name;
		u32 nameLen = val->u.object.values[i].name_length;
		json_value* subVal = val->u.object.values[i].value;
		if(subVal->type == json_string) {
			if(strncmp(name, str1.c_str(), nameLen) == 0) {
				rj.strvalue1 = subVal->u.string.ptr;
			} else if(strncmp(name, str2.c_str(), nameLen) == 0) {
				rj.strvalue2 = subVal->u.string.ptr;
			} else if(strncmp(name, str3.c_str(), nameLen) == 0) {
				rj.strvalue3 = subVal->u.string.ptr;
			} else if(strncmp(name, str4.c_str(), nameLen) == 0) {
				rj.strvalue4 = subVal->u.string.ptr;
			} else if(strncmp(name, str5.c_str(), nameLen) == 0) {
				rj.strvalue5 = subVal->u.string.ptr;
			} else if(strncmp(name, str6.c_str(), nameLen) == 0) {
				rj.strvalue6 = subVal->u.string.ptr;
			} else if(strncmp(name, str7.c_str(), nameLen) == 0) {
				rj.strvalue7 = subVal->u.string.ptr;
			} else if(strncmp(name, str8.c_str(), nameLen) == 0) {
				rj.strvalue8 = subVal->u.string.ptr;
			}
		}
	}
	return rj;
}

/**
 * Check for a TWLoader update.
 * @return 0 if an update is available; non-zero if up to date or an error occurred.
 */
int checkUpdate(void) {
	if (logEnabled)	LogFM("checkUpdate", "Checking updates...");
	static const char title[] = "Now checking TWLoader version...";
	DialogBoxAppear(12, 16, title);
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	if (screenmode == SCREEN_MODE_SETTINGS) {
		sf2d_draw_texture(settingstex, 0, 0);
	}
	sf2d_draw_texture(dialogboxtex, 0, 0);
	setTextColor(RGBA8(0, 0, 0, 255));
	renderText(12, 16, 0.5f, 0.5f, false, title);
	sf2d_end_frame();
	sf2d_swapbuffers();
	
	u32 responseCode = 0;
	httpcContext context;	
	
	httpcInit(0);
	if(R_FAILED(httpcOpenContext(&context, HTTPC_METHOD_GET, JSON_URL, 0))) {
		if (logEnabled)	LogFM("checkUpdate", "Error opening context.");
		return -1;
	}
	if(R_FAILED(httpcAddRequestHeaderField(&context, "User-Agent", "TWLoader"))) {
		if (logEnabled)	LogFM("checkUpdate", "Error requesting header field.");
		return -1;
	}
	if(R_FAILED(httpcSetSSLOpt(&context, SSLCOPT_DisableVerify))) {
		if (logEnabled)	LogFM("checkUpdate", "Error setting SSL certificate.");
		return -1;
	}
	if(R_FAILED(httpcSetKeepAlive(&context, HTTPC_KEEPALIVE_ENABLED))) {
		if (logEnabled)	LogFM("checkUpdate", "Error while keeping alive the conection.");
		return -1;
	}
	if(R_FAILED(httpcBeginRequest(&context))) {
		if (logEnabled)	LogFM("checkUpdate", "Error begining the request.");
		return -1;
	}
	if(R_FAILED(httpcGetResponseStatusCode(&context, &responseCode))) {
		if (logEnabled)	LogFM("checkUpdate", "Error getting response code.");
		return -1;
	}
    if (responseCode != 200) {
		if (logEnabled)	LogFM("checkUpdate", "Error reaching the update.json file.");
		return -1;
	}
	
	u32 size = 0;
	httpcGetDownloadSizeState(&context, NULL, &size);	
	char* jsonText = (char*) calloc(sizeof(char), size);
	if (logEnabled) LogFM("checkUpdate", "Downloading JSON info.");
	if(jsonText != NULL) {
		u32 bytesRead = 0;
		http_read_internal(&context, &bytesRead, (u8*) jsonText, size);
		json_value* json = json_parse(jsonText, size);
		if (logEnabled) LogFM("checkUpdate", "JSON read.");

		if(json != NULL) {
			if(json->type == json_object) { // {} are objects, [] are arrays				
				if (logEnabled) LogFM("checkUpdate", "JSON main object read.");
				
				char read_gui_major[3];
				char read_gui_minor[3];
				char read_gui_micro[3];
				
				char read_nand_major[3];
				char read_nand_minor[3];
				char read_nand_micro[3];
				
				char read_nandpart2_major[3];
				char read_nandpart2_minor[3];
				char read_nandpart2_micro[3];
				
				char read_nandstg2_major[3];
				char read_nandstg2_minor[3];
				char read_nandstg2_micro[3];
				

				// Search in GUI object
				json_value* val = json->u.object.values[0].value;
				ReadJson result = internal_json_reader(json, val, "latest_major", "latest_minor", "latest_micro", "gui_url", "gui_3dsx_url", "gui_smdh_url", "demo_gui_3dsx_url", "demo_gui_smdh_url");
				strncpy(read_gui_major, result.strvalue1.c_str(), sizeof(read_gui_major));
				strncpy(read_gui_minor, result.strvalue2.c_str(), sizeof(read_gui_minor));
				strncpy(read_gui_micro, result.strvalue3.c_str(), sizeof(read_gui_micro));
				gui_url = result.strvalue4.c_str();
				gui_3dsx_url = result.strvalue5.c_str();
				gui_smdh_url = result.strvalue6.c_str();
				demo_gui_3dsx_url = result.strvalue7.c_str();
				demo_gui_smdh_url = result.strvalue8.c_str();

				// Search in NAND object.
				val = json->u.object.values[1].value;
				json_value* val2 = val->u.object.values[0].value;
				result = internal_json_reader(json, val2, "not_major", "not_minor", "not_micro", "nand_url", "nand_null", "nand_null", "nand_null", "nand_null");
				strncpy(read_nand_major, result.strvalue1.c_str(), sizeof(read_nand_major));
				strncpy(read_nand_minor, result.strvalue2.c_str(), sizeof(read_nand_minor));
				strncpy(read_nand_micro, result.strvalue3.c_str(), sizeof(read_nand_micro));
				nand_url = result.strvalue4.c_str();				

				// Search in NAND (part2) object.
				val2 = val->u.object.values[1].value;
				result = internal_json_reader(json, val2, "p2_not_major", "p2_not_minor", "p2_not_micro", "nand_part2_url", "part2_null", "part2_null", "part2_null", "part2_null");
				strncpy(read_nandpart2_major, result.strvalue1.c_str(), sizeof(read_nandpart2_major));
				strncpy(read_nandpart2_minor, result.strvalue2.c_str(), sizeof(read_nandpart2_minor));
				strncpy(read_nandpart2_micro, result.strvalue3.c_str(), sizeof(read_nandpart2_micro));
				nand_part2_url = result.strvalue4.c_str();				

				// Search in NAND STG2 object.
				val2 = val->u.object.values[2].value;
				result = internal_json_reader(json, val2, "twld_not_major", "twld_not_minor", "twld_not_micro", "nand_twld_url", "twld_null", "twld_null", "twld_null", "twld_null");
				strncpy(read_nandstg2_major, result.strvalue1.c_str(), sizeof(read_nandstg2_major));
				strncpy(read_nandstg2_minor, result.strvalue2.c_str(), sizeof(read_nandstg2_minor));
				strncpy(read_nandstg2_micro, result.strvalue3.c_str(), sizeof(read_nandstg2_micro));
				nand_twld_url = result.strvalue4.c_str();				

				// Store latest and current version of GUI and NAND.
				char latestVersion[16];
				snprintf(latestVersion, sizeof(latestVersion), "%s.%s.%s", read_gui_major, read_gui_minor, read_gui_micro);
				
				char currentVersion[16];
				snprintf(currentVersion, sizeof(currentVersion), "%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);					
				
				char latestNANDVersion[16];
				snprintf(latestNANDVersion, sizeof(latestNANDVersion), "%s.%s.%s", read_nand_major, read_nand_minor, read_nand_micro);
				
				char latestNAND2Version[16];
				snprintf(latestNAND2Version, sizeof(latestNAND2Version), "%s.%s.%s", read_nandpart2_major, read_nandpart2_minor, read_nandpart2_micro);
				
				char latestNANDSTG2Version[16];
				snprintf(latestNANDSTG2Version, sizeof(latestNANDSTG2Version), "%s.%s.%s", read_nandstg2_major, read_nandstg2_minor, read_nandstg2_micro);
				
				if (logEnabled)	LogFMA("checkUpdate", "Reading current version:", currentVersion);
				if (logEnabled)	LogFMA("checkUpdate", "Reading GUI json version:", latestVersion);
				if (logEnabled)	LogFMA("checkUpdate", "Reading NAND json version:", latestNANDVersion);
				if (logEnabled)	LogFMA("checkUpdate", "Reading NAND STG2 json version:", latestNANDSTG2Version);
				
				// Check if current version is the latest (GUI)
				if(strcmp(currentVersion, latestVersion) != 0) {
					// Update available!
					if (logEnabled)	LogFM("checkUpdate", "GUI update available.");			
					updateGUI = true;
					if(strcmp(latestNANDVersion, currentVersion) > 0) {
						if (logEnabled)	LogFM("checkUpdate", "NAND update available.");
						updateNAND = true;
					}
					if(strcmp(latestNAND2Version, currentVersion) > 0) {
						if (logEnabled)	LogFM("checkUpdate", "NAND (part 2) update available.");
						updateNAND_part2 = true;
					}
					if(strcmp(latestNANDSTG2Version, currentVersion) > 0) {
						if (logEnabled)	LogFM("checkUpdate", "NAND SD stage update available.");
						updateNAND_STG2 = true;
					}
				} else {
					sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
					if (screenmode == SCREEN_MODE_SETTINGS) {
						sf2d_draw_texture(settingstex, 0, 0);
					}
					sf2d_draw_texture(dialogboxtex, 0, 0);

					// Version is lower or same.
					if (logEnabled)	LogFMA("checkUpdate", "Comparing...", "Are the same or lower");

					sf2d_end_frame();
					sf2d_swapbuffers();

					if (logEnabled)	LogFM("checkUpdate", "TWLoader is up-to-date!");		
					free(jsonText);
					httpcCloseContext(&context);

					bool checkanswer = true;

					while(checkanswer) {
						textVtxArrayPos = 0; // Clear the text vertex array

						sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
						if (screenmode == SCREEN_MODE_SETTINGS) {
							sf2d_draw_texture(settingstex, 0, 0);
						}
						sf2d_draw_texture(dialogboxtex, 0, 0);
						static const char msg[] =
							"TWLoader is up-to-date.\n"
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
							": OK";					
						renderText(12, 16, 0.5f, 0.5f, false, msg);
						sf2d_end_frame();
						sf2d_swapbuffers();
						
						hidScanInput();

						const u32 hDown = hidKeysDown();
						
						if (hDown & KEY_A) {
							//doCleanInstall = true;
							checkanswer = false;	// Exit loop
							sf2d_end_frame();
							sf2d_swapbuffers();
							DialogBoxDisappear(12, 16, msg);
						}
					}
					
					return -1;
				}
								
				// Search in prebuilds object
				val = json->u.object.values[2].value;
				val2 = val->u.object.values[0].value;
				result = internal_json_reader(json, val2, "ace_rpg_major", "ace_rpg_minor", "ace_rpg_micro", "ace_rpg_url", "ace_rpg_null", "ace_rpg_null", "ace_rpg_null", "ace_rpg_null");
				ace_rpg_url = result.strvalue4.c_str();

				char ace_rpg_version[16];
				snprintf(ace_rpg_version, sizeof(ace_rpg_version), "%s.%s.%s", result.strvalue1.c_str(), result.strvalue2.c_str(), result.strvalue3.c_str());
				if(strcmp(ace_rpg_version, currentVersion) > 0) {
					updateACE_RPG = true;
					if (logEnabled)	LogFM("checkUpdate", "Update available for ACE_RPG");	
				}
				
				val2 = val->u.object.values[1].value;
				result = internal_json_reader(json, val2, "GBARunner2_major", "GBARunner2_minor", "GBARunner2_micro", "GBARunner2_url", "GBARunner2_null", "GBARunner2_null", "GBARunner2_null", "GBARunner2_null");
				gbarunner2_url = result.strvalue4.c_str();

				char GBARunner2_version[16];
				snprintf(GBARunner2_version, sizeof(GBARunner2_version), "%s.%s.%s", result.strvalue1.c_str(), result.strvalue2.c_str(), result.strvalue3.c_str());
				if(strcmp(GBARunner2_version, currentVersion) > 0) {
					updateGBARUNNER_2 = true;
					if (logEnabled)	LogFM("checkUpdate", "Update available for GBARunner2");
				}
				
				val2 = val->u.object.values[2].value;
				result = internal_json_reader(json, val2, "loadcard_dstt_major", "loadcard_dstt_minor", "loadcard_dstt_micro", "loadcard_dstt_url", "loadcard_dstt_null", "loadcard_dstt_null", "loadcard_dstt_null", "loadcard_dstt_null");
				loadcard_dstt_url = result.strvalue4.c_str();
				char loadcard_dstt_version[16];
				snprintf(loadcard_dstt_version, sizeof(loadcard_dstt_version), "%s.%s.%s", result.strvalue1.c_str(), result.strvalue2.c_str(), result.strvalue3.c_str());
				if(strcmp(loadcard_dstt_version, currentVersion) > 0) {
					updateLOADCARD_DSTT = true;
					if (logEnabled)	LogFM("checkUpdate", "Update available for Loadcard_DSTT");
				}
				
				val2 = val->u.object.values[3].value;
				result = internal_json_reader(json, val2, "r4_major", "r4_minor", "r4_micro", "r4_url", "r4_null", "r4_null", "r4_null", "r4_null");
				r4_url = result.strvalue4.c_str();

				char r4_version[16];
				snprintf(r4_version, sizeof(r4_version), "%s.%s.%s", result.strvalue1.c_str(), result.strvalue2.c_str(), result.strvalue3.c_str());
				if(strcmp(r4_version, currentVersion) > 0) {
					updateR4 = true;
					if (logEnabled)	LogFM("checkUpdate", "Update available for R4");
				}
			}
		}
	}
	
	if (updateGUI) {		
		free(jsonText);
		httpcCloseContext(&context);
		return 0;
	}
	
	free(jsonText);
	httpcCloseContext(&context);
	return -1;
}

/**
 * Download the TWLoader CIAs.
 */
void DownloadTWLoaderCIAs(void) {	

	bool checkanswer = true;
	bool yestoupdate = false;
	
	while(checkanswer) {
		textVtxArrayPos = 0; // Clear the text vertex array

		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		if (screenmode == SCREEN_MODE_SETTINGS) {
			sf2d_draw_texture(settingstex, 0, 0);
		}
		sf2d_draw_texture(dialogboxtex, 0, 0);
		setTextColor(RGBA8(0, 0, 0, 255));
		static const char gui_msg[] =
			"An update for TWLoader is available.\n"
			"Do you want to update?\n"
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
			": Yes\n"
			": No";
		renderText(12, 16, 0.5f, 0.5f, false, gui_msg);
		sf2d_end_frame();
		sf2d_swapbuffers();

		hidScanInput();

		const u32 hDown = hidKeysDown();
		
		if (hDown & KEY_A) {
			yestoupdate = true;
			checkanswer = false;	// Exit loop
		} else if (hDown & KEY_B) {
			checkanswer = false;	// Exit loop
		}
	}
	
	if(yestoupdate) {
		int resGUI = -1;
		int resGUI_3DSX = -1;
		int resGUI_SMDH = -1;
		int resNAND = -1;
		int resNAND_part2 = -1;
		int resNAND_STG2 = -1;
		struct stat st;
		if (updateGUI) {
			if (settings.ui.filetype == 0 || settings.ui.filetype == 2) {
				sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				if (screenmode == SCREEN_MODE_SETTINGS) {
					sf2d_draw_texture(settingstex, 0, 0);
				}
				sf2d_draw_texture(dialogboxtex, 0, 0);
				static const char gui_msg[] =
					"Now downloading latest TWLoader version...\n"
					"(GUI, CIA)\n"
					"\n"
					"Do not turn off the power.\n";
				renderText(12, 16, 0.5f, 0.5f, false, gui_msg);
				sf2d_end_frame();
				sf2d_swapbuffers();
			
				if(stat("sdmc:/cia",&st) == 0){		
					// Use root/cia folder instead
					resGUI = downloadFile(gui_url.c_str(),"/cia/TWLoader.cia", MEDIA_SD_CIA);
				}else{
					mkdir("sdmc:/_nds/twloader/cia", 0777); // Use twloader/cia folder instead
					resGUI = downloadFile(gui_url.c_str(),"/_nds/twloader/cia/TWLoader.cia", MEDIA_SD_CIA);
				}
				
				sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				if (screenmode == SCREEN_MODE_SETTINGS) {
					sf2d_draw_texture(settingstex, 0, 0);
				}
				sf2d_draw_texture(dialogboxtex, 0, 0);
			}
			if (settings.ui.filetype == 1 || settings.ui.filetype == 2) {
				sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				if (screenmode == SCREEN_MODE_SETTINGS) {
					sf2d_draw_texture(settingstex, 0, 0);
				}
				sf2d_draw_texture(dialogboxtex, 0, 0);
				static const char gui_msg[] =
					"Now downloading latest TWLoader version...\n"
					"(GUI, 3DSX)\n"
					"\n"
					"Do not turn off the power.\n";
				renderText(12, 16, 0.5f, 0.5f, false, gui_msg);
				sf2d_end_frame();
				sf2d_swapbuffers();
				
				if (isDemo) {
					mkdir("sdmc:/3ds/TWLoader_demo", 0777);
					resGUI_3DSX = downloadFile(demo_gui_3dsx_url.c_str(),"/3ds/TWLoader_demo/TWLoader_demo.3dsx", MEDIA_SD_FILE);
					resGUI_SMDH = downloadFile(demo_gui_smdh_url.c_str(),"/3ds/TWLoader_demo/TWLoader_demo.smdh", MEDIA_SD_FILE);
				} else {
					mkdir("sdmc:/3ds/TWLoader", 0777);
					resGUI_3DSX = downloadFile(gui_3dsx_url.c_str(),"/3ds/TWLoader/TWLoader.3dsx", MEDIA_SD_FILE);
					resGUI_SMDH = downloadFile(gui_smdh_url.c_str(),"/3ds/TWLoader/TWLoader.smdh", MEDIA_SD_FILE);
				}
				
				sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
				if (screenmode == SCREEN_MODE_SETTINGS) {
					sf2d_draw_texture(settingstex, 0, 0);
				}
				sf2d_draw_texture(dialogboxtex, 0, 0);
			}
		}
		if (resGUI == 0 && updateNAND) {
			static const char twlnand_msg[] =
				"Now downloading latest TWLoader version...\n"
				"(TWLNAND side CIA (part 1))\n"
				"\n"
				"Do not turn off the power.\n";
			renderText(12, 16, 0.5f, 0.5f, false, twlnand_msg);
			sf2d_end_frame();
			sf2d_swapbuffers();
			// Delete first if installed.
			if(checkTWLNANDSide()){
				amInit();
				AM_DeleteTitle(MEDIATYPE_NAND, TWLNAND_TID);
				amExit();
			}

			if(stat("sdmc:/cia",&st) == 0){		
				// Use root/cia folder instead
				resNAND = downloadFile(nand_url.c_str(),"/cia/TWLoader - TWLNAND side.cia", MEDIA_NAND_CIA);
			}else{		
				resNAND = downloadFile(nand_url.c_str(),"/_nds/twloader/cia/TWLoader - TWLNAND side.cia", MEDIA_NAND_CIA);
			}
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			if (screenmode == SCREEN_MODE_SETTINGS) {
				sf2d_draw_texture(settingstex, 0, 0);
			}
			sf2d_draw_texture(dialogboxtex, 0, 0);			
		}
		if (resNAND == 0 && updateNAND_STG2) {
			static const char twlnandstg2_msg[] =
				"Now downloading latest TWLoader version...\n"
				"(SD stage of (part 1 of) TWLNAND side)\n"
				"\n"
				"Do not turn off the power.\n";
			renderText(12, 16, 0.5f, 0.5f, false, twlnandstg2_msg);
			sf2d_end_frame();
			sf2d_swapbuffers();

			resNAND_STG2 = downloadFile(nand_twld_url.c_str(),"/_nds/twloader/TWLD.twldr", MEDIA_SD_FILE);
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			if (screenmode == SCREEN_MODE_SETTINGS) {
				sf2d_draw_texture(settingstex, 0, 0);
			}
			sf2d_draw_texture(dialogboxtex, 0, 0);			
		}
		if (resNAND_STG2 == 0 && updateNAND_part2) {
			static const char twlnand2_msg[] =
				"Now downloading latest TWLoader version...\n"
				"(TWLNAND side CIA (part 2))\n"
				"\n"
				"Do not turn off the power.\n";
			renderText(12, 16, 0.5f, 0.5f, false, twlnand2_msg);
			sf2d_end_frame();
			sf2d_swapbuffers();
			// Delete first if installed.
			if(checkTWLNANDSide2()){
				amInit();
				AM_DeleteTitle(MEDIATYPE_NAND, NTRLAUNCHER_TID);
				amExit();
			}

			if(stat("sdmc:/cia",&st) == 0){		
				// Use root/cia folder instead
				resNAND = downloadFile(nand_part2_url.c_str(),"/cia/TWLoader - TWLNAND side (part 2).cia", MEDIA_NAND_CIA);
			}else{		
				resNAND = downloadFile(nand_part2_url.c_str(),"/_nds/twloader/cia/TWLoader - TWLNAND side (part 2).cia", MEDIA_NAND_CIA);
			}
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			if (screenmode == SCREEN_MODE_SETTINGS) {
				sf2d_draw_texture(settingstex, 0, 0);
			}
			sf2d_draw_texture(dialogboxtex, 0, 0);			
		}
		// If gui or nand failed, stop before downloading prebuilds.
		if (settings.ui.filetype == 0 || settings.ui.filetype == 2) {
			if(resGUI != 0 || (updateNAND && resNAND != 0) || (updateNAND_STG2 && resNAND_STG2 != 0) || (updateNAND_part2 && resNAND_part2 != 0) ) {
				DialogBoxDisappear(12, 16, "Update failed.");
			}
		} else if (settings.ui.filetype == 1) {
			if(resGUI_3DSX != 0 || (updateNAND && resNAND != 0) || (updateNAND_STG2 && resNAND_STG2 != 0) || (updateNAND_part2 && resNAND_part2 != 0) ) {
				DialogBoxDisappear(12, 16, "Update failed.");
			}
		}
		if(resGUI == 0 && updateACE_RPG) {
			static const char msg[] =
				"Now downloading latest Ace_RPG version...\n"
				"(ace_rpg.nds)\n"
				"\n"
				"Do not turn off the power.\n";
			renderText(12, 16, 0.5f, 0.5f, false, msg);
			sf2d_end_frame();
			sf2d_swapbuffers();
			int res = downloadFile(ace_rpg_url.c_str(),"/_nds/twloader/loadflashcard/ace_rpg.nds", MEDIA_SD_FILE);
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			if (screenmode == SCREEN_MODE_SETTINGS) {
				sf2d_draw_texture(settingstex, 0, 0);
			}
			sf2d_draw_texture(dialogboxtex, 0, 0);
			if (res != 0) {
				DialogBoxDisappear(12, 16, "Download failed.");
			}
		}
		if (resGUI == 0 && updateGBARUNNER_2) {
			static const char msg[] =
				"Now downloading latest GBARunner2 version...\n"
				"(GBARunner2.nds)\n"
				"\n"
				"Do not turn off the power.\n";
			renderText(12, 16, 0.5f, 0.5f, false, msg);
			sf2d_end_frame();
			sf2d_swapbuffers();
			int res = downloadFile(gbarunner2_url.c_str(),"/_nds/GBARunner2.nds", MEDIA_SD_FILE);
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			if (screenmode == SCREEN_MODE_SETTINGS) {
				sf2d_draw_texture(settingstex, 0, 0);
			}
			sf2d_draw_texture(dialogboxtex, 0, 0);
			if (res != 0) {
				DialogBoxDisappear(12, 16, "Download failed.");
			}
		}
		if (resGUI == 0 && updateLOADCARD_DSTT) {
			static const char msg[] =
				"Now downloading latest loadcard_dstt version...\n"
				"(loadcard_dstt.nds)\n"
				"\n"
				"Do not turn off the power.\n";
			renderText(12, 16, 0.5f, 0.5f, false, msg);
			sf2d_end_frame();
			sf2d_swapbuffers();
			int res = downloadFile(loadcard_dstt_url.c_str(),"/_nds/loadcard_dstt.nds", MEDIA_SD_FILE);
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			if (screenmode == SCREEN_MODE_SETTINGS) {
				sf2d_draw_texture(settingstex, 0, 0);
			}
			sf2d_draw_texture(dialogboxtex, 0, 0);
			if (res != 0) {
				DialogBoxDisappear(12, 16, "Download failed.");
			}
		}
		if (resGUI == 0 && updateR4) {
			static const char msg[] =
				"Now downloading latest R4 version...\n"
				"(r4.nds)\n"
				"\n"
				"Do not turn off the power.\n";
			renderText(12, 16, 0.5f, 0.5f, false, msg);
			sf2d_end_frame();
			sf2d_swapbuffers();
			int res = downloadFile(r4_url.c_str(),"/_nds/twloader/loadflashcard/r4.nds", MEDIA_SD_FILE);
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			if (screenmode == SCREEN_MODE_SETTINGS) {
				sf2d_draw_texture(settingstex, 0, 0);
			}
			sf2d_draw_texture(dialogboxtex, 0, 0);
			if (res != 0) {
				DialogBoxDisappear(12, 16, "Download failed.");
			}
		}
		if (resGUI == 0) {			
			renderText(12, 16, 0.5f, 0.5f, false, "Now returning to HOME Menu...");
			sf2d_end_frame();
			sf2d_swapbuffers();
			run = false;
		}
	} else {
		DialogBoxDisappear(12, 16, "Update cancelled.");
	}
}

/**
 * Update nds-bootstrap to the latest unofficial build.
 */
void UpdateBootstrapUnofficial(void) {
	static const char title[] = "Now updating bootstrap (Unofficial)...";
	DialogBoxAppear(12, 16, title);
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	if (screenmode == SCREEN_MODE_SETTINGS) {
		sf2d_draw_texture(settingstex, 0, 0);
	}
	sf2d_draw_texture(dialogboxtex, 0, 0);
	setTextColor(RGBA8(0, 0, 0, 255));
	renderText(12, 16, 0.5f, 0.5f, false, title);
	sf2d_end_frame();
	sf2d_swapbuffers();
	remove("sdmc:/_nds/twloader/unofficial-bootstrap");
	remove("sdmc:/_nds/twloader/unofficial-bootstrap.nds");
	downloadBootstrapVersion(false);
	checkBootstrapVersion();
	downloadFile(unofficial_BS_url.c_str(),"/_nds/unofficial-bootstrap.nds", MEDIA_SD_FILE);
	if (screenmode == SCREEN_MODE_SETTINGS) {
		DialogBoxDisappear(12, 16, "Done!");
	}
}

/**
 * Update nds-bootstrap to the latest release build.
 */
void UpdateBootstrapRelease(void) {
	static const char title[] = "Now updating bootstrap (Release)...";
	DialogBoxAppear(12, 16, title);
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	if (screenmode == SCREEN_MODE_SETTINGS) {
		sf2d_draw_texture(settingstex, 0, 0);
	}
	sf2d_draw_texture(dialogboxtex, 0, 0);
	setTextColor(RGBA8(0, 0, 0, 255));
	renderText(12, 16, 0.5f, 0.5f, false, title);
	sf2d_end_frame();
	sf2d_swapbuffers();
	remove("sdmc:/_nds/twloader/release-bootstrap");
	remove("sdmc:/_nds/twloader/release-bootstrap.nds");
	downloadBootstrapVersion(true);
	checkBootstrapVersion();
	downloadFile(release_BS_url.c_str(),"/_nds/release-bootstrap.nds", MEDIA_SD_FILE);
	if (screenmode == SCREEN_MODE_SETTINGS) {
		DialogBoxDisappear(12, 16, "Done!");
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
		case 'T':
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
				ba_region = "EN";
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
 * @param location.
 * @return 0 on success; non-zero on error.
 */
static int downloadBoxArt_internal(const char *ba_TID, RomLocation location)
{
	char path[256];
	char http_url[256];
	if (logEnabled)	LogFMA("downloadBoxArt_internal", "Downloading boxart for TID", ba_TID);

	const char *ba_region_fallback = NULL;
	const char *ba_region = getGameTDBRegion(ba_TID[3], &ba_region_fallback);
	if (!ba_region)
		return -1;

	// NOTE: downloadFile() doesn't use devoptab,
	// so don't prefix the filename with sdmc:/.
	
	switch(location){
		case ROM_SD:
			snprintf(path, sizeof(path), "/_nds/twloader/boxart/%.4s.png", ba_TID);
			break;
		case ROM_FLASHCARD:
			snprintf(path, sizeof(path), "/_nds/twloader/boxart/flashcard/%.4s.png", ba_TID);
			break;
		case ROM_SLOT_1:
		default:
			snprintf(path, sizeof(path), "/_nds/twloader/boxart/slot1/%.4s.png", ba_TID);
			break;
	}
	snprintf(http_url, sizeof(http_url), "http://art.gametdb.com/ds/coverS/%s/%.4s.png",
		 ba_region, ba_TID);
	int res = downloadFile(http_url, path, MEDIA_SD_FILE);
	if ((res != 0) && (ba_region_fallback != NULL)) {
		// Try the fallback region.
		if (logEnabled)	LogFM("downloadBoxArt_internal", "Now using fallback region.");
		snprintf(http_url, sizeof(http_url), "http://art.gametdb.com/ds/coverS/%s/%.4s.png",
			 ba_region_fallback, ba_TID);
		res = downloadFile(http_url, path, MEDIA_SD_FILE);
	}
	return res;
}

/**
 * Download bootstrap version files
 * @return non zero if error
 */

int downloadBootstrapVersion(bool type)
{
	u32 responseCode = 0;
	httpcContext context;	
	int res = -1;	
	
	httpcInit(0);
	if(R_FAILED(httpcOpenContext(&context, HTTPC_METHOD_GET, JSON_URL, 0))) {
		if (logEnabled)	LogFM("checkUpdate", "Error opening context.");
		return -1;
	}
	if(R_FAILED(httpcAddRequestHeaderField(&context, "User-Agent", "TWLoader"))) {
		if (logEnabled)	LogFM("checkUpdate", "Error requesting header field.");
		return -1;
	}
	if(R_FAILED(httpcSetSSLOpt(&context, SSLCOPT_DisableVerify))) {
		if (logEnabled)	LogFM("checkUpdate", "Error setting SSL certificate.");
		return -1;
	}
	if(R_FAILED(httpcSetKeepAlive(&context, HTTPC_KEEPALIVE_ENABLED))) {
		if (logEnabled)	LogFM("checkUpdate", "Error while keeping alive the conection.");
		return -1;
	}
	if(R_FAILED(httpcBeginRequest(&context))) {
		if (logEnabled)	LogFM("checkUpdate", "Error begining the request.");
		return -1;
	}
	if(R_FAILED(httpcGetResponseStatusCode(&context, &responseCode))) {
		if (logEnabled)	LogFM("checkUpdate", "Error getting response code.");
		return -1;
	}
    if (responseCode != 200) {
		if (logEnabled)	LogFM("checkUpdate", "Error reaching the update.json file.");
		return -1;
	}
	
	u32 size = 0;
	httpcGetDownloadSizeState(&context, NULL, &size);	
	char* jsonText = (char*) calloc(sizeof(char), size);
	if (logEnabled) LogFM("Bootstrap", "Downloading JSON info.");
	if(jsonText != NULL) {
		u32 bytesRead = 0;
		http_read_internal(&context, &bytesRead, (u8*) jsonText, size);
		json_value* json = json_parse(jsonText, size);

		if(json != NULL) {
			if(json->type == json_object) {				
				
				json_value* val = json->u.object.values[4].value;

				for(u32 i = 0; i < (json->u.object.length - 1); i++) {
					char* name = val->u.object.values[i].name;
					u32 nameLen = val->u.object.values[i].name_length;
					json_value* subVal = val->u.object.values[i].value;
					if(subVal->type == json_string) {
						if(strncmp(name, "release_ver", nameLen) == 0) {
							release_BS_ver = subVal->u.string.ptr;
						} else if(strncmp(name, "unofficial_ver", nameLen) == 0) {
							unofficial_BS_ver = subVal->u.string.ptr;
						} else if(strncmp(name, "release_url", nameLen) == 0) {
							release_BS_url = subVal->u.string.ptr;
						} else if(strncmp(name, "unofficial_url", nameLen) == 0) {
							unofficial_BS_url = subVal->u.string.ptr;
						}
					}
				}
				
			}
		}
	}
	
	free(jsonText);
	httpcCloseContext(&context);		
		
	if (type){
		FILE* ver = fopen("sdmc:/_nds/twloader/release-bootstrap", "w");
		if(!ver) {
			return res;
		}
		fputs(release_BS_ver.c_str(), ver);
		fclose(ver);
	}else{
		FILE* ver = fopen("sdmc:/_nds/twloader/unofficial-bootstrap", "w");
		fputs(unofficial_BS_ver.c_str(), ver);
		fclose(ver);
	}
	
	return res;
}

/**
 * check, download and store bootstrap version
 */

void checkBootstrapVersion(void){
	
	bool res = false;
	long fileSize;
	char buf[128];
	if (logEnabled) LogFM("download.checkBootstrapVersion()", "Checking bootstrap version");
	// Clean buf array
	for (size_t i=0; i< sizeof(buf); i++){
		buf[i] = '\0';
	}
	
	FILE* VerFile = fopen("sdmc:/_nds/twloader/release-bootstrap", "r");
	if (!VerFile){
		if (logEnabled) LogFM("download.checkBootstrapVersion()", "release-bootstrap ver file not found.");
		if(checkWifiStatus()){
			if (logEnabled) LogFM("download.checkBootstrapVersion()", "downloading release-bootstrap ver file.");
			res = downloadBootstrapVersion(true); // true == release
		}else{
			if (logEnabled) LogFM("download.checkBootstrapVersion()", "No release version file available.");
			settings_releasebootstrapver = "No version available";
		}
	}else{
		fseek(VerFile , 0 , SEEK_END);
		fileSize = ftell(VerFile);
		rewind(VerFile);
		fread(buf,1,fileSize,VerFile);
		settings_releasebootstrapver = buf;
		fclose(VerFile);
		if (logEnabled) LogFMA("download.checkBootstrapVersion()", "Reading release bootstrap ver file:", settings_releasebootstrapver.c_str());
	}
	
	if (res) settings_releasebootstrapver = "No version available";
	
	// Clean buf array
	for (size_t i=0; i< sizeof(buf); i++){
		buf[i] = '\0';
	}
	
	if(res == 0){
		if (logEnabled) LogFM("download.checkBootstrapVersion()", "Re-opening release bootstrap ver file.");
		// Try to open again
		VerFile = fopen("sdmc:/_nds/twloader/release-bootstrap", "r");
		if (!VerFile){
				if (logEnabled) LogFM("download.checkBootstrapVersion()", "No release version file available #2.");
				settings_releasebootstrapver = "No version available";
		}else{
			fseek(VerFile , 0 , SEEK_END);
			fileSize = ftell(VerFile);
			rewind(VerFile);
			fread(buf,1,fileSize,VerFile);
			settings_releasebootstrapver = buf;
			fclose(VerFile);
			if (logEnabled) LogFMA("download.checkBootstrapVersion()", "Reading release bootstrap ver file #2:", settings_releasebootstrapver.c_str());
		}
	}
	
	res = false; // Just to be sure	
	
	VerFile = fopen("sdmc:/_nds/twloader/unofficial-bootstrap", "r");
	if (!VerFile){
		if (logEnabled) LogFM("download.checkBootstrapVersion()", "unofficial-bootstrap ver file not found.");
		if(checkWifiStatus()){
			if (logEnabled) LogFM("download.checkBootstrapVersion()", "downloading unofficial-bootstrap ver file.");
			res = downloadBootstrapVersion(false); // false == unofficial
		}else{
			if (logEnabled) LogFM("download.checkBootstrapVersion()", "No unofficial version file available.");
			settings_unofficialbootstrapver = "No version available";
		}
	}else{
		fseek(VerFile , 0 , SEEK_END);
		fileSize = ftell(VerFile);
		rewind(VerFile);
		fread(buf,1,fileSize,VerFile);
		settings_unofficialbootstrapver = buf;
		fclose(VerFile);
		if (logEnabled) LogFMA("download.checkBootstrapVersion()", "Reading unofficial bootstrap ver file:", settings_unofficialbootstrapver.c_str());
	}
	if (res) settings_unofficialbootstrapver = "No version available";

	if(res == 0){
		if (logEnabled) LogFM("download.checkBootstrapVersion()", "Re-opening unofficial bootstrap ver file.");
		// Try to open again
		VerFile = fopen("sdmc:/_nds/twloader/unofficial-bootstrap", "r");
		if (!VerFile){
				if (logEnabled) LogFM("download.checkBootstrapVersion()", "No unofficial version file available #2.");		
				settings_unofficialbootstrapver = "No version available";
		}else{
			fseek(VerFile , 0 , SEEK_END);
			fileSize = ftell(VerFile);
			rewind(VerFile);
			fread(buf,1,fileSize,VerFile);
			settings_unofficialbootstrapver = buf;
			fclose(VerFile);
			if (logEnabled) LogFMA("download.checkBootstrapVersion()", "Reading unofficial bootstrap ver file #2:", settings_unofficialbootstrapver.c_str());
		}
	}
}
 
/**
 * Download Slot-1 boxart.
 */
void downloadSlot1BoxArt(const char* TID)
{
	if (logEnabled)	LogFM("Main.downloadSlot1BoxArt", "Checking box art (Slot-1 card).");
	
	char ba_TID[5];
	snprintf(ba_TID, sizeof(ba_TID), "%.4s", TID);
	ba_TID[4] = 0;

	char path[256];
	snprintf(path, sizeof(path), "/_nds/twloader/boxart/slot1/%.4s.png", ba_TID);
	if (!access(path, F_OK)) {
		// File already exists.
		return;
	}

	if (logEnabled)	LogFM("Main.downloadSlot1BoxArt", "Downloading box art (Slot-1 card)");
	downloadBoxArt_internal(ba_TID, ROM_SLOT_1);
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
	if (logEnabled)	LogFM("Download.downloadBoxArt", "Checking missing boxart on SD card...");

	// Check if we're missing any boxart for ROMs on the SD card.
	for (size_t boxartnum = 0; boxartnum < files.size(); boxartnum++) {
		// Get the title ID from the ROM image.
		const char *tempfile = files.at(boxartnum).c_str();
		snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.romfolder.c_str(), tempfile);
		FILE *f_nds_file = fopen(path, "rb");
		if (!f_nds_file)
			continue;
		
		if (logEnabled)	LogFMA("DownloadBoxArt.SD CARD", "Path: ", path);
		char ba_TID[5];
		grabTID(f_nds_file, ba_TID);
		ba_TID[4] = 0;
		fclose(f_nds_file);

		// Did we already check for this boxart?
		// NOTE: Storing byteswapped in order to sort correctly.
		u32 tid;
		memcpy(&tid, ba_TID, sizeof(tid));
		tid = __builtin_bswap32(tid);
		if (tid == 0x4E54524A || tid == 0x23232323) {
			// NTRJ: Generic game code used for
			// prototypes and some DL Play games.
			// ####: Code used for homebrew games/apps.
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
		}else{
			// Maybe boxart exist with fullname instead of TID
			snprintf(path, sizeof(path), "sdmc:/_nds/twloader/boxart/%s.png", tempfile);
			if(!access(path, F_OK)){
				// Boxart with fullname exist
				continue;
			}
		}

		// Boxart file does not exist. Download it.
		boxart_dl_tids.push_back(tid);
	}
	
	if (logEnabled)	LogFM("DownloadBoxArt.Read_BoxArt", "SD boxart read correctly.");
	if (boxart_dl_tids.empty()) {
		// No boxart to download.
		if (logEnabled)	LogFM("Download.downloadBoxArt", "No boxart to download. (SD side)");
	}else {		
		// Sort the TIDs for convenience purposes.
		std::sort(boxart_dl_tids.begin(), boxart_dl_tids.end());

		// Download the boxart.
		char s_boxart_total[12];
		snprintf(s_boxart_total, sizeof(s_boxart_total), "%zu", boxart_dl_tids.size());
		if (logEnabled)	LogFM("DownloadBoxArt.downloading_process", "Downloading missing boxart (SD side)");
		for (size_t boxartnum = 0; boxartnum < boxart_dl_tids.size(); boxartnum++) {
			static const char title[] = "Downloading missing boxart (SD side)...";

			// Convert the TID back to char.
			char ba_TID[5];
			u32 tid = __builtin_bswap32(boxart_dl_tids[boxartnum]);
			memcpy(ba_TID, &tid, 4);
			ba_TID[4] = 0;
			
			char str[256] = "";
			snprintf(str, sizeof(str), "%zu", boxartnum);
			
			// Show the dialog.
			DialogBoxAppear(12, 16, title);
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_draw_texture(dialogboxtex, 0, 0);
			setTextColor(RGBA8(0, 0, 0, 255));
			renderText(12, 16, 0.5f, 0.5f, false, title);
			renderText(12, 32, 0.5f, 0.5f, false, str);
			renderText(39, 32, 0.5f, 0.5f, false, "/");
			renderText(44, 32, 0.5f, 0.5f, false, s_boxart_total);
			renderText(12, 64, 0.5f, 0.5f, false, "Downloading:");
			renderText(108, 64, 0.5f, 0.5f, false, ba_TID);
			sf2d_end_frame();
			sf2d_swapbuffers();
			
			downloadBoxArt_internal(ba_TID, ROM_SD);
		}
		DialogBoxDisappear(12, 16, NULL);
		boxart_dl_tids.clear();
	}
	// Check if we're missing any boxart for ROMs on the flashcard.
	for (size_t boxartnum = 0; boxartnum < fcfiles.size(); boxartnum++) {
		// Get the title ID from the INI file.
		const char *tempfile = fcfiles.at(boxartnum).c_str();
		snprintf(path, sizeof(path), "sdmc:/%s/%s", settings.ui.fcromfolder.c_str(), tempfile);
		CIniFile setfcrompathini(path);							
		
		std::string ba_TID = setfcrompathini.GetString("FLASHCARD-ROM", "TID", "");		
		if (ba_TID.size() < 4) {
			// Invalid TID.
			continue;
		}
		
		if (logEnabled)	LogFMA("DownloadBoxArt.FLASHCARD", "TID: ", ba_TID.c_str());		
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
		snprintf(path, sizeof(path), "sdmc:/_nds/twloader/boxart/flashcard/%.4s.png", ba_TID.c_str());
		if (!access(path, F_OK)) {
			// Boxart file exists.
			continue;
		}else{
			// Maybe boxart exist with fullname instead of TID
			snprintf(path, sizeof(path), "sdmc:/_nds/twloader/boxart/flashcard/%s.png", tempfile);
			if(!access(path, F_OK)){
				// Boxart with fullname exist
				continue;
			}
		}

		// Boxart file does not exist. Download it.
		boxart_dl_tids.push_back(tid);
	}

	if (logEnabled)	LogFM("DownloadBoxArt.Read_BoxArt", "Flashcard boxart read correctly.");
	if (boxart_dl_tids.empty()) {
		// No boxart to download.
		if (logEnabled)	LogFM("Download.downloadBoxArt", "No boxart to download. (Flashcard side)");
	}else {		
		// Sort the TIDs for convenience purposes.
		std::sort(boxart_dl_tids.begin(), boxart_dl_tids.end());

		// Download the boxart.
		char s_boxart_total[12];
		snprintf(s_boxart_total, sizeof(s_boxart_total), "%zu", boxart_dl_tids.size());
		if (logEnabled)	LogFM("DownloadBoxArt.downloading_process", "Downloading missing boxart (Flashcard side)");
		for (size_t boxartnum = 0; boxartnum < boxart_dl_tids.size(); boxartnum++) {
			static const char title[] = "Downloading missing boxart (Flashcard side)...";

			// Convert the TID back to char.
			char ba_TID[5];
			u32 tid = __builtin_bswap32(boxart_dl_tids[boxartnum]);
			memcpy(ba_TID, &tid, 4);
			ba_TID[4] = 0;
			
			char str[256] = "";
			snprintf(str, sizeof(str), "%zu", boxartnum);
			
			// Show the dialog.
			DialogBoxAppear(12, 16, title);
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sf2d_draw_texture(dialogboxtex, 0, 0);
			setTextColor(RGBA8(0, 0, 0, 255));
			renderText(12, 16, 0.5f, 0.5f, false, title);
			renderText(12, 32, 0.5f, 0.5f, false, str);
			renderText(39, 32, 0.5f, 0.5f, false, "/");
			renderText(44, 32, 0.5f, 0.5f, false, s_boxart_total);
			renderText(12, 64, 0.5f, 0.5f, false, "Downloading:");
			renderText(108, 64, 0.5f, 0.5f, false, ba_TID);
			sf2d_end_frame();
			sf2d_swapbuffers();

			downloadBoxArt_internal(ba_TID, ROM_FLASHCARD);
		}
		DialogBoxDisappear(12, 16, NULL);
		boxart_dl_tids.clear();
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
			snprintf(path, sizeof(path), "sdmc:/_nds/twloader/boxart/slot1/%.4s.png", card_gameID);
			if (access(path, F_OK) != 0) {
				// Boxart file does not exist. Download it.
				boxart_dl_tids.push_back(tid);
			}
		}
	}
	
	if (logEnabled)	LogFM("DownloadBoxArt.Read_BoxArt", "Slot-1 boxart read correctly.");
	if (boxart_dl_tids.empty()) {
		// No boxart to download.
		if (logEnabled)	LogFM("Download.downloadBoxArt", "No boxart to download. (Slot-1 Side)");
		return;
	}

	// Sort the TIDs for convenience purposes.
	std::sort(boxart_dl_tids.begin(), boxart_dl_tids.end());

	// Download the boxart.
	char s_boxart_total[12];
	snprintf(s_boxart_total, sizeof(s_boxart_total), "%zu", boxart_dl_tids.size());
	if (logEnabled)	LogFM("DownloadBoxArt.downloading_process", "Downloading missing boxart (Slot-1 Side)");
	for (size_t boxartnum = 0; boxartnum < boxart_dl_tids.size(); boxartnum++) {
		static const char title[] = "Downloading missing boxart (Slot-1 side)...";

		// Convert the TID back to char.
		char ba_TID[5];
		u32 tid = __builtin_bswap32(boxart_dl_tids[boxartnum]);
		memcpy(ba_TID, &tid, 4);
		ba_TID[4] = 0;
		
		char str[256] = "";
		snprintf(str, sizeof(str), "%zu", boxartnum);
		
		// Show the dialog.
		DialogBoxAppear(12, 16, title);
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(dialogboxtex, 0, 0);
		setTextColor(RGBA8(0, 0, 0, 255));
		renderText(12, 16, 0.5f, 0.5f, false, title);
		renderText(12, 32, 0.5f, 0.5f, false, str);
		renderText(39, 32, 0.5f, 0.5f, false, "/");
		renderText(44, 32, 0.5f, 0.5f, false, s_boxart_total);
		renderText(12, 64, 0.5f, 0.5f, false, "Downloading:");
		renderText(108, 64, 0.5f, 0.5f, false, ba_TID);
		sf2d_end_frame();
		sf2d_swapbuffers();

		downloadBoxArt_internal(ba_TID, ROM_SLOT_1);
	}
}
