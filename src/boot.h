#ifndef BOOT_H
#define BOOT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EMULATION

#include <3ds.h>
//#include "scanner.h"
//#include "titles.h"

//extern int targetProcessId;
//extern titleInfo_s target_title;

bool isNinjhax2(void);
//int bootApp(char* executablePath, executableMetadata_s* em, char* arg);
int bootApp(const char *executablePath, const char *arg);

#else

int bootApp(const char *executablePath, const char *arg);

#endif // !EMULATION

#ifdef __cplusplus
}
#endif

#endif // BOOT_H
