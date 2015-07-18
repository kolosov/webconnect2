find_package(PythonInterp REQUIRED)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
find_package(wxWidgets REQUIRED base core aui)

set(wc_python_src_DIR ${CMAKE_CURRENT_SOURCE_DIR})

message("wx_phoenix path is ${WX_PHOENIX_PATH}")
message("wxWidgets includes are ${wxWidgets_INCLUDE_DIRS}")
message("wc_python_src_path is ${wc_python_src_DIR}")
#message("webconnect lib is ${webconlib}")


#config header
configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/configure.py.in"
  "${CMAKE_CURRENT_BINARY_DIR}/configure.py"
  )


include_directories("${PROJECT_BINARY_DIR}")

#generate SIPMakefile
add_custom_target (
	SIPMakefile
	COMMAND ${PYTHON_EXECUTABLE} configure.py
	DEPENDS webconnect.sip webconlib
	COMMENT "Generating SIP files and SIPMakefile"
)

add_custom_target (
	webconnect.so
	COMMAND make -f SIPMakefile
	DEPENDS SIPMakefile webconlib
	COMMENT "Building webconnect.so by SIPMakefile"
)


#generate SIP
#set(sip_cmd sip)
#set(sip_args -c ./ -I ${PROJECT_SOURCE_DIR}/webconnect/ -I ${WX_PHOENIX_PATH}/sip/gen -I ${WX_PHOENIX_PATH}/src  ${CMAKE_CURRENT_SOURCE_DIR}/webconnect.sip)

#add_custom_target(
#	webconnect.sip
#	COMMAND ${sip_cmd} ${sip_args}
#	DEPENDS webconlib
#	COMMENT "Generating sip file"
#)

#build webconnect module by configure.py

#add_custom_target (
#	webconnect.so
#	COMMAND ${PYTHON_EXECUTABLE} configure.py
#	DEPENDS webconnect.sip
#	COMMENT "Building webconnect.so by python script"
#)


#add_dependencies()

#add_custom_command (
#	OUTPUT webconnect.sip
#	COMMAND "${sip_cmd} ${sip_args}"
#	DEPENDS webconlib
#	COMMENT "Generating sip file"
#)

#build webconnect module by configure.py
#add_custom_command (
#	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/webconnect.so
#	COMMAND "python configure.py"
#	DEPENDS webconnect.sip
#	COMMENT "Building webconnect.so by python script"
#)


#add_custom_target(webconnect.so ALL )
#execute_process(sip -c ./ -I ${CMAKE_PROJECT_SOURCE_DIR}/webconnect/ -I ${WX_PHOENIX_PATH}/sip/gen -I ${WX_PHOENIX_PATH}/src  webconnect.sip)
#execute_process( COMMAND ${sip_cmd} ${sip_args} 
#		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#		RESULT_VARIABLE sip_result
#		OUTPUT_VARIABLE sip_out)

#message ("run cmd " "${sip_cmd}: ${sip_args}")
#message (STATUS "sip [${sip_res}]: ${sip_out}")

#sip -c ./ -I ${CMAKE_PROJECT_SOURCE_DIR}/webconnect/ -I ${WX_PHOENIX_PATH}/sip/gen -I ${WX_PHOENIX_PATH}/src  webconnect.sip)
