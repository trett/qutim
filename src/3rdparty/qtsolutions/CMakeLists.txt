file(GLOB SRC "qtlocalpeer.cpp" "qtlocalpeer.h" "qtlockedfile.cpp" "qtlockedfile.h" "qtlockedfile_unix.cpp")
find_package(Qt5 COMPONENTS Core Network REQUIRED)
add_library(Qtsolutions ${SRC})
target_link_libraries(Qtsolutions Qt5::Core Qt5::Network)
