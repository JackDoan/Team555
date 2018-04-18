//
// Created by mike on 4/18/18.
//

#include <opencv2/core/types.hpp>
#include <opencv/cv.hpp>
#include "../../inc/motion/Calibration.h"
#include "../../inc/Camera.h"
#include "../../inc/MotorDriver.h"
#include "../../inc/Table.h"
#include "../../inc/Settings.h"
#include "../../inc/Mallet.h"

bool isAt(cv::Point_<int> at, cv::Point_<int> desired, int tolerance) {
    return abs(at.x - desired.x) < tolerance && abs(at.y - desired.y) < tolerance;
}


void Calibration::home() {
    Camera& camera = Camera::getInstance();
    MotorDriver& motorDriver = MotorDriver::getInstance();
    const int homeDecayMax = 5;
    Mallet mallet = Mallet();
    Table table; //todo remove
    bool home = false;
    int homeDecay = 0;
    cv::Mat calHomeGrabbed, calHomeSmall;

    while (!home) {

        calHomeGrabbed = camera.getFrame();

        if (calHomeGrabbed.empty()) {
            printf("No frames!\n");
            break;
        }

        mallet.findOne(calHomeGrabbed, table, true);
        cv::resize(calHomeGrabbed, calHomeSmall, cv::Size(), 0.5, 0.5);
        imshow("Video", calHomeSmall);

        if (!mallet.found) {
            printf("Mallet not found in calibrateHome run!\n");
            if (cv::waitKey(100) >= 0)
                continue;
        }
        else if(isAt(mallet.location,Table::home, 3)) {
            if(++homeDecay >= homeDecayMax) {
                home = true;
            }
        }
        else {
            homeDecay = 0;
            auto difference = Table::home - mallet.location;
            motorDriver.moveBy(difference);
        }

        if (cv::waitKey(100) >= 0) { break; }
    }
    // set the current position to the home position
    if(home) {
        motorDriver.setHome();
    }

}