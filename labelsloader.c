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
#include "labelsloader.h"
#include "message.h"
#include "cfg.h"

#define MAX_LABEL_NAME 64

typedef struct
{
    LabelRepository *repository;
    uint16_t logical;
    size_t   physical;
    uint8_t  flag;
    char name[MAX_LABEL_NAME];
} LabelParserPayload;

/* Label CFG section start */
static int labelBeginSectionCallback(void *data, const char *sectionName)
{
    LabelParserPayload *payload = (LabelParserPayload*)data;
    size_t len = strlen(sectionName);
    if(len >= MAX_LABEL_NAME)
    {
        len = MAX_LABEL_NAME-1;
    }
    memcpy(payload->name, sectionName, len);
    payload->name[len] = '\0';

    payload->logical  = 0x0000;
    payload->physical = INVALID_PHYSICAL_ADDRESS;
    payload->flag     = 0;
    
    return 1;
}

/* Label CFG section end */
static int labelEndSectionCallback(void *data)
{
    int ret;
    LabelParserPayload *payload = (LabelParserPayload*)data;
    
    if(0x03 != payload->flag)
    {
        if(0 == (payload->flag & 0x01))
        {
            ERROR_MSG("Missing physical address for %s", payload->name);
        }
        if(0 == (payload->flag & 0x02))
        {
            ERROR_MSG("Missing logical address for %s", payload->name);
        }
        return 0;
    }
    
    ret = addLabel(payload->repository, payload->name, payload->logical, payload->physical);
    if(ret < 0)
    {
        ERROR_MSG("Duplicate label %s.", payload->name);
        return 0;
    }
    else if(ret == 0)
    {
        ERROR_MSG("An error occured while adding label %s to repository.", payload->name);
        return 0;
    }
    return 1;
}
/* Add name/label to repository */
int labelKeyValueCallback(void *data, const char* key, const char* value)
{
    LabelParserPayload *payload = (LabelParserPayload*)data;
    char *end = NULL;

    size_t addr = (size_t)strtoul(value, &end, 16);
    if((NULL != end) && ('\0' != *end))
    {
        ERROR_MSG("Invalid %s address : %s", key, strerror(errno));
        return 0;
    }

    if(0 == strcmp("physical", key))
    {
        payload->physical = addr;
        payload->flag |= 0x01;
    }
    else if(0 == strcmp("logical", key))
    {
        payload->logical = addr & 0xffff;
        payload->flag |= 0x02;
    }
    
    return 1;
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
    
    LabelParserPayload labelPayload;
    labelPayload.repository = repository;

    payload.data = &labelPayload;
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
