    .include "syscard.inc"
   
	.code
	.bank 0
	.org $e000
cd_boot:
          jmp     irq_reset
cd_reset:
          jmp     cd_reset.impl
cd_base:
          jmp     cd_base.impl
cd_read:
          jmp     cd_read.impl
cd_seek:
          jmp     cd_seek.impl
cd_exec:
          jmp     cd_exec.impl
cd_play:
          jmp     cd_play.impl
cd_search:
          jmp     cd_search.impl
cd_pause:
          jmp     cd_pause.impl
cd_stat:
          jmp     cd_stat.impl
cd_subq:
          jmp     cd_subq.impl
cd_dinfo:
          jmp     cd_dinfo.impl
cd_contnts:
          jmp     cd_contnts.impl
cd_subrd:
          jmp     cd_subrd.impl
cd_pcmrd:
          jmp     cd_pcmrd.impl
cd_fade:
          jmp     cd_fade.impl
ad_reset:
          jmp     ad_reset.impl
ad_trans:
          jmp     ad_trans.impl
ad_read:
          jmp     ad_read.impl
ad_write:
          jmp     ad_write.impl
ad_play:
          jmp     ad_play.impl
ad_cplay:
          jmp     ad_cplay.impl
ad_stop:
          jmp     ad_stop.impl
ad_stat:
          jmp     ad_stat.impl
bm_format:
          jmp     bm_format.impl
bm_free:
          jmp     bm_free.impl
bm_read:
          jmp     bm_read.impl
bm_write:
          jmp     bm_write.impl
bm_delete:
          jmp     bm_delete.impl
bm_files:
          jmp     bm_files.impl
ex_getver:
          jmp     ex_getver.impl
ex_setvec:
          jmp     ex_setvec.impl
ex_getfnt:
          jmp     ex_getfnt.impl
ex_joysns:
          jmp     ex_joysns.impl
ex_joyrep:
          jmp     ex_joyrep.impl
ex_scrsize:
          jmp     ex_scrsize.impl
ex_dotmod:
          jmp     ex_dotmod.impl
ex_scrmod:
          jmp     ex_scrmod.impl
ex_imode:
          jmp     ex_imode.impl
ex_vmode:
          jmp     ex_vmode.impl
ex_hmode:
          jmp     ex_hmode.impl
ex_vsync:
          jmp     ex_vsync.impl
ex_rcron:
          jmp     ex_rcron.impl
ex_rcroff:
          jmp     ex_rcroff.impl
ex_irqon:
          jmp     ex_irqon.impl
ex_irqoff:
          jmp     ex_irqoff.impl
ex_bgon:
          jmp     ex_bgon.impl
ex_bgoff:
          jmp     ex_bgoff.impl
ex_spron:
          jmp     ex_spron.impl
ex_sproff:
          jmp     ex_sproff.impl
ex_dspon:
          jmp     ex_dspon.impl
ex_dspoff:
          jmp     ex_dspoff.impl
ex_dmamod:
          jmp     ex_dmamod.impl
ex_sprdma:
          jmp     ex_sprdma.impl
ex_satclr:
          jmp     ex_satclr.impl
ex_sprput:
          jmp     ex_sprput.impl
ex_setrcr:
          jmp     ex_setrcr.impl
ex_setred:
          jmp     ex_setred.impl
ex_setwrt:
          jmp     ex_setwrt.impl
ex_setdma:
          jmp     ex_setdma.impl
ex_binbcd:
          jmp     ex_binbcd.impl
ex_bcdbin:
          jmp     ex_bcdbin.impl
ex_rnd:
          jmp     ex_rnd.impl
ma_mul8u:
          jmp     ma_mul8u.impl
ma_mul8s:
          jmp     ma_mul8s.impl
ma_mul16u:
          jmp     ma_mul16u.impl
ma_div16u:
          jmp     ma_div16u.impl
ma_div16s:
          jmp     ma_div16s.impl
ma_sqrt:
          jmp     ma_sqrt.impl
ma_sin:
          jmp     ma_sin.impl
ma_cos:
          jmp     ma_cos.impl
ma_atni:
          jmp     ma_atni.impl
psg_bios:
          jmp     psg_bios.impl
grp_bios:
          jmp     grp_bios.impl
ex_memopen:
          jmp     ex_memopen.impl
psg_drive:
          jmp     psg_drive.impl
ex_colorcmd:
          jmp     ex_colorcmd.impl
sys_unknown_1:
          jmp     sys_unknown_1.impl
sys_unknown_2:
          jmp     sys_unknown_2.impl
ma_mul16s:
          jmp     ma_mul16s.impl
ma_cbasis:
          jmp     ma_cbasis.impl
irq_reset:
          sei     
          csh     
          cld     
          ldx     #$ff
          txs     
          txa     
          tam     #$00
          lda     #$f8
          tam     #$01
          bsr     init_mmap
          lda     #$01
          tam     #$06
          jsr     $cb0f
le109_00:
          jsr     $c860
          tax     
          beq     le118_00
          bsr     init_mmap
          lda     #$03
          tam     #$06
          jmp     $cf78
le118_00:
          jsr     cd_contnts.impl
          bsr     le14c_00
          bcc     le12c_00
          lda     #$01
          tam     #$06
          jsr     $cb03
          jsr     $cb54
          jmp     le109_00
le12c_00:
          lda     #$04
          bsr     le13d_00
          jmp     $4000
          lda     #$17
          bsr     le13d_00
          jmp     $4000
init_mmap:
          lda     $fff5
le13d_00:
          tam     #$02
          inc     A
          tam     #$03
          inc     A
          tam     #$04
          inc     A
          tam     #$05
          inc     A
          tam     #$06
          rts     
le14c_00:
          lda     tnomin
le14f_00:
          pha     
          sta     <_ah
          lda     #$02
          sta     <_al
          lda     #$56
          sta     <_bl
          lda     #$22
          sta     <_bh
          jsr     cd_dinfo.impl
          pla     
          tst     #$04, $2259
          bne     le176_00
          cmp     $226b
          bcs     le174_00
          sed     
          clc     
          adc     #$01
          cld     
          bra     le14f_00
le174_00:
          clc     
ex_joyrep.impl:
          rts     
le176_00:
          sta     $2269
          jsr     $cc17
          tii     _al, recbase0_h, $0003
          tii     _al, recbase1_h, $0003
          stz     <_cl
          stz     <_ch
          stz     <_dl
          lda     #$00
          sta     <_bl
          lda     #$28
          sta     <_bh
          lda     #$01
          sta     <_dh
          inc     A
          sta     <_al
          stz     <_ah
          jsr     cd_read.impl
          lda     #$1f
          tam     #$06
          lda     #$00
          sta     <zl0
          lda     #$28
          sta     <zh0
          lda     #$e1
          sta     <zl1
          lda     #$da
          sta     <zh1
          lda     #$d5
          sta     <_cl
          lda     #$04
          sta     <_ch
le1c0_00:
          lda     [zl0]
          cmp     [zl1]
          bne     le209_00
          inc     <zl0
          bne     le1cc_00
          inc     <zh0
le1cc_00:
          inc     <zl1
          bne     le1d2_00
          inc     <zh1
le1d2_00:
          lda     <_cl
          bne     le1d8_00
          dec     <_ch
le1d8_00:
          dec     <_cl
          lda     <_cl
          ora     <_ch
          bne     le1c0_00
          lda     #$4a
          sta     <_cl
          lda     #$00
          sta     <_ch
le1e8_00:
          lda     [zl0]
          bne     le209_00
          inc     <zl0
          bne     le1f2_00
          inc     <zh0
le1f2_00:
          inc     <zl1
          bne     le1f8_00
          inc     <zh1
le1f8_00:
          lda     <_cl
          bne     le1fe_00
          dec     <_ch
le1fe_00:
          dec     <_cl
          lda     <_cl
          ora     <_ch
          bne     le1e8_00
          jsr     $2b26
le209_00:
          sec     
          rts     
le20b:
          cmp     #$06
          beq     le263_00
          cmp     #$02
          bne     le263_00
          ldy     #$09
le215_00:
          lda     $224c, Y
          sta     $2291, Y
          dey     
          bpl     le215_00
          jsr     lf053_00
          lda     suberrc
          beq     le263_00
          cmp     #$04
          bne     le22f_00
          sta     notrdyflg
          bra     le263_00
le22f_00:
          cmp     #$0b
          beq     le265_00
          cmp     #$0d
          beq     le265_00
          cmp     #$11
          beq     le263_00
          cmp     #$15
          beq     le263_00
          cmp     #$16
          beq     le263_00
          cmp     #$1c
          beq     le265_00
          cmp     #$1d
          beq     le265_00
          cmp     #$20
          beq     le265_00
          cmp     #$21
          beq     le265_00
          cmp     #$22
          beq     le265_00
          cmp     #$25
          beq     le265_00
          cmp     #$2a
          beq     le265_00
          cmp     #$2c
          beq     le265_00
le263_00:
          clc     
          rts     
le265_00:
          sec     
          rts     
ex_scrsize.impl:
          and     #$07
          asl     A
          asl     A
          asl     A
          asl     A
          tay     
          lda     #$8f
          bra     le277_00
ex_dotmod.impl:
          and     #$8f
          tay     
          lda     #$70
le277_00:
          and     $2242
          sta     $2242
          tya     
          ora     $2242
          sta     $2242
          lda     #$09
          sta     <reg_box
          sta     video_reg_l
          lda     $2242
          sta     video_data_l
          rts     
le292_00:
          cla     
          ldx     #$20
          ldy     #$1e
          bsr     ex_scrmod.impl
          lda     #$01
          sec     
          rts     
ex_scrmod.impl:
          sta     $2244
          and     #$05
          sta     $2244
          stx     $2245
          sty     $2246
          lda     $2246
          cmp     #$0a
          bcc     le292_00
          cmp     #$1f
          bcs     le292_00
          tst     #$01, $2244
          bne     le2fe_00
          lda     $2245
          cmp     #$0a
          bcc     le292_00
          cmp     #$23
          bcs     le292_00
          bsr     le341_00
          lda     #$0a
          sta     <reg_box
          sta     video_reg_l
          st1     #$02
          lda     $2245
          tax     
          dex     
          lsr     A
          eor     #$ff
          clc     
          adc     #$13
          sta     video_data_h
          clc     
          adc     $2245
          eor     #$ff
          tay     
          lda     #$0b
          sta     <reg_box
          sta     video_reg_l
          stx     video_data_l
          tya     
          clc     
          adc     #$27
          sta     video_data_h
          cla     
          jsr     ex_dotmod.impl
          clc     
          rts     
le2fe_00:
          lda     $2245
          cmp     #$10
          bcc     le292_00
          cmp     #$2d
          bcs     le292_00
          bsr     le341_00
          lda     #$0a
          sta     <reg_box
          sta     video_reg_l
          st1     #$03
          lda     $2245
          tax     
          dex     
          lsr     A
          eor     #$ff
          clc     
          adc     #$19
          sta     video_data_h
          clc     
          adc     $2245
          eor     #$ff
          tay     
          lda     #$0b
          sta     <reg_box
          sta     video_reg_l
          stx     video_data_l
          tya     
          clc     
          adc     #$35
          sta     video_data_h
          lda     #$0a
          jsr     ex_dotmod.impl
          clc     
          rts     
le341_00:
          lda     #$0c
          sta     <reg_box
          sta     video_reg_l
          st1     #$02
          lda     $2246
          asl     A
          asl     A
          asl     A
          tax     
          dex     
          lsr     A
          eor     #$ff
          clc     
          adc     #$88
          sta     video_data_h
          lda     #$0d
          sta     <reg_box
          sta     video_reg_l
          stx     video_data_l
          st2     #$00
          lda     #$0e
          sta     <reg_box
          sta     video_reg_l
          txa     
          inc     A
          inc     A
          lsr     A
          adc     #$7d
          sta     video_data_l
          st2     #$00
          lda     $2244
          eor     #$04
          sta     color_ctrl
          rts     
