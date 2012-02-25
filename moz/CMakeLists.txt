#add mozilla major version

if(CMAKE_SYSTEM_NAME MATCHES Linux)
 if ( ${MOZILLA_VERSION_1} VERSION_LESS "7")
  add_definitions(-fshort-wchar)
 endif()
 add_definitions(-g -std=c++0x -fno-rtti -DXPCOM_GLUE_USE_NSPR -DXPCOM_GLUE=1)
 #set(GECKO_SDK_PATH "/home/sk/work/src/build/sdk/xulrunner-dev-6.0")
endif()

if(CMAKE_SYSTEM_NAME MATCHES Windows)
 if ( ${MOZILLA_VERSION_1} VERSION_LESS "7")
  add_definitions(/Zc:wchar_t-)
 endif()
 add_definitions(-DXPCOM_GLUE_USE_NSPR -DXPCOM_GLUE=1 -DXP_WIN=1)
 #set(GECKO_SDK_PATH "c:/dev/xulrunner-sdk")
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

set(mozlib_src ConsoleListener.cpp ContentListener.cpp DOMEventListener.cpp EmbeddingSetup.cpp WebBrowserChrome.cpp embed.cpp)

add_library(moz STATIC ${mozlib_src})
