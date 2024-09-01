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
#include "file.h"

static const char* g_log_filename = "etripator.log";

/// Check if the log file can be opened and written to.
/// \param [in] printer Message printer implementation.
/// \return true if the log file was successfully opened.
/// \return false if an error occured.
static bool file_message_printer_open(MessagePrinter* printer) {
    bool ret = false;
    if(printer == NULL) {       
        fprintf(stderr, "Invalid file message printer");
    } else {
        int fd = open(g_log_filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if(fd < 0) {
            fprintf(stderr, "Failed to open log file %s: %s\n", g_log_filename, strerror(errno));
        } else if(close(fd) < 0) {
            fprintf(stderr, "Failed to close log file %s: %s\n", g_log_filename, strerror(errno));
        } else {
            ret = true;
        }
    }
    return ret;
}

/// Do nothing.
/// \param [in] printer Message printer implementation.
/// \return true always.
static bool file_message_printer_close(MessagePrinter *printer __attribute__((unused))) {
    return true;
}

/// Prints message to file.
/// \param [in] printer  Msg printer implementation.
/// \param [in] type     Message type.
/// \param [in] file     Name of the file where the print message command was issued.
/// \param [in] line     Line number in the file where the print message command was issued.
/// \param [in] function Function where the print message command was issued.
/// \param [in] format   Format string.
/// \param [in] args     Argument lists.
/// \return true if the message was successfully written to the log file.
/// \return false if an error occured.
static bool file_message_printer_output(MessagePrinter* printer, MessageType type, const char* file, size_t line, const char* function, const char* format, va_list args) {
    static const char *message_type_name[MESSAGE_TYPE_COUNT] = {
        "[Error]",
        "[Warning]",
        "[Info]",
        "[Debug]"
    };
    bool ret = false;
    if(printer == NULL) {
        fprintf(stderr, "Invalid file logger.\n");
    } else {
        FILE *out = fopen(g_log_filename, "a+");
        if(out == NULL) {
            fprintf(stderr, "Failed to open log file %s: %s\n", g_log_filename, strerror(errno));   
        } else {
            fprintf(out, "%s %s:%zd %s : ", message_type_name[type], file, line, function);
            vfprintf(out, format, args);
            fputc('\n', out);
            fflush(out);
            if(ferror(out)) {
                fprintf(stderr, "Failed to output log to %s: %s\n", g_log_filename, strerror(errno));
            } else {
                ret = true;
            }
            fclose(out);
        }
    }
    return ret;
}

static MessagePrinter g_file_message_printer = {
    .open   = file_message_printer_open,
    .close  = file_message_printer_close,
    .output = file_message_printer_output,
    .next   = NULL,
};

/* Setups file message writer. */
bool file_message_printer_init() {
    return message_printer_add(&g_file_message_printer);
}

