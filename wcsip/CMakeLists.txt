find_package(PythonInterp REQUIRED)
find_package(PythonLibs)
find_package(wxWidgets REQUIRED base core aui)


include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(wc_python_src_DIR ${CMAKE_CURRENT_SOURCE_DIR})

message("wx_phoenix path is ${WX_PHOENIX_PATH}")
message("wxWidgets includes are ${wxWidgets_INCLUDE_DIRS}")
message("wc_python_src_path is ${wc_python_src_DIR}")

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/configure.py.in"
  "${CMAKE_CURRENT_BINARY_DIR}/configure.py"
  )

include_directories("${PROJECT_BINARY_DIR}")
include_directories ("${PROJECT_SOURCE_DIR}/webconnect")

include_directories("${WX_PHOENIX_PATH}/sip/gen")
include_directories("${WX_PHOENIX_PATH}/sip/siplib")

include_directories("${PYTHON_INCLUDE_DIR}")

include(${wxWidgets_USE_FILE})

#Old way to build module. Leave it just in case
#generate SIPMakefile
#add_custom_target (
#	SIPMakefile
#	COMMAND ${PYTHON_EXECUTABLE} configure.py
#	DEPENDS webconnect.sip webconlib
#	COMMENT "Generating SIP files and SIPMakefile"
#)

#add_custom_target (
#	webconnect.so ALL
#	COMMAND make -f SIPMakefile
#	DEPENDS SIPMakefile webconlib
#	COMMENT "Building webconnect.so by SIPMakefile"
#)


#generate SIP
set(sip_cmd ${WX_PHOENIX_PATH}/bin/sip-4.16.7-linux)
set(sip_args -c ./ -I ${WX_PHOENIX_PATH}/sip/gen -I ${WX_PHOENIX_PATH}/src  ${CMAKE_CURRENT_SOURCE_DIR}/webconnect.sip)

set(pywebconnect_SRC sipwebconnectcmodule.cpp sipwebconnectwxWebContentHandler.cpp sipwebconnectwxWebPreferences.cpp
 sipwebconnectSimpleGeckoEngine.cpp  sipwebconnectwxWebControl.cpp sipwebconnectwxWebProgressBase.cpp
 sipwebconnectwxDOMDocument.cpp      sipwebconnectwxWebPostData.cpp )

add_custom_command (
	OUTPUT ${pywebconnect_SRC}
#	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND ${sip_cmd} ${sip_args}
	DEPENDS webconnect.sip
	COMMENT "Generating src files from sip"
)

if (CMAKE_SYSTEM_NAME MATCHES Linux)
  add_definitions( -DXPCOM_GLUE_USE_NSPR -std=c++11 -fno-rtti)
endif()

set(CMAKE_SHARED_LIBRARY_PREFIX "")
add_library(webconnect SHARED ${pywebconnect_SRC})

if(CMAKE_SYSTEM_NAME MATCHES Linux)
	#set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--whole-archive -L${GECKO_SDK_PATH}/lib ${gecko_xpcomglue} ${gecko_libs_mozglue} ${gecko_libs_memory} -Wl,--no-whole-archive -lxul -lmozalloc  -Wl,--version-script=webconnect.exp")
    target_link_libraries(webconnect webconlib ${wxWidgets_LIBRARIES} -L${GECKO_SDK_PATH}/lib -Wl,-rpath=${GECKO_SDK_PATH}/bin  dl pthread )
endif()

