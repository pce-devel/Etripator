# Etripator : a PC Engine disassembler

[![Travis build Status](https://travis-ci.org/BlockoS/Etripator.svg)](https://travis-ci.org/BlockoS/Etripator) [![AppVeyor Build status](https://ci.appveyor.com/api/projects/status/github/BlockoS/etripator?svg=true)](https://ci.appveyor.com/project/BlockoS/etripator/branch/master) [![Coverity Scan Build Status](https://scan.coverity.com/projects/6483/badge.svg)](https://scan.coverity.com/projects/blockos-etripator)

## Examples
* [Monster Pro Wrestling tutorial](examples/monster_puroresu).
* [System Card bank #0](examples/syscard).
* [Gfx unpacking](examples/maerchen_maze) in Maerchen Maze.
* [Memory Base 128 detection](examples/tadaima) in Taidama Yusha Boshuchu.
* [Joypad routine](examples/sf2) of Street Fighter II' Champion Edition.
* [Games Express CD Card bank #0](examples/games_express).

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

 * **type** *(mandatory)* : values are **code** or **data**.
    * **code** indicates that the section will be disassembled and output as asm code.
    * **data** means that the section contains data, but it will be output as code directives (.db or .dw) or binary according to the **data** configuration.

 * **page** *(mandatory)* : memory page of the current section. It will be used to compute the file offset of the section if the **offset** field is missing.


 * **logical**  *(mandatory)* : logical address. Just like **page**', it will be used to compute file offset if there's  no **offset** field.


 * **offset** : input file offset. This field is *mandatory* for CD-ROM disassembly.


 * **size** : section size. For code section, a zero (or missing size) means that the disassembly will stop when a RTS or RTI instruction is found. This field is *mandatory* for data sections. and CD-ROM disassembly.


 * **id** : section id. If this option is set, all the label will be postfixed with this value. Otherwise the section index will be used.

 * **mpr** : an array containing the page value for each memory page register.
 
 * **data** : an object with 2 entries :
     * **type** : **binary**, **hex** or **string**.
     * **element_size** *(default value: 1)* : element size in bytes. The only supported values are 1 or 2.
     * **elements_per_line** *(default value: 16)* : number of elements per line. 
  
There must be only one occurence of each field per section.

Example:
```json
{
    "cdbios_functions": {
        "filename": "syscard.asm",
        "type": "code",
        "page": "0",
        "logical" : "e000",
        "size": "505",
        "mpr": ["ff", "f8", 0, 0, 0, 0, 0, 0 ]
    },
    "ex_colorcmd.impl": {
        "filename": "syscard.asm",
        "type": "code",
        "page": "0",
        "logical" : "e509",
        "size": "ce",
        "mpr": ["ff", "f8", 0, 0, 0, 0, 0, 0 ]
    },
    "irq_vectors": {
        "filename": "syscard.asm",
        "type": "data",
        "page": "0",
        "logical": "fff6",
        "size": "a",
        "mpr": ["ff", "f8", 0, 0, 0, 0, 0, 0 ],
        "data": { "type": "hex", "element_size": 2, "elements_per_line": 1 }
    }

}
```

## Labels definition file format

The labels definition file is a standard **JSON** file containing an array of labels.
Each entry is an object containing the following fields:
 * **name**: label name.
 * **logical** : logical address of the label in hexadecimal.
 * **page** : physical page, i.e. the value of the mpr of the logical address.

Example:
```json
[
	{ "name":"cd_reset", "logical":"e22a", "page":"00"},
	{ "name":"irq_2", "logical":"eaa3", "page":"00"},
	{ "name":"irq_1", "logical":"eba5", "page":"00"},
	{ "name":"irq_timer", "logical":"ea9c", "page":"00"},
	{ "name":"irq_nmi", "logical":"ea9b", "page":"00"},
	{ "name":"irq_reset", "logical":"eab3", "page":"00"},
	{ "name":"main", "logical":"f8a4", "page":"00"}
]
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
