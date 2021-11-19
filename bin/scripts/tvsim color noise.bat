set batch_path=%~dp0
set /a rand_name=%random% %% 100000
set opts=--tv_type color --resolution_type CD-NTSC ^
--filter_power 6 --filter_power_u 8 --filter_power_v 16 ^
--interlace --noise_level 0.08 --noise_level_u 0.2 --noise_level_v 0.2 ^
--power 1 --power_u 1.2 --power_v 1.2 ^
--signal_shift -0.035 --hbound 12 --beam_reverse_speed 4.7 ^
--fading 0.1 --white_level 1 --black_level 0.11 ^
--blank_level 0 --sync_level -0.4
"%batch_path%..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\plugins\\windows\\free\\tvsim.dll" ^
--opts "%opts%"