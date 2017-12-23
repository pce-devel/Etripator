# Memory base 128 detection routine

## Command
```bash
./etripator --cd -l labels.json mb128.json Track02.iso
```

## Console
```
mb128_detect
782a long jump to 7770 (82) 
782e long jump to 7754 (82) 
783c long jump to 7754 (82) 
7848 short jump to 784f (82)
784d short jump to 785c (82)
7852 short jump to 7858 (82)
7854 short jump to 7828 (82)
7856 short jump to 785c (82)
785e long jump to 7754 (82)
```

## Output
```
	.code
	.bank 82
	.org $7826
mb128_detect:
          phx     
          clx     
mb128_detect.loop:
          lda     #$a8
          jsr     mb128_write_byte
          cla     
          jsr     mb128_write_bit
          lda     $1000
          asl     A
          asl     A
          asl     A
          asl     A
          sta     <$fe
          lda     #$01
          jsr     mb128_write_bit
          lda     $1000
          and     #$0f
          ora     <$fe
          cmp     #$04
          bne     mb128_detect.retry
          cla     
          plx     
          rts     
          bra     mb128_detect.bit0
mb128_detect.retry:
          inx     
          cpx     #$03
          bcs     mb128_detect.failed
          bra     mb128_detect.loop
          bra     mb128_detect.bit0
mb128_detect.failed:
          lda     #$ff
          plx     
          rts     
mb128_detect.bit0:
          pha     
          cla     
          jsr     mb128_write_bit
          pla     
          rts     
```
