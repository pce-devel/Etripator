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
#ifndef ETRIPATOR_OPCODES_H
#define ETRIPATOR_OPCODES_H

#include "config.h"

/// @defgroup Opcode Opcode
///@{

/// Opcode types:
/// -# `OPC`
/// -# `OPC A`
/// -# `OPC #nn`
/// -# `OPC #nn, ZZ`
/// -# `OPC #nn, ZZ, X`
/// -# `OPC #nn, hhll`
/// -# `OPC #nn, hhll, X`
/// -# `OPC ZZ`
/// -# `OPC ZZ, X`
/// -# `OPC ZZ, Y`
/// -# `OPC (ZZ)`
/// -# `OPC (ZZ, X)`
/// -# `OPC (ZZ), Y`
/// -# `OPC ZZ, hhll`
/// -# `OPC hhll`
/// -# `OPC (hhll)`
/// -# `OPC hhll, X`
/// -# `OPC hhll, Y`
/// -# `OPC shsl,dhdl,hhll` 
/// -# `OPC l_hhll` (label)
/// -# `OPC ZZ, l_hhll` (label)
/// -# `OPC [hhll, X]` 
/// -# `.db OPC` (unsupported opcode output as raw binary data)
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

#define OPCODE_NAME_MAX_LEN 5U

/// Opcode
typedef struct {
  char    name[OPCODE_NAME_MAX_LEN]; //< name
  uint8_t size; //< size in bytes
  uint8_t type; //< addressing type
} Opcode;

/// Get opcode description
/// \param [in] op Opcode id
/// \return Pointer to opcode description
const Opcode* opcode_get(uint8_t op);

/// Get opcode format string
/// \param [in] op Pointer to opcode description
/// \param [in] i  Opcode argument id
/// \return Argument format string
/// \return NULL if the argument id is out of opcode argument count
const char* opcode_format(const Opcode *op, int i);

/// Is the instruction a local jump ?
bool opcode_is_local_jump(uint8_t op);

/// Is the instruction a "far" jump ?
bool opcode_is_far_jump(uint8_t op);

/// @}

#endif // ETRIPATOR_OPCODES_H
