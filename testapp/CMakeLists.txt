find_package(wxWidgets REQUIRED base core aui)
include(${wxWidgets_USE_FILE})

if(CMAKE_SYSTEM_NAME MATCHES Linux)
add_definitions(-g -std=c++0x -fno-rtti -DXPCOM_GLUE_USE_NSPR -DXPCOM_GLUE=1)
endif()

if(CMAKE_SYSTEM_NAME MATCHES Windows)
	add_definitions(-DXPCOM_GLUE_USE_NSPR -DXPCOM_GLUE=1 -DXP_WIN=1)
endif()

find_library(gecko_libs xpcomglue PATHS ${GECKO_SDK_PATH}/lib)

include_directories ("${PROJECT_SOURCE_DIR}/webconnect")

set(testapp_src testapp.cpp)

add_executable(testapp WIN32 ${testapp_src})
target_link_libraries(testapp ${wxWidgets_LIBRARIES} webconlib ${gecko_libs})
#target_link_libraries(testapp ${QT_LIBRARIES} webconlib ${gecko_libs})

