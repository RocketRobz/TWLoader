// Citro3D functions.
#ifndef CITROSTUFF_H
#define CITROSTUFF_H

#include <vector>
#include <string>

#define CLEAR_COLOR 0x68B0D8FF

#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

typedef struct { float position[3]; float texcoord[2]; } textVertex_s;

static DVLB_s* vshader_dvlb;
static shaderProgram_s program;
static int uLoc_projection;
static C3D_Mtx projection;
static C3D_Tex* glyphSheets;
static textVertex_s* textVtxArray;
static int textVtxArrayPos;

#define TEXT_VTX_ARRAY_COUNT (4*1024)

static void sceneInit(void);
static void setTextColor(u32 color);
static void addTextVertex(float vx, float vy, float tx, float ty);
static void renderText(float x, float y, float scaleX, float scaleY, bool baseline, const char* text);


#endif /* CITROSTUFF_H */
