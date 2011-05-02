#ifndef OPCODES_H
#define OPCODES_H

/*
  Opcodes:
     name
     size (in bytes)
     type (for output, see below.)
*/
struct opcode_ {
  char          name[5];
  unsigned char size;
  unsigned char type;
};
typedef struct opcode_ Opcode;

#define PCE_TYPE_COUNT 23
#define PCE_ARG_COUNT 7

/*
  Opcode type : 

  00: OPC 
  01: OPC A
  02: OPC #nn
  03: OPC #nn, ZZ
  04: OPC #nn, ZZ, X
  05: OPC #nn, hhll
  06: OPC #nn, hhll, X
  07: OPC ZZ
  08: OPC ZZ, X
  09: OPC ZZ, Y
  10: OPC (ZZ)
  11: OPC (ZZ, X)
  12: OPC (ZZ), Y
  13: OPC ZZ, hhll   
  14: OPC hhll
  15: OPC (hhll)
  16: OPC hhll, X
  17: OPC hhll, Y
  18: OPC shsl,dhdl,hhll 
  19: OPC l_hhll (label)
  20: OPC ZZ, l_hhll (label)
  21: OPC [hhll, X] 
  22: .db OPC (unsupported opcode output as raw binary data)
*/
/* Opcode output for wla-dx */
extern char* pce_opstring[PCE_TYPE_COUNT][PCE_ARG_COUNT];

/* PC engine opcodes */
extern Opcode pce_opcode[256];

#endif // OPCODES_H
