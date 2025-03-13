cmake_minimum_required(VERSION 3.16)
project(file_organizer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS 
    Widgets
    Concurrent
)

add_executable(file_organizer
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    resources.qrc
)

target_link_libraries(file_organizer PRIVATE 
    Qt6::Widgets
    Qt6::Concurrent
) 