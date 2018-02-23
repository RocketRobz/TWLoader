#include "ndsheaderbanner.h"
#include "log.h"
#include "textfns.h"

#include <stdio.h>
#include <malloc.h>
#include <unistd.h>

#include "pp2d/pp2d.h"
#include "graphic.h"

#include <string>
#include <vector>
using std::string;
using std::vector;
using std::wstring;

// Subroutine function signatures arm9
u32 moduleParamsSignature[2]   = {0xDEC00621, 0x2106C0DE};

//
// Look in @data for @find and return the position of it.
//
u32 getOffset(u32* addr, size_t size, u32* find, size_t sizeofFind, int direction)
{
	u32* end = addr + size/sizeof(u32);

    //u32 result = 0;
	bool found = false;

	do {
		for(int i=0;i<(int)sizeofFind;i++) {
			if (addr[i] != find[i]) 
			{
				break;
			} else if(i==(int)sizeofFind-1) {
				found = true;
			}
		}
		if(!found) addr+=direction;
	} while (addr != end && !found);

	if (addr == end) {
		return NULL;
	}

	return (u32)addr;
}

GX_TRANSFER_FORMAT gpuToGxFormat[13] = {
        GX_TRANSFER_FMT_RGBA8,
        GX_TRANSFER_FMT_RGB8,
        GX_TRANSFER_FMT_RGB5A1,
        GX_TRANSFER_FMT_RGB565,
        GX_TRANSFER_FMT_RGBA4,
        GX_TRANSFER_FMT_RGBA8, // Unsupported
        GX_TRANSFER_FMT_RGBA8, // Unsupported
        GX_TRANSFER_FMT_RGBA8, // Unsupported
        GX_TRANSFER_FMT_RGBA8, // Unsupported
        GX_TRANSFER_FMT_RGBA8, // Unsupported
        GX_TRANSFER_FMT_RGBA8, // Unsupported
        GX_TRANSFER_FMT_RGBA8, // Unsupported
        GX_TRANSFER_FMT_RGBA8  // Unsupported
};

/**
 * Convert a color from NDS BGR555 to RGB5A1.
 * @param px16 BGR555 color value.
 * @return RGB5A1 color.
 */
static inline u16 BGR555_to_RGB5A1(u16 px16) {
	// BGR555: xBBBBBGG GGGRRRRR
	// RGB565: RRRRRGGG GGGBBBBB
	// RGB5A1: RRRRRGGG GGBBBBBA
	return   (px16 << 11) |	1 |		// Red (and alpha)
		((px16 <<  1) & 0x07C0) |	// Green
		((px16 >>  9) & 0x003E);	// Blue
}

/**
 * Get the title ID.
 * @param ndsFile DS ROM image.
 * @param buf Output buffer for title ID. (Must be at least 4 characters.)
 * @param isCIA Is the game a CIA?
 * @return 0 on success; non-zero on error.
 */
int grabTID(FILE* ndsFile, char *buf, bool isCia) {
	if(isCia) fseek(ndsFile, offsetof(sNDSHeadertitlecodeonly, gameCode)+0x3900, SEEK_SET);
	else fseek(ndsFile, offsetof(sNDSHeadertitlecodeonly, gameCode), SEEK_SET);
	size_t read = fread(buf, 1, 4, ndsFile);
	return !(read == 4);
}

/**
 * Get text from an NDS banner.
 * @param ndsBanner NDS banner.
 * @param bnrtitlenum Title number. (aka language)
 * @return Vector containing each line as a wide string. (empty on error)
 */
vector<wstring> grabText(const sNDSBanner* ndsBanner, int bnrtitlenum) {
	// Check for unsupported languages.
	switch (bnrtitlenum-1) {
		case N3DS_LANG_DUTCH:
		case N3DS_LANG_PORTUGUESE:
		case N3DS_LANG_RUSSIAN:
			// Use Spanish.
			bnrtitlenum = NDS_LANG_SPANISH;
			break;
		case N3DS_LANG_CHINESE_TRADITIONAL:
			// Use simplified Chinese.
			bnrtitlenum = NDS_LANG_CHINESE;
			break;
		default:
			if (bnrtitlenum > N3DS_LANG_CHINESE_TRADITIONAL) {
				// Invalid language number.
				// Default to English.
				bnrtitlenum = NDS_LANG_ENGLISH;
			}
			break;
	}

	// Check the banner version.
	switch (ndsBanner->version) {
		case NDS_BANNER_VER_ORIGINAL:
			if (bnrtitlenum > NDS_LANG_SPANISH) {
				// Unsupported language. (Chinese or Korean)
				// Default to Japanese.
				bnrtitlenum = NDS_LANG_JAPANESE;
			}
			break;

		case NDS_BANNER_VER_ZH:
			if (bnrtitlenum > NDS_LANG_CHINESE) {
				// Unsupported language. (Korean)
				// Default to Japanese.
				bnrtitlenum = NDS_LANG_JAPANESE;
			}
			break;

		case NDS_BANNER_VER_ZH_KO:
		case NDS_BANNER_VER_DSi:
			break;

		case 0:
		default:
			// Unsupported banner version.
			vector<wstring> vec_wstr;
			vec_wstr.push_back(L"No Info");
			return vec_wstr;
	}

	// Convert the UTF-16 description (with newlines) to a vector of wstrings.
	return utf16_nl_to_vwstring(ndsBanner->titles[bnrtitlenum], 128);
}

/**
 * Get text from a cached NDS banner.
 * @param binFile Banner file.
 * @param bnrtitlenum Title number. (aka language)
 * @return Vector containing each line as a wide string. (empty on error)
 */
