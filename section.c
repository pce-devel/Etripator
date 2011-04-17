#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include <stdlib.h>
#include <stdio.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <winmmap.h>
#else
#include <sys/mman.h>
#endif
#include <fcntl.h>
#include <string.h>
#include <limits.h>

#include "csv.h"
#include "section.h"
#include "labels.h"

#ifdef _MSC_VER
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

#define TOKEN_COUNT 5

static const char* supportedSectionTypeName[] =
{
	"bin_data", "inc_data", "code", NULL
};

/*
 * Validate section type
 */
static int validateType(const char* value, Section *currentSection)
{
	int i;
		
	for( i=0; supportedSectionTypeName[i] != NULL; ++i )
	{
		if(strcasecmp(value, supportedSectionTypeName[i]) == 0)
		{
			currentSection->type = i;
			return 1;
		}
	}
	return 0;
}

/*
 * Validate bank 
 */
static int validateBank(const char* value, Section *currentSection)
{
	unsigned long bank;
	
	bank = strtoul(value, NULL, 16);
	if(bank == ULONG_MAX)
	{
		return 0;
	}
	
	currentSection->bank = (uint8_t)bank;
	
	return 1;
}

/*
 * Validate org 
 */
static int validateOrg(const char* value, Section *currentSection)
{
	unsigned long org;
	
	org = strtoul(value, NULL, 16);
	if(org == ULONG_MAX)
	{
		return 0;
	}
	
	currentSection->org = (uint16_t)org;
	
	return 1;
}


/*
 * Validate offset 
 */
static int validateOffset(const char* value, Section *currentSection)
{
	unsigned long offset;
	
	offset = strtoul(value, NULL, 16);
	if(offset  == ULONG_MAX)
	{
		return 0;
	}
	
	currentSection->start  = offset ;
	
	return 1;
}

/*
 * Validate size
 */
static int validateSize(const char* value, Section *currentSection)
{
	long size;
	
	size = strtol(value, NULL, 16);
	if(size  == LONG_MAX)
	{
		return 0;
	}
	
	currentSection->size = (int32_t)size;
	
	return 1;
}

static struct
{
	const char* key;
	int (*validate)(const char* value, Section* section); 
} sectionValidator [TOKEN_COUNT] =
{
	{ "type",   validateType},
	{ "bank",   validateBank},
	{ "org",    validateOrg},
	{ "offset", validateOffset},
	{ "size",   validateSize}
};

#define TYPE_MASK   1
#define BANK_MASK   (1<<1)
#define ORG_MASK    (1<<2)
#define OFFSET_MASK (1<<3)
#define SIZE_MASK   (1<<4)

static int validateTuple(const char *key, const char *value, uint32_t *flag, Section* currentSection)
{
	int i;
	for(i=0; i<TOKEN_COUNT; ++i)
	{
		if(strcmp(sectionValidator[i].key, key) == 0)
		{
			if(*flag & (1 << i))
			{
				fprintf( stderr, "[error] %s already set\n", key );
				return 0;
			}
			*flag |= (1 << i);
			return sectionValidator[i].validate(value, currentSection);
		}
	}
	return 0;
}

/*
 * Check section attributes and compute start offset if needed.
 */
 static int finalizeSection( uint32_t flag, Section* currentSection )
 {
	if( !(flag & (TYPE_MASK)) )
	{
		fprintf( stderr, "[%s] : missing type.\n", currentSection->name );
		return 0;
	}
	
	if( !(flag & (BANK_MASK)) )
	{
		fprintf( stderr, "[%s] : missing bank.\n", currentSection->name );
		return 0;
	}
	
	if( !(flag & (ORG_MASK)) )
	{
		fprintf( stderr, "[%s] : missing org.\n", currentSection->name );
		return 0;
	}
	
	if( !(flag & OFFSET_MASK) )
	{
		currentSection->start = (currentSection->bank << 13) | (currentSection->org & 0x1fff);
	}

	if((currentSection->type != CODE) && (currentSection->size <= 0))
	{
		fprintf(stderr, "[%s] : Automatic section size detection doesn't work for data.\n", currentSection->name);
		return 0;
	}

	return 1;
 }

