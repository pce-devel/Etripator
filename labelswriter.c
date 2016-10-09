/*
    This file is part of Etripator,
    copyright (c) 2009--2016 Vincent Cruz.

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
#include "labelswriter.h"
#include "message.h"

/**
 * Write labels to file.
 * \param [in] filename Configuration file.
 * \param [in] reposity Label repository.
 * \return 1 if the labels in the repository were succesfully written to the file.
 *         0 if an error occured.
 */
int writeLabels(const char* filename, LabelRepository* repository)
{
    int i;
    int count = 0;
    int ret = 0;
    FILE* stream = NULL;
    if((NULL == repository) || (NULL == filename))
    {
        ERROR_MSG("Invalid parameters.");
        return 0;
    }
    count = getLabelCount(repository);
    if(0 == count)
    {
        ERROR_MSG("Empty label repository.");
        return 0;
    }
    stream = fopen(filename, "wb");
    if(NULL == stream)
    {
        ERROR_MSG("Failed to open %s: %s", filename, strerror(errno));
        return 0;
    }

    ret = 1;
    for(i=0; ret && (i<count); i++)
    {
        uint32_t physical;
        uint16_t logical;
        char* name;
        if(!getLabel(repository, i, &physical, &logical, &name))
        {
            ERROR_MSG("Failed to retrieve label #%d", i);
            ret = 0;
        }
        else
        {
            fprintf(stream, "[%s]\nlogical=%04x\nphysical=%06x\n\n", name, logical, physical); 
        }
    }

    fclose(stream);
    return ret;
}
