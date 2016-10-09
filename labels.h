/*
    This file is part of Etripator,
    copyright (c) 2009--2015 Vincent Cruz.

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
#ifndef LABELS_H
#define LABELS_H

#include "config.h"

#define INVALID_PHYSICAL_ADDRESS 0xffffffff

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
 * \param [in]     physical   Physical address.
 */
int addLabel(LabelRepository* repository, const char* name, uint16_t logical, uint32_t physical);

/**
 * Find if a label is associated to the specified physical address.
 * \param [in]  repository  Label repository.
 * \param [in]  physical    Physical address.
 * \param [out] name        Label name (if found).
 * \return 1 if a label was found, 0 otherwise.
 */
int findLabelByPhysicalAddress(LabelRepository* repository, uint32_t physical, char** name);

/**
 * Find if a label is associated to the specified logical address.
 * \param [in]  repository  Label repository.
 * \param [in]  logical     Logical address.
 * \param [out] name        Label name (if found).
 * \return 1 if a label was found, 0 otherwise.
 * \todo How to handle multiple results ?
 */
int findLabelByLogicalAddress(LabelRepository* repository, uint16_t logical, char** name);

/**
 * Get the number of labels stored in the repository.
 * \param [in] repository Label repository.
 * \return Label count.
 */
int getLabelCount(LabelRepository* repository);

/**
 * Retrieve the label at the specified label.
 * \param [in] repository Label repository.
 * \param [in] index      Label index.
 * \param [out] physical  Physical address.
 * \param [out] logical   Logical address.
 * \param [out] name      Label name.
 * \return 1 if a label exists for the specified index, 0 otherwise.
 */
int getLabel(LabelRepository* repository, int index, uint32_t* physical, uint16_t* logical, char** name);

#endif // LABELS_H
