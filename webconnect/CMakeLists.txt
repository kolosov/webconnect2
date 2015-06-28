#SET(wxWidgets_USE_LIBS base core aui)
find_package(wxWidgets REQUIRED base core aui)
include(${wxWidgets_USE_FILE})

if(CMAKE_SYSTEM_NAME MATCHES Linux)
  add_definitions(-fshort-wchar)
endif()

#set nsall header
include_directories(${GECKO_SDK_PATH}/include ${GECKO_SDK_PATH}/include/nspr)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/xulheaders")
include_directories("/usr/include/nspr") #FIXME

set(webconlib_src dom.cpp nsimpl.cpp promptservice.cpp webcontrol.cpp webframe.cpp webprefs.cpp )
if((CMAKE_SYSTEM_NAME MATCHES Darwin) AND (MOZILLA_VERSION_1 VERSION_LESS "33"))
  set ( webconlib_src ${webconlib_src} nsEmbedWeakReference.cpp)
endif()

add_library(webconlib STATIC ${webconlib_src})

