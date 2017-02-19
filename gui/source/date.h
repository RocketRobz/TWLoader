#ifndef DATE_H
#define DATE_H

#include <3ds/types.h>
#include <string>
#include <stddef.h>

typedef enum {
	FORMAT_YDM	= 0,
	FORMAT_YMD	= 1,
	FORMAT_DM	= 2,
	FORMAT_MD	= 3,
	FORMAT_M_D	= 4,
	FORMAT_MY	= 5,
} DateFormat;

/**
 * Get the current date as a C string.
 * @param format Date format.
 * @param buf Output buffer.
 * @param size Size of the output buffer.
 * @return Number of bytes written, excluding the NULL terminator.
 * @return Current date. (Caller must free() this string.)
 */
size_t GetDate(DateFormat format, char *buf, size_t size);

/**
 * Get the current time formatted for the top bar.
 * This includes the blinking ':'.
 * @param donotblink If true, reset the blink counter.
 * @return std::string containing the time.
 */
std::string RetTime(bool donotblink);

/**
 * Draw the date using the specified format and color.
 * @param Xpos X position.
 * @param Ypos Y position.
 * @param format Date format.
 * @param color Text color.
 * @param size Text size.
 */
void DrawDateF(int Xpos, int Ypos, DateFormat format, u32 color, int size);

/**
 * Draw the month and year using the specified color.
 * Format is selected based on the language setting.
 * @param Xpos X position.
 * @param Ypos Y position.
 * @param color Text color.
 * @param size Text size.
 */
void DrawDate(int Xpos, int Ypos, u32 color, int size);

#endif // DATE_H
