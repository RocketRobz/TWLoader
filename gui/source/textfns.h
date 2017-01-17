// Text functions.
#ifndef TEXTFNS_H
#define TEXTFNS_H

#include <string>

/**
 * Convert a UTF-8 string to wstring.
 * @param str UTF-8 string.
 * @return wstring. (UTF-32)
 */
std::wstring utf8_to_wstring(const char *str);

/**
 * Convert a UTF-8 string to wchar_t*.
 * @param str UTF-8 string.
 * @return malloc()'d wchar_t*. (UTF-32) (NOTE: If str is nullptr, this returns nullptr.)
 */
wchar_t *utf8_to_wchar(const char *str);

#endif /* TEXTFNS_H */
