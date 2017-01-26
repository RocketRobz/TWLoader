#include "rmkdir.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

// Based on https://gist.github.com/ChisholmKyle/0cbedcd3e64132243a39
// and http://nion.modprobe.de/blog/archives/357-Recursive-directory-creation.html

/**
 * Recursively mkdir() a directory.
 * @param path Absolute path.
 * @param omde Mode.
 * @return 0 on success; non-zero on error.
 */
int rmkdir(const char *path, int mode)
{
	char tmp[260];
	char *p = NULL;
	size_t len;

	strncpy(tmp, path, sizeof(tmp));
	len = strlen(tmp);
	if (len >= sizeof(tmp)) {
		return -1;
	}

	// Remove the trailing slash, if present.
	if (tmp[len-1] == '/') {
		tmp[len-1] = 0;
	}

	// Recursive mkdir
	for (p = tmp+1; *p; p++) {
		if (*p != '/')
			continue;
	
		// Found a slash.
		// If the previous character is ':', this is
		// the device name and should be ignored.
		if (*(p-1) == ':')
			continue;

		// NOTE: We can't determine if a file is a
		// directory or not on 3DS, so just keep
		// going, even if the mkdir fails.
		*p = 0;
		mkdir(tmp, mode);
		*p = '/';
	}

	// Create the lowest-level directory.
	mkdir(tmp, mode);
	return 0;
}
