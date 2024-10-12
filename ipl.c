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
  copyright (c) 2009--2024 Vincent Cruz.
 
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
#include "ipl.h"
#include "message.h"

#define IPL_HEADER_SIZE 0x80
#define IPL_DATA_SIZE 0xB2

static int ipl_read_header(ipl_t *out, FILE *in, const char *filename) {
    int ret = 0;
    if(fread(out->load_start_record, 1, 3, in) != 3) {
        ERROR_MSG("Failed to read IPLBLK from %s: %s", filename, strerror(errno));
    } else if(fread(&out->load_sector_count, 1, 1, in) != 1) {
        ERROR_MSG("Failed to read IPLBLN from %s: %s", filename, strerror(errno));
    } else if(fread(out->load_store_address, 1, 2, in) != 2) {
        ERROR_MSG("Failed to read IPLSTA from %s: %s", filename, strerror(errno));
    } else if(fread(out->load_exec_address, 1, 2, in) != 2) {
        ERROR_MSG("Failed to read IPLJMP from %s: %s", filename, strerror(errno));
    } else if(fread(out->mpr, 1, 5, in) != 5) {
        ERROR_MSG("Failed to read IPLMPR from %s: %s", filename, strerror(errno));
    } else if(fread(&out->opening_mode, 1, 1, in) != 1) {
        ERROR_MSG("Failed to read OPENMODE from %s: %s", filename, strerror(errno));
    } else if(fread(out->opening_gfx_record, 1, 3, in) != 3) {
        ERROR_MSG("Failed to read GRPBLK from %s: %s", filename, strerror(errno));
    } else if(fread(&out->opening_gfx_sector_count, 1, 1, in) != 1) {
        ERROR_MSG("Failed to read GRPBLN from %s: %s", filename, strerror(errno));
    } else if(fread(out->opening_gfx_read_address, 1, 2, in) != 2) {
        ERROR_MSG("Failed to read GRPADR from %s: %s", filename, strerror(errno));
    } else if(fread(out->opening_adpcm_record, 1, 3, in) != 3) {
        ERROR_MSG("Failed to read ADPBLK from %s: %s", filename, strerror(errno));
    } else if(fread(&out->opening_adpcm_sector_count, 1, 1, in) != 1) {
        ERROR_MSG("Failed to read ADPBLN from %s: %s", filename, strerror(errno));
    } else if(fread(&out->opening_adpcm_sampling_rate, 1, 1, in) != 1) {
        ERROR_MSG("Failed to read ADPRATE from %s: %s", filename, strerror(errno));
    } else if(fread(out->reserved, 1, 7, in) != 7) {
        ERROR_MSG("Failed to read RESERVED from %s: %s", filename, strerror(errno));
    } else if(fread(out->id, 1, 24, in) != 24) {
        ERROR_MSG("Failed to read ID STR from %s: %s", filename, strerror(errno));
    } else if(fread(out->legal, 1, 50, in) != 50) {
        ERROR_MSG("Failed to read LEGAL from %s: %s", filename, strerror(errno));
    } else if(fread(out->program_name, 1, 16, in) != 16) {
        ERROR_MSG("Failed to read PROGRAM NAME from %s: %s", filename, strerror(errno));
    } else if(fread(out->extra, 1, 6, in) != 6) {
        ERROR_MSG("Failed to read EXTRA from %s: %s", filename, strerror(errno));
    } else {
        ret = 1;
    }
    return ret;
}