vector<wstring> grabText(FILE* binFile, int bnrtitlenum) {
	// Load the banner.
	// NOTE: ndsBanner is cleared first in case the banner
	// file is smaller than NDS_BANNER_SIZE_DSi.
	sNDSBanner ndsBanner;
	memset(&ndsBanner, 0, sizeof(ndsBanner));
	fseek(binFile, 0, SEEK_SET);
	size_t read = fread(&ndsBanner, 1, sizeof(ndsBanner), binFile);
	if (read < NDS_BANNER_VER_ORIGINAL) {
		// Banner file is invalid.
		// Unsupported banner version.
		vector<wstring> vec_wstr;
		vec_wstr.push_back(L"No Info");
		return vec_wstr;
	}

	// Get the banner text.
	return grabText(&ndsBanner, bnrtitlenum);
}

/**
 * Get overlay sizes from an NDS file.
 * @param ndsFile NDS file.
 * @param filename NDS ROM filename.
 * @param isCIA Is the game a CIA?
 * @return 0 on success; non-zero on error.
 */
bool getOverlaySize(FILE* ndsFile, const char* filename, bool isCia) {
	if (logEnabled)	LogFMA("NDSBannerHeader.getOverlaySize", "Reading .NDS file:", filename);
	sNDSHeader NDSHeader;
	if(isCia) fseek(ndsFile, 0x3900, SEEK_SET);
	else fseek(ndsFile, 0, SEEK_SET);
	fread(&NDSHeader, 1, sizeof(NDSHeader), ndsFile);
	if (logEnabled)	LogFMA("NDSBannerHeader.getOverlaySize", ".NDS file read:", filename);

	sNDSBanner ndsBanner;
	memset(&ndsBanner, 0, sizeof(ndsBanner));
	if (NDSHeader.unitCode == 0x02) {
		if (logEnabled)	LogFMA("NDSBannerHeader.getOverlaySize", "ROM is DSi-Enhanced. proceeding:", filename);
		if (NDSHeader.arm9overlaySize == 0x00000000 && NDSHeader.arm7overlaySize == 0x00000000) {
			if (logEnabled)	LogFMA("NDSBannerHeader.getOverlaySize", "ROM has no overlays:", filename);
			return false;
		} else {
			if (logEnabled)	LogFMA("NDSBannerHeader.getOverlaySize", "ROM has overlays:", filename);
			return true;
		}
	} else {
		if (logEnabled)	LogFMA("NDSBannerHeader.getOverlaySize", "ROM is DSi-Exclusive/DSiWare. skipping:", filename);
		return false;
	}
}

char arm9binary[0x400000];

/**
 * Get SDK version from an NDS file.
 * @param ndsFile NDS file.
 * @param filename NDS ROM filename.
 * @return 0 on success; non-zero on error.
 */
u32 getSDKVersion(FILE* ndsFile, const char* filename) {
	if (logEnabled)	LogFMA("NDSBannerHeader.getSDKVersion", "Reading .NDS file:", filename);
	sNDSHeader NDSHeader;
	fseek(ndsFile, 0, SEEK_SET);
	fread(&NDSHeader, 1, sizeof(NDSHeader), ndsFile);
	if (logEnabled)	LogFMA("NDSBannerHeader.getSDKVersion", ".NDS file read:", filename);
	
	fseek(ndsFile , NDSHeader.arm9romOffset, SEEK_SET);
	fread(&arm9binary, 1, NDSHeader.arm9binarySize, ndsFile);

	// Looking for moduleparams
	uint32_t moduleparams = getOffset((u32*)arm9binary, NDSHeader.arm9binarySize, (u32*)moduleParamsSignature, 2, 1);
	if(!moduleparams) {
		if (logEnabled) LogFM("NDSBannerHeader.getSDKVersion", "No SDK version found");
		return 0;
	}

	if (logEnabled) {
		char textSDKver[8];
		snprintf(textSDKver, sizeof(textSDKver), "%x", ((module_params_t*)(moduleparams - 0x1C))->sdk_version);
		LogFMA("NDSBannerHeader.getSDKVersion", "SDK version:", textSDKver);
	}
	return ((module_params_t*)(moduleparams - 0x1C))->sdk_version;
}

/**
 * Cache the banner from an NDS file.
 * @param ndsFile NDS file.
 * @param filename NDS ROM filename.
 * @param isCIA Is the game a CIA?
 * @return 0 on success; non-zero on error.
 */
