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

/// Comment repository.
struct CommentRepositoryImpl {
    size_t size;       //< Size of comment repository.
    size_t last;       //< Last element in the repository.
    Comment *comments; //< Comments.
};

/// Get comment index by its address.
/// \param [in]  repository  Coment repository.
/// \param [in]  logical     Logical address.
/// \param [in]  page        Memory page.
/// \return comment index.
/// \return -1 if the label was not found.
static int comment_repository_index(CommentRepository* repository, uint16_t logical, uint8_t page) {
    size_t i;
    for(i=0; i<repository->last; i++) {
        if( (repository->comments[i].page == page) &&
            (repository->comments[i].logical == logical) ) {
            return (int)i;
        }
    }
    return -1;
}

// Create comment repository.
CommentRepository* comment_repository_create() {
    CommentRepository *repository;
    repository = (CommentRepository*)malloc(sizeof(CommentRepository));
    if(repository == NULL) {
        ERROR_MSG("Failed to create comment repository: %s", strerror(errno));
    } else {
        repository->last  = 0;
        repository->comments = NULL;

        repository->size = COMMENT_ARRAY_INC;
        repository->comments = (Comment*)malloc(repository->size * sizeof(Comment));
        if(repository->comments == NULL) {
            ERROR_MSG("Failed to create comments: %s", strerror(errno));
            comment_repository_destroy(repository);
            free(repository);
            repository = NULL;
        }
    }    
    return repository;
}

// Release comment repository resources.
void comment_repository_destroy(CommentRepository* repository) {
    assert(repository != NULL);
    
    repository->size  = 0;
    repository->last  = 0;

    if(repository->comments != NULL) {
        for(size_t i=0; i<repository->last; i++) {
            free(repository->comments[i].text);
        }
        free(repository->comments);
        repository->comments = NULL;
    }
}

// Add comment to repository.
bool comment_repository_add(CommentRepository* repository, uint16_t logical, uint8_t page, const char *text) {
    assert(repository != NULL);
    assert(text != NULL);

    bool ret = true;
    int index = comment_repository_index(repository, logical, page);
    if(index >= 0) {
        WARNING_MSG("Duplicate comment for logical address $%04x in page $%02x", logical, page);
    } else {
        // Expand arrays if necessary.
        if(repository->last >= repository->size) {
            Comment *ptr;
            size_t new_size = repository->size + COMMENT_ARRAY_INC;                
            ptr = (Comment*)realloc(repository->comments, new_size * sizeof(Comment));
            if(ptr == NULL) {
                ERROR_MSG("Failed to expand comment buffer: %s", strerror(errno));
                comment_repository_destroy(repository);
                ret = false;
            } else {
                repository->comments = ptr;
                repository->size = new_size;
            }
        }
        if(ret) {
            // Push addresses & text.
            repository->comments[repository->last].logical = logical;
            repository->comments[repository->last].page    = page;
            repository->comments[repository->last].text    = strdup(text);

            ++repository->last;
        }
    }
    return ret;
}

// Find a comment by its address.
bool comment_repository_find(CommentRepository* repository, uint16_t logical, uint8_t page, Comment *out) {
    assert(repository != NULL);
    assert(out != NULL);

    int index = comment_repository_index(repository, logical, page);
    bool ret = (index >= 0);
    if(ret) {	
        memcpy(out, &repository->comments[index], sizeof(Comment));
    } else {
        memset(out, 0, sizeof(Comment));
    }
    return ret;
}

// Get the number of comments stored in the repository.
int comment_repository_size(CommentRepository* repository) {
    assert(repository != NULL);
    return (int)repository->last;
}

// Retrieve the comment at the specified index.
bool comment_repository_get(CommentRepository* repository, int index, Comment *out)  {
    assert(repository != NULL);
    assert(out != NULL);

    bool ret = false;
    if((index >= 0) && (index < (int)repository->last)) {    
        memcpy(out, &repository->comments[index], sizeof(Comment));
        ret = true;
    } else {
        memset(out, 0, sizeof(Comment));
    }
    return ret;
}

// Delete comments.
void comment_repository_delete(CommentRepository* repository, uint16_t first, uint16_t end, uint8_t page)  {
    assert(repository != NULL);

    for(size_t i=0; i<repository->last; i++) {
        if( (repository->comments[i].page == page) &&
            (repository->comments[i].logical >= first) && 
            (repository->comments[i].logical < end) ) {
            if(repository->last) {
                repository->last--;
                if(repository->comments[i].text) {
                    free(repository->comments[i].text);
                }
                memcpy(&repository->comments[i], &repository->comments[repository->last], sizeof(Comment));
                i--;
            }
        }
    }
}

#if 0
// [todo] move to comment/load
// Load comments from file.
bool comment_repository_load(CommentRepository* repository, const char* filename) {
    assert(filename != NULL);
    assert(repository != NULL);

    bool ret = false;

    json_t* root;
    json_error_t err;
    json_t* value;
    int index = 0;
    root = json_load_file(filename, 0, &err);
    if(!root) {
        ERROR_MSG("Failed to parse %s:%d:%d: %s", filename, err.line, err.column, err.text);
    } else {
        if(!json_is_array(root)) {
            ERROR_MSG("Array expected.");
        } else for (index = 0, ret = true; ret && (index < json_array_size(root)) && (value = json_array_get(root, index)); index++) {
            ret = false;
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
                        char* text = json_load_description(value, "text");
                        if(text == NULL) {
                            ERROR_MSG("Invalid or missing text.");
                        } else if((num < 0) || (num > 0xff)) {
                            ERROR_MSG("Page value out of range.");
                        } else if(comment_repository_add(repository, logical, (uint8_t)num, text)) {
                            ret = true;
                        }
                        free(text);
                    }
                }
            }
        }
        json_decref(root);
    }
    return ret;
}
#endif