/*
 * Extract sections from a CFG file
 */ 
int readSectionsFromCFG(char* iFileName, Section** iSection, size_t* iSectionCount)
{
	FILE *stream;
	int ret, nMatched, line;
	long last;
	uint32_t flag;
	Section *current;
	char filename[33], key[17], value[65];
	size_t capacity;
	
	/* Some sections at the beginning of the array may be reserved */
	capacity = *iSectionCount + 4;
	
	/* Open file */
	stream = fopen( iFileName, "rb" );
	if( stream == NULL )
	{
		fprintf( stderr, "Unable to open %s\n", iFileName );
		return 0;
	}

	/* Allocate sections */
	*iSection = (Section*)malloc(capacity * sizeof(Section));
	current = *iSection + *iSectionCount;

	line = 0;
	nMatched = 1;
	ret = 1;
	while( nMatched != EOF )
	{
		flag = 0;
		
		current->bank  = 0;
		current->org   = 0;
		current->start = 0;
		current->size  =-1;
		
		nMatched = fscanf( stream, " [ %32[a-zA-Z0-9._-] ] \n", filename );
		if( nMatched != 1 )
		{
			fprintf( stderr, "[%s:%d] Unable to parse.\n", iFileName, line);
			ret = 0;
			break;
		}
		++line;
		current->name = strdup( filename );
		
		while(1)
		{
			last = ftell( stream );
			nMatched = fscanf( stream, " %16[a-zA-Z] = %32[0-9a-zA-Z_] \n", key, value );
			if( nMatched != 2 )
				break;
				
			ret = validateTuple(key, value, &flag, current);
			if(!ret)
			{
				fprintf(stderr, "[%s:%d] Wrong key/pair (%s,%s)\n", iFileName, line, key, value);
				break;
			}

			++line;
		}
		ret = finalizeSection( flag, current );
		if(!ret)
		{
			break;
		}
		
		*iSectionCount += 1;
		if(*iSectionCount < capacity)
		{
			Section *tmp;
			
			capacity += 4;
			tmp = (Section*)realloc( *iSection, capacity * sizeof(Section));
			if(tmp == NULL)
			{
				fprintf( stderr, "Unable to realloc :(\n" );
				free(*iSection);
				*iSection = NULL;
				*iSectionCount = 0;
				return 0;
			}
			*iSection = tmp;
		}

		current = *iSection + *iSectionCount;
		
		fseek( stream, last, SEEK_SET );
	}
	
	fclose(stream);
	return ret;
}

/*
 * Extract sections from a CSV file
 */ 
