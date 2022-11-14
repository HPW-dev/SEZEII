set batch_path=%~dp0
set /a rand_name=%random% %% 100000
set opts=-w 364 -h 266 --desat_type 0 --scale_type_in 0 --scale_type_out 3 --hfront 35 --hback 21 --hsync_sz 280 --vfront 120 --vback 80 --vsync_sz 6571 --vsync_needed_cnt 1000 --beam_spd_x 1 --beam_spd_y 1 --beam_spd_back 4 --white_lvl 0.68 --black_lvl 0 --beam_off_signal -0.1 --sync_lvl -0.44 --sync_signal -0.58 --fading 0.12 --pre_amp 1.041 --amp 1.332 --filter_type 2 --filter_power 2 --noise_level 0.033 --am_freg 1.5811 --am_depth 1.102 --am_tune 0.8935 --fix_opts 1 --interlacing 1 --use_fading 1 --use_scale 1 --use_am 1 --debug 0 --debug_black_bg 0  --shift_u 0 --shift_v 0 --filter_power_uv 37 --noise_uv 0.311 --amp_u 1.521 --amp_v 1.166
"%batch_path%..\..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\..\plugins\windows\free\tvsim2yuv.dll" ^
--opts "%opts%"
