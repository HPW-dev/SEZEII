set batch_path=%~dp0
set /a rand_name=%random% %% 100000
set opts=--tv_type color --resolution_type NTSC ^
--filter_power 1 --filter_type accurate --filter_power_u 16 --filter_power_v 16 ^
--noise_level 0 --noise_level_u 0.5 --noise_level_v 0.5 ^
--power 1 --power_u 1.2 --power_v 1.2 --interlace --fading 0.0003
"%batch_path%..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\plugins\\windows\\free\\tvsim.dll" ^
--opts "%opts%"