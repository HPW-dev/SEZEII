set batch_path=%~dp0
set /a rand_name=%random% %% 100000
set opts=-w 214 -h 192 --desat_type 1 --scale_type_in 2 --scale_type_out 2 --hfront 0 --hback 0 --hsync_sz 300 --vfront 0 --vback 0 --vsync_sz 6571 --vsync_needed_cnt 1000 --beam_spd_x 1 --beam_spd_y 1 --beam_spd_back 10 --white_lvl 2 --black_lvl -0.96 --beam_off_signal -0.96 --sync_lvl -0.96 --sync_signal -1.15 --fading 0.02 --pre_amp 0.807 --amp 0.976 --filter_type 1 --filter_power 2 --noise_level 0 --am_freg 0.99627 --am_depth 1.0775 --am_tune 1 --fix_opts --interlacing --use_fading --use_scale --use_am
"%batch_path%..\..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\..\plugins\windows\free\tvsim2bw.dll" ^
--opts "%opts%"