ex_imode.impl:
          rmb3    <crh_m
          rmb4    <crh_m
          asl     A
          asl     A
          asl     A
          and     #$18
          ora     <crh_m
          sta     <crh_m
          bra     le3a8_00
ex_vmode.impl:
          smb3    <crh_m
          lda     $2242
          and     #$30
          beq     le3a6_00
          smb4    <crh_m
          and     #$01
          bne     le3a8_00
          rmb3    <crh_m
          bra     le3a8_00
ex_hmode.impl:
          rmb3    <crh_m
le3a6_00:
          rmb4    <crh_m
le3a8_00:
          lda     #$05
          sta     <reg_box
          sta     video_reg_l
          lda     <crh_m
          sta     video_data_h
          rts     
ex_vsync.impl:
          bbs1    <irq_m, le3c6_00
          cli     
          bbs3    <crl_m, le3be_00
          bsr     ex_irqon.impl
le3be_00:
          lda     irq_cnt
le3c1_00:
          cmp     irq_cnt
          beq     le3c1_00
le3c6_00:
          rts     
ex_rcron.impl:
          smb2    <crl_m
          bra     le3d5_00
ex_rcroff.impl:
          rmb2    <crl_m
          bra     le3d5_00
ex_irqon.impl:
          smb3    <crl_m
          bra     le3d5_00
ex_irqoff.impl:
          rmb3    <crl_m
le3d5_00:
          lda     #$05
          sta     <reg_box
          sta     video_reg_l
          lda     <crl_m
          sta     video_data_l
          rts     
ex_bgon.impl:
          smb7    <crl_m
          rts     
ex_bgoff.impl:
          rmb7    <crl_m
          rts     
ex_spron.impl:
          smb6    <crl_m
          rts     
ex_sproff.impl:
          rmb6    <crl_m
          rts     
ex_dspon.impl:
          smb6    <crl_m
          smb7    <crl_m
          rts     
ex_dspoff.impl:
          rmb6    <crl_m
          rmb7    <crl_m
          rts     
ex_dmamod.impl:
          and     #$1f
          sta     $2243
          lda     #$0f
          sta     <reg_box
          sta     video_reg_l
          lda     $2243
          sta     video_data_l
          rts     
ex_sprdma.impl:
          lda     #$13
          sta     <reg_box
          sta     video_reg_l
          lda     sat_addr
          sta     video_data_l
          lda     sat_addr+1
          sta     video_data_h
          rts     
ex_setrcr.impl:
          pha     
          lda     #$06
          sta     <reg_box
          sta     video_reg_l
          pla     
          sta     video_data_l
          stx     video_data_h
          rts     
ex_setred.impl:
          pha     
          lda     #$01
          sta     <reg_box
          sta     video_reg_l
          pla     
          sta     video_data_l
          stx     video_data_h
          lda     #$02
          sta     <reg_box
          sta     video_reg_l
          rts     
ex_setwrt.impl:
          pha     
          lda     #$00
          sta     <reg_box
          sta     video_reg_l
          pla     
          sta     video_data_l
          stx     video_data_h
          lda     #$02
          sta     <reg_box
          sta     video_reg_l
          rts     
ex_setdma.impl:
          lda     <reg_box
          pha     
          lda     #$10
          sta     <reg_box
          sta     video_reg_l
          lda     <_bl
          sta     video_data_l
          lda     <_bh
          sta     video_data_h
          lda     #$11
          sta     <reg_box
          sta     video_reg_l
          lda     <_dl
          sta     video_data_l
          lda     <_dh
          sta     video_data_h
          lda     #$12
          sta     <reg_box
          sta     video_reg_l
          lda     <_cl
          sta     video_data_l
          lda     <_ch
          sta     video_data_h
          pla     
          sta     <reg_box
          sta     video_reg_l
          rts     
ex_joysns.impl:
          lda     #$01
          sta     joyport
          lda     #$03
          sta     joyport
          cly     
le4a5_00:
          lda     #$01
          sta     joyport
          pha     
          pla     
          nop     
          nop     
          lda     joy, Y
          sta     joyold, Y
          lda     joyport
          asl     A
          asl     A
          asl     A
          asl     A
          sta     joy, Y
          stz     joyport
          pha     
          pla     
          nop     
          nop     
          lda     joyport
          and     #$0f
          ora     joy, Y
          eor     #$ff
          sta     joy, Y
          eor     joyold, Y
          and     joy, Y
          sta     joytrg, Y
          iny     
          cpy     #$05
          bcc     le4a5_00
          cly     
le4e1_00:
          lda     joyena
          and     unknown.0, Y
          beq     le4fe_00
          lda     joytrg, Y
          cmp     #$04
          bne     le4fe_00
          lda     joy, Y
          cmp     #$0c
          bne     le4fe_00
          lda     #$80
          sta     <main_sw
          jmp     [$2284]
le4fe_00:
          iny     
          cpy     #$05
          bcc     le4e1_00
          rts     
unknown.0:
          ora     [$02, X]

	.data
	.bank 0
	.org $e504
unknown.0:
    .db $01,$02,$04,$08,$10

	.code
	.bank 0
	.org $e509
ex_colorcmd.impl:
          lda     color_cmd
          bne     le50f_00
          rts     
le50f_00:
          and     #$01
          beq     le515_00
          bne     le579_00
le515_00:
          lda     #$e3
          sta     $2286
          lda     #$04
          sta     $2289
          sta     $228a
          lda     #$60
          sta     $228d
          lda     bgc_len
          beq     le54d_00
          stz     $228c
          asl     A
          asl     A
          asl     A
          asl     A
          rol     $228c
          asl     A
          rol     $228c
          sta     $228b
          tii     bgc_ptr, $2287, $0002
          stz     color_reg_l
          stz     color_reg_h
          jsr     $2286
le54d_00:
          lda     sprc_len
          beq     le575_00
          stz     $228c
          asl     A
          asl     A
          asl     A
          asl     A
          rol     $228c
          asl     A
          rol     $228c
          sta     $228b
          tii     sprc_ptr, $2287, $0002
          stz     color_reg_l
          lda     #$01
          sta     color_reg_h
          jsr     $2286
le575_00:
          stz     color_cmd
          rts     
le579_00:
          tii     unknown.1, $2286, $0003
          lda     #$60
          sta     $228d
          lda     bgc_len
          beq     le5ab_00
          stz     $228c
          asl     A
          asl     A
          asl     A
          asl     A
          rol     $228c
          asl     A
          rol     $228c
          sta     $228b
          tii     bgc_ptr, $2289, $0002
          stz     color_reg_l
          stz     color_reg_h
          jsr     $2286
le5ab_00:
          lda     sprc_len
          beq     le5d3_00
          stz     $228c
          asl     A
          asl     A
          asl     A
          asl     A
          rol     $228c
          asl     A
          rol     $228c
          sta     $228b
          tii     sprc_ptr, $2289, $0002
          stz     color_reg_l
          lda     #$01
          sta     color_reg_h
          jsr     $2286
le5d3_00:
          stz     color_cmd
          rts     

	.data
	.bank 0
	.org $e5d7
unknown.1:
    .db $f3,$04,$04

	.code
	.bank 0
	.org $e5da
ex_satclr.impl:
          lda     #$00
          sta     <reg_box
          sta     video_reg_l
          lda     sat_addr
          sta     video_data_l
          lda     sat_addr+1
          sta     video_data_h
          lda     #$02
          sta     <reg_box
          sta     video_reg_l
          clx     
le5f5_00:
          st1     #$00
          st2     #$00
          dex     
          bne     le5f5_00
          stz     spr_ptr
          rts     
ex_bcdbin.impl:
          phx     
          tax     
          and     #$f0
          cmp     #$a0
          bcs     le61e_00
          lsr     A
          sta     <zl0
          lsr     A
          lsr     A
          clc     
          adc     <zl0
          sta     <zl0
          txa     
          and     #$0f
          cmp     #$0a
          bcs     le61e_00
          clc     
          adc     <zl0
          tax     
          clc     
le61e_00:
          txa     
          plx     
          rts     
ex_binbcd.impl:
          cmp     #$64
          bcs     le63b_00
          phx     
          ldx     #$ff
          sec     
le629_00:
          inx     
          sbc     #$0a
          bcs     le629_00
          adc     #$0a
          sax     
          asl     A
          asl     A
          asl     A
          asl     A
          stx     <zl0
          adc     <zl0
          plx     
          clc     
le63b_00:
          rts     
ex_sprput.impl:
          lda     spr_ptr
          cmp     #$40
          bcs     le67d_00
          phy     
          lda     #$00
          sta     <reg_box
          sta     video_reg_l
          lda     spr_ptr
          asl     A
          asl     A
          clc     
          adc     sat_addr
          sta     video_data_l
          lda     sat_addr+1
          adc     #$00
          sta     video_data_h
          lda     #$02
          sta     <reg_box
          sta     video_reg_l
          cly     
le667_00:
          lda     spr_yl, Y
          sta     video_data_l
          iny     
          lda     spr_yl, Y
          sta     video_data_h
          iny     
          cpy     #$08
          bne     le667_00
          inc     spr_ptr
          ply     
le67d_00:
          rts     
ex_rnd.impl:
          lda     rndh
          rol     A
          rol     A
          rol     A
          eor     rndl
          ror     A
          php     
          lda     rndh
          eor     rndl
          sta     rndh
          lda     rndm
          eor     rndl
          sta     rndl
          plp     
          rol     rndl
          rol     rndh
          lda     rndh
          eor     rndl
          rts     
irq_nmi:
          bbs3    <irq_m, le6ad_00
          rti     
le6ad_00:
          jmp     [nmi_jmp]
le6b0_00:
          jmp     [tim_jmp]
irq_timer:
          bbs2    <irq_m, le6b0_00
          pha     
          phx     
          phy     
          sta     irq_status
          lda     <time_sw
          bne     le6cb_00
          inc     <time_sw
          cli     
          lda     <main_sw
          bne     le6c9_00
          bsr     psg_drive.impl
le6c9_00:
          stz     <time_sw
le6cb_00:
          ply     
          plx     
          pla     
          rti     
psg_drive.impl:
          tma     #$04
          pha     
          tma     #$05
          pha     
          tma     #$06
          pha     
          lda     $22eb
          tam     #$04
          lda     $22ec
          tam     #$05
          lda     #$02
          tam     #$06
          lda     $22df
          beq     le6fa_00
          jsr     $cd12
          cmp     #$00
          bne     le6fa_00
          lda     $22de
          ora     #$40
          sta     $22de
le6fa_00:
          lda     $22df
          cmp     #$01
          beq     le710_00
          jsr     $c2ef
          cmp     #$00
          bne     le710_00
          lda     $22de
          ora     #$80
          sta     $22de
le710_00:
          lda     <main_sw
          and     #$01
          bne     le723_00
          lda     $22d4
          bmi     le723_00
          sta     timer_cnt
          ora     #$80
          sta     $22d4
le723_00:
          pla     
          tam     #$06
          pla     
          tam     #$05
          pla     
          tam     #$04
          lda     $22ed
          sta     psg_ch
          rts     
le733_00:
          jmp     [irq2_jmp]
irq_2:
          bbs0    <irq_m, le733_00
          pha     
          phx     
          phy     
          bsr     le742_00
          ply     
          plx     
          pla     
          rti     
