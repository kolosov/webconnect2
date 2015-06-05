It is deeply modification of webconnect 1.1 project(please read old webconnect 
readme_1.1.txt)

Improvements:
 - supports gecko from 1.9.0 to 10 (gecko 10 ask some additional workaround 
with mozalloc library, under windows Gecko 10 build requires a mozutils.dll
 copy to root binary directory).
 - cmake build system
 - support Linux (32bit and 64bit) and Windows build

Limitations:
 - I used webconnect only for base embedding, for example, working with 
DOM objects may not work
 - needs Gecko SDK for build

Webconnect2 requires gecko sdk for compilation and uses cmake based build
 system. It is works both under linux and windows.

Steps for windows building:

1) mkdir build_w2 && cd build_w2
2) cmake ../webconnect2 -G "Visual Studio 9 2008" \ 
-DGECKO_SDK_PATH=c:/dev/xulrunner-sdk_7 -DwxWidgets_ROOT_DIR=c:/wxmsw-2.8 
(or  cmake ../webconnect2  
-DGECKO_SDK_PATH=/home/sk/work/src/build/sdk/xulrunner-9.0/lib/xulrunner-devel-9.0 
-DECLIPSE_CDT4_GENERATE_SOURCE_PROJECT=TRUE -G"Eclipse CDT4 - Unix Makefiles" for Linux)
3) open webconnect2.sln by Visual Studio
4) Put to directory gecko runtime (you can get gecko runtime by gecko sdk from bin/ 
directory) to directory build_w2/Debug/xr
5) Run testapp.exe

MSC_VER		MS VSC++
1600		2010

xulrunner	MSC_VER
 31		1600
 34		1600
 36		1800


Steps for linux building:

1) mkdir build_w2 && cd build_w2
2) cmake ../webconnect2 \-DGECKO_SDK_PATH=/home/sk/work/src/build/\
xulrunner-dev-9.0.1/lib/xulrunner-devel-9.0.1/ -G"Eclipse CDT4 - Unix Makefiles"
3) make
4) Put to directory gecko runtime (you can get gecko runtime by gecko sdk from 
bin/ directory, cp -r /home/sk/work/src/build/xulrunner-dev-9.0.1/lib/\
xulrunner-devel-9.0.1/bin xr)
5) testapp/testapp

