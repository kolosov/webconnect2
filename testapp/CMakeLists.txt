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

if(CMAKE_SYSTEM_NAME MATCHES Linux)
# add_definitions(-fno-rtti -std=c++11)
# target_link_libraries(gtkskel listener geckoembed ${gecko_libs} ${GTK2_LIBRARIES} dl -Wl,--whole-archive ${gecko_libs_mozglue} -Wl,--no-whole-archive ${gecko_libs_memory} pthread ${xulrunner_mozalloc} )
 target_link_libraries(testapp ${wxWidgets_LIBRARIES} dl  webconlib ${gecko_libs})
# target_link_libraries(testapp ${wxWidgets_LIBRARIES} webconlib ${gecko_libs})
endif()

if(CMAKE_SYSTEM_NAME MATCHES Windows)
# target_link_libraries(gtkskel listener geckoembed ${gecko_libs}  ${gecko_libs_mozglue} ${xulrunner_mozalloc} ${GTK2_LIBRARIES} )
 target_link_libraries(testapp ${wxWidgets_LIBRARIES} webconlib ${gecko_libs})
endif()

#add_executable(testapp WIN32 ${testapp_src})
#target_link_libraries(testapp ${wxWidgets_LIBRARIES} webconlib ${gecko_libs})

