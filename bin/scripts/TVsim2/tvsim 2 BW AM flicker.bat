set batch_path=%~dp0
set /a rand_name=%random% %% 100000
set opts=-w 548 -h 314 --desat_type 0 --scale_type_in 2 --scale_type_out 2 --hfront 0 --hback 0 --hsync_sz 117 --vfront 0 --vback 0 --vsync_sz 738 --vsync_needed_cnt 313 --beam_spd_x 1 --beam_spd_y 1 --beam_spd_back 10 --white_lvl 1 --black_lvl 0 --beam_off_signal -0.1 --sync_lvl -0.27 --sync_signal -0.93 --fading 0.5 --pre_amp 1 --amp 1 --filter_type 1 --filter_power 4 --noise_level 0 --am_freg 1.55 --am_depth 1.2835 --am_tune 1 --fix_opts 1 --interlacing 1 --use_fading 1 --use_scale 1 --use_am 1 --debug 0 --debug_black_bg 0
"%batch_path%..\..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\..\plugins\windows\free\tvsim2bw.dll" ^
--opts "%opts%"
