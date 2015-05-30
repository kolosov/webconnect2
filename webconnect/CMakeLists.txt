#SET(wxWidgets_USE_LIBS base core aui)
find_package(wxWidgets REQUIRED base core aui)
include(${wxWidgets_USE_FILE})

if(CMAKE_SYSTEM_NAME MATCHES Linux)
  add_definitions(-fshort-wchar -fno-rtti -std=c++11)
endif()

#if(CMAKE_SYSTEM_NAME MATCHES Linux)
# add_definitions(-g -std=c++0x -fno-rtti -DXPCOM_GLUE_USE_NSPR -DXPCOM_GLUE=1)
#endif()

if(CMAKE_SYSTEM_NAME MATCHES Windows)
 add_definitions(-DXPCOM_GLUE_USE_NSPR -DXPCOM_GLUE=1 -DXP_WIN=1)
endif()

#set nsall header
include_directories(${GECKO_SDK_PATH}/include ${GECKO_SDK_PATH}/include/nspr)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/xulheaders")
include_directories("/usr/include/nspr") #FIXME

set(webconlib_src dom.cpp nsimpl.cpp promptservice.cpp webcontrol.cpp webframe.cpp webprefs.cpp)

add_library(webconlib STATIC ${webconlib_src})

