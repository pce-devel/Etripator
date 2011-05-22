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