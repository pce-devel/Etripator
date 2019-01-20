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
#ifndef ETRIPATOR_SECTION_LOAD_H
#define ETRIPATOR_SECTION_LOAD_H

#include "../section.h"

/**
 * Load sections from a JSON file.
 * \param [in]  filename Input filename.
 * \param [out] out Loaded sections.
 * \param [out] n Number of loaded sections. 
 * \return 1 if the sections contained in the file were succesfully loaded.
 *         0 if an error occured.
 */
int section_load(const char *filename, section_t **out, int *n);

#endif // ETRIPATOR_SECTION_LOAD_H
