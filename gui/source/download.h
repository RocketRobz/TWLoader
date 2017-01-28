#ifndef DOWNLOAD_H
#define DOWNLOAD_H

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
 * Update nds-bootstrap to the latest unofficial build.
 */
void UpdateBootstrapUnofficial(void);

/**
 * Update nds-bootstrap to the latest release build.
 */
void UpdateBootstrapRelease(void);

/**
 * Download bootstrap version files
 */

int downloadBootstrapVersion(bool type);

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
