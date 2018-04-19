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
            printf("Calibration::home at (%d,%d), going to (%d,%d), off by (%d,%d)\n",
                   mallet.location.x, mallet.location.y,  Table::home.x, Table::home.y, difference.x, difference.y);
        }

        display();
        if (cv::waitKey(80) >= 0) { break; }
    }
    printf("Homing complete!\n");
}

std::vector<Calibration::pointAndTime> Calibration::moveTo(Mallet& mallet, const cv::Point_<int>& destination) {
    MotorDriver& motorDriver = MotorDriver::getInstance();
    double timeDelta = 0.0;
    clock_t start;
    bool done = false;
    start = clock();
    std::vector<Calibration::pointAndTime> locsTimes;
    Calibration::pointAndTime tmp;
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
            //printf("Motion::calGoto: Moving from (%d,%d) to (%d,%d)\n",mallet.location.x, mallet.location.y,  destination.x, destination.y);
            motorDriver.moveTo(destination);
        }
        if (isAt(mallet.location, destination, 8)) {
            done = true;
        }

        display();
        if (cv::waitKey(1) >= 0) { break; }
    }
    return locsTimes;
}

void pATtoCSV(const std::vector<Calibration::pointAndTime>& in, const char* name) {
    auto f = fopen(name, "w");
    char b[256];
    //for(int i = 0; i < in.size(); i++) {
    for(const Calibration::pointAndTime& i : in) {
        //std::cout << in[i].time << "," << in[i].location.x << "," << in[i].location.y << std::endl;
        snprintf(b, sizeof(b), "%f,%d,%d\n", i.time, i.location.x, i.location.y);
        //printf(b);
//        fwrite(b, sizeof(char), strlen(b), f);
        fprintf(f, "%f,%d,%d\n", i.time, i.location.x, i.location.y);
    }
    fflush(f);
    fclose(f);
}

void Calibration::speed() {
    Mallet mallet;
//    moveTo(mallet, {Table::home.x, Table::strikeLimitMax.y});
//    moveTo(mallet, {Table::home.x, Table::strikeLimitMin.y});
//    moveTo(mallet, {Table::strikeLimitMax.x, Table::strikeLimitMax.y});
//    moveTo(mallet, {Table::strikeLimitMin.x, Table::strikeLimitMax.y});
    const std::vector<Calibration::pointAndTime>* things[] = {&up, &left, &down, &right, &home2Top, &home2Left, &home2Bottom, &home2Right};
    std::vector<const char*> thingNames = {"Up", "Left", "Down", "Right", "home2Top", "home2Left", "home2Bottom", "home2Right"};
    moveTo(mallet, Table::strikeLimitMax);
    up = moveTo(mallet, {Table::strikeLimitMax.x, Table::strikeLimitMin.y});
    left = moveTo(mallet, Table::strikeLimitMin);
    down = moveTo(mallet, {Table::strikeLimitMin.x, Table::strikeLimitMax.y});
    right = moveTo(mallet, Table::strikeLimitMax);
    home2Top = moveTo(mallet, {Table::home.x, Table::strikeLimitMin.y});
    moveTo(mallet, Table::home);
    home2Left = moveTo(mallet, {Table::strikeLimitMin.x, Table::home.y});
    moveTo(mallet, Table::home);
    home2Bottom = moveTo(mallet, {Table::home.x, Table::strikeLimitMax.y});
    moveTo(mallet, Table::home);
    home2Right = moveTo(mallet, {Table::strikeLimitMax.x, Table::home.y});
    moveTo(mallet, Table::home);

    for (int i = 0; i < 8; i++) {
        char name[64];
        snprintf(name, sizeof(name), "%s.csv", thingNames[i]);
        pATtoCSV(*things[i], name);
    }

    printf("up: %f\nleft: %f\ndown: %f\nright: %f\n", up.back().time, left.back().time, down.back().time, right.back().time);
    printf("home2Top: %f\nhome2Left: %f\nhome2Bottom: %f\nhome2Right: %f\n", home2Top.back().time, home2Left.back().time, home2Bottom.back().time, home2Right.back().time);

    // todo: test this
    double upDelay = calculateTXDelay(up);
    printf("up TX delay = %f\n", upDelay);
    double leftDelay = calculateTXDelay(left);
    printf("left TX delay = %f\n", leftDelay);
}


double Calibration::calculateTXDelay(std::vector<Calibration::pointAndTime> input) {
    for (int i = 1; i < input.size(); i++) {
        if (abs(input[i].location.x - input[i-1].location.x) > 5 || abs(input[i].location.y - input[i-1].location.y) > 5) {
            return input[i].time;
        }
    }
    return input.back().time;
}
