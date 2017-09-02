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

void initTextures() {
	rectangletex = 1;
	dialogboxtex = 2;
	dboxtex_iconbox = 3;
	dboxtex_button = 4;
	dboxtex_buttonback = 5;
	twloaderlogotex = 6;
	settingslogotex = 7;
	settingslogotwltex = 8; 
	settingslogoloadertex = 9; 
	settingslogodemotex = 10; 
	slot1boxarttex = 11; 
	bnricontexnum = 12; 
	bnricontexlaunch = 13; 
	boxarttexnum = 14; 
	bnricontex[0] = 15;
	bnricontex[1] = 16;
	bnricontex[2] = 17;
	bnricontex[3] = 18;
	bnricontex[4] = 19;
	bnricontex[5] = 20;
	bnricontex[6] = 21; 
	bnricontex[7] = 22; 
	bnricontex[8] = 23; 
	bnricontex[9] = 24; 
	bnricontex[10] = 25; 
	bnricontex[11] = 26; 
	bnricontex[12] = 27; 
	bnricontex[13] = 28; 
	bnricontex[14] = 29; 
	bnricontex[15] = 30; 
	bnricontex[16] = 31; 
	bnricontex[17] = 32; 
	bnricontex[18] = 33; 
	bnricontex[19] = 34; 
	bnricontex[20] = 35;	
	boxarttex[0] = 36;
	boxarttex[1] = 37;
	boxarttex[2] = 38;
	boxarttex[3] = 39;
	boxarttex[4] = 40;
	boxarttex[5] = 41;
	boxfulltex = 42; 
	shoulderLtex = 43; 
	shoulderRtex = 44;
	voltex[0] = 45;
	voltex[1] = 46;
	voltex[2] = 47;
	voltex[3] = 48;
	voltex[4] = 49;
	voltex[5] = 50;
	batteryIcon = 51; 
	batterychrgtex = 52;
	batterytex[0] = 53;
	batterytex[1] = 54;
	batterytex[2] = 55;
	batterytex[3] = 56;
	batterytex[4] = 57;
	batterytex[5] = 58;
	cartnulltex = 59;
	cartntrtex = 60; 
	carttwltex = 61;
	cartctrtex = 62;
	bottomtex  = 63;
	scrollbartex = 64;
	buttonarrowtex = 65;
	bubbletex = 66;
	r4loadingtex = 67;
	toplogotex = 68;
	toptex = 69;
	topbgtex = 70;
	iconstex = 71;
	sdicontex = 72;
	flashcardicontex = 73;
	gbaicontex = 74;
	smallsettingsicontex = 75;
	iconnulltex = 76;
	homeicontex = 77;
	bottomlogotex = 78;
	bipstex = 79;
	scrollwindowtex = 80;
	scrollwindowfronttex = 81;
	dotcircletex = 82;
	startbordertex = 83;
	settingsicontex = 84;
	getfcgameboxtex = 85;
	boxemptytex = 86;
	bracetex = 87;
	ndslogotex = 88;
	itex = 89;
	topotex = 90;
	bottomotex = 91;
	bigotex = 92;
	nintendotex = 93;
	hstexttex = 94;
	hstouchtex = 95;
	hstex = 96;
	wipetex = 97;
	card_icon = 98;
	whomeicontex = 99;
	setvoltex[0] = 100;
	setvoltex[1] = 101;
	setvoltex[2] = 102;
	setvoltex[3] = 103;
	setvoltex[4] = 104;
	setvoltex[5] = 105;
	setbatterychrgtex = 106;
	setbatterytex[0] = 107;
	setbatterytex[1] = 108;
	setbatterytex[2] = 109;
	setbatterytex[3] = 110;
	setbatterytex[4] = 111;
	setbatterytex[5] = 112;
	dsboottex = 113;
	dsiboottex = 114;
	invdsboottex = 115;
	invdsiboottex = 116;
	dshstex = 117;
	dsihstex = 118;
	disabledtex = 119;
	settingstex = 120;
	settingslogooadertex = 121;	
}