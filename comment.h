/*
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
*/
#ifndef ETRIPATOR_COMMENT_H
#define ETRIPATOR_COMMENT_H

#include "config.h"

/**
 * Comment.
 */
typedef struct {
    uint16_t logical;   /**< Logical address */
    uint8_t  page;      /**< Memory page */
    char*    text;      /**< Comment text */
} comment_t;

typedef struct comment_repository_impl comment_repository_t;

/**
 * Create comment repository.
 * \return A pointer to a comment repository or NULL if an error occured.
 */
comment_repository_t* comment_repository_create();

/**
 * Release comment repository resources.
 * \param [in,out] repository Comment repository.
 */
void comment_repository_destroy(comment_repository_t* repository);

/**
 * Add comment to repository.
 * \param [in,out] repository  Comment repository.
 * \param [in]     logical     Logical address.
 * \param [in]     page        Memory page.
 * \param [in]     text        Comment text.
 */
int comment_repository_add(comment_repository_t* repository, uint16_t logical, uint8_t page, const char *text);

/**
 * Find a comment by its address.
 * \param [in]  repository  Comment repository.
 * \param [in]  logical     Logical address.
 * \param [in]  page        Memory page.
 * \param [out] out         Associated comment (if any).
 * \return 1 if a comment was found, 0 otherwise.
 */
int comment_repository_find(comment_repository_t* repository, uint16_t logical, uint8_t page, comment_t *out);

/**
 * Get the number of comments stored in the repository.
 * \param [in] repository Comment repository.
 * \return Comment count.
 */
int comment_repository_size(comment_repository_t* repository);

/**
 * Retrieve the comment at the specified index.
 * \param [in]  repository Comment repository.
 * \param [in]  index      Comment index.
 * \param [out] out        Comment (if any).
 * \return 1 if a comment exists for the specified index, 0 otherwise.
 */
int comment_repository_get(comment_repository_t* repository, int index, comment_t *out);

/**
 * Delete comments.
 * \param [in]  repository  Comment repository.
 * \param [in]  first       Start of the logical address range.
 * \param [in]  end         End of the logical address range.
 * \param [in]  page        Memory page.
 */
int comment_repository_delete(comment_repository_t* repository, uint16_t first, uint16_t end, uint8_t page);

/**
 * Load comments from file.
 * \param [in]  filename Input filename.
 * \param [out] repository Comment repository.
 * \return 1 if the comments contained in the file was succesfully added to the repository.
 *         0 if an error occured.
 */
int comment_repository_load(const char* filename, comment_repository_t* repository);

#endif // ETRIPATOR_COMMENT_H