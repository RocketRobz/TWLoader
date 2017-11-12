// TWLoader: Settings screen.
#include "settings.h"
#include "main.h"
#include "date.h"
#include "sound.h"
#include "download.h"
#include "log.h"
#include "language.h"
#include "textfns.h"
#include "inifile.h"

#include <unistd.h>
#include <string>
using std::string;
using std::wstring;

#include <3ds.h>
#include "keyboard.h"

enum SubScreenMode {
	SUBSCREEN_MODE_FRONTEND = 0,			// Frontend settings (page 1)
	SUBSCREEN_MODE_FRONTEND2 = 1,			// Frontend settings (page 2)
	SUBSCREEN_MODE_FRONTEND3 = 2,			// Frontend settings (page 3)
	SUBSCREEN_MODE_NTR = 3,				// NTR mode settings
	SUBSCREEN_MODE_FLASH_CARD = 4,			// Flash card options
	SUBSCREEN_MODE_SUB_THEME = 5,			// Sub-theme select
	SUBSCREEN_MODE_CHANGE_ROM_PATH = 6,		// Sub-menu with rom path location
	SUBSCREEN_MODE_TWLNAND_NOT_FOUND = 7,	// TWLNAND side not found message
};
static SubScreenMode subscreenmode = SUBSCREEN_MODE_FRONTEND;

/** Settings **/
const char *twlnand_msg;

const char* srldrsettingsinipath = "sdmc:/_nds/srloader/settings.ini";
static CIniFile settingsini("sdmc:/_nds/twloader/settings.ini");
static CIniFile srldrsettingsini(srldrsettingsinipath);

static bool settings_tex_loaded = false;

// Color settings.
// Use SET_ALPHA() to replace the alpha value.
const ColorData *color_data = NULL;
u32 menucolor;

// 3D offsets. (0 == Left, 1 == Right)
Offset3D offset3D[2] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

// Cursor position. (one per subscreen)
static int cursor_pos[7] = {0, 0, 0, 0, 0, 0, 0};

// Location of the bottom screen image.
const char* bottomloc = NULL;

// Settings
Settings_t settings;

/**
 * Reset the settings screen's subscreen mode.
 */
void settingsResetSubScreenMode(void)
{
	if (TWLNANDnotfound_msg != 2) {
		subscreenmode = SUBSCREEN_MODE_TWLNAND_NOT_FOUND;
	} else {
		subscreenmode = SUBSCREEN_MODE_FRONTEND;
	}
	memset(cursor_pos, 0, sizeof(cursor_pos));
}

/**
 * Load the settings textures.
 */
void settingsLoadTextures(void)
{
	if (settings_tex_loaded)
		return;

	/** Top screen **/
	pp2d_load_texture_png(setvoltex[0], "romfs:/graphics/settings/volume0.png"); // Show no volume (settings)
	pp2d_load_texture_png(setvoltex[1], "romfs:/graphics/settings/volume1.png"); // Volume low above 0 (settings)
	pp2d_load_texture_png(setvoltex[2], "romfs:/graphics/settings/volume2.png"); // Volume medium (settings)
	pp2d_load_texture_png(setvoltex[3], "romfs:/graphics/settings/volume3.png"); // Hight volume (settings)	
	pp2d_load_texture_png(setvoltex[4], "romfs:/graphics/settings/volume4.png"); // 100% (settings)
	pp2d_load_texture_png(setvoltex[5], "romfs:/graphics/settings/volume5.png"); // No DSP firm found (settings)

	pp2d_load_texture_png(setbatterychrgtex, "romfs:/graphics/settings/battery_charging.png");
	pp2d_load_texture_png(setbatterytex[0], "romfs:/graphics/settings/battery0.png");
	pp2d_load_texture_png(setbatterytex[1], "romfs:/graphics/settings/battery1.png");
	pp2d_load_texture_png(setbatterytex[2], "romfs:/graphics/settings/battery2.png");
	pp2d_load_texture_png(setbatterytex[3], "romfs:/graphics/settings/battery3.png");
	pp2d_load_texture_png(setbatterytex[4], "romfs:/graphics/settings/battery4.png");
	pp2d_load_texture_png(setbatterytex[5], "romfs:/graphics/settings/battery5.png");

	pp2d_load_texture_png(dsboottex, "romfs:/graphics/settings/dsboot.png"); // DS boot screen in settings
	pp2d_load_texture_png(dsiboottex, "romfs:/graphics/settings/dsiboot.png"); // DSi boot screen in settings
	pp2d_load_texture_png(invdsboottex, "romfs:/graphics/settings/inv_dsboot.png"); // DS boot screen in settings
	pp2d_load_texture_png(invdsiboottex, "romfs:/graphics/settings/inv_dsiboot.png"); // DSi boot screen in settings
	switch (settings.ui.language) {
		case 0:
			pp2d_load_texture_png(dshstex, "romfs:/graphics/settings/dshs_JA.png"); // DS H&S screen in settings
			pp2d_load_texture_png(dsihstex, "romfs:/graphics/settings/dshs_JA.png"); // DS H&S screen in settings
			break;
		case 1:
		default:
			pp2d_load_texture_png(dshstex, "romfs:/graphics/settings/dsihs.png"); // DS H&S screen in settings
			pp2d_load_texture_png(dsihstex, "romfs:/graphics/settings/dsihs.png"); // DSi H&S screen in settings
			break;
		case 2:
			pp2d_load_texture_png(dshstex, "romfs:/graphics/settings/dshs_FR.png"); // DS H&S screen in settings
			pp2d_load_texture_png(dsihstex, "romfs:/graphics/settings/dshs_FR.png"); // DS H&S screen in settings
			break;
		case 3:
			pp2d_load_texture_png(dshstex, "romfs:/graphics/settings/dshs_DE.png"); // DS H&S screen in settings
			pp2d_load_texture_png(dsihstex, "romfs:/graphics/settings/dshs_DE.png"); // DS H&S screen in settings
			break;
		case 4:
			pp2d_load_texture_png(dshstex, "romfs:/graphics/settings/dshs_IT.png"); // DS H&S screen in settings
			pp2d_load_texture_png(dsihstex, "romfs:/graphics/settings/dshs_IT.png"); // DS H&S screen in settings
			break;
		case 5:
			pp2d_load_texture_png(dshstex, "romfs:/graphics/settings/dshs_ES.png"); // DS H&S screen in settings
			pp2d_load_texture_png(dsihstex, "romfs:/graphics/settings/dshs_ES.png"); // DS H&S screen in settings
			break;
		case 6:
		case 11:
			pp2d_load_texture_png(dshstex, "romfs:/graphics/settings/dshs_CH.png"); // DS H&S screen in settings
			pp2d_load_texture_png(dsihstex, "romfs:/graphics/settings/dshs_CH.png"); // DS H&S screen in settings
			break;
		case 7:
			pp2d_load_texture_png(dshstex, "romfs:/graphics/settings/dshs_KO.png"); // DS H&S screen in settings
			pp2d_load_texture_png(dsihstex, "romfs:/graphics/settings/dshs_KO.png"); // DS H&S screen in settings
			break;
	}
	pp2d_load_texture_png(disabledtex, "romfs:/graphics/settings/disable.png"); // Red circle with line

	/** Bottom screen **/
	pp2d_load_texture_png(settingstex, "romfs:/graphics/settings/screen.png"); // Bottom of settings screen
	pp2d_load_texture_png(whomeicontex, "romfs:/graphics/settings/whomeicon.png"); // HOME icon

	// All textures loaded.
	settings_tex_loaded = true;
}

/**
 * Unload the settings textures.
 */
void settingsUnloadTextures(void)
{
	if (!settings_tex_loaded)
		return;

	/** Top screen **/
	for (int i = 0; i < 6; i++) {
		pp2d_free_texture(setvoltex[i]);
	}
	pp2d_free_texture(setbatterychrgtex);

	for (int i = 0; i < 6; i++) {
		pp2d_free_texture(setbatterytex[i]);
	}

	pp2d_free_texture(dsboottex);
	pp2d_free_texture(dsiboottex);
	pp2d_free_texture(invdsboottex);
	pp2d_free_texture(invdsiboottex);
	pp2d_free_texture(dshstex);
	pp2d_free_texture(dsihstex);
	pp2d_free_texture(disabledtex);

	/** Bottom screen **/
	pp2d_free_texture(settingstex);
	pp2d_free_texture(whomeicontex);

	// All textures unloaded.
	settings_tex_loaded = false;
}

int twlfaderun = true;
int oaderfaderun = false;
int twlfade = true;
int oaderfade = true;
int demofade = true;
int twlfadealpha = 0;
int oaderfadealpha = 0;
int demofadealpha = 0;

/**
 * Draw the top settings screen.
 */
