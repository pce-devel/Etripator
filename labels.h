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
#ifndef ETRIPATOR_LABELS_H
#define ETRIPATOR_LABELS_H

#include "config.h"

typedef struct LabelRepositoryImpl LabelRepository;

/**
 * Create label repository.
 * \return A pointer to a label repository or NULL if an error occured.
 */
LabelRepository* createLabelRepository();

/**
 * Release label repository resources.
 * \param [in,out] repository Label repository.
 */
void deleteLabelRepository(LabelRepository* repository);

/**
 * Add label to repository.
 * \param [in,out] repository Label repository.
 * \param [in]     name       Name.
 * \param [in]     logical    Logical address.
 * \param [in]     page       Memory page.
 */
int addLabel(LabelRepository* repository, const char* name, uint16_t logical, uint8_t page);

/**
 * Find a label by its address.
 * \param [in]  repository  Label repository.
 * \param [in]  logical     Logical address.
 * \param [in]  page        Memory page.
 * \param [out] name        Label name (if found).
 * \return 1 if a label was found, 0 otherwise.
 */
int findLabel(LabelRepository* repository, uint16_t logical, uint8_t page, char** name);

/**
 * Get the number of labels stored in the repository.
 * \param [in] repository Label repository.
 * \return Label count.
 */
int getLabelCount(LabelRepository* repository);

/**
 * Retrieve the label at the specified index.
 * \param [in] repository Label repository.
 * \param [in] index      Label index.
 * \param [out] logical   Logical address.
 * \param [out]  page     Memory page.
 * \param [out] name      Label name.
 * \return 1 if a label exists for the specified index, 0 otherwise.
 */
int getLabel(LabelRepository* repository, int index, uint16_t* logical, uint8_t* page, char** name);
/**
 * Delete labels
 * \param [in]  repository  Label repository.
 * \param [in]  first       Start of the logical address range.
 * \param [in]  end         End of the logical address range.
 * \param [in]  page        Memory page.
 */
int deleteLabels(LabelRepository* repository, uint16_t first, uint16_t end, uint8_t page);

#endif // ETRIPATOR_LABELS_H
