/*
    This file is part of Etripator,
    copyright (c) 2009--2019 Vincent Cruz.

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
 * Label.
 */
typedef struct {
    size_t   name;    /**< Offset in the repository name buffer */
    uint16_t logical; /**< Logical address */
    uint8_t  page;    /**< Memory page  */
} label_t;

/**
 * Label repository.
 */
struct label_repository_impl {
    size_t size;            /**< Size of label repository */
    size_t last;            /**< Last element in the repository */
    label_t *labels;        /**< Labels */
    size_t name_buffer_len; /**< Label name buffer length */
    char   *name_buffer;    /**< Label name buffer */
};

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

    repository->name_buffer     = NULL;
    repository->name_buffer_len = 0;

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

    repository->name_buffer_len = 0;

    if(repository->labels != NULL) {
        free(repository->labels);
        repository->labels = NULL;
    }

    if(repository->name_buffer != NULL) {
        free(repository->name_buffer);
        repository->name_buffer = NULL;
    }
}

/* Set name and add it to label name buffer */
static int label_set(label_repository_t* repository, label_t *label, const char* name) {
    char *tmp;
    size_t name_len = strlen(name) + 1;
    size_t len      = repository->name_buffer_len + name_len;
    tmp = (char*)realloc(repository->name_buffer, len);
    if(NULL == tmp) {
        return 0;
    }

    label->name = repository->name_buffer_len;
    memcpy(tmp+label->name, name, name_len);

    repository->name_buffer     = tmp;
    repository->name_buffer_len = len;
    return 1;
}

/**
 * Add label to repository.
 * \param [in,out] repository Label repository.
 * \param [in]     name     Name.
 * \param [in]     logical  Logical address.
 * \param [in]     page     Memory page.
 */
int label_repository_add(label_repository_t* repository, const char* name, uint16_t logical, uint8_t page) {
    char *dummy;

    if(label_repository_find(repository, logical, page, &dummy)) {
        if(strcmp(name, dummy)) {
        //    return 0;
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
    
    /* Push name */
    if(!label_set(repository, &repository->labels[repository->last], name)) {
        label_repository_destroy(repository);
        return 0;
    }

    ++repository->last;
    return 1;
}

/**
 * Find a label by its address.
 * \param [in]  repository  Label repository.
 * \param [in]  logical     Logical address.
 * \param [in]  page        Memory page.
 * \param [out] name        Label name (if found).
 * \return 1 if a label was found, 0 otherwise.
 */
int label_repository_find(label_repository_t* repository, uint16_t logical, uint8_t page, char** name) {
    size_t i;
    for(i=0; i<repository->last; i++) {
        if( (repository->labels[i].page == page) &&
            (repository->labels[i].logical == logical) ) {
            *name = repository->name_buffer + repository->labels[i].name;
            return 1;
        }
    }
    *name = "";
    return 0;
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
 * \param [in] repository Label repository.
 * \param [in] index      Label index.
 * \param [out] logical   Logical address.
 * \param [out] page      Memory page.
 * \param [out] name      Label name.
 * \return 1 if a label exists for the specified index, 0 otherwise.
 */
int label_repository_get(label_repository_t* repository, int index, uint16_t* logical, uint8_t* page, char** name) {
    *page = 0;
    *logical  = 0;
    *name = NULL;
    if(repository == NULL) {
        return 0;
    }
    else {
        int end = (int)repository->last;
        if((index < 0) || (index >= end)) {
            return 0;
        }
        if(repository->labels[index].name >= repository->name_buffer_len) {
            return 0;
        }
        *logical = repository->labels[index].logical;
        *page = repository->labels[index].page;
        *name = repository->name_buffer + repository->labels[index].name;
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
                memcpy(&repository->labels[i], &repository->labels[repository->last], sizeof(label_t));
                i--;
            }
        }
    }
    int ret = 1;
    char *tmp = repository->name_buffer;
    repository->name_buffer_len = 0;
    repository->name_buffer     = NULL;
    for(i=0; ret && (i<repository->last); i++) {
        ret = label_set(repository, &repository->labels[i], tmp + repository->labels[i].name);
    }    
    if(tmp) {
        free(tmp);
    }
    return ret;
}
