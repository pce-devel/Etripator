#ifndef _SECTION_H_
#define _SECTION_H_

#include "config.h"
#include "labels.h"

/*
 * Section :
 *   Define a ROM area.
 * 	 This area can contain data or code.
 */
struct Section_ {
	char     type;  /* type (defined below) */
	char*    name;  /* name */
	uint8_t  bank;  /* rom bank */
	uint16_t org;   /* org */
	off_t    start; /* start address (file offset in bytes) */
	int32_t  size;  /* section size */
	uint8_t  id;    /* section id */
};
typedef struct Section_ Section;

enum SectionType {
	BIN_DATA = 0,
	INC_DATA,
	CODE
};

/*
 * Extract sections from a CFG file
 */ 
int readSectionsFromCFG(char* iFileName, Section** iSection, size_t* iSectionCount);

/*
 * Extract sections from a config file
 */
int readSectionsFromCFG(char* iFileName, Section** iSection, size_t* iSectionCount);

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

#endif // _SECTION_H_
