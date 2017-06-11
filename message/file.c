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
 * \param [in] impl Msg printer implementation.
 * \return 0 upon success.
 */
static int FileMsgPrinterOpen(void* impl)
{
    FileMsgPrinter* printer = (FileMsgPrinter*)impl; 
    if(NULL == printer)
    {
        fprintf(stderr, "Invalid file logger.\n");
        return 1;
    }
    printer->out = fopen(g_logFilename, "ab");
    if(NULL == printer->out)
    {
        fprintf(stderr, "Failed to open log file %s: %s\n", g_logFilename, strerror(errno));
        return 1;
    }
    return 0;
}

/**
 * \brief Closes log file.
 * \param [in] impl Msg printer implementation.
 * \return 0 upon success.
 */
static int FileMsgPrinterClose(void* impl)
{
    FileMsgPrinter* printer = (FileMsgPrinter*)impl; 
    if((NULL == printer) || (NULL == printer->out))
    {
        fprintf(stderr, "Invalid file logger.\n");
        return 1;
    }
    if(fclose(printer->out))
    {
        fprintf(stderr, "Failed to close log file %s : %s\n", g_logFilename, strerror(errno));
        return 1;
    }
    printer->out = NULL;
    return 0;
}

/**
 * \brief Prints message to file.
 * \param [in] impl Msg printer implementation.
 * \param [in] type      Message type.
 * \param [in] file      Name of the file where the print message command was issued.
 * \param [in] line      Line number in the file where the print message command was issued.
 * \param [in] function  Function where the print message command was issued.
 * \param [in] format    Format string.
 * \param [in] args      Argument lists.
 * \return 0 upon success.
 */
static int FileMsgPrinterOutput(void* impl, MessageType type, const char* file, size_t line, const char* function, const char* format, va_list args)
{
    static const char *messageTypeName[] =
    {
        "[Error]",
        "[Warning]",
        "[Info]"
    };

    if(NULL == impl)
    {
        fprintf(stderr, "Invalid file logger.\n");
        return 1;
    }
    else 
    {
        FileMsgPrinter* printer = (FileMsgPrinter*)impl; 
        fprintf(printer->out, "%s %s:%zd %s : ", messageTypeName[type], file, line, function);
        vfprintf(printer->out, format, args);
        fputc('\n', printer->out);
        fflush(printer->out);
        if(ferror(printer->out))
        {
            fprintf(stderr, "Failed to output log to %s: %s\n", g_logFilename, strerror(errno));
            return 1;
        }
    }
    return 0;
}

/**
 * \brief Setups file message writer.
 * \param [in] impl Msg printer implementation.
 * \return 0 upon success.
 */
int SetupFileMsgPrinter(FileMsgPrinter *printer)
{
    printer->super.open   = FileMsgPrinterOpen;
    printer->super.close  = FileMsgPrinterClose;
    printer->super.output = FileMsgPrinterOutput; 
    printer->out    = NULL;
    return 0;
}

