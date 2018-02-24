#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <3ds.h>
#include "pp2d/pp2d.h"

// Textures
extern size_t rectangletex;
extern size_t dialogboxtex;
extern size_t dboxtex_iconbox;
extern size_t dboxtex_button;
extern size_t dboxtex_buttonback;
extern size_t twloaderlogotex;
extern size_t settingslogotex;
extern size_t settingslogotwltex; 
extern size_t settingslogoloadertex; 
extern size_t settingslogodemotex; 
extern size_t slot1boxarttex; 
extern size_t bnricontexnum;
extern size_t boxarttexnum; 
extern size_t bnricontex[64];
extern size_t boxarttex[7];
extern size_t boxtex; 
extern size_t shouldertex;
extern size_t voltex;
extern size_t batteryIcon;
extern size_t batterytex;
extern size_t cartnulltex;
extern size_t cartntrtex; 
extern size_t carttwltex;
extern size_t cartctrtex;
extern size_t bottomtex;
extern size_t scrollbartex;
extern size_t buttonarrowtex;
extern size_t bubbletex;
extern size_t r4loadingtex;
extern size_t toplogotex;
extern size_t toptex;
extern size_t topbgtex;
extern size_t iconstex;
extern size_t sdicontex;
extern size_t flashcardicontex;
extern size_t gbaicontex;
extern size_t smallsettingsicontex;
extern size_t iconnulltex;
extern size_t homeicontex;
extern size_t bottomlogotex;
extern size_t bipstex;
extern size_t scrollwindowtex;
extern size_t scrollwindowfronttex;
extern size_t dotcircletex;
extern size_t startbordertex;
extern size_t settingsicontex;
extern size_t getfcgameboxtex;
extern size_t _3dsbottopbartex;
extern size_t bracetex;
extern size_t ndslogotex;
extern size_t itex;
extern size_t topotex;
extern size_t bottomotex;
extern size_t bigotex;
extern size_t nintendotex;
extern size_t hstexttex;
extern size_t hstouchtex;
extern size_t hstex;
extern size_t wipetex;
extern size_t card_icon;
extern size_t whomeicontex;
extern size_t setvoltex;
extern size_t setbatterytex;
extern size_t dsboottex;
extern size_t dsiboottex;
extern size_t invdsboottex;
extern size_t invdsiboottex;
extern size_t dshstex;
extern size_t invdshstex;
extern size_t dsihstex;
extern size_t disabledtex;
extern size_t settingstex;
extern size_t settingslogooadertex;
extern size_t new_tex;
extern size_t gbctex;
extern size_t nestex;
extern size_t anniversarytex;
extern size_t _3dsbotbotbartex;
extern size_t _3dsbotbotbarbuttex;
extern size_t regularloadtex;
extern size_t pongloadtex;
extern size_t tictactoeloadtex;
extern size_t _3dsshouldertex;

// Colors
#define TRANSPARENT RGBA8(0, 0, 0, 0)

#define BLACK RGBA8(0, 0, 0, 255)
#define WHITE RGBA8(255, 255, 255, 255)
#define GRAY RGBA8(127, 127, 127, 255)
#define BLUE RGBA8(0, 0, 255, 255)
#define GREEN RGBA8(0, 255, 0, 255)
#define RED RGBA8(255, 0, 0, 255)

#define TIME RGBA8(16, 0, 0, 223)
#define DSSPLASH RGBA8(61, 161, 191, 255)

void pp2d_draw_texture_scale_blend(size_t id, int x, int y, float scaleX, float scaleY, u32 color);
void pp2d_draw_texture_scale_flip(size_t id, int x, int y, float scaleX, float scaleY, flipType fliptype);
void pp2d_draw_texture_flip_blend(size_t id, int x, int y, flipType fliptype, u32 color);
void pp2d_draw_texture_part_blend(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, u32 color);
void pp2d_draw_texture_part_flip(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, flipType fliptype);
void pp2d_draw_texture_part_scale(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, float scaleX, float scaleY);
void pp2d_draw_texture_part_scale_flip(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, float scaleX, float scaleY, flipType fliptype);
void pp2d_draw_texture_part_scale_blend(size_t id, int x, int y, int xbegin, int ybegin, int width, int height, float scaleX, float scaleY, u32 color);
void pp2d_draw_texture_rotate_flip_blend(size_t id, int x, int y, float angle, flipType fliptype, u32 color);
#endif // GRAPHIC_H