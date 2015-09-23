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
#include "labels.h"
#include "cfg.h"
#include "message.h"

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
    iRepository->first = 0;
    iRepository->last  = 0;

    iRepository->nameBuffer    = NULL;
    iRepository->nameBufferLen = 0;
    
    iRepository->sorted = NULL;

    return 1;
}

/* Reset label repsitory */
void resetLabelRepository(LabelRepository* iRepository)
{
    iRepository->last = iRepository->first;
}

/* Delete label repository */
void deleteLabelRepository(LabelRepository* iRepository)
{
    iRepository->first = 0;
    iRepository->size  = 0;
    iRepository->last  = 0;

    if(NULL != iRepository->labels)
    {
        free(iRepository->labels);
        iRepository->labels = NULL;
    }

    if(NULL != iRepository->nameBuffer)
    {
        free(iRepository->nameBuffer);
        iRepository->nameBuffer = NULL;
    }
    iRepository->nameBufferLen = 0;

    if(NULL != iRepository->sorted)
    {
        free(iRepository->sorted);
        iRepository->sorted = NULL;
    }
}

/* Set name and add it to label name buffer */
static int addLabelName(LabelRepository* iRepository, Label *label, const char* name)
{
    char *tmp;
    size_t nameLen = strlen(name) + 1;
    size_t len     = iRepository->nameBufferLen + nameLen;
    tmp = (char*)realloc(iRepository->nameBuffer, len);
    if(NULL == tmp)
    {
        return 0;
    }

    label->name = tmp + iRepository->nameBufferLen;
    memcpy(label->name, name, nameLen+1);

    iRepository->nameBuffer    = tmp;
    iRepository->nameBufferLen = len;

    return 1;
}

/* Push label to repository */
int pushLabel(LabelRepository* iRepository, uint16_t iOffset, const char* name)
{
    if(iRepository->last >= iRepository->size)
    {
        Label  *ptr;
        /* Expand arrays */
        iRepository->size += LABEL_ARRAY_INC;
        ptr = (Label*)realloc(iRepository->labels, iRepository->size * sizeof(Label));
        if(NULL == ptr)
        {
            deleteLabelRepository(iRepository);
            return 0;
        }	
        iRepository->labels = ptr;
    }
    /* Push offset */
    iRepository->labels[iRepository->last].offset       = iOffset;
    iRepository->labels[iRepository->last].displacement = 0;
    if(0 == addLabelName(iRepository, &iRepository->labels[iRepository->last], name))
    {
        deleteLabelRepository(iRepository);
        return 0;
    }
    ++iRepository->last;
    return 1;
}

/* Compare two labels */
static int compareLabels(const void* iL0, const void* iL1)
{
	const Label *l0 = (const Label*)iL0;
	const Label *l1 = (const Label*)iL1;
	// [todo]
	return (l0->offset - l1->offset);
}

#define CSWAP(i,j) \
{ \
    size_t x = repository->sorted[i]; \
    size_t a = repository->labels[x].offset; \
    size_t y = repository->sorted[j]; \
    size_t b = repository->labels[y].offset; \
    repository->sorted[i]  = (b < a) ? x : y; \
    repository->sorted[j] ^= repository->sorted[i] ^ x; \
}

/* 3-way partition */
static void partition(LabelRepository* repository, size_t lo, size_t hi, size_t *lt, size_t *gt)
{
    size_t i, tmp;
    if(repository->labels[repository->sorted[hi]].offset < repository->labels[repository->sorted[lo]].offset)
    {
        tmp = repository->sorted[hi];
        repository->sorted[hi] = repository->sorted[lo];
        repository->sorted[lo] = i;
    }
    *lt = lo+1;
    *gt = hi-1;
    i = *lt;
    
    while(i <= *gt)
    {
        if(repository->labels[repository->sorted[i]].offset <  repository->labels[repository->sorted[lo]].offset)
        {
           tmp = repository->sorted[*lt];
           repository->sorted[*lt] = repository->sorted[i]; 
           repository->sorted[i]   = tmp;
            ++*lt;
        }
        else if(repository->labels[repository->sorted[hi]].offset <  repository->labels[repository->sorted[i]].offset)
        {
            while((repository->labels[repository->sorted[*gt]].offset > repository->labels[repository->sorted[hi]].offset) && (i < *gt))
            {
                --*gt;
            }
           tmp = repository->sorted[*gt];
           repository->sorted[*gt] = repository->sorted[i]; 
           repository->sorted[i]   = tmp;

            --*gt;
            if(repository->labels[repository->sorted[i]].offset <  repository->labels[repository->sorted[lo]].offset)
            {
               tmp = repository->sorted[*lt]; 
               repository->sorted[*lt] = repository->sorted[i]; 
               repository->sorted[i]   = tmp;
                ++*lt;
            }
        }
        ++i;
    }

    --*lt;
    ++*gt;

    tmp = repository->sorted[*lt];
    repository->sorted[*lt] = repository->sorted[lo]; 
    repository->sorted[ lo]   = tmp;

    tmp = repository->sorted[*gt]; 
    repository->sorted[*gt] = repository->sorted[hi]; 
    repository->sorted[ hi]   = tmp;
}