int readSectionsFromCSV(char* iFileName, char iSeparator, Section** iSection, size_t* iSectionCount) {
  int fd, i, found, line, ok;
  struct stat sb;
  char *data;
  Section* ptr;
  CSVState state;
  CSVToken token[6];
  size_t reserved;

  /* Set token types */
  token[0].type = STRING;
  token[1].type = STRING;
  token[2].type = STRING;
  token[3].type = STRING;	
  token[4].type = STRING;

  for(i=0; i<TOKEN_COUNT; ++i) {
  	token[i].value.string = NULL;
  }

  ok = 1;

  /* Some sections at the beginning of the array may be reserved */
  reserved = *iSectionCount;

  /* Open file */
  if((fd = open(iFileName, O_RDONLY)) < 0) {
  	fprintf(stderr, "Error: Unable to open %s.\n", iFileName);
  	return 0;
  }

  /* Get infos */
  if(fstat(fd, &sb) < 0) {
  	fprintf(stderr, "Error: Could not stat %s.\n",iFileName);
  	close(fd);
  	return 0;
  }

  if(sb.st_size < TOKEN_COUNT) {
  	fprintf(stderr, "Error: %s is too small.\n",iFileName);
  	close(fd);
  	return 0;
  }

  /* Mmap it */
  data = (char*)mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
  if(data == MAP_FAILED) {
  	fprintf(stderr, "Error: Could not mmap %s.\n",iFileName);
  	close(fd);
  	return 0;
  }
	
  /* Initalize csv state */
  CSVSetState(data, data + sb.st_size, &state);
  
  /* Count lines */
  for(; CSVJumpToNextLine(&state); ++*iSectionCount);
  ++*iSectionCount;

  /* Allocate sctions */
  *iSection = (Section*)malloc(*iSectionCount * sizeof(Section));
  
  /* Reset state */
  CSVSetState(data, data + sb.st_size, &state);
  
  line = 0;
  ptr = *iSection + reserved;
  do {
  	/* Extract tokens */
	found = CSVExtractTokensFromLine(&state, iSeparator, token, TOKEN_COUNT);

	/* Not enough tokens found */
	if(found < TOKEN_COUNT) {
		fprintf(stderr, "Error : too few tokens on line %d\n", line);
		ok = 0;
	} 
	else {
		/* Process tokens :			*/
		/*		1. type				*/
		if(strcmp(token[0].value.string, "code") == 0) {
			ptr[line].type = CODE;
		}
		else if(strcmp(token[0].value.string, "inc_data") == 0) {
			ptr[line].type = INC_DATA;
		}
		else if(strcmp(token[0].value.string, "bin_data") == 0) {
			ptr[line].type = BIN_DATA;
		}
		else {
			fprintf(stderr, "Error : Unknow section type : %s. Switching to default (bin data)\n",
					token[0].value.string);
			ptr[line].type = BIN_DATA;
		}
		
		/*		2. name				*/
		ptr[line].name = strdup(token[1].value.string);
		/*		3. rom bank			*/
		ptr[line].bank = (uint8_t)strtol(token[2].value.string, (char**)NULL, 16);
		/*		4. org				*/
		ptr[line].org = (uint16_t)strtol(token[3].value.string, (char**)NULL, 16);
		/*		5. section size		*/
		ptr[line].size   = strtol(token[4].value.string, (char**)NULL, 16);
		
		ptr[line].start  = (ptr[line].bank << 13) | (ptr[line].org & 8191);
		
		/* Data validation */
		if((ptr[line].type != CODE) && (ptr[line].size <= 0))
		{
			fprintf(stderr, "Error in section %s : Automatic section size detection doesn't work for data.\n",
					ptr[line].name);
			ok = 0;
		}
	}

	/* Cleanup strings */
	for(i=0; i<found; ++i) {
		if((token[i].type == STRING) &&
		   (token[i].value.string != NULL) ) {
			free(token[i].value.string);
			token[i].value.string = NULL;
		}
	}
	
	++line;
  }while(CSVJumpToNextLine(&state) && ok);
	
  /* Release map */
  if(munmap(data, sb.st_size) < 0) {
    fprintf(stderr, "Error: Could not mmap %s.\n",iFileName);
  	close(fd);
  	return 0;
  }

  /* Close file */
  close(fd);

  return ok;
}

/*
 * Initialize section processor
 */
int initializeSectionProcessor(SectionProcessor* iProcessor)
{
	iProcessor->sectionId = 0;
	iProcessor->processed = NULL;
	iProcessor->in        = NULL;
	iProcessor->out       = NULL;
	
	iProcessor->fileOffset = 0;
	iProcessor->orgOffset  = 0;
	iProcessor->page       = 0;

	iProcessor->instruction = 0,
	memset(iProcessor->data, 0, 6);
	
	iProcessor->buffer      = NULL;
	iProcessor->labelIndex  = 0;
	return initializeLabelRepository(&iProcessor->labelRepository);
}