le742_00:
          lda     $1802
          and     bram_lock
          ora     <cdi_b
          sta     <cdi_b
          bbr2    <cdi_b, le7b3_00
          lda     #$04
          trb     $1802
          lda     #$04
          trb     <cdi_b
          cli     
          lda     $229e
          ora     $229f
          ora     $22a0
          beq     le7b3_00
          lda     cd_port
          and     #$80
          beq     le773_00
          stz     $22a4
          jsr     lf6ce_00
          bra     le7b3_00
le773_00:
          jsr     lf0ee_00
          lda     #$08
          sta     $224c
          lda     $22a1
          sta     $224d
          lda     $22a2
          sta     $224e
          lda     $22a3
          sta     $224f
          ldx     #$10
          lda     $229f
          ora     $22a0
          bne     le79f_00
          lda     $229e
          cmp     #$10
          bcs     le79f_00
          tax     
le79f_00:
          txa     
          sta     $229d
          sta     $2250
          lda     #$01
          sta     $22a4
          jsr     le905_00
          lda     #$60
          tsb     $1802
le7b3_00:
          bbr6    <cdi_b, le7c5_00
          lda     #$40
          trb     $1802
          lda     #$40
          trb     <cdi_b
          cli     
          lda     #$02
          sta     $180b
le7c5_00:
          bbr5    <cdi_b, le82d_00
          lda     #$20
          trb     $1802
          lda     #$20
          trb     <cdi_b
          cli     
          lda     $22a4
          beq     le7da_00
          stz     $180b
le7da_00:
          lda     $1801
          sta     scsists
          beq     le7e8_00
          stz     $22a4
          jsr     lf6ce_00
le7e8_00:
          lda     #$80
          tsb     $1802
le7ed_00:
          tst     #$40, $1800
          bne     le7ed_00
          lda     #$80
          trb     $1802
le7f8_00:
          lda     cd_port
          and     #$f8
          cmp     #$f8
          bne     le7f8_00
          lda     $1801
          lda     #$80
          tsb     $1802
le809_00:
          tst     #$40, $1800
          bne     le809_00
          lda     #$80
          trb     $1802
le814_00:
          tst     #$80, $1800
          bne     le814_00
          lda     $22a4
          beq     le82d_00
          stz     $22a4
          jsr     lf68c_00
          stz     $229d
          lda     #$04
          tsb     $1802
le82d_00:
          bbr4    <cdi_b, le842_00
          lda     #$10
          trb     $1802
          lda     #$10
          trb     <cdi_b
          cli     
          jsr     lf35a_00
          lda     #$10
          tsb     $1802
le842_00:
          bbr3    <cdi_b, le866_00
          lda     #$0c
          trb     $1802
          lda     #$08
          trb     <cdi_b
          lda     #$60
          trb     $180d
          jsr     lf6ce_00
          cli     
          lda     $22a5
          beq     le866_00
le85c_00:
          lda     $180c
          and     #$08
          bne     le85c_00
          jsr     lf5f5_00
le866_00:
          rts     
le867_00:
          jmp     [rcr_jmp]
le86a_00:
          jmp     [sync_jmp]
le86d_00:
          jmp     [irq_jmp]
irq_1:
          bbs1    <irq_m, le86d_00
          pha     
          phx     
          phy     
          lda     video_reg_l
          sta     <str_b
          bbr5    <str_b, le8b9_00
          st0     #$05
          lda     <crl_m
          sta     video_data_l
          lda     <crh_m
          sta     video_data_h
          inc     irq_cnt
          bbs5    <irq_m, le8b4_00
          st0     #$07
          tii     bgx1, video_data_l, $0002
          st0     #$08
          tii     bgy1, video_data_l, $0002
          jsr     ex_colorcmd.impl
          inc     rndl
          lda     <main_sw
          cmp     #$01
          bne     le8b1_00
          jsr     psg_drive.impl
le8b1_00:
          jsr     ex_joysns.impl
le8b4_00:
          bbr4    <irq_m, le8b9_00
          bsr     le86a_00
le8b9_00:
          bbr2    <str_b, le8da_00
          bbs7    <irq_m, le8d5_00
          bsr     le866_00
          bsr     le866_00
          st0     #$07
          tii     bgx2, video_data_l, $0002
          st0     #$08
          tii     bgy2, video_data_l, $0002
le8d5_00:
          bbr6    <irq_m, le8da_00
          bsr     le867_00
le8da_00:
          lda     <reg_box
          sta     video_reg_l
          ply     
          plx     
          pla     
          rti     
cd_reset.impl:
          lda     #$02
          tsb     $1804
          ldy     #$0a
le8ea_00:
          ldx     #$3b
le8ec_00:
          dex     
          bne     le8ec_00
          dey     
          bne     le8ea_00
          lda     $1804
          and     #$fd
          sta     $1804
          ldx     #$77
le8fc_00:
          dex     
          bne     le8fc_00
          rts     
le900_00:
          lda     $22a4
          bne     le900_00
le905_00:
          stz     $229b
le908_00:
          lda     #$81
          sta     $1801
          tst     #$80, $1800
          beq     le944_00
          lda     #$60
          trb     $1802
          sta     cd_port
          ldx     #$95
le91d_00:
          lda     #$ed
le91f_00:
          dec     A
          bne     le91f_00
          nop     
          nop     
          nop     
          dex     
          bne     le91d_00
          lda     #$ff
          sta     $1801
          tst     #$40, $1800
          beq     le908_00
le933_00:
          jsr     lea27_00
le936_00:
          tst     #$40, $1800
          bne     le933_00
          tst     #$80, $1800
          bne     le936_00
          bra     le908_00
le944_00:
          sta     cd_port
          clx     
le948_00:
          lda     cd_port
          and     #$40
          bne     le95a_00
          lda     #$5a
le951_00:
          dec     A
          bne     le951_00
          nop     
          dex     
          bne     le948_00
          bra     le908_00
le95a_00:
          stz     $227a
          clx     
le95e_00:
          lda     cd_port
          and     #$f8
          sta     $227a
          cmp     #$d0
          beq     le97a_00
          and     #$b8
          cmp     #$98
          beq     le98a_00
          cmp     #$88
          beq     le98a_00
          cmp     #$80
          beq     le98a_00
          bra     le95e_00
le97a_00:
          lda     $224c, X
          inx     
          sta     $1801
          nop     
          nop     
          nop     
          nop     
          jsr     lea27_00
          bra     le95e_00
le98a_00:
          lda     $22a4
          ora     $2248
          bne     le9c4_00
          lda     $224c
          cmp     #$d9
          bne     le9a4_00
          lda     $224d
          cmp     #$01
          beq     le9c4_00
          cmp     #$02
          beq     le9c4_00
le9a4_00:
          ldx     irq_cnt
le9a7_00:
          lda     cd_port
          and     #$f8
          sta     $227a
          bit     #$40
          bne     le9c4_00
          txa     
          sec     
          sbc     irq_cnt
          eor     #$ff
          inc     A
          cmp     #$fe
          bcc     le9a7_00
          lda     #$01
          sta     $229b
le9c4_00:
          rts     
le9c5_00:
          lda     $229b
          beq     le9d3_00
          stz     $229b
          jsr     cd_reset.impl
          lda     #$06
          rts     
le9d3_00:
          jsr     le9dc_00
          bne     le9db_00
          stz     suberrc
le9db_00:
          rts     
le9dc_00:
          stz     scsists
le9df_00:
          lda     cd_port
          and     #$f8
          sta     $227a
          cmp     #$d8
          bne     le9df_00
          lda     $1801
          sta     scsists
          bsr     lea35_00
le9f3_00:
          tst     #$40, $1800
          bne     le9f3_00
          bsr     lea2f_00
le9fb_00:
          lda     cd_port
          and     #$f8
          sta     $227a
          cmp     #$f8
          beq     lea0d_00
          cmp     #$d8
          bne     le9fb_00
          bra     le9df_00
lea0d_00:
          lda     $1801
          sta     $227a
          bsr     lea35_00
lea15_00:
          tst     #$40, $1800
          bne     lea15_00
          bsr     lea2f_00
lea1d_00:
          tst     #$80, $1800
          bne     lea1d_00
          lda     scsists
          rts     
lea27_00:
          bsr     lea35_00
lea29_00:
          tst     #$40, $1800
          bne     lea29_00
lea2f_00:
          lda     #$80
          trb     $1802
          rts     
lea35_00:
          lda     #$80
          tsb     $1802
          rts     
lea3b_00:
          lda     cd_port
          and     #$f8
          sta     $227a
          cmp     #$c8
          beq     lea4d_00
          cmp     #$d8
          beq     lea67_00
          bra     lea3b_00
lea4d_00:
          lda     $1801
          sta     [_bl]
          bsr     lea27_00
          inc     <_bl
          bne     lea5a_00
          inc     <_bh
lea5a_00:
          lda     <_al
          bne     lea60_00
          dec     <_ah
lea60_00:
          dec     A
          sta     <_al
          ora     <_ah
          bne     lea3b_00
lea67_00:
          lda     cd_port
          and     #$f8
          sta     $227a
          and     #$b8
          cmp     #$88
          bne     lea78_00
          jmp     leb5e_00
lea78_00:
          rts     
lea79_00:
          lda     cd_port
          and     #$f8
          sta     $227a
          cmp     #$c8
          beq     lea8b_00
          cmp     #$d8
          beq     leabd_00
          bra     lea79_00
lea8b_00:
          lda     <_al
          ora     <_ah
          beq     leab5_00
          ldx     #$03
lea93_00:
          dex     
          bne     lea93_00
          cly     
          ldx     #$08
lea99_00:
          lda     $1808
          sta     [_bl], Y
          lda     <_al
          bne     leaa4_00
          dec     <_ah
leaa4_00:
          dec     A
          sta     <_al
          ora     <_ah
          beq     leab5_00
          iny     
          bne     lea99_00
          inc     <_bh
          dex     
          bne     lea99_00
          bra     lea79_00
leab5_00:
          lda     cd_port
          and     #$f8
          sta     $227a
leabd_00:
          rts     
leabe_00:
          lda     cd_port
          and     #$f8
          sta     $227a
          cmp     #$c8
          beq     lead0_00
          cmp     #$d8
          beq     leb05_00
          bra     leabe_00
lead0_00:
          ldx     #$03
lead2_00:
          dex     
          bne     lead2_00
          cly     
          ldx     #$04
lead8_00:
          lda     $1808
          sta     video_data_l
          nop     
          nop     
          nop     
          nop     
          nop     
          nop     
          nop     
          nop     
          lda     $1808
          sta     video_data_h
          lda     <_al
          sec     
          sbc     #$02
          sta     <_al
          lda     <_ah
          sbc     #$00
          sta     <_ah
          ora     <_al
          beq     leb05_00
          dey     
          bne     lead8_00
          dex     
          bne     lead8_00
          bra     leabe_00
leb05_00:
          lda     cd_port
          and     #$f8
          sta     $227a
          and     #$b8
          cmp     #$88
          bne     leb15_00
          bra     leb5e_00
leb15_00:
          rts     
leb16_00:
          lda     #$02
          sta     <_bl
          lda     #$00
          sta     <_bh
leb1e_00:
          lda     cd_port
          and     #$f8
          sta     $227a
          cmp     #$c8
          beq     leb30_00
          cmp     #$d8
          beq     leb5d_00
          bra     leb1e_00
leb30_00:
          lda     $1801
          sta     [_bl]
          jsr     lea27_00
          lda     <_bl
          eor     #$01
          sta     <_bl
          lda     <_al
          sec     
          sbc     #$01
          sta     <_al
          lda     <_ah
          sbc     #$00
          sta     <_ah
          ora     <_al
          bne     leb1e_00
          lda     cd_port
          and     #$f8
          sta     $227a
          and     #$b8
          cmp     #$88
          beq     leb5e_00
leb5d_00:
          rts     
