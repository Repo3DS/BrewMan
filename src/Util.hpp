#ifndef BREWMAN_UTIL_HPP
#define BREWMAN_UTIL_HPP

namespace BrewMan
{

bool pathExists(const char* path, bool escape = true);
int removeDirectory(const char *path, bool onlyIfEmpty = false);

} // namespace BrewMan

#endif // BREWMAN_UTIL_HPP
