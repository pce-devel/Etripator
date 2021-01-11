/*
    This file is part of Etripator,
    copyright (c) 2009--2021 Vincent Cruz.

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
#include "opcodes.h"

#define PCE_TYPE_COUNT 23
#define PCE_ARG_COUNT 7

/* Opcode output. */
static char* pce_opstring[PCE_TYPE_COUNT][PCE_ARG_COUNT] = {
  {  NULL,         NULL,       NULL,     NULL,     NULL,    NULL,  NULL },
  {  NULL,         NULL,       NULL,     NULL,     NULL,    NULL,  NULL },
  { "#$%02x",      NULL,       NULL,     NULL,     NULL,    NULL,  NULL },
  { "#$%02x, ",   "<$%02x",    NULL,     NULL,     NULL,    NULL,  NULL },
  { "#$%02x, ",   "<$%02x, X", NULL,     NULL,     NULL,    NULL,  NULL },
  { "#$%02x, ",   "$%02x",    "%02x",    NULL,     NULL,    NULL,  NULL },
  { "#$%02x, ",   "$%02x",    "%02x, X", NULL,     NULL,    NULL,  NULL },
  { "<$%02x",      NULL,       NULL,     NULL,     NULL,    NULL,  NULL },
  { "<$%02x, X",   NULL,       NULL,     NULL,     NULL,    NULL,  NULL },
  { "<$%02x, Y",   NULL,       NULL,     NULL,     NULL,    NULL,  NULL },
  { "[$%02x]",     NULL,       NULL,     NULL,     NULL,    NULL,  NULL },
  { "[$%02x, X]",  NULL,       NULL,     NULL,     NULL,    NULL,  NULL },
  { "[$%02x], Y",  NULL,       NULL,     NULL,     NULL,    NULL,  NULL },
  { "<$%02x, ",   "$%02x",    "%02x",    NULL,     NULL,    NULL,  NULL },
  { "$%02x",      "%02x",      NULL,     NULL,     NULL,    NULL,  NULL },
  { "[$%02x",     "%02x]",     NULL,     NULL,     NULL,    NULL,  NULL },
  { "$%02x",      "%02x, X",   NULL,     NULL,     NULL,    NULL,  NULL },
  { "$%02x",      "%02x, Y",   NULL,     NULL,     NULL,    NULL,  NULL },
  { "$%02x",      "%02x, ",   "$%02x",  "%02x, ", "$%02x", "%02x", NULL },
  { "l%02x",      "%02x",     "_%02x",   NULL,     NULL,    NULL,  NULL },
  { "<$%02x, ",   "l%02x",    "%02x",    "_%02x",  NULL,    NULL,  NULL },
  { "[$%02x",     "%02x, X]",  NULL,     NULL,     NULL,    NULL,  NULL },
  { "$%02x",       NULL,       NULL,     NULL,     NULL,    NULL,  NULL }
};

