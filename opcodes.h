/*
    This file is part of Etripator,
    copyright (c) 2009--2019 Vincent Cruz.

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
#ifndef ETRIPATOR_OPCODES_H
#define ETRIPATOR_OPCODES_H

#include "config.h"

/**
 * Opcode
 */
typedef struct {
  char    name[5]; /**< name. **/
  uint8_t size;    /**< size in bytes. **/
  uint8_t type;    /**< addressing type. **/
} opcode_t;

/**
 * Get opcode description.
 */
const opcode_t* opcode_get(uint8_t op);

/**
 * Is the instruction a local jump ?
 */
int opcode_is_local_jump(uint8_t op);

/**
 * Is the instruction a "far" jump ?
 */
int opcode_is_far_jump(uint8_t op);

#endif // ETRIPATOR_OPCODES_H
