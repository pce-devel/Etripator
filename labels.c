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

/**
 * Initialize label repository.
 * \param [in,out] repository Label repository.
 * \return 1 if initialization succedded, 0 if an error occured.
 */
int initializeLabelRepository(LabelRepository* repository)
{
    repository->size = LABEL_ARRAY_INC;
    repository->labels = (Label*)malloc(repository->size * sizeof(Label));
    if(repository->labels == NULL)
    {
        return 0;
    }
    repository->first = 0;
    repository->last  = 0;

    repository->nameBuffer    = NULL;
    repository->nameBufferLen = 0;
    
    repository->sorted = NULL;

    return 1;
}

/* Reset label repsitory */
void resetLabelRepository(LabelRepository* repository)
{
    repository->last = repository->first;
}

/* Delete label repository */
void deleteLabelRepository(LabelRepository* repository)
{
    repository->first = 0;
    repository->size  = 0;
    repository->last  = 0;

    if(NULL != repository->labels)
    {
        free(repository->labels);
        repository->labels = NULL;
    }

    if(NULL != repository->nameBuffer)
    {
        free(repository->nameBuffer);
        repository->nameBuffer = NULL;
    }
    repository->nameBufferLen = 0;

    if(NULL != repository->sorted)
    {
        free(repository->sorted);
        repository->sorted = NULL;
    }
}

/* Set name and add it to label name buffer */
static int setLabelName(LabelRepository* repository, Label *label, const char* name)
{
    char *tmp;
    size_t nameLen = strlen(name) + 1;
    size_t len     = repository->nameBufferLen + nameLen;
    tmp = (char*)realloc(repository->nameBuffer, len);
    if(NULL == tmp)
    {
        return 0;
    }

    label->name = repository->nameBufferLen;
    memcpy(tmp+label->name, name, nameLen);

    repository->nameBuffer    = tmp;
    repository->nameBufferLen = len;

    return 1;
}

/* Push label to repository */
int pushLabel(LabelRepository* repository, uint16_t iOffset, const char* name)
{
    if(repository->last >= repository->size)
    {
        Label  *ptr;
        /* Expand arrays */
        repository->size += LABEL_ARRAY_INC;
        ptr = (Label*)realloc(repository->labels, repository->size * sizeof(Label));
        if(NULL == ptr)
        {
            deleteLabelRepository(repository);
            return 0;
        }
        repository->labels = ptr;
    }
    /* Push offset */
    repository->labels[repository->last].offset       = iOffset;
    repository->labels[repository->last].displacement = 0;
    if(0 == setLabelName(repository, &repository->labels[repository->last], name))
    {
        deleteLabelRepository(repository);
        return 0;
    }
    ++repository->last;
    return 1;
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
        repository->sorted[lo] = tmp;
    }

    *lt = lo+1;
    *gt = hi-1;
    i = *lt;
    
    while(i <= *gt)
    {
        if(repository->labels[repository->sorted[i]].offset < repository->labels[repository->sorted[lo]].offset)
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
    if(len < 17)
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
            if(lo < lt) sortLabelRepository(repository,   lo, lt-1);
            if(lt < gt) sortLabelRepository(repository, lt+1, gt-1);
            if(gt < hi) sortLabelRepository(repository, gt+1, hi  );
        }
        else
        {
            if(lo < lt) sortLabelRepository(repository, lo  , lt-1);
            if(gt < hi) sortLabelRepository(repository, gt+1, hi  );
        }
    }
}

/* Finalize label repository */
void finalizeLabelRepositoty(LabelRepository* repository)
{
    size_t i;
    size_t *tmp;

    if((repository->labels == NULL) || (repository->last < 2))
        return;
        
    tmp = (size_t*)realloc(repository->sorted, repository->size * sizeof(size_t));
    if(NULL == tmp)
    {
        deleteLabelRepository(repository);
        // [todo] return 0;
    }   
    repository->sorted = tmp;
    for(i=0; i<repository->size; i++)
    {
        repository->sorted[i] = i;
    }

    /* Sort offsets */
    sortLabelRepository(repository, 0, repository->last-1);
}

/**
 * Check if there's a label at the specified logical address.
 * \param [in]  repository  Label repository.
 * \param [in]  logical     Logical address.
 * \param [in]  nextLogical Next instruction logical address.
 * \param [out] name        Label name (if found).
 * \return 1 if a label was found, 0 otherwise.
 */
int findLabel(LabelRepository* repository, uint16_t logical, uint16_t nextLogical, char** name)
{
    size_t i = 0;
    for(i=0; i<repository->last; i++)
    {
        size_t j = repository->sorted[i];
        if((repository->labels[j].offset >= logical) &&
           (repository->labels[j].offset < nextLogical))
        {
            *name = repository->nameBuffer + repository->labels[j].name;
            repository->labels[j].displacement = repository->labels[j].offset - logical;
            return 1;
        }
    }
    *name = "_";
    return 0;
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
