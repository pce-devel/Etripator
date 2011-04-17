#include <stdlib.h>
#include "labels.h"

#define LABEL_ARRAY_INC 16

/* Initialize label repository */
int initializeLabelRepository(LabelRepository* iRepository)
{
	iRepository->size = LABEL_ARRAY_INC;
	iRepository->labels = (Label*)malloc(iRepository->size * sizeof(Label));
	if(iRepository->labels == NULL)
	{
		return 0;
	}
	
	iRepository->last = 0;
	return 1;
}

/* Reset label repsitory */
void resetLabelRepository(LabelRepository* iRepository)
{
	iRepository->last = 0;
}

/* Delete label repository */
void deleteLabelRepository(LabelRepository* iRepository)
{
	if(iRepository->labels != NULL)
	{
		free(iRepository->labels);
		iRepository->labels = NULL;
	}
	iRepository->size = iRepository->last = 0;
}


/* Push label to repository */
int pushLabel(LabelRepository* iRepository, uint16_t iOffset)
{
	if(iRepository->last >= iRepository->size)
	{
		Label *ptr;
		/* Expand array */
		iRepository->size += LABEL_ARRAY_INC;
		ptr = (Label*)realloc(iRepository->labels, iRepository->size * sizeof(Label));
		if(ptr == NULL)
		{
			deleteLabelRepository(iRepository);
			return 0;
		}
		
		iRepository->labels = ptr;
	}
	
	/* Push offset */
	iRepository->labels[iRepository->last].offset	    = iOffset;
	iRepository->labels[iRepository->last].displacement = 0;
		
	++iRepository->last;
	return 1;
}

/* Compare two labels */
static int compareLabels(const void* iL0, const void* iL1)
{
	const Label *l0 = (const Label*)iL0;
	const Label *l1 = (const Label*)iL1;
	
	return (l0->offset - l1->offset);
}

/* Finalize label repository */
void finalizeLabelRepositoty(LabelRepository* iRepository)
{
	if((iRepository->labels == NULL) || (iRepository->last < 2))
		return;

	/* Sort offsets */
	qsort(iRepository->labels, iRepository->last, sizeof(Label), compareLabels);
}

/* Parse section to identify potential labels */
int getLabels(FILE* iStream, Section* iSection, LabelRepository* iRepository)
{
	int32_t nBytesProcessed;
	unsigned char inst, data[6], eor;
	uint16_t labelOffset, offset;
	uint8_t  page;
	int delta,i=0;

	if(iSection->type != CODE)
		return 1;
		
	printf("\n%s:\n", iSection->name);

	nBytesProcessed = 0;
	offset          = iSection->org;
	
	/* Jump to section start */
	fseek(iStream, iSection->start, SEEK_SET);
		
	/* Push the section org */
	if(pushLabel(iRepository + ((offset>>13) & 0xff), offset) == 0)
		return 0;

	/* Walk along section */
	eor    = 0;
	while(!eor)
	{
		/* Read instruction */
		fread(&inst, 1, 1, iStream);

		/* Read data */
		if(pce_opcode[inst].size > 1)
		{
			fread(data, 1, pce_opcode[inst].size-1, iStream);
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
			page        = (labelOffset>>13) & 0xff;

			/* Insert offset to repository */
			if(pushLabel(iRepository + page, labelOffset) == 0)
				return 0;

			printf("%04x short jump to %04x\n", offset, labelOffset);
		}
		else 
		{
			if(isFarJump(inst))
			{
				labelOffset = data[0] | (data[1] << 8);
				page        = (labelOffset>>13) & 0xff;
				/* Insert offset to repository */
				if(pushLabel(iRepository + page, labelOffset) == 0)
					return 0;

				printf("%04x long jump to %04x\n", offset, labelOffset);
			}
		}
		
		/* Search end of section */
		if(iSection->size < 0)
		{
			if((inst == 0x40) || (inst == 0x60))
			{
				iSection->size = nBytesProcessed;
				eor            = 1;
			}
		}
		else
		{
			if(nBytesProcessed >= iSection->size)
				eor = 1;
		}		
	}
	return 1;
}

/* Initialize label index so that it points to the label close to current org offset */
void getLabelIndex(LabelRepository* iLabels, SectionProcessor* iProcessor)
{
	int i;
	
	/* Room for huge improvments */
	for(i=iProcessor->labelIndex; i<iLabels->last; ++i)
	{
		if(iLabels->labels[i].offset >= iProcessor->orgOffset)
		{
			break;
		}
	}
	
	iProcessor->labelIndex = i;
}

