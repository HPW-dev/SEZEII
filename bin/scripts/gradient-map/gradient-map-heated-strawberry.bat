set batch_path=%~dp0
set /a rand_name=%random% %% 100000
"%batch_path%..\\..\seze.exe" -i %1 -o %1-%rand_name%.avi ^
--plug "%batch_path%..\\..\plugins\\windows\\free\\gradient_map.dll" ^
--opts "-p "%batch_path%..\\..\\palettes\\gradient-map\\heated-strawberry.png""