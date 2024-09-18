set batch_path=%~dp0
set /a rand_name=%random% %% 100000
set opts= -w 320 -h 502 --desat_type 5 --scale_type_in 2 --scale_type_out 2 --hfront 35 --hback 21 --hsync_sz 252 --vfront 120 --vback 80 --vsync_sz 7000 --vsync_needed_cnt 1932 --beam_spd_x 1 --beam_spd_y 1 --beam_spd_back 100 --white_lvl 2 --black_lvl 0 --beam_off_signal -0.04 --sync_lvl -0.83 --sync_signal -2 --fading 0.211 --pre_amp 1.098 --amp 0.962 --filter_type 1 --filter_power 2 --noise_level 0.125 --am_freg 1 --am_depth 1 --am_tune 1 --sharp_power 0.6 --fix_opts 1 --interlacing 1 --use_fading 1 --use_scale 1 --use_am 0 --debug 0 --debug_black_bg 0  --shift_u 1 --shift_v -1 --filter_power_uv 4 --noise_uv 0.26 --amp_u 1.067 --amp_v 0.936
"%batch_path%..\..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\..\plugins\windows\free\tvsim2yuv.dll" ^
--opts "%opts%"
