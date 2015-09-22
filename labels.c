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

            return 0;
        }
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

/* Finalize label repository */
void finalizeLabelRepositoty(LabelRepository* iRepository)
{
    size_t i;
    size_t *tmp;

    if((iRepository->labels == NULL) || (iRepository->last < 2))
        return;
        
    tmp = (size_t*)realloc(iRepository->sorted, iRepository->size * size(size_t));
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
