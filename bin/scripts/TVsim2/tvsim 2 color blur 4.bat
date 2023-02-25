set batch_path=%~dp0
set /a rand_name=%random% %% 100000
set opts= -w 540 -h 280 --desat_type 0 --scale_type_in 3 --scale_type_out 3 --hfront 0 --hback 0 --hsync_sz 200 --vfront 0 --vback 0 --vsync_sz 1000 --vsync_needed_cnt 201 --beam_spd_x 1 --beam_spd_y 1 --beam_spd_back 10 --white_lvl 2 --black_lvl -0.54 --beam_off_signal -0.54 --sync_lvl -0.91 --sync_signal -2 --fading 0.2 --pre_amp 1 --amp 1 --filter_type 1 --filter_power 0 --noise_level 0 --am_freg 1 --am_depth 1 --am_tune 1 --fix_opts 1 --interlacing 0 --use_fading 0 --use_scale 0 --use_am 0 --debug 0 --debug_black_bg 0  --shift_u 0 --shift_v 0 --filter_power_uv 23 --noise_uv 0.228 --amp_u 1 --amp_v 1
"%batch_path%..\..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\..\plugins\windows\free\tvsim2yuv.dll" ^
--opts "%opts%"
