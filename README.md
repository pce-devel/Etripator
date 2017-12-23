# Etripator : a PC Engine disassembler

[![Travis build Status](https://travis-ci.org/BlockoS/Etripator.svg)](https://travis-ci.org/BlockoS/Etripator)

[![Coverity Scan Build Status](https://scan.coverity.com/projects/6483/badge.svg)](https://scan.coverity.com/projects/blockos-etripator)

## Examples
* [System Card bank #0](examples/syscard)
* [Gfx unpacking](examples/maerchen_maze) in Maerchen Maze.
* [Memory Base 128 detection](examples/tadaima) in Taidama Yusha Boshuchu.

## Usage
```
etripator [options] <cfg> <in>
```
The options are :
* **--irq-detect** or **-i** : automatically detect and extract irq vectors when disassembling a ROM, or extract opening code and gfx from CDROM IPL data.
* **--cd** or **-c** : cdrom image disassembly. Irq detection and rom header jump are not performed.
* **--help** or **-h** : displays help.
* **--out** or **-o < file >** : main asm file containing includes for all sections as long the irq vector table if the irq-detect  option is enabled.
* **--labels** or **-l < file >** : labels definition filename.
* **--labels-out <file>** : extracted labels output filename. Otherwise the labels will be written to <in>.YYMMDDhhmmss.lbl.\n"
* **cfg** :  configuration file. It is optional if irq detection is enabled.
* **in** : binary to be disassembled (ROM or CDROM track).

## Configuration file format

The configuration file is a standard **JSON** file.
The supported fields are :
 * **filename** *(mandatory)* : name of the output file. If multiple sections have the same output filename, their output will be appended to that file. On the other hand, note that the output file will be overwritten at each session.

 * **type** *(mandatory)* : values are **code**, **bin_data** or **inc_data**.
    * **code** indicates that the section will be disassembled and output as asm code.
    * **inc_data** means that the section is pure data. but it will be output as code directives (.db).
    * **bin_data** is also for data section. But the output file will contain binary data.

 * **bank** *(mandatory)* : bank of the current section. It will be used to compute the file offset of the section if the **offset** field is missing.


 * **org**  *(mandatory)* : program counter location. Just like **bank**', it will be used to compute file offset if there's  no **offset** field.


 * **offset**  : input file offset. This field is *mandatory* for CD-ROM disassembly.


 * **size** : section size. For code section, a zero (or missing size) means that the disassembly will stop when a RTS or RTI instruction is found. This field is *mandatory* for data sections. and CD-ROM disassembly.


 * **id** : section id. If this option is set, all the label will be postfixed with this value. Otherwise the section index will be used.

 * **mpr** : an array containing the page value for each memory page register.
 
There must be only one occurence of each field per section.

Example:
```json
{
    "cdbios_functions": {
        "filename": "syscard.asm",
        "type": "code",
        "bank": "0",
        "org" : "e000",
        "size": "505",
        "mpr": ["ff", "f8", 0, 0, 0, 0, 0, 0 ]
    },
    "ex_colorcmd.impl": {
        "filename": "syscard.asm",
        "type": "code",
        "bank": "0",
        "org" : "e509",
        "size": "ce",
        "mpr": ["ff", "f8", 0, 0, 0, 0, 0, 0 ]
    }
}
```

## Labels definition file format

The labels definition file is a standard **JSON** file.
Each section starts with the name of the label between square brackets.
The supported fields are :
 * **logical** : Logical address of the label in hexadecimal.
 * **page** : physical page associated to the address, i.e. the value of the mpr.

Example:
```json
{
    "cd_reset": {"logical":"e22a", "page":"00"},
    
	"irq_2": {"logical":"eaa3", "page":"00"},
	"irq_1": {"logical":"eba5", "page":"00"},
	"irq_timer": {"logical":"ea9c", "page":"00"},
	"irq_nmi": {"logical":"ea9b", "page":"00"},
	"irq_reset": {"logical":"eab3", "page":"00"},

	"main": {"logical":"f8a4", "page":"00"}
}
```

## Build
**Etripator** uses **CMake** as its build system.
Theorically you can build **Etripator** for any platform supported by **CMake**.
Here are the build step:
### Configuration
```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```
### Build
```
cmake --build . --config Release
```

## Authors

Vincent "MooZ" Cruz
