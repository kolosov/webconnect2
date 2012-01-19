cmake_minimum_required (VERSION 2.6)
project(webconnect2)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
#check for xulrunner dir
if(NOT GECKO_SDK_PATH)
 if(CMAKE_SYSTEM_NAME MATCHES Linux)
  set(GECKO_SDK_PATH "/home/sk/work/src/build/xulrunner-dev-1.9.2/lib/xulrunner-devel-1.9.2.19")
 endif()
 if(CMAKE_SYSTEM_NAME MATCHES Windows)
  set(GECKO_SDK_PATH "c:/dev/xulrunner-sdk")
 endif()
endif()

set(is_mozver_3 false)

#set mozilla version
message( "GECKO_SDK_PATH is ${GECKO_SDK_PATH}")
file(READ ${GECKO_SDK_PATH}/include/mozilla-config.h MCONFILE)
string(REGEX MATCH "#define[ ]+MOZILLA_VERSION[ ]\"+[0-9.a-zA-Z]+\"" MOZVERSTR ${MCONFILE})
string(REGEX REPLACE "#define[ ]MOZILLA_VERSION[ ]\"([0-9]+).[0-9.a-zA-Z]+\"" "\\1" MOZILLA_VERSION_1 ${MOZVERSTR})
string(REGEX REPLACE "#define[ ]MOZILLA_VERSION[ ]\"([0-9]+).([0-9]+)([0-9.a-zA-Z]*)\"" "\\2" MOZILLA_VERSION_2 ${MOZVERSTR})
string(REGEX REPLACE "#define[ ]MOZILLA_VERSION[ ]\"([0-9]+).([0-9]+).([0-9]+)([0-9.a-zA-Z]*)\"" "\\3" MOZILLA_VERSION_3 ${MOZVERSTR})

#check that version_3 is exist
string(COMPARE EQUAL "${MOZILLA_VERSION_3}" "${MOZVERSTR}" is_mozver_3)
if(${is_mozver_3})
 set (MOZILLA_VERSION_3 0)
endif()
message("MOZVERSTR is ${MOZVERSTR}")
message("MOZILLA_VERSION_1 is ${MOZILLA_VERSION_1}")
message("MOZILLA_VERSION_2 is ${MOZILLA_VERSION_2}")
message("MOZILLA_VERSION_3 is ${MOZILLA_VERSION_3}")

if(CMAKE_SYSTEM_NAME MATCHES Windows)
  string(REGEX REPLACE "/" "\\\\\\\\" GECKO_SDK_PATH_CONFIG ${GECKO_SDK_PATH})
 else()
  set(GECKO_SDK_PATH_CONFIG ${GECKO_SDK_PATH})
#  include_directories("/usr/include/nspr") #FIXME
endif()
message("GECKO_SDK_PATH_CONFIG is ${GECKO_SDK_PATH_CONFIG}")
#config header
configure_file (
  "${PROJECT_SOURCE_DIR}/moz_config.h.in"
  "${PROJECT_BINARY_DIR}/moz_config.h"
  )
include_directories("${PROJECT_BINARY_DIR}")

add_subdirectory (webconnect)
add_subdirectory (testapp)

