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
 * Label.
 */
typedef struct
{
    size_t   name;     /**< Offset in the repository name buffer */
    uint16_t logical;  /**< Logical address */
    uint32_t physical; /**< Physical address */
} Label;

/**
 * Label repository.
 */
struct LabelRepositoryImpl
{
    size_t size;          /**< Size of label repository */
    size_t last;          /**< Last element in the repository */
    Label  *labels;       /**< Labels */
    size_t *sorted;       /**< Index of sorted labels */
    size_t nameBufferLen; /**< Label name buffer length */
    char   *nameBuffer;   /**< Label name buffer */
};

/**
 * Create label repository.
 * \return A pointer to a label repository or NULL if an error occured.
 */
LabelRepository* createLabelRepository()
{
    LabelRepository *repository;
    repository = (LabelRepository*)malloc(sizeof(LabelRepository));
    if(NULL == repository)
    {
        return NULL;
    }
    
    repository->last  = 0;

    repository->nameBuffer    = NULL;
    repository->nameBufferLen = 0;

    repository->labels = NULL;

    repository->size = LABEL_ARRAY_INC;
    repository->labels = (Label*)malloc(repository->size * sizeof(Label));
    if(NULL == repository->labels)
    {
        deleteLabelRepository(repository);
        free(repository);
        return NULL;
    }
    
    return repository;
}

/**
 * Delete label repository.
 * \param [in,out] repository Label repository.
 */
void deleteLabelRepository(LabelRepository* repository)
{
    repository->size  = 0;
    repository->last  = 0;

    repository->nameBufferLen = 0;

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

/**
 * Add label to repository.
 * \param [in,out] repository Label repository.
 * \param [in]     name       Name.
 * \param [in]     logical    Logical address.
 * \param [in]     physical   Physical address.
 */
int addLabel(LabelRepository* repository, const char* name, uint16_t logical, uint32_t physical)
{
    // Check if there's an entry for the given physical address
    if(INVALID_PHYSICAL_ADDRESS != physical)
    {
        char *dummy;
        if(findLabelByPhysicalAddress(repository, physical, &dummy))
        {
            return -1;
        }
    }

    /* Expand arrays if necessary */
    if(repository->last >= repository->size)
    {
        Label *ptr;
        repository->size += LABEL_ARRAY_INC;
        
        ptr = (Label*)realloc(repository->labels, repository->size * sizeof(Label));
        if(NULL == ptr)
        {
            deleteLabelRepository(repository);
            return 0;
        }
        repository->labels = ptr;
    }
    
    /* Push addresses */
    repository->labels[repository->last].logical  = logical;
    repository->labels[repository->last].physical = physical;
    
    /* Push name */
    if(0 == setLabelName(repository, &repository->labels[repository->last], name))
    {
        deleteLabelRepository(repository);
        return 0;
    }

    ++repository->last;
    return 1;
}

/**
 * Find if a label is associated to the specified physical address.
 * \param [in]  repository  Label repository.
 * \param [in]  physical    Physical address.
 * \param [out] name        Label name (if found).
 * \return 1 if a label was found, 0 otherwise.
 */
int findLabelByPhysicalAddress(LabelRepository* repository, uint16_t physical, char** name)
{
    size_t i;
    for(i=0; i<repository->last; i++)
    {
        if(repository->labels[i].physical == physical)
        {
            *name = repository->nameBuffer + repository->labels[i].name;
            return 1;
        }
    }
    *name = "";
    return 0;
}

/**
 * Find if a label is associated to the specified logical address.
 * \param [in]  repository  Label repository.
 * \param [in]  logical     Logical address.
 * \param [out] name        Label name (if found).
 * \return 1 if a label was found, 0 otherwise.
 * \todo How to handle multiple results ?
 */
int findLabelByLogicalAddress(LabelRepository* repository, uint16_t logical, char** name)
{
    size_t i;
    for(i=0; i<repository->last; i++)
    {
        if(repository->labels[i].logical == logical)
        {
            *name = repository->nameBuffer + repository->labels[i].name;
            return 1;
        }
    }
    *name = "";
    return 0;
}

// [todo] Move to a new file!
// [todo] New CFG format
// [name]
// logical=e812
// physical=a6812
//
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
        return addLabel(repository, key, addr, 0xcafe); // [todo] Do this in section end callback
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
    return 1;
}