int cacheBanner(FILE* ndsFile, const char* filename, const char* title, const char* counter1, const char* counter2, bool isCia) {
	char bannerpath[256];
	snprintf(bannerpath, sizeof(bannerpath), "sdmc:/_nds/twloader/bnricons/%s.bin", filename);

	if (!access(bannerpath, F_OK)) {
		// Banner is already cached.
		// TODO: If it's 0 bytes, re-cache it?
		return 0;
	}
	pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
	pp2d_draw_text(12, 16, 0.5f, 0.5f, WHITE, title);
	pp2d_draw_text(12, 48, 0.5f, 0.5f, WHITE, counter1);
	pp2d_draw_text(39, 48, 0.5f, 0.5f, WHITE, "/");
	pp2d_draw_text(44, 48, 0.5f, 0.5f, WHITE, counter2);

	if (logEnabled)	LogFMA("NDSBannerHeader.cacheBanner", "Reading .NDS file:", filename);
	sNDSHeader NDSHeader;
	if(isCia) fseek(ndsFile, 0x3900, SEEK_SET);
	else fseek(ndsFile, 0, SEEK_SET);
	fread(&NDSHeader, 1, sizeof(NDSHeader), ndsFile);
	if (logEnabled)	LogFMA("NDSBannerHeader.cacheBanner", ".NDS file read:", filename);

	sNDSBanner ndsBanner;
	memset(&ndsBanner, 0, sizeof(ndsBanner));
	if(isCia) NDSHeader.bannerOffset += 0x3900;
	u32 bannersize = 0;
	if (NDSHeader.bannerOffset != 0x00000000 && NDSHeader.bannerOffset != 0x00003900) {
		// Read the banner from the NDS file.
		fseek(ndsFile , NDSHeader.bannerOffset, SEEK_SET);
		fread(&ndsBanner, 1, sizeof(ndsBanner), ndsFile);

		pp2d_draw_text(12, 32, 0.5f, 0.5f, WHITE, "Now caching banner data (SD Card)...");
		pp2d_end_draw();
		if (logEnabled)	LogFMA("NDSBannerHeader.cacheBanner", "Caching banner data:", bannerpath);

		switch (ndsBanner.version) {
			case NDS_BANNER_VER_DSi:
				bannersize = NDS_BANNER_SIZE_DSi;
				break;
			case NDS_BANNER_VER_ZH_KO:
				bannersize = NDS_BANNER_SIZE_ZH_KO;
				break;
			case NDS_BANNER_VER_ZH:
				bannersize = NDS_BANNER_SIZE_ZH;
				break;
			case NDS_BANNER_VER_ORIGINAL:
			default:
				bannersize = NDS_BANNER_SIZE_ORIGINAL;
				break;
		}

		/* Banner fixes start here */

		// Fire Emblem - Heroes of Light and Shadow (English Translation)
		if(ndsBanner.crc[0] == 0xECF9
		&& ndsBanner.crc[1] == 0xD18F
		&& ndsBanner.crc[2] == 0xE22A
		&& ndsBanner.crc[3] == 0xD8F4)
		{
			// Use fixed banner.
			FILE* fixedBannerFile = fopen("romfs:/fixedbanners/Fire Emblem - Heroes of Light and Shadow (J) (Eng).bnr", "rb");
			if (logEnabled)	LogFMA("NDSBannerHeader.cacheBanner", "Caching banner data (fixed):", bannerpath);
			bannersize = NDS_BANNER_SIZE_DSi;
			fread(&ndsBanner, 1, bannersize, fixedBannerFile);
			fclose(fixedBannerFile);
		} else // Pokemon Blaze Black (Clean Version)
		if(ndsBanner.crc[0] == 0x4683
		&& ndsBanner.crc[1] == 0x40AD
		&& ndsBanner.crc[2] == 0x5641
		&& ndsBanner.crc[3] == 0xEE5D)
		{
			// Use fixed banner.
			FILE* fixedBannerFile = fopen("romfs:/fixedbanners/Pokemon Blaze Black (Clean Version).bnr", "rb");
			if (logEnabled)	LogFMA("NDSBannerHeader.cacheBanner", "Caching banner data (fixed):", bannerpath);
			bannersize = NDS_BANNER_SIZE_DSi;
			fread(&ndsBanner, 1, bannersize, fixedBannerFile);
			fclose(fixedBannerFile);
		} else // Pokemon Blaze Black (Full Version)
		if(ndsBanner.crc[0] == 0xA251
		&& ndsBanner.crc[1] == 0x40AD
		&& ndsBanner.crc[2] == 0x5641
		&& ndsBanner.crc[3] == 0xEE5D)
		{
			// Use fixed banner.
			FILE* fixedBannerFile = fopen("romfs:/fixedbanners/Pokemon Blaze Black (Full Version).bnr", "rb");
			if (logEnabled)	LogFMA("NDSBannerHeader.cacheBanner", "Caching banner data (fixed):", bannerpath);
			bannersize = NDS_BANNER_SIZE_DSi;
			fread(&ndsBanner, 1, bannersize, fixedBannerFile);
			fclose(fixedBannerFile);
		} else // Pokemon Volt White (Clean Version)
		if(ndsBanner.crc[0] == 0x77F4
		&& ndsBanner.crc[1] == 0x5C94
		&& ndsBanner.crc[2] == 0xBF18
		&& ndsBanner.crc[3] == 0x0C88)
		{
			// Use fixed banner.
			FILE* fixedBannerFile = fopen("romfs:/fixedbanners/Pokemon Volt White (Clean Version).bnr", "rb");
			if (logEnabled)	LogFMA("NDSBannerHeader.cacheBanner", "Caching banner data (fixed):", bannerpath);
			bannersize = NDS_BANNER_SIZE_DSi;
			fread(&ndsBanner, 1, bannersize, fixedBannerFile);
			fclose(fixedBannerFile);
		} else // Pokemon Volt White (Full Version)
		if(ndsBanner.crc[0] == 0x9CA8
		&& ndsBanner.crc[1] == 0x5C94
		&& ndsBanner.crc[2] == 0xBF18
		&& ndsBanner.crc[3] == 0x0C88)
		{
			// Use fixed banner.
			FILE* fixedBannerFile = fopen("romfs:/fixedbanners/Pokemon Volt White (Full Version).bnr", "rb");
			if (logEnabled)	LogFMA("NDSBannerHeader.cacheBanner", "Caching banner data (fixed):", bannerpath);
			bannersize = NDS_BANNER_SIZE_DSi;
			fread(&ndsBanner, 1, bannersize, fixedBannerFile);
			fclose(fixedBannerFile);
		}
	} else {
		// No banner. Use the generic version.
		FILE* nobannerFile = fopen("romfs:/notextbanner", "rb");
		pp2d_draw_text(12, 32, 0.5f, 0.5f, WHITE, "Now caching banner data (SD Card)...");
		pp2d_end_draw();
		if (logEnabled)	LogFMA("NDSBannerHeader.cacheBanner", "Caching banner data (empty):", bannerpath);
		// notextbanner is v0003 (ZH/KO)
		bannersize = NDS_BANNER_SIZE_ZH_KO;
		fread(&ndsBanner, 1, bannersize, nobannerFile);
		fclose(nobannerFile);
	}

	if (bannersize == 0) {
		// Invalid banner.
		if (logEnabled)	LogFMA("NDSBannerHeader.cacheBanner", "Failed to open NDS source file:", filename);
		pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
		pp2d_draw_text(12, 32, 0.5f, 0.5f, WHITE, "Invalid banner loaded; not caching.");
		pp2d_end_draw();
		return -1;
	}

	// Write the cached banner.
	FILE* filetosave = fopen(bannerpath, "wb");
	if (!filetosave) {
		// Error opening the banner cache file.
		if (logEnabled)	LogFMA("NDSBannerHeader.cacheBanner", "Failed to write banner cache file:", bannerpath);
		pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
		pp2d_draw_text(12, 32, 0.5f, 0.5f, WHITE, "Error writing the banner cache file.");
		pp2d_end_draw();
		return -2;
	}
	fwrite(&ndsBanner, 1, bannersize, filetosave);
	fclose(filetosave);
	if (logEnabled)	LogFMA("NDSBannerHeader.cacheBanner", "Banner data cached:", bannerpath);
	return 0;
}

