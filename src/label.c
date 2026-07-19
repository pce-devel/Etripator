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
#include <assert.h>

#include <etripator/message.h>
#include <etripator/string.h>

#define LABEL_ARRAY_INC (16U)

/// Label.
struct LabelImpl {
    uint16_t logical;     //< Logical address
    uint8_t  page;        //< Memory page
    uint32_t file_id;     //< Id of the file in the output registry where the label was written
    size_t line;          //< Current line.
    size_t column;        //< Cursor position in the current line.
    String name;          //< Label name
    String description;   //< Description (optional) 
};

#include <etripator/label.h>
#include <etripator/utils.h>

static void label_from_impl(Label* out, const LabelImpl* in) {
    out->name = string_get_view(&in->name);
    out->logical = in->logical;
    out->page = in->page;
    out->description = string_get_view(&in->description);
    out->file_id = in->file_id;
    out->line = in->line;
    out->column = in->column;
}

/// Get label index by its address.
/// \param [in]  repository  Label repository.
/// \param [in]  logical     Logical address.
/// \param [in]  page        Memory page.
/// \return label index.
/// \return  -1 if the label was not found.
static int label_repository_index(LabelRepository *repository, uint16_t logical, uint8_t page) {
    for(size_t i=0; i<repository->last; i++) {
        if( (repository->labels[i].page == page) &&
            (repository->labels[i].logical == logical) ) {
            return (int)i;
        }
    }
    return -1;
}

// Create label repository.
bool label_repository_create(LabelRepository* repository) {
    SANITY_CHECK(repository != NULL, false);

    repository->last  = 0;
    repository->labels = NULL;
    repository->size = LABEL_ARRAY_INC;
    repository->labels = calloc(repository->size, sizeof(LabelImpl));
    if(repository->labels == NULL) {
        ERROR_MSG("Failed to create label: %s", strerror(errno));
        label_repository_destroy(repository);
        return false;
    }
    return true;
}

//  Delete label repository.
void label_repository_destroy(LabelRepository* repository) {
    SANITY_CHECK(repository != NULL);

    repository->size  = 0;
    repository->last  = 0;

    if(repository->labels != NULL) {
        for(size_t i=0; i<repository->last; i++) {
            string_release(&repository->labels[i].name);
            string_release(&repository->labels[i].description);
        }
        free(repository->labels);
        repository->labels = NULL;
    }
}

// Add label to repository.
bool label_repository_add(LabelRepository* repository, uint16_t logical, uint8_t page, StringView name, StringView description) {
    SANITY_CHECK(repository != NULL, false);

    if(string_view_empty(name) && string_view_empty(description)) {
        ERROR_MSG("Name and description can't be both empty for logical addres: %04x page: %02x", logical, page);
        return false;
    }
    
    int index = label_repository_index(repository, logical, page);
    if(index >= 0) {
        // [todo] warning + return true ?
        if(string_length(&repository->labels[index].description) == 0) {
            string_from_view(&repository->labels[index].description, description);
        }
        return true;
    }
    /* Expand arrays if necessary */
    if(repository->last >= repository->size) {
        LabelImpl *ptr;
        repository->size += LABEL_ARRAY_INC;                
        ptr = (LabelImpl*)realloc(repository->labels, repository->size * sizeof(LabelImpl));
        if(ptr == NULL) {
            label_repository_destroy(repository);
            return false;
        }
        repository->labels = ptr;
    }
    LabelImpl *labels = &repository->labels[repository->last];
    /* Push addresses */
    labels->logical = logical;
    labels->page    = page;

    /* Push name and description */
    string_from_view(&labels->name, name);
    if(description.length && description.data) {
        string_from_view(&labels->description, description);
    }
    ++repository->last;

    return true;
}

// Find a label by its address.
bool label_repository_find(LabelRepository* repository, uint16_t logical, uint8_t page, Label *out) {
    int index = label_repository_index(repository, logical, page);
    bool ret = (index >= 0);
    if(ret) {
        label_from_impl(out, &repository->labels[index]);
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
        label_from_impl(out, &repository->labels[index]);
    } else {
        memset(out, 0, sizeof(Label));
    }
    return ret;
}

static void label_repository_delete_entry(LabelRepository* repository, size_t index) {
    if(index >= repository->last) {
        return;
    }
    repository->last--;
    string_release(&repository->labels[index].name);
    string_release(&repository->labels[index].description);
    if(index != repository->last) {
        memcpy(&repository->labels[index], &repository->labels[repository->last], sizeof(LabelImpl));
    }
}

// Delete labels.
void label_repository_delete(LabelRepository* repository, uint16_t first, uint16_t end, uint8_t page) {
    size_t i;
    for(i=0; i<repository->last; i++) {
        if( (repository->labels[i].page == page) &&
            (repository->labels[i].logical >= first) && 
            (repository->labels[i].logical < end) ) {
            label_repository_delete_entry(repository, i);
            i--;
        }
    }
}

// Update label name
bool label_repository_update_name(LabelRepository* repository, uint16_t logical, uint8_t page, const StringView name) {
    SANITY_CHECK(repository != NULL, false);

    int index = label_repository_index(repository, logical, page);
    if(index < 0) {
        ERROR_MSG("No label found at logical address: %04x page: %02x", logical, page);
        return false;
    }
    LabelImpl *label = &repository->labels[index];
    if(string_view_empty(name)) {
        if(label->description.length) {
            string_release(&label->name);
        } else {
            label_repository_delete_entry(repository, index);
        }
    } else if(string_copy(&label->name, name.data, name.length) != true) {
        ERROR_MSG("Failed to update label name at logical address: %04x page: %02x", logical, page);
        return false;
    }
    return true;
}

// Update label description
bool label_repository_update_description(LabelRepository* repository, uint16_t logical, uint8_t page, const StringView description) {
    SANITY_CHECK(repository != NULL, false);

    int index = label_repository_index(repository, logical, page);
    if(index < 0) {
        ERROR_MSG("No label found at logical address: %04x page: %02x", logical, page);
        return false;
    }
    LabelImpl *label = &repository->labels[index];
    if(string_view_empty(description)) {
        if(label->name.length) {
            string_release(&label->description);
        } else {
            label_repository_delete_entry(repository, index);            
        }
    } else if(string_copy(&label->description, description.data, description.length) != true) {
        ERROR_MSG("Failed to update label description at logical address: %04x page: %02x", logical, page);
        return false;
    }
    return true;
}
