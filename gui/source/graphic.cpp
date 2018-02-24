#include "graphic.h"

void pp2d_draw_texture_scale_blend(size_t id, int x, int y, float scaleX, float scaleY, u32 color)
{
	pp2d_texture_select(id, x, y);
	pp2d_texture_blend(color);
	pp2d_texture_scale(scaleX, scaleY);
	pp2d_texture_draw();		
}

void pp2d_draw_texture_scale_flip(size_t id, int x, int y, float scaleX, float scaleY, flipType fliptype)
{
	pp2d_texture_select(id, x, y);
	pp2d_texture_scale(scaleX, scaleY);
	pp2d_texture_flip(fliptype);
	pp2d_texture_draw();		
}

void pp2d_draw_texture_flip_blend(size_t id, int x, int y, flipType fliptype, u32 color)
{
	pp2d_texture_select(id, x, y);
	pp2d_texture_blend(color);
	pp2d_texture_flip(fliptype);
	pp2d_texture_draw();
}

void pp2d_draw_texture_part_blend(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, u32 color)
{
	pp2d_texture_select_part(id, x, y, xbegin, ybegin, width, height);
	pp2d_texture_blend(color);
	pp2d_texture_draw();		
}

void pp2d_draw_texture_part_flip(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, flipType fliptype)
{
	pp2d_texture_select_part(id, x, y, xbegin, ybegin, width, height);
	pp2d_texture_flip(fliptype);
	pp2d_texture_draw();
}

void pp2d_draw_texture_part_scale(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, float scaleX, float scaleY)
{
	pp2d_texture_select_part(id, x, y, xbegin, ybegin, width, height);
	pp2d_texture_scale(scaleX, scaleY);
	pp2d_texture_draw();
}

void pp2d_draw_texture_part_scale_flip(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, float scaleX, float scaleY, flipType fliptype)
{
	pp2d_texture_select_part(id, x, y, xbegin, ybegin, width, height);
	pp2d_texture_scale(scaleX, scaleY);
	pp2d_texture_flip(fliptype);
	pp2d_texture_draw();
}

void pp2d_draw_texture_part_scale_blend(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, float scaleX, float scaleY, u32 color)
{
	pp2d_texture_select_part(id, x, y, xbegin, ybegin, width, height);
	pp2d_texture_blend(color);
	pp2d_texture_scale(scaleX, scaleY);
	pp2d_texture_draw();		
}

void pp2d_draw_texture_rotate_flip_blend(size_t id, int x, int y, float angle, flipType fliptype, u32 color)
{
	pp2d_texture_select(id, x, y);
	pp2d_texture_rotate(angle);
	pp2d_texture_flip(fliptype);
	pp2d_texture_blend(color);
	pp2d_texture_draw();
}

size_t rectangletex = 1;
size_t dialogboxtex = 2;
size_t dboxtex_iconbox = 3;
size_t dboxtex_button = 4;
size_t dboxtex_buttonback = 5;
size_t twloaderlogotex = 6;
size_t settingslogotex = 7;
size_t settingslogotwltex = 8; 
size_t settingslogoloadertex = 9; 
size_t settingslogodemotex = 10; 
size_t slot1boxarttex = 11; 
size_t bnricontexnum = 12;
size_t boxarttexnum = 14; 
size_t bnricontex[64] = {15, 16, 17, 18, 19, 20, 21, 22,
			23, 24, 25, 26, 27, 28, 29, 30,
			31, 32, 33, 34, 35, 36, 37, 38,
			39, 40, 41, 42, 43, 44, 45, 46,
			47, 48, 49, 50, 51, 52, 53, 54,
			55, 56, 57, 58, 59, 60, 61, 62,
			63, 64, 65, 66, 67, 68, 69, 70,
			71, 72, 73, 74, 75, 76, 77, 78};
size_t boxarttex[7] = {80, 81, 82, 83, 84, 85, 86};
size_t boxtex = 87; 
size_t shouldertex = 88;
size_t voltex = 90;
size_t batteryIcon = 96;
size_t batterytex = 97;
size_t cartnulltex = 104;
size_t cartntrtex = 105; 
size_t carttwltex = 106;
size_t cartctrtex = 107;
size_t bottomtex  = 108;
size_t scrollbartex = 109;
size_t buttonarrowtex = 110;
size_t bubbletex = 111;
size_t r4loadingtex = 112;
size_t toplogotex = 113;
size_t toptex = 114;
size_t topbgtex = 115;
size_t iconstex = 116;
size_t sdicontex = 117;
size_t flashcardicontex = 118;
size_t gbaicontex = 119;
size_t smallsettingsicontex = 120;
size_t iconnulltex = 121;
size_t homeicontex = 122;
size_t bottomlogotex = 123;
size_t bipstex = 124;
size_t scrollwindowtex = 125;
size_t scrollwindowfronttex = 126;
size_t dotcircletex = 127;
size_t startbordertex = 128;
size_t settingsicontex = 129;
size_t getfcgameboxtex = 130;
size_t _3dsbottopbartex = 131;
size_t bracetex = 132;
size_t ndslogotex = 133;
size_t itex = 134;
size_t topotex = 135;
size_t bottomotex = 136;
size_t bigotex = 137;
size_t nintendotex = 138;
size_t hstexttex = 139;
size_t hstouchtex = 140;
size_t hstex = 141;
size_t wipetex = 142;
size_t card_icon = 143;
size_t whomeicontex = 144;
size_t setvoltex = 145;
size_t setbatterytex = 151;
size_t dsboottex = 157;
size_t dsiboottex = 158;
size_t invdsboottex = 159;
size_t invdsiboottex = 160;
size_t dshstex = 161;
size_t dsihstex = 162;
size_t disabledtex = 163;
size_t settingstex = 164;
size_t settingslogooadertex = 165;
size_t new_tex = 166;
size_t gbctex = 167;
size_t nestex = 168;
size_t anniversarytex = 169;
size_t _3dsbotbotbartex = 170;
size_t _3dsbotbotbarbuttex = 171;
size_t invdshstex = 172;
size_t regularloadtex = 173;
size_t pongloadtex = 174;
size_t tictactoeloadtex = 175;
size_t _3dsshouldertex = 176;