/**
 * Get the icon from an NDS banner.
 * @param binFile NDS banner.
 * @return Icon texture. (NULL on error)
 */
void* grabIconDSi(const sNDSBanner* ndsBanner, int palnum) {
	// Convert the palette from BGR555 to RGB5A1.
	// (We need to ensure the MSB is set for all except
	// color 0, which is transparent.)
	u16 palette[16];
	palette[0] = 0;	// Color 0 is always transparent.
	for (int i = 16-1; i > 0; i--) {
		// Convert from NDS BGR555 to RGB5A1.
		// NOTE: The GPU expects byteswapped data.
		palette[i] = BGR555_to_RGB5A1(ndsBanner->dsi_palette[palnum][i]);
	}

	static const int width = 32;
	static const int height = 256; // 32 8x for 8 icon frames
	u8 icon[32 * 256 * 2];
	for(u32 x = 0; x < 32; x++) {
		for(u32 y = 0; y < 32*8; y++) {
			u32 srcPos = (((y >> 3) * 4 + (x >> 3)) * 8 + (y & 7)) * 4 + ((x & 7) >> 1);
			u32 srcShift = (x & 1) * 4;
			u16 srcPx = palette[(ndsBanner->dsi_icon[0][srcPos] >> srcShift) & 0xF];

			u32 dstPos = (y * 32 + x) * 2;
			icon[dstPos + 0] = (u8) (srcPx & 0xFF);
			icon[dstPos + 1] = (u8) ((srcPx >> 8) & 0xFF);
		}
	}

	u32 pixelSize = sizeof(icon) / width / height;

	u8* textureData = (u8*)linearAlloc(32 * 256 * pixelSize);

	memset(textureData, 0, 32 * 256 * pixelSize);

	for(u32 x = 0; x < width; x++) {
		for(u32 y = 0; y < height; y++) {
			u32 dataPos = (y * width + x) * pixelSize;
			u32 texPos = (y * 32 + x) * pixelSize;

			for(u32 i = 0; i < pixelSize; i++) {
				textureData[texPos + i] = ((u8*) icon)[dataPos + i];
			}
		}
	}

	return textureData;
}

/**
 * Get the icon from an NDS banner.
 * @param binFile NDS banner.
 * @return Icon texture. (NULL on error)
 */
void* grabIcon(const sNDSBanner* ndsBanner) {
	// Convert the palette from BGR555 to RGB5A1.
	// (We need to ensure the MSB is set for all except
	// color 0, which is transparent.)
	u16 palette[16];
	palette[0] = 0;	// Color 0 is always transparent.
	for (int i = 16-1; i > 0; i--) {
		// Convert from NDS BGR555 to RGB5A1.
		// NOTE: The GPU expects byteswapped data.
		palette[i] = BGR555_to_RGB5A1(ndsBanner->palette[i]);
	}

	static const int width = 32;
	static const int height = 32;
	u8 icon[32 * 32 * 2];
	for(u32 x = 0; x < 32; x++) {
		for(u32 y = 0; y < 32; y++) {
			u32 srcPos = (((y >> 3) * 4 + (x >> 3)) * 8 + (y & 7)) * 4 + ((x & 7) >> 1);
			u32 srcShift = (x & 1) * 4;
			u16 srcPx = palette[(ndsBanner->icon[srcPos] >> srcShift) & 0xF];

			u32 dstPos = (y * 32 + x) * 2;
			icon[dstPos + 0] = (u8) (srcPx & 0xFF);
			icon[dstPos + 1] = (u8) ((srcPx >> 8) & 0xFF);
		}
	}

	u32 pixelSize = sizeof(icon) / width / height;

	u8* textureData = (u8*)linearAlloc(32 * 64 * pixelSize);

	memset(textureData, 0, 32 * 64 * pixelSize);

	for(u32 x = 0; x < width; x++) {
		for(u32 y = 0; y < height; y++) {
			u32 dataPos = (y * width + x) * pixelSize;
			u32 texPos = (y * 32 + x) * pixelSize;

			for(u32 i = 0; i < pixelSize; i++) {
				textureData[texPos + i] = ((u8*) icon)[dataPos + i];
			}
		}
	}

	return textureData;
}

