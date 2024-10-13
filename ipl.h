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
#ifndef IPL_H
#define IPL_H

#include "config.h"
#include "section.h"

/// IPL Information block data format
typedef struct {
    /// IPLBLK - load start record no. of CD (3 bytes high/medium/low) 
    ///          this is where the program is stored.
    uint8_t load_start_record[3];
    /// IPLBLN - load block length of CD (1 byte)
    ///          number of sectors to read.
    uint8_t load_sector_count;
    /// IPLSTA - program load address (2 bytes low/high)
    ///          main memory address for program read.
    uint8_t load_store_address[2];
    /// IPLJMP - program execute address (2 bytes low/high)
    ///          starting address of execution after program read.
    uint8_t load_exec_address[2];
    /// IPLMPR - ipl set mpr2-6 (5 bytes)
    uint8_t mpr[5];    
    /// OPENMODE - opening mode
    ///     bit 0: data read to vram (0: not read, 1: read)
    ///     bit 1: data read to adpcm buffer (0: not read, 1: read)
    ///     bit 5: bg display (0: display on, 1: display off)
    ///     bit 6: adpcm play (0: play, 1: not play)
    ///     bit 7: adpcm play mode (0: single, 1: repeat)
	  uint8_t opening_mode;
    /// GRPBLK - opening graphic data record no. (3 bytes high/medium/low)
    uint8_t opening_gfx_record[3];
    /// GRPBLN - opening graphic data length (1 byte)
    uint8_t opening_gfx_sector_count;
    /// GRPADR - opening graphic data read address (2 bytes low/high)
    uint8_t opening_gfx_read_address[2];
    /// ADPBLK - opening ADPCM data record no. (3 bytes high/medium/low)
    uint8_t opening_adpcm_record[3];
    /// ADPBLN - opening ADPCM data length (1 byte)
    uint8_t opening_adpcm_sector_count;
    /// ADPRATE - opening ADPCM sampling rate (1 byte)
    uint8_t opening_adpcm_sampling_rate;
    /// RESERVED (7 bytes)
    uint8_t reserved[7];
    /// ID STR - "PC Engine CD-ROM SYSTEM", 0
    uint8_t id[24];
    /// LEGAL - "Copyright HUDSON SOFT / NEC Home Electronics, Ltd.", 0
    uint8_t legal[50];
    /// PROGRAM NAME - program name (16 bytes)
    uint8_t program_name[16];
    /// EXTRA - (6 bytes)
    uint8_t extra[6];
} IPL;
/* [todo]
 IPL graphic block
 color palette - 1 record
 BAT data      - 1 record
 BG font data  - GRPBLN - 2 records
 */

/// Display IPL infos.
/// \param [in] in IPL infos.
void ipl_print(IPL *in);

/// Read IPL data from file.
/// \param [out] out IPL infos.
/// \param [in] filename Input filename.
/// \return 0 on error, 1 otherwise.
bool ipl_read(IPL *out, const char *filename);

/// Get irq code offsets from IPL.
/// \param [in]  in IPL infos.
/// \param [out] section Sections.
/// \param [out] count  Section count.
/// \return 0 on error, 1 otherwise.
bool ipl_sections(IPL *in, Section **out, int *count);

#endif // IPL_H
