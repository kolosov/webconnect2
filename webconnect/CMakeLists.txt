find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})

if(CMAKE_SYSTEM_NAME MATCHES Linux)
# if ( ${MOZILLA_VERSION_1} VERSION_LESS "7") FIXME cross check!
#  add_definitions(-fshort-wchar)
# endif()
 add_definitions(-g -std=c++0x -fno-rtti -DXPCOM_GLUE_USE_NSPR -DXPCOM_GLUE=1)
endif()

if(CMAKE_SYSTEM_NAME MATCHES Windows)
# if ( ${MOZILLA_VERSION_1} VERSION_LESS "7") FIXME cross check!
#  add_definitions(/Zc:wchar_t-)
# endif()
 add_definitions(-DXPCOM_GLUE_USE_NSPR -DXPCOM_GLUE=1 -DXP_WIN=1)
endif()

#set nsall header

if ( ${MOZILLA_VERSION_1} VERSION_EQUAL "1" AND ${MOZILLA_VERSION_2} VERSION_EQUAL "9")
 message("Using nsall.h_1.9.2")
 configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/nsall.h_1.9.2"
  "${CMAKE_CURRENT_BINARY_DIR}/nsall.h"
  COPYONLY
  )
endif()

if ( ${MOZILLA_VERSION_1} VERSION_EQUAL "2")
message("Using nsall.h_2.0.1")
 configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/nsall.h_2.0.1"
  "${CMAKE_CURRENT_BINARY_DIR}/nsall.h"
  COPYONLY
  )
endif()

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

set(webconlib_src dom.cpp nsimpl.cpp promptservice.cpp webcontrol.cpp webframe.cpp webprefs.cpp)

add_library(webconlib STATIC ${webconlib_src})

