/*
    This file is part of Etripator,
    copyright (c) 2009--2018 Vincent Cruz.

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
#ifndef _SECTION_H_
#define _SECTION_H_

#include "config.h"

/**
 * Section type.
 */
typedef enum {
    UnknownSectionType = -1,
    Binary,
    Data,
    Code,
    SectionTypeCount
} SectionType;

/**
 * Data section configuration.
 */
typedef struct {
    int32_t element_size;      /**< element size (string=0, byte=1, word=2). **/
    int32_t elements_per_line; /**< number of elements per line. **/
} DataConfig;

/**
 * Section description.
 */
typedef struct {
    char *name;       /**< name. **/

    SectionType type; /**< type. **/
    
    uint8_t page;     /**< memory page. **/
    uint16_t logical; /**< logical address. **/
    uint32_t offset;  /**< input offset. **/
    int32_t size;     /**< size (in bytes). **/

    uint8_t mpr[8];   /**< mpr registers value. **/
    
    char *output;     /**< output filename. **/
    
    DataConfig data;  /**< data configuration (only valid for *data* sections) **/
} Section;

/**
 * Reset a section to its default values.
 **/
void ResetSection(Section *section);

/**
 * Group section per output filename and sort them in page/logical address order.
 * \param [in][out] sections Sections.
 * \param [in] count Number of sections to sort.
 */
void SortSections(Section *sections, size_t count);

/**
 * Load sections from a JSON file.
 * \param [in]  filename Input filename.
 * \param [out] sections Loaded sections.
 * \param [out] count Number of loaded sections. 
 * \return 1 if the sections contained in the file were succesfully loaded.
 *         0 if an error occured.
 */
int LoadSections(const char *filename, Section **sections, int *count);

/**
 * Save sections to a JSON file.
 * \param [in] filename Output filename.
 * \param [in] sections Sections to be saved.
 * \param [in] count Number of sections. 
 * \return 1 if the sections were succesfully saved.
 *         0 if an error occured.
 */
int SaveSections(const char *filename, Section *sections, int count);
 
/**
 * Delete sections.
 */
void DeleteSections(Section *sections, int count);

#endif // _SECTION_H_
