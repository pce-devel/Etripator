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
#ifndef ETRIPATOR_SECTION_H
#define ETRIPATOR_SECTION_H

#include "config.h"

/// Section type.
typedef enum {
    SECTION_TYPE_UNKNOWN = -1,
    SECTION_TYPE_DATA = 0,
    SECTION_TYPE_CODE,
    SECTION_TYPE_COUNT
} SectionType;

/// Retrieves section type name
/// \param [in] type Section type
/// \return "data" if type is SECTION_TYPE_DATA
/// \return "code" if type is SECTION_TYPE_CODE
/// \return "unknown" otherwise
const char* section_type_name(SectionType type);

/// Data type
typedef enum {
    DATA_TYPE_UNKNOWN = -1,
    DATA_TYPE_BINARY = 0,
    DATA_TYPE_HEX,
    DATA_TYPE_STRING,
    DATA_TYPE_JUMP_TABLE,
    DATA_TYPE_COUNT
} DataType;

/// Retrieves data type name
/// \param [in] type Data type
/// \return "binary" if type is DATA_TYPE_BINARY
/// \return "hex" if type is DATA_TYPE_HEX
/// \return "string" if type is DATA_TYPE_STRING
/// \return "jumptable if type is DATA_TYPE_JUMP_TABLE
/// \return "unknown" otherwise
const char* data_type_name(DataType type);

/// Data section configuration
typedef struct {
    DataType type;             //< type
    int32_t element_size;      //< element size (string=0, byte=1, word=2)
    int32_t elements_per_line; //< number of elements per line
    
    uint8_t delimiter[8U];  //< string delimiter
    int32_t delimiter_size; //< string delimiter length
} DataConfig;

/// Section description
typedef struct {
    char *name;        //< Name
    SectionType type;  //< Type
    uint8_t page;      //< Memory page
    uint16_t logical;  //< logical address
    uint32_t offset;   //< input offset
    int32_t size;      //< size (in bytes)
    uint8_t mpr[8];    //< mpr registers value
    char *output;      //< output filename
    DataConfig data;   //< data configuration (only valid for *data* sections)
    char *description; //< optional description
} Section;

/// Section array
typedef struct {
    Section *data;      ///< Pointer to section array.
    size_t count;       ///< Number of sections currently in use.
    size_t capacity;    ///< Number the section array can hold.
} SectionArray;

/// Reset a section array.
/// \note The internal data pointer will not be freed.
/// \param [in out] arr Section array to be reseted.
void section_array_reset(SectionArray *arr);

/// Release section array memory.
/// \param [in out] arr Section array.
void section_array_delete(SectionArray *arr);

/// Add a new section.
/// \param [in out] arr Section array the section will be added to.
/// \param [in] in Section that will be added to the section array.
/// \return  1 if the section was succesfully added.
/// \return  0 if the section was merged with one from the section array.
/// \return -1 if the section can not be merged or if there is not enough memory to add a new one.
int section_array_add(SectionArray *arr, const Section* in);

/// Merge and sort sections.
/// \param [in out] arr Section array the section will be added to.
void section_array_tidy(SectionArray *arr);

/// Retrieve the ith section from the array.
/// \param [in] arr Section array.
/// \param [in] i Index of the section to be retrieved.
/// \return A pointer to the section if the index is within the section array bounds.
/// \return NULL if the index is out of the section array bounds.
const Section* section_array_get(SectionArray *arr, size_t i);

/// Reset a section to its default values.
void section_reset(Section *s);

/// Delete section.
void section_delete(Section *ptr);

// Load sections from a JSON file.
// \param [out] arr Loaded sections.
// \param [in]  filename Input filename.
// \return true if the sections contained in the file were succesfully loaded.
// \return false if an error occured.
bool section_load(SectionArray *arr, const char *filename);

// Save sections to a JSON file.
// \param [in] ptr Sections to be saved.
// \param [in] count Number of sections. 
// \param [in] filename Output filename.
// \return true if the sections were succesfully saved.
// \return false if an error occured.
bool section_save(const Section *ptr, int n, const char *filename); // [todo] use SectionArray

#endif // ETRIPATOR_SECTION_H
