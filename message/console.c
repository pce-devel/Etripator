/*
    This file is part of Etripator,
    copyright (c) 2009--2022 Vincent Cruz.

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
#include "../config.h"
#include "console.h"

/**
 * \brief Tests if the console has support for colors and other things.
 * \param [in] impl Console message printer.
 * \return 0 upon success.
 */
static int console_msg_printer_open(void* impl) {
    console_msg_printer_t* printer = (console_msg_printer_t*)impl;
    printer->use_escape_code = isatty(fileno(stdout)) ? 1 : 0;
    if(!printer->use_escape_code) {
        fprintf(stderr, "Escape code disabled.\n");
    }
    return 0;
}

/**
 * \brief Closes console msg printer.
 * \param [in] impl Console message printer.
 * \return 0 upon success.
 */
static int console_msg_printer_close(void* impl) {
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
static int console_msg_printer_output(void* impl, msg_type_t type, const char* file, size_t line, const char* function, const char* format, va_list args) {
    static const char *msg_type_name[] = {
        "[Error]",
        "[Warning]",
        "[Info]"
    };
    static const char *msg_type_prefix[] = {
        "\x1b[1;31m",
        "\x1b[1;33m",
        "\x1b[1;32m"
    };

    if(!impl) {
        fprintf(stderr, "Invalid console logger.\n");
        return 1;
    }

    console_msg_printer_t* printer = (console_msg_printer_t*)impl;
    if(printer->use_escape_code) {
        fprintf(stderr, "%s%s\x1b[0m %s:%zd \x1b[0;33m %s \x1b[1;37m : ", msg_type_prefix[type], msg_type_name[type], file, line, function);
    }
    else {
        fprintf(stderr, "%s %s:%zd %s : ", msg_type_name[type], file, line, function);
    }
    vfprintf(stderr, format, args);
    
    if(printer->use_escape_code) {
        fprintf(stderr, "\x1b[0m\n");
    }
    else {
        fputc('\n', stderr);
    }
    fflush(stderr);
    return 0;
}

/**
 * \brief Setups console message writer.
 * \param [in] printer Console message printer.
 * \return 0 upon success.
 */
int console_msg_printer_init(console_msg_printer_t *printer) {
    printer->super.open   = console_msg_printer_open;
    printer->super.close  = console_msg_printer_close;
    printer->super.output = console_msg_printer_output; 
    printer->use_escape_code = 0;
    return 0;
}

