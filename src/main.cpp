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
#include "../inc/Puck.h"
#include "../inc/Camera.h"
#include "../inc/Corners.h"
#include "../inc/Serial.h"
#include "../inc/MotorDriver.h"
#include "../inc/Config.h"
#include "../inc/Mallet.h"
#include "../inc/Supervisor.h"

#include <time.h>

///Jack's settings:
///Exposure: 1/170, Gain: 6.59db, Sharpness: 3, Gamma: 66, Denoise: 25, Sat: 255

int main(int argc, char* argv[]) {
    Settings settings;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            // TODO: Calibrate mode is broken, threshold is not working properly, cant see the pucks
            if (std::string(argv[i]) == "--calibrate") {
                settings.calibrateCorners = true;
                printf("calibrateCorners set to true!\n");
            } else {
                settings.calibrateCorners = false;
                printf("calibrateCorners set to false!\n");
            }
        }
//        printf("%s\n", std::string(argv[2]));
    }


// TODO: need to create a debug mode that allows us to turn on and off every possible debugging feature we could use
// like trackbars for thresholding, thresholded images, writing and not writing the video, maybe even wrap all
// all of our printfs for debuggin in a debug version of printf?


    Table table = Table();
    Puck puck = Puck();
    Mallet mallet = Mallet();
    //MotorDriver motorDriver = MotorDriver();
    Corners corners = Corners(settings.calibrateCorners);
    if (!settings.calibrateCorners) {
        table.setLimits(corners.sortedX, corners.sortedY);
        puck.setWalls(corners.sortedX, corners.sortedY);
    }
    Supervisor supervisor = Supervisor(table, puck, mallet, corners, settings);

    if (settings.calibrateCorners) {
        supervisor.runCalibrateCorners();
    } else {
        supervisor.run();
    }

    cvDestroyAllWindows();
    Camera::getInstance().close();

    return 0;
}