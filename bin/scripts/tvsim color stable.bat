set batch_path=%~dp0
set /a rand_name=%random% %% 100000
set opts=--tv_type color --resolution_type CD-NTSC ^
--filter_power 2 --filter_power_u 6 --filter_power_v 6 ^
--interlace --noise_level 0.009 --noise_level_u 0.1 --noise_level_v 0.1 ^
--power 1.1 --power_u 1.1 --power_v 1.1 --interlace
"%batch_path%..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\plugins\\windows\\free\\tvsim.dll" ^
--opts "%opts%"