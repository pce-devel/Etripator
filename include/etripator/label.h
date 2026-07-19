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
#ifndef ETRIPATOR_LABEL_H
#define ETRIPATOR_LABEL_H

#include "config.h"
#include "string_view.h"
#include "data.h"

/// Label.
typedef struct {
    StringView  name;       //< Name
    uint16_t logical;       //< Logical address
    uint8_t  page;          //< Memory page
    StringView description; //< Description (optional) 
    uint32_t file_id;       //< Id of the file in the output registry where the label was written
    size_t line;            //< Current line.
    size_t column;          //< Cursor position in the current line.
} Label;

typedef struct LabelImpl LabelImpl;

/// Label repository.
typedef struct {
    size_t     size;   //< Size of label repository.
    size_t     last;   //< Last element in the repository.
    LabelImpl* labels; //< Labels.
} LabelRepository;

/// Create label repository.
/// \param [in out] repository Label repository.
/// \return true if the repository was succesfully initialized
/// \return false if an error occured
bool label_repository_create(LabelRepository* repository);

/// Release label repository resources.
/// \param [in,out] repository Label repository.
void label_repository_destroy(LabelRepository* repository);

/// Add label (or inline description) to repository.
/// \param [in,out] repository Label repository.
/// \param [in]     name        Name. If the name is empty, then this label is an inline description.
/// \param [in]     logical     Logical address.
/// \param [in]     page        Memory page.
/// \param [in]     description Description (optional if name is set, mandatory otherwise).
/// \return true if the entry was successfully added to the repository.
/// \return false if an error occured.
bool label_repository_add(LabelRepository* repository, uint16_t logical, uint8_t page, StringView name, StringView description);

/// Update label name.
/// Note that the entry will be removed if the update process will leave it without a name or description.
/// \param [in,out] repository Label repository.
/// \param [in]     logical     Logical address.
/// \param [in]     page        Memory page.
/// \param [in]     name        Name.
/// \return true if the entry was successfully updated.
/// \return false if the entry was not found or if an error occured.
bool label_repository_update_name(LabelRepository* repository, uint16_t logical, uint8_t page, const StringView name);

/// Update label description.
/// Note that the entry will be removed if the update process will leave it without a name or description.
/// \param [in,out] repository Label repository.
/// \param [in]     logical     Logical address.
/// \param [in]     page        Memory page.
/// \param [in]     description Description.
/// \return true if the entry was successfully updated.
/// \return false if the entry was not found or if an error occured.
bool label_repository_update_description(LabelRepository* repository, uint16_t logical, uint8_t page, const StringView description);

/// Find a label by its address.
/// \param [in]  repository  Label repository.
/// \param [in]  logical     Logical address.
/// \param [in]  page        Memory page.
/// \param [out] out         Associated label (if any).
/// \return true if a label was found.
/// \return 0 otherwise.
bool label_repository_find(LabelRepository* repository, uint16_t logical, uint8_t page, Label *out);

/// Get the number of labels stored in the repository.
/// \param [in] repository Label repository.
/// \return Label count.
int label_repository_size(LabelRepository* repository);

/// Retrieve the label at the specified index.
/// \param [in]  repository Label repository.
/// \param [in]  index      Label index.
/// \param [out] out        Label (if any).&
/// \return true if a label exists for the specified index.
/// \return false otherwise.
bool label_repository_get(LabelRepository* repository, int index, Label *out);

/// Delete labels
/// \param [in]  repository  Label repository.
/// \param [in]  first       Start of the logical address range.
/// \param [in]  end         End of the logical address range.
/// \param [in]  page        Memory page.
void label_repository_delete(LabelRepository* repository, uint16_t first, uint16_t end, uint8_t page);

/// Load labels from file.
/// \param [out] repository Label repository.
/// \param [in]  in         Input data.
/// \return true if the labels were succesfully added to the repository.
/// \return false if an error occured.
bool label_repository_load(LabelRepository* repository, Data *in);

// [todo]
/// Save labels to file.
/// \param [in] reposity Label repository.
/// \param [in] out      Output.
/// \return true if the labels in the repository were succesfully written.
/// \return false if an error occured.
bool label_repository_save(LabelRepository* repository, Data *out);

#endif // ETRIPATOR_LABEL_H