void settingsDrawTopScreen(void)
{
	if (!settings_tex_loaded) {
		settingsLoadTextures();
	}
	update_battery_level(setbatterychrgtex, setbatterytex);

	// Draw twice; once per 3D framebuffer.
	for (int topfb = GFX_LEFT; topfb <= GFX_RIGHT; topfb++) {
//		pp2d_begin_draw(GFX_TOP, (gfx3dSide_t)topfb);
		pp2d_draw_on(GFX_TOP, (gfx3dSide_t)topfb);
		pp2d_draw_texture_scale(settingstex, 0, 0, 1.32, 1);
		if (subscreenmode == SUBSCREEN_MODE_FRONTEND2) {
			if (settings.ui.bootscreen >= 3) {
				if (settings.ui.bootscreen == 4) {
					pp2d_draw_texture(invdsiboottex, offset3D[topfb].boxart+136, 20); // Draw boot screen
				} else {
					pp2d_draw_texture(invdsboottex, offset3D[topfb].boxart+136, 20); // Draw boot screen
				}
				pp2d_draw_rectangle(offset3D[topfb].boxart+120, 20, 16, 96, BLACK);
				pp2d_draw_rectangle(offset3D[topfb].boxart+264, 20, 16, 96, BLACK);
			} else if (settings.ui.bootscreen == 2) {
				pp2d_draw_texture(dsiboottex, offset3D[topfb].boxart+136, 20); // Draw boot screen
				pp2d_draw_rectangle(offset3D[topfb].boxart+120, 20, 16, 96, WHITE);
				pp2d_draw_rectangle(offset3D[topfb].boxart+264, 20, 16, 96, WHITE);
			} else if (settings.ui.bootscreen <= 1) {
				pp2d_draw_texture(dsboottex, offset3D[topfb].boxart+136, 20); // Draw boot screen
				if (settings.ui.bootscreen == 1) {
					pp2d_draw_rectangle(offset3D[topfb].boxart+120, 20, 16, 96, BLACK);
					pp2d_draw_rectangle(offset3D[topfb].boxart+264, 20, 16, 96, BLACK);
				} else {
					pp2d_draw_rectangle(offset3D[topfb].boxart+120, 20, 16, 96, WHITE);
					pp2d_draw_rectangle(offset3D[topfb].boxart+264, 20, 16, 96, WHITE);
				}
			} else {
				pp2d_draw_rectangle(offset3D[topfb].boxart+136, 20, 128, 96, WHITE);
			}
			if (settings.ui.healthsafety == 1) {
				if (settings.ui.bootscreen >= 3) {
					pp2d_draw_rectangle(offset3D[topfb].boxart+136, 124, 128, 96, BLACK);
					pp2d_draw_text(offset3D[topfb].boxart+162, 152, 0.55, 0.55, WHITE, "  Preview\nunavailable");					
				} else if (settings.ui.bootscreen == 2) {
					pp2d_draw_texture(dsihstex, offset3D[topfb].boxart+136, 124); // Draw H&S screen
				} else if (settings.ui.bootscreen <= 1) {
					pp2d_draw_texture(dshstex, offset3D[topfb].boxart+136, 124); // Draw H&S screen
				} else {
					pp2d_draw_rectangle(offset3D[topfb].boxart+136, 124, 128, 96, WHITE);
				}
			} else {
				if (settings.ui.bootscreen >= 3) {
					// Draw a black screen in place of the H&S screen.
					pp2d_draw_rectangle(offset3D[topfb].boxart+136, 124, 128, 96, BLACK);
				} else {
					// Draw a white screen in place of the H&S screen.
					pp2d_draw_rectangle(offset3D[topfb].boxart+136, 124, 128, 96, WHITE);
				}
			}
			if (!settings.ui.showbootscreen) {
				pp2d_draw_texture(disabledtex, offset3D[topfb].disabled+136, 20); // Draw disabled texture
				pp2d_draw_texture(disabledtex, offset3D[topfb].disabled+136, 124); // Draw disabled texture	
			}
		} else {
			pp2d_draw_texture(settingslogotex, offset3D[topfb].boxart+400/2 - 256/2, 240/2 - 128/2);
			pp2d_draw_texture_blend(settingslogotwltex, offset3D[topfb].boxart+400/2 - 256/2, 240/2 - 128/2, RGBA8(255,255,255,twlfadealpha));
			pp2d_draw_texture_blend(settingslogooadertex, offset3D[topfb].boxart+400/2 - 256/2, 240/2 - 128/2, RGBA8(255,255,255,oaderfadealpha));

			if(isDemo) pp2d_draw_texture_blend(settingslogodemotex, offset3D[topfb].boxart+400/2 - 256/2, 240/2 - 128/2, RGBA8(255,255,255,demofadealpha));
			if (subscreenmode == SUBSCREEN_MODE_NTR) {
				pp2d_draw_wtext(offset3D[topfb].disabled+72, 174, 0.60, 0.60, BLUE, TR(STR_SETTINGS_XBUTTON_RELEASE));
				pp2d_draw_wtext(offset3D[topfb].disabled+72, 190, 0.60, 0.60, GREEN, TR(STR_SETTINGS_YBUTTON_UNOFFICIAL));
			} else if (subscreenmode == SUBSCREEN_MODE_CHANGE_ROM_PATH) {
				pp2d_draw_text(offset3D[topfb].disabled+32, 192, 0.55, 0.55, WHITE, "TWLoader will auto-restart if location is changed.");
			}
		}

		pp2d_draw_text(318.0f, 1, 0.58f, 0.58f, WHITE, RetTime(false).c_str());
		
		if(!isNightly){
			std::string version = settings_vertext;		
			if (version.substr(version.find_first_not_of(' '), (version.find_last_not_of(' ') - version.find_first_not_of(' ') + 1)).size() > 8) {
				pp2d_draw_text(324, 222, 0.60, 0.60f, WHITE, settings_vertext);
			}else{
				pp2d_draw_text(336, 222, 0.60, 0.60f, WHITE, settings_vertext);
			}
		}else{
			char nightlyhash[16];
			snprintf(nightlyhash, 16, "%s", NIGHTLY);
			pp2d_draw_text(272, 222, 0.60, 0.60f, WHITE, nightlyhash);			
		}
		if (settings.twl.bootstrapfile == 1) {
			fat = "sd:/";
			// Green
			pp2d_draw_text(5, 222, 0.60, 0.60f, GREEN, settings_unofficialbootstrapver.c_str());
		} else {
			fat = "sd:/";
			// Blue
			pp2d_draw_text(5, 222, 0.60, 0.60f, BLUE, settings_releasebootstrapver.c_str());
		}

		draw_volume_slider(setvoltex);
		pp2d_draw_texture(batteryIcon, 371, 2);
		if (!settings.ui.name.empty()) {
			pp2d_draw_text(34.0f, 1.0f, 0.58, 0.58f, SET_ALPHA(color_data->color, 255), settings.ui.name.c_str());
		}
		DrawDate(264.0f, 1.0f, 0.58f, 0.58f, WHITE);
		if (fadealpha > 0) pp2d_draw_rectangle(0, 0, 400, 240, RGBA8(0, 0, 0, fadealpha)); // Fade in/out effect
	}

	if(demofade) {
		demofadealpha += 5;
		if (demofadealpha == 255) {
			demofade = false;
		}
	} else {
		demofadealpha -= 5;
		if (demofadealpha == 0) {
			demofade = true;
		}
	}
	if(twlfaderun) {
		if(twlfade && twlfadealpha != 254) {
			twlfadealpha += 2;
			if (twlfadealpha == 254) {
				twlfadealpha = 255;
				twlfaderun = false;
				twlfade = false;
				oaderfaderun = true;
			}
		} else {
			twlfadealpha -= 2;
			if (twlfadealpha == 1) {
				twlfadealpha = 0;
				twlfade = true;
			}
		}
	}
	if(oaderfaderun) {
		if(oaderfade && oaderfadealpha != 254) {
			oaderfadealpha += 2;
			if (oaderfadealpha == 254) {
				oaderfadealpha = 255;
				oaderfaderun = false;
				oaderfade = false;
				twlfaderun = true;
			}
		} else {
			oaderfadealpha -= 2;
			if (oaderfadealpha == 1) {
				oaderfadealpha = 0;
				oaderfade = true;
			}
		}
	}
}

/**
 * Draw the bottom settings screen.
 */