/*
 * Reset section processor
 */
void resetSectionProcessor(FILE* iIn, FILE* iOut, int iId, Section* iProcessed, SectionProcessor* iProcessor)
{
	iProcessor->sectionId = iId;
	iProcessor->processed = iProcessed;
	iProcessor->in        = iIn;
	iProcessor->out       = iOut;

	iProcessor->fileOffset = iProcessed->start;
	iProcessor->orgOffset  = iProcessed->org;
	iProcessor->page       = (iProcessed->org >> 13) & 0x07;
	fseek(iIn, iProcessed->start, SEEK_SET);

	iProcessor->instruction = 0,
	memset(iProcessor->data, 0, 6);

	iProcessor->labelIndex = 0;
	resetLabelRepository(&iProcessor->labelRepository);
}
  
/*
 * Delete section processor
 */
void deleteSectionProcessor(SectionProcessor* iProcessor)
{
	if(iProcessor->buffer != NULL)
	{
		free(iProcessor->buffer);
		iProcessor->buffer = NULL;
	}

	deleteLabelRepository(&iProcessor->labelRepository);
}

/*
 * Process data section
 */
int processDataSection(SectionProcessor* iProcessor)
{
	int j;
	char eor = 0;
	off_t fileEnd;
	
	fileEnd = iProcessor->processed->start + iProcessor->processed->size;
	
	if(iProcessor->processed->type == INC_DATA)
	{
		uint8_t data[8];
					
		while(!eor) {
			fprintf(iProcessor->out, "  .db ");
			/* Read data */
			fread(data, 1, 8, iProcessor->in);

			j=0;
			while(1)
			{
				fprintf(iProcessor->out, "$%02x", data[j]);

				++j;
				++iProcessor->fileOffset;
				if((j<8) && (iProcessor->fileOffset < fileEnd))
					fprintf(iProcessor->out, ", ");
				else
				{
					fprintf(iProcessor->out, "\n");
					break;
				}
			}

			if(iProcessor->fileOffset >= fileEnd)
				eor  = 1;
		}
	}
	else
	{
		size_t  nRead, size;
		int32_t total;
	
		/* Allocate buffer if needed */
		if(iProcessor->buffer == NULL)
		{
			iProcessor->buffer = (uint8_t*)malloc(256);
			if(iProcessor->buffer == NULL)
			{
				fprintf(stderr, "Not enough memory\n");
				return 0;
			}
		}

		/* Copy data to binary file */
		for(total=0; total < iProcessor->processed->size; total += nRead)
		{
			size = iProcessor->processed->size - total;
			if(size > 256)
				size = 256;
			if(size == 0)
				break;
		
			nRead = fread(iProcessor->buffer, 1, size, iProcessor->in);
			if(nRead == 0)
				break;
			fwrite(iProcessor->buffer, 1, nRead, iProcessor->out);
		}
	}
	
	return 1;
}

