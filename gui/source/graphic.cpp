#include "graphic.h"

void pp2d_draw_texture_scale_blend(size_t id, int x, int y, float scaleX, float scaleY, u32 color)
{
	pp2d_texture_select(id, x, y);
	pp2d_texture_blend(color);
	pp2d_texture_scale(scaleX, scaleY);
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
size_t bnricontexlaunch = 13; 
size_t boxarttexnum = 14; 
size_t bnricontex[21] = {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
size_t boxarttex[7] = {36, 37, 38, 39, 40, 41, 42};
size_t boxtex = 43; 
size_t shoulderLtex = 44; 
size_t shoulderRtex = 45;
size_t voltex[6] = {46, 47, 48, 49, 50, 51};
size_t batteryIcon = 52; 
size_t batterychrgtex = 53;
size_t batterytex[6] = {54, 55, 56, 57, 58, 59};
size_t cartnulltex = 60;
size_t cartntrtex = 61; 
size_t carttwltex = 62;
size_t cartctrtex = 63;
size_t bottomtex  = 64;
size_t scrollbartex = 65;
size_t buttonarrowtex = 66;
size_t bubbletex = 67;
size_t r4loadingtex = 68;
size_t toplogotex = 69;
size_t toptex = 70;
size_t topbgtex = 71;
size_t iconstex = 72;
size_t sdicontex = 73;
size_t flashcardicontex = 74;
size_t gbaicontex = 75;
size_t smallsettingsicontex = 76;
size_t iconnulltex = 77;
size_t homeicontex = 78;
size_t bottomlogotex = 79;
size_t bipstex = 80;
size_t scrollwindowtex = 81;
size_t scrollwindowfronttex = 82;
size_t dotcircletex = 83;
size_t startbordertex = 84;
size_t settingsicontex = 85;
size_t getfcgameboxtex = 86;
size_t _3dsbottopbartex = 87;
size_t bracetex = 88;
size_t ndslogotex = 89;
size_t itex = 90;
size_t topotex = 91;
size_t bottomotex = 92;
size_t bigotex = 93;
size_t nintendotex = 94;
size_t hstexttex = 95;
size_t hstouchtex = 96;
size_t hstex = 97;
size_t wipetex = 98;
size_t card_icon = 99;
size_t whomeicontex = 100;
size_t setvoltex[6] = {101, 102, 103, 104, 105, 106};
size_t setbatterychrgtex = 107;
size_t setbatterytex[6] = {108, 109, 110, 111, 112, 113};
size_t dsboottex = 114;
size_t dsiboottex = 115;
size_t invdsboottex = 116;
size_t invdsiboottex = 117;
size_t dshstex = 118;
size_t dsihstex = 119;
size_t disabledtex = 120;
size_t settingstex = 121;
size_t settingslogooadertex = 122;
size_t new_tex = 123;
size_t gbctex = 124;
size_t anniversarytex = 125;
size_t _3dsbotbotbartex = 126;
size_t _3dsbotbotbarbuttex = 127;
size_t invdshstex = 128;
size_t regularloadtex = 129;
size_t pongloadtex = 130;
size_t tictactoeloadtex = 131;
size_t _3dsshouldertex = 132;
