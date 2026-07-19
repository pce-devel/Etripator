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
  copyright (c) 2009--2026 Vincent Cruz.
 
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
#include <etripator/string.h>

struct CommentImpl {
    uint16_t logical; //< Logical address.
    uint8_t page;     //< Memory page.
    String text;      //< Comment text.
};

#include <etripator/comment.h>
#include <etripator/message.h>
#include <etripator/utils.h>

#define COMMENT_ARRAY_INC (16U)

/// Get comment index by its address.
/// \param [in]  repository  Coment repository.
/// \param [in]  logical     Logical address.
/// \param [in]  page        Memory page.
/// \return comment index.
/// \return -1 if the label was not found.
static int comment_repository_index(CommentRepository* repository, uint16_t logical, uint8_t page) {
    size_t i;
    CommentImpl *comments = repository->comments;
    for(i=0; i<repository->last; i++) {
        if( (comments[i].page == page) &&
            (comments[i].logical == logical) ) {
            return (int)i;
        }
    }
    return -1;
}

// Create comment repository.
bool comment_repository_create(CommentRepository *repository) {
    SANITY_CHECK(repository != NULL, false);
    repository->last  = 0;
    repository->comments = NULL;

    repository->size = COMMENT_ARRAY_INC;
    repository->comments = (CommentImpl*)calloc(repository->size, sizeof(CommentImpl));
    if(repository->comments == NULL) {
        ERROR_MSG("Failed to create comments: %s", strerror(errno));
        comment_repository_destroy(repository);
        return false;
    }
    return true;
}

// Release comment repository resources.
void comment_repository_destroy(CommentRepository* repository) {
    SANITY_CHECK(repository != NULL);
    
    repository->size  = 0;
    repository->last  = 0;

    if(repository->comments != NULL) {
        CommentImpl *comments = repository->comments;
        for(size_t i=0; i<repository->last; i++) {
            string_release(&comments[i].text);
        }
        free(repository->comments);
        repository->comments = NULL;
    }
}

// Add comment to repository.
bool comment_repository_add(CommentRepository* repository, uint16_t logical, uint8_t page, StringView text) {
    SANITY_CHECK(repository != NULL, false);
    SANITY_CHECK(string_view_empty(text) == false, false);

    int index = comment_repository_index(repository, logical, page);
    if(index >= 0) {
        WARNING_MSG("Duplicate comment for logical address $%04x in page $%02x", logical, page);
        return false;
    }
    // Expand arrays if necessary.
    if(repository->last >= repository->size) {
        size_t new_size = repository->size + COMMENT_ARRAY_INC;                
        void *tmp = (CommentImpl*)realloc(repository->comments, new_size * sizeof(CommentImpl));
        if(tmp == NULL) {
            ERROR_MSG("Failed to expand comment buffer: %s", strerror(errno));
            return false;
        } 
        repository->comments = tmp;
        repository->size = new_size;
    }
    CommentImpl *comments = repository->comments
    ;
    // Push addresses & text.
    bool ret = string_from_view(&comments[repository->last].text, text);
    // [todo] handle ret == false 
    comments[repository->last].logical = logical;
    comments[repository->last].page    = page;
    ++repository->last;
    return ret;
}

// Find a comment by its address.
bool comment_repository_find(CommentRepository* repository, uint16_t logical, uint8_t page, Comment *out) {
    SANITY_CHECK(repository != NULL, false);
    SANITY_CHECK(out != NULL, false);

    bool ret = false;
    int index = comment_repository_index(repository, logical, page);
    if(index >= 0) {
        CommentImpl *comments = repository->comments;
        out->text = string_get_view(&comments[index].text);
        out->logical = comments[index].logical;
        out->page = comments[index].page;
        ret = true;
    } else {
        memset(out, 0, sizeof(Comment));
    }
    return ret;
}

// Update comment text
bool comment_repository_update(CommentRepository* repository, uint16_t logical, uint8_t page, StringView text) {
    SANITY_CHECK(repository != NULL, false);

    int index = comment_repository_index(repository, logical, page);
    if(index < 0) {
        ERROR_MSG("No comment found at logical address: %04x page: %02x", logical, page);
        return false;
    }
    if(!string_view_empty(text)) {
        return string_from_view(&repository->comments[index].text, text);
    }
    repository->last--;
    string_release(&repository->comments[index].text);
    if((size_t)index != repository->last) {
        memcpy(&repository->comments[index], &repository->comments[repository->last], sizeof(CommentImpl));
    }
    return true;
}

// Get the number of comments stored in the repository.
int comment_repository_size(CommentRepository* repository) {
    assert(repository != NULL);
    return (int)repository->last;
}

// Retrieve the comment at the specified index.
// [todo] same as find... do some copy-left return stuff?
bool comment_repository_get(CommentRepository* repository, int index, Comment *out)  {
    assert(repository != NULL);
    assert(out != NULL);

    bool ret = false;
    if((index >= 0) && (index < (int)repository->last)) {   
        CommentImpl *comments = repository->comments;
        out->text = string_get_view(&comments[index].text);
        out->logical = comments[index].logical;
        out->page = comments[index].page;
        ret = true;
    } else {
        memset(out, 0, sizeof(Comment));
    }
    return ret;
}

// Delete comments.
void comment_repository_delete(CommentRepository* repository, uint16_t first, uint16_t end, uint8_t page)  {
    assert((repository != NULL) && (repository->comments != NULL));

    CommentImpl *comments = repository->comments;
    for(size_t i=0; i<repository->last; i++) {
        if( (comments[i].page == page) &&
            (comments[i].logical >= first) && 
            (comments[i].logical < end) &&
            (repository->last) ) {
            repository->last--;
            string_release(&comments[i].text);
            if(i != repository->last) {
                memcpy(&comments[i], &comments[repository->last], sizeof(CommentImpl));
            }
            i--;
        }
    }
}