/* Parse section to identify potential labels */
int getLabels(SectionProcessor* iProcessor)
{
	int32_t nBytesProcessed;
	unsigned char inst, data[6], eor;
	uint16_t labelOffset, offset;
	int delta,i=0;

	LabelRepository *repository = &(iProcessor->labelRepository);
	Section *section = iProcessor->processed;
	FILE    *in      = iProcessor->in;

	if(section->type != CODE)
		return 1;
		
	printf("\n%s:\n", section->name);

	nBytesProcessed = 0;
	offset          = section->org;
	
	/* Jump to section start */
	fseek(in, section->start, SEEK_SET);
		
	/* Push the section org */
	if(pushLabel(repository, offset) == 0)
		return 0;

	/* Walk along section */
	eor    = 0;
	while(!eor)
	{
		/* Read instruction */
		fread(&inst, 1, 1, iProcessor->in);

		/* Read data */
		if(pce_opcode[inst].size > 1)
		{
			fread(data, 1, pce_opcode[inst].size-1, in);
		}
		
		nBytesProcessed += pce_opcode[inst].size;
		offset          += pce_opcode[inst].size;

		if(isLocalJump(inst))
		{

			/* For BBR* and BBS* displacement is stored in the 2nd byte */
			i = (((inst) & 0x0F) == 0x0F) ? 1 : 0;

			/* Detect negative number */
			if(data[i] & 128)
				delta = - ((data[i] - 1) ^ 0xff);
			else
				delta = data[i];

			labelOffset = offset + delta;
			/* Insert offset to repository */
			if(pushLabel(repository, labelOffset) == 0)
				return 0;

			printf("%04x short jump to %04x\n", offset, labelOffset);
		}
		else 
		{
			if(isFarJump(inst))
			{
				labelOffset = data[0] | (data[1] << 8);
				/* Insert offset to repository */
				if(pushLabel(repository, labelOffset) == 0)
					return 0;

				printf("%04x long jump to %04x\n", offset, labelOffset);
			}
		}
		
		/* Search end of section */
		if( (section->size < 0) && ((inst == 0x40) || (inst == 0x60)) )
		{
			section->size = nBytesProcessed;
			eor            = 1;
		}
		else if( nBytesProcessed >= section->size )
		{
			eor = 1;
		}		
	}

	finalizeLabelRepositoty(repository);

	return 1;
}

/* Initialize label index so that it points to the label close to current org offset */
void getLabelIndex(SectionProcessor* iProcessor)
{
	int i;
	
	/* Room for huge improvments */
	for(i=iProcessor->labelIndex; i<iProcessor->labelRepository.last; ++i)
	{
		if(iProcessor->labelRepository.labels[i].offset >= iProcessor->orgOffset)
		{
			break;
		}
	}
	
	iProcessor->labelIndex = i;
}

/* Maximum number of characters per line */
#define MAX_CHAR_PER_LINE 80

/*
 * Process opcode
 */
