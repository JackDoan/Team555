//
// Created by jad140230 on 1/21/2018.
//

#include <opencv/cv.h>
#include <opencv/cv.hpp>
#include "../inc/Camera.h"
#include "../inc/Table.h"
#include <vector>


//using namespace cv;


Table::~Table() = default;
const cv::Point_<int> Table::home = cv::Point_<int>(1160,370);
cv::Point_<int> Table::max;
cv::Point_<int> Table::min;
cv::Point_<int> Table::strikeLimitMin;
cv::Point_<int> Table::strikeLimitMax;
cv::Point_<int> Table::motionLimitMin;
cv::Point_<int> Table::motionLimitMax;
cv::Rect Table::motionLimit;

void Table::setLimits(std::vector<cv::Point_<int>> sortedX, std::vector<cv::Point_<int>> sortedY) {
    min.x = sortedX[0].x; printf("Setting min X: %d\n", min.x);
    max.x = sortedX[3].x; printf("Setting max X: %d\n", max.x);
    min.y = sortedY[0].y; printf("Setting min Y: %d\n", min.y);
    max.y = sortedY[3].y; printf("Setting max Y: %d\n", max.y);

    //todo make these ratios                adjust drawGoalVector after made into ratios
    motionLimitMin.y = min.y + 80;
    motionLimitMax.y = max.y - 80;
    motionLimitMin.x = min.x + 780;
    motionLimitMax.x = max.x - 30;
    motionLimit = cv::Rect_<int>(motionLimitMin,motionLimitMax);

    strikeLimitMin.y = motionLimitMin.y + 40;
    strikeLimitMax.y = motionLimitMax.y - 40;
    strikeLimitMin.x = motionLimitMin.x + 30;
    strikeLimitMax.x = motionLimitMax.x - 30;


}

cv::Point_<int> Table::pixelsToSteps(cv::Point_<int> pixels) {
    return 4*(pixels - home);
}

cv::Point_<int> Table::stepsToPixels(cv::Point_<int> steps) {
    return (steps/4) + home;
}