/**
 * Get the icon from a cached NDS banner.
 * @param binFile Banner file.
 * @return Icon texture. (NULL on error)
 */
void* grabIconDSi(FILE* binFile, int palnum) {
	sNDSBanner ndsBanner;
	fseek(binFile, 0, SEEK_SET);
	size_t read = fread(&ndsBanner, 1, sizeof(ndsBanner), binFile);
	if (read < NDS_BANNER_VER_ORIGINAL) {
		// Banner file is invalid.
		return NULL;
	}

	// Get the icon.
	return grabIconDSi(&ndsBanner, palnum);
}

/**
 * Get the icon from a cached NDS banner.
 * @param binFile Banner file.
 * @return Icon texture. (NULL on error)
 */
void* grabIcon(FILE* binFile) {
	sNDSBanner ndsBanner;
	fseek(binFile, 0, SEEK_SET);
	size_t read = fread(&ndsBanner, 1, sizeof(ndsBanner), binFile);
	if (read < NDS_BANNER_VER_ORIGINAL) {
		// Banner file is invalid.
		return NULL;
	}

	// Get the icon.
	return grabIcon(&ndsBanner);
}

/**
 * Get banner version from NDS banner.
 * @param binFile Banner file.
 * @return Version number. (NULL on error)
 */
u16 grabBannerVersion(FILE* binFile) {
	sNDSBanner ndsBanner;
	fseek(binFile, 0, SEEK_SET);
	size_t read = fread(&ndsBanner, 1, sizeof(ndsBanner), binFile);
	if (read < NDS_BANNER_VER_ORIGINAL) {
		// Banner file is invalid.
		return NULL;
	}

	// Return banner version.
	return ndsBanner.version;
}

// bnriconframeseq[]: 0-19; 20 is for R4 theme only, 21 is for game cart
static u16 bnriconframeseq[gamesPerPage+2][64] = {0x0000};

// bnriconframenum[]: 0-19; 20 is for R4 theme only, 21 is for game cart
int bnriconPalLine[gamesPerPage+2] = {0};
int bnriconframenumY[gamesPerPage+2] = {0};
flipType bannerFlip[gamesPerPage+2] = {NONE};

// bnriconisDSi[]: 0-19; 20 is for R4 theme only, 21 is for game cart
bool bnriconisDSi[gamesPerPage+2] = {false};

/**
 * Get banner sequence from banner file.
 * @param binFile Banner file.
 */
void* grabBannerSequence(const sNDSBanner* ndsBanner, int iconnum) {
	for (int i = 0; i < 64; i++) {
		bnriconframeseq[iconnum][i] = ndsBanner->dsi_seq[i];
	}
	return 0;
}

/**
 * Get banner sequence from banner file.
 * @param binFile Banner file.
 */
void* grabBannerSequence(FILE* binFile, int iconnum) {
	sNDSBanner ndsBanner;
	fseek(binFile, 0, SEEK_SET);
	size_t read = fread(&ndsBanner, 1, sizeof(ndsBanner), binFile);
	
	return grabBannerSequence(&ndsBanner, iconnum);
}

/**
 * Clear loaded banner sequence.
 */
void clearBannerSequence(int iconnum) {
	for (int i = 0; i < 64; i++) {
		bnriconframeseq[iconnum][i] = 0x0000;
	}
}

static u16 bannerDelayNum[gamesPerPage+2] = {0x0000};
int currentbnriconframeseq[gamesPerPage+2] = {0};

/**
 * Play banner sequence.
 * @param binFile Banner file.
 */