void settingsDrawBottomScreen(void)
{
	if (!settings_tex_loaded) {
		settingsLoadTextures();
	}

	pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
	pp2d_draw_texture(settingstex, 0, 0);
	pp2d_draw_rectangle(0, 0, 320, 28, RGBA8(0, 0, 0, 31));
	for (int i = 0; i < 80; i++)
		pp2d_draw_rectangle(i*4, 26, 2, 1, GRAY);
	pp2d_draw_rectangle(0, 179, 320, 41, RGBA8(0, 0, 0, 31));
	for (int i = 0; i < 80; i++)
		pp2d_draw_rectangle(i*4, 180, 2, 1, GRAY);
	for (int i = 0; i < 80; i++)
		pp2d_draw_rectangle(i*4, 218, 2, 1, GRAY);
	
	if (subscreenmode < 4) {
		u32 colorssm = WHITE;
		if (subscreenmode == 0) {
			colorssm = SET_ALPHA(color_data->color, 255);
		} else{
			colorssm = WHITE;
		}
		pp2d_draw_text(252, 6, 0.50, 0.50, colorssm, "1");
		if (subscreenmode == 1) {
			colorssm = SET_ALPHA(color_data->color, 255);
		} else{
			colorssm = WHITE;
		}
		pp2d_draw_text(268, 6, 0.50, 0.50, colorssm, "2");
		if (subscreenmode == 2) {
			colorssm = SET_ALPHA(color_data->color, 255);
		} else{
			colorssm = WHITE;
		}
		pp2d_draw_text(284, 6, 0.50, 0.50, colorssm, "3");
		if (subscreenmode == 3) {
			colorssm = SET_ALPHA(color_data->color, 255);
		} else{
			colorssm = WHITE;
		}
		pp2d_draw_text(300, 6, 0.50, 0.50, colorssm, "4");
	}
	
	// X positions.
	static const int Xpos = 24;
	static const int XposValue = 236;
	// Title for the bottom screen.
	const wchar_t *title = L"";

	if (subscreenmode == SUBSCREEN_MODE_FRONTEND) {
		pp2d_draw_texture(shoulderLtex, 0, LshoulderYpos);
		pp2d_draw_texture(shoulderRtex, 248, RshoulderYpos);
		pp2d_draw_text(17, LshoulderYpos+4, 0.50, 0.50, BLACK, Lshouldertext);
		pp2d_draw_text(252, RshoulderYpos+4, 0.50, 0.50, BLACK, Rshouldertext);

		// Language.
		static const char *const language_text[] = {
			"日本語",	// Japanese
			"English",	// English
			"Français",	// French
			"Deutsch",	// German
			"Italiano",	// Italian
			"Español",	// Spanish
			"ZHCN",		// Simplified Chinese (TODO)
			"Korean",	// Korean [Font is missing characters]
			"Nederlands",	// Dutch
			"Português",	// Portuguese
			"Russian",	// Russian (TODO) [Font's characters are too wide]
			"ZHTW",		// Traditional Chinese (TODO)
			"Turkish",		// Turkish
			"Finnish",		// Finnish
		};
		// TODO: Cache the conversion?
		wstring languagevaluetext;
		if (settings.ui.language >= 0 && settings.ui.language < 14) {
			languagevaluetext = utf8_to_wstring(language_text[settings.ui.language]);
		} else {
			// TODO: Translate?
			languagevaluetext = latin1_to_wstring("System");
		}

		// Theme text.
		static const char *const theme_text[] = {
			"DSi Menu", "3DS HOME Menu", "R4", "akMenu/Wood"
		};
		if (settings.ui.theme < THEME_DSIMENU || settings.ui.theme > THEME_AKMENU)
			settings.ui.theme = THEME_DSIMENU;
		const char *const themevaluetext = theme_text[settings.ui.theme];

		// Color text.
		const wchar_t *color_text[] = {
			TR(STR_SETTINGS_VALUES_GRAY),
			TR(STR_SETTINGS_VALUES_BROWN),
			TR(STR_SETTINGS_VALUES_RED),
			TR(STR_SETTINGS_VALUES_PINK),
			TR(STR_SETTINGS_VALUES_ORANGE),
			TR(STR_SETTINGS_VALUES_YELLOW),
			TR(STR_SETTINGS_VALUES_YELLOW_GREEN),
			TR(STR_SETTINGS_VALUES_GREEN_1),
			TR(STR_SETTINGS_VALUES_GREEN_2),
			TR(STR_SETTINGS_VALUES_LIGHT_GREEN),
			TR(STR_SETTINGS_VALUES_SKY_BLUE),
			TR(STR_SETTINGS_VALUES_LIGHT_BLUE),
			TR(STR_SETTINGS_VALUES_BLUE),
			TR(STR_SETTINGS_VALUES_VIOLET),
			TR(STR_SETTINGS_VALUES_PURPLE),
			TR(STR_SETTINGS_VALUES_FUCHSIA),
			TR(STR_SETTINGS_VALUES_RED_AND_BLUE),
			TR(STR_SETTINGS_VALUES_GREEN_AND_YELLOW),
			TR(STR_SETTINGS_VALUES_CHRISTMAS)
		};
		if (settings.ui.color < 0 || settings.ui.color > 18)
			settings.ui.color = 0;
		const wchar_t *colorvaluetext = color_text[settings.ui.color];

		// Menu color text.
		const wchar_t *menu_color_text[] = {
			TR(STR_SETTINGS_VALUES_WHITE),
			TR(STR_SETTINGS_VALUES_BLACK),
			TR(STR_SETTINGS_VALUES_BROWN),
			TR(STR_SETTINGS_VALUES_RED),
			TR(STR_SETTINGS_VALUES_PINK),
			TR(STR_SETTINGS_VALUES_ORANGE),
			TR(STR_SETTINGS_VALUES_YELLOW),
			TR(STR_SETTINGS_VALUES_YELLOW_GREEN),
			TR(STR_SETTINGS_VALUES_GREEN_1),
			TR(STR_SETTINGS_VALUES_GREEN_2),
			TR(STR_SETTINGS_VALUES_LIGHT_GREEN),
			TR(STR_SETTINGS_VALUES_SKY_BLUE),
			TR(STR_SETTINGS_VALUES_LIGHT_BLUE),
			TR(STR_SETTINGS_VALUES_BLUE),
			TR(STR_SETTINGS_VALUES_VIOLET),
			TR(STR_SETTINGS_VALUES_PURPLE),
			TR(STR_SETTINGS_VALUES_FUCHSIA)
		};
		if (settings.ui.menucolor < 0 || settings.ui.menucolor > 18)
			settings.ui.menucolor = 0;
		const wchar_t  *menucolorvaluetext = menu_color_text[settings.ui.menucolor];

		const char *const filenamevaluetext = (settings.ui.filename ? "On" : "Off");
		const char *const countervaluetext = (settings.ui.counter ? "On" : "Off");

		title = TR(STR_SETTINGS_GUI);
		struct {
			int x;
			int y;
		} buttons[] = {
			{ 17,  39},
			{169,  39},
			{ 17,  87},
			{169,  87},
			{ 17, 135},
			{169, 135},
		};
		const wchar_t *button_titles[] = {
			TR(STR_SETTINGS_LANGUAGE),
			TR(STR_SETTINGS_THEME),
			TR(STR_SETTINGS_COLOR),
			TR(STR_SETTINGS_MENUCOLOR),
			TR(STR_SETTINGS_FILENAME),
			TR(STR_SETTINGS_COUNTER),
		};
		const wchar_t *button_descw[] = {
			languagevaluetext.c_str(),
			NULL,
			colorvaluetext,
			menucolorvaluetext,
			NULL,
			NULL,
		};
		const char *button_desc[] = {
			NULL,
			themevaluetext,
			NULL,
			NULL,
			filenamevaluetext,
			countervaluetext,
		};
		
		for (int i = (int)(sizeof(buttons)/sizeof(buttons[0]))-1; i >= 0; i--) {
			if (cursor_pos[0] == i) {
				// Button is highlighted.
				pp2d_draw_texture(dboxtex_button, buttons[i].x, buttons[i].y);
			} else {
				// Button is not highlighted. Darken the texture.
				pp2d_draw_texture_blend(dboxtex_button, buttons[i].x, buttons[i].y, GRAY);
			}

			const wchar_t *title = button_titles[i];
			const wchar_t *value_descw = button_descw[i];
			const char *value_desc = button_desc[i];

			// Determine the text height.
			// NOTE: Button texture size is 132x34.
			const int h = 32;

			// Draw the title.
			int y = buttons[i].y + ((34 - h) / 2);
			int w = 0;
			int x = ((2 - w) / 2) + buttons[i].x;
			pp2d_draw_wtext(x, y, 0.50, 0.50, BLACK, title);
			y += 16;

			// Draw the value.
			w = 0;
			x = ((2 - w) / 2) + buttons[i].x;
			u32 color_ = BLACK;
			if (i == 2) color_ = SET_ALPHA(color_data->color, 255);
			if (i == 0 || i == 2 || i == 3) pp2d_draw_wtext(x, y, 0.50, 0.50, color_, value_descw);
			else pp2d_draw_text(x, y, 0.50, 0.50, color_, value_desc);
		}
		if (cursor_pos[0] == 0) {
			pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_LANGUAGE_1));
			pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_LANGUAGE_2));
		}
		if (cursor_pos[0] == 1) {
			pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_THEME_1));
			pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_THEME_2));
		}
		if (cursor_pos[0] == 2) {
			pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_COLOR_1));
			pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_COLOR_2));
		}
		if (cursor_pos[0] == 3) {
			pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_MENUCOLOR_1));
			pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_MENUCOLOR_2));
		}
		if (cursor_pos[0] == 4) {
			pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_FILENAME_1));
			pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_FILENAME_2));
		}
		if (cursor_pos[0] == 5) {
			pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_COUNTER_1));
			pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_COUNTER_2));
		}
	} else if (subscreenmode == SUBSCREEN_MODE_FRONTEND2) {
		pp2d_draw_texture(shoulderLtex, 0, LshoulderYpos);
		pp2d_draw_texture(shoulderRtex, 248, RshoulderYpos);
		pp2d_draw_text(17, LshoulderYpos+4, 0.50, 0.50, BLACK, Lshouldertext);
		pp2d_draw_text(252, RshoulderYpos+4, 0.50, 0.50, BLACK, Rshouldertext);

		const char *const custombotvaluetext = (settings.ui.custombot ? "On" : "Off");
		
		const char *autodlvaluetext = (settings.ui.autoupdate_twldr ? "On" : "Off");

		const char *filetypevaluetext;
		switch (settings.ui.filetype) {
			case 0:
			default:
				filetypevaluetext = "CIA";
				break;
			case 1:
				filetypevaluetext = "3DSX";
				break;
			case 2:
				filetypevaluetext = "CIA & 3DSX";
				break;
		}
		
		// Boot screen text.
		static const char *const bootscreen_text[] = {
			"Nintendo DS", "Nintendo DS (OAR)", "Nintendo DSi", "Nintendo DS (Black)", "Nintendo DSi (Black)"
		};
		if (settings.ui.bootscreen < 0 || settings.ui.bootscreen > 4)
			settings.ui.bootscreen = 0;
		const char *const bootscreenvaluetext = bootscreen_text[settings.ui.bootscreen];
		const char *healthsafetyvaluetext = (settings.ui.healthsafety ? "On" : "Off");

		title = TR(STR_SETTINGS_GUI);
		struct {
			int x;
			int y;
		} buttons[] = {
			{ 17,  39},
			{169,  39},
			{ 17,  87},
			{169,  87},
			{ 17, 135},
			{169, 135},
		};
		const wchar_t *button_titles[] = {
			TR(STR_SETTINGS_CUSTOM_BOTTOM),
			TR(STR_SETTINGS_AUTOUPDATE_TWLOADER),
			TR(STR_SETTINGS_TWLOADER_FILETYPE),
			TR(STR_SETTINGS_UPDATE_TWLOADER),
			TR(STR_SETTINGS_DS_DSi_BOOT_SCREEN),
			TR(STR_SETTINGS_DS_DSi_SAFETY_MESSAGE),
		};
		const char *button_desc[] = {
			custombotvaluetext,
			autodlvaluetext,
			filetypevaluetext,
			"",
			bootscreenvaluetext,
			healthsafetyvaluetext,
		};
		
		for (int i = (int)(sizeof(buttons)/sizeof(buttons[0]))-1; i >= 0; i--) {
			if (cursor_pos[1] == i) {
				// Button is highlighted.
				pp2d_draw_texture(dboxtex_button, buttons[i].x, buttons[i].y);
			} else {
				// Button is not highlighted. Darken the texture.
				pp2d_draw_texture_blend(dboxtex_button, buttons[i].x, buttons[i].y, RGBA8(127, 127, 127, 255));
			}

			const wchar_t *title = button_titles[i];
			const char *value_desc = button_desc[i];

			// Determine the text height.
			// NOTE: Button texture size is 132x34.
			const int h = 32;

			// Draw the title.
			int y = buttons[i].y + ((34 - h) / 2);
			int w = 0;
			int x = ((2 - w) / 2) + buttons[i].x;
			pp2d_draw_wtext(x, y, 0.50, 0.50, BLACK, title);
			y += 16;

			// Draw the value.
			w = 0;
			x = ((2 - w) / 2) + buttons[i].x;
			pp2d_draw_text(x, y, 0.50, 0.50, BLACK, value_desc);
		}
		if (cursor_pos[1] == 0) {
			pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_CUSTOM_BOTTOM_1));
			pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_CUSTOM_BOTTOM_2));
		}
		if (cursor_pos[1] == 1) {
			pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_AUTOUPDATE_TWLOADER_1));
			pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_AUTOUPDATE_TWLOADER_2));
		}
		if (cursor_pos[1] == 2) {
			pp2d_draw_text(8, 184, 0.60, 0.60f, WHITE, "Select the filetype of TWLoader");
			pp2d_draw_text(8, 198, 0.60, 0.60f, WHITE, "you're using.");
		}
		if (cursor_pos[1] == 3) {
			pp2d_draw_text(8, 184, 0.60, 0.60f, WHITE, "Press  to update TWLoader.");
		}
		if (cursor_pos[1] == 4) {
			pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_DS_DSi_BOOT_SCREEN_1));
			pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_DS_DSi_BOOT_SCREEN_2));
		}
		if (cursor_pos[1] == 5) {
			pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_DS_DSi_SAFETY_MESSAGE_1));
			pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_DS_DSi_SAFETY_MESSAGE_2));
		}
	} else if (subscreenmode == SUBSCREEN_MODE_FRONTEND3) {
		pp2d_draw_texture(shoulderLtex, 0, LshoulderYpos);
		pp2d_draw_texture(shoulderRtex, 248, RshoulderYpos);
		pp2d_draw_text(17, LshoulderYpos+4, 0.50, 0.50, BLACK, Lshouldertext);
		pp2d_draw_text(252, RshoulderYpos+4, 0.50, 0.50, BLACK, Rshouldertext);

		const char *showbootscreenvaluetext;
		switch (settings.ui.showbootscreen) {
			case 0:
			default:
				showbootscreenvaluetext = "No";
				break;
			case 1:
				showbootscreenvaluetext = "Before ROM select screen";
				break;
			case 2:
				showbootscreenvaluetext = "After launching a ROM";
				break;
		}

		char printedROMpath[256];
		snprintf (printedROMpath, sizeof(printedROMpath), "SD:/%s", settings.ui.romfolder.c_str());

		title = TR(STR_SETTINGS_GUI);
		struct {
			int x;
			int y;
		} buttons[] = {
			{ 17,  39},
			{169,  39},
		};
		const wchar_t *button_titles[] = {
			TR(STR_SETTINGS_SHOW_BOOT_SCREEN),
			TR(STR_SETTINGS_ROM_PATH),
		};
		const char *button_desc[] = {
			showbootscreenvaluetext,
			printedROMpath,
		};
		
		for (int i = (int)(sizeof(buttons)/sizeof(buttons[0]))-1; i >= 0; i--) {
			if (cursor_pos[2] == i) {
				// Button is highlighted.
				pp2d_draw_texture(dboxtex_button, buttons[i].x, buttons[i].y);
			} else {
				// Button is not highlighted. Darken the texture.
				pp2d_draw_texture_blend(dboxtex_button, buttons[i].x, buttons[i].y, RGBA8(127, 127, 127, 255));
			}

			const wchar_t *title = button_titles[i];
			const char *value_desc = button_desc[i];

			// Determine the text height.
			// NOTE: Button texture size is 132x34.
			const int h = 32;

			// Draw the title.
			int y = buttons[i].y + ((34 - h) / 2);
			int w = 0;
			int x = ((2 - w) / 2) + buttons[i].x;
			pp2d_draw_wtext(x, y, 0.50, 0.50, BLACK, title);
			y += 16;

			// Draw the value.
			w = 0;
			x = ((2 - w) / 2) + buttons[i].x;
			pp2d_draw_text(x, y, 0.50, 0.50, BLACK, value_desc);
		}
		if (cursor_pos[2] == 0) {
			pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_SHOW_BOOT_SCREEN_1));
			pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_SHOW_BOOT_SCREEN_2));
		}
		if (cursor_pos[2] == 1) {
			// Selected			
			pp2d_draw_text(8, 184, 0.60, 0.60f, WHITE, "Press  to change rom location folder.");
		}
	} else if (subscreenmode == SUBSCREEN_MODE_NTR) {
		pp2d_draw_texture(shoulderLtex, 0, LshoulderYpos);
		pp2d_draw_texture(shoulderRtex, 248, RshoulderYpos);
		pp2d_draw_text(17, LshoulderYpos+4, 0.50, 0.50, BLACK, Lshouldertext);
		pp2d_draw_text(252, RshoulderYpos+4, 0.50, 0.50, BLACK, Rshouldertext);

		const char *rainbowledvaluetext = (settings.twl.rainbowled ? "On" : "Off");
		const char *cpuspeedvaluetext = (settings.twl.cpuspeed ? "133mhz (TWL)" : "67mhz (NTR)");
		const char *soundfreqvaluetext = (settings.twl.soundfreq ? "47.61 kHz" : "32.73 kHz");
		const char *enablesdvaluetext = (settings.twl.enablesd ? "On" : "Off");
		const char *resetslot1valuetext = (settings.twl.resetslot1 ? "On" : "Off");
		const char *loadingscreenvaluetext = (settings.twl.loadingscreen ? "On" : "Off");

		const char *autoupdatevaluetext;
		switch (settings.ui.autoupdate) {
			case 0:
			default:
				autoupdatevaluetext = "Off";
				break;
			case 1:
				autoupdatevaluetext = "Release";
				break;
			case 2:
				autoupdatevaluetext = "Unofficial";
				break;
		}

		const char *bootstrapfilevaluetext;
		switch (settings.twl.bootstrapfile) {
			case 0:
			default:
				bootstrapfilevaluetext = "Release";
				break;
			case 1:
				bootstrapfilevaluetext = "Unofficial";
				break;
			case 2:
				bootstrapfilevaluetext = "Old";
				break;
		}

		const char *consolevaluetext;
		switch (settings.twl.console) {
			case 0:
			default:
				consolevaluetext = "Off";
				break;
			case 1:
				consolevaluetext = "On";
				break;
			case 2:
				consolevaluetext = "On (Debug)";
				break;
		}

		title = TR(STR_SETTINGS_NTR);
		int Ypos = 40;
		if (cursor_pos[SUBSCREEN_MODE_NTR] == 0) {			
			pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_FLASHCARD_SELECT_1));
			pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_FLASHCARD_SELECT_2));
			pp2d_draw_wtext(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), TR(STR_SETTINGS_FLASHCARD_SELECT));
		} else {
			pp2d_draw_wtext(Xpos, Ypos, 0.55, 0.55, WHITE, TR(STR_SETTINGS_FLASHCARD_SELECT));
		}

		Ypos += 12;
		if (cursor_pos[SUBSCREEN_MODE_NTR] == 1) {
			pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_RAINBOW_LED_1));
			pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_RAINBOW_LED_2));
			pp2d_draw_wtext(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), TR(STR_SETTINGS_RAINBOW_LED));
			pp2d_draw_text(XposValue, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), rainbowledvaluetext);
		} else {
			pp2d_draw_wtext(Xpos, Ypos, 0.55, 0.55, WHITE, TR(STR_SETTINGS_RAINBOW_LED));
			pp2d_draw_text(XposValue, Ypos, 0.55, 0.55, WHITE, rainbowledvaluetext);
		}

		Ypos += 12;
		if (cursor_pos[SUBSCREEN_MODE_NTR] == 2) {
			pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_ARM9_CPU_SPEED_1));
			pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_ARM9_CPU_SPEED_2));
			pp2d_draw_wtext(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), TR(STR_SETTINGS_ARM9_CPU_SPEED));
			pp2d_draw_text(XposValue, Ypos, 0.45, 0.55, SET_ALPHA(color_data->color, 255), cpuspeedvaluetext);
		} else {
			pp2d_draw_wtext(Xpos, Ypos, 0.55, 0.55, WHITE, TR(STR_SETTINGS_ARM9_CPU_SPEED));
			pp2d_draw_text(XposValue, Ypos, 0.45, 0.55, WHITE, cpuspeedvaluetext);
		}

		Ypos += 12;
		if (cursor_pos[SUBSCREEN_MODE_NTR] == 3) {
			pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_SOUND_FREQ_1));
			pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_SOUND_FREQ_2));
			if(language==5) pp2d_draw_wtext(Xpos, Ypos, 0.45, 0.55, SET_ALPHA(color_data->color, 255), TR(STR_SETTINGS_SOUND_FREQ));
			else pp2d_draw_wtext(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), TR(STR_SETTINGS_SOUND_FREQ));
			pp2d_draw_text(XposValue, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), soundfreqvaluetext);
		} else {
			if(language==5) pp2d_draw_wtext(Xpos, Ypos, 0.45, 0.55, WHITE, TR(STR_SETTINGS_SOUND_FREQ));
			else pp2d_draw_wtext(Xpos, Ypos, 0.55, 0.55, WHITE, TR(STR_SETTINGS_SOUND_FREQ));
			pp2d_draw_text(XposValue, Ypos, 0.55, 0.55, WHITE, soundfreqvaluetext);
		}

		Ypos += 12;
		if (cursor_pos[SUBSCREEN_MODE_NTR] == 4) {
			pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_ENABLE_SD_1));
			pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_ENABLE_SD_2));
			pp2d_draw_wtext(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), TR(STR_SETTINGS_ENABLE_SD));
			pp2d_draw_text(XposValue, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), enablesdvaluetext);
		} else {
			pp2d_draw_wtext(Xpos, Ypos, 0.55, 0.55, WHITE, TR(STR_SETTINGS_ENABLE_SD));
			pp2d_draw_text(XposValue, Ypos, 0.55, 0.55, WHITE, enablesdvaluetext);
		}

		Ypos += 12;
		if (cursor_pos[SUBSCREEN_MODE_NTR] == 5) {
			pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_RESET_SLOT_1_1));
			pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_RESET_SLOT_1_2));
			pp2d_draw_wtext(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), TR(STR_SETTINGS_RESET_SLOT_1));
			pp2d_draw_text(XposValue, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), resetslot1valuetext);
		} else {
			pp2d_draw_wtext(Xpos, Ypos, 0.55, 0.55, WHITE, TR(STR_SETTINGS_RESET_SLOT_1));
			pp2d_draw_text(XposValue, Ypos, 0.55, 0.55, WHITE, resetslot1valuetext);
		}
		
		Ypos += 12;
		if (cursor_pos[SUBSCREEN_MODE_NTR] == 6) {
			pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_BOOTSTRAP_LOADING_SCREEN_1));
			pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_BOOTSTRAP_LOADING_SCREEN_2));
			pp2d_draw_wtext(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), TR(STR_SETTINGS_BOOTSTRAP_LOADING_SCREEN));
			pp2d_draw_text(XposValue, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), loadingscreenvaluetext);
		} else {
			pp2d_draw_wtext(Xpos, Ypos, 0.55, 0.55, WHITE, TR(STR_SETTINGS_BOOTSTRAP_LOADING_SCREEN));
			pp2d_draw_text(XposValue, Ypos, 0.55, 0.55, WHITE, loadingscreenvaluetext);
		}

		Ypos += 12;
		if (cursor_pos[SUBSCREEN_MODE_NTR] == 7) {
			pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_CONSOLE_OUTPUT_1));
			pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_CONSOLE_OUTPUT_2));
			pp2d_draw_wtext(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), TR(STR_SETTINGS_CONSOLE_OUTPUT));
			pp2d_draw_text(XposValue, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), consolevaluetext);
		} else {
			pp2d_draw_wtext(Xpos, Ypos, 0.55, 0.55, WHITE, TR(STR_SETTINGS_CONSOLE_OUTPUT));
			pp2d_draw_text(XposValue, Ypos, 0.55, 0.55, WHITE, consolevaluetext);
		}

		Ypos += 12;
		if (cursor_pos[SUBSCREEN_MODE_NTR] == 8) {
			pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_AUTOUPDATE_BOOTSTRAP_1));
			pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_AUTOUPDATE_BOOTSTRAP_2));
			pp2d_draw_wtext(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), TR(STR_SETTINGS_AUTOUPDATE_BOOTSTRAP));
			pp2d_draw_text(XposValue, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), autoupdatevaluetext);
		} else {
			pp2d_draw_wtext(Xpos, Ypos, 0.55, 0.55, WHITE, TR(STR_SETTINGS_AUTOUPDATE_BOOTSTRAP));
			pp2d_draw_text(XposValue, Ypos, 0.55, 0.55, WHITE, autoupdatevaluetext);
		}

		Ypos += 12;
		if (cursor_pos[SUBSCREEN_MODE_NTR] == 9) {
			pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_BOOTSTRAP_1));
			pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_DESCRIPTION_BOOTSTRAP_2));
			pp2d_draw_wtext(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), TR(STR_SETTINGS_BOOTSTRAP));
			pp2d_draw_text(XposValue, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), bootstrapfilevaluetext);
		} else {
			pp2d_draw_wtext(Xpos, Ypos, 0.55, 0.55, WHITE, TR(STR_SETTINGS_BOOTSTRAP));
			pp2d_draw_text(XposValue, Ypos, 0.55, 0.55, WHITE, bootstrapfilevaluetext);
		}

	} else if (subscreenmode == SUBSCREEN_MODE_FLASH_CARD) {
		// Flash card options.
		static const char *const flash_card_options[][6] = {
			{"DSTT", "R4i Gold", "R4i-SDHC (Non-v1.4.x version) (www.r4i-sdhc.com)",
				"R4 SDHC Dual-Core", "R4 SDHC Upgrade", "SuperCard DSONE"},
			{"Original R4", "M3 Simply", " ", " ", " ", " "},
			{"R4iDSN", "R4i Gold RTS", " ", " ", " ", " "},
			{"Acekard 2(i)", "Galaxy Eagle", "M3DS Real", " ", " ", " "},
			{"Acekard RPG", " ", " ", " ", " ", " "},
			{"Ace 3DS+", "Gateway Blue Card", "R4iTT", " ", " ", " "},
			{"SuperCard DSTWO", " ", " ", " ", " ", " "},
		};

		if (settings.twl.flashcard < 0 || settings.twl.flashcard > 6) {
			settings.twl.flashcard = 0;
		}
		const char *const *fctext = flash_card_options[settings.twl.flashcard];
		title = TR(STR_SETTINGS_FLASHCARD_SELECT);
		int Ypos = 40;
		for (int i = 0; i < 6; i++, Ypos += 12) {
			pp2d_draw_text(Xpos, Ypos, 0.45, 0.45, WHITE, fctext[i]);
		}
		pp2d_draw_wtext(8, 184, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_LEFTRIGHT_PICK));
		pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_AB_SAVE_RETURN));
	} else if (subscreenmode == SUBSCREEN_MODE_SUB_THEME) {
		if (settings.ui.theme == THEME_DSIMENU) {
			title = TR(STR_SETTINGS_SUBTHEME_DSi);
			pp2d_draw_wtext(Xpos, 40, 0.55, 0.55, SET_ALPHA(color_data->color, 255), TR(STR_SETTINGS_NO_SUB_THEMES));
		} else if (settings.ui.theme == THEME_R4) {
			title = TR(STR_SETTINGS_SUBTHEME_R4);
			int Ypos = 30;
			if (settings.ui.subtheme == 0) {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), "theme01");
			} else {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, WHITE, "theme01");
			}
			
			Ypos += 12;
			if (settings.ui.subtheme == 1) {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), "theme02");
			} else {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, WHITE, "theme02");
			}

			Ypos += 12;
			if (settings.ui.subtheme == 2) {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), "theme03");
			} else {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, WHITE, "theme03");
			}

			Ypos += 12;
			if (settings.ui.subtheme == 3) {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), "theme04");
			} else {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, WHITE, "theme04");
			}

			Ypos += 12;
			if (settings.ui.subtheme == 4) {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), "theme05");
			} else {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, WHITE, "theme05");
			}

			Ypos += 12;
			if (settings.ui.subtheme == 5) {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), "theme06");
			} else {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, WHITE, "theme06");
			}

			Ypos += 12;
			if (settings.ui.subtheme == 6) {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), "theme07");
			} else {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, WHITE, "theme07");
			}

			Ypos += 12;
			if (settings.ui.subtheme == 7) {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), "theme08");
			} else {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, WHITE, "theme08");
			}

			Ypos += 12;
			if (settings.ui.subtheme == 8) {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), "theme09");
			} else {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, WHITE, "theme09");
			}

			Ypos += 12;
			if (settings.ui.subtheme == 9) {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), "theme10");
			} else {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, WHITE, "theme10");
			}

			Ypos += 12;
			if (settings.ui.subtheme == 10) {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), "theme11");
			} else {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, WHITE, "theme11");
			}

			Ypos += 12;
			if (settings.ui.subtheme == 11) {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), "theme12");
			} else {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, WHITE, "theme12");
			}
		} else if (settings.ui.theme == THEME_AKMENU) {
			title = TR(STR_SETTINGS_SUBTHEME_WOOD);
			int Ypos = 40;
			if (settings.ui.subtheme == 0) {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), "GBATemp");
			} else {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, WHITE, "GBATemp");
			}
			
			Ypos += 12;
			if (settings.ui.subtheme == 1) {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), "Acekard black");
			} else {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, WHITE, "Acekard black");
			}
			Ypos += 12;
			if (settings.ui.subtheme == 2) {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), "akaio");
			} else {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, WHITE, "akaio");
			}
			Ypos += 12;
			if (settings.ui.subtheme == 3) {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, SET_ALPHA(color_data->color, 255), "DSTWO");
			} else {
				pp2d_draw_text(Xpos, Ypos, 0.55, 0.55, WHITE, "DSTWO");
			}
		}
		pp2d_draw_wtext(8, 198, 0.60, 0.60f, WHITE, TR(STR_SETTINGS_AB_SAVE_RETURN));
	}else if (subscreenmode == SUBSCREEN_MODE_CHANGE_ROM_PATH) {
		title = L"Rom path location";
	
		char printedROMpath[256];
		snprintf (printedROMpath, sizeof(printedROMpath), "SD:/%s", settings.ui.romfolder.c_str());

		char printedFCROMpath[256];
		snprintf (printedFCROMpath, sizeof(printedFCROMpath), "SD:/%s", settings.ui.fcromfolder.c_str());

		if (cursor_pos[SUBSCREEN_MODE_CHANGE_ROM_PATH] == 0){
			// Selected SD
			pp2d_draw_text(24, 40, 0.55, 0.55, SET_ALPHA(color_data->color, 255), "SD ROM location:");
			pp2d_draw_text(30, 52, 0.55, 0.55, SET_ALPHA(color_data->color, 255), printedROMpath);
			
			// Unselected Flashcard
			pp2d_draw_text(24, 66, 0.55, 0.55, WHITE, "Flashcard INI location:");
			pp2d_draw_text(30, 78, 0.55, 0.55, WHITE, printedFCROMpath);
			
		}else if (cursor_pos[SUBSCREEN_MODE_CHANGE_ROM_PATH] == 1){
			// Unselected SD
			pp2d_draw_text(24, 40, 0.55, 0.55, WHITE, "SD ROM location:");
			pp2d_draw_text(30, 52, 0.55, 0.55, WHITE, printedROMpath);
			
			// Selected Flashcard
			pp2d_draw_text(24, 66, 0.55, 0.55, SET_ALPHA(color_data->color, 255), "Flashcard INI location:");
			pp2d_draw_text(30, 78, 0.55, 0.55, SET_ALPHA(color_data->color, 255), printedFCROMpath);
			
		}		
		pp2d_draw_text(8, 184, 0.60, 0.60, WHITE, ": Change path");
		pp2d_draw_text(8, 198, 0.60, 0.60, WHITE, ": Return");
	} else if (subscreenmode == SUBSCREEN_MODE_TWLNAND_NOT_FOUND) {
		if(!isDemo) {
			const wchar_t *home_text = TR(STR_RETURN_TO_HOME_MENU);
			const int home_width = 144+16;
			const int home_x = (320-home_width)/2;
			pp2d_draw_texture(whomeicontex, home_x, 221); // Draw HOME icon
			pp2d_draw_wtext(home_x+20, 222, 0.50, 0.50, WHITE, home_text);
		}

		title = L"An error has occurred.";

		if (TWLNANDnotfound_msg == 0) {
			twlnand_msg =
				"TWLNAND-side (part 1) has not been installed.\n"
				"Please install the TWLNAND-side (part 1) CIA:\n"
				"\n"
				"sd:/_nds/twloader/cias/TWLoader - TWLNAND side.cia";
		} else if (TWLNANDnotfound_msg == 1) {
			twlnand_msg =
				"TWLNAND-side (part 2) has not been installed.\n"
				"Please install the TWLNAND-side (part 2) CIA:\n"
				"\n"
				"sd:/_nds/twloader/cias/\n"
				"TWLoader - TWLNAND side (part 2).cia";
		}
		pp2d_draw_text(16, 40, 0.4, 0.4, WHITE, twlnand_msg);
	}
	pp2d_draw_wtext(2, 2, 0.75, 0.75, WHITE, title);
}

