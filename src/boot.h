#ifndef BOOT_H
#define BOOT_H

#ifndef EMULATION

#ifdef __cplusplus
extern "C" {
#endif

#include <3ds.h>
//#include "scanner.h"
//#include "titles.h"

//extern int targetProcessId;
//extern titleInfo_s target_title;

bool isNinjhax2(void);
//int bootApp(char* executablePath, executableMetadata_s* em, char* arg);
int bootApp(const char *executablePath, const char *arg);

#ifdef __cplusplus
}
#endif

#else

int bootApp(const char *executablePath, const char *arg) {}

#endif // !EMULATION

#endif // BOOT_H
