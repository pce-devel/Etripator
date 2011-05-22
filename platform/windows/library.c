#include "../../config.h"
#include "../../message.h"
#include "../../library.h"

// Win32 specific error message
static void LibraryErr(const char* msg, const char* extra)
{
	// TODO : use wchar :(
	char* buffer;
	DWORD err  = GetLastError();
	DWORD size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, (LPSTR)&buffer, 8, NULL);
		
	ERROR_MSG("%s %s : %s", msg, extra, buffer);
		
	LocalFree(buffer);
}

/**
 * \brief Load a shared library.
 * \param filename File name
 * \param handle   Shared library handle
 * \return 0 if an error occured, 1 otherwise.
 */
int OpenLibrary(const char* filename, Library* handle)
{
	if( (handle == NULL) || (filename == NULL) )
	{
		ERROR_MSG("Invalid arguments!");
		return 0;
	}

	*handle = (Library)LoadLibraryA(filename);
	if(*handle == NULL)
	{
		LibraryErr("Unable to open library", filename);
		return 0;
	}

	return 1;
}

/**
 * \brief Release loaded plugin.
 * \param handle Shared library handle
 * \return 0 if an error occured, 1 otherwise.
 */
int CloseLibrary(Library* handle)
{
	BOOL res;

	if( handle == NULL )
	{
		ERROR_MSG("Invalid argument!");
		return 0;
	}

	res = FreeLibrary((HMODULE)*handle);
	if(res != TRUE)
	{
		LibraryErr("Unable to close library", "");
		return 0;
	}

	*handle = NULL;
	return 1;
}

/**
 * \brief Fetch the address of an exported function or variable from the specified library.
 * \param handle Shared library handle
 * \param name   Function name
 * \return The address of the function or variable is returned upon success. Otherwise NULL is returned.
 */
LibraryProc LoadProcAddress(Library handle, const char* name)
{
	FARPROC proc;
	if( (handle == NULL) || (name == NULL) )
	{
		ERROR_MSG("Invalid arguments!");
		return 0;
	}

	proc = GetProcAddress((HMODULE)handle, name);
	if(proc == NULL)
	{
		LibraryErr("Unable to find symbol", name);
	}

	return (LibraryProc)proc;
}
