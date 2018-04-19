//
// Created by mike on 4/18/18.
//

#include <opencv2/core/types.hpp>
#include <opencv/cv.hpp>
#include <ctime>
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
    clock_t start, end;
    bool done = false;
    start = clock();
    std::vector<struct pointAndTime> locsTimes;
    struct pointAndTime tmp;
    while(!done) {

        if(!getFrame()) { break; }
        mallet.findOne(calGrabbed, true);
        tmp.location = mallet.location;
        tmp.time = ((clock() - start) / (double) CLOCKS_PER_SEC);
        locsTimes.emplace_back(tmp);
        if (!mallet.found) {
            printf("Mallet not found!\n");
            if (cv::waitKey(1) >= 0)
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
        if (cv::waitKey(1) >= 0) { break; }
    }
    end = clock();
    double toReturn = ((end - start) / (double) CLOCKS_PER_SEC);
    return toReturn;
}

void Calibration::speed() {
    Mallet mallet;
    clock_t now;
//    moveTo(mallet, {Table::home.x, Table::strikeLimitMax.y});
//    moveTo(mallet, {Table::home.x, Table::strikeLimitMin.y});
//    moveTo(mallet, {Table::strikeLimitMax.x, Table::strikeLimitMax.y});
//    moveTo(mallet, {Table::strikeLimitMin.x, Table::strikeLimitMax.y});

    // todo: test this
    moveTo(mallet, Table::strikeLimitMax);
    upTime = moveTo(mallet, {Table::strikeLimitMax.x, Table::strikeLimitMin.y});
    leftTime = moveTo(mallet, Table::strikeLimitMin);
    downTime = moveTo(mallet, {Table::strikeLimitMin.x, Table::strikeLimitMax.y});
    rightTime = moveTo(mallet, Table::strikeLimitMax);
    home2Top = moveTo(mallet, {Table::home.x, Table::strikeLimitMin.y});
    moveTo(mallet, Table::home);
    home2Left = moveTo(mallet, {Table::strikeLimitMin.x, Table::home.y});
    moveTo(mallet, Table::home);
    home2Bottom = moveTo(mallet, {Table::home.x, Table::strikeLimitMax.y});
    moveTo(mallet, Table::home);
    home2Right = moveTo(mallet, {Table::strikeLimitMax.x, Table::home.y});
    moveTo(mallet, Table::home);

    printf("up: %f\nleft: %f\ndown: %f\nright: %f\n", upTime, leftTime, downTime, rightTime);
    printf("home2Top: %f\nhome2Left: %f\nhome2Bottom: %f\nhome2Right: %f\n", home2Top, home2Left, home2Bottom, home2Right);


    // todo: test this
    double delay1 = calcTXDelay(leftTime, home2Left, abs(Table::strikeLimitMax.x - Table::strikeLimitMin.x), abs(Table::home.x - Table::strikeLimitMin.x));
}

double Calibration::calcTXDelay(double edge2EdgeTime, double home2EdgeTime, double edge2EdgeDist, double home2EdgeDist) {
    double eq1[4];
    double eq2[4];
    double velo;
    double delay;
    eq1[0] = edge2EdgeTime;
    eq1[1] = 1;
    eq1[2] = edge2EdgeDist;
    eq2[0] = home2EdgeTime;
    eq2[1] = 1;
    eq2[2] = home2EdgeDist;
    velo = (eq1[0] - eq2[0]) / (eq1[2] - eq2[2]);
    eq1[3] = velo;
    eq2[3] = velo;
    double d1 = (eq1[0] - eq1[2]*eq1[3]);
    double d2 = (eq2[0] - eq2[2]*eq2[3]);
    delay = (d1+d2)/2;

    printf("Velocity: %f\nDelay: %f\n", velo, delay);
    return delay;
}