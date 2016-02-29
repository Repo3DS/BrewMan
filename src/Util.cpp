#include "Util.hpp"
#include <sys/stat.h>
#include <dirent.h>
#include <cpp3ds/System/FileSystem.hpp>
#include <sys/unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

namespace BrewMan
{

bool pathExists(const char* path, bool escape)
{
	struct stat buffer;
	if (escape)
		return stat(cpp3ds::FileSystem::getFilePath(path).c_str(), &buffer) == 0;
	else
		return stat(path, &buffer) == 0;
}

int removeDirectory(const char *path, bool onlyIfEmpty)
{
	DIR *d = opendir(path);
	size_t path_len = strlen(path);
	int r = -1;

	if (d) {
		struct dirent *p;

		r = 0;

		while (!r && (p = readdir(d))) {
			int r2 = -1;
			char *buf;
			size_t len;

			/* Skip the names "." and ".." as we don't want to recurse on them. */
			if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
				continue;

			len = path_len + strlen(p->d_name) + 2;
			buf = (char*) malloc(len);

			if (buf) {
				struct stat sb;
				snprintf(buf, len, "%s/%s", path, p->d_name);

				if (!stat(buf, &sb)) {
					if (S_ISDIR(sb.st_mode))
						r2 = removeDirectory(buf, onlyIfEmpty);
					else if (!onlyIfEmpty)
						r2 = unlink(buf);
				}

				free(buf);
			}

			r = r2;
		}

		closedir(d);
	}

	printf("%s\n", path);

	if (!r)
		r = rmdir(path);

	return r;
}

} // namespace BrewMan
