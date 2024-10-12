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
  copyright (c) 2009--2024 Vincent Cruz.
 
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
#ifndef ETRIPATOR_MESSAGE_H
#define ETRIPATOR_MESSAGE_H

#include "config.h"

/// @defgroup Message Message printing
///@{

// Message types.
typedef enum {
    MESSAGE_TYPE_ERROR = 0,
    MESSAGE_TYPE_WARNING,
    MESSAGE_TYPE_INFO,
    MESSAGE_TYPE_DEBUG,
    MESSAGE_TYPE_COUNT,
} MessageType;

struct MessagePrinter;

/// Initializes and allocates any resources necessary for the message printer.
/// \return true if the message printer was successfully opened.
/// \return false if an error occured.
typedef bool (*MessagePrinterOpen)();

/// Releases resources used by the message printer.
/// \return true if the resources used by the message printer were successfully released.
/// \return false if an error occured.
typedef bool (*MessagePrinterClose)();

/// \brief Prints message.
/// \param [in] type     Message type.
/// \param [in] file     Name of the file where the print message command was issued.
/// \param [in] line     Line number in the file where the print message command was issued.
/// \param [in] function Function where the print message command was issued.
/// \param [in] format   Format string.
/// \param [in] args     Argument lists.
/// \return true if the message was successfully formatted and printed.
/// \return false if an error occured.
typedef bool (*MessagePrinterOutput)(MessageType type, const char* file, size_t line, const char* function, const char* format, va_list args);

/// Message printer implementation.
typedef struct MessagePrinter {
    MessagePrinterOpen open;
    MessagePrinterClose close;
    MessagePrinterOutput output;
    struct MessagePrinter* next;
} MessagePrinter;

#define ERROR_MSG(format, ...) message_print(MESSAGE_TYPE_ERROR, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)

#define WARNING_MSG(format, ...) message_print(MESSAGE_TYPE_WARNING, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)

#define INFO_MSG(format, ...) message_print(MESSAGE_TYPE_INFO, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)

#if NDEBUG
#    define DEBUG_MSG(format, ...)
#else
#    define DEBUG_MSG(format, ...) message_print(MESSAGE_TYPE_DEBUG, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif

/// Setup global message printer list.
void message_printer_init();
/// Releases resources used by message printers.
void message_printer_destroy();

/// Opens and adds a new message printer to the global list.
/// \param [in] printer Message printer to be added to the list.
/// \return true if the message printer was successfully added to the list.
/// \return false if the message printer failed to open and could not be added to the message printer list.
bool message_printer_add(MessagePrinter *printer);

/// Dispatch message to printers.
/// \param type      Message type.
/// \param file      Name of the file where the print message command was issued.
/// \param line      Line number in the file where the print message command was issued.
/// \param function  Function where the print message command was issued.
/// \param format    Format string.
void message_print(MessageType type, const char* file, size_t line, const char* function, const char* format, ...);

/// @}

#endif // ETRIPATOR_MESSAGE_H
