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

#endif /* TEXTFNS_H */
