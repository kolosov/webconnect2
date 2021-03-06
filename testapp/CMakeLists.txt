find_package(wxWidgets REQUIRED base core aui)
find_library(gecko_mozalloc mozalloc PATHS ${GECKO_SDK_PATH}/sdk/lib)
find_library(gecko_libs xpcomglue PATHS ${GECKO_SDK_PATH}/sdk/lib)
find_library(gecko_libs_mozglue mozglue PATHS ${GECKO_SDK_PATH}/sdk/lib)

if(CMAKE_SYSTEM_NAME MATCHES Linux)
  find_library(gecko_libs_memory memory PATHS ${GECKO_SDK_PATH}/sdk/lib)
endif()

message ("Building for gecko version: ${MOZILLA_VERSION_1}")
#message ("wxWidgets libs: ${wxWidgets_LIBRARIES}")
#message ("wxWidgets lib dirs: ${wxWidgets_LIBRARY_DIRS}")


include(${wxWidgets_USE_FILE})
include_directories ("${PROJECT_SOURCE_DIR}/webconnect")

set(testapp_src testapp.cpp)
#set(testapp2_src testapp2.cpp)

#add_executable(testapp WIN32 ${testapp_src})
add_executable(testapp ${testapp_src})
add_executable(testapp2 ${testapp2_src})

if(CMAKE_SYSTEM_NAME MATCHES Linux)
  if(WC_STATIC)
    target_link_libraries(testapp webconlib ${gecko_libs} ${wxWidgets_LIBRARIES} dl -Wl,--whole-archive ${gecko_libs_mozglue} -Wl,--no-whole-archive ${gecko_libs_memory} pthread ${gecko_mozalloc} )
#    target_link_libraries(testapp2 webconlib ${gecko_libs} ${wxWidgets_LIBRARIES} dl -Wl,--whole-archive ${gecko_libs_mozglue} -Wl,--no-whole-archive ${gecko_libs_memory} pthread ${gecko_mozalloc} )
  else()
    target_link_libraries(testapp webconlib ${wxWidgets_LIBRARIES} -L${GECKO_SDK_PATH}/lib -Wl,-rpath=${GECKO_SDK_PATH}/bin  dl pthread )
#    target_link_libraries(testapp2 webconlib ${wxWidgets_LIBRARIES} -L${GECKO_SDK_PATH}/lib -Wl,-rpath=${GECKO_SDK_PATH}/bin  dl pthread )
  endif()
endif()

if(CMAKE_SYSTEM_NAME MATCHES Windows)
# add_definitions("-D_ITERATOR_DEBUG_LEVEL=0")
# target_link_libraries(gtkskel listener geckoembed ${gecko_libs}  ${gecko_libs_mozglue} ${xulrunner_mozalloc} ${GTK2_LIBRARIES} )
  set_target_properties(testapp PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
  set_target_properties(testapp PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
  set_target_properties(testapp PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
  if(${MOZILLA_VERSION_1} GREATER 33)
    target_link_libraries(testapp webconlib ${gecko_libs} ${gecko_libs_mozglue}  ${gecko_mozalloc} ${wxWidgets_LIBRARIES})
  else()
    target_link_libraries(testapp webconlib ${gecko_libs} ${gecko_mozalloc} ${wxWidgets_LIBRARIES})
  endif()
endif()

if(CMAKE_SYSTEM_NAME MATCHES Darwin)
  if(WC_STATIC)
    target_link_libraries(testapp webconlib ${gecko_libs} ${wxWidgets_LIBRARIES} dl  pthread )
#    target_link_libraries(testapp2 webconlib ${gecko_libs} ${wxWidgets_LIBRARIES} dl  pthread )
#    target_link_libraries(testapp webconlib ${gecko_libs} ${wxWidgets_LIBRARIES} dl -Wl,--whole-archive ${gecko_libs_mozglue} -Wl,--no-whole-archive ${gecko_libs_memory} pthread ${gecko_mozalloc} )
  else()
    target_link_libraries(testapp webconlib ${gecko_libs} ${wxWidgets_LIBRARIES} dl  pthread )
#    target_link_libraries(testapp2 webconlib ${gecko_libs} ${wxWidgets_LIBRARIES} dl  pthread )
#    target_link_libraries(testapp webconlib ${wxWidgets_LIBRARIES} -L${GECKO_SDK_PATH}/lib -Wl,-rpath=${GECKO_SDK_PATH}/bin  dl pthread )
  endif()
#  target_link_libraries(testapp webconlib ${gecko_libs} ${wxWidgets_LIBRARIES} dl  pthread "-rpath-link,${GECKO_SDK_PATH}/bin" )
#  target_link_libraries(testapp webconlib ${gecko_libs} ${wxWidgets_LIBRARIES} dl -Wl,--whole-archive ${gecko_libs_mozglue} -Wl,--no-whole-archive ${gecko_libs_memory} pthread ${gecko_mozalloc} )
endif()

