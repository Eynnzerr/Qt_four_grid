cmake_minimum_required(VERSION 3.22)
project(four_grid)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/home/eynnzerr/open/Qt/5.15.2/gcc_64")

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)
add_executable(
        four_grid
        main.cpp
        fourgrid.cpp
        fourgrid.h
        widget.cpp
        widget.h
        welcome.cpp
        welcome.h
        fourgrid.qrc
        )

target_link_libraries(four_grid
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        )
