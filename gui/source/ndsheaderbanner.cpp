#include "ndsheaderbanner.h"
#include "main.h"
#include "log.h"
#include "textfns.h"

#include <stdio.h>
#include <malloc.h>
#include <unistd.h>

#include "pp2d/pp2d/pp2d.h"
#include "graphic.h"

#include <string>
#include <vector>
using std::string;
using std::vector;
using std::wstring;

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

// From sf2d_texture.c
//Grabbed from: http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
unsigned int next_pow2(unsigned int v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v >= 32 ? v : 32;
}

typedef struct {
	C3D_Tex tex;
	int width;
	int height;
} ndsicon;

ndsicon *ndsicon_create_texture(int w, int h)
{
	// Based on sf2d_create_texture
	ndsicon *texture = (ndsicon *) calloc(1, sizeof(*texture));
	if (!texture) return NULL;

	bool success = false;
	texture->width = w;
	texture->height = h;
	success = C3D_TexInit(&texture->tex, next_pow2(w), next_pow2(h), GPU_RGBA5551); // RGB5A1

	if (!success) {
		if (logEnabled) LogFM("NDS banner icon", "Error creating texture!");
		free(texture);
		return NULL;
	}

	C3D_TexSetWrap(&texture->tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
	return texture;
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

	// Un-tile the icon.
	// NOTE: Allocating 64x32, because the "sf2d_create_texture_mem_RGBA8" (deprecated)
	// function hates small sizes like 32x32 (TWLoader freezes if that size is used).
	static const int w = 64;
	static const int h = 32;
	u16 *textureData = (u16*)linearAlloc(w*h*sizeof(u16));
	const u8 *offset = ndsBanner->icon;
	for (int y = 0; y < 32; y += 8) {
		for (int x = 0; x < 32; x += 8) {
			for (int y2 = 0; y2 < 8; y2++) {
				u16 *texptr = &textureData[x + ((y + y2) * 64)];
				for (int x2 = 0; x2 < 8; x2 += 2) {
					texptr[0] = palette[*offset & 0x0F];
					texptr[1] = palette[*offset >> 4];
					offset++;
					texptr += 2;
				}
			}
		}
	}

	// Create an RGB5A1 texture directly.
	ndsicon *texture = ndsicon_create_texture(w,h);
	
	//Tile the texture for the GPU.
	const u32 flags = (GX_TRANSFER_FLIP_VERT(1) | GX_TRANSFER_OUT_TILED(1) | GX_TRANSFER_RAW_COPY(0) |
		GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGB5A1) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB5A1) |
		GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO));

	GSPGPU_FlushDataCache(textureData, (w*h)*2);
	GSPGPU_FlushDataCache(texture->tex.data, texture->tex.size);

	C3D_SafeDisplayTransfer(
		(u32*)textureData,
		GX_BUFFER_DIM(w, h),
		(u32*)texture->tex.data,
		GX_BUFFER_DIM(texture->tex.width, texture->tex.height),
		flags
	);

	gspWaitForPPF();
	linearFree(textureData);
	
	return texture->tex.data;
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
