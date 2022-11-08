set batch_path=%~dp0
set /a rand_name=%random% %% 100000
"%batch_path%..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\plugins\\windows\\free\\stars.dll" ^
--opts "-l 32 -p 1.666 -t 225 -m avr -r -s 1 --rsize"