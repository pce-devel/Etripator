/*
 * This file is part of Etripator,
 * copyright (c) 2009--2023 Vincent Cruz.
 *
 * Etripator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Etripator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Etripator.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ETRIPATOR_LABEL_H
#define ETRIPATOR_LABEL_H

#include "config.h"

/**
 * Label.
 */
typedef struct {
    char*    name;        /**< Offset in the repository name buffer */
    uint16_t logical;     /**< Logical address */
    uint8_t  page;        /**< Memory page  */
    char*    description; /**< Description (optional) */
} label_t;

typedef struct label_repository_impl label_repository_t;

/**
 * Create label repository.
 * \return A pointer to a label repository or NULL if an error occured.
 */
label_repository_t* label_repository_create();

/**
 * Release label repository resources.
 * \param [in,out] repository Label repository.
 */
void label_repository_destroy(label_repository_t* repository);

/**
 * Add label (or inline description) to repository.
 * \param [in,out] repository Label repository.
 * \param [in]     name        Name. If the name is NULL, then this label is an inline description.
 * \param [in]     logical     Logical address.
 * \param [in]     page        Memory page.
 * \param [in]     description Description (optional if name is set, mandatory otherwise).
 */
int label_repository_add(label_repository_t* repository, const char* name, uint16_t logical, uint8_t page, const char *description);

/**
 * Find a label by its address.
 * \param [in]  repository  Label repository.
 * \param [in]  logical     Logical address.
 * \param [in]  page        Memory page.
 * \param [out] out         Associated label (if any).
 * \return 1 if a label was found, 0 otherwise.
 */
int label_repository_find(label_repository_t* repository, uint16_t logical, uint8_t page, label_t *out);

/**
 * Get the number of labels stored in the repository.
 * \param [in] repository Label repository.
 * \return Label count.
 */
int label_repository_size(label_repository_t* repository);

/**
 * Retrieve the label at the specified index.
 * \param [in]  repository Label repository.
 * \param [in]  index      Label index.
 * \param [out] out        Label (if any).
 * \return 1 if a label exists for the specified index, 0 otherwise.
 */
int label_repository_get(label_repository_t* repository, int index, label_t *out);

/**
 * Delete labels
 * \param [in]  repository  Label repository.
 * \param [in]  first       Start of the logical address range.
 * \param [in]  end         End of the logical address range.
 * \param [in]  page        Memory page.
 */
int label_repository_delete(label_repository_t* repository, uint16_t first, uint16_t end, uint8_t page);

/**
 * Load labels from file.
 * \param [in]  filename Input filename.
 * \param [out] repository Label repository.
 * \return 1 if the labels contained in the file was succesfully added to the repository.
 *         0 if an error occured.
 */
int label_repository_load(const char* filename, label_repository_t* repository);

/**
 * Save labels to file.
 * \param [in] filename Configuration file.
 * \param [in] reposity Label repository.
 * \return 1 if the labels in the repository were succesfully written to the file.
 *         0 if an error occured.
 */
int label_repository_save(const char* filename, label_repository_t* repository);

#endif // ETRIPATOR_LABEL_H
