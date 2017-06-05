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
#ifndef MESSAGE_H
#define MESSAGE_H

#include "config.h"

/**
 * \brief Message types
 */
typedef enum
{
	MSG_TYPE_ERROR=0,
	MSG_TYPE_WARNING,
	MSG_TYPE_INFO
} MessageType;

/**
 * \brief Initializes and allocates any resources necessary for the message printer.
 * \param [in] userData  User data.
 * \return 0 upon success.
 */
typedef int (*MsgPrinterOpen)(void* userData);

/**
 * \brief Deletes, clean up resources used by the message printer.
 * \param [in] userData  User data.
 * \return 0 upon success.
 */
typedef int (*MsgPrinterClose)(void* userData);

/**
 * \brief Prints message.
 * \param userData  User data.
 * \param type      Message type.
 * \param file      Name of the file where the print message command was issued.
 * \param line      Line number in the file where the print message command was issued.
 * \param function  Function where the print message command was issued.
 * \param format    Format string.
 *
 * \return 0 upon success.
 */
typedef int (*MsgPrinterOutput)(void* userData, MessageType type, const char* file, size_t line, const char* function, const char* format, ...);

/**
 * \brief
 */
typedef struct
{
    MsgPrinterOpen   open;
    MsgPrinterClose  close;
    MsgPrinterOutput output;
    void* userData;
} MsgPrinter;

#define ERROR_MSG(format, ...) if(g_MsgPrinter.output) g_MsgPrinter.output(g_MsgPrinter.userData, MSG_TYPE_ERROR, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)

#define WARNING_MSG(format, ...) if(g_MsgPrinter.output) g_MsgPrinter.output(g_MsgPrinter.userData, MSG_TYPE_WARNING, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)

#define INFO_MSG(format, ...) if(g_MsgPrinter.output) g_MsgPrinter.output(g_MsgPrinter.userData, MSG_TYPE_INFO, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)

/**
 * Global message printer.
 */
extern MsgPrinter g_MsgPrinter;

#endif // MESSAGE_H