void playBannerSequence(int iconnum) {
	if(bnriconframeseq[iconnum][currentbnriconframeseq[iconnum]] == 0x0001
	&& bnriconframeseq[iconnum][currentbnriconframeseq[iconnum]+1] == 0x0100) {
		// Do nothing if icon isn't animated
		bnriconPalLine[iconnum] = 0;
		bnriconframenumY[iconnum] = 0;
		bannerFlip[iconnum] = NONE;
	} else {
		u16 setframeseq = bnriconframeseq[iconnum][currentbnriconframeseq[iconnum]];

		// Check for pal line 1
		if((setframeseq >= 0x0000)
		&& (setframeseq < 0x0100)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x0100)
				&& (setframeseq < 0x0200)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x0200)
				&& (setframeseq < 0x0300)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x0300)
				&& (setframeseq < 0x0400)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x0400)
				&& (setframeseq < 0x0500)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x0500)
				&& (setframeseq < 0x0600)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x0600)
				&& (setframeseq < 0x0700)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x0700)
				&& (setframeseq < 0x0800)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = NONE;
		} else // Check for pal line 2
		if((setframeseq >= 0x0800)
		&& (setframeseq < 0x0900)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x0900)
				&& (setframeseq < 0x0A00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x0A00)
				&& (setframeseq < 0x0B00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x0B00)
				&& (setframeseq < 0x0C00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x0C00)
				&& (setframeseq < 0x0D00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x0D00)
				&& (setframeseq < 0x0E00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x0E00)
				&& (setframeseq < 0x0F00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x0F00)
				&& (setframeseq < 0x1000)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = NONE;
		} else // Check for pal line 3
		if((setframeseq >= 0x1000)
		&& (setframeseq < 0x1100)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x1100)
				&& (setframeseq < 0x1200)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x1200)
				&& (setframeseq < 0x1300)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x1300)
				&& (setframeseq < 0x1400)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x1400)
				&& (setframeseq < 0x1500)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x1500)
				&& (setframeseq < 0x1600)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x1600)
				&& (setframeseq < 0x1700)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x1700)
				&& (setframeseq < 0x1800)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = NONE;
		} else // Check for pal line 4
		if((setframeseq >= 0x1800)
		&& (setframeseq < 0x1900)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x1900)
				&& (setframeseq < 0x1A00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x1A00)
				&& (setframeseq < 0x1B00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x1B00)
				&& (setframeseq < 0x1C00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x1C00)
				&& (setframeseq < 0x1D00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x1D00)
				&& (setframeseq < 0x1E00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x1E00)
				&& (setframeseq < 0x1F00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x1F00)
				&& (setframeseq < 0x2000)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = NONE;
		} else // Check for pal line 5
		if((setframeseq >= 0x2000)
		&& (setframeseq < 0x2100)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x2100)
				&& (setframeseq < 0x2200)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x2200)
				&& (setframeseq < 0x2300)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x2300)
				&& (setframeseq < 0x2400)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x2400)
				&& (setframeseq < 0x2500)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x2500)
				&& (setframeseq < 0x2600)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x2600)
				&& (setframeseq < 0x2700)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x2700)
				&& (setframeseq < 0x2800)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = NONE;
		} else // Check for pal line 6
		if((setframeseq >= 0x2800)
		&& (setframeseq < 0x2900)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x2900)
				&& (setframeseq < 0x2A00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x2A00)
				&& (setframeseq < 0x2B00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x2B00)
				&& (setframeseq < 0x2C00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x2C00)
				&& (setframeseq < 0x2D00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x2D00)
				&& (setframeseq < 0x2E00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x2E00)
				&& (setframeseq < 0x2F00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x2F00)
				&& (setframeseq < 0x3000)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = NONE;
		} else // Check for pal line 7
		if((setframeseq >= 0x3000)
		&& (setframeseq < 0x3100)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x3100)
				&& (setframeseq < 0x3200)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x3200)
				&& (setframeseq < 0x3300)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x3300)
				&& (setframeseq < 0x3400)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x3400)
				&& (setframeseq < 0x3500)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x3500)
				&& (setframeseq < 0x3600)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x3600)
				&& (setframeseq < 0x3700)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x3700)
				&& (setframeseq < 0x3800)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = NONE;
		} else // Check for pal line 8
		if((setframeseq >= 0x3800)
		&& (setframeseq < 0x3900)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x3900)
				&& (setframeseq < 0x3A00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x3A00)
				&& (setframeseq < 0x3B00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x3B00)
				&& (setframeseq < 0x3C00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x3C00)
				&& (setframeseq < 0x3D00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x3D00)
				&& (setframeseq < 0x3E00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x3E00)
				&& (setframeseq < 0x3F00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = NONE;
		} else if((setframeseq >= 0x3F00)
				&& (setframeseq < 0x4000)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = NONE;
		} else // Check for horizontally flipped frames
		// Check for pal line 1
		if((setframeseq >= 0x4000)
		&& (setframeseq < 0x4100)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x4100)
				&& (setframeseq < 0x4200)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x4200)
				&& (setframeseq < 0x4300)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x4300)
				&& (setframeseq < 0x4400)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x4400)
				&& (setframeseq < 0x4500)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x4500)
				&& (setframeseq < 0x4600)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x4600)
				&& (setframeseq < 0x4700)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x4700)
				&& (setframeseq < 0x4800)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = HORIZONTAL;
		} else // Check for pal line 2
		if((setframeseq >= 0x4800)
		&& (setframeseq < 0x4900)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x4900)
				&& (setframeseq < 0x4A00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x4A00)
				&& (setframeseq < 0x4B00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x4B00)
				&& (setframeseq < 0x4C00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x4C00)
				&& (setframeseq < 0x4D00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x4D00)
				&& (setframeseq < 0x4E00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x4E00)
				&& (setframeseq < 0x4F00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x4F00)
				&& (setframeseq < 0x5000)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = HORIZONTAL;
		} else // Check for pal line 3
		if((setframeseq >= 0x5000)
		&& (setframeseq < 0x5100)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x5100)
				&& (setframeseq < 0x5200)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x5200)
				&& (setframeseq < 0x5300)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x5300)
				&& (setframeseq < 0x5400)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x5400)
				&& (setframeseq < 0x5500)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x5500)
				&& (setframeseq < 0x5600)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x5600)
				&& (setframeseq < 0x5700)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x5700)
				&& (setframeseq < 0x5800)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = HORIZONTAL;
		} else // Check for pal line 4
		if((setframeseq >= 0x5800)
		&& (setframeseq < 0x5900)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x5900)
				&& (setframeseq < 0x5A00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x5A00)
				&& (setframeseq < 0x5B00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x5B00)
				&& (setframeseq < 0x5C00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x5C00)
				&& (setframeseq < 0x5D00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x5D00)
				&& (setframeseq < 0x5E00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x5E00)
				&& (setframeseq < 0x5F00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x5F00)
				&& (setframeseq < 0x6000)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = HORIZONTAL;
		} else // Check for pal line 5
		if((setframeseq >= 0x6000)
		&& (setframeseq < 0x6100)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x6100)
				&& (setframeseq < 0x6200)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x6200)
				&& (setframeseq < 0x6300)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x6300)
				&& (setframeseq < 0x6400)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x6400)
				&& (setframeseq < 0x6500)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x6500)
				&& (setframeseq < 0x6600)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x6600)
				&& (setframeseq < 0x6700)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x6700)
				&& (setframeseq < 0x6800)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = HORIZONTAL;
		} else // Check for pal line 6
		if((setframeseq >= 0x6800)
		&& (setframeseq < 0x6900)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x6900)
				&& (setframeseq < 0x6A00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x6A00)
				&& (setframeseq < 0x6B00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x6B00)
				&& (setframeseq < 0x6C00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x6C00)
				&& (setframeseq < 0x6D00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x6D00)
				&& (setframeseq < 0x6E00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x6E00)
				&& (setframeseq < 0x6F00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x6F00)
				&& (setframeseq < 0x7000)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = HORIZONTAL;
		} else // Check for pal line 7
		if((setframeseq >= 0x7000)
		&& (setframeseq < 0x7100)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x7100)
				&& (setframeseq < 0x7200)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x7200)
				&& (setframeseq < 0x7300)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x7300)
				&& (setframeseq < 0x7400)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x7400)
				&& (setframeseq < 0x7500)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x7500)
				&& (setframeseq < 0x7600)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x7600)
				&& (setframeseq < 0x7700)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x7700)
				&& (setframeseq < 0x7800)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = HORIZONTAL;
		} else // Check for pal line 8
		if((setframeseq >= 0x7800)
		&& (setframeseq < 0x7900)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x7900)
				&& (setframeseq < 0x7A00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x7A00)
				&& (setframeseq < 0x7B00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x7B00)
				&& (setframeseq < 0x7C00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x7C00)
				&& (setframeseq < 0x7D00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x7D00)
				&& (setframeseq < 0x7E00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x7E00)
				&& (setframeseq < 0x7F00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = HORIZONTAL;
		} else if((setframeseq >= 0x7F00)
				&& (setframeseq < 0x8000)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = HORIZONTAL;
		} else // Check for vertically flipped frames
		// Check for pal line 1
		if((setframeseq >= 0x8000)
		&& (setframeseq < 0x8100)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x8100)
				&& (setframeseq < 0x8200)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x8200)
				&& (setframeseq < 0x8300)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x8300)
				&& (setframeseq < 0x8400)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x8400)
				&& (setframeseq < 0x8500)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x8500)
				&& (setframeseq < 0x8600)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x8600)
				&& (setframeseq < 0x8700)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x8700)
				&& (setframeseq < 0x8800)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = VERTICAL;
		} else // Check for pal line 2
		if((setframeseq >= 0x8800)
		&& (setframeseq < 0x8900)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x8900)
				&& (setframeseq < 0x8A00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x8A00)
				&& (setframeseq < 0x8B00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x8B00)
				&& (setframeseq < 0x8C00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x8C00)
				&& (setframeseq < 0x8D00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x8D00)
				&& (setframeseq < 0x8E00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x8E00)
				&& (setframeseq < 0x8F00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x8F00)
				&& (setframeseq < 0x9000)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = VERTICAL;
		} else // Check for pal line 3
		if((setframeseq >= 0x9000)
		&& (setframeseq < 0x9100)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x9100)
				&& (setframeseq < 0x9200)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x9200)
				&& (setframeseq < 0x9300)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x9300)
				&& (setframeseq < 0x9400)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x9400)
				&& (setframeseq < 0x9500)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x9500)
				&& (setframeseq < 0x9600)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x9600)
				&& (setframeseq < 0x9700)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x9700)
				&& (setframeseq < 0x9800)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = VERTICAL;
		} else // Check for pal line 4
		if((setframeseq >= 0x9800)
		&& (setframeseq < 0x9900)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x9900)
				&& (setframeseq < 0x9A00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x9A00)
				&& (setframeseq < 0x9B00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x9B00)
				&& (setframeseq < 0x9C00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x9C00)
				&& (setframeseq < 0x9D00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x9D00)
				&& (setframeseq < 0x9E00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x9E00)
				&& (setframeseq < 0x9F00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0x9F00)
				&& (setframeseq < 0xA000)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = VERTICAL;
		} else // Check for pal line 5
		if((setframeseq >= 0xA000)
		&& (setframeseq < 0xA100)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xA100)
				&& (setframeseq < 0xA200)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xA200)
				&& (setframeseq < 0xA300)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xA300)
				&& (setframeseq < 0xA400)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xA400)
				&& (setframeseq < 0xA500)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xA500)
				&& (setframeseq < 0xA600)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xA600)
				&& (setframeseq < 0xA700)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xA700)
				&& (setframeseq < 0xA800)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = VERTICAL;
		} else // Check for pal line 6
		if((setframeseq >= 0xA800)
		&& (setframeseq < 0xA900)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xA900)
				&& (setframeseq < 0xAA00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xAA00)
				&& (setframeseq < 0xAB00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xAB00)
				&& (setframeseq < 0xAC00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xAC00)
				&& (setframeseq < 0xAD00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xAD00)
				&& (setframeseq < 0xAE00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xAE00)
				&& (setframeseq < 0xAF00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xAF00)
				&& (setframeseq < 0xB000)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = VERTICAL;
		} else // Check for pal line 7
		if((setframeseq >= 0xB000)
		&& (setframeseq < 0xB100)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xB100)
				&& (setframeseq < 0xB200)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xB200)
				&& (setframeseq < 0xB300)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xB300)
				&& (setframeseq < 0xB400)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xB400)
				&& (setframeseq < 0xB500)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xB500)
				&& (setframeseq < 0xB600)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xB600)
				&& (setframeseq < 0xB700)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xB700)
				&& (setframeseq < 0xB800)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = VERTICAL;
		} else // Check for pal line 8
		if((setframeseq >= 0xB800)
		&& (setframeseq < 0xB900)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xB900)
				&& (setframeseq < 0xBA00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xBA00)
				&& (setframeseq < 0xBB00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xBB00)
				&& (setframeseq < 0xBC00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xBC00)
				&& (setframeseq < 0xBD00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xBD00)
				&& (setframeseq < 0xBE00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xBE00)
				&& (setframeseq < 0xBF00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = VERTICAL;
		} else if((setframeseq >= 0xBF00)
				&& (setframeseq < 0xC000)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = VERTICAL;
		} else // Check for both horizontally & vertically flipped frames
		// Check for pal line 1
		if((setframeseq >= 0xC000)
		&& (setframeseq < 0xC100)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xC100)
				&& (setframeseq < 0xC200)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xC200)
				&& (setframeseq < 0xC300)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xC300)
				&& (setframeseq < 0xC400)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xC400)
				&& (setframeseq < 0xC500)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xC500)
				&& (setframeseq < 0xC600)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xC600)
				&& (setframeseq < 0xC700)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xC700)
				&& (setframeseq < 0xC800)) {
			bnriconPalLine[iconnum] = 0;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = BOTH;
		} else // Check for pal line 2
		if((setframeseq >= 0xC800)
		&& (setframeseq < 0xC900)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xC900)
				&& (setframeseq < 0xCA00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xCA00)
				&& (setframeseq < 0xCB00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xCB00)
				&& (setframeseq < 0xCC00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xCC00)
				&& (setframeseq < 0xCD00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xCD00)
				&& (setframeseq < 0xCE00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xCE00)
				&& (setframeseq < 0xCF00)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xCF00)
				&& (setframeseq < 0xD000)) {
			bnriconPalLine[iconnum] = 1;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = BOTH;
		} else // Check for pal line 3
		if((setframeseq >= 0xD000)
		&& (setframeseq < 0xD100)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xD100)
				&& (setframeseq < 0xD200)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xD200)
				&& (setframeseq < 0xD300)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xD300)
				&& (setframeseq < 0xD400)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xD400)
				&& (setframeseq < 0xD500)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xD500)
				&& (setframeseq < 0xD600)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xD600)
				&& (setframeseq < 0xD700)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xD700)
				&& (setframeseq < 0xD800)) {
			bnriconPalLine[iconnum] = 2;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = BOTH;
		} else // Check for pal line 4
		if((setframeseq >= 0xD800)
		&& (setframeseq < 0xD900)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xD900)
				&& (setframeseq < 0xDA00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xDA00)
				&& (setframeseq < 0xDB00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xDB00)
				&& (setframeseq < 0xDC00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xDC00)
				&& (setframeseq < 0xDD00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xDD00)
				&& (setframeseq < 0xDE00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xDE00)
				&& (setframeseq < 0xDF00)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xDF00)
				&& (setframeseq < 0xE000)) {
			bnriconPalLine[iconnum] = 3;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = BOTH;
		} else // Check for pal line 5
		if((setframeseq >= 0xE000)
		&& (setframeseq < 0xE100)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xE100)
				&& (setframeseq < 0xE200)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xE200)
				&& (setframeseq < 0xE300)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xE300)
				&& (setframeseq < 0xE400)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xE400)
				&& (setframeseq < 0xE500)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xE500)
				&& (setframeseq < 0xE600)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xE600)
				&& (setframeseq < 0xE700)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xE700)
				&& (setframeseq < 0xE800)) {
			bnriconPalLine[iconnum] = 4;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = BOTH;
		} else // Check for pal line 6
		if((setframeseq >= 0xE800)
		&& (setframeseq < 0xE900)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xE900)
				&& (setframeseq < 0xEA00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xEA00)
				&& (setframeseq < 0xEB00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xEB00)
				&& (setframeseq < 0xEC00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xEC00)
				&& (setframeseq < 0xED00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xED00)
				&& (setframeseq < 0xEE00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xEE00)
				&& (setframeseq < 0xEF00)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xEF00)
				&& (setframeseq < 0xF000)) {
			bnriconPalLine[iconnum] = 5;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = BOTH;
		} else // Check for pal line 7
		if((setframeseq >= 0xF000)
		&& (setframeseq < 0xF100)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xF100)
				&& (setframeseq < 0xF200)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xF200)
				&& (setframeseq < 0xF300)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xF300)
				&& (setframeseq < 0xF400)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xF400)
				&& (setframeseq < 0xF500)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xF500)
				&& (setframeseq < 0xF600)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xF600)
				&& (setframeseq < 0xF700)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xF700)
				&& (setframeseq < 0xF800)) {
			bnriconPalLine[iconnum] = 6;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = BOTH;
		} else // Check for pal line 8
		if((setframeseq >= 0xF800)
		&& (setframeseq < 0xF900)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 0;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xF900)
				&& (setframeseq < 0xFA00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 1;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xFA00)
				&& (setframeseq < 0xFB00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 2;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xFB00)
				&& (setframeseq < 0xFC00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 3;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xFC00)
				&& (setframeseq < 0xFD00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 4;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xFD00)
				&& (setframeseq < 0xFE00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 5;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xFE00)
				&& (setframeseq < 0xFF00)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 6;
			bannerFlip[iconnum] = BOTH;
		} else if((setframeseq >= 0xFF00)
				&& (setframeseq < 0x10000)) {
			bnriconPalLine[iconnum] = 7;
			bnriconframenumY[iconnum] = 7;
			bannerFlip[iconnum] = BOTH;
		}

		bannerDelayNum[iconnum]++;
		if(bannerDelayNum[iconnum] >= (setframeseq & 0x00FF)) {
			bannerDelayNum[iconnum] = 0x0000;
			currentbnriconframeseq[iconnum]++;
			if(bnriconframeseq[iconnum][currentbnriconframeseq[iconnum]] == 0x0000) {
				currentbnriconframeseq[iconnum] = 0;	// Reset sequence
			}
		}

	}
}
