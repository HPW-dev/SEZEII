# Как скомпилировать SEZE II
+ Установите [Python3](https://www.python.org/downloads/) и [Scons](https://scons.org/doc/production/HTML/scons-user/ch01s02.html).
+ Для линукса установти dev-пакеты FFmpeg и SDL2.
+ Скачайте dev-релиз SEZE с github.

execute this commands in project root:\
+ For build SEZE II: ```scons .```
+ For build free plugins (output in ./bin/plugins/): ```scons . -Q build_plugins=1```
+ For debug & watch ffmpeg debug log:
```
scons -Q debug=0 -Q build_plugins=0 -Q fflog=0
scons -Q debug=0 -Q build_plugins=1
```
### Как скомпилировать seze-gui
Install [pyinstaller](https://www.pyinstaller.org/) for build seze-gui and execute:
```
mkdir buid_gui
cd buid_gui
pyinstaller --onefile -c ../src/seze-gui.py -i ../resources/SEZEII.ico
```
release files located in ```dist/``` directory
