#include "main.h"
#include "language.h"
#include "log.h"
#include "settings.h"

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
#include <3ds/types.h>
#include <sf2d.h>
#include <sfil.h>
#include "citrostuff.h"

#define CONFIG_3D_SLIDERSTATE (*(float *)0x1FF81080)

sound *sfx_boot;

/**
 * Play DS boot splash.
 */
void bootSplash() {
	sf2d_texture *ndslogotex = sfil_load_PNG_file("romfs:/graphics/BootSplash/ndslogo.png", SF2D_PLACE_RAM);
	sf2d_texture *topotex = sfil_load_PNG_file("romfs:/graphics/BootSplash/topo.png", SF2D_PLACE_RAM);
	sf2d_texture *bottomotex = sfil_load_PNG_file("romfs:/graphics/BootSplash/bottomo.png", SF2D_PLACE_RAM);
	sf2d_texture *nintendotex = sfil_load_PNG_file("romfs:/graphics/BootSplash/nintendo.png", SF2D_PLACE_RAM);
	sf2d_texture *hstex = sfil_load_PNG_file("romfs:/graphics/BootSplash/hs.png", SF2D_PLACE_RAM);
	sf2d_texture *wipetex = sfil_load_PNG_file("romfs:/graphics/BootSplash/wipe.png", SF2D_PLACE_RAM);
	
	// Load the sound effects if DSP is available.
	if (dspfirmfound) {
		sfx_boot = new sound("romfs:/sounds/BootSplash/dsboot.wav", 2, false);
	}
	
	bool bootsoundplayed = false;

	float offset3D_nint = 0.0f;
	
	float offset3D_temp = 0.0f;
	
	int wipePos = 0;
	int bootSplash_fade = 255;
	int touchtocontinue_alpha = 0;
	bool touchtocontinue_fadein = true;
	
	int topoPos = 12;
	int bottomoPos = 167;
	
	bool fadeout = false;
	
	int splashScreenTime = 0;
	
	for (int i = 0; i < 75; i++) {
		for (int topfb = GFX_LEFT; topfb <= GFX_RIGHT; topfb++) {
			sf2d_start_frame(GFX_TOP, (gfx3dSide_t)topfb);
			drawRectangle(0, 0, 400, 240, RGBA8(255, 255, 255, 255));
			sf2d_end_frame();
		}
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		drawRectangle(0, 0, 320, 240, RGBA8(255, 255, 255, 255));
		sf2d_end_frame();

		sf2d_swapbuffers();
	}

	while (1) {
		// Scan hid shared memory for input events
		hidScanInput();

		const u32 hDown = hidKeysDown();
		const u32 hHeld = hidKeysHeld();

		textVtxArrayPos = 0; // Clear the text vertex array
		
		offset3D_nint = CONFIG_3D_SLIDERSTATE * 2.0f;
		
		if (wipePos < 800) wipePos += 20;
		if (!fadeout) {
			if (bootSplash_fade > 0) bootSplash_fade -= 9;
			else bootSplash_fade = 0;
		} else {
			if (bootSplash_fade < 255) bootSplash_fade += 9;
			else bootSplash_fade = 255;
		}
		
		if (topoPos < 80) topoPos += 2;
		else {
			topoPos = 80;
			if (!bootsoundplayed && dspfirmfound) {
				sfx_boot->play();
				bootsoundplayed = true;
			}
		}
		if (bottomoPos > 101) bottomoPos -= 2;
		else bottomoPos = 101;
		
		if (touchtocontinue_fadein) {
			touchtocontinue_alpha += 9;
			if (touchtocontinue_alpha > 255) {
				touchtocontinue_alpha = 255;
				touchtocontinue_fadein = false;
			}
		} else {
			touchtocontinue_alpha -= 9;
			if (touchtocontinue_alpha < 0) {
				touchtocontinue_alpha = 0;
				touchtocontinue_fadein = true;
			}
		}

		for (int topfb = GFX_LEFT; topfb <= GFX_RIGHT; topfb++) {
			sf2d_start_frame(GFX_TOP, (gfx3dSide_t)topfb);
			drawRectangle(0, 0, 400, 240, RGBA8(255, 255, 255, 255));
			sf2d_draw_texture(ndslogotex, 40+21, 78);
			sf2d_draw_texture(topotex, 40+179, topoPos);
			sf2d_draw_texture(bottomotex, 40+179, bottomoPos);
			if (topfb == 1) offset3D_temp = offset3D_nint;
			else offset3D_temp = -offset3D_nint;
			sf2d_draw_texture(nintendotex, offset3D_temp+40+84, 177);
			sf2d_draw_texture(wipetex, wipePos-320, 0);
			drawRectangle(wipePos, 0, 400, 240, RGBA8(255, 255, 255, 255));
			if (fadeout) drawRectangle(0, 0, 400, 240, RGBA8(255, 255, 255, bootSplash_fade));
			sf2d_end_frame();
		}
		
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		drawRectangle(0, 0, 320, 240, RGBA8(255, 255, 255, 255));
		sf2d_draw_texture(hstex, 12, 16);
		setTextColor(RGBA8(0, 0, 0, 255));
		renderText(34, 19, 0.65, 0.65, false, "WARNING - HEALTH AND SAFETY");
		renderText(29, 56, 0.55, 0.55, false, "BEFORE PLAYING, READ THE HEALTH");
		renderText(29, 80, 0.55, 0.55, false, "AND SAFETY PRECAUTIONS BOOKLET");
		renderText(47, 104, 0.55, 0.55, false, "FOR IMPORTANT INFORMATION");
		renderText(35, 128, 0.55, 0.55, false, "ABOUT YOUR HEALTH AND SAFETY.");
		renderText(16, 160, 0.38, 0.50, false, "TO GET AN EXTRA COPY FOR YOUR REGION, GO ONLINE AT");
		setTextColor(RGBA8(61, 161, 191, 255));
		renderText(36, 178, 0.55, 0.55, false, "www.nintendo.com/healthandsafety/");
		setTextColor(RGBA8(0, 0, 0, touchtocontinue_alpha));
		renderText(32, 209, 0.55, 0.55, false, "Touch the Touch Screen to continue.");
		drawRectangle(0, 0, 320, 240, RGBA8(255, 255, 255, bootSplash_fade));
		sf2d_end_frame();
		
		splashScreenTime += 1;
		sf2d_swapbuffers();

		if (!fadeout) {
			if ((hDown & (KEY_L | KEY_R
				| KEY_A | KEY_B | KEY_X | KEY_Y
				| KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT
				| KEY_START | KEY_SELECT | KEY_TOUCH)) || splashScreenTime == 60*60)
			{
				sfx_boot->stop();
				sfx_select->play();
				fadeout = true;
			}
		}
		
		if (fadeout && bootSplash_fade == 255) break;
		
	}
	
	delete sfx_boot;

	sf2d_free_texture(ndslogotex);
	sf2d_free_texture(topotex);
	sf2d_free_texture(bottomotex);
	sf2d_free_texture(nintendotex);
	sf2d_free_texture(hstex);
	sf2d_free_texture(wipetex);

}