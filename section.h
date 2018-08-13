/*
    This file is part of Etripator,
    copyright (c) 2009--2017 Vincent Cruz.

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
 * Section type
 */
typedef enum {
    UnknownSectionType = -1,
    BinData = 0, /**< The section will be extracted from the input ROM and
                      stored in a separate file. **/
    IncData, /**< The section will be output as a series of .db directives. **/
    Code,
    SectionTypeCount
} SectionType;

/**
 * Data configuration.
 */
typedef struct {
    int32_t element_size;      /**< element size (string=0, byte=1, word=2). **/
    int32_t elements_per_line; /**< number of elements per line. **/
} DataConfig;

/**
 * Define a ROM area.
 * This area can contain data or code.
 */
typedef struct {
    SectionType type;/**< type **/
    char *name;      /**< name **/
    uint8_t bank;    /**< rom bank / memory page **/
    uint16_t org;    /**< org **/
    uint32_t offset; /**< start address (file offset in bytes) **/
    int32_t size;    /**< section size (in bytes) **/
    char *filename;  /**< output filename **/
    uint8_t mpr[8];  /**< mpr registers **/
    DataConfig data; /**< data configuration **/
} Section;
/**
 * Reset a section to its default values.
 */
void resetSection(Section *out);
/**
 * Group section per output filename and sort them in bank/org order.
 * \param [in][out] sections Sections.
 * \param [in] count Number of sections to sort.
 */
void sortSections(Section *sections, int count);

#endif // _SECTION_H_
