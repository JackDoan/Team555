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
add_executable(AirHockeyRobot main.cpp Puck.cpp Puck.h Table.cpp Table.h Camera.cpp Camera.h ThresholdImage.cpp ThresholdImage.h ViewFinder.cpp ViewFinder.h helpers.h  motors/MotorComm.cpp motors/MotorComm.h corners.cpp corners.h Serial.cpp Serial.h MotorDriver.cpp MotorDriver.h Config.cpp Config.h Control.cpp Control.h Supervisor.cpp Supervisor.h Idle.cpp Idle.h)
link_directories(${OpenCV_LIB_DIR})
target_link_libraries( AirHockeyRobot ${OpenCV_LIBS} )


