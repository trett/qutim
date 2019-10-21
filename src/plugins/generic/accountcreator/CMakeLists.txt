find_package(Qt5 COMPONENTS Gui Widgets)
add_library(accountcreator accountcreator.cpp)
target_link_libraries(accountcreator Qt5::Gui Qt5::Widgets)
