set batch_path=%~dp0
set /a rand_name=%random% %% 100000
set opts=--tv_type color --resolution_type RAW ^
--filter_power 2 --filter_power_u 4 --filter_power_v 12 ^
--interlace --noise_level 0.007 --noise_level_u 0.15 --noise_level_v 0.25 ^
--power 1.0 --power_u 1.15 --power_v 1.15 --interlace ^
--ringing_power 0.2 --ringing_type right --ringing_ratio 1.1 --ringing_length 16
"%batch_path%..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\plugins\windows\free\tvsim.dll" ^
--opts "%opts%"