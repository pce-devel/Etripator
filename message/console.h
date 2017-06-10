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
#ifndef MESSAGE_CONSOLE_H
#define MESSAGE_CONSOLE_H

#include "../message.h"

/**
 * \brief Setups console message writer.
 * \param [in] printer Message printer.
 * \return 0 upon success.
 */
int ConsoleMsgPrinter(MsgPrinter *printer);

#endif // MESSAGE_FILE_H

