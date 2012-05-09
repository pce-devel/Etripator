/*
    This file is part of Etripator,
    copyright (c) 2009--2012 Vincent Cruz.

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
