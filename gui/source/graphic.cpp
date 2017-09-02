#include "graphic.h"

void pp2d_draw_texture_scale_blend(size_t id, int x, int y, float scaleX, float scaleY, u32 color)
{
	pp2d_texture_select(id, x, y);
	pp2d_texture_blend(color);
	pp2d_texture_scale(scaleX, scaleY);
	pp2d_texture_draw();		
}

void pp2d_draw_texture_part_blend(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, u32 color)
{
	pp2d_texture_select_part(id, x, y, xbegin, ybegin, width, height);
	pp2d_texture_blend(color);
	pp2d_texture_draw();		
}

void pp2d_draw_texture_part_scale(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, float scaleX, float scaleY)
{
	pp2d_texture_select_part(id, x, y, xbegin, ybegin, width, height);
	pp2d_texture_scale(scaleX, scaleY);
	pp2d_texture_draw();
}

void pp2d_draw_texture_part_scale_blend(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, float scaleX, float scaleY, u32 color)
{
	pp2d_texture_select_part(id, x, y, xbegin, ybegin, width, height);
	pp2d_texture_blend(color);
	pp2d_texture_scale(scaleX, scaleY);
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
size_t boxarttex[6] = {36, 37, 38, 39, 40, 41};
size_t boxfulltex = 42; 
size_t shoulderLtex = 43; 
size_t shoulderRtex = 44;
size_t voltex[6] = {45, 46, 47, 48, 49, 50};
size_t batteryIcon = 51; 
size_t batterychrgtex = 52;
size_t batterytex[6] = {53, 54, 55, 56, 57, 58};
size_t cartnulltex = 59;
size_t cartntrtex = 60; 
size_t carttwltex = 61;
size_t cartctrtex = 62;
size_t bottomtex  = 63;
size_t scrollbartex = 64;
size_t buttonarrowtex = 65;
size_t bubbletex = 66;
size_t r4loadingtex = 67;
size_t toplogotex = 68;
size_t toptex = 69;
size_t topbgtex = 70;
size_t iconstex = 71;
size_t sdicontex = 72;
size_t flashcardicontex = 73;
size_t gbaicontex = 74;
size_t smallsettingsicontex = 75;
size_t iconnulltex = 76;
size_t homeicontex = 77;
size_t bottomlogotex = 78;
size_t bipstex = 79;
size_t scrollwindowtex = 80;
size_t scrollwindowfronttex = 81;
size_t dotcircletex = 82;
size_t startbordertex = 83;
size_t settingsicontex = 84;
size_t getfcgameboxtex = 85;
size_t boxemptytex = 86;
size_t bracetex = 87;
size_t ndslogotex = 88;
size_t itex = 89;
size_t topotex = 90;
size_t bottomotex = 91;
size_t bigotex = 92;
size_t nintendotex = 93;
size_t hstexttex = 94;
size_t hstouchtex = 95;
size_t hstex = 96;
size_t wipetex = 97;
size_t card_icon = 98;
size_t whomeicontex = 99;
size_t setvoltex[6] = {100, 101, 102, 103, 104, 105};
size_t setbatterychrgtex = 106;
size_t setbatterytex[6] = {107, 108, 109, 110, 111, 112};
size_t dsboottex = 113;
size_t dsiboottex = 114;
size_t invdsboottex = 115;
size_t invdsiboottex = 116;
size_t dshstex = 117;
size_t dsihstex = 118;
size_t disabledtex = 119;
size_t settingstex = 120;
size_t settingslogooadertex = 121;	