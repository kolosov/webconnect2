find_package(wxWidgets REQUIRED base core aui)
find_library(gecko_libs xpcomglue PATHS ${GECKO_SDK_PATH}/sdk/lib)
find_library(xulrunner_mozalloc mozalloc PATHS ${GECKO_SDK_PATH}/sdk/lib)
find_library(gecko_libs_mozglue mozglue PATHS ${GECKO_SDK_PATH}/sdk/lib)

if(CMAKE_SYSTEM_NAME MATCHES Linux)
  find_library(gecko_libs_memory memory PATHS ${GECKO_SDK_PATH}/sdk/lib)
endif()

message ("Building for gecko version: ${MOZILLA_VERSION_1}")

include(${wxWidgets_USE_FILE})
include_directories ("${PROJECT_SOURCE_DIR}/webconnect")

set(testapp_src testapp.cpp)

#add_executable(testapp WIN32 ${testapp_src})
add_executable(testapp ${testapp_src})

if(CMAKE_SYSTEM_NAME MATCHES Linux)
  target_link_libraries(testapp webconlib ${gecko_libs} ${wxWidgets_LIBRARIES} dl -Wl,--whole-archive ${gecko_libs_mozglue} -Wl,--no-whole-archive ${gecko_libs_memory} pthread ${xulrunner_mozalloc} )
# target_link_libraries(testapp ${wxWidgets_LIBRARIES} dl  webconlib ${gecko_libs})
# target_link_libraries(testapp ${wxWidgets_LIBRARIES} webconlib ${gecko_libs})
endif()

if(CMAKE_SYSTEM_NAME MATCHES Windows)
# add_definitions("-D_ITERATOR_DEBUG_LEVEL=0")
# target_link_libraries(gtkskel listener geckoembed ${gecko_libs}  ${gecko_libs_mozglue} ${xulrunner_mozalloc} ${GTK2_LIBRARIES} )
  set_target_properties(testapp PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
  set_target_properties(testapp PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
  set_target_properties(testapp PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
  if(${MOZILLA_VERSION_1} GREATER 33)
    target_link_libraries(testapp webconlib ${gecko_libs} ${gecko_libs_mozglue}  ${xulrunner_mozalloc} ${wxWidgets_LIBRARIES})
  else()
    target_link_libraries(testapp webconlib ${gecko_libs} ${xulrunner_mozalloc} ${wxWidgets_LIBRARIES})
  endif()
endif()

if(CMAKE_SYSTEM_NAME MATCHES Darwin)
  target_link_libraries(testapp webconlib ${gecko_libs} ${wxWidgets_LIBRARIES} dl  pthread )
#  target_link_libraries(testapp webconlib ${gecko_libs} ${wxWidgets_LIBRARIES} dl -Wl,--whole-archive ${gecko_libs_mozglue} -Wl,--no-whole-archive ${gecko_libs_memory} pthread ${xulrunner_mozalloc} )
# target_link_libraries(testapp ${wxWidgets_LIBRARIES} dl  webconlib ${gecko_libs})
# target_link_libraries(testapp ${wxWidgets_LIBRARIES} webconlib ${gecko_libs})
endif()

