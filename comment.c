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
#include "comment.h"
#include "message.h"

#include <jansson.h>
#include "jsonhelpers.h"

#define COMMENT_ARRAY_INC 16

/**
 * Comment repository.
 */
struct comment_repository_impl {
    size_t size;            /**< Size of comment repository */
    size_t last;            /**< Last element in the repository */
    comment_t *comments;        /**< Comments */
};

/**
 * Get comment index by its address.
 * \param [in]  repository  Coment repository.
 * \param [in]  logical     Logical address.
 * \param [in]  page        Memory page.
 * \return comment index or -1 if the label was not found.
 */
static int comment_repository_index(comment_repository_t* repository, uint16_t logical, uint8_t page) {
    size_t i;
    for(i=0; i<repository->last; i++) {
        if( (repository->comments[i].page == page) &&
            (repository->comments[i].logical == logical) ) {
            return (int)i;
        }
    }
    return -1;
}

/* Create comment repository. */
comment_repository_t* comment_repository_create() {
    comment_repository_t *repository;
    repository = (comment_repository_t*)malloc(sizeof(comment_repository_t));
    if(repository == NULL) {
        ERROR_MSG("Failed to create comment repository: %s", strerror(errno));
    } else {
        repository->last  = 0;
        repository->comments = NULL;

        repository->size = COMMENT_ARRAY_INC;
        repository->comments = (comment_t*)malloc(repository->size * sizeof(comment_t));
        if(repository->comments == NULL) {
            ERROR_MSG("Failed to create comments: %s", strerror(errno));
            comment_repository_destroy(repository);
            free(repository);
            repository = NULL;
        }
    }    
    return repository;
}

/* Release comment repository resources. */
void comment_repository_destroy(comment_repository_t* repository) {
    repository->size  = 0;
    repository->last  = 0;

    if(repository->comments != NULL) {
        for(int i=0; i<repository->last; i++) {
            if(repository->comments[i].text) {
                free(repository->comments[i].text);
            }
        }
        free(repository->comments);
        repository->comments = NULL;
    }
}

/* Add comment to repository. */
int comment_repository_add(comment_repository_t* repository, uint16_t logical, uint8_t page, const char *text) {
    int ret = 1;
    int index = comment_repository_index(repository, logical, page);
    if(index >= 0) {
        WARNING_MSG("Duplicate comment for logical address $%04x in page $%02x", logical, page);
    } else {
        /* Expand arrays if necessary */
        if(repository->last >= repository->size) {
            comment_t *ptr;
            repository->size += COMMENT_ARRAY_INC;                
            ptr = (comment_t*)realloc(repository->comments, repository->size * sizeof(comment_t));
            if(ptr == NULL) {
                comment_repository_destroy(repository);
                ret = 0;
            } else {
                repository->comments = ptr;
            }
        }
        if(ret != 0) {
            /* Push addresses & text */
            repository->comments[repository->last].logical = logical;
            repository->comments[repository->last].page    = page;
            repository->comments[repository->last].text    = strdup(text);

            ++repository->last;
        }
    }
    return ret;
}

/* Find a comment by its address. */
int comment_repository_find(comment_repository_t* repository, uint16_t logical, uint8_t page, comment_t *out) {
    int index = comment_repository_index(repository, logical, page);
    if(index < 0) {	
        memset(out, 0, sizeof(comment_t));
        return 0;
    }
    memcpy(out, &repository->comments[index], sizeof(comment_t));
    return 1;
}

/* Get the number of comments stored in the repository. */
int comment_repository_size(comment_repository_t* repository) {
    return repository ? (int)repository->last : 0;
}

/* Retrieve the comment at the specified index. */
int comment_repository_get(comment_repository_t* repository, int index, comment_t *out)  {
    if((repository != NULL) && ((index >= 0) && (index < (int)repository->last))) {    
        memcpy(out, &repository->comments[index], sizeof(comment_t));
        return 1;
    } else {
        memset(out, 0, sizeof(comment_t));
        return 0;
    } 
}

/* Delete comments. */
int comment_repository_delete(comment_repository_t* repository, uint16_t first, uint16_t end, uint8_t page)  {
    size_t i;        
    for(i=0; i<repository->last; i++) {
        if( (repository->comments[i].page == page) &&
            (repository->comments[i].logical >= first) && 
            (repository->comments[i].logical < end) ) {
            if(repository->last) {
                repository->last--;
                if(repository->comments[i].text) {
                    free(repository->comments[i].text);
                }
                memcpy(&repository->comments[i], &repository->comments[repository->last], sizeof(comment_t));
                i--;
            }
        }
    }
    return 1;}

/* Load comments from file. */
int comment_repository_load(const char* filename, comment_repository_t* repository) {
    json_t* root;
    json_error_t err;
    json_t* value;
    int ret = 0, index = 0;
    root = json_load_file(filename, 0, &err);
    if(!root) {
        ERROR_MSG("Failed to parse %s: %s", filename, err.text);
    } else {
        if(!json_is_array(root)) {
            ERROR_MSG("Array expected.");
        } else for (index = 0, ret = 1; ret && (index < json_array_size(root)) && (value = json_array_get(root, index)); index++) {
            ret = 0;
            if(!json_is_object(value)) {
                ERROR_MSG("Expected object.");
            } else {
                int num;
                // logical
                json_t *tmp = json_object_get(value, "logical");
                if(!json_validate_int(tmp, &num)) {
                    ERROR_MSG("Invalid or missing logical address.");
                } else if((num < 0) || (num > 0xffff)) {
                    ERROR_MSG("Logical address out of range.");
                } else {
                    uint16_t logical = (uint16_t)num;
                    // page
                    tmp = json_object_get(value, "page");
                    if(!json_validate_int(tmp, &num)) {
                        ERROR_MSG("Invalid or missing page.");
                    } else {
                        // text (same format as section/label description)
                        const char* text = json_load_description(value, "text");
                        if((num < 0) || (num > 0xff)) {
                            ERROR_MSG("Page value out of range.");
                        } else if(comment_repository_add(repository, logical, (uint8_t)num, text)) {
                            ret = 1;
                        }
                    }
                }
            }
        }
        json_decref(root);
    }
    return ret;
}
