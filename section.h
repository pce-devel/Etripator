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
#ifndef ETRIPATOR_SECTION_H
#define ETRIPATOR_SECTION_H

#include "config.h"

#if 0

/// Section types.
typedef enum {
    /// Unknown or invalid section type.
    UnknownSectionType = -1,
    /// Data type.
    /// This a "meta" section type. A data section contains either
    ///   * Raw binary data
    ///   * Strings
    ///   * Addresses
    /// @see DataType for a more detailed description.
    DataSectionType = 0,
    /// The section contains executable code. @see CodeSection
    CodeSectionType,
    /// Number of supported section types.
    SectionTypeCount
} SectionType;

/// Data section types.
typedef enum {
    /// Raw binary data.
    RawDataType = 0,
    /// String data.
    StringDataType,
    /// Address list (usually jump table)
    JumpTableType,
    /// Number of supported data section types.
    DataTypeCount
} DataType,

/// Raw binary data
typedef struct {
    /// Must be equal to RawDataType.
    DataType type;
    ///
} RawData;

typedef struct {
    DataType type;
    // [todo]
} StringData;

typedef struct {
    DataType type;
} JumpTable;

typedef struct {
    SectionType type;
    union {
        RawDataSection raw;
        StringDataSection string;
        JumpTableDataSection jump_table;
    };
} DataSection;

typedef struct {
    SectionType type;
    // [todo]
} CodeSection;

/*
typedef struct {
    SectionType type;
    MemoryAddress
    {
        uint16_t page;
        uint16_t logical;
    } address;
    
    MemoryConfiguration  ???;
    
    size_t size;
} SectionBase;

*/



typedef union {
    SectionType type;
    CodeSection code;
    DataSection data;
} Section;

#endif

/**
 * Section type.
 */
typedef enum {
    UnknownSectionType = -1,
    Data,
    Code,
    SectionTypeCount
} section_type_t;

/**
 * Retrieves section type name.
 */
const char* section_type_name(section_type_t type);

/**
 * Retrieves data type name.
 */
typedef enum {
    UnknownDataType = -1,
    Binary,
    Hex,
    String,
    JumpTable,
    DataTypeCount
} data_type_t;

/**
 * Retrieves data type name.
 */
const char* data_type_name(data_type_t type);

/**
 * Data section configuration.
 */
typedef struct {
    data_type_t type;          /**< type. **/
    int32_t element_size;      /**< element size (string=0, byte=1, word=2). **/
    int32_t elements_per_line; /**< number of elements per line. **/
} data_config_t;

/**
 * Section description.
 */
typedef struct {
    char *name;          /**< name. **/
    section_type_t type; /**< type. **/
    uint8_t page;        /**< memory page. **/
    uint16_t logical;    /**< logical address. **/
    uint32_t offset;     /**< input offset. **/
    int32_t size;        /**< size (in bytes). **/
    uint8_t mpr[8];      /**< mpr registers value. **/ 
    char *output;        /**< output filename. **/ 
    data_config_t data;  /**< data configuration (only valid for *data* sections) **/
    char *description;   /**< optional description. **/
} section_t;

/**
 * Reset a section to its default values.
 **/
void section_reset(section_t *s);

/**
 * Group section per output filename and sort them in page/logical address order.
 * \param [in][out] ptr Sections.
 * \param [in] n Number of sections to sort.
 */
void section_sort(section_t *ptr, size_t n);

/**
 * Delete sections.
 */
void section_delete(section_t *ptr, int n);

/**
 * Load sections from a JSON file.
 * \param [in]  filename Input filename.
 * \param [out] out Loaded sections.
 * \param [out] n Number of loaded sections. 
 * \return 1 if the sections contained in the file were succesfully loaded.
 *         0 if an error occured.
 */
int section_load(const char *filename, section_t **out, int *n);

/**
 * Save sections to a JSON file.
 * \param [in] filename Output filename.
 * \param [in] ptr Sections to be saved.
 * \param [in] count Number of sections. 
 * \return 1 if the sections were succesfully saved.
 *         0 if an error occured.
 */
int section_save(const char *filename, section_t *ptr, int n);

#endif // ETRIPATOR_SECTION_H
