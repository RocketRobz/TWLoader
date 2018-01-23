#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include "graphic.h"
#include "pp2d/pp2d.h"

/**
 * Check Wi-Fi status.
 * @return True if Wi-Fi is connected; false if not.
 */
bool checkWifiStatus(void);

enum MediaType {
	MEDIA_SD_FILE = 0,	// Plain old file on the SD card.
	MEDIA_SD_CIA = 1,	// CIA installed to the SD card.
	MEDIA_NAND_CIA = 2,	// CIA installed to NAND.
};

enum RomLocation {
	ROM_SD = 0,			// Location for SD files
	ROM_FLASHCARD = 1,	// Location for flashcard files
	ROM_SLOT_1 = 2,		// Location for SLOT-1 files
};

/**
 * Download a file.
 * @param url URL of the file.
 * @param file Local filename.
 * @param mediaType How the file should be handled.
 * @return 0 on success; non-zero on error.
 */
int downloadFile(const char* url, const char* file, MediaType mediaType);

/**
 * Check for a TWLoader update.
 * @return 0 if an update is available; non-zero if up to date or an error occurred.
 */
int checkUpdate(void);

/**
 * Download the TWLoader CIAs.
 */
void DownloadTWLoaderCIAs(void);

/**
 * Check for missing files, and download them.
 * @return non-zero if an error occurred.
 */
int DownloadMissingFiles(void);

/**
 * Update nds-bootstrap to the latest unofficial build.
 */
void UpdateSDK1BootstrapUnofficial(void);
void UpdateSDK5BootstrapUnofficial(void);
void UpdateBootstrapUnofficial(void);

/**
 * Update nds-bootstrap to the latest release build.
 */
void UpdateSDK1BootstrapRelease(void);
void UpdateSDK5BootstrapRelease(void);
void UpdateBootstrapRelease(void);

/**
 * Download bootstrap version files
 */

int downloadBootstrapVersion(bool type, bool sdk5);

/**
 * Download store bootstrap version
 * @return non zero if error
 */

void checkBootstrapVersion(void);
 
/**
 * Download Slot-1 boxart.
 */
void downloadSlot1BoxArt(const char* TID);

/**
 * Download boxart.
 */
void downloadBoxArt(void);

#endif /* DOWNLOAD_H */
