cmake_minimum_required(VERSION 3.9)
project(Team555)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++1y")
set(CMAKE_CXX_STANDARD 11)
#set(OpenCV_PREFIX C:/opencv/mingw)
set(OpenCV_DIR C:/opencv/mingw/install)
#find_package( OpenCV REQUIRED opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs
 #       PATHS ${OpenCV_PREFIX}
 #       ) # For some reason CMake uses its defaults before the above paths.)
find_package( OpenCV REQUIRED )
#INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )
add_executable(AirHockeyRobot src/main.cpp src/Table.cpp inc/Table.h src/Camera.cpp inc/Camera.h src/ViewFinder.cpp inc/ViewFinder.h inc/helpers.h src/Corners.cpp inc/Corners.h src/Serial.cpp inc/Serial.h src/MotorDriver.cpp inc/MotorDriver.h src/Config.cpp inc/Config.h src/Supervisor.cpp inc/Supervisor.h inc/Settings.h src/GamePieces/Thing.cpp inc/GamePieces/Thing.h src/motion/Motion.cpp inc/motion/Motion.h src/helpers.cpp inc/motion/Offense.h src/motion/Offense.cpp inc/motion/Offense.h inc/Serial_Linux.h PlayTime.cpp PlayTime.h src/DoubleBuffer.cpp inc/DoubleBuffer.h inc/states.h inc/motion/Calibration.h src/motion/Calibration.cpp src/motion/Defense.cpp inc/motion/Defense.h src/motion/Impulse.cpp inc/motion/Impulse.h src/GamePieces/History.cpp inc/GamePieces/History.h src/GameState.cpp src/GameState.h src/motion/Trajectory.cpp inc/motion/Trajectory.h)
link_directories(${OpenCV_LIB_DIR})
target_link_libraries( AirHockeyRobot ${OpenCV_LIBS} )

