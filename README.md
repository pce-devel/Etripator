![logo](logo.png)

# Etripator : a PC Engine disassembler
![Github action Status](https://github.com/BlockoS/Etripator/actions/workflows/build.yml/badge.svg) [![AppVeyor Build status](https://ci.appveyor.com/api/projects/status/github/BlockoS/etripator?svg=true)](https://ci.appveyor.com/project/BlockoS/etripator/branch/main) [![Coverity Scan Build Status](https://scan.coverity.com/projects/6483/badge.svg)](https://scan.coverity.com/projects/blockos-etripator)

## Examples
* [Monster Pro Wrestling - モンスタープロレス tutorial](examples/monster_puroresu).
* [System Card bank #0](examples/syscard).
* [Gfx unpacking](examples/maerchen_maze) in Maerchen Maze - メルヘンメイズ .
* [Memory Base 128 detection](examples/tadaima) in Taidama Yusha Boshuchu - ただいま勇者募集中.
* [Joypad routine](examples/sf2) of Street Fighter II' Champion Edition - ストリートファイターII'.
* [Games Express CD Card](https://github.com/BlockoS/GamesExpressCDCard).
* [Password check](examples/youkai_douchuuki) in Youkai Douchuuki - 妖怪道中記.

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
* **--labels-out <file>** : extracted labels output filename. Otherwise the labels will be written to `"<in>.YYMMDDhhmmss.lbl"`
* **--comments <file>** : comments description filename.
* **--address** : print statement logical address and page in an inline comment.
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
     * **type** : **binary**, **hex**, **string** or **jumptable**.
     * **element_size** *(default value: 1 or 2 for **jumptable**)* : element size in bytes. The only supported values are 1 or 2.
     * **elements_per_line** *(default value: 16)* : number of elements per line. 

* **description** *(optional)*: description as a string or an array of strings for multiline description. 

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
        "mpr": ["ff", "f8", 0, 0, 0, 0, 0, 0 ],
        "description": "CDROM Bios functions"
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
 * **description** *(optional)* : description (as a string or as an array of strings).

Example:
```json
[
    { "name":"cd_reset", "logical":"e22a", "page":"00", "description": [
        "Reset CD drive",
        "input: none",
        "output: A - $00: OK",
        "            sub error code"
    ]},
    { "name":"irq_2", "logical":"eaa3", "page":"00"},
    { "name":"irq_1", "logical":"eba5", "page":"00"},
    { "name":"irq_timer", "logical":"ea9c", "page":"00"},
    { "name":"irq_nmi", "logical":"ea9b", "page":"00"},
    { "name":"irq_reset", "logical":"eab3", "page":"00"},
    { "name":"main", "logical":"f8a4", "page":"00"}
]
```

## Comments file format

The comments file is very similar to the labels definition file. It is a standard **JSON** file containing an array of comments. Each entry is an object containing the following fields:
 * **logical** : logical address of the statement to comment in hexadecimal.
 * **page** : physical page, i.e. the value of the mpr of the logical address.
 * **text** : comment text (as a string or as an array of strings).

Example:
```json
[
    { "logical": "eab3", "page":"00", "text": " switch CPU to high speed mode"},
    { "logical": "eab4", "page":"00", "text": " disable interrupts"},
    { "logical": "eab5", "page":"00", "text": " clear decimal flag"},
    { "logical": "eab6", "page":"00", "text": " map I/O to the 1st memory page"},
    { "logical": "eaba", "page":"00", "text": " map RAM to the 2nd memory page"},
    { "logical": "eabe", "page":"00", "text": " clear zero page"},
    { "logical": "eac7", "page":"00", "text": " clear bss"},
    { "logical": "ead1", "page":"00", "text": [
            " disable interrupts",
            " this looks like the soft reset entry point"
        ]
    },
    { "logical": "ead2", "page":"00", "text": " disable CPU timer"},
    { "logical": "ead5", "page":"00", "text": " switch CPU to high speed mode"},
    { "logical": "ead6", "page":"00", "text": " reset stack pointer"}
]
```
This comments configuration file comes from the [Games Express CD Card bank #0](examples/games_express) example and produces the following assembly code.
```asm
irq_reset:                              ; bank: $000 logical: $eab3
          csh                           ; switch CPU to high speed mode
          sei                           ; disable interrupts
          cld                           ; clear decimal flag
          lda     #$ff                  ; map I/O to the 1st memory page
          tam     #$00
          lda     #$f8                  ; map RAM to the 2nd memory page
          tam     #$01
          stz     <$00                  ; clear zero page
          tii     $2000, $2001, $00ff
          stz     irq1_jmp              ; clear bss
          tii     irq1_jmp, $2201, $1dff
lead1_00:                               ; bank: $000 logical: $ead1
          sei                           ; disable interrupts
                                        ; this looks like the soft reset entry point
          stz     $0c01                 ; disable CPU timer
          csh                           ; switch CPU to high speed mode
          ldx     #$ff                  ; reset stack pointer
```

## Build
**Etripator** uses **CMake** as its build system.
Theorically you can build **Etripator** for any platform supported by **CMake**.
Here are the build step:
### Configuration
If you cloned the repository, you first need to fetch submodules with 
```
git submodule update --init --recursive
```

You will also need to have [jansson](https://github.com/akheron/jansson) installed on your system.
This can be acheived using a package manager (apt, brew, chocolate, pacman, ...) or by building it from source.

```
wget https://github.com/akheron/jansson/archive/v2.12.zip
unzip v2.12.zip
cd jansson-2.12
mkdir -p build
cmake -Bbuild/cmake -S . \
      -DJANSSON_BUILD_DOCS=OFF \
      -DJANSSON_WITHOUT_TESTS=ON \
      -DCMAKE_INSTALL_PREFIX=build/install 
cmake --build build/cmake --target install
```

Now on to building etripator. 
```
cd etripator
mkdir -p build
cmake -Bbuild/cmake -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=cmake/install
```

If you build jansson as shown earlier you may configure the project with
```
cmake -Bbuild/cmake -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=build/install \
    -DJANSSON_INCLUDE_DIR=../../jansson-2.12/build/install/include \
    -DJANSSON_LIBRARY=../../jansson-2.12/build/install/lib/jansson.lib 
```

### Build

If everything went right, you can now compile the project with:
```
cmake --build build/cmake --target install
```

## Authors

Vincent "MooZ" Cruz
