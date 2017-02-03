#include "date.h"
#include "language.h"

#include <sftd.h>

#include <ctime>
#include <cstdio>
#include <malloc.h>

#include <string>
using std::string;

// from main.cpp
extern sftd_font *font;

/**
 * Get the current date as a C string.
 * @param Format Date format.
 * @return Current date. (Caller must free() this string.)
 */
char *GetDate(int Format)
{
	char *Ret = (char*)malloc(24);

	time_t Raw;
	time(&Raw);
	tm *Time = localtime(&Raw);

	switch(Format) {
		case FORMAT_YDM:
			strftime(Ret, 24, "%Y-%d-%m_%k-%M", Time);
			break;

		case FORMAT_YMD:
			strftime(Ret, 24, "%Y-%m-%d_%k-%M", Time);
			break;
		case FORMAT_DM:
			strftime(Ret, 24, "%d/%m", Time); // Ex: 26/12
			break;
		case FORMAT_MD:
			strftime(Ret, 24, "%m/%d", Time); // Ex: 12/26
			break;
		case FORMAT_M_D:
			strftime(Ret, 24, "%d.%m.", Time); // Ex: 26.12.
			break;
	}

	return Ret;
}

/**
 * Get the current time formatted for the top bar.
 * This includes the blinking ':'.
 * @return std::string containing the time.
 */
string RetTime(int donotblink)
{
	time_t Raw;
	time(&Raw);
	tm *Time = localtime(&Raw);

	// Blink the ':' approximately once per second.
	// (120 is because two top frames are drawn every 1/60th
	//  due to 3D.)
	static int chartimer = 0;
	if (donotblink == 0)
		chartimer++;
	else
		chartimer = 0;
	if (chartimer >= 120*2)
		chartimer = 0;

	char Tmp[24];
	if (chartimer >= 120) {
		strftime(Tmp, sizeof(Tmp), "%k %M", Time);
	} else {
		strftime(Tmp, sizeof(Tmp), "%k:%M", Time);
	}

	return string(Tmp);
}

/**
 * Draw the date using the specified color.
 * Date format depends on language setting.
 * @param color Text color.
 */
void DrawDate(u32 color)
{
	// Date formats.
	// - Index: Language ID.
	// - Value: Date format.
	static const uint8_t date_fmt[12] = {
		FORMAT_MD,	// Japanese
		FORMAT_MD,	// English
		FORMAT_DM,	// French
		FORMAT_M_D,	// German
		FORMAT_DM,	// Italian
		FORMAT_DM,	// Spanish
		FORMAT_MD,	// Simplified Chinese
		FORMAT_MD,	// Korean
		FORMAT_DM,	// Dutch
		FORMAT_DM,	// Portuguese
		FORMAT_M_D,	// Russian
		FORMAT_MD,	// Traditional Chinese
	};

	// NOTE: GetDate() malloc()'s the date string.
	char *date_str = GetDate(date_fmt[language]);
	if (!date_str)
		return;
	sftd_draw_text(font, 282, 3, color, 12, date_str);
	free(date_str);
}