/**
 * Move the cursor if necessary.
 * @param hDown Key value from hidKeysDown().
 * @return True if the bottom screen needs to be updated.
 */
bool settingsMoveCursor(u32 hDown)
{
	touchPosition touch;
	hidTouchRead(&touch);

	Lshouldertext = "Previous";
	Rshouldertext = "Next";

	if (hDown == 0) {
		// Nothing to do here.
		return false;
	}

	// Sound effect to play.
	sound *sfx = NULL;

	if (subscreenmode == SUBSCREEN_MODE_SUB_THEME) {
		if (hDown & KEY_UP) {
			settings.ui.subtheme--;
			sfx = sfx_select;
		} else if (hDown & KEY_DOWN) {
			settings.ui.subtheme++;
			sfx = sfx_select;
		} else if (hDown & (KEY_A | KEY_B)) {
			subscreenmode = SUBSCREEN_MODE_FRONTEND;
			sfx = sfx_select;
		}
		if (settings.ui.theme == THEME_DSIMENU) {
			settings.ui.subtheme = 0;
		} else if (settings.ui.theme == THEME_R4) {
			if (settings.ui.subtheme < 0)
				settings.ui.subtheme = 11;
			else if (settings.ui.subtheme > 11)
				settings.ui.subtheme = 0;
		} else if (settings.ui.theme == THEME_AKMENU) {
			if (settings.ui.subtheme < 0)
				settings.ui.subtheme = 3;
			else if (settings.ui.subtheme > 3)
				settings.ui.subtheme = 0;
		}
	} else if (subscreenmode == SUBSCREEN_MODE_FLASH_CARD) {
		if (hDown & KEY_LEFT && settings.twl.flashcard > 0) {
			settings.twl.flashcard--; // Flashcard
			sfx = sfx_select;
		} else if (hDown & KEY_RIGHT && settings.twl.flashcard < 6) {
			settings.twl.flashcard++; // Flashcard
			sfx = sfx_select;
		} else if (hDown & (KEY_A | KEY_B)) {
			subscreenmode = SUBSCREEN_MODE_NTR;
			sfx = sfx_select;
		}
	} else if (subscreenmode == SUBSCREEN_MODE_NTR) {
		if (hDown & (KEY_A | KEY_LEFT | KEY_RIGHT)) {
			switch (cursor_pos[SUBSCREEN_MODE_NTR]) {
				case 0:
				default:
					// Top item: Only listen to 'A'.
					if (hDown & KEY_A) {
						subscreenmode = SUBSCREEN_MODE_FLASH_CARD;
					} else {
						// Ignore this key.
						return false;
					}
					break;
				case 1:	// Rainbow LED
					settings.twl.rainbowled = !settings.twl.rainbowled;
					break;
				case 2:	// CPU speed
					settings.twl.cpuspeed = !settings.twl.cpuspeed;
					break;
				case 3:	// Sound/Microphone frequency
					settings.twl.soundfreq = !settings.twl.soundfreq;
					break;
				case 4:	// SD card access for Slot-1
					settings.twl.enablesd = !settings.twl.enablesd;
					break;
				case 5:	// Reset Slot-1
					settings.twl.resetslot1 = !settings.twl.resetslot1;
					break;
				case 6:	// Bootstrap loading screen
					settings.twl.loadingscreen = !settings.twl.loadingscreen;
					break;
				case 7:	// Console output
					if (hDown & (KEY_A | KEY_RIGHT)) {
						settings.twl.console++;
						if (settings.twl.console > 2) {
							settings.twl.console = 0;
						}
					} else if (hDown & KEY_LEFT) {
						settings.twl.console--;
						if (settings.twl.console < 0) {
							settings.twl.console = 2;
						}
					}
					break;
				case 8:	// Enable or disable autoupdate
					if (hDown & (KEY_A | KEY_RIGHT)) {
						settings.ui.autoupdate++;
						if (settings.ui.autoupdate > 1) {
							settings.ui.autoupdate = 0;
						}
					} else if (hDown & KEY_LEFT) {
						settings.ui.autoupdate--;
						if (settings.ui.autoupdate < 0) {
							settings.ui.autoupdate = 1;
						}
					}
					break;
				case 9: // Bootstrap version
					if (hDown & (KEY_A | KEY_RIGHT)) {
						settings.twl.bootstrapfile++;
						if (settings.twl.bootstrapfile > 1) {
							settings.twl.bootstrapfile = 0;
						}
					} else if (hDown & KEY_LEFT) {
						settings.twl.bootstrapfile--;
						if (settings.twl.bootstrapfile < 0) {
							settings.twl.bootstrapfile = 1;
						}
					}
					break;					
			}
			sfx = sfx_select;
		} else if ((hDown & KEY_DOWN) && cursor_pos[SUBSCREEN_MODE_NTR] < 9) {
			cursor_pos[SUBSCREEN_MODE_NTR]++;
			sfx = sfx_select;
		} else if ((hDown & KEY_UP) && cursor_pos[SUBSCREEN_MODE_NTR] > 0) {
			cursor_pos[SUBSCREEN_MODE_NTR]--;
			sfx = sfx_select;
		} else if (hDown & KEY_L) {
			subscreenmode = SUBSCREEN_MODE_FRONTEND3;
			sfx = sfx_switch;
		} else if (hDown & KEY_R) {
			subscreenmode = SUBSCREEN_MODE_FRONTEND;
			sfx = sfx_switch;
		} else if (hDown & KEY_X) {
			if (checkWifiStatus()) {
				// Play the sound now instead of waiting.
				if (dspfirmfound && sfx_select) {
					sfx_select->stop();	// Prevent freezing
					sfx_select->play();
				}
				UpdateBootstrapRelease();
			} else {
				// Wi-Fi is not connected.
				sfx = sfx_wrong;
			}
		} else if (hDown & KEY_Y) {
			if (checkWifiStatus()) {
				// Play the sound now instead of waiting.
				if (dspfirmfound && sfx_select) {
					sfx_select->stop();	// Prevent freezing
					sfx_select->play();
				}
				UpdateBootstrapUnofficial();
			} else {
				// Wi-Fi is not connected.
				sfx = sfx_wrong;
			}
		} else if (hDown & KEY_B) {
			titleboxXmovetimer = 1;
			fadeout = true;
			sfx = sfx_back;
		}
		if(hDown & KEY_TOUCH){
			if (touch.px <= 72 && touch.py >= 220) {
				subscreenmode = SUBSCREEN_MODE_FRONTEND3;
				sfx = sfx_switch;
			}
			if (touch.px >= 248 && touch.py >= 220) {
				subscreenmode = SUBSCREEN_MODE_FRONTEND;
				sfx = sfx_switch;
			}
		}
	} else if (subscreenmode == SUBSCREEN_MODE_FRONTEND3) {
		if (hDown & (KEY_A | KEY_Y)) {
			switch (cursor_pos[SUBSCREEN_MODE_FRONTEND3]) {
				case 0:
				default:
					if (hDown & KEY_A) {
						settings.ui.showbootscreen++;
						if (settings.ui.showbootscreen > 2) {
							settings.ui.showbootscreen = 0;
						}
					} else if (hDown & KEY_Y) {
						settings.ui.showbootscreen--;
						if (settings.ui.showbootscreen < 0) {
							settings.ui.showbootscreen = 2;
						}
					}
					break;
				case 1: // Rom path
					if (hDown & KEY_A) {
						subscreenmode = SUBSCREEN_MODE_CHANGE_ROM_PATH;
					}
					break;
			}
			sfx = sfx_select;
		} else if ((hDown & KEY_DOWN) && cursor_pos[2] < 2) {
			cursor_pos[2] += 2;
			if (cursor_pos[2] > 1) cursor_pos[2] -= 2;
			sfx = sfx_select;
		} else if ((hDown & KEY_UP) && cursor_pos[2] > 0) {
			cursor_pos[2] -= 2;
			if (cursor_pos[2] < 0) cursor_pos[2] += 2;
			sfx = sfx_select;
		} else if ((hDown & KEY_RIGHT) && cursor_pos[2] < 2) {
			if (cursor_pos[2] == 0
			|| cursor_pos[2] == 2
			|| cursor_pos[2] == 4)
				cursor_pos[2]++;
			sfx = sfx_select;
		} else if ((hDown & KEY_LEFT) && cursor_pos[2] > 0) {
			if (cursor_pos[2] == 1
			|| cursor_pos[2] == 3
			|| cursor_pos[2] == 5)
				cursor_pos[2]--;
			sfx = sfx_select;
		} else 	if (hDown & KEY_L) {
			subscreenmode = SUBSCREEN_MODE_FRONTEND2;
			sfx = sfx_switch;
		} else if (hDown & KEY_R) {
			subscreenmode = SUBSCREEN_MODE_NTR;
			sfx = sfx_switch;
		} else if (hDown & KEY_B) {
			titleboxXmovetimer = 1;
			fadeout = true;
			sfx = sfx_back;
		}
		if(hDown & KEY_TOUCH){
			if (touch.px <= 72 && touch.py >= 220) {
				subscreenmode = SUBSCREEN_MODE_FRONTEND2;
				sfx = sfx_switch;
			}
			if (touch.px >= 248 && touch.py >= 220) {
				subscreenmode = SUBSCREEN_MODE_NTR;
				sfx = sfx_switch;
			}
		}
	} else if (subscreenmode == SUBSCREEN_MODE_FRONTEND2) {
		if (hDown & (KEY_A | KEY_Y)) {
			switch (cursor_pos[SUBSCREEN_MODE_FRONTEND2]) {
				case 0:	// Custom bottom image
				default:
					settings.ui.custombot = !settings.ui.custombot;
					LoadBottomImage();
					break;
				case 1:	// Enable or disable autoupdate TWLoader
					settings.ui.autoupdate_twldr = !settings.ui.autoupdate_twldr;
					break;
				case 2:
					if (!isDemo) {
						if (hDown & KEY_A) {
							settings.ui.filetype++;
							if (settings.ui.filetype > 2) {
								settings.ui.filetype = 0;
							}
						} else if (hDown & KEY_Y) {
							settings.ui.filetype--;
							if (settings.ui.filetype < 0) {
								settings.ui.filetype = 2;
							}
						}
					}
					break;
				case 3:
					if (checkWifiStatus()) {
						if (checkUpdate() == 0) {
							// Play the sound now instead of waiting.
							if (dspfirmfound && sfx_select) {
								sfx_select->stop();	// Prevent freezing
								sfx_select->play();
							}
							DownloadTWLoaderCIAs();
						}
					}
					break;
				case 4:	// Boot screen
					if (hDown & KEY_A) {
						settings.ui.bootscreen++;
						if (settings.ui.bootscreen > 4) {
							settings.ui.bootscreen = 0;
						}
					} else if (hDown & KEY_Y) {
						settings.ui.bootscreen--;
						if (settings.ui.bootscreen < 0) {
							settings.ui.bootscreen = 4;
						}
					}
					break;
				case 5:	// H&S message
					settings.ui.healthsafety = !settings.ui.healthsafety;
					break;
			}
			sfx = sfx_select;
		} else if ((hDown & KEY_DOWN) && cursor_pos[1] < 6) {
			cursor_pos[1] += 2;
			if (cursor_pos[1] > 5) cursor_pos[1] -= 2;
			sfx = sfx_select;
		} else if ((hDown & KEY_UP) && cursor_pos[1] > 0) {
			cursor_pos[1] -= 2;
			if (cursor_pos[1] < 0) cursor_pos[1] += 2;
			sfx = sfx_select;
		} else if ((hDown & KEY_RIGHT) && cursor_pos[1] < 6) {
			if (cursor_pos[1] == 0
			|| cursor_pos[1] == 2
			|| cursor_pos[1] == 4)
				cursor_pos[1]++;
			sfx = sfx_select;
		} else if ((hDown & KEY_LEFT) && cursor_pos[1] > 0) {
			if (cursor_pos[1] == 1
			|| cursor_pos[1] == 3
			|| cursor_pos[1] == 5)
				cursor_pos[1]--;
			sfx = sfx_select;
		} else 	if (hDown & KEY_L) {
			subscreenmode = SUBSCREEN_MODE_FRONTEND;
			sfx = sfx_switch;
		} else if (hDown & KEY_R) {
			subscreenmode = SUBSCREEN_MODE_FRONTEND3;
			sfx = sfx_switch;
		} else if (hDown & KEY_B) {
			titleboxXmovetimer = 1;
			fadeout = true;
			sfx = sfx_back;
		}
		if(hDown & KEY_TOUCH){
			if (touch.px <= 72 && touch.py >= 220) {
				subscreenmode = SUBSCREEN_MODE_FRONTEND;
				sfx = sfx_switch;
			}
			if (touch.px >= 248 && touch.py >= 220) {
				subscreenmode = SUBSCREEN_MODE_FRONTEND3;
				sfx = sfx_switch;
			}
		}
	} else if (subscreenmode == SUBSCREEN_MODE_FRONTEND) {
		if (hDown & (KEY_A | KEY_Y)) {
			switch (cursor_pos[SUBSCREEN_MODE_FRONTEND]) {
				case 0:	// Language
				default:
					if (hDown & KEY_A) {
						settings.ui.language++;
						if (settings.ui.language > 13) {
							settings.ui.language = -1;
						}
					} else if (hDown & KEY_Y) {
						settings.ui.language--;
						if (settings.ui.language < -1) {
							settings.ui.language = 13;
						}
					}
					langInit();
					break;
				case 1:	// Theme
					if (hDown & KEY_A) {
						settings.ui.subtheme = 0;
						settings.ui.theme++;
						if (settings.ui.theme > THEME_AKMENU) {
							settings.ui.theme = THEME_DSIMENU;
						}
					} else if (hDown & KEY_Y) {
						settings.ui.subtheme = 0;
						settings.ui.theme--;
						if (settings.ui.theme < THEME_DSIMENU) {
							settings.ui.theme = THEME_AKMENU;
						}
					}
					break;
				case 2:	// Color
					if (hDown & KEY_A) {
						settings.ui.color++;
						if (settings.ui.color > 18) {
							settings.ui.color = 0;
						}
					} else if (hDown & KEY_Y) {
						settings.ui.color--;
						if (settings.ui.color < 0) {
							settings.ui.color = 18;
						}
					}
					LoadColor();
					break;
				case 3:	// Menu color
					if (hDown & KEY_A) {
						settings.ui.menucolor++;
						if (settings.ui.menucolor > 16) {
							settings.ui.menucolor = 0;
						}
					} else if (hDown & KEY_Y) {
						settings.ui.menucolor--;
						if (settings.ui.menucolor < 0) {
							settings.ui.menucolor = 16;
						}
					}
					LoadMenuColor();
					break;
				case 4:	// Show filename
					settings.ui.filename = !settings.ui.filename;
					break;
				case 5:	// Game counter
					settings.ui.counter = !settings.ui.counter;
					break;
			}
			sfx = sfx_select;
		} else if ((hDown & KEY_START) && cursor_pos[0] == 1) {
			subscreenmode = SUBSCREEN_MODE_SUB_THEME;
		} else if ((hDown & KEY_DOWN) && cursor_pos[0] < 6) {
			cursor_pos[0] += 2;
			if (cursor_pos[0] > 5) cursor_pos[0] -= 2;
			sfx = sfx_select;
		} else if ((hDown & KEY_UP) && cursor_pos[0] > 0) {
			cursor_pos[0] -= 2;
			if (cursor_pos[0] < 0) cursor_pos[0] += 2;
			sfx = sfx_select;
		} else if ((hDown & KEY_RIGHT) && cursor_pos[0] < 6) {
			if (cursor_pos[0] == 0
			|| cursor_pos[0] == 2
			|| cursor_pos[0] == 4)
				cursor_pos[0]++;
			sfx = sfx_select;
		} else if ((hDown & KEY_LEFT) && cursor_pos[0] > 0) {
			if (cursor_pos[0] == 1
			|| cursor_pos[0] == 3
			|| cursor_pos[0] == 5)
				cursor_pos[0]--;
			sfx = sfx_select;
		} else if (hDown & KEY_L) {
			subscreenmode = SUBSCREEN_MODE_NTR;
			sfx = sfx_switch;
		} else if (hDown & KEY_R) {
			subscreenmode = SUBSCREEN_MODE_FRONTEND2;
			sfx = sfx_switch;
		} else if ((hDown & KEY_B) && !fadein) {
			titleboxXmovetimer = 1;
			fadeout = true;
			sfx = sfx_back;
		}
		if(hDown & KEY_TOUCH){
			if (touch.px <= 72 && touch.py >= 220) {
				subscreenmode = SUBSCREEN_MODE_NTR;
				sfx = sfx_switch;
			}
			if (touch.px >= 248 && touch.py >= 220) {
				subscreenmode = SUBSCREEN_MODE_FRONTEND2;
				sfx = sfx_switch;
			}
		}
	}else if (subscreenmode == SUBSCREEN_MODE_CHANGE_ROM_PATH) {
		if (hDown & KEY_UP) {
			cursor_pos[SUBSCREEN_MODE_CHANGE_ROM_PATH]--;
			if (cursor_pos[SUBSCREEN_MODE_CHANGE_ROM_PATH] < 0)
				cursor_pos[SUBSCREEN_MODE_CHANGE_ROM_PATH] = 0;			
			sfx = sfx_select;
		} else if (hDown & KEY_DOWN) {
			cursor_pos[SUBSCREEN_MODE_CHANGE_ROM_PATH]++;
			if (cursor_pos[SUBSCREEN_MODE_CHANGE_ROM_PATH] > 1)
				cursor_pos[SUBSCREEN_MODE_CHANGE_ROM_PATH] = 1;
			sfx = sfx_select;
		} else if (hDown & KEY_B) {
			subscreenmode = SUBSCREEN_MODE_FRONTEND3;
			sfx = sfx_select;
		} else if (hDown & KEY_A) {
			std::string oldPath = (cursor_pos[SUBSCREEN_MODE_CHANGE_ROM_PATH] == 0) ? settings.ui.romfolder : settings.ui.fcromfolder;
			std::wstring widestr = (cursor_pos[SUBSCREEN_MODE_CHANGE_ROM_PATH] == 0) ? 
													std::wstring(settings.ui.romfolder.begin(), settings.ui.romfolder.end()) : 
													std::wstring(settings.ui.fcromfolder.begin(), settings.ui.fcromfolder.end());
			
			const wchar_t* currentPath = widestr.c_str();
			std::string newPath = keyboardInput(currentPath);
			
			(cursor_pos[SUBSCREEN_MODE_CHANGE_ROM_PATH] == 0) ? settings.ui.romfolder = newPath : settings.ui.fcromfolder = newPath;			

			if(oldPath != newPath){						
				// Buffers for APT_DoApplicationJump().
				u8 param[0x300];
				u8 hmac[0x20];
				// Clear both buffers
				memset(param, 0, sizeof(param));
				memset(hmac, 0, sizeof(hmac));
				
				APT_PrepareToDoApplicationJump(0, 0x00040000047C4200LL, MEDIATYPE_SD);
				// Tell APT to trigger the app launch and set the status of this app to exit
				SaveSettings();
				APT_DoApplicationJump(param, sizeof(param), hmac);
			}
		}
	}

	// Do we need to play a sound effect?
	if (dspfirmfound && sfx) {
		sfx->stop();	// Prevent freezing
		sfx->play();
	}

	// Bottom screen needs to be redrawn.
	return true;
}