void ipl_print(ipl_t *in) {
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

    FILE *stream = fopen("ipl.inc", "wb");
    if(stream == NULL) {
        ERROR_MSG("failed to open ipl.inc: %s", strerror(errno));
    } else {
        fprintf(stream,  "IPLBLK: .db $%02x, $%02x, $%02x\n", in->load_start_record[0], in->load_start_record[1], in->load_start_record[2]); 
        fprintf(stream, "IPLBKN: .db $%02x\n", in->load_sector_count);
        fprintf(stream, "IPLSTA: .dw $%02x%02x\n", in->load_store_address[0], in->load_store_address[1]);
        fprintf(stream, "IPLJMP: .dw $%02x%02x\n", in->load_exec_address[0], in->load_exec_address[1]);
        fprintf(stream, "IPLMPR: .db $%02x, $%02x, $%02x, $%02x, $%02x\n", in->mpr[0], in->mpr[1], in->mpr[2], in->mpr[3], in->mpr[4]);
        fprintf(stream, "OPENMODE: .db $%02x\n", in->opening_mode);
        fprintf(stream, "GRPBLK: .db $%02x, $%02x, $%02x\n", in->opening_gfx_record[0], in->opening_gfx_record[1], in->opening_gfx_record[2]); 
        fprintf(stream, "GRPBLN: .db $%02x\n", in->opening_gfx_sector_count);
        fprintf(stream, "GRPADR: .dw $%02x%02x\n", in->opening_gfx_read_address[0], in->opening_gfx_read_address[1]);
        fprintf(stream, "ADPBLK: .db $%02x, $%02x, $%02x\n", in->opening_adpcm_record[0], in->opening_adpcm_record[1], in->opening_adpcm_record[2]);
        fprintf(stream, "ADPBLN: .db %02x\n", in->opening_adpcm_sector_count);
        fprintf(stream, "ADPRATE: .db $%02x\b", in->opening_adpcm_sampling_rate);
        fprintf(stream, "RESERVED: .db $%02x, $%02x, $%02x, $%02x, $%02x, $%02x, $%02x\n", 
            in->reserved[0], in->reserved[1], in->reserved[2], in->reserved[3],
            in->reserved[4], in->reserved[5], in->reserved[6]);
        fprintf(stream, "ID STR: .db \"%.24s\"\n", in->id);
        fprintf(stream, "LEGAL: .db \"%.50s\"\n", in->legal);
        fprintf(stream,"PROGRAM NAME: .db \"%.16s\"\n", in->program_name);
        fprintf(stream,"EXTRA: .db \"%.6s\"\n", in->extra);
        fclose(stream);
    }
}

/* Read IPL data from file. */
int ipl_read(ipl_t *out, const char *filename) {
    FILE *in = fopen(filename, "rb");
    int ret = 0;
    if(in == NULL) {
        ERROR_MSG("Failed to open %s: %s", filename, strerror(errno));
    } else {
        if(fseek(in, 0x800, SEEK_SET)) {
            ERROR_MSG("Failed to seek to IPL header in %s: %s", filename, strerror(errno));
        } else if(ipl_read_header(out, in, filename)) {
            ipl_print(out);
            ret = 1;
        }
        fclose(in);
    }
    return ret;
}

/* Get irq code offsets from IPL. */
int ipl_sections(ipl_t *in, section_t **out, int *count) {
    int i, j, k, extra;
    section_t *section;
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
    section = (section_t*)realloc(*out, (j+extra) * sizeof(section_t));
    if(NULL == section) {
        ERROR_MSG("Failed to add extra sections.");
        return 0;
    }
    *count += extra;
    *out = section;

    memset(&section[j], 0, extra * sizeof(section_t));

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
        section[j].name    = strdup(section_name[0]);
        section[j].type    = Code;
        section[j].page    = section[j].mpr[in->load_exec_address[1]>>5];
        section[j].logical = (in->load_exec_address[1] << 8) | in->load_exec_address[0];
        section[j].offset  = record * 2048;
        section[j].size    = in->load_sector_count * 2048;
        section[j].output  = strdup(section_filename[0]);
        j++;
    }
    // "GFX"
    if(in->opening_gfx_sector_count) {
        record = (in->opening_gfx_record[0] << 16) | (in->opening_gfx_record[1] << 8) | in->opening_gfx_record[2];
        section[j].name    = strdup(section_name[1]);
        section[j].type    = Data;
        section[j].page    = section[j].mpr[in->opening_gfx_read_address[1]>>5];
        section[j].logical = (in->opening_gfx_read_address[1] << 8) | in->opening_gfx_read_address[0];
        section[j].offset  = record * 2048;
        section[j].size    = in->opening_gfx_sector_count * 2048;
        section[j].output  = strdup(section_filename[1]);
        section[j].data.type = Binary;
        section[j].data.element_size = 1;
        section[j].data.elements_per_line = 16;
    }
    return 1;
}
