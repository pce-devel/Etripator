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
  copyright (c) 2009--2023 Vincent Cruz.
 
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

#define LABEL_ARRAY_INC 16

/**
 * Label repository.
 */
struct label_repository_impl {
    size_t size;            /**< Size of label repository */
    size_t last;            /**< Last element in the repository */
    label_t *labels;        /**< Labels */
};

/**
 * Get label index by its address.
 * \param [in]  repository  Label repository.
 * \param [in]  logical     Logical address.
 * \param [in]  page        Memory page.
 * \return label index or -1 if the label was not found.
 */
static int label_repository_index(label_repository_t* repository, uint16_t logical, uint8_t page) {
    size_t i;
    for(i=0; i<repository->last; i++) {
        if( (repository->labels[i].page == page) &&
            (repository->labels[i].logical == logical) ) {
            return (int)i;
        }
    }
    return -1;
}

/* Create label repository. */
label_repository_t* label_repository_create() {
    label_repository_t *repository;
    repository = (label_repository_t*)malloc(sizeof(label_repository_t));
    if(repository == NULL) {
        ERROR_MSG("Failed to create label repository: %s", strerror(errno));
        return NULL;
    }
    
    repository->last  = 0;

    repository->labels = NULL;

    repository->size = LABEL_ARRAY_INC;
    repository->labels = (label_t*)malloc(repository->size * sizeof(label_t));
    if(repository->labels == NULL) {
        ERROR_MSG("Failed to create label: %s", strerror(errno));
        label_repository_destroy(repository);
        free(repository);
        return NULL;
    }
    
    return repository;
}

/*  Delete label repository. */
void label_repository_destroy(label_repository_t* repository) {
    repository->size  = 0;
    repository->last  = 0;

    if(repository->labels != NULL) {
        for(int i=0; i<repository->last; i++) {
            if(repository->labels[i].name) {
                free(repository->labels[i].name);
            }
            if(repository->labels[i].description) {
                free(repository->labels[i].description);
            }
        }
        free(repository->labels);
        repository->labels = NULL;
    }
}

/* Add label to repository. */
int label_repository_add(label_repository_t* repository, const char* name, uint16_t logical, uint8_t page, const char *description) {
    int ret = 1;
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
            label_t *ptr;
            repository->size += LABEL_ARRAY_INC;                
            ptr = (label_t*)realloc(repository->labels, repository->size * sizeof(label_t));
            if(ptr == NULL) {
                label_repository_destroy(repository);
                ret = 0;
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

/* Find a label by its address. */
int label_repository_find(label_repository_t* repository, uint16_t logical, uint8_t page, label_t *out) {
    int index = label_repository_index(repository, logical, page);
    if(index < 0) {
        memset(out, 0, sizeof(label_t));
        return 0;
    }
    memcpy(out, &repository->labels[index], sizeof(label_t));
    return 1;
}

/* Get the number of labels stored in the repository. */
int label_repository_size(label_repository_t* repository) {
    return repository ? (int)repository->last : 0;
}

/* Retrieve the label at the specified index. */
int label_repository_get(label_repository_t* repository, int index, label_t *out) {
    if((repository != NULL) && ((index >= 0) && (index < (int)repository->last))) {    
        memcpy(out, &repository->labels[index], sizeof(label_t));
        return 1;
    } else {
        memset(out, 0, sizeof(label_t));
        return 0;
    }
}
/* Delete labels */
int label_repository_delete(label_repository_t* repository, uint16_t first, uint16_t end, uint8_t page) {
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
                memcpy(&repository->labels[i], &repository->labels[repository->last], sizeof(label_t));
                i--;
            }
        }
    }
    return 1;
}
