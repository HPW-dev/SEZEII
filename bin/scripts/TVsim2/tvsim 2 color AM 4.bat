set batch_path=%~dp0
set /a rand_name=%random% %% 100000
set opts= -w 388 -h 328 --desat_type 0 --scale_type_in 2 --scale_type_out 2 --hfront 0 --hback 0 --hsync_sz 100 --vfront 0 --vback 0 --vsync_sz 500 --vsync_needed_cnt 300 --beam_spd_x 1 --beam_spd_y 1 --beam_spd_back 10 --white_lvl 1 --black_lvl 0 --beam_off_signal -0.1 --sync_lvl -0.43 --sync_signal -0.5 --fading 0.15 --pre_amp 1 --amp 1 --filter_type 2 --filter_power 2 --noise_level 0.058 --am_freg 1.50303 --am_depth 1.392 --am_tune 0.899 --fix_opts 1 --interlacing 1 --use_fading 1 --use_scale 1 --use_am 1 --debug 0 --debug_black_bg 0  --shift_u 29 --shift_v -6 --filter_power_uv 55 --noise_uv 0.39 --amp_u 1.228 --amp_v 1.253
"%batch_path%..\..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\..\plugins\windows\free\tvsim2yuv.dll" ^
--opts "%opts%"
