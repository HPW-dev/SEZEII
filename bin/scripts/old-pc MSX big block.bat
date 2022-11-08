set batch_path=%~dp0
set /a rand_name=%random% %% 100000
"%batch_path%..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\plugins\\windows\\free\\old pc.dll" ^
--opts "-w 16 -h 16 -d 16x16 -v 0.3 -p "%batch_path%..\palettes\msx.txt""