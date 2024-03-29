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
#ifndef ETRIPATOR_DECODE_H
#define ETRIPATOR_DECODE_H

#include "config.h"
#include "label.h"
#include "section.h"
#include "memorymap.h"
#include "comment.h"

/**
 * Finds any jump address from the current section.
 * @param [in] section Current section.
 * @param [in] map Memory map.
 * @param [in out] repository Label repository.
 * @return 1 upon success, 0 otherwise.
 */
int label_extract(section_t *section, memmap_t *map, label_repository_t *repository);

/**
 * Process data section. The result will be output has a binary file or an asm file containing hex values or strings.
 * @param [out] out File output.
 * @param [in] section Current section.
 * @param [in] map Memory map.
 * @param [in] repository Label repository.
 * @param [in] comments Comments repository.
 * @param [in] extra_infos Display extra informations as comments (if none set).
 * @return 1 upon success, 0 otherwise.
 */
int data_extract(FILE *out, section_t *section, memmap_t *map, label_repository_t *repository, comment_repository_t *comments, int extra_infos);

/**
 * Process code section.
 * @param [out] out File output.
 * @param [in out] logical Current logical address.
 * @param [in] section Current section.
 * @param [in] map Memory map.
 * @param [in] repository Label repository.
 * @param [in] comments Comments repository.
 * @param [in] extra_infos Display extra informations as comments (if none set).
 * @return 1 if rts, rti or brk instruction was decoded, 0 otherwise.
 */
int decode(FILE *out, uint16_t *logical, section_t *section, memmap_t *map, label_repository_t *repository, comment_repository_t *comments, int extra_infos);

/**
 * Computes section size.
 * @param [in] sections Section array.
 * @param [in] index Index of the current section.
 * @param [in] count Number of sections.
 * @param [in] map Memory map.
 * @return Section size. 
 */
int32_t compute_size(section_t *sections, int index, int count, memmap_t *map);

/**
 * Output hardware IO port and RAM labels.
 * @param [out] out File output.
 * @param [in] map Memory map.
 * @param [in] repository Label repository.
 */
void label_dump(FILE *out, memmap_t *map, label_repository_t *repository);

#endif // ETRIPATOR_DECODE_H
