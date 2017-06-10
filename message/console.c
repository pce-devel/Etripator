/*
    This file is part of Etripator,
    copyright (c) 2009--2017 Vincent Cruz.

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
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "console.h"

typedef struct
{
    int useEscapeCode; 
} ConsoleMsgPrinterImpl;

/**
 * \brief Tests if the console has support for colors and other things.
 * \param [in] userData  User data.
 * \return 0 upon success.
 */
static int ConsoleMsgPrinterOpen(void* userData)
{
    ConsoleMsgPrinterImpl *out = (ConsoleMsgPrinterImpl*)userData;
    
    out->useEscapeCode = isatty(fileno(stdout)) ? 1 : 0;
    if(!out->useEscapeCode)
    {
        fprintf(stderr, "Escape code disabled.\n");
    }
    return 0;
}

/**
 * \brief Closes console msg printer.
 * \param [in] userData  User data.
 * \return 0 upon success.
 */
static int ConsoleMsgPrinterClose(void* userData)
{
    (void)userData; // Unused atm.
    return 0;
}

/**
 * \brief Prints message to console.
 * \param userData  User data.
 * \param type      Message type.
 * \param file      Name of the file where the print message command was issued.
 * \param line      Line number in the file where the print message command was issued.
 * \param function  Function where the print message command was issued.
 * \param format    Format string.
 *
 * \return 0 upon success.
 */
static int ConsoleMsgPrinterOutput(void* userData, MessageType type, const char* file, size_t line, const char* function, const char* format, ...)
{
    static const char *messageTypeName[] =
    {
        "[Error]",
        "[Warning]",
        "[Info]"
    };
    static const char *consolePrefix[] =
    {
        "\x1b[1;37;41m",
        "\x1b[1;37;43m",
        "\x1b[1;37;42m"
    };

    if(NULL == userData)
    {
        fprintf(stderr, "Invalid console logger.\n");
        return 1;
    }
    else 
    {
        ConsoleMsgPrinterImpl *out = (ConsoleMsgPrinterImpl*)userData;
        va_list argList;
        const char *begin = (out->useEscapeCode) ? consolePrefix[type] : "";
        const char *end   = (out->useEscapeCode) ? "\x1b[0m" : "";
        fprintf(stderr, "%s%s%s %s:%zd %s : ", begin, messageTypeName[type], end, file, line, function);

        va_start(argList, format);
        vfprintf(stderr, format, argList);
        va_end(argList);
        
        fputc('\n', stderr);
        fflush(stderr);
        return 0;
    }
}

/**
 * \brief Setups console message writer.
 * \param [in] printer Message printer.
 * \return 0 upon success.
 */
int ConsoleMsgPrinter(MsgPrinter *printer)
{
    printer->open   = ConsoleMsgPrinterOpen;
    printer->close  = ConsoleMsgPrinterClose;
    printer->output = ConsoleMsgPrinterOutput; 
    printer->userData = malloc(sizeof(ConsoleMsgPrinterImpl));
    if(NULL == printer->userData)
    {
        fprintf(stderr, "Failed to create console logger: %s", strerror(errno));
        return 1;
    }
    return 0;
}

