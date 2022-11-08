set batch_path=%~dp0
set /a rand_name=%random% %% 100000
set opts= -w 469 -h 282 --desat_type 1 --scale_type_in 2 --scale_type_out 3 --hfront 0 --hback 0 --hsync_sz 71 --vfront 0 --vback 0 --vsync_sz 6571 --vsync_needed_cnt 1000 --beam_spd_x 1 --beam_spd_y 1 --beam_spd_back 10 --white_lvl 1.18 --black_lvl 0 --beam_off_signal 0 --sync_lvl -0.43 --sync_signal -1.27 --fading 0.3 --pre_amp 1 --amp 1.092 --filter_type 2 --filter_power 3 --noise_level 0 --am_freg 1.00052 --am_depth 1.0875 --am_tune 1 --fix_opts 1 --interlacing 1 --use_fading 1 --use_scale 1 --use_am 1
"%batch_path%..\..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\..\plugins\windows\free\tvsim2bw.dll" ^
--opts "%opts%"
