### How to build SEZE II
+ Install [Python3](https://www.python.org/downloads/) and [Scons](https://scons.org/doc/production/HTML/scons-user/ch01s02.html).
+ For Linux install FFmpeg and SDL2 dev-packages.
+ Download SEZE dev. release from github .

execute this commands in project root:
+ For build SEZE II: ```scons .```
+ For build free plugins (output in ./bin/plugins/): ```scons . -Q build_plugins=1```
+ For debug & watch ffmpeg debug log:
```
scons -Q debug=0 -Q build_plugins=0 -Q fflog=0
scons -Q debug=0 -Q build_plugins=1
```
### How to build seze-gui
Install [pyinstaller](https://www.pyinstaller.org/) and execute:
```
mkdir buid_gui
cd buid_gui
pyinstaller --onefile -c ../src/seze-gui.py -i ../resources/SEZEII.ico
```
Release files located in ```dist/``` directory