/**
 * Load the primary color from the configuration.
 */
void LoadColor(void) {
	static const ColorData colors[] = {
		{
			"romfs:/graphics/topbg/0-gray.png",
			"romfs:/graphics/dotcircle/0-gray.png",
			"romfs:/graphics/start_border/0-gray.png",
			(u32)RGBA8(99, 127, 127, 255)
		},
		{
			"romfs:/graphics/topbg/1-brown.png",
			"romfs:/graphics/dotcircle/1-brown.png",
			"romfs:/graphics/start_border/1-brown.png",
			(u32)RGBA8(139, 99, 0, 255)
		},
		{
			"romfs:/graphics/topbg/2-red.png",
			"romfs:/graphics/dotcircle/2-red.png",
			"romfs:/graphics/start_border/2-red.png",
			(u32)RGBA8(255, 0, 0, 255)
		},
		{
			"romfs:/graphics/topbg/3-pink.png",
			"romfs:/graphics/dotcircle/3-pink.png",
			"romfs:/graphics/start_border/3-pink.png",
			(u32)RGBA8(255, 127, 127, 255)
		},
		{
			"romfs:/graphics/topbg/4-orange.png",
			"romfs:/graphics/dotcircle/4-orange.png",
			"romfs:/graphics/start_border/4-orange.png",
			(u32)RGBA8(223, 63, 0, 255)
		},
		{
			"romfs:/graphics/topbg/5-yellow.png",
			"romfs:/graphics/dotcircle/5-yellow.png",
			"romfs:/graphics/start_border/5-yellow.png",
			(u32)RGBA8(215, 215, 0, 255)
		},
		{
			"romfs:/graphics/topbg/6-yellowgreen.png",
			"romfs:/graphics/dotcircle/6-yellowgreen.png",
			"romfs:/graphics/start_border/6-yellowgreen.png",
			(u32)RGBA8(215, 255, 0, 255)
		},
		{
			"romfs:/graphics/topbg/7-green1.png",
			"romfs:/graphics/dotcircle/7-green1.png",
			"romfs:/graphics/start_border/7-green1.png",
			(u32)RGBA8(0, 255, 0, 255)
		},
		{
			"romfs:/graphics/topbg/8-green2.png",
			"romfs:/graphics/dotcircle/8-green2.png",
			"romfs:/graphics/start_border/8-green2.png",
			(u32)RGBA8(63, 255, 63, 255)
		},
		{
			"romfs:/graphics/topbg/9-lightgreen.png",
			"romfs:/graphics/dotcircle/9-lightgreen.png",
			"romfs:/graphics/start_border/9-lightgreen.png",
			(u32)RGBA8(31, 231, 31, 255)
		},
		{
			"romfs:/graphics/topbg/10-skyblue.png",
			"romfs:/graphics/dotcircle/10-skyblue.png",
			"romfs:/graphics/start_border/10-skyblue.png",
			(u32)RGBA8(0, 63, 255, 255)
		},
		{
			"romfs:/graphics/topbg/11-lightblue.png",
			"romfs:/graphics/dotcircle/11-lightblue.png",
			"romfs:/graphics/start_border/11-lightblue.png",
			(u32)RGBA8(63, 63, 255, 255)
		},
		{
			"romfs:/graphics/topbg/12-blue.png",
			"romfs:/graphics/dotcircle/12-blue.png",
			"romfs:/graphics/start_border/12-blue.png",
			(u32)RGBA8(0, 0, 255, 255)
		},
		{
			"romfs:/graphics/topbg/13-violet.png",
			"romfs:/graphics/dotcircle/13-violet.png",
			"romfs:/graphics/start_border/13-violet.png",
			(u32)RGBA8(127, 0, 255, 255)
		},
		{
			"romfs:/graphics/topbg/14-purple.png",
			"romfs:/graphics/dotcircle/14-purple.png",
			"romfs:/graphics/start_border/14-purple.png",
			(u32)RGBA8(255, 0, 255, 255)
		},
		{
			"romfs:/graphics/topbg/15-fuchsia.png",
			"romfs:/graphics/dotcircle/15-fuchsia.png",
			"romfs:/graphics/start_border/15-fuchsia.png",
			(u32)RGBA8(255, 0, 127, 255)
		},
		{
			"romfs:/graphics/topbg/16-red_and_blue.png",
			"romfs:/graphics/dotcircle/16-red_and_blue.png",
			"romfs:/graphics/start_border/16-red_and_blue.png",
			(u32)RGBA8(255, 0, 255, 255)
		},
		{
			"romfs:/graphics/topbg/17-green_and_yellow.png",
			"romfs:/graphics/dotcircle/17-green_and_yellow.png",
			"romfs:/graphics/start_border/17-green_and_yellow.png",
			(u32)RGBA8(215, 215, 0, 255)
		},
		{
			"romfs:/graphics/topbg/18-christmas.png",
			"romfs:/graphics/dotcircle/18-christmas.png",
			"romfs:/graphics/start_border/18-christmas.png",
			(u32)RGBA8(255, 255, 0, 255)
		},
	};

	if (settings.ui.color < 0 || settings.ui.color > 18)
		settings.ui.color = 0;
	color_data = &colors[settings.ui.color];
	if (logEnabled)	LogFM("LoadColor()", "Colors load successfully");
}

