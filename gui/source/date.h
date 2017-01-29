#ifndef DATE_H
#define DATE_H

#define FORMAT_YDM 0
#define FORMAT_YMD 1
#define FORMAT_DM  2

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
std::string RetTime(void);

#endif // DATE_H
