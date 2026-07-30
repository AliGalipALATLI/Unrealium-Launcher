cmake_minimum_required(VERSION 3.20)
project(Unrealium-Launcher LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

add_executable(Unrealium-Launcher
    src/main.cpp
    src/MainWindow.cpp
    src/AddEditorDialog.cpp
    src/DesktopEntryWriter.cpp
    src/ConfigManager.cpp
    src/ViewEditorsPage.cpp
    src/ViewProjectsPage.cpp
    src/EditorCardWidget.cpp
    src/ProjectCardWidget.cpp
    src/NodeManager.cpp
    src/NodeDetailDialog.cpp
    src/ProjectScannerService.cpp
    src/EditorSwitchDialog.cpp
)

target_link_libraries(Unrealium-Launcher PRIVATE Qt6::Widgets Qt6::Core)

# Copy runtime assets and assistant data into the build directory
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY assistant DESTINATION ${CMAKE_BINARY_DIR})
