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
#include "../../inc/motion/Motion.h"
#include "../../inc/Supervisor.h"

void Calibration::display() {
    cv::resize(gs.frame, calSmall, cv::Size(), 0.5, 0.5);
    cv::cvtColor(calSmall, calSmall, cv::COLOR_HSV2RGB);
    imshow("Video", calSmall);
    if (Settings::video_output) {
        Supervisor::video.write(calSmall);
    }
}

bool Calibration::getFrame() {
    Camera& camera = Camera::getInstance();
    bool toReturn = true;

    auto mat = camera.getFrame();
    gs = GameStateManager::get(mat);

    if (gs.frame.empty()) {
        printf("No frames!\n");
        toReturn = false;
    }

    return toReturn;
}

void Calibration::run() {
    static bool speedDone = false;
    home();
    if(!speedDone) {
        speed();
        speedDone = true;
    }
}

void Calibration::home() {

    MotorDriver& motorDriver = MotorDriver::getInstance();
    const int homeDecayMax = 10;
    bool home = false;
    int homeDecay = 0;

    while (!home) {

        if(!getFrame()) { break; }
        if (!gs.mallet.found) {
            printf("Mallet not found in calibrateHome run!\n");
            if (cv::waitKey(100) >= 0)
                continue;
        }
        else if(Motion::isAt(gs.mallet.location,Table::home, 3)) {
            if(++homeDecay >= homeDecayMax) {
                home = true;
                motorDriver.setHome();
            }
        }
        else {
            homeDecay = 0;
            auto difference = Table::home - gs.mallet.location;
            motorDriver.moveBy(difference);
            printf("Calibration::home at (%d,%d), going to (%d,%d), off by (%d,%d)\n",
                   gs.mallet.location.x, gs.mallet.location.y,  Table::home.x, Table::home.y, difference.x, difference.y);
        }

        display();
        if (cv::waitKey(80) >= 0) { break; }
    }
    printf("Homing complete!\n");
}

double Calibration::moveTo(const cv::Point_<int>& destination) {
    MotorDriver& motorDriver = MotorDriver::getInstance();
    double timeDelta = 0.0;
    clock_t start;
    bool done = false;
    start = clock();
    Calibration::pointAndTime tmp;
    while(!done) {

        if(!getFrame()) { break; }
        tmp.location = destination;
        tmp.time = ((clock() - start) / (double) CLOCKS_PER_SEC);
        if (!gs.mallet.found) {
            printf("Mallet not found!\n");
            if (cv::waitKey(1) >= 0)
                continue;
        }
        else {
            printf("Motion::calGoto: Moving from (%d,%d) to (%d,%d)\n",gs.mallet.location.x, gs.mallet.location.y,  destination.x, destination.y);
            motorDriver.moveTo(destination);
        }
        if (Motion::isAt(gs.mallet.location, destination, 5)) {
            done = true; //todo 30 is too big
         }

        display();

        if (cv::waitKey(1) >= 0) { break; }
    }
    return tmp.time;
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

bool isSquare(int n) {
    if (n < 0)
        return false;
    int root(round(sqrt(n)));
    return n == root * root;
}


void Calibration::generatePointField (int length) {

    if (!isSquare(length)) {
        printf("Point field was not generated, input was not square\n");
        return;
    }

    int xlen = Table::motionLimitMax.x - Table::motionLimitMin.x;
    int ylen = Table::motionLimitMax.y - Table::motionLimitMin.y;
    double xstep = (double)xlen/(double)length;
    double ystep = (double)ylen/(double)length;

    int xpos, ypos;
    std::vector<cv::Point_<int>> rowVector;

    for (int i = 0; i < length; i++) {
        ypos = round(Table::motionLimitMin.y + ystep * i);
        for (int j = 0; j < length; j++) {
            xpos = round(Table::motionLimitMin.x + xstep * j);
            rowVector.emplace_back(xpos, ypos);
        }
        pointField.emplace_back(rowVector);
    }

}


void Calibration::speed() {
    int fieldSize = 4;
    generatePointField(fieldSize);

    while (cvWaitKey(1) != 't') {
        moveTo(Table::home);
        cvWaitKey(200);
        for (int i = 0; i < pointField.size(); i++) {
            std::vector<double> rowVector;
            for (int j = 0; j < pointField[i].size(); j++) {
                rowVector.emplace_back(moveTo(pointField[i][j]));
                cvWaitKey(200);
            }
            speedNumbers.emplace_back(rowVector);
        }
    }

    /*for (int i = 0; i < 8; i++) {
        char name[64];
        snprintf(name, sizeof(name), "%d.csv", i);
        pATtoCSV(*things[i], name);
    }*/


}


double Calibration::calculateTXDelay(std::vector<Calibration::pointAndTime> input) {
    for (int i = 1; i < input.size(); i++) {
        if (abs(input[i].location.x - input[i-1].location.x) > 5 || abs(input[i].location.y - input[i-1].location.y) > 5) {
            return input[i].time;
        }
    }
    return input.back().time;
}

