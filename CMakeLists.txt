cmake_minimum_required(VERSION 3.20)
project(UnrealiumLauncher LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Network)

add_executable(UnrealiumLauncher
    src/main.cpp
    src/MainWindow.cpp
    src/AddEditorDialog.cpp
    src/DownloadEditorDialog.cpp
    src/DesktopEntryWriter.cpp
    src/ConfigManager.cpp
    src/ViewEditorsPage.cpp
    src/EditorCardWidget.cpp
    src/NodeManager.cpp
    src/NodeDetailDialog.cpp
)

target_link_libraries(UnrealiumLauncher PRIVATE Qt6::Widgets Qt6::Core Qt6::Network)

# Copy runtime assets and assistant data into the build directory
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY assistant DESTINATION ${CMAKE_BINARY_DIR})
