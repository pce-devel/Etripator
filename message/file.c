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
#include "../config.h"
#include "file.h"

static const char* g_log_filename = "etripator.log";

/**
 * \brief Opens log file.
 * \param [in] impl Msg printer implementation.
 * \return 0 upon success.
 */
static int file_msg_printer_open(void* impl) {
    int ret = 1;
    file_msg_printer_t* printer = (file_msg_printer_t*)impl; 
    if(!printer) {
        fprintf(stderr, "Invalid file logger.\n");
    } else {
        printer->out = fopen(g_log_filename, "ab");
        if(!printer->out) {
            fprintf(stderr, "Failed to open log file %s: %s\n", g_log_filename, strerror(errno));
        } else {
            ret = 0;
        }             
    }
    return ret;
}

/**
 * \brief Closes log file.
 * \param [in] impl Msg printer implementation.
 * \return 0 upon success.
 */
static int file_msg_printer_close(void* impl) {
    int ret = 1;
    file_msg_printer_t* printer = (file_msg_printer_t*)impl; 
    if((!printer) || (!printer->out)) {
        fprintf(stderr, "Invalid file logger.\n");
    } else if(fclose(printer->out)) {
        fprintf(stderr, "Failed to close log file %s : %s\n", g_log_filename, strerror(errno));
    } else {
        printer->out = NULL;
        ret = 0;
    }
    return ret;
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
static int file_msg_printer_output(void* impl, msg_type_t type, const char* file, size_t line, const char* function, const char* format, va_list args) {
    static const char *msg_type_name[] = {
        "[Error]",
        "[Warning]",
        "[Info]"
    };
    int ret = 1;
    if(!impl) {
        fprintf(stderr, "Invalid file logger.\n");
    } else {
        file_msg_printer_t* printer = (file_msg_printer_t*)impl; 

        fprintf(printer->out, "%s %s:%zd %s : ", msg_type_name[type], file, line, function);
        vfprintf(printer->out, format, args);
        fputc('\n', printer->out);
        fflush(printer->out);
        if(ferror(printer->out)) {
            fprintf(stderr, "Failed to output log to %s: %s\n", g_log_filename, strerror(errno));
        } else {
            ret = 0;
        }
    }
    return ret;
}

/**
 * \brief Setups file message writer.
 * \param [in] impl Msg printer implementation.
 * \return 0 upon success.
 */
int file_msg_printer_init(file_msg_printer_t *printer) {
    printer->super.open   = file_msg_printer_open;
    printer->super.close  = file_msg_printer_close;
    printer->super.output = file_msg_printer_output; 
    printer->out    = NULL;
    return 0;
}

