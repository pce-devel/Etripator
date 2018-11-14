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
#ifndef ETRIPATOR_MESSAGE_CONSOLE_H
#define ETRIPATOR_MESSAGE_CONSOLE_H

#include "../message.h"

typedef struct {
    msg_printer_t super;
    int use_escape_code; 
} console_msg_printer_t;

/**
 * \brief Setups console message writer.
 * \param [in] printer Console message printer.
 * \return 0 upon success.
 */
int console_msg_printer_init(console_msg_printer_t *printer);

#endif // ETRIPATOR_MESSAGE_FILE_H

