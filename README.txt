It is webconnect2 based on webconnect 1.1 project(please read readme_1.1.txt)

webconnect2 ask gecko sdk for compilation and uses cmake based build system.
It is works both under linux and windows. Under windows I use wxwidget compiled 
without UNICODE support because there are some promlem seems linked with WCHAR.
Steps for windows buildung:

1) mkdir build_w2
2) cd build_w2
3) cmake ../webconnect2 -G "Visual Studio 9 2008" \ 
-DGECKO_SDK_PATH=c:/dev/xulrunner-sdk_7 -DwxWidgets_ROOT_DIR=c:/wxmsw-2.8
4) open webconnect2.sln by Visual Studio
5) Select testapp properies and switch Configuration properties/Linker/System/SubSystem from "Console" to "Windows"
6) Put to directory gecko runtime (you can get gecko runtime by gecko sdk from bin/ directory)
 to directory build_w2/Debug/xr
7) Run testapp.exe

