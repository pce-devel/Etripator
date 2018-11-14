/*
    This file is part of Etripator,
    copyright (c) 2009--2018 Vincent Cruz.

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
#ifndef DECODE_H
#define DECODE_H

#include "config.h"
#include "labels.h"
#include "section.h"
#include "memorymap.h"

/**
 * Decoder
 */
typedef struct
{
    Section   *current; /* Section being processed */
    MemoryMap *memmap;  /* Memory map */
    FILE      *out;     /* Output */

    uint16_t logical;  /* Logical address. */
    int32_t offset; /* Current section offset */

    LabelRepository *labels; /* label repository */

    /* Used for raw binary data output */
    uint8_t *buffer; /* Data buffer */
} Decoder;

/*
 * Initializes decoder
 * \return 1 upon success, 0 otherwise.
 */
int createDecoder(Decoder* decoder);

/*
 * Reset decoder
 */
void resetDecoder(MemoryMap *memmap, FILE *out, Section *section, Decoder *decoder);
  
/*
 * Release decoder resources
 */
void deleteDecoder(Decoder *decoder);

/*
 * Process data section
 */
int processDataSection(Decoder *decoder);

/* 
 * Parse section to identify potential labels 
 */
int extractLabels(Decoder *decoder);

/*
 * Process opcode
 */
char processOpcode(Decoder *decoder);

#endif // DECODE_H
