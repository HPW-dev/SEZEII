set batch_path=%~dp0
set /a rand_name=%random% %% 100000
set opts=--tv_type color --resolution_type UMD ^
--filter_power 1 --filter_power_u 9 --filter_power_v 9 ^
--interlace --noise_level 0.008 --noise_level_u 0.2 --noise_level_v 0.2 ^
--power 1 --power_u 1.1 --power_v 1.1 --interlace ^
--ringing_power 0.5 --ringing_type right --ringing_ratio 0.2 --ringing_length 16
"%batch_path%..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\plugins\windows\free\tvsim.dll" ^
--opts "%opts%"