leb5e_00:
          lda     cd_port
          and     #$f8
          sta     $227a
          cmp     #$c8
          beq     leb70_00
          cmp     #$d8
          beq     leb8b_00
          bra     leb5e_00
leb70_00:
          ldx     #$05
leb72_00:
          dex     
          bne     leb72_00
          cly     
          ldx     #$08
leb78_00:
          lda     $1808
          nop     
          nop     
          nop     
          nop     
          nop     
          nop     
          nop     
          nop     
          iny     
          bne     leb78_00
          dex     
          bne     leb78_00
          bra     leb5e_00
leb8b_00:
          lda     $227a
          rts     
cd_base.impl:
          lda     <_bh
          beq     lebc7_00
          bbs6    <_bh, lebab_00
          lda     <_al
          sta     <_ah
          lda     #$02
          sta     <_al
          lda     #$56
          sta     <_bl
          lda     #$22
          sta     <_bh
          jsr     cd_dinfo.impl
          bra     lebba_00
lebab_00:
          lda     <_al
          sta     $2256
          lda     <_ah
          sta     $2257
          lda     <_bl
          sta     $2258
lebba_00:
          tma     #$06
          pha     
          lda     #$01
          tam     #$06
          jsr     $cc17
          pla     
          tam     #$06
lebc7_00:
          bbs1    <_cl, lebd9_00
          lda     <_al
          sta     recbase0_h
          lda     <_ah
          sta     recbase0_m
          lda     <_bl
          sta     recbase0_l
lebd9_00:
          bbs0    <_cl, lebeb_00
          lda     <_al
          sta     recbase1_h
          lda     <_ah
          sta     recbase1_m
          lda     <_bl
          sta     recbase1_l
lebeb_00:
          rts     
cd_exec.impl:
          lda     <_bl
          sta     $2282
          lda     <_bh
          sta     $2283
          bsr     cd_read.impl
          cmp     #$00
          beq     lebff_00
          jmp     irq_reset
lebff_00:
          ldx     #$ff
          txs     
          jmp     [$2282]
cd_read.impl:
          stz     $2273
          jsr     lf309_00
lec0b_00:
          jsr     lf0ee_00
          lda     #$08
          sta     $224c
          jsr     lf104_00
          ldx     #$04
          ldy     #$01
          jsr     lf327_00
          lda     <_dh
          cmp     #$02
          bcc     lec31_00
          cmp     #$ff
          beq     lec79_00
          cmp     #$fe
          bne     lec2e_00
          jmp     lecbb_00
lec2e_00:
          jmp     led03_00
lec31_00:
          lda     <_dh
          beq     lec43_00
          lda     <_al
          sta     $2280
          asl     A
          asl     A
          asl     A
          sta     <_ah
          stz     <_al
          bra     lec46_00
lec43_00:
          jsr     led9f_00
lec46_00:
          lda     $2280
          beq     lec76_00
          sta     $2250
          jsr     le900_00
          cmp     #$c8
          bne     lec5f_00
          jsr     lea79_00
          cmp     #$c8
          bne     lec5f_00
          jsr     leb5e_00
lec5f_00:
          jsr     le9c5_00
          cmp     #$00
          beq     lec6c_00
          jsr     le20b
          bcc     lec73_00
          rts     
lec6c_00:
          lda     <_al
          ora     <_ah
          bne     lec73_00
          rts     
lec73_00:
          jmp     lecf1_00
lec76_00:
          lda     #$22
          rts     
lec79_00:
          lda     <_al
          beq     lec76_00
          sta     $2250
          jsr     le900_00
          cmp     #$c8
          bne     leca7_00
          lda     <_al
          asl     A
          asl     A
          asl     A
          sta     <_ah
          stz     <_al
          lda     #$00
          sta     <reg_box
          sta     video_reg_l
          jsr     lf337_00
          lda     #$02
          sta     <reg_box
          sta     video_reg_l
          jsr     leabe_00
          stz     vdtin_flg
leca7_00:
          jsr     le9c5_00
          cmp     #$00
          beq     lecb4_00
          jsr     le20b
          bcc     lecf1_00
          rts     
lecb4_00:
          lda     <_al
          ora     <_ah
          bne     lecf1_00
          rts     
lecbb_00:
          jsr     led9f_00
          lda     $2280
          beq     lec76_00
          sta     $2250
          jsr     le900_00
          cmp     #$c8
          bne     lece4_00
          lda     #$00
          sta     <reg_box
          sta     video_reg_l
          jsr     lf337_00
          lda     #$02
          sta     <reg_box
          sta     video_reg_l
          jsr     leb16_00
          stz     vdtin_flg
lece4_00:
          jsr     le9c5_00
          cmp     #$00
          beq     lecb4_00
          jsr     le20b
          bcc     lecf1_00
          rts     
lecf1_00:
          jsr     lf318_00
          lda     suberrc
          cmp     #$10
          bcc     led00_00
          lda     #$01
          sta     $2273
led00_00:
          jmp     lec0b_00
led03_00:
          ldx     <_al
          beq     led0b_00
          cmp     #$07
          bmi     led0e_00
led0b_00:
          lda     #$22
          rts     
led0e_00:
          tax     
          lda     #$01
led11_00:
          asl     A
          dex     
          bne     led11_00
          sta     $228f
          lda     #$43
          sta     $228e
          lda     #$60
          sta     $2290
          jsr     $228e
          sta     $2268
          lda     <_bl
          sta     <_dl
          lda     <_al
          sta     $2250
          lsr     A
          lsr     A
          sta     $2280
          lda     <_al
          and     #$03
          sta     $2281
          jsr     le900_00
          ora     #$40
          cmp     #$c8
          bne     led82_00
led46_00:
          lda     $2280
          beq     led51_00
          lda     #$20
          sta     <_ah
          bra     led5b_00
led51_00:
          lda     $2281
          beq     led82_00
          asl     A
          asl     A
          asl     A
          sta     <_ah
led5b_00:
          stz     <_al
          lda     <_dh
          asl     A
          asl     A
          asl     A
          asl     A
          asl     A
          sta     <_bh
          stz     <_bl
          lda     #$53
          sta     $228e
          lda     <_dl
          jsr     $228e
          jsr     lea79_00
          ora     #$40
          cmp     #$c8
          bne     led82_00
          inc     <_dl
          dec     $2280
          bpl     led46_00
led82_00:
          jsr     le9c5_00
          cmp     #$00
          beq     led91_00
          jsr     le20b
          bcs     led91_00
          jmp     lecf1_00
led91_00:
          pha     
          lda     #$53
          sta     $228e
          lda     $2268
          jsr     $228e
          pla     
          rts     
led9f_00:
          lda     #$01
          sta     $2281
          lda     <_ah
          lsr     A
          bcc     ledac_00
          stz     $2281
ledac_00:
          lsr     A
          bcc     ledb2_00
          stz     $2281
ledb2_00:
          lsr     A
          bcc     ledb8_00
          stz     $2281
ledb8_00:
          sta     $2280
          lda     <_al
          beq     ledc2_00
          stz     $2281
ledc2_00:
          lda     $2281
          bne     ledca_00
          inc     $2280
ledca_00:
          rts     
cd_seek.impl:
          stz     $2273
          jsr     lf309_00
ledd1_00:
          jsr     lf0ee_00
          lda     #$08
          sta     $224c
          jsr     lf104_00
          ldx     #$04
          ldy     #$01
          jsr     lf327_00
          lda     #$01
          sta     $2250
          jsr     le900_00
          cmp     #$c8
          bne     ledf2_00
          jsr     leb5e_00
ledf2_00:
          jsr     le9c5_00
          cmp     #$00
          beq     ledfe_00
          jsr     le20b
          bcc     ledff_00
ledfe_00:
          rts     
ledff_00:
          jsr     lf318_00
          lda     suberrc
          cmp     #$10
          bcc     ledd1_00
          lda     #$01
          sta     $2273
          bra     ledd1_00
cd_play.impl:
          jsr     lf309_00
lee13_00:
          jsr     lf0ee_00
          lda     <_bh
          and     #$c0
          cmp     #$c0
          beq     lee55_00
          cmp     #$80
          bne     lee26_00
          stz     <_ah
          stz     <_bl
lee26_00:
          sta     $2255
          lda     #$d8
          sta     $224c
          lda     <_bh
          bne     lee3a_00
          clx     
          ldy     #$03
          jsr     lf327_00
          bra     lee40_00
lee3a_00:
          clx     
          ldy     #$02
          jsr     lf327_00
lee40_00:
          jsr     le900_00
          jsr     le9c5_00
          cmp     #$00
          beq     lee55_00
          jsr     le20b
          bcc     lee50_00
          rts     
lee50_00:
          jsr     lf318_00
          bra     lee13_00
lee55_00:
          lda     <_bh
          and     #$01
          beq     lee5d_00
lee5b_00:
          cla     
          rts     
lee5d_00:
          jsr     lf0ee_00
          lda     #$d9
          sta     $224c
          lda     <_dh
          and     #$07
          sta     $224d
          lda     <_dh
          and     #$c0
          sta     $2255
          bne     lee7e_00
          ldx     #$04
          ldy     #$03
          jsr     lf327_00
          bra     leebe_00
lee7e_00:
          cmp     #$c0
          beq     lee93_00
          cmp     #$80
          bne     lee8a_00
          stz     <_ch
          stz     <_dl
lee8a_00:
          ldx     #$04
          ldy     #$02
          jsr     lf327_00
          bra     leebe_00
lee93_00:
          lda     outmin
          sta     $226f
          lda     outsec
          sta     $2270
          lda     outfrm
          sta     $2271
          bsr     lef02_00
          lda     $226f
          sta     $224e
          lda     $2270
          sta     $224f
          lda     $2271
          sta     $2250
          lda     #$40
          sta     $2255
leebe_00:
          jsr     le900_00
          cmp     #$d8
          beq     leee5_00
          lda     $224d
          cmp     #$04
          bne     leecf_00
          lda     $229c
leecf_00:
          sta     $229c
          cmp     #$01
          beq     lee5b_00
          cmp     #$02
          bne     leee5_00
          stz     $22a4
          lda     #$20
          tsb     $1802
          jmp     lee5b_00
leee5_00:
          lda     $224d
          cmp     #$04
          bne     leeef_00
          sta     $229c
leeef_00:
          jsr     le9c5_00
          cmp     #$00
          beq     lef01_00
          jsr     le20b
          bcs     lef01_00
          jsr     lf318_00
          jmp     lee55_00
lef01_00:
          rts     
lef02_00:
          pha     
          sed     
          lda     $2271
          sec     
          sbc     #$01
          sta     $2271
          cmp     #$75
          bcc     lef31_00
          lda     #$74
          sta     $2271
          lda     $2270
          sec     
          sbc     #$01
          sta     $2270
          cmp     #$60
          bcc     lef31_00
          lda     #$59
          sta     $2270
          lda     $226f
          sec     
          sbc     #$01
          sta     $226f
lef31_00:
          cld     
          pla     
          rts     
cd_search.impl:
          jsr     lf309_00
lef37_00:
          jsr     lf0ee_00
          lda     #$d8
          sta     $224c
          lda     <_bh
          and     #$c0
          sta     $2255
          bne     lef50_00
          clx     
          ldy     #$03
          jsr     lf327_00
          bra     lef5e_00
lef50_00:
          cmp     #$80
          bne     lef58_00
          stz     <_ah
          stz     <_bl
lef58_00:
          clx     
          ldy     #$02
          jsr     lf327_00
lef5e_00:
          lda     <_bh
          tax     
          and     #$01
          sta     $2248
          txa     
          and     #$02
          lsr     A
          sta     $224d
          jsr     le900_00
          lda     $2248
          beq     lef82_00
          stz     $22a4
          lda     #$20
          tsb     $1802
          stz     $2248
          cla     
          rts     