/**
 * Load the menu color from the configuration.
 */
void LoadMenuColor(void) {
	static const u32 menu_colors[] = {
		(u32)WHITE,		// White
		(u32)RGBA8(63, 63, 63, 195),		// Black
		(u32)RGBA8(139, 99, 0, 195),		// Brown
		(u32)RGBA8(255, 0, 0, 195),			// Red
		(u32)RGBA8(255, 163, 163, 195),		// Pink
		(u32)RGBA8(255, 127, 0, 223),		// Orange
		(u32)RGBA8(255, 255, 0, 223),		// Yellow
		(u32)RGBA8(215, 255, 0, 223),		// Yellow-Green
		(u32)RGBA8(0, 255, 0, 223),			// Green 1
		(u32)RGBA8(95, 223, 95, 193),		// Green 2
		(u32)RGBA8(127, 231, 127, 223),		// Light Green
		(u32)RGBA8(63, 127, 255, 223),		// Sky Blue
		(u32)RGBA8(127, 127, 255, 223),		// Light Blue
		(u32)RGBA8(0, 0, 255, 195),			// Blue
		(u32)RGBA8(127, 0, 255, 195),		// Violet
		(u32)RGBA8(255, 0, 255, 195),		// Purple
		(u32)RGBA8(255, 63, 127, 195),		// Fuchsia
	};

	if (settings.ui.menucolor < 0 || settings.ui.menucolor > 16)
		settings.ui.menucolor = 0;
	menucolor = menu_colors[settings.ui.menucolor];
	if (logEnabled)	LogFM("LoadMenuColor()", "Menu color load successfully");
}

