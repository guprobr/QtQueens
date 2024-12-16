cmake_minimum_required(VERSION 3.16)

# Project name
project(QtQueens LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt's MOC, UIC, and RCC processing
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Add the Qt resource file
set(RESOURCES
    resources.qrc
)

# Find required Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Add source files to the project
set(SOURCES
    main.cpp
    mainwindow.cpp
    chessboard.cpp
    queen.cpp
    utils.cpp
)

set(HEADERS
    mainwindow.h
    chessboard.h
    queen.h
    utils.h
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})

# Link Qt6 libraries to the executable
target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Widgets)

# Include header directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
