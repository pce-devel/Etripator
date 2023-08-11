/*
    This file is part of Etripator,
    copyright (c) 2009--2023 Vincent Cruz.

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
#include "config.h"
#include "message.h"

#include <cwalk.h>

static msg_printer_t* g_msg_printer = NULL;

/**
 * Setup global message printer list.
 */
void msg_printer_init() {
    g_msg_printer = NULL;
    // nothing much atm...
}
/**
 * Releases the resources used by message printers.
 */
void msg_printer_destroy() {
    msg_printer_t* printer;
    for(printer=g_msg_printer; NULL != printer; printer=printer->next) {
        printer->close(printer);
    }
}
/**
 * Adds a new message printer to the global list.
 * \param [in] printer Message printer to be added to the list.
 * \return 0 upon success.
 */
int msg_printer_add(msg_printer_t *printer) {
    if(printer->open(printer)) {
        return 1;
    }
    printer->next = g_msg_printer;
    g_msg_printer = printer;
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
void print_msg(msg_type_t type, const char* file, size_t line, const char* function, const char* format, ...) {
    msg_printer_t* printer;
    const char* filename;
    size_t length;
    cwk_path_get_basename(file, &filename, &length);
    if(filename == NULL) {
        filename = file;
    }
    for(printer=g_msg_printer; NULL != printer; printer=printer->next) {
        if(printer->output) {
            va_list args; 
            va_start(args, format);
            printer->output(printer, type, filename, line, function, format, args);
            va_end(args);
        }
    }
}