/* PC engine opcodes */
static opcode_t pce_opcode[256] = {
  /* 00 */  { "brk ", 1 ,  0 },  /* BRK                 */  
  /* 01 */  { "ora ", 2 , 11 },  /* ORA  (ZZ, X)        */
  /* 02 */  { "sxy ", 1 ,  0 },  /* SXY                 */
  /* 03 */  { "st0 ", 2 ,  2 },  /* STO  #nn            */
  /* 04 */  { "tsb ", 2 ,  7 },  /* TSB  ZZ             */
  /* O5 */  { "ora ", 2 ,  7 },  /* ORA  ZZ             */
  /* 06 */  { "asl ", 2 ,  7 },  /* ASL  ZZ             */
  /* 07 */  { "rmb0", 2 ,  7 },  /* RMB0 ZZ             */
  /* 08 */  { "php ", 1 ,  0 },  /* PHP                 */
  /* O9 */  { "ora ", 2 ,  2 },  /* ORA  #nn            */
  /* 0A */  { "asl ", 1 ,  1 },  /* ASL  A              */
  /* 0B */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 0C */  { "tsb ", 3 , 14 },  /* TRB  hhll           */
  /* 0D */  { "ora ", 3 , 14 },  /* ORA  hhll           */
  /* 0E */  { "asl ", 3 , 14 },  /* ASL  hhll           */
  /* 0F */  { "bbr0", 3 , 20 },  /* BBR0 ZZ, lhhll      */
  /* 10 */  { "bpl ", 2 , 19 },  /* BPL  lhhll          */
  /* 11 */  { "ora ", 2 , 12 },  /* ORA  (ZZ), Y        */
  /* 12 */  { "ora ", 2 , 10 },  /* ORA  (ZZ)           */
  /* 13 */  { "st1 ", 2 ,  2 },  /* ST1  #nn            */
  /* 14 */  { "trb ", 2 ,  7 },  /* TRB  ZZ             */
  /* 15 */  { "ora ", 2 ,  8 },  /* ORA  ZZ, X          */
  /* 16 */  { "asl ", 2 ,  8 },  /* ASL  ZZ, X          */
  /* 17 */  { "rmb1", 2 ,  7 },  /* RMB1 ZZ             */
  /* 18 */  { "clc ", 1 ,  0 },  /* CLC                 */
  /* 19 */  { "ora ", 3 , 17 },  /* ORA  hhll, Y        */
  /* 1A */  { "inc ", 1 ,  1 },  /* INC  A              */
  /* 1B */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 1C */  { "trb ", 3 , 14 },  /* TRB  hhll           */
  /* 1D */  { "ora ", 3 , 16 },  /* ORA  hhll, X        */
  /* 1E */  { "asl ", 3 , 16 },  /* ASL  hhll, X        */
  /* 1F */  { "bbr1", 3 , 20 },  /* BBR1 ZZ, lhhll      */
  /* 20 */  { "jsr ", 3 , 19 },  /* JSR  lhhll          */
  /* 21 */  { "and ", 2 , 11 },  /* AND  (ZZ, X)        */
  /* 22 */  { "sax ", 1 ,  0 },  /* SAX                 */
  /* 23 */  { "st2 ", 2 ,  2 },  /* ST2  #nn            */
  /* 24 */  { "bit ", 2 ,  7 },  /* BIT  ZZ             */
  /* 25 */  { "and ", 2 ,  7 },  /* AND  ZZ             */
  /* 26 */  { "rol ", 2 ,  7 },  /* ROL  ZZ             */
  /* 27 */  { "rmb2", 2 ,  7 },  /* RMB2 ZZ             */
  /* 28 */  { "plp ", 1 ,  0 },  /* PLP                 */
  /* 29 */  { "and ", 2 ,  2 },  /* AND  #nn            */
  /* 2A */  { "rol ", 1 ,  1 },  /* ROL  A              */
  /* 2B */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 2C */  { "bit ", 3 , 14 },  /* BIT  hhll           */
  /* 2D */  { "and ", 3 , 14 },  /* AND  hhll           */
  /* 2E */  { "rol ", 3 , 14 },  /* ROL  hhll           */
  /* 2F */  { "bbr2", 3 , 20 },  /* BBR2 ZZ, lhhll      */
  /* 30 */  { "bmi ", 2 , 19 },  /* BMI  lhhll          */
  /* 31 */  { "and ", 2 , 12 },  /* AND  (ZZ), Y        */
  /* 32 */  { "and ", 2 , 10 },  /* AND  (ZZ)           */
  /* 33 */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 34 */  { "bit ", 2 ,  8 },  /* BIT  ZZ, X          */
  /* 35 */  { "and ", 2 ,  8 },  /* AND  ZZ, X          */
  /* 36 */  { "rol ", 2 ,  7 },  /* ROL  ZZ             */
  /* 37 */  { "rmb3", 2 ,  7 },  /* RMB3 ZZ             */
  /* 38 */  { "sec ", 1 ,  0 },  /* SEC                 */
  /* 39 */  { "and ", 3 , 17 },  /* AND  hhll, Y        */
  /* 3A */  { "dec ", 1 ,  1 },  /* DEC  A              */
  /* 3B */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 3C */  { "bit ", 3 , 16 },  /* BIT  hhll, X        */
  /* 3D */  { "and ", 3 , 16 },  /* AND  hhll, X        */
  /* 3E */  { "rol ", 3 , 16 },  /* ROL  hhll, X        */
  /* 3F */  { "bbr3", 3 , 20 },  /* BBR3 ZZ, lhhll      */
  /* 40 */  { "rti ", 1 ,  0 },  /* RTI                 */
  /* 41 */  { "eor ", 2 , 11 },  /* EOR  (ZZ, X)        */ 
  /* 42 */  { "say ", 1 ,  0 },  /* SAY                 */
  /* 43 */  { "tma ", 2 ,  2 },  /* TMA  #nn            */ 
  /* 44 */  { "bsr ", 2 , 19 },  /* BSR  lhhll          */  
  /* 45 */  { "eor ", 2 ,  7 },  /* EOR  ZZ             */
  /* 46 */  { "lsr ", 2 ,  7 },  /* LSR  ZZ             */
  /* 47 */  { "rmb4", 2 ,  7 },  /* RMB4 ZZ             */
  /* 48 */  { "pha ", 1 ,  0 },  /* PHA                 */
  /* 49 */  { "eor ", 2 ,  2 },  /* EOR  #nn            */
  /* 4A */  { "lsr ", 1 ,  1 },  /* LSR  A              */
  /* 4B */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 4C */  { "jmp ", 3 , 19 },  /* JMP  lhhll          */
  /* 4D */  { "eor ", 3 , 14 },  /* EOR  hhll           */
  /* 4E */  { "lsr ", 3 , 14 },  /* LSR  hhll           */
  /* 4F */  { "bbr4", 3 , 20 },  /* BBR4 ZZ, lhhll      */
  /* 50 */  { "bvc ", 2 , 19 },  /* BVC  lhhll          */
  /* 51 */  { "eor ", 2 , 12 },  /* EOR  (ZZ),Y         */ 
  /* 52 */  { "eor ", 2 , 10 },  /* EOR  (ZZ)           */ 
  /* 53 */  { "tam ", 2 ,  2 },  /* TAM  #nn            */ 
  /* 54 */  { "csl ", 1 ,  0 },  /* CSL                 */
  /* 55 */  { "eor ", 2 ,  8 },  /* EOR  ZZ, X          */ 
  /* 56 */  { "lsr ", 2 ,  8 },  /* LSR  ZZ, X          */
  /* 57 */  { "rmb5", 2 ,  7 },  /* RMB5 ZZ             */
  /* 58 */  { "cli ", 1 ,  0 },  /* CLI                 */
  /* 59 */  { "eor ", 3 , 17 },  /* EOR  hhll, Y        */
  /* 5A */  { "phy ", 1 ,  0 },  /* PHY                 */
  /* 5B */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 5C */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 5D */  { "eor ", 3 , 16 },  /* EOR  hhll, X        */
  /* 5E */  { "lsr ", 3 , 16 },  /* LSR  hhll, X        */
  /* 5F */  { "bbr5", 3 , 20 },  /* BBR5 ZZ, lhhll      */
  /* 60 */  { "rts ", 1 ,  0 },  /* RTS                 */
  /* 61 */  { "adc ", 2 , 11 },  /* ADC  (ZZ, X)        */
  /* 62 */  { "cla ", 1 ,  0 },  /* CLA                 */
  /* 63 */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 64 */  { "stz ", 2 ,  7 },  /* STZ  ZZ             */
  /* 65 */  { "adc ", 2 ,  7 },  /* ADC  ZZ             */
  /* 66 */  { "ror ", 2 ,  7 },  /* ROR  ZZ             */
  /* 67 */  { "rmb6", 2 ,  7 },  /* RMB6 ZZ             */
  /* 68 */  { "pla ", 1 ,  0 },  /* PLA                 */
  /* 69 */  { "adc ", 2 ,  2 },  /* ADC  #nn            */
  /* 6A */  { "ror ", 1 ,  1 },  /* ROR  A              */
  /* 6B */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 6C */  { "jmp ", 3 , 15 },  /* JMP  (hhll)         */
  /* 6D */  { "adc ", 3 , 14 },  /* ADC  hhll           */
  /* 6E */  { "ror ", 3 , 14 },  /* ROR  hhll           */
  /* 6F  */ { "bbr6", 3 , 20 },  /* BBR6 ZZ, lhhll      */
  /* 70 */  { "bvs ", 2 , 19 },  /* BVS  lhhll          */
  /* 71 */  { "adc ", 2 , 12 },  /* ADC  (ZZ), Y        */
  /* 72 */  { "adc ", 2 , 10 },  /* ADC  (ZZ)           */
  /* 73 */  { "tii ", 7 , 18 },  /* TII  shsl,dhdl,lhll */
  /* 74 */  { "stz ", 2 ,  8 },  /* STZ  ZZ, X          */
  /* 75 */  { "adc ", 2 ,  8 },  /* ADC  ZZ, X          */
  /* 76 */  { "ror ", 2 ,  8 },  /* ROR  ZZ, X          */
  /* 77 */  { "rmb7", 2 ,  7 },  /* RMB7 ZZ             */
  /* 78 */  { "sei ", 1 ,  0 },  /* SEI                 */
  /* 79 */  { "adc ", 3 , 17 },  /* ADC  hhll, Y        */
  /* 7A */  { "ply ", 1 ,  0 },  /* PLY                 */
  /* 7B */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 7C */  { "jmp ", 3 , 21 },  /* JMP  (hhll, X)      */
  /* 7D */  { "adc ", 3 , 16 },  /* ADC  hhll, X        */
  /* 7E */  { "ror ", 3 , 16 },  /* ROR  hhll, X        */
  /* 7F */  { "bbr7", 3 , 20 },  /* BBR7 ZZ, lhhll      */
  /* 80 */  { "bra ", 2 , 19 },  /* BRA  lhhll          */
  /* 81 */  { "sta ", 2 , 11 },  /* STA  (ZZ, X)        */
  /* 82 */  { "clx ", 1 ,  0 },  /* CLX                 */
  /* 83 */  { "tst ", 3 ,  3 },  /* TST  #nn, ZZ        */
  /* 84 */  { "sty ", 2 ,  7 },  /* STY  ZZ             */
  /* 85 */  { "sta ", 2 ,  7 },  /* STA  ZZ             */
  /* 86 */  { "stx ", 2 ,  7 },  /* STX  ZZ             */
  /* 87 */  { "smb0", 2 ,  7 },  /* SMB0 ZZ             */
  /* 88 */  { "dey ", 1 ,  0 },  /* DEY                 */
  /* 89 */  { "bit ", 2 ,  2 },  /* BIT  #nn            */
  /* 8A */  { "txa ", 1 ,  0 },  /* TXA                 */
  /* 8B */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 8C */  { "sty ", 3 , 14 },  /* STY  hhll           */
  /* 8D */  { "sta ", 3 , 14 },  /* STA  hhll           */
  /* 8E */  { "stx ", 3 , 14 },  /* STX  hhll           */
  /* 8F */  { "bbs0", 3 , 20 },  /* BBS0 ZZ, lhhll      */
  /* 90 */  { "bcc ", 2 , 19 },  /* BCC  lhhll          */
  /* 91 */  { "sta ", 2 , 12 },  /* STA  (ZZ), Y        */
  /* 92 */  { "sta ", 2 , 10 },  /* STA  (ZZ)           */
  /* 93 */  { "tst ", 4 ,  5 },  /* TST  #nn, hhll      */
  /* 94 */  { "sty ", 2 ,  8 },  /* STY  ZZ, X          */
  /* 95 */  { "sta ", 2 ,  8 },  /* STA  ZZ, X          */
  /* 96 */  { "stx ", 2 ,  9 },  /* STX  ZZ, Y          */
  /* 97 */  { "smb1", 2 ,  7 },  /* SMB1 ZZ             */
  /* 98 */  { "tya ", 1 ,  0 },  /* TYA                 */
  /* 99 */  { "sta ", 3 , 17 },  /* STA  hhll, Y        */
  /* 9A */  { "txs ", 1 ,  0 },  /* TXS                 */
  /* 9B */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* 9C */  { "stz ", 3 , 14 },  /* STZ  hhll           */
  /* 9D */  { "sta ", 3 , 16 },  /* STA  hhll, X        */
  /* 9E */  { "stz ", 3 , 16 },  /* STZ  hhll, X        */
  /* 9F */  { "bbs1", 3 , 20 },  /* BBS1 ZZ, lhhll      */
  /* A0 */  { "ldy ", 2 ,  2 },  /* LDY  #nn            */
  /* A1 */  { "lda ", 2 , 11 },  /* LDA  (ZZ, X)        */
  /* A2 */  { "ldx ", 2 ,  2 },  /* LDA  #nn            */
  /* A3 */  { "tst ", 3 ,  4 },  /* TST  #nn, ZZ, X     */
  /* A4 */  { "ldy ", 2 ,  7 },  /* LDY  ZZ             */
  /* A5 */  { "lda ", 2 ,  7 },  /* LDA  ZZ             */
  /* A6 */  { "ldx ", 2 ,  7 },  /* LDX  ZZ             */
  /* A7 */  { "smb2", 2 ,  7 },  /* SMB2 ZZ             */
  /* A8 */  { "tay ", 1 ,  0 },  /* TAY                 */
  /* A9 */  { "lda ", 2 ,  2 },  /* LDA  #nn            */
  /* AA */  { "tax ", 1 ,  0 },  /* TAX                 */
  /* AB */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* AC */  { "ldy ", 3 , 14 },  /* LDY  hhll           */
  /* AD */  { "lda ", 3 , 14 },  /* LDA  hhll           */
  /* AE */  { "ldx ", 3 , 14 },  /* LDX  hhll           */
  /* AF */  { "bbs2", 3 , 20 },  /* BBS2 ZZ, lhhll      */
  /* B0 */  { "bcs ", 2 , 19 },  /* BCS  lhhll         */
  /* B1 */  { "lda ", 2 , 12 },  /* LDA  (ZZ), Y        */
  /* B2 */  { "lda ", 2 , 10 },  /* LDA  (ZZ)           */
  /* B3 */  { "tst ", 4 ,  6 },  /* TST  #nn, hhll, X   */
  /* B4 */  { "ldy ", 2 ,  8 },  /* LDY  ZZ, X          */
  /* B5 */  { "lda ", 2 ,  8 },  /* LDA  ZZ, X          */
  /* B6 */  { "ldx ", 2 ,  9 },  /* LDX  ZZ, Y          */
  /* B7 */  { "smb3", 2 ,  7 },  /* SMB3 ZZ             */
  /* B8 */  { "clv ", 1 ,  0 },  /* CLV                 */
  /* B9 */  { "lda ", 3 , 17 },  /* LDA  hhll, Y        */
  /* BA */  { "tsx ", 1 ,  0 },  /* TSX                 */
  /* BB */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* BC */  { "ldy ", 3 , 16 },  /* LDY  hhll, X        */
  /* BD */  { "lda ", 3 , 16 },  /* LDA  hhll, X        */
  /* BE */  { "ldx ", 3 , 17 },  /* LDX  hhll, Y        */
  /* BF */  { "bbs3", 3 , 20 },  /* BBS3 ZZ, lhhll      */
  /* C0 */  { "cpy ", 2 ,  2 },  /* CPY  #nn            */
  /* C1 */  { "cmp ", 2 , 11 },  /* CMP  (ZZ, X)        */
  /* C2 */  { "cly ", 1 ,  0 },  /* CLY                 */
  /* C3 */  { "tdd ", 7 , 18 },  /* TDD  shsl,dhdl,lhlh */
  /* C4 */  { "cpy ", 2 ,  7 },  /* CPY  ZZ             */
  /* C5 */  { "cmp ", 2 ,  7 },  /* CMP  ZZ             */
  /* C6 */  { "dec ", 2 ,  7 },  /* DEC  ZZ             */
  /* C7 */  { "smb4", 2 ,  7 },  /* SMB4 ZZ             */
  /* C8 */  { "iny ", 1 ,  0 },  /* INY                 */
  /* C9 */  { "cmp ", 2 ,  2 },  /* CMP  #nn            */
  /* CA */  { "dex ", 1 ,  0 },  /* DEX                 */
  /* CB */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* CC */  { "cpy ", 3 , 14 },  /* CPY  hhll           */      
  /* CD */  { "cmp ", 3 , 14 },  /* CMP  hhll           */
  /* CE */  { "dec ", 3 , 14 },  /* DEC  hhll           */
  /* CF */  { "bbs4", 3 , 20 },  /* BBS4 ZZ, lhhll      */
  /* D0 */  { "bne ", 2 , 19 },  /* BNE  lhhll          */
  /* D1 */  { "cmp ", 2 , 12 },  /* CMP  (ZZ), Y        */
  /* D2 */  { "cmp ", 2 , 10 },  /* CMP  (ZZ)           */
  /* D3 */  { "tin ", 7 , 18 },  /* TIN  shsl,dhdl,lhlh */
  /* D4 */  { "csh ", 1 ,  0 },  /* CSH                 */
  /* D5 */  { "cmp ", 2 ,  8 },  /* CMP  ZZ, X          */
  /* D6 */  { "dec ", 2 ,  8 },  /* DEC  ZZ, X          */
  /* D7 */  { "smb5", 2 ,  7 },  /* SMB5 ZZ             */
  /* D8 */  { "cld ", 1 ,  0 },  /* CLD                 */
  /* D9 */  { "cmp ", 3 , 17 },  /* CMP  hhll, Y        */
  /* DA */  { "phx ", 1 ,  0 },  /* PHX                 */
  /* DB */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* DC */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* DD */  { "cmp ", 3 , 16 },  /* CMP  hhll, X        */
  /* DE */  { "dec ", 3 , 16 },  /* DEC  hhll, X        */
  /* DF */  { "bbs5", 3 , 20 },  /* BBS5 ZZ, lhhll      */
  /* E0 */  { "cpx ", 2 ,  2 },  /* CPX  #nn            */
  /* E1 */  { "sbc ", 2 , 11 },  /* SBC  (ZZ,X)         */
  /* E2 */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* E3 */  { "tia ", 7 , 18 },  /* TIA  shsl,dhdl,lhlh */
  /* E4 */  { "cpx ", 2 ,  7 },  /* CPX  ZZ             */
  /* E5 */  { "sbc ", 2 ,  7 },  /* SBC  ZZ             */
  /* E6 */  { "inc ", 2 ,  7 },  /* INC  ZZ             */
  /* E7 */  { "smb6", 2 ,  7 },  /* SMB6 ZZ             */
  /* E8 */  { "inx ", 1 ,  0 },  /* INX                 */
  /* E9 */  { "sbc ", 2 ,  2 },  /* SBC  #nn            */
  /* EA */  { "nop ", 1 ,  0 },  /* NOP                 */
  /* EB */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* EC */  { "cpx ", 3 , 14 },  /* CPX  hhll           */
  /* ED */  { "sbc ", 3 , 14 },  /* SBC  hhll           */
  /* EE */  { "inc ", 3 , 14 },  /* INC  hhll           */
  /* EF */  { "bbs6", 3 , 20 },  /* BBS6 ZZ, lhhll      */  
  /* F0 */  { "beq ", 2 , 19 },  /* BEQ  lhhll          */
  /* F1 */  { "sbc ", 2 , 12 },  /* SBC  (ZZ), Y        */
  /* F2 */  { "sbc ", 2 , 10 },  /* SBC  (ZZ)           */
  /* F3 */  { "tai ", 7 , 18 },  /* TAI  shsl,dhdl,lhlh */
  /* F4 */  { "set ", 1 ,  0 },  /* SET                 */ 
  /* F5 */  { "sbc ", 2 ,  8 },  /* SBC  ZZ, X          */
  /* F6 */  { "inc ", 2 ,  8 },  /* INC  ZZ, X          */
  /* F7 */  { "smb7", 2 ,  7 },  /* SMB7 ZZ             */
  /* F8 */  { "sed ", 1 ,  0 },  /* SED                 */
  /* F9 */  { "sbc ", 3 , 17 },  /* SBC  hhll, Y        */
  /* FA */  { "plx ", 1 ,  0 },  /* PLX                 */
  /* FB */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* FC */  { ".db ", 1 , 22 },  /* UNUSED              */
  /* FD */  { "sbc ", 3 , 16 },  /* SBC  hhll, X        */
  /* FE */  { "inc ", 3 , 16 },  /* INC  hhll, X        */
  /* FF */  { "bbs7", 3 , 20 }   /* BBS7 ZZ, lhhll      */
};

/**
 * Get opcode description.
 */
const opcode_t* opcode_get(uint8_t op) {
    return &pce_opcode[op];
}

/**
 *
 */
const char* opcode_format(const opcode_t *op, int i) {
    if((i < 0) || (i >= PCE_ARG_COUNT)) {
        return NULL;
    }
    return pce_opstring[op->type][i];
}

/**
 * Is the instruction a local jump ?
 */
int opcode_is_local_jump(uint8_t op) {
    return 
	    ((op & 0x0F) == 0x0F) || /* BBR* and BBS* */
	    (op          == 0x90) || /* BCC           */
	    (op          == 0xB0) || /* BCS           */
	    (op          == 0x80) || /* BRA           */
	    (op          == 0xF0) || /* BEQ           */
	    (op          == 0x30) || /* BMI           */
	    (op          == 0xD0) || /* BNE           */
	    (op          == 0x10) || /* BPL           */
	    (op          == 0x44) || /* BSR           */
	    (op          == 0x50) || /* BVC           */
	    (op          == 0x70);   /* BVS           */
}

/**
 * Is the instruction a "far" jump ?
 */
int opcode_is_far_jump(uint8_t op) {
    return
        (op == 0x4C) || /* JMP */
        (op == 0x20);   /* JSR */
}
