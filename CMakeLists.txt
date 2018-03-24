cmake_minimum_required(VERSION 3.9)
project(Team555)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
set(CMAKE_CXX_STANDARD 11)
#set(OpenCV_PREFIX C:/opencv/mingw)
set(OpenCV_DIR C:/opencv/mingw/install)
find_package( OpenCV REQUIRED opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs
        PATHS ${OpenCV_PREFIX}
        NO_DEFAULT_PATH) # For some reason CMake uses its defaults before the above paths.)
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )
add_executable(AirHockeyRobot main.cpp Puck.cpp inc/Puck.h Table.cpp inc/Table.h Camera.cpp inc/Camera.h ViewFinder.cpp inc/ViewFinder.h inc/helpers.h corners.cpp inc/Corners.h Serial.cpp inc/Serial.h MotorDriver.cpp inc/MotorDriver.h Config.cpp inc/Config.h Control.cpp inc/Control.h Supervisor.cpp inc/Supervisor.h Idle.cpp inc/Idle.h Mallet.cpp inc/Mallet.h ImageProcess.cpp inc/ImageProcess.h inc/Settings.h Thing.cpp inc/Thing.h Motion.cpp inc/Motion.h helpers.cpp)
link_directories(${OpenCV_LIB_DIR})
target_link_libraries( AirHockeyRobot ${OpenCV_LIBS} )


