Etripator : pc-engine ROM disassembler
by Vincent "MooZ" Cruz

* Infos
-----------------

Usage : etripator [options] <csv> <in>\
--irq-detect or -i	: automatically detect and extract irq vectors
--cd or -c		: cdrom image disassembly, irq detection is not
                          performed
--help or -h		: displays this message
--out or -o <file>      : main asm file containing includes for all sections
			  as long the irq vector table if the irq-detect option
			  is enabled

file.csv contains the description of rom sections to dump or disassemble.
There are 3 kind of sections: code ,bin_data and inc_data. For each section,
you specify the base offset (org)  and its size.
For code section, a negative size means that the disassembly will stop when a
RTS or RTI instruction is found. The file syntax is as follow :

type;name;bank;org;size

Each section will be dumped into a file called "name".
"type" is either code or data.
"bank" is the bank of the section.
"org" is the org the section.
"size" is the size of the section.

Note that the csv file can be empty.

Whatever section you specify, the interrupt vectors will be disassembled and
dumped into files: irq_1, irq_2, irq_nmi, irq_timer and irq_reset.

data sections are dumped into raw binary format (bin_data) or in asm code (inc_data).

! IMPORTANT
-----------------

You don't have to add the header size to file offsets. It will be automatically
detected and added to your file offset.

* Example :
-----------------

First you create a file called  dda.csv :
bin_data;sample_aa;1;6000;2000
bin_data;sample_ab;2;6000;2000
bin_data;sample_ac;3;6000;1dab

Then you call the disassembler with the following arguments :
etripator -i -o dda.asm dda.csv dda.pce" 

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

As we enabled irq detection, etripator will output the org and file offsets
(without the header size for hucards)  of the found irq vector codes. For each
found section, the found jump addresses will be output (as long as the address
where the jump instruction is). A short jump is a local conditional jump
(bra, bne, beq, etc...). Long jumps are "jmp" "jsr" and "bsr". Shorts jumps
allways occured in the same mpr as the current code. Note that it may not be
the case for long jumps. It may even jump to some address in ram.

You'll end up with 9 files :
	sample_aa
	sample_ab
	sample_ac
	irq_1.asm
	irq_2.asm
	irq_timer.asm
	irq_reset.asm
	dda.asm

"sample*" will contain the binary data found at the offsets given in the csv
file.
"irq*.asm" contains the extracted code for the various pcengine irq vectors.
And finally "dda.asm" will include all these files and the irq vector table.

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

