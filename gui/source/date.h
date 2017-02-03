#ifndef DATE_H
#define DATE_H

#define FORMAT_YDM 0
#define FORMAT_YMD 1
#define FORMAT_DM  2
#define FORMAT_MD  3
#define FORMAT_M_D 4

#include <3ds/types.h>
#include <string>

/**
 * Get the current date as a C string.
 * @param Format Date format.
 * @return Current date. (Caller must free() this string.)
 */
char *GetDate(int Format);

/**
 * Get the current time formatted for the top bar.
 * This includes the blinking ':'.
 * @return std::string containing the time.
 */
std::string RetTime(int donotblink);

/**
 * Draw the date using the specified color.
 * Date format depends on language setting.
 * @param color Text color.
 */
void DrawDate(u32 color);

#endif // DATE_H
