#ifndef CONFIG_H
#define CONFIG_H

#if defined(_MSC_VER)
#include "platform/windows/config_win.h"
#else
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define strncasecmp _strnicmp
#define strcasecmp _stricmp

#include <unistd.h>
#include <getopt.h>
#include <stdint.h>
#include <sys/mman.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <ctype.h>
#endif

#endif // CONFIG_H