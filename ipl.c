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

#include "ipl.h"
#include "message.h"

#define IPL_DATA_SIZE 0xb2

static int readIPLHeader(IPL *out, FILE *in, const char *filename) {
    size_t nRead;
    nRead = fread(out->load_start_record, 1, 3, in);
    if(nRead != 3) {
        ERROR_MSG("Failed to read IPLBLK from %s: %s", filename, strerror(errno));
        return 0;
    }
    nRead = fread(&out->load_sector_count, 1, 1, in);
    if(nRead != 1) {
        ERROR_MSG("Failed to read IPLBLN from %s: %s", filename, strerror(errno));
        return 0;
    }
    nRead = fread(out->load_store_address, 1, 2, in);
    if(nRead != 2) {
        ERROR_MSG("Failed to read IPLSTA from %s: %s", filename, strerror(errno));
        return 0;
    }
    nRead = fread(out->load_exec_address, 1, 2, in);
    if(nRead != 2) {
        ERROR_MSG("Failed to read IPLJMP from %s: %s", filename, strerror(errno));
        return 0;
    }
    nRead = fread(out->mpr, 1, 5, in);
    if(nRead != 5) {
        ERROR_MSG("Failed to read IPLMPR from %s: %s", filename, strerror(errno));
        return 0;
    }
    nRead = fread(&out->opening_mode, 1, 1, in);
    if(nRead != 1) {
        ERROR_MSG("Failed to read OPENMODE from %s: %s", filename, strerror(errno));
        return 0;
    }
    nRead = fread(out->opening_gfx_record, 1, 3, in);
    if(nRead != 3) {
        ERROR_MSG("Failed to read GRPBLK from %s: %s", filename, strerror(errno));
        return 0;
    }
    nRead = fread(&out->opening_gfx_sector_count, 1, 1, in);
    if(nRead != 1) {
        ERROR_MSG("Failed to read GRPBLN from %s: %s", filename, strerror(errno));
        return 0;
    }
    nRead = fread(out->opening_gfx_read_address, 1, 2, in);
    if(nRead != 2) {
        ERROR_MSG("Failed to read GRPADR from %s: %s", filename, strerror(errno));
        return 0;
    }
    nRead = fread(out->opening_adpcm_record, 1, 3, in);
    if(nRead != 3) {
        ERROR_MSG("Failed to read ADPBLK from %s: %s", filename, strerror(errno));
        return 0;
    }
    nRead = fread(&out->opening_adpcm_sector_count, 1, 1, in);
    if(nRead != 1) {
        ERROR_MSG("Failed to read ADPBLN from %s: %s", filename, strerror(errno));
        return 0;
    }
    nRead = fread(&out->opening_adpcm_sampling_rate, 1, 1, in);
    if(nRead != 1) {
        ERROR_MSG("Failed to read ADPRATE from %s: %s", filename, strerror(errno));
        return 0;
    }
    nRead = fread(out->reserved, 1, 7, in);
    if(nRead != 7) {
        ERROR_MSG("Failed to read RESERVED from %s: %s", filename, strerror(errno));
        return 0;
    }
    nRead = fread(out->id, 1, 24, in);
    if(nRead != 24) {
        ERROR_MSG("Failed to read ID STR from %s: %s", filename, strerror(errno));
        return 0;
    }
    nRead = fread(out->legal, 1, 50, in);
    if(nRead != 50) {
        ERROR_MSG("Failed to read LEGAL from %s: %s", filename, strerror(errno));
        return 0;
    }
    nRead = fread(out->program_name, 1, 16, in);
    if(nRead != 16) {
        ERROR_MSG("Failed to read PROGRAM NAME from %s: %s", filename, strerror(errno));
        return 0;
    }
    nRead = fread(out->extra, 1, 6, in);
    if(nRead != 6) {
        ERROR_MSG("Failed to read EXTRA from %s: %s", filename, strerror(errno));
        return 0;
    }
    return 1;
}

