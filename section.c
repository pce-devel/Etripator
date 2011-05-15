#include "section.h"
#include "message.h"
#include "labels.h"
#include "cfg.h"

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

/*
 * Validate section Id
 */
static int validateId(const char* value, Section *currentSection)
{
	unsigned long id;
	
	id = strtoul(value, NULL, 16);
	if(id  == ULONG_MAX)
	{
		return 0;
	}

	if(id > 0xff)
	{
		return 0;
	}
	
	currentSection->id  = (uint8_t)id;
	
	return 1;
}

#define KEY_COUNT 6

#define TYPE_MASK   1
#define BANK_MASK   (1<<1)
#define ORG_MASK    (1<<2)
#define OFFSET_MASK (1<<3)
#define SIZE_MASK   (1<<4)
#define ID_MASK     (1<<5)

/**
 * \brief CFG key parsing helper
 */
typedef struct
{
	const char* key;
	int (*validate)(const char* value, Section* section); 
} KeyValidator;

 /**
  * \brief Helper structure for CFG parsing of sections.
  */
 struct SectionValidator
 {
	Section*      section;      /**< Section array. */
	size_t        sectionCount; /**< Number of processed sections. */
	size_t        capacity;     /**< Section array element count. */
	Section*      current;      /**< Current section being processed. */
	uint32_t      flag;         /**< Bitfield where each bit corresponds to a specific key. */
	int           keyCount;     /**< Number of supported key count. */
	KeyValidator* keyChecker;   /**< . Set of key validators. */
 };

/*
 * \brief Expand section array.
 *
 * \param validator  contains the section array to expand.
 * \return
 *     SECTION_MEMORY_ISSUE if reallocation failed.
 *     SECTION_OK if the section array was succesfully expanded.
 */
static SECTION_ERR ExpandSection(struct SectionValidator* validator)
{
	int count = validator->capacity + 4;
	Section *tmp;

	tmp = (Section*)malloc(count * sizeof(Section));
	if(tmp == NULL)
	{
		return SECTION_MEMORY_ISSUE;
	}
	
	validator->current = tmp + (validator->current - validator->section);

	if(validator->section != NULL)
	{
		memcpy(tmp, validator->section, validator->capacity * sizeof(Section));	
		free(validator->section);
	}

	validator->capacity = count;
	validator->section  = tmp;
	
	return SECTION_OK;
}

/**
 * \brief Section start callback
 *
 * \param data         Section validator
 * \param sectionName  Section name
 * \return 0 if an error occured, 1 on success
 */
static int beginCFGSection(void *data, const char* sectionName)
{
	struct SectionValidator *validator = (struct SectionValidator*)data;

	if(validator->sectionCount == validator->capacity)
	{
		if(ExpandSection(validator) != SECTION_OK)
		{
			return 0;
		}
	}

	validator->current        = validator->section + validator->sectionCount;
	validator->current->bank  = 0;
	validator->current->org   = 0;
	validator->current->start = 0;
	validator->current->size  =-1;
	validator->current->id    = validator->sectionCount;
	validator->current->name  = strdup(sectionName);

	validator->flag  = 0;

	++validator->sectionCount;

	return 1;
}

/*
 * \brief Check section attributes and compute start offset if needed.
 * \param data         Section validator
 * \return 0 if an error occured, 1 on success
 */
 static int finalizeCFGSection( void *data )
 {
	struct SectionValidator *validator = (struct SectionValidator*)data;

	uint32_t flag           = validator->flag;
	Section *currentSection = validator->current;

	if( !(flag & (TYPE_MASK)) )
	{
		ERROR_MSG("[%s] : missing type.\n", currentSection->name );
		return 0;
	}
	
	if( !(flag & (BANK_MASK)) )
	{
		ERROR_MSG("[%s] : missing bank.\n", currentSection->name );
		return 0;
	}
	
	if( !(flag & (ORG_MASK)) )
	{
		ERROR_MSG("[%s] : missing org.\n", currentSection->name );
		return 0;
	}
	
	if( !(flag & OFFSET_MASK) )
	{
		currentSection->start = (currentSection->bank << 13) | (currentSection->org & 0x1fff);
	}

	if((currentSection->type != CODE) && (currentSection->size <= 0))
	{
		ERROR_MSG("[%s] Automatic section size detection doesn't work for data.\n", currentSection->name);
		return 0;
	}

	return 1;
 }

/**
 * \brief Validate current key value pair.
 * \param data   Pointer to current section validator.
 * \param key    Key string.
 * \param value  Value string.
 * \return 0 on error, 1 on sucess.
 */
static int validateCFGTuple(void *data, const char* key, const char* value)
{
	struct SectionValidator *validator = (struct SectionValidator*)data;

	int i;
	for(i=0; i<validator->keyCount; ++i)
	{
		if(strcmp(validator->keyChecker[i].key, key) == 0)
		{
			if(validator->flag & (1 << i))
			{
				ERROR_MSG("%s already set", key );
				return 0;
			}
			validator->flag |= (1 << i);
			return validator->keyChecker[i].validate(value, validator->current);
		}
	}

	return 0;
}

/*
 * \brief Extract sections from a CFG file
 *
 * \param iFileName     CFG file name
 * \param iSection      Section array (in/out)
 * \param iSectionCount Number of read sections (in/out)
 * \return SECTION_OK on success.
 */ 
SECTION_ERR readSectionsFromCFG(char* iFileName, Section** iSection, size_t* iSectionCount)
{
	struct CFGPayload payload;
	struct SectionValidator validator;

	CFG_ERR cfgErr;
	SECTION_ERR sectionErr;

	static KeyValidator keyCheckers[KEY_COUNT] =
	{
		{ "type",   validateType},
		{ "bank",   validateBank},
		{ "org",    validateOrg},
		{ "offset", validateOffset},
		{ "size",   validateSize},
		{ "id",     validateId}
	};

	validator.capacity     = *iSectionCount;
	validator.sectionCount = *iSectionCount;
	validator.current      = (Section*)(*iSectionCount); 
	validator.section      = NULL;
	validator.keyCount     = KEY_COUNT;
	validator.keyChecker   = keyCheckers;

	sectionErr = ExpandSection(&validator);
	if(sectionErr != SECTION_OK)
	{
		return sectionErr;
	}

	payload.data                 = &validator;
	payload.keyValueCallback     = validateCFGTuple;
	payload.beginSectionCallback = beginCFGSection;
	payload.endSectionCallback   = finalizeCFGSection;
	
	cfgErr = ParseCFG(iFileName, &payload);
	if(cfgErr != CFG_OK)
	{
		ERROR_MSG("%s[%d] %s", iFileName, payload.line, GetCFGErrorMsg(cfgErr));
		return SECTION_PARSING_ERROR;
	}

	*iSectionCount = validator.sectionCount;
	*iSection      = validator.section;

	return SECTION_OK;
}






/*
 * Initialize section processor
 */
int initializeSectionProcessor(SectionProcessor* iProcessor)
{
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
void resetSectionProcessor(FILE* iIn, FILE* iOut, Section* iProcessed, SectionProcessor* iProcessor)
{
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
				ERROR_MSG("Allocation failed : %s", strerror(errno));
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
		sprintf(line, "l%04x_%02x: ", iProcessor->labelRepository.labels[iProcessor->labelIndex].offset,  iProcessor->processed->id);

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
			data[3] = iProcessor->processed->id & 0xff;
		}
		else if(pce_opcode[inst].type == 19)
		{
			data[2] = iProcessor->processed->id & 0xff;
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


