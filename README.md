# Etripator : a PC Engine disassembler
by Vincent "MooZ" Cruz

## Usage
```
etripator [options] <cfg> <in>
```
The options are :
* **--irq-detect** or **-i** : automatically detect and extract irq vectors.
* **--cd** or **-c** : cdrom image disassembly. Irq detection and rom header jump are not performed.
* **--help** or **-h** : displays help.
* **--out** or **-o <file>** : main asm file containing includes for all sections as long the irq vector table if the irq-detect  option is enabled.
* **cfg** :  configuration file. It is optional if irq detection is enabled. 
* **in** : binary to be disassembled (ROM or CDROM track).

## Configuration file format

The configuration file is a standard **CFG**/**INI** file.

Each section starts with the name of the output file between square
brackets. The supported fields are :
 * **type** *(mandatory)* : values are **code**, **bin_data** or **inc_data**.
	* **code** indicates that the section will be disassembled and output as asm code.
    * **inc_data** means that the section is pure data. but it will be output as code directives (.db).
    * **bin_data** is also for data section. But the output file will contain binary data.
    
 * **bank** *(mandatory)* : bank of the current section. It will be used to compute the file offset of the section if the **offset** field is missing.
 
 
 * **org**  *(mandatory)* : program counter location. Just like **bank**', it will be used to compute file offset if there's  no **offset** field.
 
 
 * **offset**  : input file offset. This field is *mandatory* for CD-ROM disassembly.
 
 
 * **size** : section size. For code section, a zero (or missing size) means that the disassembly will stop when a RTS or RTI instruction is found. This field is *mandatory* for data sections. and CD-ROM disassembly.
 
 
 * **id** : section id. If this option is set, all the label will be postfixed with this value. Otherwise the section index will be used.

There must be only one occurence of each field per section.