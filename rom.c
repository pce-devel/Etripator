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
#include "rom.h"
#include "message.h"

static int rom_load_data(const char *filename, memmap_t *map) {
    int ret = 0;
    size_t size = 0;
    FILE *in;

    /* Open file */
    in = fopen(filename, "rb");
    if (in == NULL) {
        ERROR_MSG("Unable to open %s : %s", filename, strerror(errno));
    } else {
        /* Compute file size. */
        struct stat infos;
        int fd = fileno(in);
        if (fd < 0) {
            ERROR_MSG("Failed to retrieve file descriptior for %s : %s", filename, strerror(errno));
        } else if (fstat(fd, &infos) < 0) {
            ERROR_MSG("Failed to retrieve file informations of %s : %s", filename, strerror(errno));
        } else if (infos.st_size == 0) {
            ERROR_MSG("Empty file: %s", filename);
        } else {
            size = infos.st_size;
            /* Check for possible header */
            if (size & 0x200) {
                /* Jump header */
                size &= ~0x200;
                if (fseek(in, 0x200, SEEK_SET)) {
                    ERROR_MSG("Failed to jump rom header in %s: %s", filename, strerror(errno));
                }
            }
        }
        if(size) {
            /* Allocate rom storage */
            if (!mem_create(&map->mem[PCE_MEM_ROM], (size + 0x1fff) & ~0x1fff)) {
                ERROR_MSG("Failed to allocate ROM storage : %s", strerror(errno));
            } else {
                /* Fill rom with 0xff */
                memset(map->mem[PCE_MEM_ROM].data, 0xff, map->mem[PCE_MEM_ROM].len);
                /* Read ROM data */
                size_t count = (size < map->mem[PCE_MEM_ROM].len) ? size : map->mem[PCE_MEM_ROM].len;
                size_t nread = fread(map->mem[PCE_MEM_ROM].data, 1, count, in);
                if (nread != count) {
                    ERROR_MSG("Failed to read ROM data from %s : %s", filename, strerror(errno));
                } else {
                    ret = 1;
                }
            }
        }
        fclose(in);
    }
    return ret;
}

/* Load ROM from file. */
int rom_load(const char *filename, memmap_t *map) {
    FILE *in;
    int ret = 0;
    if(rom_load_data(filename, map) == 0) {
        mem_destroy(&map->mem[PCE_MEM_ROM]);
    } else {
        unsigned int i;
        /* Initialize ROM pages. */
        if (map->mem[PCE_MEM_ROM].len == 0x60000) {
            for (i = 0; i < 64; i++) {
                map->page[i] = &map->mem[PCE_MEM_ROM].data[(i & 0x1f) * 8192];
            }
            for (i = 64; i < 128; i++) {
                map->page[i] = &map->mem[PCE_MEM_ROM].data[((i & 0x0f) + 32) * 8192];
            }
        } else if (map->mem[PCE_MEM_ROM].len == 0x80000) {
            for (i = 0; i < 64; i++) {
                map->page[i] = &map->mem[PCE_MEM_ROM].data[(i & 0x3f) * 8192];
            }
            for (i = 64; i < 128; i++) {
                map->page[i] = &map->mem[PCE_MEM_ROM].data[((i & 0x1f) + 32) * 8192];
            }
        } else {
            for (i = 0; i < 128; i++) {
                uint8_t bank = (uint8_t)(i % (map->mem[PCE_MEM_ROM].len / 8192));
                map->page[i] = &map->mem[PCE_MEM_ROM].data[bank * 8192];
            }
        }
        ret = 1;
    }
    return ret;
}
