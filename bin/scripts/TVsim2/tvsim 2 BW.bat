set batch_path=%~dp0
set /a rand_name=%random% %% 100000
set opts= -w 468 -h 388 --desat_type 0 --scale_type_in 2 --scale_type_out 2 --hfront 0 --hback 0 --hsync_sz 60 --vfront 0 --vback 0 --vsync_sz 270 --vsync_needed_cnt 106 --beam_spd_x 1 --beam_spd_y 1 --beam_spd_back 10 --white_lvl 1 --black_lvl 0 --beam_off_signal -0.1 --sync_lvl -0.27 --sync_signal -0.5 --fading 0.15 --pre_amp 1 --amp 1 --filter_type 2 --filter_power 4 --noise_level 0.181 --am_freg 1 --am_depth 1 --am_tune 1 --fix_opts 1 --interlacing 1 --use_fading 1 --use_scale 1 --use_am 0 --debug 0 --debug_black_bg 0
"%batch_path%..\..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\..\plugins\windows\free\tvsim2bw.dll" ^
--opts "%opts%"
