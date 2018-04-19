//
// Created by mike on 4/18/18.
//

#ifndef TEAM555_CALIBRATION_H
#define TEAM555_CALIBRATION_H

#include "../Mallet.h"

class Calibration {

private:
    cv::Mat calGrabbed, calSmall;
    bool getFrame();
    void display();
    double moveTo(Mallet& mallet, const cv::Point_<int>& destination);
    clock_t yHome2EdgeTime;
    clock_t yEdge2EdgeTime;
    clock_t xHome2EdgeTime;
    clock_t xEdge2EdgeTime;

public:
    void speed();
    void home();
    double calcTXDelay(double edge2EdgeTime, double home2EdgeTime, double edge2EdgeDist, double home2EdgeDist);

    struct pointAndTime {
        cv::Point_<int> location;
        double time;
    };
    double upTime;
    double leftTime;
    double downTime;
    double rightTime;
    double home2Top;
    double home2Left;
    double home2Bottom;
    double home2Right;
};


#endif //TEAM555_CALIBRATION_H
