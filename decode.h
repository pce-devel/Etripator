#ifndef DECODE_H
#define DECODE_H

#include "config.h"
#include "section.h"

/*
 *
 */
struct SectionProcessor_
{
	Section  *processed;    /* Section being processed */
	FILE     *in;           /* Input file (rom or cd image) */
	FILE     *out;          /* Output */
	off_t	 fileOffset;    /* Current offset in file (will save us calling ftell everytime) */
	uint16_t orgOffset;     /* Offset in section org */
	uint8_t  page;          /* Memory page (MPR) */

	/* Used for code output */
	uint8_t	instruction;    /* Current instruction */
	uint8_t data[6];        /* Associated data (its size depends on the instruction) */
		
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
void resetSectionProcessor(FILE*, FILE*, Section*, SectionProcessor*);
  
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