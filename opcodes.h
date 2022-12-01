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
#ifndef ETRIPATOR_OPCODES_H
#define ETRIPATOR_OPCODES_H

#include "config.h"

/**
 * Opcode types:
 * -# `OPC`
 * -# `OPC A`
 * -# `OPC #nn`
 * -# `OPC #nn, ZZ`
 * -# `OPC #nn, ZZ, X`
 * -# `OPC #nn, hhll`
 * -# `OPC #nn, hhll, X`
 * -# `OPC ZZ`
 * -# `OPC ZZ, X`
 * -# `OPC ZZ, Y`
 * -# `OPC (ZZ)`
 * -# `OPC (ZZ, X)`
 * -# `OPC (ZZ), Y`
 * -# `OPC ZZ, hhll`
 * -# `OPC hhll`
 * -# `OPC (hhll)`
 * -# `OPC hhll, X`
 * -# `OPC hhll, Y`
 * -# `OPC shsl,dhdl,hhll` 
 * -# `OPC l_hhll` (label)
 * -# `OPC ZZ, l_hhll` (label)
 * -# `OPC [hhll, X]` 
 * -# `.db OPC` (unsupported opcode output as raw binary data)
 */
enum PCE_ADDRESSING_MODE {    
    PCE_OP = 0,
    PCE_OP_A,
    PCE_OP_nn,
    PCE_OP_nn_ZZ,
    PCE_OP_nn_ZZ_X,
    PCE_OP_nn_hhll,
    PCE_OP_nn_hhll_X,
    PCE_OP_ZZ,
    PCE_OP_ZZ_X,
    PCE_OP_ZZ_Y,
    PCE_OP__ZZ__,
    PCE_OP__ZZ_X__,
    PCE_OP__ZZ__Y_,
    PCE_OP_ZZ_hhll,   
    PCE_OP_hhll,
    PCE_OP__hhll__,
    PCE_OP_hhll_X,
    PCE_OP_hhll_Y,
    PCE_OP_shsl_dhdl_hhll, 
    PCE_OP__lbl__,
    PCE_OP_ZZ_lbl,
    PCE_OP__hhll_X__,
    PCE_unknown
};

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
 *
 */
const char* opcode_format(const opcode_t *op, int i);

/**
 * Is the instruction a local jump ?
 */
int opcode_is_local_jump(uint8_t op);

/**
 * Is the instruction a "far" jump ?
 */
int opcode_is_far_jump(uint8_t op);

#endif // ETRIPATOR_OPCODES_H
