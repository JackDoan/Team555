//
// Created by jad140230 on 1/21/2018.
//

#ifndef AIRHOCKEYROBOT_TABLE_H
#define AIRHOCKEYROBOT_TABLE_H


// All units in milimeters?

#include "Camera.h"
#include "helpers.h"


class Table {
public:


    cv::Point_<int> max, min, motionLimitMin, motionLimitMax, strikeLimitMin, strikeLimitMax;
    cv::Rect_<int> motionLimit;
    static const cv::Point_<int> home;
    static cv::Point_<int> pixelsToSteps(cv::Point_<int> pixels);
    cv::Point_<int> stepsToPixels(cv::Point_<int> steps);

    void setLimits(std::vector<cv::Point_<int>> sortedX, std::vector<cv::Point_<int>> sortedY);
    int preview = 1;


    void annotate(cv::Mat);
    Table() =default;
    ~Table();
};

#endif //AIRHOCKEYROBOT_TABLE_H
