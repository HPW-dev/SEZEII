set batch_path=%~dp0
set launch_path=%cd%
copy "%batch_path%\imgui-bak.ini" "%launch_path%\imgui.ini"
"%batch_path%..\..\tvsim2-conf.exe" -i %1
del "%launch_path%\imgui.ini"
