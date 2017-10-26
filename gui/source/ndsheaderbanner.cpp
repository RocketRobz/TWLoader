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

// From FBI core/screen.c
//Grabbed from: http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
unsigned int next_pow2(u32 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    i |= i >> 16;
    i++;

    return i;
}

typedef struct {
	C3D_Tex tex;
	int width;
	int height;
} ndsicon;

/**
 * Get the icon from an NDS banner.
 * @param binFile NDS banner.
 * @return Icon texture. (NULL on error)
 */
void* grabIcon(const sNDSBanner* ndsBanner) {
	// Un-tile the icon.
	// NOTE: Allocating 64x32, because the "sf2d_create_texture_mem_RGBA8" (deprecated)
	// function hates small sizes like 32x32 (TWLoader freezes if that size is used).
	static const int width = 32;
	static const int height = 32;
	u8 icon[32 * 32 * 2];
	for(u32 x = 0; x < 32; x++) {
		for(u32 y = 0; y < 32; y++) {
			u32 srcPos = (((y >> 3) * 4 + (x >> 3)) * 8 + (y & 7)) * 4 + ((x & 7) >> 1);
			u32 srcShift = (x & 1) * 4;
			u16 srcPx = ndsBanner->palette[(ndsBanner->icon[srcPos] >> srcShift) & 0xF];

			u8 r = (u8) (srcPx & 0x1F);
			u8 g = (u8) ((srcPx >> 5) & 0x1F);
			u8 b = (u8) ((srcPx >> 10) & 0x1F);

			u16 reversedPx = (u16) ((r << 11) | (g << 6) | (b << 1) | 1);

			u32 dstPos = (y * 32 + x) * 2;
			icon[dstPos + 0] = (u8) (reversedPx & 0xFF);
			icon[dstPos + 1] = (u8) ((reversedPx >> 8) & 0xFF);
		}
	}

	// Based on sf2d_create_texture
	ndsicon *texture = (ndsicon *) calloc(1, sizeof(*texture));
	if (!texture) return NULL;

	bool success = false;

    u32 pow2Width = next_pow2(width);

    if(pow2Width < 64) {
        pow2Width = 64;
    }

    u32 pow2Height = next_pow2(height);
    if(pow2Height < 64) {
        pow2Height = 64;
    }

    u32 pixelSize = sizeof(icon) / width / height;

    u8* pow2Tex = (u8*)linearAlloc(pow2Width * pow2Height * pixelSize);

    memset(pow2Tex, 0, pow2Width * pow2Height * pixelSize);

    for(u32 x = 0; x < width; x++) {
        for(u32 y = 0; y < height; y++) {
            u32 dataPos = (y * width + x) * pixelSize;
            u32 pow2TexPos = (y * pow2Width + x) * pixelSize;

            for(u32 i = 0; i < pixelSize; i++) {
                pow2Tex[pow2TexPos + i] = ((u8*) icon)[dataPos + i];
            }
        }
    }

	success = C3D_TexInit(&texture->tex, (int) pow2Width, (int) pow2Height, GPU_RGBA5551); // RGB5A1
	if (!success) {
		if (logEnabled) LogFM("NDS banner icon", "Error creating texture!");
		free(texture);
		return NULL;
	}

    GSPGPU_FlushDataCache(pow2Tex, pow2Width * pow2Height * 4);

    C3D_SafeDisplayTransfer((u32*) pow2Tex, GX_BUFFER_DIM(pow2Width, pow2Height), (u32*) texture->tex.data, GX_BUFFER_DIM(pow2Width, pow2Height), GX_TRANSFER_FLIP_VERT(1) | GX_TRANSFER_OUT_TILED(1) | GX_TRANSFER_RAW_COPY(0) | GX_TRANSFER_IN_FORMAT((u32) gpuToGxFormat[GPU_RGBA5551]) | GX_TRANSFER_OUT_FORMAT((u32) gpuToGxFormat[GPU_RGBA5551]) | GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO));
    gspWaitForPPF();

    linearFree(pow2Tex);
	
	return pow2Tex;
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
