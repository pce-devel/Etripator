/*
    This file is part of Etripator,
    copyright (c) 2009--2022 Vincent Cruz.

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
#ifndef ETRIPATOR_IRQ_H
#define ETRIPATOR_IRQ_H

#include "config.h"
#include "section.h"
#include "memorymap.h"

/**
 * Get irq code offsets from rom.
 * \param [in]  map Memory map.
 * \param [out] section Sections.
 * \param [out] count  Section count.
 * \return 0 on error, 1 otherwise.
 */
int irq_read(memmap_t* map, section_t **section, int *count);

#endif // ETRIPATOR_IRQ_H
