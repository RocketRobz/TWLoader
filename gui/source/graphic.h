#pragma once
#include <3ds.h>
#include <citro3d.h>

// Textures
#define rectangletex 1
#define dialogboxtex 2
#define dboxtex_iconbox 3
#define dboxtex_button 4
#define dboxtex_buttonback 5
#define twloaderlogotex 6
#define settingslogotex 7 
#define settingslogotwltex 8 
#define settingslogoloadertex 9 
#define settingslogodemotex 10 
#define slot1boxarttex 11 
#define bnricontexnum 12 
#define bnricontexlaunch 13 
#define boxarttexnum 14 
//static size_t bnricontex[] = {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
//static size_t boxarttex[] = {36, 37, 38, 39, 40, 41};
#define boxfulltex 42 
#define shoulderltex 43 
#define shoulderrtex 44
//static size_t voltex[] = {45, 46, 47, 48, 49, 50};
#define batteryIcon 51 
#define batterychrgtex 52 
#define batterytex 53
#define cartnulltex 54
#define cartntrtex 55 
#define carttwltex 56
#define cartctrtex 57
#define bottomtex 58
#define scrollbartex 59
#define buttonarrowtex 60
#define bubbletex 61
#define r4loadingtex 62
#define toplogotex 63
#define toptex 64
#define topbgtex 65
#define iconstex 66
#define sdicontex 67
#define flashcardicontex 68
#define gbaicontex 69
#define smallsettingsicontex 70
#define iconnulltex 71
#define homeicontex 72
#define bottomlogotex 73
#define bipstex 74
#define scrollwindowtex 75
#define scrollwindowfronttex 76
#define dotcircletex 77
#define startbordertex 78
#define settingsicontex 79
#define getfcgameboxtex 80
#define boxemptytex 81
#define bracetex 82
#define ndslogotex 83
#define itex 84
#define topotex 85
#define bottomotex 86
#define bigotex 87
#define nintendotex 88
#define hstexttex 89
#define hstouchtex 90
#define hstex 91
#define wipetex 92
#define card_icon 93
#define whomeicontex 94
//static size_t setvoltex[] = {95, 96, 97, 98, 99, 100};
#define setbatterychrgtex 101
//static size_t setbatterytex[] = {102, 103, 104, 105, 106, 107};
#define dsboottex 108
#define dsiboottex 109
#define invdsboottex 110
#define invdsiboottex 111
#define dshstex 112
#define dsihstex 113
#define disabledtex 114
#define settingstex 115
#define settingslogooadertex 116

// Colors
#define TRANSPARENT RGBA8(0, 0, 0, 0)

#define BLACK RGBA8(0, 0, 0, 255)
#define WHITE RGBA8(255, 255, 255, 255)
#define GRAY RGBA8(127, 127, 127, 255)
#define BLUE RGBA8(0, 0, 255, 255)
#define GREEN RGBA8(0, 255, 0, 255)		

#define TIME RGBA8(16, 0, 0, 223)
#define DSSPLASH RGBA8(61, 161, 191, 255)

void pp2d_draw_texture_scale_blend(size_t id, int x, int y, float scaleX, float scaleY, u32 color);
void pp2d_draw_texture_part_blend(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, u32 color);
void pp2d_draw_texture_part_scale(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, float scaleX, float scaleY);
void pp2d_draw_texture_part_scale_blend(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, float scaleX, float scaleY, u32 color);