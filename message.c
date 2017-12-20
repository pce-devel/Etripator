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
#include "message.h"

static MsgPrinter* g_MsgPrinter = NULL;

/**
 * Setup global message printer list.
 */
void SetupMsgPrinters()
{
    g_MsgPrinter = NULL;
    // nothing much atm...
}
/**
 * Releases the resources used by message printers.
 */
void DestroyMsgPrinters()
{
    MsgPrinter* printer;
    for(printer=g_MsgPrinter; NULL != printer; printer=printer->next)
    {
        printer->close(printer);
    }
}
/**
 * Adds a new message printer to the global list.
 * \param [in] printer Message printer to be added to the list.
 * \return 0 upon success.
 */
int AddMsgPrinter(MsgPrinter *printer)
{
    if(printer->open(printer))
    {
        return 1;
    }
    printer->next = g_MsgPrinter;
    g_MsgPrinter = printer;
    return 0;
}
/**
 * Dispatch messages to printers.
 * \param type      Message type.
 * \param file      Name of the file where the print message command was issued.
 * \param line      Line number in the file where the print message command was issued.
 * \param function  Function where the print message command was issued.
 * \param format    Format string.
 */
void PrintMsg(MessageType type, const char* file, size_t line, const char* function, const char* format, ...)
{
    MsgPrinter* printer;
    const char* filename = strrchr(file, '/');
    filename = filename ? (filename+1) : file;
    
    for(printer=g_MsgPrinter; NULL != printer; printer=printer->next)
    {
        if(printer->output)
        {
            va_list args; 
            va_start(args, format);
            printer->output(printer, type, filename, line, function, format, args);
            va_end(args);
        }
    }
}