/**
 * Load the filename of the bottom screen image.
 */
void LoadBottomImage() {
	bottomloc = "romfs:/graphics/bottom.png";

	if (settings.ui.custombot == 1) {
		if( access( "sdmc:/_nds/twloader/bottom.png", F_OK ) != -1 ) {
			bottomloc = "sdmc:/_nds/twloader/bottom.png";
			if (logEnabled)	LogFM("LoadBottomImage()", "Using custom bottom image. Method load successfully.");
		} else {
			bottomloc = "romfs:/graphics/bottom.png";
			if (logEnabled)	LogFM("LoadBottomImage()", "Using default bottom image. Method load successfully.");
		}
	} else {
		if (logEnabled) LogFM("LoadBottomImage()", "Using default bottom image. No custom image found.");
	}
}

/**
 * Remove trailing slashes from a pathname, if present.
 * @param path Pathname to modify.
 */
static void RemoveTrailingSlashes(string& path)
{
	while (!path.empty() && path[path.size()-1] == '/') {
		path.resize(path.size()-1);
	}
}

/**
 * Load settings.
 */
void LoadSettings(void) {
	// UI settings.
	settings.ui.name = settingsini.GetString("FRONTEND", "NAME", "");
	settings.ui.romfolder = settingsini.GetString("FRONTEND", "ROM_FOLDER", "");
	RemoveTrailingSlashes(settings.ui.romfolder);
	settings.ui.fcromfolder = settingsini.GetString("FRONTEND", "FCROM_FOLDER", "");
	RemoveTrailingSlashes(settings.ui.fcromfolder);
	settings.ui.gbromfolder = settingsini.GetString("FRONTEND", "GBROM_FOLDER", "");
	RemoveTrailingSlashes(settings.ui.gbromfolder);
	settings.ui.pagenum = settingsini.GetInt("FRONTEND", "PAGE_NUMBER", 0);

	// Customizable UI settings.
	settings.ui.language = settingsini.GetInt("FRONTEND", "LANGUAGE", -1);
	settings.ui.theme = settingsini.GetInt("FRONTEND", "THEME", 0);
	settings.ui.subtheme = settingsini.GetInt("FRONTEND", "SUB_THEME", 0);
	settings.ui.color = settingsini.GetInt("FRONTEND", "COLOR", 0);
	settings.ui.menucolor = settingsini.GetInt("FRONTEND", "MENU_COLOR", 0);
	settings.ui.filename = settingsini.GetInt("FRONTEND", "SHOW_FILENAME", 0);
	settings.ui.topborder = settingsini.GetInt("FRONTEND", "TOP_BORDER", 1);
	settings.ui.iconsize = settingsini.GetInt("FRONTEND", "ICON_SIZE", 0);
	settings.ui.counter = settingsini.GetInt("FRONTEND", "COUNTER", 0);
	settings.ui.custombot = settingsini.GetInt("FRONTEND", "CUSTOM_BOTTOM", 0);
	settings.romselect.toplayout = settingsini.GetInt("FRONTEND", "TOP_LAYOUT", 0);
	if (!isDemo) settings.ui.filetype = settingsini.GetInt("FRONTEND", "FILETYPE", 0);
	else settings.ui.filetype = 1;
	settings.ui.showbootscreen = settingsini.GetInt("FRONTEND", "SHOW_BOOT_ANIMATION", 2);
	settings.ui.bootscreen = settingsini.GetInt("FRONTEND", "BOOT_ANIMATION", 0);
	settings.ui.healthsafety = settingsini.GetInt("FRONTEND", "HEALTH&SAFETY_MSG", 1);
	settings.ui.autoupdate = settingsini.GetInt("FRONTEND", "AUTOUPDATE", 0);
	settings.ui.autoupdate_twldr = settingsini.GetInt("FRONTEND", "AUTODOWNLOAD", 0);

	// TWL settings.
	settings.twl.rainbowled = settingsini.GetInt("TWL-MODE", "RAINBOW_LED", 0);
	settings.twl.cpuspeed = settingsini.GetInt("TWL-MODE", "TWL_CLOCK", 0);
	settings.twl.soundfreq = settingsini.GetInt("TWL-MODE", "SOUND_FREQ", 0);
	settings.twl.lockarm9scfgext = settingsini.GetInt("TWL-MODE", "LOCK_ARM9_SCFG_EXT", 0);
	settings.twl.resetslot1 = settingsini.GetInt("TWL-MODE", "RESET_SLOT1", 0);
	settings.twl.enablesd = settingsini.GetInt("TWL-MODE", "SLOT1_ENABLESD", 0);
	settings.twl.romtype = settingsini.GetInt("TWL-MODE", "ROM_TYPE", 0);
	settings.twl.forwarder = settingsini.GetInt("TWL-MODE", "FORWARDER", 0);
	settings.twl.flashcard = settingsini.GetInt("TWL-MODE", "FLASHCARD", 0);
	settings.twl.bootstrapfile = settingsini.GetInt("TWL-MODE", "BOOTSTRAP_FILE", 0);
	if (settings.twl.bootstrapfile > 1) {
		settings.twl.bootstrapfile = 0;
	}

	// TODO: Change the default to -1?
	switch (settingsini.GetInt("TWL-MODE", "DEBUG", 0)) {
		case 1:
			settings.twl.console = 2;
			break;
		case 0:
		default:
			settings.twl.console = 1;
			break;
		case -1:
			settings.twl.console = 0;
			break;
	}
	if (logEnabled)	LogFM("Settings.LoadSettings", "Settings loaded successfully");
}

