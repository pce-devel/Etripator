#ifndef _SECTION_H_
#define _SECTION_H_

#include "config.h"
#include "labels.h"

/**
 * \brief Section related errors
 */
typedef enum
{
	SECTION_OK = 0,                  /*< Success. */
	SECTION_INVALID_PARAMETERS,      /*< At least one of the parameter has an invalid value. */
	SECTION_PARSING_ERROR,           /*< Todo . */
	SECTION_MEMORY_ISSUE             /*< Something went wrong with the memory. */
} SECTION_ERR;

/**

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
SECTION_ERR readSectionsFromCFG(char* iFileName, Section** iSection, size_t* iSectionCount);

#endif // _SECTION_H_
