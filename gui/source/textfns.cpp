// Text functions.
#include "textfns.h"

#include <cstring>
#include <malloc.h>

#include <string>
using std::wstring;

/**
 * Convert a UTF-8 string to wchar_t. (internal function
 * @param wstr	[out] UTF-32 buffer.
 * @param str	[in] UTF-8 string.
 * @return Number of characters used in wstr, not including the NULL terminator.
 */
static int utf8_to_wchar_internal(wchar_t *wstr, const char *str)
{
	if (!str) {
		// No string.
		*wstr = 0;
		return 0;
	}

	int size = 0;
	for (; *str != 0; str++, wstr++, size++) {
		if (!(*str & 0x80)) {
			// ASCII. (U+0000-U+007F)
			*wstr = *str;
		} else if ((*str & 0xE0) == 0xC0) {
			// Two-byte UTF-8.
			if ((str[1] & 0xC0) != 0x80) {
				// Invalid sequence.
				*wstr = (wchar_t)0xFFFD;
			} else {
				*wstr = (wchar_t)(
					((str[0] & 0x1F) << 6) |
					((str[1] & 0x3F)));
				str++;
			}
		} else if ((*str & 0xF0) == 0xE0) {
			// Three-byte UTF-8.
			if ((str[1] & 0xC0) != 0x80 ||
			    (str[2] & 0xC0) != 0x80)
			{
				// Invalid sequence.
				*wstr = (wchar_t)0xFFFD;
			} else {
				*wstr = (wchar_t)(
					((str[0] & 0x0F) << 12) |
					((str[1] & 0x3F) << 6)  |
					((str[2] & 0x3F)));
				str += 2;
			}
		} else if ((*str & 0xF8) == 0xF0) {
			// Four-byte UTF-8.
			if ((str[1] & 0xC0) != 0x80 ||
			    (str[2] & 0xC0) != 0x80 ||
			    (str[3] & 0xC0) != 0x80)
			{
				// Invalid sequence.
				*wstr = (wchar_t)0xFFFD;
			} else {
				*wstr = (wchar_t)(
					((str[0] & 0x07) << 18) |
					((str[1] & 0x3F) << 12) |
					((str[2] & 0x3F) << 6)  |
					((str[3] & 0x3F)));
				str += 3;
			}
		} else {
			// Invalid UTF-8 sequence.
			*wstr = (wchar_t)0xFFFD;
		}
	}

	// Add a NULL terminator.
	*wstr = 0;
	return size;
}

/**
 * Convert a UTF-8 string to wstring.
 * @param str UTF-8 string.
 * @return wstring. (UTF-32)
 */
wstring utf8_to_wstring(const char *str)
{
	wstring wstr;
	if (!str) {
		// No string.
		return wstr;
	}

	// Allocate at least as many UTF-32 units
	// as there are bytes in the string, plus
	// one for the NULL terminator.
	wstr.resize(strlen(str)+1);

	// Convert the string.
	int size = utf8_to_wchar_internal(&wstr[0], str);

	// Resize the string to trim extra spaces.
	wstr.resize(size);
	return wstr;
}

/**
 * Convert a UTF-8 string to wchar_t*.
 * @param str UTF-8 string.
 * @return malloc()'d wchar_t*. (UTF-32) (NOTE: If str is nullptr, this returns nullptr.)
 */
wchar_t *utf8_to_wchar(const char *str)
{
	if (!str) {
		// No string.
		return nullptr;
	}

	// Allocate at least as many UTF-32 units
	// as there are bytes in the string, plus
	// one for the NULL terminator.
	wchar_t *wstr = (wchar_t*)malloc((strlen(str)+1) * sizeof(wchar_t));

	// Convert the string.
	utf8_to_wchar_internal(wstr, str);
	return wstr;
}
