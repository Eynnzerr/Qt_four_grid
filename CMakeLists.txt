cmake_minimum_required(VERSION 3.22)
project(four_grid)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/home/eynnzerr/open/Qt/5.15.2/gcc_64")

# import BOOST
set(BOOST_ROOT /home/eynnzerr/open/github/boost_1_83_0/build)
set(Boost_NO_SYSTEM_PATHS ON)

find_package(Boost REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})

    MESSAGE( STATUS "Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}.")
    MESSAGE( STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}.")
    MESSAGE( STATUS "Boost_LIB_VERSION = ${Boost_LIB_VERSION}.")
endif()

# import Qt5
find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        Charts
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
        Configuration.cpp
        Configuration.h
        chartWidget.cpp
        chartWidget.h
        TopoDisplay.cpp
        TopoDisplay.h
        StreamConfiguration.cpp
        StreamConfiguration.h
        CompletePage.cpp
        CompletePage.h
        TopoWidget.cpp
        TopoWidget.h
        RealSimPage.cpp RealSimPage.h)

target_link_libraries(four_grid
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Charts
        ${Boost_LIBRARIES}
        -lpthread -lm
        )
