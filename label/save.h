/*
    This file is part of Etripator,
    copyright (c) 2009--2021 Vincent Cruz.

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
#ifndef ETRIPATOR_LABEL_SAVE_H
#define ETRIPATOR_LABEL_SAVE_H

#include "../label.h"

/**
 * Save labels to file.
 * \param [in] filename Configuration file.
 * \param [in] reposity Label repository.
 * \return 1 if the labels in the repository were succesfully written to the file.
 *         0 if an error occured.
 */
int label_repository_save(const char* filename, label_repository_t* repository);

#endif // ETRIPATOR_LABEL_SAVE_H
