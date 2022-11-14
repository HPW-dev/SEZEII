set batch_path=%~dp0
set /a rand_name=%random% %% 100000
set opts=-w 334 -h 250 --desat_type 0 --scale_type_in 2 --scale_type_out 2 --hfront 0 --hback 0 --hsync_sz 100 --vfront 0 --vback 0 --vsync_sz 2000 --vsync_needed_cnt 1000 --beam_spd_x 1 --beam_spd_y 1 --beam_spd_back 4 --white_lvl 0.68 --black_lvl 0 --beam_off_signal -0.1 --sync_lvl -0.44 --sync_signal -0.58 --fading 0.12 --pre_amp 0.948 --amp 1.332 --filter_type 2 --filter_power 3 --noise_level 0.034 --am_freg 1.5811 --am_depth 1.102 --am_tune 0.8935 --fix_opts 1 --interlacing 0 --use_fading 1 --use_scale 1 --use_am 0 --debug 0 --debug_black_bg 0  --shift_u -13 --shift_v 2 --filter_power_uv 26 --noise_uv 0.201 --amp_u 1.807 --amp_v 1.224
"%batch_path%..\..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\..\plugins\windows\free\tvsim2yuv.dll" ^
--opts "%opts%"
