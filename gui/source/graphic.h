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
//#define bnricontex[21] 15 
#define bnricontex 15 
//#define boxarttex[6] 16 
#define boxarttex 16 
#define boxfulltex 17 
#define shoulderltex 18 
#define shoulderrtex 19 
//#define voltex[6] 20 
#define voltex 20 
#define batteryIcon 21 
#define batterychrgtex 22 
#define batterytex 23 
#define cartnulltex 24 
#define cartntrtex 25 
#define carttwltex 26
#define cartctrtex 27
#define bottomtex 28
#define scrollbartex 29
#define buttonarrowtex 30
#define bubbletex 31
#define r4loadingtex 32
#define toplogotex 33
#define toptex 34
#define topbgtex 35
#define iconstex 36
#define sdicontex 37
#define flashcardicontex 38
#define gbaicontex 39
#define smallsettingsicontex 40
#define iconnulltex 41
#define homeicontex 42
#define bottomlogotex 43
#define bipstex 44
#define scrollwindowtex 45
#define scrollwindowfronttex 46
#define dotcircletex 47
#define startbordertex 48
#define settingsicontex 49
#define getfcgameboxtex 50
#define boxemptytex 51
#define bracetex 52
#define ndslogotex 53
#define itex 54
#define topotex 55
#define bottomotex 56
#define bigotex 57
#define nintendotex 58
#define hstexttex 59
#define hstouchtex 60
#define hstex 61
#define wipetex 62
#define card_icon 63
#define whomeicontex 64
//#define setvoltex[6] 65
#define setvoltex 65
#define setbatterychrgtex 66
//#define setbatterytex[6] 67
#define setbatterytex 67
#define dsboottex 68
#define dsiboottex 69
#define invdsboottex 70
#define invdsiboottex 71
#define dshstex 72
#define dsihstex 73
#define disabledtex 74
#define settingstex 75
#define settingslogooadertex 76

// Colors
#define TRANSPARENT RGBA8(0, 0, 0, 0)

#define BLACK RGBA8(0, 0, 0, 255)
#define WHITE RGBA8(255, 255, 255, 255)
#define GRAY RGBA8(127, 127, 127, 255)
#define BLUE RGBA8(0, 0, 255, 255)
#define GREEN RGBA(0, 255, 0, 255)		

#define TIME RGBA8(16, 0, 0, 223)
#define DSSPLASH RGBA8(61, 161, 191, 255)

void pp2d_draw_texture_scale_blend(size_t id, int x, int y, float scaleX, float scaleY, u32 color);
void pp2d_draw_texture_part_blend(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, u32 color);
void pp2d_draw_texture_part_scale(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, float scaleX, float scaleY);
void pp2d_draw_texture_part_scale_blend(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, float scaleX, float scaleY, u32 color);