/**
 * Save settings.
 */
void SaveSettings(void) {
	bool srldrsettingsFound = false;
	if (!access(srldrsettingsinipath, F_OK)) srldrsettingsFound = true;

	// UI settings.
	if (!gbarunnervalue) settingsini.SetString("FRONTEND", "ROM_FOLDER", settings.ui.romfolder);
	if (!gbarunnervalue) settingsini.SetString("FRONTEND", "FCROM_FOLDER", settings.ui.fcromfolder);
	if (!gbarunnervalue) settingsini.SetString("FRONTEND", "GBROM_FOLDER", settings.ui.gbromfolder);
	settingsini.SetInt("FRONTEND", "PAGE_NUMBER", settings.ui.pagenum);
	settingsini.SetInt("FRONTEND", "LANGUAGE", settings.ui.language);
	settingsini.SetInt("FRONTEND", "THEME", settings.ui.theme);
	settingsini.SetInt("FRONTEND", "SUB_THEME", settings.ui.subtheme);
	settingsini.SetInt("FRONTEND", "COLOR", settings.ui.color);
	settingsini.SetInt("FRONTEND", "MENU_COLOR", settings.ui.menucolor);
	settingsini.SetInt("FRONTEND", "SHOW_FILENAME", settings.ui.filename);
	settingsini.SetInt("FRONTEND", "TOP_BORDER", settings.ui.topborder);
	settingsini.SetInt("FRONTEND", "ICON_SIZE", settings.ui.iconsize);
	settingsini.SetInt("FRONTEND", "COUNTER", settings.ui.counter);
	settingsini.SetInt("FRONTEND", "CUSTOM_BOTTOM", settings.ui.custombot);
	settingsini.SetInt("FRONTEND", "TOP_LAYOUT", settings.romselect.toplayout);
	if (!isDemo) settingsini.SetInt("FRONTEND", "FILETYPE", settings.ui.filetype);
	if (dspfirmfound) settingsini.SetInt("FRONTEND", "SHOW_BOOT_ANIMATION", settings.ui.showbootscreen);
	settingsini.SetInt("FRONTEND", "BOOT_ANIMATION", settings.ui.bootscreen);
	settingsini.SetInt("FRONTEND", "HEALTH&SAFETY_MSG", settings.ui.healthsafety);
	settingsini.SetInt("FRONTEND", "AUTOUPDATE", settings.ui.autoupdate);
	settingsini.SetInt("FRONTEND", "AUTODOWNLOAD", settings.ui.autoupdate_twldr);

	// TWL settings.
	settingsini.SetInt("TWL-MODE", "RAINBOW_LED", settings.twl.rainbowled);
	settingsini.SetInt("TWL-MODE", "TWL_CLOCK", settings.twl.cpuspeed);
	settingsini.SetInt("TWL-MODE", "SOUND_FREQ", settings.twl.soundfreq);
	settingsini.SetInt("TWL-MODE", "LOCK_ARM9_SCFG_EXT", settings.twl.lockarm9scfgext);
	settingsini.SetInt("TWL-MODE", "LAUNCH_SLOT1", settings.twl.launchslot1);
	settingsini.SetInt("TWL-MODE", "RESET_SLOT1", settings.twl.resetslot1);
	settingsini.SetInt("TWL-MODE", "SLOT1_ENABLESD", settings.twl.enablesd);
	settingsini.SetInt("TWL-MODE", "SLOT1_KEEPSD", keepsdvalue);
	settingsini.SetInt("TWL-MODE", "BOOTSTRAP_FILE", settings.twl.bootstrapfile);

	// TODO: Change default to 0?
	switch (settings.twl.console) {
		case 0:
			settingsini.SetInt("TWL-MODE", "DEBUG", -1);
			break;
		case 1:
		default:
			settingsini.SetInt("TWL-MODE", "DEBUG", 0);
			break;
		case 2:
			settingsini.SetInt("TWL-MODE", "DEBUG", 1);
			break;
	}

	settingsini.SetInt("TWL-MODE", "ROM_TYPE", settings.twl.romtype);
	settingsini.SetString("TWL-MODE", "HOMEBREW_ARG", homebrew_arg);
	settingsini.SetInt("TWL-MODE", "FORWARDER", settings.twl.forwarder);
	settingsini.SetInt("TWL-MODE", "FLASHCARD", settings.twl.flashcard);
	settingsini.SetInt("TWL-MODE", "GBARUNNER", gbarunnervalue);
	settingsini.SaveIniFile("sdmc:/_nds/twloader/settings.ini");

	if(srldrsettingsFound) {
		// Save some settings to SRLoader as well.
		CIniFile srldrsettingsini( srldrsettingsinipath );

		srldrsettingsini.SetInt("SRLOADER", "IS_3DS", 1);
		srldrsettingsini.SetInt("SRLOADER", "BOOTSTRAP_FILE", settings.twl.bootstrapfile);
		if(settings.ui.theme <= THEME_3DSMENU) {
			srldrsettingsini.SetInt("SRLOADER", "THEME", 0);
		} else {
			srldrsettingsini.SetInt("SRLOADER", "THEME", 1);
		}
		srldrsettingsini.SaveIniFile(srldrsettingsinipath);
	}
}
