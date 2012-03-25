#SET(wxWidgets_USE_LIBS base core aui)
find_package(wxWidgets REQUIRED base core aui)
include(${wxWidgets_USE_FILE})

if(CMAKE_SYSTEM_NAME MATCHES Linux)
if ( ${MOZILLA_VERSION_1} VERSION_LESS "7") #FIXME cross check!
#add_definitions(-fshort-wchar)
endif()
 add_definitions(-g -std=c++0x -fno-rtti -DXPCOM_GLUE_USE_NSPR -DXPCOM_GLUE=1)
endif()

if(CMAKE_SYSTEM_NAME MATCHES Windows)
if ( ${MOZILLA_VERSION_1} VERSION_LESS "7") #FIXME cross check!
add_definitions(/Zc:wchar_t-)
#add_definitions(-DMOZILLA_STRICT_API)
endif()
add_definitions(-DXPCOM_GLUE_USE_NSPR -DXPCOM_GLUE=1 -DXP_WIN=1)
endif()

#set nsall header
if ( ${MOZILLA_VERSION_1} VERSION_LESS "2")
        include_directories(${GECKO_SDK_PATH}/include ${GECKO_SDK_PATH}/include/nspr 
        ${GECKO_SDK_PATH}/include/xpcom ${GECKO_SDK_PATH}/include/string 
        ${GECKO_SDK_PATH}/include/uriloader ${GECKO_SDK_PATH}/include/docshell 
        ${GECKO_SDK_PATH}/include/xulapp ${GECKO_SDK_PATH}/include/necko 
        ${GECKO_SDK_PATH}/include/dom ${GECKO_SDK_PATH}/include/webbrwsr 
        ${GECKO_SDK_PATH}/include/embed_base ${GECKO_SDK_PATH}/include/widget 
        ${GECKO_SDK_PATH}/include/pref ${GECKO_SDK_PATH}/include/gfx 
        ${GECKO_SDK_PATH}/include/windowwatcher ${GECKO_SDK_PATH}/include/profdirserviceprovider 
        ${GECKO_SDK_PATH}/include/pipnss ${GECKO_SDK_PATH}/include/exthandler
        ${GECKO_SDK_PATH}/include/mimetype ${GECKO_SDK_PATH}/include/find
        ${GECKO_SDK_PATH}/include/webshell ${GECKO_SDK_PATH}/include/webbrowserpersist
        ${GECKO_SDK_PATH}/include/nkcache ${GECKO_SDK_PATH}/include/caps
        ${GECKO_SDK_PATH}/include/xpconnect ${GECKO_SDK_PATH}/include/js
)
else()
 include_directories(${GECKO_SDK_PATH}/include ${GECKO_SDK_PATH}/include/nspr)
endif ()


include_directories("${CMAKE_CURRENT_BINARY_DIR}")
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/xulheaders")
include_directories("/usr/include/nspr") #FIXME

set(webconlib_src dom.cpp nsimpl.cpp promptservice.cpp webcontrol.cpp webframe.cpp webprefs.cpp ContentListener.cpp DOMEventListener.cpp ConsoleListener.cpp)

add_library(webconlib STATIC ${webconlib_src})

