#ifndef CONFIG_H
#define CONFIG_H

#define _GNU_SOURCE

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include <stdlib.h>
#include <stdio.h>

#if defined(_MSC_VER)
#define strncasecmp _strnicmp
#define strcasecmp _stricmp

#include "platform/windows/getopt_win.h"
#include "platform/windows/stdint.h"
#else
#include <unistd.h>
#include <getopt.h>
#include <stdint.h>
#include <sys/mman.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <ctype.h>

#endif // CONFIG_H