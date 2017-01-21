// Text functions.
#include "textfns.h"

#include <cstring>
#include <malloc.h>

#include <string>
#include <vector>
using std::vector;
using std::wstring;

/** UTF-16 **/

/**
 * Convert a UTF-16 string to wchar_t. (internal function
 * @param wstr	[out] UTF-32 buffer.
 * @param str	[in] UTF-16 string.
 * @return Number of characters used in wstr, not including the NULL terminator.
 */
static int utf16_to_wchar_internal(wchar_t *wstr, const u16 *str)
{
	if (!str) {
		// No string.
		*wstr = 0;
		return 0;
	}

	int size = 0;
	for (; *str != 0; str++, wstr++, size++) {
		// Convert the UTF-16 character to UTF-32.
		// Special handling is needed only for surrogate pairs.
		// (TODO: Test surrogate pair handling.)
		if ((*str & 0xFC00) == 0xD800) {
			// High surrogate. (0xD800-0xDBFF)
			if ((str[1] & 0xFC00) == 0xDC00) {
				// Low surrogate. (0xDC00-0xDFFF)
				// Recombine to get the actual character.
				wchar_t wchr = 0x10000;
				wchr += ((str[0] & 0x3FF) << 10);
				wchr +=  (str[1] & 0x3FF);
				*wstr = wchr;
				// Make sure we don't process the low surrogate
				// on the next iteration.
				str++;
			} else {
				// Unpaired high surrogate.
				*wstr = (wchar_t)(0xFFFD);
			}
		} else if ((*str & 0xFC00) == 0xDC00) {
			// Unpaired low surrogate.
			*wstr = (wchar_t)(0xFFFD);
		} else {
			// Standard UTF-16 character.
			*wstr = (wchar_t)*str;
		}
	}

	// Add a NULL terminator.
	*wstr = 0;
	return size;
}

/**
 * Convert a UTF-16 string to wstring.
 * @param str UTF-16 string.
 * @return wstring. (UTF-32)
 */
wstring utf16_to_wstring(const u16 *str)
{
	wstring wstr;
	if (!str) {
		// No string.
		return wstr;
	}

	// Allocate at least as many UTF-32 units
	// as there are characters in the string,
	// plus one for the NULL terminator.
	int len = 0;
	for (const u16 *p = str; *p != 0; p++, len++) { }
	wstr.resize(len+1);

	// Convert the string.
	int size = utf16_to_wchar_internal(&wstr[0], str);

	// Resize the string to trim extra spaces.
	wstr.resize(size);
	return wstr;
}

/**
 * Convert a UTF-16 string with newlines to a vector of wstrings.
 * @param str UTF-16 string with newlines.
 * @param len Length of str.
 * @return vector<wstring>, split on newline boundaries.
 */
vector<wstring> utf16_nl_to_vwstring(const u16 *str, int len)
{
	// Buffers for the strings.
	// Assuming wchar_t is 32-bit.
	static_assert(sizeof(wchar_t) == 4, "wchar_t is not 32-bit.");
	vector<wstring> vec_wstr;
	vec_wstr.reserve(3);
	wstring wstr;
	wstr.reserve(64);

	for (; *str != 0 && len > 0; str++, len--) {
		// Convert the UTF-16 character to UTF-32.
		// Special handling is needed only for surrogate pairs.
		// (TODO: Test surrogate pair handling.)
		if ((*str & 0xFC00) == 0xD800) {
			// High surrogate. (0xD800-0xDBFF)
			if (len > 2 && (str[1] & 0xFC00) == 0xDC00) {
				// Low surrogate. (0xDC00-0xDFFF)
				// Recombine to get the actual character.
				wchar_t wchr = 0x10000;
				wchr += ((str[0] & 0x3FF) << 10);
				wchr +=  (str[1] & 0x3FF);
				wstr += wchr;
				// Make sure we don't process the low surrogate
				// on the next iteration.
				str++;
				len--;
			} else {
				// Unpaired high surrogate.
				wstr += (wchar_t)(0xFFFD);
			}
		} else if ((*str & 0xFC00) == 0xDC00) {
			// Unpaired low surrogate.
			wstr += (wchar_t)(0xFFFD);
		} else {
			// Standard UTF-16 character.
			switch (*str) {
				case L'\r':
					// Skip carriage returns.
					break;
				case L'\n':
					// Newline.
					vec_wstr.push_back(wstr);
					wstr.clear();
					break;
				default:
					// Add the character.
					wstr += *str;
					break;
			}
		}
	}

	// Add the last line if it's not empty.
	if (!wstr.empty()) {
		vec_wstr.push_back(wstr);
	}

	return vec_wstr;
}

/**
 * Convert a UTF-16 string to wchar_t*.
 * @param str UTF-16 string.
 * @return malloc()'d wchar_t*. (UTF-32) (NOTE: If str is nullptr, this returns nullptr.)
 */
wchar_t *utf16_to_wchar(const u16 *str)
{
	if (!str) {
		// No string.
		return nullptr;
	}

	// Allocate at least as many UTF-32 units
	// as there are characters in the string,
	// plus one for the NULL terminator.
	int len = 0;
	for (const u16 *p = str; *p != 0; p++, len++) { }
	wchar_t *wstr = (wchar_t*)malloc((len+1) * sizeof(wchar_t));

	// Convert the string.
	utf16_to_wchar_internal(wstr, str);
	return wstr;
}

/** UTF-8 **/

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

/** Latin-1 **/

/**
 * Convert a Latin-1 string to wchar_t. (internal function
 * @param wstr	[out] UTF-32 buffer.
 * @param str	[in] Latin-1 string.
 * @return Number of characters used in wstr, not including the NULL terminator.
 */
static int latin1_to_wchar_internal(wchar_t *wstr, const char *str)
{
	if (!str) {
		// No string.
		*wstr = 0;
		return 0;
	}

	int size = 0;
	for (; *str != 0; str++, wstr++, size++) {
		*wstr = *str;
	}

	// Add a NULL terminator.
	*wstr = 0;
	return size;
}

/**
 * Convert a Latin-1 string to wstring.
 * @param str Latin-1 string.
 * @return wstring. (UTF-32)
 */
wstring latin1_to_wstring(const char *str)
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
	int size = latin1_to_wchar_internal(&wstr[0], str);

	// Resize the string to trim extra spaces.
	wstr.resize(size);
	return wstr;
}

/**
 * Convert a Latin-1 string to wchar_t*.
 * @param str Latin-1 string.
 * @return malloc()'d wchar_t*. (UTF-32) (NOTE: If str is nullptr, this returns nullptr.)
 */
wchar_t *latin1_to_wchar(const char *str)
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
	latin1_to_wchar_internal(wstr, str);
	return wstr;
}
