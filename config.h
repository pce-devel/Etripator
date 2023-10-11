/*
 * This file is part of Etripator,
 * copyright (c) 2009--2023 Vincent Cruz.
 *
 * Etripator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Etripator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Etripator.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ETRIPATOR_CONFIG_H
#define ETRIPATOR_CONFIG_H

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#include <stdint.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <ctype.h>

#if defined(__linux__) || defined(__APPLE__)
#   include <sys/time.h>
#   include <unistd.h>
#endif

#if defined(_MSC_VER)
#   define strncasecmp _strnicmp
#   define strcasecmp _stricmp
#   define snprintf _snprintf
#   define access _access
#endif

#endif // ETRIPATOR_CONFIG_H
