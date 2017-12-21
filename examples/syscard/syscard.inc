video_reg_l = $0000
video_reg_h = $0001
video_data_l = $0002
video_data_h = $0003
color_ctrl = $0400
color_reg_l = $0402
color_reg_h = $0403
color_data_l = $0404
color_data_h = $0405
psg_ch = $0800
psg_mainvol = $0801
psg_freq.lo = $0802
psg_freq.hi = $0803
psg_ctrl = $0804
psg_pan = $0805
psg_wavebuf = $0806
psg_noise = $0807
psg_lfoctrl = $0809
psg_lfofreq = $0808
timer_cnt = $0C00
timer_ctrl = $0C01
joyport = $1000
irqport = $1400
irq_disable = $1402
irq_status = $1403
cd_port = $1800
bram_lock = $1803
bram_unlock = $1807
vi_bitpat = $20dc
vi_rvbitpat = $20dd
vi_padrs = $20de

vi_porg = $20e0
vi_ft_back = $20e1

vi_stack = $20e4

time_sw = $20e6
main_sw = $20e7
si = $20e8

r0 = $20ea

zl0 = $20ec
zh0 = $20ed
zl1 = $20ee
zh1 = $20ef
zl2 = $20f0
zh2 = $20f1
cdi_b = $20f2
crl_m = $20f3
crh_m = $20f4
irq_m = $20f5
str_b = $20f6
reg_box = $20f7
_al = $20f8
_ah = $20f9
_bl = $20fa
_bh = $20fb
_cl = $20fc
_ch = $20fd
_dl = $20fe
_dh = $20ff
irq2_jmp = $2200

irq_jmp = $2202

tim_jmp = $2204

nmi_jmp = $2206

sync_jmp = $2208

rcr_jmp = $220a

bgx1 = $220c

bgx2 = $220e

bgy1 = $2210

bgy2 = $2212

sat_addr = $2214

spr_ptr = $2216
spr_yl = $2217
spr_yh = $2218
spr_xl = $2219
spr_xh = $221a
spr_nl = $221b
spr_nh = $221c
spr_al = $221d
spr_ah = $221e
color_cmd = $221f
bgc_ptr = $2220

bgc_len = $2222
sprc_ptr = $2223

sprc_len = $2225
joykeyflg = $2226
joyena = $2227
joy = $2228
joytrg = $222d
joyold = $2232
irq_cnt = $2241
notrdyflg = $2247
rndl = $2249
rndh = $224a
rndm = $224b
tnomin = $226a
outmin = $226c
outsec = $226d
outfrm = $226e
vdtin_flg = $2272
recbase0_h = $2274
recbase0_m = $2275
recbase0_l = $2276
recbase1_h = $2277
recbase1_m = $2278
recbase1_l = $2279
scsists = $227b
suberrc = $227c
sibcode = $227e
psg_work_top = $22d0
graph_work_top = $2616
key_work_top = $2649
user_work_top = $267f
