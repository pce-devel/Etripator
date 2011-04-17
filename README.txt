Etripator : pc-engine ROM disassembler
by Vincent "MooZ" Cruz

* Infos
-----------------

Usage : etripator [options] <cfg> <in>
--irq-detect or -i : automatically detect and extract irq vectors.
--cd or -c         : cdrom image disassembly. Irq detection and rom
                     header jump are not performed.
--help or -h       : displays this message.
--out or -o <file> : main asm file containing includes for all sections
                     as long the irq vector table if the irq-detect
                     option is enabled.
--csv              : the <cfg> configuration file is a csv file.
                     WARNING, csv files are now replaced by cfg files.
                     This option may be removed in future release.

! IMPORTANT
-----------------

You don't have to add the header size to file offsets. It will be
automatically detected and added to your file offset.

* Configuration file format :
----------------------------------
The csv configuration file is now replaced by a CFG/INI file. 
Each section starts with the name of the output file between square
brackets. The supported fields are :
	* type (mandatory) : values are code ,bin_data or inc_data.
	                     'code' is self explanatory.
	                     'inc_data' means that the section is pure data.
	                     but it will be output as code directives (.db)
	                     'bin_data' is also for data section. But the
	                     output file will contain binary data.
	* bank (mandatory) : bank of the current section. It will be used to
	                     compute the file offset of the section if the
	                     'offset' field is missing.
	* org  (mandatory) : program counter location. Just like 'bank', it
	                     will be used to compute file offset if there's
	                     no 'offset' field.
	* offset           : file offset. This field is useful for CD-ROM
	                     disassembly.
	* size             : section size. For code section, a negative (or
	                     missing size) means that the disassembly will
	                     stop when a RTS or RTI instruction is found.
	                     This field is mandatory for data sections.                     

There must be only one occurence of each field per section.

* Example :
-----------------

First you create a file called  dda.cfg :
[sample_aa]
type=bin_data
bank=1
org=6000
size=2000

[sample_ab]
type=bin_data
bank=2
org=6000
size=2000

[sample_ac]
type=bin_data
bank=3
org=6000
size=1dab

Then you call the disassembler with the following arguments :
etripator -i -o dda.asm dda.cfg dda.pce" 

It will output:
irq_2.asm found at e0a4 (a4)
irq_1.asm found at e0a3 (a3)
irq_timer.asm found at e057 (57)
irq_nmi.asm found at e0a2 (a2)
irq_reset.asm found at e000 (0)

irq_2.asm:

irq_1.asm:

irq_timer.asm:
e08c short jump to e08e
e094 short jump to e09e
e09a short jump to e09e

irq_nmi.asm:

irq_reset.asm:
e023 long jump to e023
e056 short jump to e050

As we enabled irq detection, etripator will output the org and file
offsets (without the header size for hucards)  of the found irq vector
codes. For each found section, the found jump addresses will be output
(as long as the address where the jump instruction is). A short jump is
a local conditional jump (bra, bne, beq, etc...). 
Long jumps are "jmp" "jsr" and "bsr". Shorts jumps allways occured in
the same mpr as the current code. Note that it may not be the case for
long jumps. It may even jump to some address in ram.

You'll end up with 9 files :
	sample_aa
	sample_ab
	sample_ac
	irq_1.asm
	irq_2.asm
	irq_timer.asm
	irq_reset.asm
	dda.asm

"sample*" will contain the binary data found at the offsets given in the
cfgfile.
"irq*.asm" contains the extracted code for the various pcengine irq
vectors. And finally "dda.asm" will include all these files and the irq
vector table.

        .include "irq_1.asm"
        .include "irq_2.asm"
        .include "irq_timer.asm"
        .include "irq_reset.asm"

        .data
        .bank 1
        .org  $6000 
        .incbin "sample_aa"

        .bank 2
        .org  $6000 
        .incbin "sample_ab"

        .bank 3
        .org  $6000 
        .incbin "sample_ac"
        
        .bank 0        
        .org $FFF6
        .dw $e0a4
        .dw $e0a3
        .dw $e057
        .dw $e0a2

Recompile it with pceas and voila! :p

! OBSOLETE CONFIGURATION USING CSV FILES
-----------------------------------------

file.csv contains the description of rom sections to dump or 
disassemble.
There are 3 kind of sections: code ,bin_data and inc_data. For each
section, you specify the base offset (org)  and its size.
For code section, a negative size means that the disassembly will stop
when a RTS or RTI instruction is found. The file syntax is as follow :

type;name;bank;org;size

Each section will be dumped into a file called "name".
"type" is either code or data.
"bank" is the bank of the section.
"org" is the org the section.
"size" is the size of the section.

Note that the csv file can be empty.

Whatever section you specify, the interrupt vectors will be disassembled
and dumped into files: irq_1, irq_2, irq_nmi, irq_timer and irq_reset.

data sections are dumped into raw binary format (bin_data) or in asm
code (inc_data).
