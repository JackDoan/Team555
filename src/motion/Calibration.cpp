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

const bool isAt(const cv::Point_<int>& at, const cv::Point_<int>& desired, const int& tolerance) {
    return abs(at.x - desired.x) < tolerance && abs(at.y - desired.y) < tolerance;
}

void Calibration::display() {
    cv::resize(calGrabbed, calSmall, cv::Size(), 0.5, 0.5);
    cv::cvtColor(calSmall, calSmall, cv::COLOR_HSV2RGB);
    imshow("Video", calSmall);
}

bool Calibration::getFrame() {
    Camera& camera = Camera::getInstance();
    bool toReturn = true;
    calGrabbed = camera.getFrame();

    if (calGrabbed.empty()) {
        printf("No frames!\n");
        toReturn = false;
    }

    return toReturn;
}

void Calibration::home() {

    MotorDriver& motorDriver = MotorDriver::getInstance();
    const int homeDecayMax = 5;
    Mallet mallet = Mallet();
    bool home = false;
    int homeDecay = 0;

    while (!home) {

        if(!getFrame()) { break; }
        mallet.findOne(calGrabbed, true);
        if (!mallet.found) {
            printf("Mallet not found in calibrateHome run!\n");
            if (cv::waitKey(100) >= 0)
                continue;
        }
        else if(isAt(mallet.location,Table::home, 3)) {
            if(++homeDecay >= homeDecayMax) {
                home = true;
                motorDriver.setHome();
            }
        }
        else {
            homeDecay = 0;
            auto difference = Table::home - mallet.location;
            motorDriver.moveBy(difference);
        }

        display();
        if (cv::waitKey(80) >= 0) { break; }
    }
    printf("Homing complete!\n");
}

double Calibration::moveTo(Mallet& mallet, const cv::Point_<int>& destination) {
    MotorDriver& motorDriver = MotorDriver::getInstance();
    double timeDelta = 0.0;
    bool done = false;
    while(!done) {

        if(!getFrame()) { break; }
        mallet.findOne(calGrabbed, true);

        if (!mallet.found) {
            printf("Mallet not found!\n");
            if (cv::waitKey(100) >= 0)
                continue;
        }
        else {
            printf("Motion::calGoto: Moving from (%d,%d) to (%d,%d)\n",mallet.location.x, mallet.location.y,  destination.x, destination.y);
            motorDriver.moveTo(destination);
        }
        if (isAt(mallet.location, destination, 8)) {
            done = true;
        }

        display();
        if (cv::waitKey(80) >= 0) { break; }
    }
    return timeDelta;
}

void Calibration::speed() {
    Mallet mallet;
    clock_t now;
//    moveTo(mallet, {Table::home.x, Table::strikeLimitMax.y});
//    moveTo(mallet, {Table::home.x, Table::strikeLimitMin.y});
//    moveTo(mallet, {Table::strikeLimitMax.x, Table::strikeLimitMax.y});
//    moveTo(mallet, {Table::strikeLimitMin.x, Table::strikeLimitMax.y});
    moveTo(mallet,Table::strikeLimitMax);
    moveTo(mallet,{Table::strikeLimitMin.x, Table::strikeLimitMax.y});
    moveTo(mallet,Table::strikeLimitMin);
    moveTo(mallet,{Table::strikeLimitMax.x, Table::strikeLimitMin.y});
    moveTo(mallet, Table::home);
}
