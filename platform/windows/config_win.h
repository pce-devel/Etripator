#ifndef CONFIG_WIN_H
#define CONFIG_WIN_H

#define _GNU_SOURCE

#pragma warning(disable : 4996)

#include <Windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#define snprintf _snprintf

#include "getopt_win.h"
#include "stdint.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <ctype.h>

#ifdef ARCH_EXPORTS
#define ARCH_API __declspec(dllexport)
#else
#define ARCH_API __declspec(dllimport)
#endif

#endif // CONFIG_WIN_H