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
#include "graphic.h"
#include "pp2d/pp2d.h"

#define CONFIG_3D_SLIDERSTATE (*(float *)0x1FF81080)

sound *sfx_boot;

/**
 * Fade from white to black.
 */
void fade_whiteToBlack() {
	if (settings.ui.bootscreen < 3) {
		int fade = 0;
		while (aptMainLoop()) {
			pp2d_begin_draw(GFX_TOP, GFX_LEFT);

			fade += 9;
			if (fade > 255)
				fade = 255;
			
			for (int topfb = GFX_LEFT; topfb <= GFX_RIGHT; topfb++) {
				pp2d_draw_on(GFX_TOP, (gfx3dSide_t)topfb);
				pp2d_draw_rectangle(0, 0, 400, 240, WHITE);
				pp2d_draw_rectangle(0, 0, 400, 240, RGBA8(0, 0, 0, fade));
				if (settings.ui.bootscreen == 1) {
					pp2d_draw_rectangle(0, 0, 40, 240, BLACK); // Left black bar
					pp2d_draw_rectangle(360, 0, 40, 240, BLACK); // Right black bar
				}
			}
			pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
			pp2d_draw_rectangle(0, 0, 320, 240, WHITE);
			pp2d_draw_rectangle(0, 0, 320, 240, RGBA8(0, 0, 0, fade));
			pp2d_end_draw();

			if (fade == 255) break;
		}
	}
	screenoff();
}

/**
 * Play DS boot splash.
 */