char processOpcode(SectionProcessor* iProcessor) {
	int i, delta;
	uint8_t inst, data[6], isJump, page;
	char line[MAX_CHAR_PER_LINE], eor, *ptr;
	uint16_t offset, nextOrgOffset;

	eor = 0;
	ptr = line;

	/* Opcode */
	fread(&inst, 1, 1, iProcessor->in);

	/* Get label index */
	getLabelIndex(iProcessor);

	nextOrgOffset = iProcessor->orgOffset + pce_opcode[inst].size;
	
	/* Is there a label ? */
	if( (iProcessor->labelIndex < iProcessor->labelRepository.last) &&
	    (iProcessor->labelRepository.labels[iProcessor->labelIndex].offset >= iProcessor->orgOffset) &&
	    (iProcessor->labelRepository.labels[iProcessor->labelIndex].offset < nextOrgOffset) )
	{
		/* Print label*/
		sprintf(line, "l%04x_%02x: ", iProcessor->labelRepository.labels[iProcessor->labelIndex].offset,  iProcessor->sectionId);

		/* Add displacement */
		if(iProcessor->labelRepository.labels[iProcessor->labelIndex].offset != iProcessor->orgOffset)
		{
			iProcessor->labelRepository.labels[iProcessor->labelIndex].displacement = iProcessor->labelRepository.labels[iProcessor->labelIndex].offset - iProcessor->orgOffset;
		}
	}
	else
	{
		memset(line, ' ', 10 * sizeof(char));
	}

	ptr += 10;

	/* Print opcode sting */
	memcpy(ptr, pce_opcode[inst].name, 4 * sizeof(char));
	ptr += 4;

	/* Add spacing */  
	memset(ptr, ' ', 4 * sizeof(char));
	ptr += 4;

	/* End Of Routine (eor) is set to 1 if the instruction is RTI or RTS */
	if((inst == 64) || (inst == 96)) eor = 1;

	/* Data */
	if(pce_opcode[inst].size > 1)
	{
		fread(data, 1, pce_opcode[inst].size-1, iProcessor->in);
	}

	iProcessor->orgOffset   = nextOrgOffset;
	iProcessor->fileOffset += pce_opcode[inst].size;
    
	/* Swap LSB and MSB for words */
	if(pce_opcode[inst].size > 2)
	{
		uint8_t swap;
		/* Special case : TST, BBR*, BBS*. The first byte is zp var */
		i = ((pce_opcode[inst].type ==  3) ||
		     (pce_opcode[inst].type ==  4) ||
		     (pce_opcode[inst].type ==  5) ||
		     (pce_opcode[inst].type ==  6) ||
		     (pce_opcode[inst].type == 13) ||
		     (pce_opcode[inst].type == 20)) ? 1 : 0;

		for(; i<(pce_opcode[inst].size-2); i+=2)
		{
			swap      = data[i];
			data[i]   = data[i+1];
			data[i+1] = swap;
		}
	}

	/* Handle special cases (jumps, tam/tma and unsupported opcodes ) */
	isJump = 0;
	if(isLocalJump(inst)) {
		isJump = 1;

		i = (((inst) & 0x0F) == 0x0F) ? 1 : 0;

		/* Detect negative number */
		if(data[i] & 128)
			delta = - ((data[i] - 1) ^ 0xff);
		else
			delta = data[i];

		offset = iProcessor->orgOffset + delta;
		data[i  ] = offset >> 8;
		data[i+1] = offset & 0xff;
		
		page = iProcessor->page;
	}
	else 
	{
		if(isFarJump(inst))
		{
			isJump = 1;
			/* Extract page */
			page  = (data[1] >> 5) & 0xff;
			/* Build offset */
			offset = (data[1] << 8) | data[0];
			
		}
		else 
		{
			page   = 0;
			offset = 0;
			
			/* Unknown instructions are output as raw data 
			 * (ie) .db XX
			 */
			if(pce_opcode[inst].type == 22)
			{
				data[0] = inst;
			}
		}
	}

	if(pce_opcode[inst].type == 1) {
		*(ptr++) = 'A';
	}
	else {
		if((inst == 0x43) || (inst == 0x53))
		{
			/* tam and tma */
			/* Compute log base 2 of data */
			for(i=0; (i<8) && ((data[0] & 1) == 0); ++i, data[0] >>= 1);
			data[0] = i;		
		}

		/* Add section to jump label */
		if(pce_opcode[inst].type == 20)
		{
			data[3] = iProcessor->sectionId & 0xff;
		}
		else if(pce_opcode[inst].type == 19)
		{
			data[2] = iProcessor->sectionId & 0xff;
		}

		/* Print data */
		if(pce_opcode[inst].type)
		{
			for(i=0; pce_opstring[pce_opcode[inst].type][i] != NULL; ++i)
			{
				sprintf(ptr, pce_opstring[pce_opcode[inst].type][i], data[i]);
				ptr += strlen(ptr);
			}
		}
	}

	/* Add displacement to jump offset ? */
	if(isJump)
	{
		/* Search in label database ( todo : dicotomic search ) */
		for(i=0; i < iProcessor->labelRepository.last; ++i)
		{
			if(iProcessor->labelRepository.labels[i].offset == offset)
			{
				/* Displacement may not exceed 7 (maximum opcode data size) */
				if(iProcessor->labelRepository.labels[i].displacement != 0)
				{
					ptr[0] = '+';
					ptr[1] = '0' + iProcessor->labelRepository.labels[i].displacement;
					ptr += 2;
				}
				break;
			}
		}
	}

	*(ptr++) = '\n';
	*ptr = '\0';

	/* Output line */
	fwrite(line, sizeof(char), ptr-line, iProcessor->out);

	return eor;
}


