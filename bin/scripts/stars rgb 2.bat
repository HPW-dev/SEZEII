set batch_path=%~dp0
set /a rand_name=%random% %% 100000
"%batch_path%..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\plugins\\windows\\free\\stars.dll" ^
--opts "-l 6 -p 1 -t 128 -r -s 6 -m rgb"