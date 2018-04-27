#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/core/opengl.hpp>
#include <opencv2/highgui.hpp>
#include <opencv/cv.hpp>
#include <opencv2/imgproc.hpp>
#ifdef WINDOWS
    #include <sysinfoapi.h>
#endif
#include <string>
#include <thread>
#include <ctime>

#include "../inc/Table.h"
#include "../inc/Camera.h"
#include "../inc/Corners.h"
#include "../inc/Serial.h"
#include "../inc/MotorDriver.h"
#include "../inc/Config.h"
#include "../inc/Supervisor.h"

#include <time.h>


int main(int argc, char* argv[]) {
    Settings::readConfigValues();
    Table::setup();
    Supervisor supervisor = Supervisor();
    supervisor.run();
    cvDestroyAllWindows();
    Camera::getInstance().close();
    MotorDriver::getInstance().stop();
    return 0;
}