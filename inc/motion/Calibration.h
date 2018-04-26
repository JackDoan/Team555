//
// Created by mike on 4/18/18.
//

#ifndef TEAM555_CALIBRATION_H
#define TEAM555_CALIBRATION_H

#include "../GameState.h"

class Calibration {

private:
    cv::Mat calSmall;
    bool getFrame();
    void display();
    GameState gs;
    void generatePointField(int length);

    clock_t yHome2EdgeTime;
    clock_t yEdge2EdgeTime;
    clock_t xHome2EdgeTime;
    clock_t xEdge2EdgeTime;

public:
    void speed();
    void home();
    double calcTXDelay(double edge2EdgeTime, double home2EdgeTime, double edge2EdgeDist, double home2EdgeDist);
    typedef struct pointAndTime_s {
        cv::Point_<int> location;
        double time;
    } pointAndTime;
    pointAndTime moveTo(const cv::Point_<int>& destination);
    double calculateTXDelay(std::vector<Calibration::pointAndTime> input);
    void run();

    std::vector<std::vector<cv::Point_<int>>> pointField;
    std::vector<std::vector<Calibration::pointAndTime>> speedNumbers;

    std::vector<pointAndTime> up;
    std::vector<pointAndTime> left;
    std::vector<pointAndTime> down;
    std::vector<pointAndTime> right;
    std::vector<pointAndTime> home2Top;
    std::vector<pointAndTime> home2Left;
    std::vector<pointAndTime> home2Bottom;
    std::vector<pointAndTime> home2Right;

//    double upTime;
//    double leftTime;
//    double downTime;
//    double rightTime;
//    double home2Top;
//    double home2Left;
//    double home2Bottom;
//    double home2Right;
};


#endif //TEAM555_CALIBRATION_H
