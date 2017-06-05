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

#include "file.h"

static const char* g_logFilename = "etripator.log";

/**
 * \brief Opens log file.
 * \param [in] userData  User data.
 * \return 0 upon success.
 */
static int FileMsgPrinterOpen(void* userData)
{
    FILE **out = (FILE**)userData; 
    if(NULL == out)
    {
        fprintf(stderr, "Invalid file logger.\n");
        return 1;
    }
    *out = fopen(g_logFilename, "ab");
    if(NULL == *out)
    {
        fprintf(stderr, "Failed to open log file %s: %s\n", g_logFilename, strerror(errno));
        return 1;
    }
    return 0;
}

/**
 * \brief Closes log file.
 * \param [in] userData  User data.
 * \return 0 upon success.
 */
static int FileMsgPrinterClose(void* userData)
{
    FILE **out = (FILE**)userData;
    if((NULL == userData) || (NULL == *out))
    {
        fprintf(stderr, "Invalid file logger.\n");
        return 1;
    }
    if(fclose(*out))
    {
        fprintf(stderr, "Failed to close log file %s : %s\n", g_logFilename, strerror(errno));
        return 1;
    }
    *out = NULL;
    return 0;
}

/**
 * \brief Prints message to file.
 * \param userData  User data.
 * \param type      Message type.
 * \param file      Name of the file where the print message command was issued.
 * \param line      Line number in the file where the print message command was issued.
 * \param function  Function where the print message command was issued.
 * \param format    Format string.
 *
 * \return 0 upon success.
 */
static int FileMsgPrinterOutput(void* userData, MessageType type, const char* file, size_t line, const char* function, const char* format, ...)
{
    static const char *messageTypeName[] =
    {
        "[Error]",
        "[Warning]",
        "[Info]"
    };

    if(NULL == userData)
    {
        fprintf(stderr, "Invalid file logger.\n");
        return 1;
    }
    else 
    {
        FILE *out = *((FILE**)userData);
        va_list argList;

        fprintf(out, "%s %s:%zd %s : ", messageTypeName[type], file, line, function);

        va_start(argList, format);
        vfprintf(out, format, argList);
        va_end(argList);
        
        fputc('\n', out);
        fflush(out);
        if(ferror(out))
        {
            fprintf(stderr, "Failed to output log to %s: %s\n", g_logFilename, strerror(errno));
            return 1;
        }
    }
    return 0;
}

/**
 * \brief Setups file message writer.
 * \param [in] printer Message printer.
 * \return 0 upon success.
 */
int FileMsgPrinter(MsgPrinter *printer)
{
    printer->open   = FileMsgPrinterOpen;
    printer->close  = FileMsgPrinterClose;
    printer->output = FileMsgPrinterOutput; 
    printer->userData = malloc(sizeof(FILE**));
    if(NULL == printer->userData)
    {
        fprintf(stderr, "Failed to create file logger: %s", strerror(errno));
        return 1;
    }
    return 0;
}

