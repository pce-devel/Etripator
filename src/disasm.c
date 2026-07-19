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
  copyright (c) 2009--2026 Vincent Cruz.
 
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
#include <etripator/disasm.h>

#include <etripator/utils.h>
#include <etripator/string.h>
#include <etripator/memory_map.h>
#include <etripator/label.h>

#define PCE_ARG_NONE     (0)
#define PCE_ARG_A        (1)
#define PCE_ARG_X        (2)
#define PCE_ARG_Y        (3)
#define PCE_ARG_ABS      (4)
#define PCE_ARG_ZP       (5)
#define PCE_ARG_HHLL     (6)
#define PCE_ARG_INDIRECT (1<<7U)

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
/// -# `OPC ZZ, hhll`
/// -# `OPC (ZZ)`
/// -# `OPC (ZZ, X)`
/// -# `OPC (ZZ), Y`
/// -# `OPC hhll`
/// -# `OPC hhll, X`
/// -# `OPC hhll, Y`
/// -# `OPC (hhll)`
/// -# `OPC (hhll, X)`
/// -# `OPC shsl,dhdl,hhll` 
/// -# `.db OPC` (unsupported opcode output as raw binary data)
enum PCE_ADDRESSING_MODE {
    PCE_OP = 0,
    PCE_OP_A,
    PCE_OP_ABS,
    PCE_OP_ABS_ZP,
    PCE_OP_ABS_ZP_X,
    PCE_OP_ABS_HHLL,
    PCE_OP_ABS_HHLL_X,
    PCE_OP_ZZ,
    PCE_OP_ZZ_X,
    PCE_OP_ZZ_Y,
    PCE_OP_ZZ_HHLL,   
    PCE_OP_ZZ_IND,
    PCE_OP_ZZ_X_IND,
    PCE_OP_ZZ_IND_Y,
    PCE_OP_HHLL,
    PCE_OP_HHLL_X,
    PCE_OP_HHLL_Y,
    PCE_OP_HHLL_IND,
    PCE_OP_HHLL_X_IND,
    PCE_OP_HHLL_HHLL_HHLL,
    PCE_TYPE_COUNT,
};

enum PCE_TAG {
    PCE_TAG_NONE = 0,
    PCE_TAG_LOCAL_JUMP,
    PCE_TAG_MMAP,
    PCE_TAG_UNKNOWN,
};

/// Opcode output string
static const uint16_t pce_opstring[PCE_TYPE_COUNT][3] = {
    [PCE_OP] = { PCE_ARG_NONE, PCE_ARG_NONE, PCE_ARG_NONE},
    [PCE_OP_A] = { PCE_ARG_A, PCE_ARG_NONE, PCE_ARG_NONE},
    [PCE_OP_ABS] = { PCE_ARG_ABS, PCE_ARG_NONE, PCE_ARG_NONE},
    [PCE_OP_ABS_ZP] = { PCE_ARG_ABS, PCE_ARG_ZP, PCE_ARG_NONE},
    [PCE_OP_ABS_ZP_X] = { PCE_ARG_ABS, PCE_ARG_ZP, PCE_ARG_X},
    [PCE_OP_ABS_HHLL] = { PCE_ARG_ABS, PCE_ARG_HHLL, PCE_ARG_NONE},
    [PCE_OP_ABS_HHLL_X] = { PCE_ARG_ABS, PCE_ARG_HHLL, PCE_ARG_X},
    [PCE_OP_ZZ] = { PCE_ARG_ZP, PCE_ARG_NONE, PCE_ARG_NONE},
    [PCE_OP_ZZ_X] = { PCE_ARG_ZP, PCE_ARG_X, PCE_ARG_NONE},
    [PCE_OP_ZZ_Y] = { PCE_ARG_ZP, PCE_ARG_Y, PCE_ARG_NONE},
    [PCE_OP_ZZ_HHLL] = { PCE_ARG_ZP, PCE_ARG_HHLL, PCE_ARG_NONE },
    [PCE_OP_ZZ_IND] = { PCE_ARG_INDIRECT | PCE_ARG_ZP, PCE_ARG_INDIRECT | PCE_ARG_X, PCE_ARG_NONE},
    [PCE_OP_ZZ_X_IND] = { PCE_ARG_INDIRECT | PCE_ARG_ZP, PCE_ARG_INDIRECT | PCE_ARG_X, PCE_ARG_NONE},
    [PCE_OP_ZZ_IND_Y] = { PCE_ARG_INDIRECT | PCE_ARG_ZP, PCE_ARG_Y, PCE_ARG_NONE},
    [PCE_OP_HHLL] = { PCE_ARG_HHLL, PCE_ARG_NONE},
    [PCE_OP_HHLL_X] = { PCE_ARG_HHLL, PCE_ARG_X, PCE_ARG_NONE},
    [PCE_OP_HHLL_Y] = { PCE_ARG_HHLL, PCE_ARG_Y, PCE_ARG_NONE},
    [PCE_OP_HHLL_IND] = { PCE_ARG_INDIRECT | PCE_ARG_HHLL, PCE_ARG_NONE, PCE_ARG_NONE},
    [PCE_OP_HHLL_X_IND] = { PCE_ARG_INDIRECT | PCE_ARG_HHLL, PCE_ARG_INDIRECT | PCE_ARG_X, PCE_ARG_NONE},
    [PCE_OP_HHLL_HHLL_HHLL] = { PCE_ARG_HHLL, PCE_ARG_HHLL, PCE_ARG_HHLL},
};

