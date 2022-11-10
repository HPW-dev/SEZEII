set batch_path=%~dp0
set /a rand_name=%random% %% 100000
set opts= -w 198 -h 199 --desat_type 0 --scale_type_in 3 --scale_type_out 1 --hfront 0 --hback 0 --hsync_sz 151 --vfront 0 --vback 0 --vsync_sz 6571 --vsync_needed_cnt 1000 --beam_spd_x 1 --beam_spd_y 1 --beam_spd_back 10 --white_lvl 1 --black_lvl 0 --beam_off_signal -0.1 --sync_lvl -0.26 --sync_signal -0.5 --fading 0.1 --pre_amp 1.012 --amp 1.203 --filter_type 1 --filter_power 2 --noise_level 0.048 --am_freg 1.56174 --am_depth 1.0945 --am_tune 1 --fix_opts 1 --interlacing 1 --use_fading 1 --use_scale 1 --use_am 1 --debug 0 --debug_black_bg 0  --shift_u 0 --shift_v 0 --filter_power_uv 11 --noise_uv 0.145 --amp_u 1.383 --amp_v 1.033
"%batch_path%..\..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\..\plugins\windows\free\tvsim2yuv.dll" ^
--opts "%opts%"
