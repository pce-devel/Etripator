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
#include <time.h>
#include <sys/time.h>

#include "console.h"

/**
 * \brief Tests if the console has support for colors and other things.
 * \param [in] impl Console message printer.
 * \return 0 upon success.
 */
static int ConsoleMsgPrinterOpen(void* impl)
{
    ConsoleMsgPrinter* printer = (ConsoleMsgPrinter*)impl;
    
    printer->useEscapeCode = isatty(fileno(stdout)) ? 1 : 0;
    if(!printer->useEscapeCode)
    {
        fprintf(stderr, "Escape code disabled.\n");
    }
    return 0;
}

/**
 * \brief Closes console msg printer.
 * \param [in] impl Console message printer.
 * \return 0 upon success.
 */
static int ConsoleMsgPrinterClose(void* impl)
{
    (void)impl; // Unused atm.
    return 0;
}

/**
 * \brief Prints message to console.
 * \param userData  User data.
 * \param [in] impl Console message printer.
 * \param [in] type      Message type.
 * \param [in] file      Name of the file where the print message command was issued.
 * \param [in] line      Line number in the file where the print message command was issued.
 * \param [in] function  Function where the print message command was issued.
 * \param [in] format    Format string.
 * \param [in] args      Argument lists.
 * \return 0 upon success.
 */
static int ConsoleMsgPrinterOutput(void* impl, MessageType type, const char* file, size_t line, const char* function, const char* format, va_list args)
{
    static const char *messageTypeName[] =
    {
        "[Error]",
        "[Warning]",
        "[Info]"
    };
    static const char *messageTypePrefix[] =
    {
        "\x1b[1;31m",
        "\x1b[1;33m",
        "\x1b[1;32m"
    };

    if(NULL == impl)
    {
        fprintf(stderr, "Invalid console logger.\n");
        return 1;
    }
    else 
    {
        ConsoleMsgPrinter* printer = (ConsoleMsgPrinter*)impl;
        struct timeval tv;
        struct tm *now;
        char dateString[128];
        
        gettimeofday(&tv, NULL);
        now = localtime(&tv.tv_sec);
        strftime(dateString, 128, "%Y-%m-%d %H:%M:%S", now);

        if(printer->useEscapeCode)
        {
            fprintf(stderr, "%s%s\x1b[0m %s.%06ld \x1b[0;33m%s:%zd %s \x1b[1;37m : ", messageTypePrefix[type], messageTypeName[type], dateString, tv.tv_usec, file, line, function);
        }
        else
        {
            fprintf(stderr, "%s %s.%06ld %s:%zd %s : ", messageTypeName[type], dateString, tv.tv_usec, file, line, function);
        }
        vfprintf(stderr, format, args);
        
        if(printer->useEscapeCode)
        {
            fprintf(stderr, "\x1b[0m\n");
        }
        else 
        {
            fputc('\n', stderr);
        }
        fflush(stderr);
        return 0;
    }
}

/**
 * \brief Setups console message writer.
 * \param [in] printer Console message printer.
 * \return 0 upon success.
 */
int SetupConsoleMsgPrinter(ConsoleMsgPrinter *printer)
{
    printer->super.open   = ConsoleMsgPrinterOpen;
    printer->super.close  = ConsoleMsgPrinterClose;
    printer->super.output = ConsoleMsgPrinterOutput; 
    printer->useEscapeCode = 0;
    return 0;
}

