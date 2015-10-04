# Memory base 128 detection routine

## Command
```bash
./etripator --cd -l labels.cfg mb128.cfg Track02.iso
```

## Console
```
mb128_detect:
782a long jump to 7770 (00105770) 
782e long jump to 7754 (00105754) 
783c long jump to 7754 (00105754) 
7848 short jump to 784f (0010584f)
784d short jump to 785c (0010585c)
7852 short jump to 7858 (00105858)
7854 short jump to 7828 (00105828)
7856 short jump to 785c (0010585c)
785e long jump to 7754 (00105754)
```

## Output
```
	.code
	.bank 82
	.org $7826
mb128_detect:
          PHX     
          CLX     
mb128_detect.loop:
          LDA     #$a8
          JSR     mb128_write_byte
          CLA     
          JSR     mb128_write_bit
          LDA     $1000
          ASL     A
          ASL     A
          ASL     A
          ASL     A
          STA     <$fe
          LDA     #$01
          JSR     mb128_write_bit
          LDA     $1000
          AND     #$0f
          ORA     <$fe
          CMP     #$04
          BNE     mb128_detect.retry
          CLA     
          PLX     
          RTS     
          BRA     mb128_detect.bit0
mb128_detect.retry:
          INX     
          CPX     #$03
          BCS     mb128_detect.failed
          BRA     mb128_detect.loop
          BRA     mb128_detect.bit0
mb128_detect.failed:
          LDA     #$ff
          PLX     
          RTS     
mb128_detect.bit0:
          PHA     
          CLA     
          JSR     mb128_write_bit
          PLA     
          RTS     
```
