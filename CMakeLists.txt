cmake_minimum_required(VERSION 3.5)

project(midicontroller LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Svg Widgets REQUIRED)

add_executable(midicontroller
    main.cpp
    mainwindow.ui
    mainwindow.h
    mainwindow.cpp
    controlsurfacewidget.h
    controlsurfacewidget.cpp
    )

target_link_libraries(midicontroller PRIVATE Qt5::Svg Qt5::Widgets)
