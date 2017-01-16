// Text functions.
#include "textfns.h"

#include <cstring>

#include <string>
using std::wstring;

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

	// TODO: This is probably not optimal.
	// TODO: Also not checking for "must be shortest sequence".
	// TODO: Fold this str++ into the cases?
	wstr.reserve(strlen(str));
	for (; *str != 0; str++) {
		if (!(*str & 0x80)) {
			// ASCII. (U+0000-U+007F)
			wstr += *str;
		} else if ((*str & 0xE0) == 0xC0) {
			// Two-byte UTF-8.
			if ((str[1] & 0xC0) != 0x80) {
				// Invalid sequence.
				wstr += (wchar_t)0xFFFD;
			} else {
				wstr += (wchar_t)(
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
				wstr += (wchar_t)0xFFFD;
			} else {
				wstr += (wchar_t)(
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
				wstr += (wchar_t)0xFFFD;
			} else {
				wstr += (wchar_t)(
					((str[0] & 0x07) << 18) |
					((str[1] & 0x3F) << 12) |
					((str[2] & 0x3F) << 6)  |
					((str[3] & 0x3F)));
				str += 3;
			}
		} else {
			// Invalid UTF-8 sequence.
			wstr += (wchar_t)0xFFFD;
		}
	}

	return wstr;
}
