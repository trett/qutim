set(CMAKE_QUTIM_VERSION_STRING "0.4.0.0")
set(CMAKE_QUTIM_VERSION_MAJOR "0")
set(CMAKE_QUTIM_VERSION_MINOR "4")
set(CMAKE_QUTIM_VERSION_SECMINOR "0")
set(CMAKE_QUTIM_VERSION_PATCH "0")

configure_file(version.h.cmake "${QUTIM_INCLUDE}/libqutim_version.h")
# TODO: Don't use GLOB
file(GLOB SRC "*.cpp")
find_package(Qt5 COMPONENTS Widgets Quick Qml Script REQUIRED)
add_definitions(-DK8JSON_INCLUDE_GENERATOR -DK8JSON_INCLUDE_COMPLEX_GENERATOR)
list(APPEND SRC "${CMAKE_SOURCE_DIR}/src/3rdparty/k8json/k8json.cpp")

add_compile_definitions(QUTIM_PLUGIN_NAME="libqutim")
add_compile_definitions(QUTIM_SHARE_DIR="${QUTIM_SHARE_DIR}")
add_library(libqutim ${SRC})
get_filename_component(HEADERS ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY CACHE PATH)
target_include_directories(libqutim PRIVATE "${HEADERS}" "${QUTIM_INCLUDE}" "${CMAKE_SOURCE_DIR}/src/3rdparty/src/k8json.h")
set_target_properties(libqutim PROPERTIES
        PREFIX ""
        VERSION ${CMAKE_QUTIM_VERSION_STRING}
        SOVERSION ${CMAKE_QUTIM_VERSION_MAJOR}
        DEFINE_SYMBOL LIBQUTIM_LIBRARY
)

target_link_libraries(libqutim Qt5::Widgets Qt5::Qml Qt5::Quick Qt5::Script)

