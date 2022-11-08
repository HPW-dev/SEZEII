set batch_path=%~dp0
set /a rand_name=%random% %% 100000
set opts=--tv_type bw --resolution_type CD-NTSC ^
--filter_power 0 --noise_level 0 --power 1 --ringing_power 0.5 ^
--ringing_type right --ringing_ratio 1.2 --ringing_length 16
"%batch_path%..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\plugins\\windows\\free\\tvsim.dll" ^
--opts "%opts%"