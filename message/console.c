/*
¬°¤*,¸¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸
¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯

  __/¯\____ ___/\__   _/\__   _/\_   _/\__   _/\___ ___/\__   __/\_   _/\__   
  \_  ____/_> ____ \_/  _  \_ \  <  /_    \_/     _>> ____ \_ >    \_/  _  \_ 
  _> ___/ ¯>__> <<__// __  _/ |>  ></ _/>  </  ¯  \\__> <<__//  /\  // __  _/ 
 _>  \7   <__/:. \__/:. \>  \_/   L/  _____/.  7> .\_/:. \__/  <_/ </:. \>  \_ 
|:::::::::::::::::::::::/:::::::::::::>::::::::/::::::::::::::::::::::::/:::::|
|¯¯\::::/\:/¯\::::/¯¯¯¯<::::/\::/¯¯\:/¯¯¯¯¯¯\::\::/¯¯\::::/¯¯\::::/¯¯¯¯<::::/¯|
|__ |¯¯|  T _ |¯¯¯| ___ |¯¯|  |¯| _ T ______ |¯¯¯¯| _ |¯¯¯| _ |¯¯¯| ___ |¯¯| _|
   \|__|/\|/ \|___|/   \|__|/\|_|/ \|/      \|    |/ \|___|/ \|___|/dNo\|__|/  

¬°¤*,¸¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸
¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯

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

¬°¤*,¸¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸
¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯
*/
#include "../config.h"
#include "console.h"

static bool g_use_escape_code = false;

/// Tests if the console has support for colors and other things.
/// \param [in] printer Console message printer.
/// \return true always.
static bool console_message_printer_open(MessagePrinter* printer __attribute__((unused))) {
    g_use_escape_code = isatty(fileno(stdout)) ? true : false;
    return true;
}

/// Do nothing.
/// \param [in] printer Message printer.
/// \return true always.
static bool console_message_printer_close(MessagePrinter *printer __attribute__((unused))) {
    return true;
}

/// Prints message to console.
/// \param [in] printer  Console message printer.
/// \param [in] type     Message type.
/// \param [in] file     Name of the file where the print message command was issued.
/// \param [in] line     Line number in the file where the print message command was issued.
/// \param [in] function Function where the print message command was issued.
/// \param [in] format   Format string.
/// \param [in] args     Argument lists.
/// \return true upon success.
static bool console_message_printer_output(MessagePrinter *printer, MessageType type, const char* file, size_t line, const char* function, const char* format, va_list args) {
    static const char *message_type_name[] = {
        "[Error]",
        "[Warning]",
        "[Info]"
    };
    static const char *message_type_prefix[] = {
        "\x1b[1;31m",
        "\x1b[1;33m",
        "\x1b[1;32m"
    };

    bool ret = true;
    if(printer == NULL) {
        fprintf(stderr, "Invalid console logger.\n");
        ret = false;
    } else if(g_use_escape_code) {
        fprintf(stderr, "%s%s\x1b[0m %s:%zd \x1b[0;33m %s \x1b[1;37m : "
                      , message_type_prefix[type]
                      , message_type_name[type]
                      , file
                      , line
                      , function
        );
    } else {
        fprintf(stderr, "%s %s:%zd %s : "
                      , message_type_name[type]
                      , file
                      , line
                      , function
        );
    }

    vfprintf(stderr, format, args);
    
    if(g_use_escape_code) {
        fprintf(stderr, "\x1b[0m\n");
    } else {
        fputc('\n', stderr);
    }
    fflush(stderr);
    return ret;
}

static MessagePrinter g_console_message_printer = {
    .open   = console_message_printer_open,
    .close  = console_message_printer_close,
    .output = console_message_printer_output,
    .next   = NULL,
};

/* Setups console message writer. */
bool console_message_printer_init() {
    g_use_escape_code = false;
    return message_printer_add(&g_console_message_printer);
}

