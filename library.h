/*
    This file is part of Etripator,
    copyright (c) 2009--2012 Vincent Cruz.

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
#ifndef LIBRARY_H
#define LIBRARY_H

#include "config.h"

typedef void* Library;
typedef int (*LibraryProc) ();

/**
 * \brief Load a shared library.
 * \param filename File name
 * \param handle   Shared library handle
 * \return 0 if an error occured, 1 otherwise.
 */
int OpenLibrary(const char* filename, Library* handle);

/**
 * \brief Release loaded plugin.
 * \param handle Shared library handle
 * \return 0 if an error occured, 1 otherwise.
 */
int CloseLibrary(Library* handle);

/**
 * \brief Fetch the address of an exported function or variable from the specified library.
 * \param handle Shared library handle
 * \param name   Function name
 * \return The address of the function or variable is returned upon success. Otherwise NULL is returned.
 */
LibraryProc LoadProcAddress(Library handle, const char* name);

#endif // PLUGIN_H