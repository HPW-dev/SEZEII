set batch_path=%~dp0
set /a rand_name=%random% %% 100000
set opts= -w 198 -h 223 --desat_type 0 --scale_type_in 3 --scale_type_out 1 --hfront 0 --hback 0 --hsync_sz 151 --vfront 0 --vback 0 --vsync_sz 6571 --vsync_needed_cnt 1000 --beam_spd_x 1 --beam_spd_y 1 --beam_spd_back 10 --white_lvl 1 --black_lvl 0 --beam_off_signal -0.1 --sync_lvl -0.26 --sync_signal -0.5 --fading 0.1 --pre_amp 0.939 --amp 0.966 --filter_type 2 --filter_power 0 --noise_level 0.028 --am_freg 1 --am_depth 1 --am_tune 1 --fix_opts 1 --interlacing 0 --use_fading 0 --use_scale 1 --use_am 0 --debug 0 --debug_black_bg 0  --shift_u 0 --shift_v 0 --filter_power_uv 4 --noise_uv 0.084 --amp_u 1.001 --amp_v 0.96
"%batch_path%..\..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\..\plugins\windows\free\tvsim2yuv.dll" ^
--opts "%opts%"