/* Sort label repository */
static void sortLabelRepository(LabelRepository* repository, size_t lo, size_t hi)
{
    size_t len = hi - lo + 1;
    switch(len)
    {
        case 0:
        case 1:
            break;
        case 2:
            CSWAP(0,1);
            break;
        case 3:
            CSWAP(0,2);
            CSWAP(0,1);
            CSWAP(1,2);
            break;
        case 4:
            CSWAP(0, 2)
            CSWAP(1, 3)
            CSWAP(0, 1)
            CSWAP(2, 3)
            CSWAP(1, 2)
            break;
        case 5:
            CSWAP(0, 1)
            CSWAP(3, 4)
            CSWAP(2, 4)
            CSWAP(2, 3)
            CSWAP(1, 4)
            CSWAP(1, 2)
            CSWAP(0, 3)
            CSWAP(0, 1)
            CSWAP(2, 3)
            break;
        case 6:
            CSWAP(0, 4)
            CSWAP(1, 5)
            CSWAP(0, 2)
            CSWAP(1, 3)
            CSWAP(2, 4)
            CSWAP(3, 5)
            CSWAP(0, 1)
            CSWAP(2, 3)
            CSWAP(4, 5)
            CSWAP(1, 4)
            CSWAP(1, 2)
            CSWAP(3, 4)
            break;
        case 7:
            CSWAP(0, 4)
            CSWAP(1, 5)
            CSWAP(2, 6)
            CSWAP(0, 2)
            CSWAP(1, 3)
            CSWAP(4, 6)
            CSWAP(2, 4)
            CSWAP(3, 5)
            CSWAP(0, 1)
            CSWAP(2, 3)
            CSWAP(4, 5)
            CSWAP(1, 4)
            CSWAP(3, 6)
            CSWAP(1, 2)
            CSWAP(3, 4)
            CSWAP(5, 6)
            break;
        case 8:
            CSWAP(0, 4)
            CSWAP(1, 5)
            CSWAP(2, 6)
            CSWAP(3, 7)
            CSWAP(0, 2)
            CSWAP(1, 3)
            CSWAP(4, 6)
            CSWAP(5, 7)
            CSWAP(2, 4)
            CSWAP(3, 5)
            CSWAP(0, 1)
            CSWAP(2, 3)
            CSWAP(4, 5)
            CSWAP(6, 7)
            CSWAP(1, 4)
            CSWAP(3, 6)
            CSWAP(1, 2)
            CSWAP(3, 4)
            CSWAP(5, 6)
            break;    
        default:
            if(repository->last < 17)
            {
                size_t i;
                /* Insertion sort */
                for(i=lo+1; i<=hi; i++)
                {
                    size_t j;
                    size_t k = repository->sorted[i];
                    size_t o = repository->labels[k].offset;
                    for(j=i; (j>lo) && (repository->labels[repository->sorted[j-1]].offset > o); j--)
                    {
                        repository->sorted[j] = repository->sorted[j-1];
                    }
                    repository->sorted[j] = k;
                }
            }
            else
            {
                size_t lt, gt;
                /* Quicksort */
                partition(repository, lo, hi, &lt, &gt);
                if(repository->labels[repository->sorted[lt]].offset < repository->labels[repository->sorted[gt]].offset)
                {
                    sortLabelRepository(repository, lt+1, gt-1);
                    sortLabelRepository(repository, gt+1, hi  );
                    sortLabelRepository(repository,   lo, lt-1);
                }
                else
                {
                    sortLabelRepository(repository, gt+1, hi  );
                    sortLabelRepository(repository, lo  , lt-1);
                }
            }
            break;
    }
}

/* Finalize label repository */
void finalizeLabelRepositoty(LabelRepository* iRepository)
{
    size_t i;
    size_t *tmp;

    if((iRepository->labels == NULL) || (iRepository->last < 2))
        return;
        
    tmp = (size_t*)realloc(iRepository->sorted, iRepository->size * sizeof(size_t));
    if(NULL == tmp)
    {
        deleteLabelRepository(iRepository);
        // [todo] return 0;
    }   
    iRepository->sorted = tmp;
    for(i=0; i<iRepository->size; i++)
    {
        iRepository->sorted[i] = i;
    }

    /* Sort offsets */
    // [todo]
    qsort(iRepository->sorted, iRepository->last, sizeof(size_t), compareLabels);
}

/* Label CFG section start */
static int labelBeginSectionCallback(void *data, const char *sectionName)
{
    (void)data;
    (void)sectionName;
    return 1;
}
/* Label CFG section end */
static int labelEndSectionCallback(void *data)
{
    (void)data;
    return 1;
}
/* Add name/label to repository */
int labelKeyValueCallback(void *data, const char* key, const char* value)
{
    LabelRepository *repository = (LabelRepository*)data;
    (void)key;
    if(NULL == repository)
    {
        return 0;
    }
    else
    {
        char *end = NULL;
        uint16_t addr = (uint16_t)(strtoul(value, &end, 16) & 0xffff);
        if((NULL != end) && ('\0' != *end))
        {
            ERROR_MSG("Invalid offset %s : %s", value, strerror(errno));
            return 0;
        }
        return pushLabel(repository, addr, key);
    }
}

/**
 * Load labels from a cfg file.
 * \param [in]  filename Cfg file.
 * \param [out] reposity Label repository.
 * \return 1 if the labels contained in the file was succesfully added to the repository.
 *         0 if an error occured.
 */
int loadLabels(const char* filename, LabelRepository* repository)
{
    CFG_ERR err;
    struct CFGPayload payload;

    payload.data = repository;
    payload.beginSectionCallback = labelBeginSectionCallback;
    payload.endSectionCallback   = labelEndSectionCallback;
    payload.keyValueCallback     = labelKeyValueCallback;

    err = ParseCFG(filename, &payload);
    if(CFG_OK != err)
    {
        ERROR_MSG("Failed to load labels from %s : %s", filename, GetCFGErrorMsg(err));
        return 0;
    }
    repository->first = repository->last;
// [todo]
    return 1;
}
