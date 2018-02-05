#include "ndsheaderbanner.h"
#include "main.h"
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
	pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
	pp2d_draw_text(12, 16, 0.5f, 0.5f, WHITE, title);
	pp2d_draw_text(12, 48, 0.5f, 0.5f, WHITE, counter1);
	pp2d_draw_text(39, 48, 0.5f, 0.5f, WHITE, "/");
	pp2d_draw_text(44, 16, 0.5f, 0.5f, WHITE, counter2);

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
	} else {
		// No banner. Use the generic version.
		FILE* nobannerFile = fopen("romfs:/notextbanner", "rb");
		pp2d_draw_text(12, 32, 0.5f, 0.5f, WHITE, "Now caching banner data (SD Card)...");
		if (logEnabled)	LogFMA("NDSBannerHeader.cacheBanner", "Caching banner data (empty):", bannerpath);
		// notextbanner is v0003 (ZH/KO)
		bannersize = NDS_BANNER_SIZE_ZH_KO;
		fread(&ndsBanner, 1, bannersize, nobannerFile);
		fclose(nobannerFile);
	}

	if (bannersize == 0) {
		// Invalid banner.
		if (logEnabled)	LogFMA("NDSBannerHeader.cacheBanner", "Failed to open NDS source file:", filename);
		pp2d_draw_text(12, 32, 0.5f, 0.5f, WHITE, "Invalid banner loaded; not caching.");
		return -1;
	}

	// Write the cached banner.
	FILE* filetosave = fopen(bannerpath, "wb");
	if (!filetosave) {
		// Error opening the banner cache file.
		if (logEnabled)	LogFMA("NDSBannerHeader.cacheBanner", "Failed to write banner cache file:", bannerpath);
		pp2d_draw_text(12, 32, 0.5f, 0.5f, WHITE, "Error writing the banner cache file.");
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
void* grabIconDSi(const sNDSBanner* ndsBanner) {
	// Convert the palette from BGR555 to RGB5A1.
	// (We need to ensure the MSB is set for all except
	// color 0, which is transparent.)
	u16 palette[8][16];
	for (int i = 0; i < 8; i++) {
		palette[i][0] = 0;	// Color 0 is always transparent.
		for (int i2 = 16-1; i2 > 0; i2--) {
			// Convert from NDS BGR555 to RGB5A1.
			// NOTE: The GPU expects byteswapped data.
			palette[i][i2] = BGR555_to_RGB5A1(ndsBanner->dsi_palette[i][i2]);
		}
	}

	static const int width = 32;
	static const int height = 256; // 32 8x for 8 icon frames
	u8 icon[32 * 256 * 2];
	for(u32 x = 0; x < 32; x++) {
		for(u32 y = 0; y < 32*8; y++) {
			u32 srcPos = (((y >> 3) * 4 + (x >> 3)) * 8 + (y & 7)) * 4 + ((x & 7) >> 1);
			u32 srcShift = (x & 1) * 4;
			u16 srcPx = palette[0][(ndsBanner->dsi_icon[0][srcPos] >> srcShift) & 0xF];

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
void* grabIconDSi(FILE* binFile) {
	sNDSBanner ndsBanner;
	fseek(binFile, 0, SEEK_SET);
	size_t read = fread(&ndsBanner, 1, sizeof(ndsBanner), binFile);
	if (read < NDS_BANNER_VER_ORIGINAL) {
		// Banner file is invalid.
		return NULL;
	}

	// Get the icon.
	return grabIconDSi(&ndsBanner);
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
u32 grabBannerVersion(FILE* binFile) {
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
static u16 bnriconframeseq[22][64] = {0x0000};

// bnriconframenum[]: 0-19; 20 is for R4 theme only, 21 is for game cart
int bnriconframenumX[22] = {0};
int bnriconframenumY[22] = {0};

// bnriconisDSi[]: 0-19; 20 is for R4 theme only, 21 is for game cart
bool bnriconisDSi[22] = {false};

/**
 * Get banner sequence from banner file.
 * @param binFile Banner file.
 */
void grabBannerSequence(FILE* binFile, int iconnum) {
	sNDSBanner ndsBanner;
	fseek(binFile, 0, SEEK_SET);
	size_t read = fread(&ndsBanner, 1, sizeof(ndsBanner), binFile);
	
	for (int i = 0; i < 64; i++) {
		bnriconframeseq[iconnum][i] = ndsBanner.dsi_seq[i];
	}
}

/**
 * Clear loaded banner sequence.
 */
void clearBannerSequence(int iconnum) {
	for (int i = 0; i < 64; i++) {
		bnriconframeseq[iconnum][i] = 0x0000;
	}
}

static u16 bannerDelayNum[22] = {0x0000};
int currentbnriconframeseq[22] = {0};

/**
 * Play banner sequence.
 * @param binFile Banner file.
 */
void playBannerSequence(int iconnum) {
	if(bnriconframeseq[iconnum][currentbnriconframeseq[iconnum]] == 0x0001
	&& bnriconframeseq[iconnum][currentbnriconframeseq[iconnum]+1] == 0x0100) {
		// Do nothing if icon isn't animated
		bnriconframenumX[iconnum] = 0;
		bnriconframenumY[iconnum] = 0;
	} else {
		u16 setframeseq = bnriconframeseq[iconnum][currentbnriconframeseq[iconnum]];

		if((setframeseq >= 0x0000) && (setframeseq < 0x0100)) {
			bnriconframenumY[iconnum] = 0;
		} else if((setframeseq >= 0x0100) && (setframeseq < 0x0200)) {
			bnriconframenumY[iconnum] = 1;
		} else if((setframeseq >= 0x0200) && (setframeseq < 0x0300)) {
			bnriconframenumY[iconnum] = 2;
		} else if((setframeseq >= 0x0300) && (setframeseq < 0x0400)) {
			bnriconframenumY[iconnum] = 3;
		} else if((setframeseq >= 0x0400) && (setframeseq < 0x0500)) {
			bnriconframenumY[iconnum] = 4;
		} else if((setframeseq >= 0x0500) && (setframeseq < 0x0600)) {
			bnriconframenumY[iconnum] = 5;
		} else if((setframeseq >= 0x0600) && (setframeseq < 0x0700)) {
			bnriconframenumY[iconnum] = 6;
		} else if((setframeseq >= 0x0700) && (setframeseq < 0x0800)) {
			bnriconframenumY[iconnum] = 7;
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
