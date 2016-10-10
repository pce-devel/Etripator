/*
    This file is part of Etripator,
    copyright (c) 2009--2016 Vincent Cruz.

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
#ifndef IPL_H
#define IPL_H

/*
 Hu7CD page 5
 IPL Information block data format

 IPLBLK   load start record no. of CD (3 bytes high/medium/low) 
          - where the program is stored
 IPLBLN   load block length of CD     (1 byte)
          - number of sectors to read
 IPLSTA   program load address        (2 bytes low/high)
          - main memory address for program read
 IPLJMP   program execute address     (2 bytes low/high)
          - startin address of execution after program read
 IPLMPR2  ipl set mpr2                (1 byte)
 IPLMPR3  ipl set mpr3                (1 byte)
 IPLMPR4  ipl set mpr4                (1 byte)
 IPLMPR5  ipl set mpr5                (1 byte)
 IPLMPR5  ipl set mpr6                (1 byte)
 OPENMODE opening mode
            bit 0: data read to vram (0: not read, 1: read)
            bit 1: data read to adpcm buffer (0: not read, 1: read)
            bit 5: bg display (0: display on, 1: display off)
            bit 6: adpcm play (0: play, 1: not play)
            bit 7: adpcm play mode (0: single, 1: repeat)
 GRPBLK   opening graphic data record no.   (3 bytes high/medium/low)
 GRPBLN   opening graphic data length       (1 byte)
 GRPADR   opening graphic data read address (2 bytes low/high)
 ADPBLK   opening ADPCM data record no. (3 bytes high/medium/low)
 ADPBLN   opening ADPCM data length     (1 byte)
 ADPRATE  opening ADPCM sampling rate   (1 byte)
 RESERVED (7 bytes)
 ID STR   "PC Engine CD-ROM SYSTEM", 0
 LEGAL    "Copyright HUDSON SOFT / NEC Home Electronics, Ltd.", 0
 PROGRAM NAME (16 bytes)
 EXTRA (6 bytes) 
 */
/*
 IPL graphic block
 color palette - 1 record
 BAT data      - 1 record
 BG font data  - GRPBLN - 2 records
 */
#endif // IPL_H
