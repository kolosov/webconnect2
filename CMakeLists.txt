cmake_minimum_required (VERSION 2.6)
project(webconnect2)

#default gecko version
if(NOT MOZ_VERSION)
  set(MOZ_VERSION "2.0")
endif()

string(REGEX REPLACE "([0-9]+).[0-9.]+" "\\1" MOZILLA_VERSION_1 ${MOZ_VERSION})
string(REGEX REPLACE "([0-9]+).([0-9]+)([0-9.]*)" "\\2" MOZILLA_VERSION_2 ${MOZ_VERSION})
message("MOZ_VERSION is ${MOZ_VERSION}")
#TODO rename MOZILLA_VERSION_* to another name
message("MOZILLA_VERSION_1 is ${MOZILLA_VERSION_1}")
message("MOZILLA_VERSION_2 is ${MOZILLA_VERSION_2}")

#config header
configure_file (
  "${PROJECT_SOURCE_DIR}/moz_config.h.in"
  "${PROJECT_BINARY_DIR}/moz_config.h"
  )
include_directories("${PROJECT_BINARY_DIR}")

#find_library(gecko_libs xpcomglue PATHS ${GECKO_SDK_PATH}/lib)

add_subdirectory (webconnect)
add_subdirectory (testapp)