static bool pce_arg(MemoryMap *map, LabelRepository *repository, String *out, uint16_t arg, uint8_t data[2]) {
    SANITY_CHECK((map != NULL) && (repository != NULL) && (out != NULL), false);
    
    Label label = {0};
    switch(arg) {
        case PCE_ARG_A:
            return string_append_n(out, 'A', 1);
        case PCE_ARG_X:
            return string_append_n(out, 'X', 1);
        case PCE_ARG_Y:
            return string_append_n(out, 'Y', 1);
        case PCE_ARG_ABS:
            return string_append_format(out, "#$%02x", data[0]);
        case PCE_ARG_ZP:
            {
                uint16_t address = 0x2000U | data[0];
                uint8_t page = memory_map_page(map, address);
                if (label_repository_find(repository, address, page, &label)) {
                    return string_append_format(out, "<%s", label.name); 
                } else {
                    return string_append_format(out, "<$%02x", data[0]); 
                }
            }
        case PCE_ARG_HHLL:
            {
                uint16_t address = (data[1] << 8) | data[0];
                uint8_t page = memory_map_page(map, address);
                if (label_repository_find(repository, address, page, &label)) {
                    return string_append(out, label.name, strlen(label.name)); 
                } else {
                    return string_append_format(out, "$%04x", address); 
                }
            }
        default:
            return false;
    }
}

static bool pce_arg_list(MemoryMap *map, LabelRepository *repository, String *out, uint8_t addressing_mode, uint8_t *data, size_t length) {
    SANITY_CHECK((map != NULL) && (repository != NULL) && (out != NULL) && (data != NULL), false);
    SANITY_CHECK(addressing_mode < PCE_TYPE_COUNT, false);

    const uint16_t *arg = pce_opstring[addressing_mode];
    const uint8_t *end = data + length;
    uint8_t previous = PCE_ARG_NONE;
    for(size_t i=0; (i<3) && (data < end) && (arg[i]!=PCE_ARG_NONE); i++) {
        uint8_t current = arg[i];

        if((current & PCE_ARG_INDIRECT) != (previous & PCE_ARG_INDIRECT)) {
            char delim = (current & PCE_ARG_INDIRECT) ? '[' : ']';
            if(string_append_n(out, delim, 1) != true) {
                return false;
            }
        }
        if(i > 0) {
            if(string_append_n(out, ',', 1) != true) {
                return false;
            }   
        }

        previous = current;
        current &= 0x3FU;

        if(pce_arg(map, repository, out, current, data) != true) {
            return false;
        }

        if((current == PCE_ARG_ABS) || (current == PCE_ARG_ZP)) {
            data++;
        } else if(current == PCE_ARG_HHLL) {
            data += 2;
        } 
    }
    if(previous & PCE_ARG_INDIRECT) {
        if(string_append_n(out, ']', 1) != true) {
            return false;
        }
    }
    return true;
}

#define OPCODE_NAME_MAX_LEN 5U

/// Opcode
typedef struct {
  char    name[OPCODE_NAME_MAX_LEN]; //< name
  uint8_t size; //< size in bytes
  uint8_t type; //< addressing type
  uint8_t tag;  //< processing tag
} Opcode;

