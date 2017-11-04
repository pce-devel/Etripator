/*
    This file is part of Etripator,
    copyright (c) 2009--2015 Vincent Cruz.

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
 * \todo Rename
 */
struct SectionProcessor_
{
    Section   *processed; /* Section being processed */
    MemoryMap *memmap;    /* Memory map */
    FILE      *out;       /* Output */

    uint16_t logical;  /* Logical address. */

    int32_t offset; /* Current section offset */

    LabelRepository* labelRepository; /* label repository for this section */

    /* Used for raw binary data output */
    uint8_t *buffer; /* Data buffer */
};
typedef struct SectionProcessor_ SectionProcessor;

/*
 * Initialize section processor
 * \return 1 upon success, 0 otherwise.
 */
int initializeSectionProcessor(SectionProcessor* processor);

/*
 * Reset section processor
 */
void resetSectionProcessor(MemoryMap*, FILE*, Section*, SectionProcessor*);
  
/*
 * Delete section processor
 */
void deleteSectionProcessor(SectionProcessor*);

/*
 * Process data section
 */
int processDataSection(SectionProcessor* iProcessor);

/* 
 * Parse section to identify potential labels 
 */
int extractLabels(SectionProcessor* processor);

/*
 * Process opcode
 */
char processOpcode(SectionProcessor* processor);

#endif // DECODE_H
