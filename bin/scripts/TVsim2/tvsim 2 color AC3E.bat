set batch_path=%~dp0
set /a rand_name=%random% %% 100000
set opts= -w 408 -h 414 --desat_type 0 --scale_type_in 3 --scale_type_out 2 --hfront 55 --hback 22 --hsync_sz 300 --vfront 120 --vback 80 --vsync_sz 6571 --vsync_needed_cnt 1000 --beam_spd_x 1 --beam_spd_y 0.94 --beam_spd_back 10 --white_lvl 1 --black_lvl 0 --beam_off_signal -0.1 --sync_lvl -0.43 --sync_signal -0.5 --fading 0.24 --pre_amp 1 --amp 1 --filter_type 1 --filter_power 3 --noise_level 0.07 --am_freg 1.00612 --am_depth 1.1765 --am_tune 1.113 --sharp_power 0 --fix_opts 1 --interlacing 0 --use_fading 1 --use_scale 1 --use_am 0 --debug 0 --debug_black_bg 0  --shift_u 3 --shift_v -1 --filter_power_uv 6 --noise_uv 0.081 --amp_u 1.199 --amp_v 1.375
"%batch_path%..\..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\..\plugins\windows\free\tvsim2yuv.dll" ^
--opts "%opts%"
