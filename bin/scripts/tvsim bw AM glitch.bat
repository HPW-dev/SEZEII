set batch_path=%~dp0
set /a rand_name=%random% %% 100000
set AM=0.00003
set opts=--tv_type bw --resolution_type CD-NTSC ^
--filter_power 3 ^
--interlace --noise_level 0.001 --power 1 ^
--enable_AM --fading 0.1 ^
--AM_ratio_in %AM% ^
--AM_ratio_in_u %AM% ^
--AM_ratio_in_v %AM% ^
--AM_ratio_out %AM% ^
--AM_ratio_out_u %AM% ^
--AM_ratio_out_v %AM%
"%batch_path%..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\plugins\\windows\\free\\tvsim.dll" ^
--opts "%opts%"