lef82_00:
          jsr     le9c5_00
          cmp     #$00
          beq     lef93_00
          jsr     le20b
          bcs     lef93_00
          jsr     lf318_00
          bra     lef37_00
lef93_00:
          rts     
cd_pause.impl:
          lda     #$56
          sta     <_bl
          lda     #$22
          sta     <_bh
          bsr     cd_subq.impl
          lda     $2256
          cmp     #$00
          beq     lefa7_00
          cla     
          rts     
lefa7_00:
          jsr     lf0ee_00
          lda     #$da
          sta     $224c
          jsr     le900_00
          jsr     le9c5_00
          cmp     #$00
          beq     lefbe_00
          jsr     le20b
          bcc     cd_pause.impl
lefbe_00:
          rts     
cd_subq.impl:
          jsr     lf309_00
lefc2_00:
          jsr     lf0ee_00
          lda     #$dd
          sta     $224c
          lda     #$0a
          sta     $224d
          jsr     le900_00
          cmp     #$c8
          bne     lefdf_00
          lda     #$0a
          sta     <_al
          stz     <_ah
          jsr     lea3b_00
lefdf_00:
          jsr     le9c5_00
          cmp     #$00
          beq     leff0_00
          jsr     le20b
          bcs     leff0_00
          jsr     lf318_00
          bra     lefc2_00
leff0_00:
          rts     
cd_dinfo.impl:
          jsr     lf309_00
leff4_00:
          jsr     lf0ee_00
          lda     #$de
          sta     $224c
          lda     <_al
          sta     $224d
          lda     <_ah
          sta     $224e
          jsr     le900_00
          cmp     #$c8
          bne     lf016_00
          lda     #$04
          sta     <_al
          stz     <_ah
          jsr     lea3b_00
lf016_00:
          jsr     le9c5_00
          pha     
          jsr     lf318_00
          pla     
          cmp     #$00
          beq     lf02c_00
          jsr     le20b
          bcs     lf02c_00
          jsr     lf318_00
          bra     leff4_00
lf02c_00:
          rts     
ex_getver.impl:
          ldx     $fec2
          ldy     $fec3
          rts     
ex_setvec.impl:
          php     
          sei     
          cmp     #$06
          bne     lf042_00
          stx     $2284
          sty     $2285
          bra     lf051_00
lf042_00:
          pha     
          phy     
          phx     
          asl     A
          tax     
          pla     
          sta     irq2_jmp, X
          inx     
          pla     
          sta     irq2_jmp, X
          pla     
lf051_00:
          plp     
          rts     
lf053_00:
          jsr     lf0f9_00
          jsr     lf0ee_00
          lda     #$03
          sta     $224c
          lda     #$0a
          sta     $2250
          jsr     le900_00
          cmp     #$c8
          bne     lf07b_00
          lda     #$56
          sta     <_bl
          lda     #$22
          sta     <_bh
          lda     #$0a
          sta     <_al
          stz     <_ah
          jsr     lea3b_00
lf07b_00:
          jsr     le9c5_00
          pha     
          cmp     #$00
          bne     lf08f_00
          lda     $2258
          sta     $227d
          lda     $225f
          sta     suberrc
lf08f_00:
          pla     
          rts     
lf091_00:
          bsr     lf0ee_00
          lda     #$00
          sta     $224c
          jsr     le900_00
          jsr     le9c5_00
          cmp     #$00
          beq     lf0a7_00
          jsr     le20b
          sec     
          rts     
lf0a7_00:
          clc     
          rts     
cd_contnts.impl:
          stz     <_al
          lda     #$56
          sta     <_bl
          lda     #$22
          sta     <_bh
          jsr     cd_dinfo.impl
          cmp     #$00
          bne     lf0ed_00
          lda     $2256
          sta     tnomin
          lda     $2257
          sta     $226b
          lda     #$01
          sta     <_al
          lda     #$56
          sta     <_bl
          lda     #$22
          sta     <_bh
          jsr     cd_dinfo.impl
          cmp     #$00
          bne     lf0ed_00
          pha     
          lda     $2256
          sta     outmin
          lda     $2257
          sta     outsec
          lda     $2258
          sta     outfrm
          pla     
lf0ed_00:
          rts     
lf0ee_00:
          phx     
          ldx     #$09
lf0f1_00:
          stz     $224c, X
          dex     
          bne     lf0f1_00
          plx     
          rts     
lf0f9_00:
          phx     
          ldx     #$09
lf0fc_00:
          stz     $2256, X
          dex     
          bpl     lf0fc_00
          plx     
          rts     
lf104_00:
          lda     $2273
          asl     A
          clc     
          adc     $2273
          tay     
          lda     <_dl
          clc     
          adc     recbase0_l, Y
          sta     <_dl
          lda     <_ch
          adc     recbase0_m, Y
          sta     <_ch
          lda     <_cl
          adc     recbase0_h, Y
          sta     <_cl
          rts     
ex_getfnt.impl:
          jsr     lf1e3_00
          bcc     lf12c_00
          lda     #$01
          rts     
lf12c_00:
          lda     #$a0
          sta     <_cl
          lda     <_bh
          sec     
          sbc     #$a0
          bpl     lf143_00
          tma     #$05
          sta     $22ba
          tma     #$06
          sta     $22bb
          bra     lf151_00
lf143_00:
          lda     #$60
          sta     <_cl
          tma     #$03
          sta     $22b8
          tma     #$04
          sta     $22b9
lf151_00:
          lda     <_al
          sta     <zl0
          lda     <_ah
          sta     <zh0
          stz     <zl1
          bbr0    <_dh, lf17d_00
          ldx     #$08
lf160_00:
          clc     
          lda     <zl0
          adc     <_al
          sta     <zl0
          lda     <zh0
          adc     <_ah
          sta     <zh0
          cla     
          adc     <zl1
          sta     <zl1
          dex     
          bne     lf160_00
          asl     <zl0
          rol     <zh0
          rol     <zl1
          bra     lf188_00
lf17d_00:
          ldx     #$05
lf17f_00:
          asl     <zl0
          rol     <zh0
          rol     <zl1
          dex     
          bne     lf17f_00
lf188_00:
          lda     <_dh
          asl     A
          inc     A
          adc     <zl1
          sta     <zl1
          lda     <zh0
          ldx     #$05
lf194_00:
          lsr     <zl1
          ror     A
          dex     
          bne     lf194_00
          sta     <zl1
          lda     <zh0
          and     #$1f
          clc     
          adc     <_cl
          sta     <zh0
          lda     <zl1
          bbs7    <_cl, lf1b1_00
          tam     #$03
          inc     A
          tam     #$04
          bra     lf1b6_00
lf1b1_00:
          tam     #$05
          inc     A
          tam     #$06
lf1b6_00:
          bbs0    <_dh, lf1c5_00
          cly     
lf1ba_00:
          lda     [zl0], Y
          sta     [_bl], Y
          iny     
          cpy     #$20
          bne     lf1ba_00
          bra     lf1c8_00
lf1c5_00:
          jsr     lf2cc_00
lf1c8_00:
          bbs7    <_cl, lf1d7_00
          lda     $22b8
          tam     #$03
          lda     $22b9
          tam     #$04
          bra     lf1e1_00
lf1d7_00:
          lda     $22ba
          tam     #$05
          lda     $22bb
          tam     #$06
lf1e1_00:
          cla     
          rts     
lf1e3_00:
          lda     <_ah
          sec     
          sbc     #$81
          cmp     #$6f
          bcs     lf214_00
          cmp     #$1f
          bcc     lf1f6_00
          cmp     #$3f
          bcc     lf214_00
          sbc     #$40
lf1f6_00:
          asl     A
          adc     #$21
          sta     <_ah
          lda     <_al
          sec     
          sbc     #$40
          cmp     #$bd
          bcs     lf214_00
          cmp     #$3f
          beq     lf214_00
          bcs     lf20b_00
          inc     A
lf20b_00:
          cmp     #$5f
          bcs     lf216_00
          clc     
          adc     #$20
          bra     lf21b_00
lf214_00:
          sec     
          rts     
lf216_00:
          inc     <_ah
          sec     
          sbc     #$3e
lf21b_00:
          sta     <_al
          lda     <_ah
          cmp     #$23
          beq     lf23c_00
          cmp     #$26
          beq     lf24f_00
          cmp     #$27
          beq     lf259_00
          cmp     #$28
          bcc     lf265_00
          cmp     #$30
          bcc     lf214_00
          cmp     #$50
          bcs     lf214_00
          sec     
          sbc     #$08
          bra     lf265_00
lf23c_00:
          lda     <_al
          cmp     #$61
          bcc     lf244_00
          sbc     #$06
lf244_00:
          cmp     #$41
          bcc     lf24a_00
          sbc     #$07
lf24a_00:
          sec     
          sbc     #$0f
          bra     lf261_00
lf24f_00:
          lda     <_al
          cmp     #$41
          bcc     lf261_00
          sbc     #$08
          bra     lf261_00
lf259_00:
          lda     <_al
          cmp     #$51
          bcc     lf261_00
          sbc     #$0f
lf261_00:
          sta     <_al
          lda     <_ah
lf265_00:
          sec     
          sbc     #$21
          asl     A
          tay     
          lda     <_al
          sec     
          sbc     #$21
          clc     
          adc     $f27e, Y
          sta     <_al
          lda     $f27f, Y
          adc     #$00
          sta     <_ah
          clc     
          rts     
          brk     
          brk     
          lsr     $6c00, X
          brk     
          tax     
          brk     
          sbc     $5300, X
          ora     [$83, X]
          ora     [$c5, X]
          ora     [$23, X]
          sxy     
          sta     [$02, X]
          bbs5    <$02, lf2d2_00
          st0     #$9b
          st0     #$f9
          st0     #$57
          tsb     <$b5
          tsb     <$13
          ora     <$71
          ora     <$cf
          ora     <$2d
          asl     <$8b
          asl     <si+1
          asl     <$47
          rmb0    <$a5
          rmb0    <$03
          php     
          adc     [$08, X]
          bbs3    <$08, lf2d2_00
          ora     #$7b
          ora     #$d9
          ora     #$37
          asl     A
          sta     <$0a, X
          tai     $510a, $af0b, $0d0b
          tsb     $0c6b
          cmp     #$0c
          rmb2    <$0d
lf2cc_00:
          cly     
lf2cd_00:
          lda     [zl0], Y
          iny     
          bsr     lf300_00
lf2d2_00:
          lda     [zl0], Y
          iny     
          pha     
          and     #$f0
          bsr     lf300_00
          lda     [zl0], Y
          iny     
          sta     $22bd
          pla     
          ldx     #$04
lf2e3_00:
          asl     $22bd
          rol     A
          dex     
          bne     lf2e3_00
          bsr     lf300_00
          lda     $22bd
          and     #$f0
          bsr     lf300_00
          cpy     #$12
          bcc     lf2cd_00
          cla     
          ldy     #$07
lf2fa_00:
          sta     [_bl], Y
          dey     
          bpl     lf2fa_00
          rts     
lf300_00:
          sta     [_bl]
          inc     <_bl
          bne     lf308_00
          inc     <_bh
lf308_00:
          rts     
lf309_00:
          phx     
          pha     
          ldx     #$07
lf30d_00:
          lda     <_al, X
          sta     $2260, X
          dex     
          bpl     lf30d_00
          pla     
          plx     
          rts     
lf318_00:
          phx     
          pha     
          ldx     #$07
lf31c_00:
          lda     $2260, X
          sta     <_al, X
          dex     
          bpl     lf31c_00
          pla     
          plx     
          rts     
lf327_00:
          lda     <_al, X
          sta     $224c, Y
          lda     <_ah, X
          sta     $224d, Y
          lda     <_bl, X
          sta     $224e, Y
          rts     
