set batch_path=%~dp0
set /a rand_name=%random% %% 100000
set opts= -w 238 -h 344 --desat_type 0 --scale_type_in 3 --scale_type_out 3 --hfront 35 --hback 21 --hsync_sz 300 --vfront 120 --vback 80 --vsync_sz 6571 --vsync_needed_cnt 1000 --beam_spd_x 1 --beam_spd_y 1 --beam_spd_back 10 --white_lvl 1.7 --black_lvl 0 --beam_off_signal -0.1 --sync_lvl -0.24 --sync_signal -0.56 --fading 0.1 --pre_amp 1 --amp 1.132 --filter_type 1 --filter_power 2 --noise_level 0.3 --am_freg 1 --am_depth 1 --am_tune 1 --fix_opts 1 --interlacing 1 --use_fading 1 --use_scale 1 --use_am 0 --debug 0 --debug_black_bg 0  --shift_u 0 --shift_v 0 --filter_power_uv 6 --noise_uv 0.116 --amp_u 1.203 --amp_v 0.919
"%batch_path%..\..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\..\plugins\windows\free\tvsim2yuv.dll" ^
--opts "%opts%"
