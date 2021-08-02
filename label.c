/*
    This file is part of Etripator,
    copyright (c) 2009--2021 Vincent Cruz.

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
            return i;
        }
    }
    return -1;
}

/**
 * Create label repository.
 * \return A pointer to a label repository or NULL if an error occured.
 */
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

/**
 * Delete label repository.
 * \param [in,out] repository Label repository.
 */
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

/**
 * Add label to repository.
 * \param [in,out] repository Label repository.
 * \param [in]     name        Name.
 * \param [in]     logical     Logical address.
 * \param [in]     page        Memory page.
 * \param [in]     description Description (optional).
 */
int label_repository_add(label_repository_t* repository, const char* name, uint16_t logical, uint8_t page, const char *description) {
    int index = label_repository_index(repository, logical, page);
    if(index >= 0) {
        if(strcmp(name, repository->labels[index].name)) {
        //    return 0;
        }
        if(description && !repository->labels[index].description) {
            repository->labels[index].description = strdup(description);
        }
        return  1;
    }

    /* Expand arrays if necessary */
    if(repository->last >= repository->size) {
        label_t *ptr;
        repository->size += LABEL_ARRAY_INC;
        
        ptr = (label_t*)realloc(repository->labels, repository->size * sizeof(label_t));
        if(ptr == NULL) {
            label_repository_destroy(repository);
            return 0;
        }
        repository->labels = ptr;
    }
    
    /* Push addresses */
    repository->labels[repository->last].logical = logical;
    repository->labels[repository->last].page    = page;
    
    /* Push name and description */
    repository->labels[repository->last].name = strdup(name);
    repository->labels[repository->last].description = description;
    ++repository->last;
    return 1;
}

/**
 * Find a label by its address.
 * \param [in]  repository  Label repository.
 * \param [in]  logical     Logical address.
 * \param [in]  page        Memory page.
 * \param [out] label       Label (if found).
 * \return 1 if a label was found, 0 otherwise.
 */
int label_repository_find(label_repository_t* repository, uint16_t logical, uint8_t page, label_t *out) {
    int index = label_repository_index(repository, logical, page);
    if(index < 0) {
        memset(out, 0, sizeof(label_t));
        return 0;
    }
    memcpy(out, &repository->labels[index], sizeof(label_t));
    return 1;
}

/**
 * Get the number of labels stored in the repository.
 * \param [in] repository Label repository.
 * \return Label count.
 */
int label_repository_size(label_repository_t* repository) {
    if(repository == NULL) {
        return 0;
    }
    return (int)repository->last;
}

/**
 * Retrieve the label at the specified index.
 * \param [in]  repository Label repository.
 * \param [in]  index      Label index.
 * \param [out] out        Label.
 * \return 1 if a label exists for the specified index, 0 otherwise.
 */
int label_repository_get(label_repository_t* repository, int index, label_t *out) {
    memset(out, 0, sizeof(label_t));
    if(repository == NULL) {
        return 0;
    }
    else {
        int end = (int)repository->last;
        if((index < 0) || (index >= end)) {
            return 0;
        }
        memcpy(out, &repository->labels[index], sizeof(label_t));
        return 1;
    }
}
/**
 * Delete labels
 * \param [in]  repository  Label repository.
 * \param [in]  first       Start of the logical address range.
 * \param [in]  end         End of the logical address range.
 * \param [in]  page        Memory page.
 */
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