lf337_00:
          lda     #$01
          sta     vdtin_flg
          lda     <_bl
          sta     video_data_l
          lda     <_bh
          sta     video_data_h
          rts     
cd_stat.impl:
          cmp     #$00
          bne     lf351_00
          lda     cd_port
          and     #$80
          rts     
lf351_00:
          jmp     lf091_00
cd_subrd.impl:
          tst     #$10, $1803
          beq     lf362_00
lf35a_00:
          lda     bram_unlock
          sta     sibcode
          clc     
          rts     
lf362_00:
          sec     
          rts     
cd_pcmrd.impl:
          sta     $1805
          ldx     #$1b
lf369_00:
          dex     
          bne     lf369_00
          ldx     $1805
          ldy     $1806
          lda     bram_lock
          and     #$02
          lsr     A
          rts     
cd_fade.impl:
          and     #$0f
          sta     $180f
          rts     
ad_reset.impl:
          lda     #$80
          sta     $180d
          stz     $180d
          stz     $180b
          lda     #$6f
          trb     $1802
          stz     $180e
          rts     
ad_trans.impl:
          tst     #$03, $180b
          beq     lf39d_00
          lda     #$04
          bra     lf3fd_00
lf39d_00:
          stz     $2273
          jsr     lf309_00
lf3a3_00:
          lda     <_dh
          bne     lf3b1_00
          ldx     <_bl
          ldy     <_bh
          jsr     lf729_00
          jsr     lf710_00
lf3b1_00:
          jsr     lf0ee_00
          lda     #$08
          sta     $224c
          jsr     lf104_00
          ldx     #$04
          ldy     #$01
          jsr     lf327_00
          lda     <_al
          sta     $2250
          jsr     le900_00
          lda     #$02
          sta     $180b
lf3d0_00:
          tst     #$04, $180c
          bne     lf3d0_00
          lda     bram_lock
          and     #$20
          beq     lf3d0_00
          stz     $180b
          jsr     le9c5_00
          cmp     #$00
          beq     lf3fd_00
          jsr     le20b
          bcs     lf3fd_00
          jsr     lf318_00
          lda     suberrc
          cmp     #$10
          bcc     lf3a3_00
          lda     #$01
          sta     $2273
          bra     lf3a3_00
lf3fd_00:
          rts     
lf3fe_00:
          lda     $180c
          bmi     lf3fe_00
          lda     $180a
          rts     
ad_read.impl:
          jsr     ad_stat.impl
          bne     ad_read.impl
          ldx     <_cl
          ldy     <_ch
          jsr     lf729_00
          jsr     lf6ff_00
          bsr     lf3fe_00
          lda     <_dh
          beq     lf423_00
          cmp     #$ff
          beq     lf43d_00
          jmp     lf46b_00
lf423_00:
          bsr     lf3fe_00
          sta     [_bl]
          inc     <_bl
          bne     lf42d_00
          inc     <_bh
lf42d_00:
          lda     <_al
          bne     lf433_00
          dec     <_ah
lf433_00:
          dec     <_al
          lda     <_al
          ora     <_ah
          bne     lf423_00
          cla     
          rts     
lf43d_00:
          lda     #$00
          sta     <reg_box
          sta     video_reg_l
          jsr     lf6f5_00
          lda     #$02
          sta     <reg_box
          sta     video_reg_l
lf44e_00:
          bsr     lf3fe_00
          sta     [_bl]
          lda     <_bl
          eor     #$01
          sta     <_bl
          lda     <_al
          bne     lf45e_00
          dec     <_ah
lf45e_00:
          dec     <_al
          lda     <_al
          ora     <_ah
          bne     lf44e_00
          stz     vdtin_flg
          cla     
          rts     
lf46b_00:
          cmp     #$07
          bmi     lf472_00
          lda     #$22
          rts     
lf472_00:
          tax     
          lda     #$01
lf475_00:
          asl     A
          dex     
          bne     lf475_00
          sta     $228f
          lda     #$43
          sta     $228e
          lda     #$60
          sta     $2290
          jsr     $228e
          sta     $2268
          lda     <_bl
          sta     <_dl
lf490_00:
          lda     <_dh
          asl     A
          asl     A
          asl     A
          asl     A
          asl     A
          sta     <_bh
          stz     <_bl
          lda     #$53
          sta     $228e
          lda     <_dl
          jsr     $228e
          ldx     #$20
          cly     
lf4a8_00:
          jsr     lf3fe_00
          sta     [_bl]
          inc     <_bl
          bne     lf4b3_00
          inc     <_bh
lf4b3_00:
          lda     <_al
          bne     lf4b9_00
          dec     <_ah
lf4b9_00:
          dec     <_al
          lda     <_al
          ora     <_ah
          beq     lf4cb_00
          dey     
          bne     lf4a8_00
          dex     
          bne     lf4a8_00
          inc     <_dl
          bra     lf490_00
lf4cb_00:
          lda     #$53
          sta     $228e
          lda     $2268
          jsr     $228e
          cla     
          rts     
ad_write.impl:
          lda     $180b
          and     #$03
          bne     lf51c_00
          lda     <_cl
          and     <_ch
          cmp     #$ff
          beq     lf4f1_00
          ldx     <_cl
          ldy     <_ch
          jsr     lf729_00
          jsr     lf710_00
lf4f1_00:
          lda     <_dh
          beq     lf4fc_00
          cmp     #$ff
          beq     lf51d_00
          jmp     lf552_00
lf4fc_00:
          lda     [_bl]
          sta     $180a
lf501_00:
          tst     #$04, $180c
          bne     lf501_00
          inc     <_bl
          bne     lf50d_00
          inc     <_bh
lf50d_00:
          lda     <_al
          bne     lf513_00
          dec     <_ah
lf513_00:
          dec     <_al
          lda     <_al
          ora     <_ah
          bne     lf4fc_00
          cla     
lf51c_00:
          rts     
lf51d_00:
          lda     #$01
          sta     <reg_box
          sta     video_reg_l
          jsr     lf6f5_00
          lda     #$02
          sta     <reg_box
          sta     video_reg_l
lf52e_00:
          tst     #$04, $180c
          bne     lf52e_00
          lda     [_bl]
          sta     $180a
          lda     <_bl
          eor     #$01
          sta     <_bl
          lda     <_al
          bne     lf545_00
          dec     <_ah
lf545_00:
          dec     <_al
          lda     <_al
          ora     <_ah
          bne     lf52e_00
          stz     vdtin_flg
          cla     
lf551_00:
          rts     
lf552_00:
          cmp     #$07
          bmi     lf55a_00
          lda     #$22
          bra     lf551_00
lf55a_00:
          tax     
          lda     #$01
lf55d_00:
          asl     A
          dex     
          bne     lf55d_00
          sta     $228f
          lda     #$43
          sta     $228e
          lda     #$60
          sta     $2290
          jsr     $228e
          sta     $2268
          lda     <_bl
          sta     <_dl
lf578_00:
          lda     <_dh
          asl     A
          asl     A
          asl     A
          asl     A
          asl     A
          sta     <_bh
          stz     <_bl
          lda     #$53
          sta     $228e
          lda     <_dl
          jsr     $228e
          ldx     #$20
          cly     
lf590_00:
          tst     #$04, $180c
          bne     lf590_00
          lda     [_bl]
          sta     $180a
          inc     <_bl
          bne     lf5a1_00
          inc     <_bh
lf5a1_00:
          lda     <_al
          bne     lf5a7_00
          dec     <_ah
lf5a7_00:
          dec     <_al
          lda     <_al
          ora     <_ah
          beq     lf5b9_00
          dey     
          bne     lf590_00
          dex     
          bne     lf590_00
          inc     <_dl
          bra     lf578_00
lf5b9_00:
          lda     #$53
          sta     $228e
          lda     $2268
          jsr     $228e
          cla     
          rts     
ad_play.impl:
          jsr     ad_stat.impl
          bne     lf61e_00
          lda     <_dl
          and     #$80
          sta     $22a5
          lda     <_dl
          and     #$7f
          bne     lf613_00
          lda     <_bl
          sta     $22a8
          lda     <_bh
          sta     $22a9
          lda     <_al
          sta     $22a6
          lda     <_ah
          sta     $22a7
          lda     <_dh
          cmp     #$10
          bcs     lf61e_00
          sta     $22aa
lf5f5_00:
          ldx     $22a8
          ldy     $22a9
          jsr     lf729_00
          jsr     lf6ff_00
          ldx     $22a6
          ldy     $22a7
          jsr     lf729_00
          jsr     lf71e_00
          lda     $22aa
          sta     $180e
lf613_00:
          lda     #$08
          tsb     $1802
          lda     #$60
          sta     $180d
          cla     
lf61e_00:
          rts     
ad_cplay.impl:
          jsr     ad_stat.impl
          bne     lf68b_00
          lda     <_al
          sta     $229e
          lda     <_ah
          sta     $229f
          lda     <_bl
          sta     $22a0
          lda     <_dl
          clc     
          adc     recbase0_l
          sta     $22a3
          lda     <_ch
          adc     recbase0_m
          sta     $22a2
          lda     <_cl
          adc     recbase0_h
          sta     $22a1
          jsr     ad_reset.impl
          lda     <_dh
          cmp     #$10
          bcs     lf68b_00
          sta     $180e
          lda     #$20
          sta     <_al
          lda     #$00
          sta     <_dh
          stz     <_bl
          stz     <_bh
          jsr     ad_trans.impl
          cmp     #$00
          bne     lf68b_00
          lda     #$20
          sta     $229d
          bsr     lf68c_00
          lda     #$ff
          sta     $1808
          sta     $1809
          jsr     lf71e_00
          stz     $22a5
          lda     #$0c
          tsb     $1802
          lda     #$60
          sta     $180d
          cla     
lf68b_00:
          rts     
lf68c_00:
          clc     
          lda     $22a3
          adc     $229d
          sta     $22a3
          lda     $22a2
          adc     #$00
          sta     $22a2
          lda     $22a1
          adc     #$00
          sta     $22a1
          sec     
          lda     $229e
          sbc     $229d
          sta     $229e
          lda     $229f
          sbc     #$00
          sta     $229f
          lda     $22a0
          sbc     #$00
          sta     $22a0
          rts     
ad_stop.impl:
          lda     #$0c
          trb     $1802
          lda     #$60
          trb     $180d
          stz     $22a5
lf6ce_00:
          stz     $229d
          stz     $229e
          stz     $229f
          stz     $22a0
          rts     
ad_stat.impl:
          lda     $180c
          and     #$01
          bne     lf6e7_00
          lda     bram_lock
          and     #$04
lf6e7_00:
          tax     
          lda     $180d
          and     #$20
          bne     lf6f4_00
          lda     $180c
          and     #$08
lf6f4_00:
          rts     
lf6f5_00:
          jsr     lf337_00
          lda     #$02
          sta     <_bl
          stz     <_bh
          rts     
lf6ff_00:
          lda     #$08
          tsb     $180d
          lda     $180a
          lda     #$05
lf709_00:
          dec     A
          bne     lf709_00
          lda     #$08
          bra     lf725_00
lf710_00:
          lda     #$03
          tsb     $180d
          lda     #$01
          trb     $180d
          lda     #$02
          bra     lf725_00
lf71e_00:
          lda     #$10
          tsb     $180d
          lda     #$10
lf725_00:
          trb     $180d
          rts     
lf729_00:
          stx     $1808
          sty     $1809
          rts     
sys_unknown_1.impl:
          tst     #$03, $180b
          beq     lf739_00
          sec     
          bra     lf76b_00
