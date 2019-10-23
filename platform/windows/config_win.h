/*
    This file is part of Etripator,
    copyright (c) 2009--2019 Vincent Cruz.

    Etripator is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Etripator is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Etripator.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef CONFIG_WIN_H
#define CONFIG_WIN_H

#define _GNU_SOURCE

#pragma warning(disable : 4996)

#include <Windows.h>
#include <io.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#define snprintf _snprintf
#define access _access

// access mode names.
#ifndef R_OK
#define R_OK 0x02
#endif
#ifndef W_OK
#define W_OK 0x04
#endif
// X_OK does not exist on msvc
#ifndef F_OK
#define F_OK 0x00
#endif

#if _MSC_VER >= 1800 // [
#include <stdint.h>
#else // ] _MSC_VER >= 1800 [
#include "stdint.h"
#endif // ]
#include "time.h"
#include "basename.h"

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
