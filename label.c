/*
¬°¤*,¸¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸
¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯

  __/¯\____ ___/\__   _/\__   _/\_   _/\__   _/\___ ___/\__   __/\_   _/\__   
  \_  ____/_> ____ \_/  _  \_ \  <  /_    \_/     _>> ____ \_ >    \_/  _  \_ 
  _> ___/ ¯>__> <<__// __  _/ |>  ></ _/>  </  ¯  \\__> <<__//  /\  // __  _/ 
 _>  \7   <__/:. \__/:. \>  \_/   L/  _____/.  7> .\_/:. \__/  <_/ </:. \>  \_ 
|:::::::::::::::::::::::/:::::::::::::>::::::::/::::::::::::::::::::::::/:::::|
|¯¯\::::/\:/¯\::::/¯¯¯¯<::::/\::/¯¯\:/¯¯¯¯¯¯\::\::/¯¯\::::/¯¯\::::/¯¯¯¯<::::/¯|
|__ |¯¯|  T _ |¯¯¯| ___ |¯¯|  |¯| _ T ______ |¯¯¯¯| _ |¯¯¯| _ |¯¯¯| ___ |¯¯| _|
   \|__|/\|/ \|___|/   \|__|/\|_|/ \|/      \|    |/ \|___|/ \|___|/dNo\|__|/  

¬°¤*,¸¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸
¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯

  This file is part of Etripator,
  copyright (c) 2009--2024 Vincent Cruz.
 
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

¬°¤*,¸¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸
¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯¬°¤*,¸_¸,*¤°¬°¤*,¸,*¤°¬¯
*/
#include "label.h"
#include "message.h"

#include <assert.h>

#define LABEL_ARRAY_INC 16

/// Get label index by its address.
/// \param [in]  repository  Label repository.
/// \param [in]  logical     Logical address.
/// \param [in]  page        Memory page.
/// \return label index.
/// \return  -1 if the label was not found.
static int label_repository_index(LabelRepository *repository, uint16_t logical, uint8_t page) {
    size_t i;
    for(i=0; i<repository->last; i++) {
        if( (repository->labels[i].page == page) &&
            (repository->labels[i].logical == logical) ) {
            return (int)i;
        }
    }
    return -1;
}

// Create label repository.
bool label_repository_create(LabelRepository* repository) {
    assert(repository != NULL);
    bool ret = true;
    repository->last  = 0;
    repository->labels = NULL;
    repository->size = LABEL_ARRAY_INC;
    repository->labels = (Label*)malloc(repository->size * sizeof(Label));
    if(repository->labels == NULL) {
        ERROR_MSG("Failed to create label: %s", strerror(errno));
        label_repository_destroy(repository);
        ret = false;
    }
    return ret;
}

//  Delete label repository.
void label_repository_destroy(LabelRepository* repository) {
    repository->size  = 0;
    repository->last  = 0;

    if(repository->labels != NULL) {
        for(size_t i=0; i<repository->last; i++) {
            free(repository->labels[i].name);
            free(repository->labels[i].description);
        }
        free(repository->labels);
        repository->labels = NULL;
    }
}

// Add label to repository.
bool label_repository_add(LabelRepository* repository, const char* name, uint16_t logical, uint8_t page, const char *description) {
    assert(repository != NULL);
    bool ret = true;
    int index = label_repository_index(repository, logical, page);
    if(index >= 0) {
#if 0
        if(strcmp(name, repository->labels[index].name)) {
            //    return 0;
        }
#endif
        if(description && !repository->labels[index].description) {
            repository->labels[index].description = strdup(description);
        }
    } else {
        /* Expand arrays if necessary */
        if(repository->last >= repository->size) {
            Label *ptr;
            repository->size += LABEL_ARRAY_INC;                
            ptr = (Label*)realloc(repository->labels, repository->size * sizeof(Label));
            if(ptr == NULL) {
                label_repository_destroy(repository);
                ret = false;
            } else {
                repository->labels = ptr;
            }
        }
        if(ret != 0) {
            /* Push addresses */
            repository->labels[repository->last].logical = logical;
            repository->labels[repository->last].page    = page;

            /* Push name and description */
            repository->labels[repository->last].name = strdup(name);
            repository->labels[repository->last].description = (description != NULL) ? strdup(description) : NULL;

            ++repository->last;
        }
    }
    return ret;
}

// Find a label by its address.
bool label_repository_find(LabelRepository* repository, uint16_t logical, uint8_t page, Label *out) {
    int index = label_repository_index(repository, logical, page);
    bool ret = (index >= 0);
    if(ret) {
        memcpy(out, &repository->labels[index], sizeof(Label));
    } else {
        memset(out, 0, sizeof(Label));
    }
    return ret;
}

// Get the number of labels stored in the repository.
int label_repository_size(LabelRepository* repository) {
    assert(repository != NULL);
    return (int)repository->last;
}

// Retrieve the label at the specified index.
bool label_repository_get(LabelRepository* repository, int index, Label *out) {
    bool ret = false;
    if((repository != NULL) && ((index >= 0) && (index < (int)repository->last))) {
        ret = true;
        memcpy(out, &repository->labels[index], sizeof(Label));
    } else {
        memset(out, 0, sizeof(Label));
    }
    return ret;
}

// Delete labels.
void label_repository_delete(LabelRepository* repository, uint16_t first, uint16_t end, uint8_t page) {
    size_t i;
    for(i=0; i<repository->last; i++) {
        if( (repository->labels[i].page == page) &&
            (repository->labels[i].logical >= first) && 
            (repository->labels[i].logical < end) ) {
            if(repository->last) {
                repository->last--;
                if(repository->labels[i].name) {
                    free(repository->labels[i].name);
                }
                if(repository->labels[i].description) {
                    free(repository->labels[i].description);
                }
                memcpy(&repository->labels[i], &repository->labels[repository->last], sizeof(Label));
                i--;
            }
        }
    }
}