lf739_00:
          lda     #$08
          sta     $224c
          jsr     lf104_00
          lda     <_cl
          sta     $224d
          lda     <_ch
          sta     $224e
          lda     <_dl
          sta     $224f
          lda     <_al
          sta     $2250
          stz     $2251
          lda     <_bl
          sta     $22c3
          lda     <_bh
          sta     $22c4
          stz     $22c1
          lda     #$01
          sta     $22c2
          clc     
lf76b_00:
          rts     
sys_unknown_2.impl:
          lda     $22c2
          beq     lf791_00
          cmp     #$01
          bne     lf779_00
          bsr     lf793_00
          bra     lf791_00
lf779_00:
          cmp     #$02
          bne     lf781_00
          bsr     lf7bb_00
          bra     lf791_00
lf781_00:
          cmp     #$03
          bne     lf789_00
          bsr     lf7ef_00
          bra     lf791_00
lf789_00:
          cmp     #$04
          bne     lf791_00
          jsr     lf809_00
          rts     
lf791_00:
          clc     
          rts     
lf793_00:
          lda     #$81
          sta     $1801
          tst     #$80, $1800
          bne     lf7b2_00
          sta     cd_port
          clx     
lf7a2_00:
          lda     cd_port
          and     #$40
          bne     lf7b5_00
          lda     #$5a
lf7ab_00:
          dec     A
          bne     lf7ab_00
          nop     
          dex     
          bne     lf7a2_00
lf7b2_00:
          lda     #$01
          rts     
lf7b5_00:
          lda     #$02
          sta     $22c2
          rts     
lf7bb_00:
          lda     cd_port
          and     #$f8
          sta     $227a
          cmp     #$d0
          beq     lf7d2_00
          and     #$b8
          cmp     #$98
          beq     lf7e9_00
          cmp     #$88
          beq     lf7e5_00
          rts     
lf7d2_00:
          ldx     $22c1
          inc     $22c1
          lda     $224c, X
          sta     $1801
          nop     
          nop     
          nop     
          nop     
          jmp     lea27_00
lf7e5_00:
          lda     #$03
          bra     lf7eb_00
lf7e9_00:
          lda     #$04
lf7eb_00:
          sta     $22c2
          rts     
lf7ef_00:
          lda     $22c3
          sta     $1808
          lda     $22c4
          sta     $1809
          jsr     lf710_00
          lda     #$02
          sta     $180b
          lda     #$04
          sta     $22c2
          rts     
lf809_00:
          lda     cd_port
          and     #$f8
          sta     $227a
          cmp     #$d8
          bne     lf856_00
          stz     $180b
          lda     $1801
          sta     scsists
          jsr     lea35_00
lf821_00:
          tst     #$40, $1800
          bne     lf821_00
          jsr     lea2f_00
lf82a_00:
          lda     cd_port
          and     #$f8
          sta     $227a
          cmp     #$f8
          bne     lf82a_00
          lda     $1801
          sta     $227a
          jsr     lea35_00
lf83f_00:
          tst     #$40, $1800
          bne     lf83f_00
          jsr     lea2f_00
lf848_00:
          tst     #$80, $1800
          bne     lf848_00
          stz     $22c2
          sec     
          lda     scsists
          rts     
lf856_00:
          clc     
          rts     
bm_format.impl:
          ldy     #$0a
lf85a_00:
          lda     $f8ad, Y
          cmp     [_al], Y
          bne     lf8a6_00
          dey     
          bpl     lf85a_00
          jsr     lfc49_00
          lda     #$80
          sta     <zh0
          stz     <zl0
          ldx     #$20
lf86f_00:
          cly     
          cla     
lf871_00:
          sta     [zl0], Y
          cmp     [zl0], Y
          bne     lf87f_00
          iny     
          bne     lf871_00
          inc     <zh0
          dex     
          bne     lf86f_00
lf87f_00:
          sty     $8004
          lda     <zh0
          sta     $8005
          ldx     #$03
lf889_00:
          lda     unknown.2, X
          sta     $8000, X
          dex     
          bpl     lf889_00
          lda     #$10
          sta     $8006
          lda     #$80
          sta     $8007
          stz     $8010
          stz     $8011
          cla     
          jmp     lfc7f_00
lf8a6_00:
          lda     #$01
          rts     

	.data
	.bank 0
	.org $f8a9
unknown.2:
    .db $48,$55,$42,$4d,$21,$42,$4d,$20
    .db $46,$4f,$52,$4d,$41,$54,$21

	.code
	.bank 0
	.org $f8b8
bm_free.impl:
          jsr     lfc32_00
          lda     $8004
          sec     
          sbc     $8006
          sta     <_cl
          lda     $8005
          sbc     $8007
          sta     <_ch
          lda     <_cl
          sec     
          sbc     #$12
          sta     <_cl
          lda     <_ch
          sbc     #$00
          sta     <_ch
          bpl     lf8df_00
          stz     <_cl
          stz     <_ch
lf8df_00:
          cla     
          jmp     lfc7f_00
bm_read.impl:
          jsr     lfc32_00
          jsr     lfb96_00
          bcs     lf94b_00
          ldy     #$01
          lda     <_dh
          cmp     [zl2], Y
          bne     lf8f7_00
          lda     <_dl
          cmp     [zl2]
lf8f7_00:
          bcs     lf943_00
          jsr     lfbfc_00
          ldy     #$02
          lda     [zl2], Y
          iny     
          clc     
          adc     <zl1
          sta     <zl1
          lda     [zl2], Y
          adc     <zh1
          ora     <zl1
          pha     
          jsr     lfb46_00
          bmi     lf923_00
          lda     $22b3
          sec     
          sbc     <zl1
          sta     $22b3
          lda     $22b4
          sbc     <zh1
          sta     $22b4
lf923_00:
          lda     $22b3
          sta     <_cl
          lda     $22b4
          sta     <_ch
          ora     $22b3
          beq     lf93d_00
          lda     <_bl
          sta     <zl1
          lda     <_bh
          sta     <zh1
          jsr     lfd17_00
lf93d_00:
          pla     
          bne     lf950_00
          jmp     lfc7f_00
lf943_00:
          stz     <_ch
          stz     <_cl
          cla     
          jmp     lfc7f_00
lf94b_00:
          lda     #$01
          jmp     lfc7f_00
lf950_00:
          lda     #$02
          jmp     lfc7f_00
bm_write.impl:
          jsr     lfc32_00
          jsr     lfb96_00
          bcc     lf965_00
          jsr     lfae2_00
          bcc     lf965_00
          jmp     lfa15_00
lf965_00:
          jsr     lfb46_00
          bpl     lf96d_00
          jmp     lf9f5_00
lf96d_00:
          lda     <zl1
          sta     $22b3
          lda     <zh1
          sta     $22b4
          lda     $8006
          clc     
          adc     $22b3
          sta     <zl1
          lda     $8007
          adc     $22b4
          sta     <zh1
          lda     <zl1
          clc     
          adc     #$02
          sta     <zl0
          lda     <zh1
          adc     #$00
          sta     <zh0
          lda     $8004
          cmp     <zl0
          lda     $8005
          sbc     <zh0
          bcc     lfa15_00
          lda     <zl1
          sta     $8006
          lda     <zh1
          sta     $8007
          ldy     #$01
          lda     [zl2]
          clc     
          adc     <zl2
          sta     <zl1
          sta     <zl0
          lda     [zl2], Y
          adc     <zh2
          sta     <zh1
          sta     <zh0
          ldy     #$01
          lda     $22b3
          clc     
          adc     [zl2]
          sta     [zl2]
          lda     $22b4
          adc     [zl2], Y
          sta     [zl2], Y
          lda     $22b3
          clc     
          adc     <zl1
          sta     <zl1
          lda     $22b4
          adc     <zh1
          sta     <zh1
          lda     $8006
          sec     
          sbc     <zl0
          sta     $22b3
          lda     $8007
          sbc     <zh0
          sta     $22b4
          jsr     lfcd3_00
          jsr     lfb46_00
lf9f5_00:
          lda     <_bl
          sta     <zl1
          lda     <_bh
          sta     <zh1
          jsr     lfd2c_00
          jsr     lfbfc_00
          ldy     #$02
          cla     
          sec     
          sbc     <zl1
          sta     [zl2], Y
          iny     
          cla     
          sbc     <zh1
          sta     [zl2], Y
          cla     
          jmp     lfc7f_00
lfa15_00:
          lda     #$01
          jmp     lfc7f_00
bm_delete.impl:
          jsr     lfc32_00
          jsr     lfb96_00
          bcs     lfa6d_00
          lda     <zl2
          sta     <zl1
          lda     <zh2
          sta     <zh1
          lda     [zl2]
          sta     $22b3
          ldy     #$01
          lda     [zl2], Y
          sta     $22b4
          lda     <zl2
          clc     
          adc     $22b3
          sta     <zl0
          lda     <zh2
          adc     $22b4
          sta     <zh0
          lda     $8006
          sec     
          sbc     <zl0
          sta     $22b3
          lda     $8007
          sbc     <zh0
          sta     $22b4
          lda     $8006
          sec     
          sbc     [zl2]
          sta     $8006
          lda     $8007
          sbc     [zl2], Y
          sta     $8007
          jsr     lfcd3_00
          jmp     lfc7f_00
lfa6d_00:
          lda     #$01
          jmp     lfc7f_00
bm_files.impl:
          jsr     lfc32_00
          lda     #$10
          sta     <zl2
          lda     #$80
          sta     <zh2
          lda     <_al
          sta     $22b3
          stz     <_al
          ldy     #$01
lfa86_00:
          lda     [zl2]
          ora     [zl2], Y
          beq     lfadd_00
          inc     <_al
          dec     $22b3
          beq     lfaa4_00
          lda     [zl2], Y
          pha     
          lda     [zl2]
          clc     
          adc     <zl2
          sta     <zl2
          pla     
          adc     <zh2
          sta     <zh2
          bra     lfa86_00
lfaa4_00:
          lda     [zl2]
          sec     
          sbc     #$10
          tax     
          lda     [zl2], Y
          sbc     #$00
          pha     
          phx     
          lda     <zl2
          clc     
          adc     #$04
          sta     <zl2
          lda     <zh2
          adc     #$00
          sta     <zh2
          lda     <_bl
          sta     <zl1
          lda     <_bh
          sta     <zh1
          jsr     lfd7b_00
          cly     
lfac9_00:
          lda     [zl2], Y
          sta     [zl1], Y
          iny     
          cpy     #$0c
          bcc     lfac9_00
          pla     
          sta     [zl1], Y
          iny     
          pla     
          sta     [zl1], Y
          cla     
          jmp     lfc7f_00
lfadd_00:
          lda     #$01
          jmp     lfc7f_00
lfae2_00:
          lda     $8006
          sta     <zl2
          lda     $8007
          sta     <zh2
          lda     $8004
          sec     
          sbc     <zl2
          sta     <zl0
          lda     $8005
          sbc     <zh2
          sta     <zh0
          lda     <zl1
          cmp     #$10
          lda     <zh1
          sbc     #$00
          bcc     lfb44_00
          lda     <zl2
          clc     
          adc     #$04
          sta     <zl0
          lda     <zh2
          adc     #$00
          sta     <zh0
          lda     <_al
          sta     <zl1
          lda     <_ah
          sta     <zh1
          jsr     lfd7b_00
          cly     
lfb1e_00:
          lda     [zl1], Y
          sta     [zl0], Y
          iny     
          cpy     #$0c
          bne     lfb1e_00
          lda     $8006
          clc     
          adc     #$10
          sta     $8006
          lda     $8007
          adc     #$00
          sta     $8007
          ldy     #$01
          lda     #$10
          sta     [zl2]
          lda     #$00
          sta     [zl2], Y
          clc     
          rts     
lfb44_00:
          sec     
          rts     