void printIPL(IPL *in) {
    INFO_MSG("IPLBLK: hi:%02x mid:%02x lo:%02x", in->load_start_record[0], in->load_start_record[1], in->load_start_record[2]); 
    INFO_MSG("IPLBKN: %02x", in->load_sector_count);
    INFO_MSG("IPLSTA: hi:%02x lo:%02x", in->load_store_address[1], in->load_store_address[0]);
    INFO_MSG("IPLJMP: hi:%02x lo:%02x", in->load_exec_address[1], in->load_exec_address[0]);
    INFO_MSG("IPLMPR: 2:%02x, 3:%02x, 4:%02x, 5:%02x, 6:%02x", in->mpr[0], in->mpr[1], in->mpr[2], in->mpr[3], in->mpr[4]);
    INFO_MSG("OPENMODE: %02x", in->opening_mode);
    INFO_MSG("GRPBLK: hi:%02x mid:%02x lo:%02x", in->opening_gfx_record[0], in->opening_gfx_record[1], in->opening_gfx_record[2]); 
    INFO_MSG("GRPBLN: %02x", in->opening_gfx_sector_count);
    INFO_MSG("GRPADR: hi:%02x lo:%02x", in->opening_gfx_read_address[1], in->opening_gfx_read_address[0]);
    INFO_MSG("ADPBLK: hi:%02x mid:%02x lo:%02x", in->opening_adpcm_record[0], in->opening_adpcm_record[1], in->opening_adpcm_record[2]);
    INFO_MSG("ADPBLN: %02x", in->opening_adpcm_sector_count);
    INFO_MSG("ADPRATE: %02x", in->opening_adpcm_sampling_rate);
    INFO_MSG("RESERVED: %02x%02x%02x%02x%02x%02x%02x", in->reserved[0], in->reserved[1], in->reserved[2], in->reserved[3], in->reserved[4]
                                                     , in->reserved[5], in->reserved[6]);
    INFO_MSG("ID STR: %.24s", in->id);
    INFO_MSG("LEGAL: %.50s", in->legal);
    INFO_MSG("PROGRAM NAME: %.16s", in->program_name);
    INFO_MSG("EXTRA: %.6s", in->extra); 
}

/**
 * Read IPL data from file.
 * \param [out] out IPL infos.
 * \param [in] filename Input filename.
 * \return 0 on error, 1 otherwise.
 */
int readIPL(IPL *out, const char *filename) {
    FILE *in = fopen(filename, "rb");
    if(NULL == in) {
        ERROR_MSG("Failed to open %s: %s", filename, strerror(errno));
        return 0;
    }

    int ret = 0;
    fseek(in, 0x800, SEEK_SET);
    ret = readIPLHeader(out, in, filename);
    if(ret) {
        printIPL(out);
    }
    fclose(in);
    return ret;
}

/**
 * Get irq code offsets from IPL.
 * \param [in]  in IPL infos.
 * \param [out] section Sections.
 * \param [out] count  Section count.
 * \return 0 on error, 1 otherwise.
 */
int getIPLSections(IPL *in, Section **out, int *count) {
    int i, j, k, extra;
    Section *section;
    static const char *section_name[2] = { "cd_start", "gfx_start" };
    static const char *section_filename[2] = { "cd_start.asm", "gfx_start.bin" };
    uint32_t record;
    
    extra = 0;
    if(in->load_sector_count) { extra++; }
    if(in->opening_gfx_sector_count) { extra++; }
    if(0 == extra) {
        INFO_MSG("No section found from IPL data.");
        return 1;
    }
        
    j = *count;
    section = (Section*)realloc(*out, (j+extra) * sizeof(Section));
    if(NULL == section) {
        ERROR_MSG("Failed to add extra sections.");
        return 0;
    }
    *count += extra;
    *out = section;

    for(k=0; k<extra; k++) {
        section[j+k].mpr[0] = 0xff;
        section[j+k].mpr[1] = 0xf8;
        section[j+k].mpr[7] = 0x00;
        for(i=0; i<5; i++) {
            section[j+k].mpr[2+i] = 0x80 + in->mpr[i];
        }
    }
    // "CD boot"
    if(in->load_sector_count) {
        record = (in->load_start_record[0] << 16) | (in->load_start_record[1] << 8) | in->load_start_record[2];
        section[j].name     = strdup(section_name[0]);
        section[j].type     = Code;
        section[j].bank     = section[j].mpr[in->load_exec_address[1]>>5];
        section[j].org      = (in->load_exec_address[1] << 8) | in->load_exec_address[0];
        section[j].offset   = record * 2048;
        section[j].size     = in->load_sector_count * 2048;
        section[j].filename = strdup(section_filename[0]);
        j++;
    }
    // "GFX"
    if(in->opening_gfx_sector_count) {
        record = (in->opening_gfx_record[0] << 16) | (in->opening_gfx_record[1] << 8) | in->opening_gfx_record[2];
        section[j].name     = strdup(section_name[1]);
        section[j].type     = BinData;
        section[j].bank     = section[j].mpr[in->opening_gfx_read_address[1]>>5];
        section[j].org      = (in->opening_gfx_read_address[1] << 8) | in->opening_gfx_read_address[0];
        section[j].offset   = record * 2048;
        section[j].size     = in->opening_gfx_sector_count * 2048;
        section[j].filename = strdup(section_filename[1]);
        j++;
    }
    return 1;
}