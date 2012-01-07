find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})

if(CMAKE_SYSTEM_NAME MATCHES Linux)
if ( ${MOZILLA_VERSION_1} VERSION_LESS "7") #FIXME cross check!
add_definitions(-fshort-wchar)
endif()
 add_definitions(-g -std=c++0x -fno-rtti -DXPCOM_GLUE_USE_NSPR -DXPCOM_GLUE=1)
endif()

if(CMAKE_SYSTEM_NAME MATCHES Windows)
if ( ${MOZILLA_VERSION_1} VERSION_LESS "7") #FIXME cross check!
  add_definitions(/Zc:wchar_t-)
endif()
 add_definitions(-DXPCOM_GLUE_USE_NSPR -DXPCOM_GLUE=1 -DXP_WIN=1)
endif()

#set nsall header

include_directories("${CMAKE_CURRENT_BINARY_DIR}")
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/xulheaders")
#include_directories("/usr/include/nspr") #FIXME

set(webconlib_src dom.cpp nsimpl.cpp promptservice.cpp webcontrol.cpp webframe.cpp webprefs.cpp)

add_library(webconlib STATIC ${webconlib_src})

