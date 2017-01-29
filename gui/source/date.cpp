#include "date.h"

#include <time.h>
#include <stdio.h>
#include <malloc.h>

#include <string>
using std::string;

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
string RetTime(void)
{
	time_t Raw;
	time(&Raw);
	tm *Time = localtime(&Raw);

	// Blink the ':' approximately once per second.
	// (120 is because two top frames are drawn every 1/60th
	//  due to 3D.)
	static int chartimer = 0;
	chartimer++;
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
