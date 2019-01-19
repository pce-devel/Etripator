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
#ifndef ETRIPATOR_SECTION_H
#define ETRIPATOR_SECTION_H

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
} section_type_t;

/**
 * Data section configuration.
 */
typedef struct {
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
 
/**
 * Delete sections.
 */
void section_delete(section_t *ptr, int n);

#endif // ETRIPATOR_SECTION_H
