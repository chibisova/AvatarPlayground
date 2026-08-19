cmake_minimum_required(VERSION 3.16)

project(SpatialAvatarPlayground)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)    

add_executable(SpatialAvatarPlayground
    src/main.cpp
    src/HUD.cpp
    src/Input.cpp
    src/Timing.cpp
    src/CameraCalibration.cpp
    src/Map3D.cpp
    src/ProcessorManager.cpp
    src/processors/BFMatcherProcessor.cpp
    src/processors/BlurProcessor.cpp
    src/processors/CannyProcessor.cpp
    src/processors/GrayProcessor.cpp
    src/processors/HarrisProcessor.cpp
    src/processors/KNNMatcherProcessor.cpp
    src/processors/MotionEstimationProcessor.cpp
    src/processors/OpticalFlowProcessor.cpp
    src/processors/ORBProcessor.cpp
    src/processors/OriginalProcessor.cpp
    src/processors/PnPProcessor.cpp
    src/processors/RANSACProcessor.cpp
    src/processors/ShiTomasiProcessor.cpp
    src/processors/ThresholdProcessor.cpp
    src/processors/TriangulationProcessor.cpp
    src/VisualOdometry.cpp
    
)

target_include_directories(SpatialAvatarPlayground PRIVATE 
include)

target_include_directories(SpatialAvatarPlayground PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(SpatialAvatarPlayground PRIVATE ${OpenCV_LIBS})

add_executable(PnPTest
    src/tests/PnPTest.cpp
    src/processors/PnPProcessor.cpp
    src/Map3D.cpp
)

target_link_libraries(PnPTest PRIVATE ${OpenCV_LIBS})

target_include_directories(PnPTest PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include

)

add_executable(TUMTest
    src/tests/TUMTest.cpp
    src/processors/MotionEstimationProcessor.cpp
)

target_link_libraries(TUMTest PRIVATE
    ${OpenCV_LIBS}
)

target_include_directories(TUMTest PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include
)
