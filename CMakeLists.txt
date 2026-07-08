cmake_minimum_required(VERSION 3.16)

project(SpatialAvatarPlayground)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)    

add_executable(SpatialAvatarPlayground
    src/main.cpp
    src/HUD.cpp
    src/Input.cpp
    src/Timing.cpp
    src/ImageProcessor.cpp
)

target_include_directories(SpatialAvatarPlayground PRIVATE include)

target_include_directories(SpatialAvatarPlayground PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(SpatialAvatarPlayground PRIVATE ${OpenCV_LIBS})
