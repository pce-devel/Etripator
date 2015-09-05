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
#include "section.h"
#include "rom.h"

/*
 *
 */
struct SectionProcessor_
{
    Section  *processed;    /* Section being processed */
    ROM      *rom;          /* Input ROM */
    FILE     *out;          /* Output */

    size_t   physicalAddr; /* Physical address. */
    uint16_t logicalAddr;  /* Logical address. */

    size_t   offset;         /* Current section offset */

    uint16_t        labelIndex;      /* current label */
    LabelRepository labelRepository; /* label repository for this section */

    /* Used for raw binary data output */
    uint8_t *buffer;       /* Data buffer */
};
typedef struct SectionProcessor_ SectionProcessor;

/*
 * Initialize section processor
 */
int initializeSectionProcessor(SectionProcessor*);

/*
 * Reset section processor
 */
void resetSectionProcessor(ROM*, FILE*, Section*, SectionProcessor*);
  
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
int getLabels(SectionProcessor* iProcessor);

/* 
 * Initialize label index so that it points to the label close to current org offset
 */
void getLabelIndex(SectionProcessor*);

/*
 * Process opcode
 */
char processOpcode(SectionProcessor*);

#endif // DECODE_H