void bootSplash() {
	if (settings.ui.bootscreen >= 3) {
		if (settings.ui.language == 6 || settings.ui.language == 11)
			pp2d_load_texture_png(ndslogotex, "romfs:/graphics/BootSplash/inv_iquelogo.png");
		else
			pp2d_load_texture_png(ndslogotex, "romfs:/graphics/BootSplash/inv_ndslogo.png");
	} else {
		if (settings.ui.language == 6 || settings.ui.language == 11)
			pp2d_load_texture_png(ndslogotex, "romfs:/graphics/BootSplash/iquelogo.png");
		else
			pp2d_load_texture_png(ndslogotex, "romfs:/graphics/BootSplash/ndslogo.png");
	}

	if (settings.ui.bootscreen >= 3) {
		pp2d_load_texture_png(itex, "romfs:/graphics/BootSplash/inv_i.png");
		pp2d_load_texture_png(topotex, "romfs:/graphics/BootSplash/inv_topo.png");
		pp2d_load_texture_png(bottomotex, "romfs:/graphics/BootSplash/bottomo.png");
		pp2d_load_texture_png(bigotex, "romfs:/graphics/BootSplash/inv_bigo.png");
		pp2d_load_texture_png(nintendotex, "romfs:/graphics/BootSplash/nintendo.png");
	} else {
		pp2d_load_texture_png(itex, "romfs:/graphics/BootSplash/i.png");
		pp2d_load_texture_png(topotex, "romfs:/graphics/BootSplash/topo.png");
		pp2d_load_texture_png(bottomotex, "romfs:/graphics/BootSplash/bottomo.png");
		pp2d_load_texture_png(bigotex, "romfs:/graphics/BootSplash/bigo.png");
		pp2d_load_texture_png(nintendotex, "romfs:/graphics/BootSplash/nintendo.png");
	}
	if (settings.ui.bootscreen >= 3) {
		if (settings.ui.language == 0) {
			pp2d_load_texture_png(hstexttex, "romfs:/graphics/BootSplash/inv_HS_JP.png");
			pp2d_load_texture_png(hstouchtex, "romfs:/graphics/BootSplash/inv_HStouch_JP.png");
		} else if (settings.ui.language == 6 || settings.ui.language == 11) {
			pp2d_load_texture_png(hstexttex, "romfs:/graphics/BootSplash/inv_HS_CH.png");
			pp2d_load_texture_png(hstouchtex, "romfs:/graphics/BootSplash/inv_HStouch_CH.png");
		} else if (settings.ui.language == 7) {
			pp2d_load_texture_png(hstexttex, "romfs:/graphics/BootSplash/inv_HS_KO.png");
			pp2d_load_texture_png(hstouchtex, "romfs:/graphics/BootSplash/inv_HStouch_KO.png");
		}
	} else {
		if (settings.ui.language == 0) {
			pp2d_load_texture_png(hstexttex, "romfs:/graphics/BootSplash/HS_JP.png");
			pp2d_load_texture_png(hstouchtex, "romfs:/graphics/BootSplash/HStouch_JP.png");
		} else if (settings.ui.language == 6 || settings.ui.language == 11) {
			pp2d_load_texture_png(hstexttex, "romfs:/graphics/BootSplash/HS_CH.png");
			pp2d_load_texture_png(hstouchtex, "romfs:/graphics/BootSplash/HStouch_CH.png");
		} else if (settings.ui.language == 7) {
			pp2d_load_texture_png(hstexttex, "romfs:/graphics/BootSplash/HS_KO.png");
			pp2d_load_texture_png(hstouchtex, "romfs:/graphics/BootSplash/HStouch_KO.png");
		}
	}
	if (settings.ui.bootscreen >= 3) {
		pp2d_load_texture_png(hstex, "romfs:/graphics/BootSplash/inv_hs.png");
		pp2d_load_texture_png(wipetex, "romfs:/graphics/BootSplash/inv_wipe.png");
	} else {
		pp2d_load_texture_png(hstex, "romfs:/graphics/BootSplash/hs.png");
		pp2d_load_texture_png(wipetex, "romfs:/graphics/BootSplash/wipe.png");
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
	
	int text_width = 0;
	
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
	
	pp2d_set_3D(1);

	for (int i = 0; i < 75; i++) {
		pp2d_begin_draw(GFX_TOP, GFX_LEFT);
		for (int topfb = GFX_LEFT; topfb <= GFX_RIGHT; topfb++) {
			pp2d_draw_on(GFX_TOP, (gfx3dSide_t)topfb);
			pp2d_draw_rectangle(0, 0, 400, 240, RGBA8(bg_col, bg_col, bg_col, 255));
			if (settings.ui.bootscreen == 1) {
				pp2d_draw_rectangle(0, 0, 40, 240, RGBA8(0, 0, 0, 255)); // Left black bar
				pp2d_draw_rectangle(360, 0, 40, 240, RGBA8(0, 0, 0, 255)); // Right black bar
			}
		}
		pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
		pp2d_draw_rectangle(0, 0, 320, 240, RGBA8(bg_col, bg_col, bg_col, 255));
		pp2d_end_draw();
		screenon();
	}

	while (aptMainLoop()) {
		pp2d_begin_draw(GFX_TOP, GFX_LEFT);

		// Scan hid shared memory for input events
		hidScanInput();
		const u32 hDown = hidKeysDown();

		// Poll for Slot-1 changes.
		bool forcePoll = false;
		bool doSlot1Update = false;
		if (gamecardIsInserted() && gamecardGetType() == CARD_TYPE_UNKNOWN) {
			// Card is inserted, but we don't know its type.
			// Force an update.
			forcePoll = true;
		}
		bool s1chg = gamecardPoll(forcePoll);
		if (s1chg) {
			// Update Slot-1 if:
			// - forcePoll is false
			// - forcePoll is true, and card is no longer unknown.
			doSlot1Update = (!forcePoll || gamecardGetType() != CARD_TYPE_UNKNOWN);
		}

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
			pp2d_draw_on(GFX_TOP, (gfx3dSide_t)topfb);
			pp2d_draw_rectangle(0, 0, 400, 240, RGBA8(bg_col, bg_col, bg_col, 255));
			if (topfb == 1) offset3D_temp = offset3D_ndslogo;
			else offset3D_temp = -offset3D_ndslogo;
			pp2d_draw_texture(ndslogotex, offset3D_temp+logopos+21, 78);
			pp2d_draw_texture(topotex, offset3D_temp+logopos+179, topoPos);
			pp2d_draw_texture(bottomotex, offset3D_temp+logopos+179, bottomoPos);
			if (oeffects) {
				if (topfb == 1) offset3D_temp = offset3D_oeffect1;
				else offset3D_temp = -offset3D_oeffect1;
				pp2d_draw_texture_scale_blend(bigotex, offset3D_temp+logopos+oeffect1_xPos, oeffect1_o1_yPos, oeffect1_scale, oeffect1_scale, RGBA8(255, 255, 255, oeffect1_alpha));
				pp2d_draw_texture_scale_blend(bigotex, offset3D_temp+logopos+oeffect1_xPos, oeffect1_o2_yPos, oeffect1_scale, oeffect1_scale, RGBA8(255, 255, 255, oeffect1_alpha));
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
				pp2d_draw_texture(nintendotex, offset3D_temp+40+84, 177);
			}
			if ((settings.ui.bootscreen == 2 && splashScreenTime > 60*1) ||
			    (settings.ui.bootscreen == 4 && splashScreenTime > 60*1)) {
				if (topfb == 1) offset3D_temp = offset3D_i;
				else offset3D_temp = -offset3D_i;
				i_alpha += 10;
				if (i_alpha > 255) i_alpha = 255;
				pp2d_draw_texture_blend(itex, offset3D_temp+logopos+312, 70, RGBA8(255, 255, 255, i_alpha));
			}
			if (oeffects) {
				if (oeffectTimer >= 10) {
					if (topfb == 1) offset3D_temp = offset3D_oeffect2;
					else offset3D_temp = -offset3D_oeffect2;
					pp2d_draw_texture_scale_blend(bigotex, offset3D_temp+logopos+oeffect2_xPos, oeffect2_yPos, oeffect2_scale, oeffect2_scale, RGBA8(255, 255, 255, oeffect2_alpha));
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
					pp2d_draw_texture_scale_blend(bigotex, offset3D_temp+logopos+oeffect3_xPos, oeffect3_yPos, oeffect3_scale, oeffect3_scale, RGBA8(255, 255, 255, oeffect3_alpha));
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
					pp2d_draw_texture_scale_blend(bigotex, offset3D_temp+logopos+oeffect4_xPos, oeffect4_yPos, oeffect4_scale, oeffect4_scale, RGBA8(255, 255, 255, oeffect4_alpha));
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
					pp2d_draw_texture_scale_blend(bigotex, offset3D_temp+logopos+oeffect5_xPos, oeffect5_yPos, oeffect5_scale, oeffect5_scale, RGBA8(255, 255, 255, oeffect5_alpha));
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
					pp2d_draw_texture_scale_blend(bigotex, offset3D_temp+logopos+oeffect6_xPos, oeffect6_yPos, oeffect6_scale, oeffect6_scale, RGBA8(255, 255, 255, oeffect6_alpha));
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
					pp2d_draw_texture_scale_blend(bigotex, offset3D_temp+logopos+oeffect7_xPos, oeffect7_yPos, oeffect7_scale, oeffect7_scale, RGBA8(255, 255, 255, oeffect7_alpha));
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
			pp2d_draw_texture(wipetex, wipePos-320, 0);
			pp2d_draw_rectangle(wipePos, 0, 400, 240, RGBA8(bg_col, bg_col, bg_col, 255));
			if (fadeout) pp2d_draw_rectangle(0, 0, 400, 240, RGBA8(bg_col, bg_col, bg_col, bootSplash_fade));
			if (settings.ui.bootscreen == 1) {
				pp2d_draw_rectangle(0, 0, 40, 240, RGBA8(0, 0, 0, 255)); // Left black bar
				pp2d_draw_rectangle(360, 0, 40, 240, RGBA8(0, 0, 0, 255)); // Right black bar
			}
			
		}
		
		pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
		pp2d_draw_rectangle(0, 0, 320, 240, RGBA8(bg_col, bg_col, bg_col, 255));

		switch ((settings.ui.language < 0 || settings.ui.language >= 14)? sys_language : settings.ui.language) {
			case 0:
			// Japanese
				if (settings.ui.healthsafety) {
					pp2d_draw_texture(hstex, 55, 14);
					pp2d_draw_texture(hstexttex, 0, 0);
					const char* text_url = "www.nintendo.co.jp/anzen/";
					text_width = pp2d_get_text_width(text_url, 0.55, 0.55);
					pp2d_draw_text(((320-text_width)/2), 176, 0.55, 0.55, DSSPLASH, text_url);
				}
				if (touchtocontinue_show) {
					pp2d_draw_texture_blend(hstouchtex, 0, touchtocontinue_yPos, RGBA8(255, 255, 255, touchtocontinue_alpha));
				}
				break;
			case 1:
			default:
			// English
				if (settings.ui.healthsafety) {
					pp2d_draw_texture(hstex, 12, 16);
					pp2d_draw_wtext(34, 19, 0.65, 0.65, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS1), 0.55, 0.55);
					pp2d_draw_wtext(((320-text_width)/2), 56, 0.55, 0.55, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS1));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS2), 0.55, 0.55);
					pp2d_draw_wtext(((320-text_width)/2), 80, 0.55, 0.55, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS2));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS3), 0.55, 0.55);
					pp2d_draw_wtext(((320-text_width)/2), 104, 0.55, 0.55, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS3));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS4), 0.55, 0.55);
					pp2d_draw_wtext(((320-text_width)/2), 128, 0.55, 0.55, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS4));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS5), 0.38, 0.50);
					pp2d_draw_wtext(((320-text_width)/2), 160, 0.38, 0.50, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS5));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS6), 0.55, 0.55);
					pp2d_draw_wtext(((320-text_width)/2), 178, 0.55, 0.55, DSSPLASH, TR(STR_DSSPLASH_WARNING_HS6));
				}
				if (touchtocontinue_show) {
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_TOUCH), 0.55, 0.55);
					pp2d_draw_wtext(((320-text_width)/2), touchtocontinue_yPos, 0.55, 0.55, RGBA8(text_col, text_col, text_col, touchtocontinue_alpha), TR(STR_DSSPLASH_TOUCH));
				}
				break;
			case 2:
			// French
				if (settings.ui.healthsafety) {
					pp2d_draw_texture(hstex, 12, 16);
					pp2d_draw_wtext(35, 19, 0.60, 0.60, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS1), 0.45, 0.47);
					pp2d_draw_wtext(((320-text_width)/2), 56, 0.45, 0.47, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS1));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS2), 0.45, 0.47);
					pp2d_draw_wtext(((320-text_width)/2), 79, 0.45, 0.47, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS2));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS3), 0.45, 0.47);
					pp2d_draw_wtext(((320-text_width)/2), 102, 0.45, 0.47, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS3));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS4), 0.38, 0.50);
					pp2d_draw_wtext(((320-text_width)/2), 126, 0.38, 0.50, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS4));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS5), 0.38, 0.50);
					pp2d_draw_wtext(((320-text_width)/2), 143, 0.38, 0.50, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS5));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS6), 0.38, 0.50);
					pp2d_draw_wtext(((320-text_width)/2), 160, 0.38, 0.50, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS6));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS7), 0.55, 0.55);
					pp2d_draw_wtext(((320-text_width)/2), 178, 0.55, 0.55, DSSPLASH, TR(STR_DSSPLASH_WARNING_HS7));
				}
				if (touchtocontinue_show) {
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_TOUCH), 0.55, 0.55);
					pp2d_draw_wtext(((320-text_width)/2), touchtocontinue_yPos, 0.55, 0.55, RGBA8(text_col, text_col, text_col, touchtocontinue_alpha), TR(STR_DSSPLASH_TOUCH));
				}
				break;
			case 3:
			// German
				if (settings.ui.healthsafety) {
					pp2d_draw_texture(hstex, 12, 16);
					pp2d_draw_wtext(35, 21, 0.50, 0.55, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS1), 0.40, 0.40);
					pp2d_draw_wtext(((320-text_width)/2), 55, 0.40, 0.40, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS1));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS1), 0.40, 0.40);
					pp2d_draw_wtext(((320-text_width)/2), 75, 0.40, 0.40, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS2));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS1), 0.40, 0.40);
					pp2d_draw_wtext(((320-text_width)/2), 95, 0.40, 0.40, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS3));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS1), 0.40, 0.40);
					pp2d_draw_wtext(((320-text_width)/2), 115, 0.40, 0.40, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS4));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS5),0.38, 0.45);
					pp2d_draw_wtext(((320-text_width)/2), 147, 0.38, 0.45, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS5));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS6), 0.38, 0.45);
					pp2d_draw_wtext(((320-text_width)/2), 161, 0.38, 0.45, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS6));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS7), 0.55, 0.55);
					pp2d_draw_wtext(((320-text_width)/2), 178, 0.55, 0.55, DSSPLASH, TR(STR_DSSPLASH_WARNING_HS7));
				}
				if (touchtocontinue_show) {
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_TOUCH), 0.55, 0.55);
					pp2d_draw_wtext(((320-text_width)/2), touchtocontinue_yPos, 0.55, 0.55, RGBA8(text_col, text_col, text_col, touchtocontinue_alpha), TR(STR_DSSPLASH_TOUCH));
				}
				break;
			case 4:
			// Italian
				if (settings.ui.healthsafety) {
					pp2d_draw_texture(hstex, 12, 16);
					pp2d_draw_wtext(34, 19, 0.60, 0.60, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS1), 0.40, 0.40);
					pp2d_draw_wtext(((320-text_width)/2), 55, 0.40, 0.40, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS1));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS2), 0.40, 0.40);
					pp2d_draw_wtext(((320-text_width)/2), 75, 0.40, 0.40, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS2));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS3), 0.40, 0.40);
					pp2d_draw_wtext(((320-text_width)/2), 95, 0.40, 0.40, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS3));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS4), 0.40, 0.40);
					pp2d_draw_wtext(((320-text_width)/2), 115, 0.40, 0.40, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS4));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS5), 0.38, 0.45);
					pp2d_draw_wtext(((320-text_width)/2), 147, 0.38, 0.45, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS5));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS6), 0.38, 0.45);
					pp2d_draw_wtext(((320-text_width)/2), 161, 0.38, 0.45, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS6));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS7), 0.55, 0.55);
					pp2d_draw_wtext(((320-text_width)/2), 178, 0.55, 0.55, DSSPLASH, TR(STR_DSSPLASH_WARNING_HS7));
				}
				if (touchtocontinue_show) {
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_TOUCH), 0.55, 0.55);
					pp2d_draw_wtext(((320-text_width)/2), touchtocontinue_yPos, 0.55, 0.55, RGBA8(text_col, text_col, text_col, touchtocontinue_alpha), TR(STR_DSSPLASH_TOUCH));
				}
				break;
			case 5:
			// Spanish
				if (settings.ui.healthsafety) {
					pp2d_draw_texture(hstex, 12, 16);
					pp2d_draw_wtext(34, 21, 0.50, 0.55, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS1), 0.40, 0.40);
					pp2d_draw_wtext(((320-text_width)/2), 55, 0.40, 0.40, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS1));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS2), 0.40, 0.40);
					pp2d_draw_wtext(((320-text_width)/2), 75, 0.40, 0.40, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS2));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS3), 0.40, 0.40);
					pp2d_draw_wtext(((320-text_width)/2), 95, 0.40, 0.40, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS3));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS4), 0.40, 0.40);
					pp2d_draw_wtext(((320-text_width)/2), 115, 0.40, 0.40, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS4));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS5), 0.38, 0.45);
					pp2d_draw_wtext(((320-text_width)/2), 147, 0.38, 0.45, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS5));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS6), 0.38, 0.45);
					pp2d_draw_wtext(((320-text_width)/2), 161, 0.38, 0.45, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS6));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS7), 0.55, 0.55);
					pp2d_draw_wtext(((320-text_width)/2), 178, 0.55, 0.55, DSSPLASH, TR(STR_DSSPLASH_WARNING_HS7));
				}
				if (touchtocontinue_show) {
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_TOUCH), 0.55, 0.55);
					pp2d_draw_wtext(((320-text_width)/2), touchtocontinue_yPos, 0.55, 0.55, RGBA8(text_col, text_col, text_col, touchtocontinue_alpha), TR(STR_DSSPLASH_TOUCH));
				}
				break;
			case 6:
			case 11:
			// Chinese
				if (settings.ui.healthsafety) {
					pp2d_draw_texture(hstex, 49, 12);
					pp2d_draw_texture(hstexttex, 0, 0);
					const char* text_url = "www.iQue.com/service/";
					text_width = pp2d_get_text_width(text_url, 0.55, 0.55);
					pp2d_draw_text(((320-text_width)/2), 166, 0.55, 0.55, DSSPLASH, text_url);
				}
				if (touchtocontinue_show) {
					pp2d_draw_texture_blend(hstouchtex, 0, touchtocontinue_yPos, RGBA8(255, 255, 255, touchtocontinue_alpha));
				}
				break;
			case 7:
			// Korean
				if (settings.ui.healthsafety) {
					pp2d_draw_texture(hstex, 29, 10);
					pp2d_draw_texture(hstexttex, 0, 0);
					const char* text_url = "www.nintendocaution.co.kr";
					text_width = pp2d_get_text_width(text_url, 0.55, 0.55);
					pp2d_draw_text(((320-text_width)/2), 176, 0.55, 0.55, DSSPLASH, text_url);
				}
				if (touchtocontinue_show) {
					pp2d_draw_texture_blend(hstouchtex, 0, touchtocontinue_yPos, RGBA8(255, 255, 255, touchtocontinue_alpha));
				}
				break;
			case 13:
			// Turkish
				if (settings.ui.healthsafety) {
					pp2d_draw_texture(hstex, 12, 16);
					pp2d_draw_wtext(34, 19, 0.50, 0.65, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS1), 0.45, 0.47);
					pp2d_draw_wtext(((320-text_width)/2), 54, 0.45, 0.47, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS1));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS2), 0.45, 0.47);
					pp2d_draw_wtext(((320-text_width)/2), 75, 0.45, 0.47, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS2));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS3), 0.45, 0.47);
					pp2d_draw_wtext(((320-text_width)/2), 96, 0.45, 0.47, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS3));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS4), 0.45, 0.47);
					pp2d_draw_wtext(((320-text_width)/2), 117, 0.45, 0.47, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS4));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS5), 0.38, 0.50);
					pp2d_draw_wtext(((320-text_width)/2), 140, 0.38, 0.50, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS5));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS6), 0.38, 0.50);
					pp2d_draw_wtext(((320-text_width)/2), 160, 0.38, 0.50, RGBA8(text_col, text_col, text_col, 255), TR(STR_DSSPLASH_WARNING_HS6));
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_WARNING_HS7), 0.55, 0.55);
					pp2d_draw_wtext(((320-text_width)/2), 178, 0.55, 0.55, DSSPLASH, TR(STR_DSSPLASH_WARNING_HS7));
				}
				if (touchtocontinue_show) {
					text_width = pp2d_get_wtext_width(TR(STR_DSSPLASH_TOUCH), 0.55, 0.55);
					pp2d_draw_wtext(((320-text_width)/2), touchtocontinue_yPos, 0.55, 0.55, RGBA8(text_col, text_col, text_col, touchtocontinue_alpha), TR(STR_DSSPLASH_TOUCH));
				}
				break;
		}
		pp2d_draw_rectangle(0, 0, 320, 240, RGBA8(bg_col, bg_col, bg_col, bootSplash_fade));
		
		splashScreenTime += 1;
		if (splashScreenTime > 60*2) touchtocontinue_show = true;
		

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
		
		pp2d_end_draw();
		
		if (fadeout && bootSplash_fade == 255) break;
		
	}
	
	if (logEnabled)	LogFM("BootSplash", "Unloading jingle sound");
	delete sfx_boot;
	if (logEnabled)	LogFM("BootSplash", "Unloaded jingle sound");

}
