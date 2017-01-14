#include "ndsheaderbanner.h"
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>

#include <string>
#include <vector>
using std::string;
using std::vector;

#include "log.h"

/**
 * Convert a color from BGR555 to RGBA8.
 * @param px16 BGR555 color value.
 * @return RGBA8 color.
 */
static inline u32 BGR555_to_RGBA8(u32 px16) {
	u32 px32;

	// BGR555: xBBBBBGG GGGRRRRR
	//  RGBA8: AAAAAAAA BBBBBBBB GGGGGGGG RRRRRRRR
	// TODO: verify this
	px32 = ((((px16 << 3) & 0x0000F8) | ((px16 >> 2) & 0x000007))) |	// Red
	       ((((px16 << 6) & 0x00F800) | ((px16 << 1) & 0x000700))) |	// Green
	       ((((px16 << 9) & 0xF80000) | ((px16 << 4) & 0x070000)));		// Blue

	// No alpha channel.
	px32 |= 0xFF000000U;
	return px32;
}

/**
 * Get the title ID.
 * @param ndsFile DS ROM image.
 * @param buf Output buffer for title ID. (Must be at least 4 characters.
 */
void grabTID(FILE* ndsFile, char *buf) {
	fseek(ndsFile, offsetof(sNDSHeadertitlecodeonly, gameCode), SEEK_SET);
	fread(buf, 1, 4, ndsFile);
}

/**
 * Get text from a cached banner file.
 * @param binFile Banner file.
 * @param bnrtitlenum Title number. (aka language)
 * @return Vector containing each line as a wide string.
 */
vector<string> grabText(FILE* binFile, int bnrtitlenum) {
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

	// Load the banner.
	// NOTE: myBanner is cleared first in case the banner
	// file is smaller than NDS_BANNER_SIZE_DSi.
	sNDSBanner myBanner;
	memset(&myBanner, 0, sizeof(myBanner));
	fseek(binFile, 0, SEEK_SET);
	fread(&myBanner, 1, sizeof(myBanner), binFile);

	// Check the banner version.
	switch (myBanner.version) {
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
			vector<string> vec_str;
			vec_str.push_back("No Info");
			return vec_str;
	}

	// UTF-16 text from the banner.
	const u16 *u16text = myBanner.titles[bnrtitlenum];

	// Buffers for the strings.
	// TODO: Return vector<wstring> for Unicode.
	vector<string> vec_str;
	vec_str.reserve(3);
	string str;
	str.reserve(64);

	for (int i = sizeof(myBanner.titles[bnrtitlenum])/sizeof(myBanner.titles[bnrtitlenum][0]);
	     *u16text != 0 && i > 0; i--, u16text++)
	{
		// Quick and dirty Unicode to "ASCII" conversion.
		// TODO: Proper UTF-16 to UTF-32 conversion.
		switch (*u16text) {
			case L'\r':
				// Skip carriage returns.
				break;
			case L'\n':
				// Newline.
				vec_str.push_back(str);
				str.clear();
				break;
			default:
				// Add the character.
				str += (char)(*u16text & 0xFF);
				break;
		}
	}

	// Add the last line if it's not empty.
	if (!str.empty()) {
		vec_str.push_back(str);
	}

	return vec_str;
}

void cacheBanner(FILE* ndsFile, const char* filename, sftd_font* setfont) {
	char bannerpath[256];
	snprintf(bannerpath, sizeof(bannerpath), "sdmc:/_nds/twloader/bnricons/%s.bin", filename);

	if (!access(bannerpath, F_OK)) {
		// Banner is already cached.
		sftd_draw_textf(setfont, 12, 32, RGBA8(0, 0, 0, 255), 12, "Banner data already exists.");
		sf2d_end_frame();
		sf2d_swapbuffers();
		return;
	}

	// Cache the banner.
	FILE* filetosave = fopen(bannerpath, "wb");
	if (!filetosave) {
		// Error opening the banner cache file...
		sftd_draw_textf(setfont, 12, 32, RGBA8(0, 0, 0, 255), 12, "Error caching the banner.");
		sf2d_end_frame();
		sf2d_swapbuffers();
		return;
	}

	sNDSHeader NDSHeader;
	sNDSBanner myBanner;
	memset(&myBanner, 0, sizeof(myBanner));

	LogFMA("NDSBannerHeader.cacheBanner", "Reading .NDS file: ", filename);
	fseek(ndsFile, 0, SEEK_SET);
	fread(&NDSHeader, 1, sizeof(NDSHeader), ndsFile);
	LogFMA("NDSBannerHeader.cacheBanner", ".NDS file read:", filename);

	if (NDSHeader.bannerOffset != 0x00000000) {
		// Read the banner from the NDS file.
		fseek(ndsFile , NDSHeader.bannerOffset, SEEK_SET);
		fread(&myBanner, 1, sizeof(myBanner), ndsFile);

		sftd_draw_textf(setfont, 12, 32, RGBA8(0, 0, 0, 255), 12, "Now caching banner data (SD Card)...");
		sf2d_end_frame();
		sf2d_swapbuffers();
		LogFMA("NDSBannerHeader.cacheBanner", "Caching banner data:", bannerpath);

		u32 bannersize;
		switch (myBanner.version) {
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
		fwrite(&myBanner, 1, bannersize, filetosave);
		LogFMA("NDSBannerHeader.cacheBanner", "Banner data cached:", bannerpath);
	} else {
		// No banner. Use the generic version.
		FILE* nobannerFile = fopen("romfs:/notextbanner", "rb");
		sftd_draw_textf(setfont, 12, 32, RGBA8(0, 0, 0, 255), 12, "Now caching banner data (SD Card)...");
		sf2d_end_frame();
		sf2d_swapbuffers();
		LogFMA("NDSBannerHeader.cacheBanner", "Caching banner data (empty):", bannerpath);
		// notextbanner is v0003 (ZH/KO)
		fread(&myBanner, 1, NDS_BANNER_SIZE_ZH_KO, nobannerFile);
		fwrite(&myBanner, 1, NDS_BANNER_SIZE_ZH_KO, filetosave);
		LogFMA("NDSBannerHeader.cacheBanner", "Banner data cached (empty):", bannerpath);
		fclose(nobannerFile);
	}

	fclose(filetosave);
}

sf2d_texture* grabIcon(FILE* binFile) {
	sNDSBanner myBanner;
	fseek(binFile, 0, SEEK_SET);
	fread(&myBanner, 1, sizeof(myBanner), binFile);    

	// Convert the palette first.
	u32 palette[16];
	palette[0] = 0;	// Color 0 is always transparent.
	for (int i = 16; i > 0; i--) {
		palette[i] = BGR555_to_RGBA8(myBanner.palette[i]);
	}

	// Un-tile the icon.
	// FIXME: Why are we allocating 64x32?
	u32 *textureData = (u32*)linearAlloc(32*64*sizeof(u32));
	const u8 *offset = myBanner.icon;
	for (int y = 0; y < 32; y += 8) {
		for (int x = 0; x < 32; x += 8) {
			for (int y2 = 0; y2 < 8; y2++) {
				u32 *texptr = &textureData[x + ((y + y2) * 64)];
				for (int x2 = 0; x2 < 8; x2 += 2) {
					texptr[0] = palette[*offset & 0x0F];
					texptr[1] = palette[*offset >> 4];
					offset++;
					texptr += 2;
				}
			}
		}
	}

	sf2d_texture *tex = sf2d_create_texture_mem_RGBA8(textureData, 64, 32, TEXFMT_RGBA8, SF2D_PLACE_RAM);
	linearFree(textureData);
	return tex;
}