// PC engine opcodes
static const Opcode g_pce_opcode[256] = {
  /* 00: BRK                 */  { "brk ", 1, PCE_OP, PCE_TAG_NONE },
  /* 01: ORA  (ZZ, X)        */  { "ora ", 2, PCE_OP_ZZ_X_IND, PCE_TAG_NONE },
  /* 02: SXY                 */  { "sxy ", 1, PCE_OP, PCE_TAG_NONE },
  /* 03: STO  #nn            */  { "st0 ", 2, PCE_OP_ABS, PCE_TAG_NONE },
  /* 04: TSB  ZZ             */  { "tsb ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* O5: ORA  ZZ             */  { "ora ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 06: ASL  ZZ             */  { "asl ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 07: RMB0 ZZ             */  { "rmb0", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 08: PHP                 */  { "php ", 1, PCE_OP, PCE_TAG_NONE },
  /* O9: ORA  #nn            */  { "ora ", 2, PCE_OP_ABS, PCE_TAG_NONE },
  /* 0A: ASL  A              */  { "asl ", 1, PCE_OP_A, PCE_TAG_NONE },
  /* 0B: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* 0C: TRB  hhll           */  { "tsb ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* 0D: ORA  hhll           */  { "ora ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* 0E: ASL  hhll           */  { "asl ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* 0F: BBR0 ZZ, lhhll      */  { "bbr0", 3, PCE_OP_ZZ_HHLL, PCE_TAG_LOCAL_JUMP },
  /* 10: BPL  lhhll          */  { "bpl ", 2, PCE_OP_HHLL, PCE_TAG_LOCAL_JUMP},      
  /* 11: ORA  (ZZ), Y        */  { "ora ", 2, PCE_OP_ZZ_IND_Y, PCE_TAG_NONE },
  /* 12: ORA  (ZZ)           */  { "ora ", 2, PCE_OP_ZZ_IND, PCE_TAG_NONE },
  /* 13: ST1  #nn            */  { "st1 ", 2, PCE_OP_ABS, PCE_TAG_NONE },
  /* 14: TRB  ZZ             */  { "trb ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 15: ORA  ZZ, X          */  { "ora ", 2, PCE_OP_ZZ_X, PCE_TAG_NONE },
  /* 16: ASL  ZZ, X          */  { "asl ", 2, PCE_OP_ZZ_X, PCE_TAG_NONE },
  /* 17: RMB1 ZZ             */  { "rmb1", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 18: CLC                 */  { "clc ", 1, PCE_OP, PCE_TAG_NONE },
  /* 19: ORA  hhll, Y        */  { "ora ", 3, PCE_OP_HHLL_Y, PCE_TAG_NONE },
  /* 1A: INC  A              */  { "inc ", 1, PCE_OP_A, PCE_TAG_NONE },
  /* 1B: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* 1C: TRB  hhll           */  { "trb ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* 1D: ORA  hhll, X        */  { "ora ", 3, PCE_OP_HHLL_X, PCE_TAG_NONE },
  /* 1E: ASL  hhll, X        */  { "asl ", 3, PCE_OP_HHLL_X, PCE_TAG_NONE },
  /* 1F: BBR1 ZZ, lhhll      */  { "bbr1", 3, PCE_OP_ZZ_HHLL, PCE_TAG_LOCAL_JUMP },
  /* 20: JSR  lhhll          */  { "jsr ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* 21: AND  (ZZ, X)        */  { "and ", 2, PCE_OP_ZZ_X_IND, PCE_TAG_NONE },
  /* 22: SAX                 */  { "sax ", 1, PCE_OP, PCE_TAG_NONE },
  /* 23: ST2  #nn            */  { "st2 ", 2, PCE_OP_ABS, PCE_TAG_NONE },
  /* 24: BIT  ZZ             */  { "bit ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 25: AND  ZZ             */  { "and ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 26: ROL  ZZ             */  { "rol ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 27: RMB2 ZZ             */  { "rmb2", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 28: PLP                 */  { "plp ", 1, PCE_OP, PCE_TAG_NONE },
  /* 29: AND  #nn            */  { "and ", 2, PCE_OP_ABS, PCE_TAG_NONE },
  /* 2A: ROL  A              */  { "rol ", 1, PCE_OP_A, PCE_TAG_NONE },
  /* 2B: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* 2C: BIT  hhll           */  { "bit ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* 2D: AND  hhll           */  { "and ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* 2E: ROL  hhll           */  { "rol ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* 2F: BBR2 ZZ, lhhll      */  { "bbr2", 3, PCE_OP_ZZ_HHLL, PCE_TAG_LOCAL_JUMP },
  /* 30: BMI  lhhll          */  { "bmi ", 2, PCE_OP_HHLL, PCE_TAG_LOCAL_JUMP },
  /* 31: AND  (ZZ), Y        */  { "and ", 2, PCE_OP_ZZ_IND_Y, PCE_TAG_NONE },
  /* 32: AND  (ZZ)           */  { "and ", 2, PCE_OP_ZZ_IND, PCE_TAG_NONE },
  /* 33: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* 34: BIT  ZZ, X          */  { "bit ", 2, PCE_OP_ZZ_X, PCE_TAG_NONE },
  /* 35: AND  ZZ, X          */  { "and ", 2, PCE_OP_ZZ_X, PCE_TAG_NONE },
  /* 36: ROL  ZZ             */  { "rol ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 37: RMB3 ZZ             */  { "rmb3", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 38: SEC                 */  { "sec ", 1, PCE_OP, PCE_TAG_NONE },
  /* 39: AND  hhll, Y        */  { "and ", 3, PCE_OP_HHLL_Y, PCE_TAG_NONE },
  /* 3A: DEC  A              */  { "dec ", 1, PCE_OP_A, PCE_TAG_NONE },
  /* 3B: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* 3C: BIT  hhll, X        */  { "bit ", 3, PCE_OP_HHLL_X, PCE_TAG_NONE },
  /* 3D: AND  hhll, X        */  { "and ", 3, PCE_OP_HHLL_X, PCE_TAG_NONE },
  /* 3E: ROL  hhll, X        */  { "rol ", 3, PCE_OP_HHLL_X, PCE_TAG_NONE },
  /* 3F: BBR3 ZZ, lhhll      */  { "bbr3", 3, PCE_OP_ZZ_HHLL, PCE_TAG_LOCAL_JUMP },
  /* 40: RTI                 */  { "rti ", 1, PCE_OP, PCE_TAG_NONE },
  /* 41: EOR  (ZZ, X)        */  { "eor ", 2, PCE_OP_ZZ_X_IND, PCE_TAG_NONE },
  /* 42: SAY                 */  { "say ", 1, PCE_OP, PCE_TAG_NONE },
  /* 43: TMA  #nn            */  { "tma ", 2, PCE_OP_ABS, PCE_TAG_MMAP },
  /* 44: BSR  lhhll          */  { "bsr ", 2, PCE_OP_HHLL, PCE_TAG_LOCAL_JUMP },
  /* 45: EOR  ZZ             */  { "eor ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 46: LSR  ZZ             */  { "lsr ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 47: RMB4 ZZ             */  { "rmb4", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 48: PHA                 */  { "pha ", 1, PCE_OP, PCE_TAG_NONE },
  /* 49: EOR  #nn            */  { "eor ", 2, PCE_OP_ABS, PCE_TAG_NONE },
  /* 4A: LSR  A              */  { "lsr ", 1, PCE_OP_A, PCE_TAG_NONE },
  /* 4B: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* 4C: JMP  lhhll          */  { "jmp ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* 4D: EOR  hhll           */  { "eor ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* 4E: LSR  hhll           */  { "lsr ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* 4F: BBR4 ZZ, lhhll      */  { "bbr4", 3, PCE_OP_ZZ_HHLL, PCE_TAG_LOCAL_JUMP },
  /* 50: BVC  lhhll          */  { "bvc ", 2, PCE_OP_HHLL, PCE_TAG_LOCAL_JUMP },
  /* 51: EOR  (ZZ), Y        */  { "eor ", 2, PCE_OP_ZZ_IND_Y, PCE_TAG_NONE },
  /* 52: EOR  (ZZ)           */  { "eor ", 2, PCE_OP_ZZ_IND, PCE_TAG_NONE },
  /* 53: TAM  #nn            */  { "tam ", 2, PCE_OP_ABS, PCE_TAG_MMAP },
  /* 54: CSL                 */  { "csl ", 1, PCE_OP, PCE_TAG_NONE },
  /* 55: EOR  ZZ, X          */  { "eor ", 2, PCE_OP_ZZ_X, PCE_TAG_NONE },
  /* 56: LSR  ZZ, X          */  { "lsr ", 2, PCE_OP_ZZ_X, PCE_TAG_NONE },
  /* 57: RMB5 ZZ             */  { "rmb5", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 58: CLI                 */  { "cli ", 1, PCE_OP, PCE_TAG_NONE },
  /* 59: EOR  hhll, Y        */  { "eor ", 3, PCE_OP_HHLL_Y, PCE_TAG_NONE },
  /* 5A: PHY                 */  { "phy ", 1, PCE_OP, PCE_TAG_NONE },
  /* 5B: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* 5C: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* 5D: EOR  hhll, X        */  { "eor ", 3, PCE_OP_HHLL_X, PCE_TAG_NONE },
  /* 5E: LSR  hhll, X        */  { "lsr ", 3, PCE_OP_HHLL_X, PCE_TAG_NONE },
  /* 5F: BBR5 ZZ, lhhll      */  { "bbr5", 3, PCE_OP_ZZ_HHLL, PCE_TAG_LOCAL_JUMP },
  /* 60: RTS                 */  { "rts ", 1, PCE_OP, PCE_TAG_NONE },
  /* 61: ADC  (ZZ, X)        */  { "adc ", 2, PCE_OP_ZZ_X_IND, PCE_TAG_NONE },
  /* 62: CLA                 */  { "cla ", 1, PCE_OP, PCE_TAG_NONE },
  /* 63: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* 64: STZ  ZZ             */  { "stz ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 65: ADC  ZZ             */  { "adc ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 66: ROR  ZZ             */  { "ror ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 67: RMB6 ZZ             */  { "rmb6", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 68: PLA                 */  { "pla ", 1, PCE_OP, PCE_TAG_NONE },
  /* 69: ADC  #nn            */  { "adc ", 2, PCE_OP_ABS, PCE_TAG_NONE },
  /* 6A: ROR  A              */  { "ror ", 1, PCE_OP_A, PCE_TAG_NONE },
  /* 6B: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* 6C: JMP  (hhll)         */  { "jmp ", 3, PCE_OP_HHLL_IND, PCE_TAG_NONE },
  /* 6D: ADC  hhll           */  { "adc ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* 6E: ROR  hhll           */  { "ror ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* 6F: BBR6 ZZ, lhhll      */  { "bbr6", 3, PCE_OP_ZZ_HHLL, PCE_TAG_LOCAL_JUMP },
  /* 70: BVS  lhhll          */  { "bvs ", 2, PCE_OP_HHLL, PCE_TAG_LOCAL_JUMP },
  /* 71: ADC  (ZZ), Y        */  { "adc ", 2, PCE_OP_ZZ_IND_Y, PCE_TAG_NONE },
  /* 72: ADC  (ZZ)           */  { "adc ", 2, PCE_OP_ZZ_IND, PCE_TAG_NONE },
  /* 73: TII  shsl,dhdl,lhlh */  { "tii ", 7, PCE_OP_HHLL_HHLL_HHLL, PCE_TAG_NONE },
  /* 74: STZ  ZZ, X          */  { "stz ", 2, PCE_OP_ZZ_X, PCE_TAG_NONE },
  /* 75: ADC  ZZ, X          */  { "adc ", 2, PCE_OP_ZZ_X, PCE_TAG_NONE },
  /* 76: ROR  ZZ, X          */  { "ror ", 2, PCE_OP_ZZ_X, PCE_TAG_NONE },
  /* 77: RMB7 ZZ             */  { "rmb7", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 78: SEI                 */  { "sei ", 1, PCE_OP, PCE_TAG_NONE },
  /* 79: ADC  hhll, Y        */  { "adc ", 3, PCE_OP_HHLL_Y, PCE_TAG_NONE },
  /* 7A: PLY                 */  { "ply ", 1, PCE_OP, PCE_TAG_NONE },
  /* 7B: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* 7C: JMP  (hhll, X)      */  { "jmp ", 3, PCE_OP_HHLL_X_IND, PCE_TAG_NONE },
  /* 7D: ADC  hhll, X        */  { "adc ", 3, PCE_OP_HHLL_X, PCE_TAG_NONE },
  /* 7E: ROR  hhll, X        */  { "ror ", 3, PCE_OP_HHLL_X, PCE_TAG_NONE },
  /* 7F: BBR7 ZZ, lhhll      */  { "bbr7", 3, PCE_OP_ZZ_HHLL, PCE_TAG_LOCAL_JUMP },
  /* 80: BRA  lhhll          */  { "bra ", 2, PCE_OP_HHLL, PCE_TAG_LOCAL_JUMP },
  /* 81: STA  (ZZ, X)        */  { "sta ", 2, PCE_OP_ZZ_X_IND, PCE_TAG_NONE },
  /* 82: CLX                 */  { "clx ", 1, PCE_OP, PCE_TAG_NONE },
  /* 83: TST  #nn, ZZ        */  { "tst ", 3, PCE_OP_ABS_ZP, PCE_TAG_NONE },
  /* 84: STY  ZZ             */  { "sty ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 85: STA  ZZ             */  { "sta ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 86: STX  ZZ             */  { "stx ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 87: SMB0 ZZ             */  { "smb0", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 88: DEY                 */  { "dey ", 1, PCE_OP, PCE_TAG_NONE },
  /* 89: BIT  #nn            */  { "bit ", 2, PCE_OP_ABS, PCE_TAG_NONE }, 
  /* 8A: TXA                 */  { "txa ", 1, PCE_OP, PCE_TAG_NONE },
  /* 8B: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* 8C: STY  hhll           */  { "sty ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* 8D: STA  hhll           */  { "sta ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* 8E: STX  hhll           */  { "stx ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* 8F: BBS0 ZZ, lhhll      */  { "bbs0", 3, PCE_OP_ZZ_HHLL, PCE_TAG_LOCAL_JUMP },
  /* 90: BCC  lhhll          */  { "bcc ", 2, PCE_OP_HHLL, PCE_TAG_LOCAL_JUMP },
  /* 91: STA  (ZZ), Y        */  { "sta ", 2, PCE_OP_ZZ_IND_Y, PCE_TAG_NONE },
  /* 92: STA  (ZZ)           */  { "sta ", 2, PCE_OP_ZZ_IND, PCE_TAG_NONE },
  /* 93: TST  #nn, hhll      */  { "tst ", 4, PCE_OP_ABS_HHLL, PCE_TAG_NONE },
  /* 94: STY  ZZ, X          */  { "sty ", 2, PCE_OP_ZZ_X, PCE_TAG_NONE },
  /* 95: STA  ZZ, X          */  { "sta ", 2, PCE_OP_ZZ_X, PCE_TAG_NONE },
  /* 96: STX  ZZ, Y          */  { "stx ", 2, PCE_OP_ZZ_Y, PCE_TAG_NONE },
  /* 97: SMB1 ZZ             */  { "smb1", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* 98: TYA                 */  { "tya ", 1, PCE_OP, PCE_TAG_NONE },
  /* 99: STA  hhll, Y        */  { "sta ", 3, PCE_OP_HHLL_Y, PCE_TAG_NONE },
  /* 9A: TXS                 */  { "txs ", 1, PCE_OP, PCE_TAG_NONE },
  /* 9B: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* 9C: STZ  hhll           */  { "stz ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* 9D: STA  hhll, X        */  { "sta ", 3, PCE_OP_HHLL_X, PCE_TAG_NONE },
  /* 9E: STZ  hhll, X        */  { "stz ", 3, PCE_OP_HHLL_X, PCE_TAG_NONE },
  /* 9F: BBS1 ZZ, lhhll      */  { "bbs1", 3, PCE_OP_ZZ_HHLL, PCE_TAG_LOCAL_JUMP },
  /* A0: LDY  #nn            */  { "ldy ", 2, PCE_OP_ABS, PCE_TAG_NONE },
  /* A1: LDA  (ZZ, X)        */  { "lda ", 2, PCE_OP_ZZ_X_IND, PCE_TAG_NONE },
  /* A2: LDA  #nn            */  { "ldx ", 2, PCE_OP_ABS, PCE_TAG_NONE },
  /* A3: TST  #nn, ZZ, X     */  { "tst ", 3, PCE_OP_ABS_ZP_X, PCE_TAG_NONE },
  /* A4: LDY  ZZ             */  { "ldy ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* A5: LDA  ZZ             */  { "lda ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* A6: LDX  ZZ             */  { "ldx ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* A7: SMB2 ZZ             */  { "smb2", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* A8: TAY                 */  { "tay ", 1, PCE_OP, PCE_TAG_NONE },
  /* A9: LDA  #nn            */  { "lda ", 2, PCE_OP_ABS, PCE_TAG_NONE },
  /* AA: TAX                 */  { "tax ", 1, PCE_OP, PCE_TAG_NONE },
  /* AB: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* AC: LDY  hhll           */  { "ldy ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* AD: LDA  hhll           */  { "lda ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* AE: LDX  hhll           */  { "ldx ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* AF: BBS2 ZZ, lhhll      */  { "bbs2", 3, PCE_OP_ZZ_HHLL, PCE_TAG_LOCAL_JUMP },
  /* B0: BCS  lhhll          */  { "bcs ", 2, PCE_OP_HHLL, PCE_TAG_LOCAL_JUMP },
  /* B1: LDA  (ZZ), Y        */  { "lda ", 2, PCE_OP_ZZ_IND_Y, PCE_TAG_NONE },
  /* B2: LDA  (ZZ)           */  { "lda ", 2, PCE_OP_ZZ_IND, PCE_TAG_NONE },
  /* B3: TST  #nn, hhll, X   */  { "tst ", 4, PCE_OP_ABS_HHLL_X, PCE_TAG_NONE },
  /* B4: LDY  ZZ, X          */  { "ldy ", 2, PCE_OP_ZZ_X, PCE_TAG_NONE },
  /* B5: LDA  ZZ, X          */  { "lda ", 2, PCE_OP_ZZ_X, PCE_TAG_NONE },
  /* B6: LDX  ZZ, Y          */  { "ldx ", 2, PCE_OP_ZZ_Y, PCE_TAG_NONE },
  /* B7: SMB3 ZZ             */  { "smb3", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* B8: CLV                 */  { "clv ", 1, PCE_OP, PCE_TAG_NONE },
  /* B9: LDA  hhll, Y        */  { "lda ", 3, PCE_OP_HHLL_Y, PCE_TAG_NONE },
  /* BA: TSX                 */  { "tsx ", 1, PCE_OP, PCE_TAG_NONE },
  /* BB: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* BC: LDY  hhll, X        */  { "ldy ", 3, PCE_OP_HHLL_X, PCE_TAG_NONE },
  /* BD: LDA  hhll, X        */  { "lda ", 3, PCE_OP_HHLL_X, PCE_TAG_NONE },
  /* BE: LDX  hhll, Y        */  { "ldx ", 3, PCE_OP_HHLL_Y, PCE_TAG_NONE },
  /* BF: BBS3 ZZ, lhhll      */  { "bbs3", 3, PCE_OP_ZZ_HHLL, PCE_TAG_LOCAL_JUMP },
  /* C0: CPY  #nn            */  { "cpy ", 2, PCE_OP_ABS, PCE_TAG_NONE },
  /* C1: CMP  (ZZ, X)        */  { "cmp ", 2, PCE_OP_ZZ_X_IND, PCE_TAG_NONE },
  /* C2: CLY                 */  { "cly ", 1, PCE_OP, PCE_TAG_NONE },
  /* C3: TDD  shsl,dhdl,lhlh */  { "tdd ", 7, PCE_OP_HHLL_HHLL_HHLL, PCE_TAG_NONE },
  /* C4: CPY  ZZ             */  { "cpy ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* C5: CMP  ZZ             */  { "cmp ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* C6: DEC  ZZ             */  { "dec ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* C7: SMB4 ZZ             */  { "smb4", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* C8: INY                 */  { "iny ", 1, PCE_OP, PCE_TAG_NONE },
  /* C9: CMP  #nn            */  { "cmp ", 2, PCE_OP_ABS, PCE_TAG_NONE },
  /* CA: DEX                 */  { "dex ", 1, PCE_OP, PCE_TAG_NONE },
  /* CB: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* CC: CPY  hhll           */  { "cpy ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* CD: CMP  hhll           */  { "cmp ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* CE: DEC  hhll           */  { "dec ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* CF: BBS4 ZZ, lhhll      */  { "bbs4", 3, PCE_OP_ZZ_HHLL, PCE_TAG_LOCAL_JUMP},
  /* D0: BNE  lhhll          */  { "bne ", 2, PCE_OP_HHLL, PCE_TAG_LOCAL_JUMP },
  /* D1: CMP  (ZZ), Y        */  { "cmp ", 2, PCE_OP_ZZ_IND_Y, PCE_TAG_NONE },
  /* D2: CMP  (ZZ)           */  { "cmp ", 2, PCE_OP_ZZ_IND, PCE_TAG_NONE },
  /* D3: TIN  shsl,dhdl,lhlh */  { "tin ", 7, PCE_OP_HHLL_HHLL_HHLL, PCE_TAG_NONE },
  /* D4: CSH                 */  { "csh ", 1, PCE_OP, PCE_TAG_NONE },
  /* D5: CMP  ZZ, X          */  { "cmp ", 2, PCE_OP_ZZ_X, PCE_TAG_NONE },
  /* D6: DEC  ZZ, X          */  { "dec ", 2, PCE_OP_ZZ_X, PCE_TAG_NONE },
  /* D7: SMB5 ZZ             */  { "smb5", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* D8: CLD                 */  { "cld ", 1, PCE_OP, PCE_TAG_NONE },
  /* D9: CMP  hhll, Y        */  { "cmp ", 3, PCE_OP_HHLL_Y, PCE_TAG_NONE },
  /* DA: PHX                 */  { "phx ", 1, PCE_OP, PCE_TAG_NONE },
  /* DB: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* DC: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* DD: CMP  hhll, X        */  { "cmp ", 3, PCE_OP_HHLL_X, PCE_TAG_NONE },
  /* DE: DEC  hhll, X        */  { "dec ", 3, PCE_OP_HHLL_X, PCE_TAG_NONE },
  /* DF: BBS5 ZZ, lhhll      */  { "bbs5", 3, PCE_OP_ZZ_HHLL, PCE_TAG_LOCAL_JUMP },
  /* E0: CPX  #nn            */  { "cpx ", 2, PCE_OP_ABS, PCE_TAG_NONE },
  /* E1: SBC  (ZZ,X)         */  { "sbc ", 2, PCE_OP_ZZ_X_IND, PCE_TAG_NONE },
  /* E2: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* E3: TIA  shsl,dhdl,lhlh */  { "tia ", 7, PCE_OP_HHLL_HHLL_HHLL, PCE_TAG_NONE },
  /* E4: CPX  ZZ             */  { "cpx ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* E5: SBC  ZZ             */  { "sbc ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* E6: INC  ZZ             */  { "inc ", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* E7: SMB6 ZZ             */  { "smb6", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* E8: INX                 */  { "inx ", 1, PCE_OP, PCE_TAG_NONE },
  /* E9: SBC  #nn            */  { "sbc ", 2, PCE_OP_ABS, PCE_TAG_NONE },
  /* EA: NOP                 */  { "nop ", 1, PCE_OP, PCE_TAG_NONE },
  /* EB: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* EC: CPX  hhll           */  { "cpx ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* ED: SBC  hhll           */  { "sbc ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* EE: INC  hhll           */  { "inc ", 3, PCE_OP_HHLL, PCE_TAG_NONE },
  /* EF: BBS6 ZZ, lhhll      */  { "bbs6", 3, PCE_OP_ZZ_HHLL, PCE_TAG_LOCAL_JUMP },
  /* F0: BEQ  lhhll          */  { "beq ", 2, PCE_OP_HHLL, PCE_TAG_LOCAL_JUMP },
  /* F1: SBC  (ZZ), Y        */  { "sbc ", 2, PCE_OP_ZZ_IND_Y, PCE_TAG_NONE },
  /* F2: SBC  (ZZ)           */  { "sbc ", 2, PCE_OP_ZZ_IND, PCE_TAG_NONE },
  /* F3: TAI  shsl,dhdl,lhlh */  { "tai ", 7, PCE_OP_HHLL_HHLL_HHLL, PCE_TAG_NONE },
  /* F4: SET                 */  { "set ", 1, PCE_OP, PCE_TAG_NONE },
  /* F5: SBC  ZZ, X          */  { "sbc ", 2, PCE_OP_ZZ_X, PCE_TAG_NONE },
  /* F6: INC  ZZ, X          */  { "inc ", 2, PCE_OP_ZZ_X, PCE_TAG_NONE },
  /* F7: SMB7 ZZ             */  { "smb7", 2, PCE_OP_ZZ, PCE_TAG_NONE },
  /* F8: SED                 */  { "sed ", 1, PCE_OP, PCE_TAG_NONE },
  /* F9: SBC  hhll, Y        */  { "sbc ", 3, PCE_OP_HHLL_Y, PCE_TAG_NONE },
  /* FA: PLX                 */  { "plx ", 1, PCE_OP, PCE_TAG_NONE },
  /* FB: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* FC: UNUSED              */  { ".db ", 1, PCE_OP_ABS, PCE_TAG_UNKNOWN },
  /* FD: SBC  hhll, X        */  { "sbc ", 3, PCE_OP_HHLL_X, PCE_TAG_NONE },
  /* FE: INC  hhll, X        */  { "inc ", 3, PCE_OP_HHLL_X, PCE_TAG_NONE },
  /* FF: BBS7 ZZ, lhhll      */  { "bbs7", 3, PCE_OP_ZZ_HHLL,  PCE_TAG_LOCAL_JUMP }
};

static void pce_tag_local_jump(uint16_t logical, uint8_t data[8], size_t *size) {
    // Heuristic to detect bb{sr}[0-7] instructions.
    size_t i = ((data[0] & 0x0FU) == 0x0FU) ? 2 : 1;

    // Detect negative number
    int16_t delta;
    if(data[i] & 0x80U) {
        delta = -((data[i] - 1) ^ 0xFFU);
    } else {
        delta = (int16_t) data[i];
    }
    uint16_t address = logical + delta;
    data[i+0] = address & 0xFFU;
    data[i+1] = address >> 8;
    *size += 1;
}

static void pce_tag_mmap(uint16_t logical __unused, uint8_t data[8], size_t *size __unused) {
    // Compute log base 2 of data
    if(data[1] == 0) {
        data[1] = 0;
    } else {
       data[1] = 31 - __builtin_clz((uint32_t)data[1]);
    }
}

static void pce_tag_unknown(uint16_t logical __unused, uint8_t data[8], size_t *size) {
    // duplicate opcode as data
    data[1] = data[0];
    *size = 1;
}

static void pce_tag(uint16_t logical, uint8_t data[8], size_t *size) {
    const uint8_t id = data[0];
    const Opcode *opcode = &g_pce_opcode[id];
    switch(opcode->tag) {
        case PCE_TAG_MMAP:
            pce_tag_mmap(logical, data, size);
            break;
        case PCE_TAG_LOCAL_JUMP:
            pce_tag_local_jump(logical, data, size);
            break;
        case PCE_TAG_UNKNOWN:
            pce_tag_unknown(logical, data, size);
            break;
        default:
            break;
    }
}

bool pce_disasm(MemoryMap *map, LabelRepository *labels, uint16_t *logical, String *out) {
    // 0. sanity check
    SANITY_CHECK((map != NULL) && (labels != NULL) && (logical != NULL) && (out != NULL), false);

    // 1. retrieve opcode
    uint16_t offset = *logical;
    uint8_t data[8] = {0};
    data[0] = memory_map_read(map, offset);
    const Opcode *opcode = &g_pce_opcode[data[0]];
    size_t size = opcode->size-1U;

    // 2. prepare data
    for(size_t i=1; i<opcode->size; i++) {
        data[i] = memory_map_read(map, offset + i);
    }

    pce_tag(offset+1, data, &size);

    // 3. output opcode
    if(!string_append(out, opcode->name, OPCODE_NAME_MAX_LEN-1U)) {
        ERROR_MSG("failed to output opcode");
        return false;
    }
    if(!string_append_n(out, ' ', 1)) {
        ERROR_MSG("failed to append spaces");
        return false;
    }

    // 4. outout args (if any)
    if(!pce_arg_list(map, labels, out, opcode->type, &data[1], size)) {
        ERROR_MSG("an error occured while outputting argument list");
        return false;
    }

    *logical += opcode->size;
    return true;
}

