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

extern DVLB_s* vshader_dvlb;
extern shaderProgram_s program;
extern int uLoc_projection;
extern C3D_Mtx projection;
extern C3D_Tex* glyphSheets;
extern textVertex_s* textVtxArray;
extern int textVtxArrayPos;

#define TEXT_VTX_ARRAY_COUNT (4*1024)

void sceneInit(void);
void setTextColor(u32 color);
void addTextVertex(float vx, float vy, float tx, float ty);
void renderText(float x, float y, float scaleX, float scaleY, bool baseline, const char* text);
// void sceneRender(float size);
void sceneExit(void);


#endif /* CITROSTUFF_H */
