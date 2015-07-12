#SET(wxWidgets_USE_LIBS base core aui)
find_package(wxWidgets REQUIRED base core aui)
include(${wxWidgets_USE_FILE})

find_library(gecko_xpcomglue xpcomglue PATHS ${GECKO_SDK_PATH}/lib)
find_library(gecko_libs_mozglue mozglue PATHS ${GECKO_SDK_PATH}/lib)


if(CMAKE_SYSTEM_NAME MATCHES Linux)
  find_library(gecko_libs_memory memory PATHS ${GECKO_SDK_PATH}/lib)
endif()
if(CMAKE_SYSTEM_NAME MATCHES Linux)
  add_definitions(-fshort-wchar)
endif()
#TODO check it
#add_definitions( -DXPCOM_GLUE=1)

#set nsall header
include_directories(${GECKO_SDK_PATH}/include ${GECKO_SDK_PATH}/include/nspr)
include_directories ("${PROJECT_SOURCE_DIR}/webconnect")

include_directories("${CMAKE_CURRENT_BINARY_DIR}")
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/xulheaders")
include_directories("/usr/include/nspr") #FIXME

set(wclib_path ${PROJECT_SOURCE_DIR}/webconnect)

set(wxgeckolib_src wxgeckolib.cpp ${wclib_path}/dom.cpp ${wclib_path}/nsimpl.cpp ${wclib_path}/promptservice.cpp ${wclib_path}/webcontrol.cpp ${wclib_path}/webframe.cpp ${wclib_path}/webprefs.cpp )
#set(wxgeckolib_src wxgeckolib.cpp ${webconlib_src} )
if((CMAKE_SYSTEM_NAME MATCHES Darwin) AND (MOZILLA_VERSION_1 VERSION_LESS "33"))
  set ( webconlib_src ${webconlib_src} nsEmbedWeakReference.cpp)
endif()

add_definitions( -DXPCOM_GLUE=1 )

if(CMAKE_SYSTEM_NAME MATCHES Linux)
  set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--whole-archive -L${GECKO_SDK_PATH}/lib ${gecko_xpcomglue} ${gecko_libs_mozglue} ${gecko_libs_memory} -Wl,--no-whole-archive -lxul -lmozalloc")
  add_library(wxgeckolib SHARED ${wxgeckolib_src})
endif()

