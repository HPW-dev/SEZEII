set batch_path=%~dp0
set /a rand_name=%random% %% 100000
"%batch_path%..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\plugins\\windows\\free\\tvsim.dll" ^
--opts "--tv_type bw --fading 0.004 --first_bound 1 --second_bound 1 --hsync_size 20 --vsync_size 100 --first_strings 40 --second_strings 30 --noise_level 0 --beam_reverse_speed 9000 --power 1 --signal_shift 0 --filter_power 0 --filter_power_u 0 --filter_power_v 0 --resolution_type RAW"