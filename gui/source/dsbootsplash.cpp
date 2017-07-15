#include "main.h"
#include "gamecard.h"
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
 * Fade from white to black.
 */
void fade_whiteToBlack() {
	if (settings.ui.bootscreen < 3) {
		int fade = 0;
		while (aptMainLoop()) {
			textVtxArrayPos = 0; // Clear the text vertex array

			fade += 9;
			if (fade > 255)
				fade = 255;
			
			for (int topfb = GFX_LEFT; topfb <= GFX_RIGHT; topfb++) {
				sf2d_start_frame(GFX_TOP, (gfx3dSide_t)topfb);
				drawRectangle(0, 0, 400, 240, RGBA8(255, 255, 255, 255));
				drawRectangle(0, 0, 400, 240, RGBA8(0, 0, 0, fade));
				if (settings.ui.bootscreen == 1) {
					drawRectangle(0, 0, 40, 240, RGBA8(0, 0, 0, 255)); // Left black bar
					drawRectangle(360, 0, 40, 240, RGBA8(0, 0, 0, 255)); // Right black bar
				}
				sf2d_end_frame();
			}
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			drawRectangle(0, 0, 320, 240, RGBA8(255, 255, 255, 255));
			drawRectangle(0, 0, 320, 240, RGBA8(0, 0, 0, fade));
			sf2d_end_frame();
			
			sf2d_swapbuffers();
			
			if (fade == 255) break;
		}
	}
}

/**
 * Play DS boot splash.
 */
