It is webconnect2 based on webconnect 1.1 project(please read readme_1.1.txt)

Webconnect2 supports gecko from 1.9.0 to 10 versions. Under windows Gecko 10 build requires
a copy of mozutils.dll to root binary directory.

Webconnect2 requires gecko sdk for compilation and uses cmake based build system.
It is works both under linux and windows.
Steps for windows buildung:

1) mkdir build_w2 && cd build_w2
2) cmake ../webconnect2 -G "Visual Studio 9 2008" \ 
-DGECKO_SDK_PATH=c:/dev/xulrunner-sdk_7 -DwxWidgets_ROOT_DIR=c:/wxmsw-2.8 (or  cmake ../webconnect2 -DGECKO_SDK_PATH=/home/sk/work/src/build/sdk/xulrunner-9.0/lib/xulrunner-devel-9.0 -DECLIPSE_CDT4_GENERATE_SOURCE_PROJECT=TRUE -G"Eclipse CDT4 - Unix Makefiles" for Linux)
3) open webconnect2.sln by Visual Studio
4) Put to directory gecko runtime (you can get gecko runtime by gecko sdk from bin/ directory)
 to directory build_w2/Debug/xr
5) Run testapp.exe