lfb46_00:
          lda     <_dl
          clc     
          adc     <_cl
          sta     <zl1
          lda     <_dh
          adc     <_ch
          sta     <zh1
          lda     <zl1
          clc     
          adc     #$10
          sta     <zl1
          lda     <zh1
          adc     #$00
          sta     <zh1
          ldy     #$01
          lda     <zl1
          sec     
          sbc     [zl2]
          sta     <zl1
          lda     <zh1
          sbc     [zl2], Y
          sta     <zh1
          lda     <zl2
          clc     
          adc     #$10
          sta     <zl0
          lda     <zh2
          adc     #$00
          sta     <zh0
          lda     <_dl
          clc     
          adc     <zl0
          sta     <zl0
          lda     <_dh
          adc     <zh0
          sta     <zh0
          lda     <_cl
          sta     $22b3
          lda     <_ch
          sta     $22b4
          lda     <zh1
          rts     
lfb96_00:
          lda     <_al
          sta     <zl1
          lda     <_ah
          sta     <zh1
          jsr     lfd7b_00
          lda     #$10
          sta     <zl2
          lda     #$80
          sta     <zh2
lfba9_00:
          ldy     #$01
          lda     [zl2]
          ora     [zl2], Y
          beq     lfbfa_00
          lda     <zl2
          clc     
          adc     #$04
          sta     <zl2
          lda     <zh2
          adc     #$00
          sta     <zh2
          cly     
lfbbf_00:
          lda     [zl1], Y
          cmp     [zl2], Y
          bne     lfbd9_00
          iny     
          cpy     #$0c
          bcc     lfbbf_00
          lda     <zl2
          sec     
          sbc     #$04
          sta     <zl2
          lda     <zh2
          sbc     #$00
          sta     <zh2
          clc     
          rts     
lfbd9_00:
          lda     <zl2
          sec     
          sbc     #$04
          sta     <zl2
          lda     <zh2
          sbc     #$00
          sta     <zh2
          ldy     #$01
          lda     [zl2], Y
          pha     
          lda     [zl2]
          clc     
          adc     <zl2
          sta     <zl2
          pla     
          adc     <zh2
          sta     <zh2
          jmp     lfba9_00
lfbfa_00:
          sec     
          rts     
lfbfc_00:
          lda     <zl2
          sta     <zl0
          lda     <zh2
          sta     <zh0
          ldy     #$01
          lda     [zl0]
          sec     
          sbc     #$04
          sta     $22b3
          lda     [zl0], Y
          sbc     #$00
          sta     $22b4
          stz     <zl1
          stz     <zh1
          ldy     #$04
lfc1b_00:
          lda     [zl0], Y
          clc     
          adc     <zl1
          sta     <zl1
          cla     
          adc     <zh1
          sta     <zh1
          iny     
          bne     lfc2c_00
          inc     <zh0
lfc2c_00:
          jsr     lfd97_00
          bne     lfc1b_00
          rts     
lfc32_00:
          bsr     lfc49_00
          ldx     #$03
lfc36_00:
          lda     $8000, X
          cmp     unknown.2, X
          bne     lfc42_00
          dex     
          bpl     lfc36_00
          rts     
lfc42_00:
          bsr     lfc7f_00
          pla     
          pla     
          lda     #$ff
          rts     
lfc49_00:
          pha     
          tma     #$00
          sta     $22b5
          tma     #$01
          sta     $22b6
          tma     #$02
          sta     $22b7
          tma     #$03
          sta     $22b8
          tma     #$04
          sta     $22b9
          tma     #$05
          sta     $22ba
          tma     #$06
          sta     $22bb
          tma     #$07
          sta     $22bc
          lda     #$f7
          tam     #$04
          csl     
          sei     
          lda     #$80
          sta     bram_unlock
          pla     
          rts     
lfc7f_00:
          pha     
          lda     $22b5
          tam     #$00
          lda     $22b6
          tam     #$01
          lda     $22b7
          tam     #$02
          lda     $22b8
          tam     #$03
          lda     $22b9
          tam     #$04
          lda     $22ba
          tam     #$05
          lda     $22bb
          tam     #$06
          lda     $22bc
          tam     #$07
          lda     bram_lock
          csh     
          lda     #$05
          sta     video_reg_l
          lda     <crl_m
          ora     #$08
          sta     video_data_l
lfcb8_00:
          lda     video_reg_l
          and     #$20
          bne     lfcb8_00
lfcbf_00:
          lda     video_reg_l
          and     #$20
          beq     lfcbf_00
          lda     <crl_m
          sta     video_data_l
          lda     <reg_box
          sta     video_reg_l
          pla     
          cli     
          rts     
lfcd3_00:
          lda     $22b3
          ora     $22b4
          beq     lfd05_00
          jsr     lfd41_00
          cly     
          txa     
          bne     lfcf5_00
lfce2_00:
          lda     [zl0], Y
          sta     [zl1], Y
          tya     
          bne     lfced_00
          dec     <zh0
          dec     <zh1
lfced_00:
          dey     
          jsr     lfd97_00
          bne     lfce2_00
          beq     lfd05_00
lfcf5_00:
          lda     [zl0], Y
          sta     [zl1], Y
          iny     
          bne     lfd00_00
          inc     <zh0
          inc     <zh1
lfd00_00:
          jsr     lfd97_00
          bne     lfcf5_00
lfd05_00:
          lda     $8006
          sta     <zl0
          lda     $8007
          sta     <zh0
          ldy     #$01
          cla     
          sta     [zl0]
          sta     [zl0], Y
          rts     
lfd17_00:
          jsr     lfd7b_00
          cly     
lfd1b_00:
          lda     [zl0], Y
          sta     [zl1], Y
          iny     
          bne     lfd26_00
          inc     <zh0
          inc     <zh1
lfd26_00:
          jsr     lfd97_00
          bne     lfd1b_00
          rts     
lfd2c_00:
          jsr     lfd7b_00
          cly     
lfd30_00:
          lda     [zl1], Y
          sta     [zl0], Y
          iny     
          bne     lfd3b_00
          inc     <zh0
          inc     <zh1
lfd3b_00:
          jsr     lfd97_00
          bne     lfd30_00
          rts     
lfd41_00:
          lda     <zl0
          sec     
          sbc     <zl1
          lda     <zh0
          sbc     <zh1
          bcs     lfd78_00
          jsr     lfd97_00
          lda     <zl0
          clc     
          adc     $22b3
          sta     <zl0
          lda     <zh0
          adc     $22b4
          sta     <zh0
          lda     <zl1
          clc     
          adc     $22b3
          sta     <zl1
          lda     <zh1
          adc     $22b4
          sta     <zh1
          inc     $22b3
          bne     lfd75_00
          inc     $22b4
lfd75_00:
          ldx     #$00
          rts     
lfd78_00:
          ldx     #$02
          rts     
lfd7b_00:
          lda     <zh1
          rol     A
          rol     A
          rol     A
          rol     A
          and     #$07
          tax     
          lda     $22b5, X
          tam     #$05
          lda     $22b6, X
          tam     #$06
          lda     <zh1
          and     #$1f
          ora     #$a0
          sta     <zh1
          rts     
lfd97_00:
          lda     $22b3
          bne     lfd9f_00
          dec     $22b4
lfd9f_00:
          dec     $22b3
          lda     $22b3
          ora     $22b4
          rts     
lfda9_00:
          pha     
          tma     #$06
          sta     $22bb
          lda     #$03
          tam     #$06
          pla     
          rts     
lfdb5_00:
          php     
          pha     
          lda     $22bb
          tam     #$06
          pla     
          plp     
          rts     
ma_mul8s.impl:
          bsr     lfda9_00
          jsr     $cd10
          bra     lfdb5_00
ma_mul8u.impl:
          bsr     lfda9_00
          jsr     $cd3a
          bra     lfdb5_00
ma_mul16s.impl:
          bsr     lfda9_00
          jsr     $cd57
          bra     lfdb5_00
ma_mul16u.impl:
          bsr     lfda9_00
          jsr     $cd92
          bra     lfdb5_00
ma_cbasis.impl:
          bsr     lfda9_00
          jsr     $cdc3
          bra     lfdb5_00
ma_div16u.impl:
          bsr     lfda9_00
          jsr     $cddb
          bra     lfdb5_00
ma_div16s.impl:
          bsr     lfda9_00
          jsr     $ce2e
          bra     lfdb5_00
ma_sqrt.impl:
          bsr     lfda9_00
          jsr     $ce75
          bra     lfdb5_00
ma_cos.impl:
          bsr     lfda9_00
          jsr     $cec3
          bra     lfdb5_00
ma_sin.impl:
          bsr     lfda9_00
          jsr     $cec8
          bra     lfdb5_00
ma_atni.impl:
          bsr     lfda9_00
          jsr     $ced2
          bra     lfdb5_00
psg_bios.impl:
          phx     
          phy     
          tma     #$06
          pha     
          lda     #$02
          tam     #$06
          lda     <_dh
          cmp     #$15
          bcs     lfe22_00
          asl     A
          tax     
          lda     <_al
          jsr     lfe2a_00
lfe22_00:
          tay     
          pla     
          tam     #$06
          tya     
          ply     
          plx     
          rts     
lfe2a_00:
          jmp     [jump_table.0, X]

	.data
	.bank 0
	.org $fe2d
jump_table.0:
    .db $00,$c0,$19,$c0,$43,$c0,$db,$c0
    .db $e6,$c0,$f1,$c0,$fc,$c0,$07,$c1
    .db $12,$c1,$1d,$c1,$28,$c1,$42,$c1
    .db $51,$c1,$6c,$c1,$85,$c1,$ca,$c1
    .db $21,$c2,$4a,$c2,$7c,$c2,$cd,$c2
    .db $e9,$c2

	.code
	.bank 0
	.org $fe57
grp_bios.impl:
          tma     #$06
          pha     
          lda     #$03
          tam     #$06
          lda     <_dh
          cmp     #$11
          bcs     lfe69_00
          asl     A
          tax     
          jsr     lfe6d_00
lfe69_00:
          pla     
          tam     #$06
          rts     
lfe6d_00:
          jmp     [jump_table.1, X]

	.data
	.bank 0
	.org $fe70
jump_table.1:
    .db $f3,$ca,$07,$cb,$ed,$cb,$b2,$ca
    .db $29,$c5,$a8,$c1,$a2,$c2,$85,$c0
    .db $3c,$c0,$67,$c0,$90,$c3,$48,$c4
    .db $14,$cb,$0e,$cb,$f4,$c7,$f0,$c6
    .db $71,$cc

	.code
	.bank 0
	.org $fe92
ex_memopen.impl:
          lda     $18c5
          cmp     #$aa
          bne     lfea5_00
          lda     $18c6
          cmp     #$55
          bne     lfea5_00
          ldx     $18c7
          bra     lfeba_00
lfea5_00:
          lda     $18c1
          cmp     #$aa
          bne     lfebf_00
          lda     $18c2
          cmp     #$55
          bne     lfebf_00
          ldx     $18c3
          sax     
          ora     #$80
          sax     
lfeba_00:
          lda     $fff4
          clc     
          rts     
lfebf_00:
          clx     
          sec     
          rts     
	.data
	.bank 0
	.org $fec2
unknown.3:
    .db $03,$00,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$ff,$ff,$ff,$ff,$ff,$ff
    .db $ff,$ff,$4c,$00,$e9,$4c,$3b,$ea
    .db $4c,$79,$ea,$4c,$be,$ea,$4c,$16
    .db $eb,$4c,$5e,$eb,$4c,$c5,$e9,$4c
    .db $dc,$e9,$68,$80

irq_vectors:
    .db $36,$e7,$70,$e8,$b3,$e6,$a9,$e6
    .db $f3,$e0
