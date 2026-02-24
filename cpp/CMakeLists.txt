cmake_minimum_required(VERSION 3.23)
project(cppbridge LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick)
find_package(OpenCV 4 REQUIRED COMPONENTS core imgcodecs imgproc)
qt_standard_project_setup(REQUIRES 6.5)

qt_add_qml_module(cppbridgeplugin
    URI cppbridge
    VERSION 1.0
    OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../package/contents/ui/cppbridge"
    SOURCES
        src/clockimageprovider.hpp
        src/clockimageprovider.cpp
        src/config.hpp
        src/renderer.hpp
        src/renderer.cpp
        src/timebridge.h
        src/timebridge.cpp
)

target_include_directories(cppbridgeplugin PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(cppbridgeplugin PRIVATE
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    opencv_core
    opencv_imgcodecs
    opencv_imgproc
)
set_target_properties(cppbridgeplugin PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
