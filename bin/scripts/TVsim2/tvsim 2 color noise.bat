set batch_path=%~dp0
set /a rand_name=%random% %% 100000
set opts= -w 198 -h 256 --desat_type 0 --scale_type_in 4 --scale_type_out 4 --hfront 0 --hback 0 --hsync_sz 25 --vfront 0 --vback 0 --vsync_sz 124 --vsync_needed_cnt 103 --beam_spd_x 1 --beam_spd_y 1 --beam_spd_back 13 --white_lvl 2 --black_lvl -0.75 --beam_off_signal -0.75 --sync_lvl -0.8 --sync_signal -1.25 --fading 0.15 --pre_amp 1 --amp 1.072 --filter_type 3 --filter_power 3 --noise_level 0.432 --am_freg 1.10606 --am_depth 1.2225 --am_tune 0.9305 --fix_opts 1 --interlacing 0 --use_fading 1 --use_scale 1 --use_am 0 --debug 0 --debug_black_bg 0  --shift_u 0 --shift_v 0 --filter_power_uv 3 --noise_uv 0.137 --amp_u 1 --amp_v 1
"%batch_path%..\..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\..\plugins\windows\free\tvsim2yuv.dll" ^
--opts "%opts%"
