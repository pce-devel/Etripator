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
#ifndef ETRIPATOR_MESSAGE_H
#define ETRIPATOR_MESSAGE_H

#include "config.h"

/**
 * \brief Message types
 */
typedef enum {
	MSG_TYPE_ERROR=0,
	MSG_TYPE_WARNING,
	MSG_TYPE_INFO
} msg_type_t;

/**
 * \brief Initializes and allocates any resources necessary for the message printer.
 * \param [in] impl Message printer.
 * \return 0 upon success.
 */
typedef int (*msg_printer_open_t)(void* impl);

/**
 * \brief Deletes, clean up resources used by the message printer.
 * \param [in] impl Message printer.
 * \return 0 upon success.
 */
typedef int (*msg_printer_close_t)(void* impl);

/**
 * \brief Prints message.
 * \param [in] impl Message printer.
 * \param [in] type      Message type.
 * \param [in] file      Name of the file where the print message command was issued.
 * \param [in] line      Line number in the file where the print message command was issued.
 * \param [in] function  Function where the print message command was issued.
 * \param [in] format    Format string.
 * \param [in] args      Argument lists.
 * \return 0 upon success.
 */
typedef int (*msg_printer_output_t)(void* impl, msg_type_t type, const char* file, size_t line, const char* function, const char* format, va_list args);

/**
 * \brief
 */
typedef struct msg_printer_t_ {
    msg_printer_open_t open;
    msg_printer_close_t close;
    msg_printer_output_t output;
    struct msg_printer_t_* next;
} msg_printer_t;

#define ERROR_MSG(format, ...) print_msg(MSG_TYPE_ERROR, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)

#define WARNING_MSG(format, ...) print_msg(MSG_TYPE_WARNING, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)

#define INFO_MSG(format, ...) print_msg(MSG_TYPE_INFO, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)

/**
 * Setup global message printer list.
 */
void msg_printer_init();
/**
 * Releases the resources used by message printers.
 */
void msg_printer_destroy();
/**
 * Adds a new message printer to the global list.
 * \param [in] printer Message printer to be added to the list.
 * \return 0 upon success.
 */
int msg_printer_add(msg_printer_t *printer);
/**
 * Dispatch messages to printers.
 * \param type      Message type.
 * \param file      Name of the file where the print message command was issued.
 * \param line      Line number in the file where the print message command was issued.
 * \param function  Function where the print message command was issued.
 * \param format    Format string.
 */
void print_msg(msg_type_t type, const char* file, size_t line, const char* function, const char* format, ...);

#endif // ETRIPATOR_MESSAGE_H