void bootSplash() {
	sf2d_texture *ndslogotex;
	if (settings.ui.bootscreen >= 3) {
		if (settings.ui.language == 6 || settings.ui.language == 11)
			ndslogotex = sfil_load_PNG_file("romfs:/graphics/BootSplash/inv_iquelogo.png", SF2D_PLACE_RAM);
		else
			ndslogotex = sfil_load_PNG_file("romfs:/graphics/BootSplash/inv_ndslogo.png", SF2D_PLACE_RAM);
	} else {
		if (settings.ui.language == 6 || settings.ui.language == 11)
			ndslogotex = sfil_load_PNG_file("romfs:/graphics/BootSplash/iquelogo.png", SF2D_PLACE_RAM);
		else
			ndslogotex = sfil_load_PNG_file("romfs:/graphics/BootSplash/ndslogo.png", SF2D_PLACE_RAM);
	}
	sf2d_texture *itex;
	sf2d_texture *topotex;
	sf2d_texture *bottomotex;
	sf2d_texture *bigotex;
	sf2d_texture *nintendotex;
	if (settings.ui.bootscreen >= 3) {
		itex = sfil_load_PNG_file("romfs:/graphics/BootSplash/inv_i.png", SF2D_PLACE_RAM);
		topotex = sfil_load_PNG_file("romfs:/graphics/BootSplash/inv_topo.png", SF2D_PLACE_RAM);
		bottomotex = sfil_load_PNG_file("romfs:/graphics/BootSplash/bottomo.png", SF2D_PLACE_RAM);
		bigotex = sfil_load_PNG_file("romfs:/graphics/BootSplash/inv_bigo.png", SF2D_PLACE_RAM);
		nintendotex = sfil_load_PNG_file("romfs:/graphics/BootSplash/nintendo.png", SF2D_PLACE_RAM);
	} else {
		itex = sfil_load_PNG_file("romfs:/graphics/BootSplash/i.png", SF2D_PLACE_RAM);
		topotex = sfil_load_PNG_file("romfs:/graphics/BootSplash/topo.png", SF2D_PLACE_RAM);
		bottomotex = sfil_load_PNG_file("romfs:/graphics/BootSplash/bottomo.png", SF2D_PLACE_RAM);
		bigotex = sfil_load_PNG_file("romfs:/graphics/BootSplash/bigo.png", SF2D_PLACE_RAM);
		nintendotex = sfil_load_PNG_file("romfs:/graphics/BootSplash/nintendo.png", SF2D_PLACE_RAM);
	}
	sf2d_texture *hstexttex = NULL;
	sf2d_texture *hstouchtex = NULL;
	if (settings.ui.bootscreen >= 3) {
		if (settings.ui.language == 0) {
			hstexttex = sfil_load_PNG_file("romfs:/graphics/BootSplash/inv_HS_JP.png", SF2D_PLACE_RAM);
			hstouchtex = sfil_load_PNG_file("romfs:/graphics/BootSplash/inv_HStouch_JP.png", SF2D_PLACE_RAM);
		} else if (settings.ui.language == 6 || settings.ui.language == 11) {
			hstexttex = sfil_load_PNG_file("romfs:/graphics/BootSplash/inv_HS_CH.png", SF2D_PLACE_RAM);
			hstouchtex = sfil_load_PNG_file("romfs:/graphics/BootSplash/inv_HStouch_CH.png", SF2D_PLACE_RAM);
		} else if (settings.ui.language == 7) {
			hstexttex = sfil_load_PNG_file("romfs:/graphics/BootSplash/inv_HS_KO.png", SF2D_PLACE_RAM);
			hstouchtex = sfil_load_PNG_file("romfs:/graphics/BootSplash/inv_HStouch_KO.png", SF2D_PLACE_RAM);
		}
	} else {
		if (settings.ui.language == 0) {
			hstexttex = sfil_load_PNG_file("romfs:/graphics/BootSplash/HS_JP.png", SF2D_PLACE_RAM);
			hstouchtex = sfil_load_PNG_file("romfs:/graphics/BootSplash/HStouch_JP.png", SF2D_PLACE_RAM);
		} else if (settings.ui.language == 6 || settings.ui.language == 11) {
			hstexttex = sfil_load_PNG_file("romfs:/graphics/BootSplash/HS_CH.png", SF2D_PLACE_RAM);
			hstouchtex = sfil_load_PNG_file("romfs:/graphics/BootSplash/HStouch_CH.png", SF2D_PLACE_RAM);
		} else if (settings.ui.language == 7) {
			hstexttex = sfil_load_PNG_file("romfs:/graphics/BootSplash/HS_KO.png", SF2D_PLACE_RAM);
			hstouchtex = sfil_load_PNG_file("romfs:/graphics/BootSplash/HStouch_KO.png", SF2D_PLACE_RAM);
		}
	}
	sf2d_texture *hstex;
	sf2d_texture *wipetex;
	if (settings.ui.bootscreen >= 3) {
		hstex = sfil_load_PNG_file("romfs:/graphics/BootSplash/inv_hs.png", SF2D_PLACE_RAM);
		wipetex = sfil_load_PNG_file("romfs:/graphics/BootSplash/inv_wipe.png", SF2D_PLACE_RAM);
	} else {
		hstex = sfil_load_PNG_file("romfs:/graphics/BootSplash/hs.png", SF2D_PLACE_RAM);
		wipetex = sfil_load_PNG_file("romfs:/graphics/BootSplash/wipe.png", SF2D_PLACE_RAM);
	}
	
	// Load the sound effects if DSP is available.
	if (dspfirmfound) {
		if (settings.ui.bootscreen == 2 || settings.ui.bootscreen == 4)
			sfx_boot = new sound("romfs:/sounds/BootSplash/dsiboot.wav", 2, false);
		else
			sfx_boot = new sound("romfs:/sounds/BootSplash/dsboot.wav", 2, false);
	}
	
	bool bootsoundplayed = false;

	bool oeffects = false;
	int oeffectTimer = 0;
	
	float offset3D_oeffect1 = 0.0f;
	float offset3D_oeffect1_chng = 1.5;
	float offset3D_oeffect2 = 0.0f;
	float offset3D_oeffect2_chng = 2.0;
	float offset3D_oeffect3 = 0.0f;
	float offset3D_oeffect3_chng = 2.0;
	float offset3D_oeffect4 = 0.0f;
	float offset3D_oeffect4_chng = 2.0;
	float offset3D_oeffect5 = 0.0f;
	float offset3D_oeffect5_chng = 2.0;
	float offset3D_oeffect6 = 0.0f;
	float offset3D_oeffect6_chng = 2.0;
	float offset3D_oeffect7 = 0.0f;
	float offset3D_oeffect7_chng = 2.0;

	float oeffect1_xPos = 169.0;
	float oeffect1_o1_yPos = 50.0;
	float oeffect1_o2_yPos = 110.0;
	float oeffect2_xPos = 174.0;
	float oeffect2_yPos = 69.0;
	float oeffect3_xPos = 164.0;
	float oeffect3_yPos = 72.0;
	float oeffect4_xPos = 182.0;
	float oeffect4_yPos = 102.0;
	float oeffect5_xPos = 175.0;
	float oeffect5_yPos = 104.0;
	float oeffect6_xPos = 167.0;
	float oeffect6_yPos = 98.0;
	float oeffect7_xPos = 191.0;
	float oeffect7_yPos = 68.0;

	int oeffect1_alpha = 255;
	int oeffect2_alpha = 63;
	int oeffect3_alpha = 63;
	int oeffect4_alpha = 63;
	int oeffect5_alpha = 63;
	int oeffect6_alpha = 63;
	int oeffect7_alpha = 63;
	
	float oeffect1_scale = 0.35;
	float oeffect2_scale = 0.15;
	float oeffect3_scale = 0.15;
	float oeffect4_scale = 0.15;
	float oeffect5_scale = 0.20;
	float oeffect6_scale = 0.15;
	float oeffect7_scale = 0.15;
	
	float oeffect2_fadeout = false;
	float oeffect3_fadeout = false;
	float oeffect4_fadeout = false;
	float oeffect5_fadeout = false;
	float oeffect6_fadeout = false;
	float oeffect7_fadeout = false;

	float offset3D_ndslogo = 0.0f;
	float offset3D_i = 0.0f;
	float offset3D_nint = 0.0f;
	
	float offset3D_temp = 0.0f;
	
	int logopos = 40;
	if (settings.ui.bootscreen == 2 || settings.ui.bootscreen == 4) logopos = 20;		// Move the DS logo to the left to make room for the "i"
	if (settings.ui.language == 6 || settings.ui.language == 11) logopos -= 40;
	
	int wipePos = 0;
	int bootSplash_fade = 255;
	int i_alpha = 0;
	int touchtocontinue_yPos;
	if (settings.ui.language == 0) {
		if (settings.ui.healthsafety)
			touchtocontinue_yPos = 200;
		else
			touchtocontinue_yPos = 115;
	} else {
		if (settings.ui.healthsafety)
			touchtocontinue_yPos = 209;
		else
			touchtocontinue_yPos = 124;
	}
	bool touchtocontinue_show = false;
	int touchtocontinue_alpha = 0;
	bool touchtocontinue_fadein = true;
	
	int topoPos = 12;
	int bottomoPos = 167;
	
	bool fadeout = false;
	
	int splashScreenTime = 0;
	
	int bg_col;
	int text_col;
	
	if (settings.ui.bootscreen >= 3) {
		bg_col = 0;
	} else {
		bg_col = 255;
	}
	
	if (settings.ui.bootscreen >= 3) {
		text_col = 255;
	} else {
		text_col = 0;
	}

	for (int i = 0; i < 75; i++) {
		for (int topfb = GFX_LEFT; topfb <= GFX_RIGHT; topfb++) {
			sf2d_start_frame(GFX_TOP, (gfx3dSide_t)topfb);
			drawRectangle(0, 0, 400, 240, RGBA8(bg_col, bg_col, bg_col, 255));
			if (settings.ui.bootscreen == 1) {
				drawRectangle(0, 0, 40, 240, RGBA8(0, 0, 0, 255)); // Left black bar
				drawRectangle(360, 0, 40, 240, RGBA8(0, 0, 0, 255)); // Right black bar
			}
			sf2d_end_frame();
		}
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		drawRectangle(0, 0, 320, 240, RGBA8(bg_col, bg_col, bg_col, 255));
		sf2d_end_frame();

		sf2d_swapbuffers();
	}

	while (aptMainLoop()) {
		// Scan hid shared memory for input events
		hidScanInput();
		const u32 hDown = hidKeysDown();
		
		textVtxArrayPos = 0; // Clear the text vertex array
		
		offset3D_oeffect1 = CONFIG_3D_SLIDERSTATE * offset3D_oeffect1_chng;
		offset3D_oeffect2 = CONFIG_3D_SLIDERSTATE * offset3D_oeffect2_chng;
		offset3D_oeffect3 = CONFIG_3D_SLIDERSTATE * offset3D_oeffect3_chng;
		offset3D_oeffect4 = CONFIG_3D_SLIDERSTATE * offset3D_oeffect4_chng;
		offset3D_oeffect5 = CONFIG_3D_SLIDERSTATE * offset3D_oeffect5_chng;
		offset3D_oeffect6 = CONFIG_3D_SLIDERSTATE * offset3D_oeffect6_chng;
		offset3D_oeffect7 = CONFIG_3D_SLIDERSTATE * offset3D_oeffect7_chng;

		offset3D_ndslogo = CONFIG_3D_SLIDERSTATE * 2.0f;		
		offset3D_i = CONFIG_3D_SLIDERSTATE * 1.8f;		
		offset3D_nint = CONFIG_3D_SLIDERSTATE * 3.0f;
		
		if (wipePos < 800) wipePos += 20;
		if (!fadeout) {
			if (bootSplash_fade > 0) bootSplash_fade -= 9;
			else bootSplash_fade = 0;
		} else {
			bootSplash_fade += 9;
			if (bootSplash_fade > 255) bootSplash_fade = 255;
		}
		
		if (topoPos < 80) topoPos += 2;
		else {
			topoPos = 80;
			if (!bootsoundplayed && !fadeout) {
				if(dspfirmfound) sfx_boot->play();
				bootsoundplayed = true;
			}
			oeffects = true;
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
		
		if (oeffects) oeffectTimer += 1;

		for (int topfb = GFX_LEFT; topfb <= GFX_RIGHT; topfb++) {
			sf2d_start_frame(GFX_TOP, (gfx3dSide_t)topfb);
			drawRectangle(0, 0, 400, 240, RGBA8(bg_col, bg_col, bg_col, 255));
			if (topfb == 1) offset3D_temp = offset3D_ndslogo;
			else offset3D_temp = -offset3D_ndslogo;
			sf2d_draw_texture(ndslogotex, offset3D_temp+logopos+21, 78);
			sf2d_draw_texture(topotex, offset3D_temp+logopos+179, topoPos);
			sf2d_draw_texture(bottomotex, offset3D_temp+logopos+179, bottomoPos);
			if (oeffects) {
				if (topfb == 1) offset3D_temp = offset3D_oeffect1;
				else offset3D_temp = -offset3D_oeffect1;
				sf2d_draw_texture_scale_blend(bigotex, offset3D_temp+logopos+oeffect1_xPos, oeffect1_o1_yPos, oeffect1_scale, oeffect1_scale, RGBA8(255, 255, 255, oeffect1_alpha));
				sf2d_draw_texture_scale_blend(bigotex, offset3D_temp+logopos+oeffect1_xPos, oeffect1_o2_yPos, oeffect1_scale, oeffect1_scale, RGBA8(255, 255, 255, oeffect1_alpha));
				offset3D_oeffect1_chng -= 0.04;
				oeffect1_xPos -= 0.58;
				oeffect1_o1_yPos -= 0.70;
				oeffect1_o2_yPos -= 0.12;
				oeffect1_scale += 0.01;
				oeffect1_alpha -= 5;
				if (oeffect1_alpha < 0) oeffect1_alpha = 0;
			}
			if (gamecardIsInserted()) {
				if (topfb == 1) offset3D_temp = offset3D_nint;
				else offset3D_temp = -offset3D_nint;
				sf2d_draw_texture(nintendotex, offset3D_temp+40+84, 177);
			}
			if ((settings.ui.bootscreen == 2 && splashScreenTime > 60*1) ||
			    (settings.ui.bootscreen == 4 && splashScreenTime > 60*1)) {
				if (topfb == 1) offset3D_temp = offset3D_i;
				else offset3D_temp = -offset3D_i;
				i_alpha += 10;
				if (i_alpha > 255) i_alpha = 255;
				sf2d_draw_texture_blend(itex, offset3D_temp+logopos+312, 70, RGBA8(255, 255, 255, i_alpha));
			}
			if (oeffects) {
				if (oeffectTimer >= 10) {
					if (topfb == 1) offset3D_temp = offset3D_oeffect2;
					else offset3D_temp = -offset3D_oeffect2;
					sf2d_draw_texture_scale_blend(bigotex, offset3D_temp+logopos+oeffect2_xPos, oeffect2_yPos, oeffect2_scale, oeffect2_scale, RGBA8(255, 255, 255, oeffect2_alpha));
					offset3D_oeffect2_chng += 0.03;
					oeffect2_xPos -= 0.50;
					oeffect2_yPos -= 0.65;
					oeffect2_scale += 0.002;
					if (oeffect2_fadeout) {
						oeffect2_alpha -= 3;
						if (oeffect2_alpha < 0) oeffect2_alpha = 0;
					} else {
						oeffect2_alpha += 3;
						if (oeffect2_alpha > 146) oeffect2_fadeout = true;
					}
				}
				if (oeffectTimer >= 13) {
					if (topfb == 1) offset3D_temp = offset3D_oeffect3;
					else offset3D_temp = -offset3D_oeffect3;
					sf2d_draw_texture_scale_blend(bigotex, offset3D_temp+logopos+oeffect3_xPos, oeffect3_yPos, oeffect3_scale, oeffect3_scale, RGBA8(255, 255, 255, oeffect3_alpha));
					offset3D_oeffect3_chng += 0.04;
					oeffect3_xPos -= 0.76;
					oeffect3_yPos -= 0.43;
					oeffect3_scale += 0.003;
					if (oeffect3_fadeout) {
						oeffect3_alpha -= 3;
						if (oeffect3_alpha < 0) oeffect3_alpha = 0;
					} else {
						oeffect3_alpha += 3;
						if (oeffect3_alpha > 146) oeffect3_fadeout = true;
					}
				}
				if (oeffectTimer >= 16) {
					if (topfb == 1) offset3D_temp = offset3D_oeffect4;
					else offset3D_temp = -offset3D_oeffect4;
					sf2d_draw_texture_scale_blend(bigotex, offset3D_temp+logopos+oeffect4_xPos, oeffect4_yPos, oeffect4_scale, oeffect4_scale, RGBA8(255, 255, 255, oeffect4_alpha));
					offset3D_oeffect4_chng += 0.03;
					oeffect4_xPos += 0.40;
					oeffect4_yPos += 0.18;
					oeffect4_scale += 0.005;
					if (oeffect4_fadeout) {
						oeffect4_alpha -= 3;
						if (oeffect4_alpha < 0) oeffect4_alpha = 0;
					} else {
						oeffect4_alpha += 3;
						if (oeffect4_alpha > 146) oeffect4_fadeout = true;
					}
				}
				if (oeffectTimer >= 21) {
					if (topfb == 1) offset3D_temp = offset3D_oeffect5;
					else offset3D_temp = -offset3D_oeffect5;
					sf2d_draw_texture_scale_blend(bigotex, offset3D_temp+logopos+oeffect5_xPos, oeffect5_yPos, oeffect5_scale, oeffect5_scale, RGBA8(255, 255, 255, oeffect5_alpha));
					offset3D_oeffect5_chng += 0.04;
					oeffect5_xPos -= 0.31;
					oeffect5_yPos += 0.47;
					oeffect5_scale += 0.003;
					if (oeffect5_fadeout) {
						oeffect5_alpha -= 3;
						if (oeffect5_alpha < 0) oeffect5_alpha = 0;
					} else {
						oeffect5_alpha += 3;
						if (oeffect5_alpha > 146) oeffect5_fadeout = true;
					}
				}
				if (oeffectTimer >= 36) {
					if (topfb == 1) offset3D_temp = offset3D_oeffect6;
					else offset3D_temp = -offset3D_oeffect6;
					sf2d_draw_texture_scale_blend(bigotex, offset3D_temp+logopos+oeffect6_xPos, oeffect6_yPos, oeffect6_scale, oeffect6_scale, RGBA8(255, 255, 255, oeffect6_alpha));
					offset3D_oeffect6_chng += 0.05;
					oeffect6_xPos -= 0.80;
					oeffect6_yPos += 0.14;
					oeffect6_scale += 0.003;
					if (oeffect6_fadeout) {
						oeffect6_alpha -= 3;
						if (oeffect6_alpha < 0) oeffect6_alpha = 0;
					} else {
						oeffect6_alpha += 3;
						if (oeffect6_alpha > 146) oeffect6_fadeout = true;
					}
				}
				if (oeffectTimer >= 53) {
					if (topfb == 1) offset3D_temp = offset3D_oeffect7;
					else offset3D_temp = -offset3D_oeffect7;
					sf2d_draw_texture_scale_blend(bigotex, offset3D_temp+logopos+oeffect7_xPos, oeffect7_yPos, oeffect7_scale, oeffect7_scale, RGBA8(255, 255, 255, oeffect7_alpha));
					offset3D_oeffect7_chng += 0.04;
					oeffect7_xPos += 0.48;
					oeffect7_yPos -= 0.37;
					oeffect7_scale += 0.002;
					if (oeffect7_fadeout) {
						oeffect7_alpha -= 3;
						if (oeffect7_alpha < 0) oeffect7_alpha = 0;
					} else {
						oeffect7_alpha += 3;
						if (oeffect7_alpha > 146) oeffect7_fadeout = true;
					}
				}
			}
			sf2d_draw_texture(wipetex, wipePos-320, 0);
			drawRectangle(wipePos, 0, 400, 240, RGBA8(bg_col, bg_col, bg_col, 255));
			if (fadeout) drawRectangle(0, 0, 400, 240, RGBA8(bg_col, bg_col, bg_col, bootSplash_fade));
			if (settings.ui.bootscreen == 1) {
				drawRectangle(0, 0, 40, 240, RGBA8(0, 0, 0, 255)); // Left black bar
				drawRectangle(360, 0, 40, 240, RGBA8(0, 0, 0, 255)); // Right black bar
			}
			sf2d_end_frame();
			sf2d_swapbuffers();
		}
		
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		drawRectangle(0, 0, 320, 240, RGBA8(bg_col, bg_col, bg_col, 255));

		switch ((settings.ui.language < 0 || settings.ui.language >= 12)? sys_language : settings.ui.language) {
			case 0:
			// Japanese
				if (settings.ui.healthsafety) {
					sf2d_draw_texture(hstex, 55, 14);
					sf2d_draw_texture(hstexttex, 0, 0);
					setTextColor(RGBA8(61, 161, 191, 255));
					renderText(64, 176, 0.55, 0.55, false, "www.nintendo.co.jp/anzen/");
				}
				if (touchtocontinue_show) {
					sf2d_draw_texture_blend(hstouchtex, 0, touchtocontinue_yPos, RGBA8(255, 255, 255, touchtocontinue_alpha));
				}
				break;
			case 1:
			default:
			// English
				if (settings.ui.healthsafety) {
					sf2d_draw_texture(hstex, 12, 16);
					setTextColor(RGBA8(text_col, text_col, text_col, 255));
					renderText_w(34, 19, 0.65, 0.65, false, TR(STR_DSSPLASH_WARNING_HS));
					renderText_w(29, 56, 0.55, 0.55, false, TR(STR_DSSPLASH_WARNING_HS1));
					renderText_w(29, 80, 0.55, 0.55, false, TR(STR_DSSPLASH_WARNING_HS2));
					renderText_w(47, 104, 0.55, 0.55, false, TR(STR_DSSPLASH_WARNING_HS3));
					renderText_w(35, 128, 0.55, 0.55, false, TR(STR_DSSPLASH_WARNING_HS4));
					renderText_w(16, 160, 0.38, 0.50, false, TR(STR_DSSPLASH_WARNING_HS5));
					setTextColor(RGBA8(61, 161, 191, 255));
					renderText_w(50, 178, 0.55, 0.55, false, TR(STR_DSSPLASH_WARNING_HS6));
				}
				if (touchtocontinue_show) {
					setTextColor(RGBA8(text_col, text_col, text_col, touchtocontinue_alpha));
					renderText_w(32, touchtocontinue_yPos, 0.55, 0.55, false, TR(STR_DSSPLASH_TOUCH));
				}
				break;
			case 2:
			// French
				if (settings.ui.healthsafety) {
					sf2d_draw_texture(hstex, 12, 16);
					setTextColor(RGBA8(text_col, text_col, text_col, 255));
					renderText_w(34, 19, 0.62, 0.62, false, TR(STR_DSSPLASH_WARNING_HS));
					renderText_w(56, 56, 0.45, 0.47, false, TR(STR_DSSPLASH_WARNING_HS1));
					renderText_w(32, 79, 0.45, 0.47, false, TR(STR_DSSPLASH_WARNING_HS2));
					renderText_w(25, 102, 0.45, 0.47, false, TR(STR_DSSPLASH_WARNING_HS3));
					renderText_w(37, 126, 0.38, 0.50, false, TR(STR_DSSPLASH_WARNING_HS4));
					renderText_w(72, 143, 0.38, 0.50, false, TR(STR_DSSPLASH_WARNING_HS5));
					renderText_w(52, 160, 0.38, 0.50, false, TR(STR_DSSPLASH_WARNING_HS6));
					setTextColor(RGBA8(61, 161, 191, 255));
					renderText_w(50, 178, 0.55, 0.55, false, TR(STR_DSSPLASH_WARNING_HS7));
				}
				if (touchtocontinue_show) {
					setTextColor(RGBA8(text_col, text_col, text_col, touchtocontinue_alpha));
					renderText_w(32, touchtocontinue_yPos, 0.55, 0.55, false, TR(STR_DSSPLASH_TOUCH));
				}
				break;
			case 3:
			// German
				if (settings.ui.healthsafety) {
					sf2d_draw_texture(hstex, 12, 16);
					setTextColor(RGBA8(text_col, text_col, text_col, 255));
					renderText_w(35, 21, 0.50, 0.55, false, TR(STR_DSSPLASH_WARNING_HS));
					renderText_w(32, 55, 0.40, 0.40, false, TR(STR_DSSPLASH_WARNING_HS1));
					renderText_w(44, 75, 0.40, 0.40, false, TR(STR_DSSPLASH_WARNING_HS2));
					renderText_w(42, 95, 0.40, 0.40, false, TR(STR_DSSPLASH_WARNING_HS3));
					renderText_w(58, 115, 0.40, 0.40, false, TR(STR_DSSPLASH_WARNING_HS4));
					renderText_w(28, 147, 0.38, 0.45, false, TR(STR_DSSPLASH_WARNING_HS5));
					renderText_w(32, 161, 0.38, 0.45, false, TR(STR_DSSPLASH_WARNING_HS6));
					setTextColor(RGBA8(61, 161, 191, 255));
					renderText_w(50, 178, 0.55, 0.55, false, TR(STR_DSSPLASH_WARNING_HS7));
				}
				if (touchtocontinue_show) {
					setTextColor(RGBA8(text_col, text_col, text_col, touchtocontinue_alpha));
					renderText_w(14, touchtocontinue_yPos, 0.55, 0.55, false, TR(STR_DSSPLASH_TOUCH));
				}
				break;
			case 4:
			// Italian
				if (settings.ui.healthsafety) {
					sf2d_draw_texture(hstex, 12, 16);
					setTextColor(RGBA8(text_col, text_col, text_col, 255));
					renderText_w(34, 19, 0.60, 0.60, false, TR(STR_DSSPLASH_WARNING_HS));
					renderText_w(22, 55, 0.40, 0.40, false, TR(STR_DSSPLASH_WARNING_HS1));
					renderText_w(30, 75, 0.40, 0.40, false, TR(STR_DSSPLASH_WARNING_HS2));
					renderText_w(24, 95, 0.40, 0.40, false, TR(STR_DSSPLASH_WARNING_HS3));
					renderText_w(70, 115, 0.40, 0.40, false, TR(STR_DSSPLASH_WARNING_HS4));
					renderText_w(21, 147, 0.38, 0.45, false, TR(STR_DSSPLASH_WARNING_HS5));
					renderText_w(49, 161, 0.38, 0.45, false, TR(STR_DSSPLASH_WARNING_HS6));
					setTextColor(RGBA8(61, 161, 191, 255));
					renderText_w(50, 178, 0.55, 0.55, false, TR(STR_DSSPLASH_WARNING_HS7));
				}
				if (touchtocontinue_show) {
					setTextColor(RGBA8(text_col, text_col, text_col, touchtocontinue_alpha));
					renderText_w(34, touchtocontinue_yPos, 0.55, 0.55, false, TR(STR_DSSPLASH_TOUCH));
				}
				break;
			case 5:
			// Spanish
				if (settings.ui.healthsafety) {
					sf2d_draw_texture(hstex, 12, 16);
					setTextColor(RGBA8(text_col, text_col, text_col, 255));
					renderText_w(34, 21, 0.50, 0.55, false, TR(STR_DSSPLASH_WARNING_HS));
					renderText_w(22, 55, 0.40, 0.40, false, TR(STR_DSSPLASH_WARNING_HS1));
					renderText_w(15, 75, 0.40, 0.40, false, TR(STR_DSSPLASH_WARNING_HS2));
					renderText_w(24, 95, 0.40, 0.40, false, TR(STR_DSSPLASH_WARNING_HS3));
					renderText_w(78, 115, 0.40, 0.40, false, TR(STR_DSSPLASH_WARNING_HS4));
					renderText_w(18, 147, 0.38, 0.45, false, TR(STR_DSSPLASH_WARNING_HS5));
					renderText_w(60, 161, 0.38, 0.45, false, TR(STR_DSSPLASH_WARNING_HS6));
					setTextColor(RGBA8(61, 161, 191, 255));
					renderText_w(50, 178, 0.55, 0.55, false, TR(STR_DSSPLASH_WARNING_HS7));
				}
				if (touchtocontinue_show) {
					setTextColor(RGBA8(text_col, text_col, text_col, touchtocontinue_alpha));
					renderText_w(34, touchtocontinue_yPos, 0.55, 0.55, false, TR(STR_DSSPLASH_TOUCH));
				}
				break;
			case 6:
			case 11:
			// Chinese
				if (settings.ui.healthsafety) {
					sf2d_draw_texture(hstex, 49, 12);
					sf2d_draw_texture(hstexttex, 0, 0);
					setTextColor(RGBA8(61, 161, 191, 255));
					renderText(81, 166, 0.55, 0.55, false, "www.iQue.com/service/");
				}
				if (touchtocontinue_show) {
					sf2d_draw_texture_blend(hstouchtex, 0, touchtocontinue_yPos, RGBA8(255, 255, 255, touchtocontinue_alpha));
				}
				break;
			case 7:
			// Korean
				if (settings.ui.healthsafety) {
					sf2d_draw_texture(hstex, 29, 10);
					sf2d_draw_texture(hstexttex, 0, 0);
					setTextColor(RGBA8(61, 161, 191, 255));
					renderText(66, 176, 0.55, 0.55, false, "www.nintendocaution.co.kr");
				}
				if (touchtocontinue_show) {
					sf2d_draw_texture_blend(hstouchtex, 0, touchtocontinue_yPos, RGBA8(255, 255, 255, touchtocontinue_alpha));
				}
				break;
		}
		drawRectangle(0, 0, 320, 240, RGBA8(bg_col, bg_col, bg_col, bootSplash_fade));
		sf2d_end_frame();
		
		splashScreenTime += 1;
		if (splashScreenTime > 60*2) touchtocontinue_show = true;
		sf2d_swapbuffers();

		if (!fadeout && touchtocontinue_show) {
			if ((hDown & (KEY_L | KEY_R
				| KEY_A | KEY_B | KEY_X | KEY_Y
				| KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT
				| KEY_START | KEY_SELECT | KEY_TOUCH)) || splashScreenTime == 60*60)
			{
				if(dspfirmfound) {
					sfx_boot->stop();
					sfx_select->play();
				}
				fadeout = true;
			}
		}
		
		if (fadeout && bootSplash_fade == 255) break;
		
	}
	
	if (logEnabled)	LogFM("BootSplash", "Unloading jingle sound");
	delete sfx_boot;
	if (logEnabled)	LogFM("BootSplash", "Unloaded jingle sound");

	if (logEnabled)	LogFM("BootSplash", "Freeing textures");
	sf2d_free_texture(ndslogotex);
	sf2d_free_texture(itex);
	sf2d_free_texture(topotex);
	sf2d_free_texture(bottomotex);
	sf2d_free_texture(bigotex);
	sf2d_free_texture(nintendotex);
	if (settings.ui.language == 0 || settings.ui.language == 6 || settings.ui.language == 7 || settings.ui.language == 11) {
		sf2d_free_texture(hstexttex);
		sf2d_free_texture(hstouchtex);
	}
	sf2d_free_texture(hstex);
	sf2d_free_texture(wipetex);
	if (logEnabled)	LogFM("BootSplash", "Textures freed up");
}
