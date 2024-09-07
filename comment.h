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
#ifndef ETRIPATOR_COMMENT_H
#define ETRIPATOR_COMMENT_H

#include "config.h"

/// Comment.
typedef struct {
    uint16_t logical; //< Logical address.
    uint8_t  page;    //< Memory page.
    char*    text;    //< Comment text.
} Comment;

typedef struct CommentRepositoryImpl CommentRepository;

/// Create comment repository.
/// \return A pointer to a comment repository.
/// \return NULL if an error occured.
CommentRepository* comment_repository_create();

/// Release comment repository resources.
/// \param [in,out] repository Comment repository.
void comment_repository_destroy(CommentRepository* repository);

/// Add comment to repository.
/// \param [in,out] repository  Comment repository.
/// \param [in]     logical     Logical address.
/// \param [in]     page        Memory page.
/// \param [in]     text        Comment text.
/// \return true if the comment was successfully added to the repository.
/// \return false if an error occured.
bool comment_repository_add(CommentRepository* repository, uint16_t logical, uint8_t page, const char *text);

/// Find a comment by its address.
/// \param [in]  repository  Comment repository.
/// \param [in]  logical     Logical address.
/// \param [in]  page        Memory page.
/// \param [out] out         Associated comment (if any).
/// \return true if a comment was found.
/// \return 0 otherwise.
bool comment_repository_find(CommentRepository* repository, uint16_t logical, uint8_t page, Comment *out);

/// Get the number of comments stored in the repository.
/// \param [in] repository Comment repository.
/// \return Comment count.
int comment_repository_size(CommentRepository* repository);

/// Retrieve the comment at the specified index.
/// \param [in]  repository Comment repository.
/// \param [in]  index      Comment index.
/// \param [out] out        Comment (if any).
/// \return true if a comment exists for the specified index.
/// \return false otherwise.
bool comment_repository_get(CommentRepository* repository, int index, Comment *out);

/// Delete comments.
/// \param [in]  repository  Comment repository.
/// \param [in]  first       Start of the logical address range.
/// \param [in]  end         End of the logical address range.
/// \param [in]  page        Memory page.
void comment_repository_delete(CommentRepository* repository, uint16_t first, uint16_t end, uint8_t page);

/// Load comments from file.
/// \param [in]  filename Input filename.
/// \param [out] repository Comment repository.
/// \return true if the comments contained in the file was succesfully added to the repository.
/// \return false if an error occured.
bool comment_repository_load(CommentRepository* repository, const char* filename);

#endif // ETRIPATOR_COMMENT_H