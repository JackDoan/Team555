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
Table::Corners Table::corners;
Table::Goals Table::goals = Goals(Table::corners);
double Table::walls[4][3];

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

    setWalls(corners.sortedX, corners.sortedY);


}

const cv::Point_<int> Table::pixelsToSteps(const cv::Point_<int>& pixels) {
    return 4*(pixels - home);
}

const cv::Point_<int> Table::stepsToPixels(const cv::Point_<int>& steps) {
    return (steps/4) + home;
}
void Table::setWalls(const std::vector<cv::Point_<int>>& sortedX, const std::vector<cv::Point_<int>>& sortedY) {
    /*
     * This calculates the Ax + By = C line equations for the walls of the table
     * A = y2-y1, B = x1-x2, C = A*x1 + B*y1
     */

    // Calculate the left (first) wall with the two minimum x corners
    walls[0][0] = sortedX[1].y - sortedX[0].y;
    walls[0][1] = sortedX[0].x - sortedX[1].x;
    walls[0][2] = walls[0][0]*sortedX[0].x + walls[0][1]*sortedX[0].y;

    // Calculate the bottom (second) wall with the two maximum y corners
    walls[1][0] = sortedY[3].y - sortedY[2].y;
    walls[1][1] = sortedY[2].x - sortedY[3].x;
    walls[1][2] = walls[1][0]*sortedY[2].x + walls[1][1]*sortedY[2].y;

    // Calculate the right (third) wall with the two maximum x corners
    walls[2][0] = sortedX[3].y - sortedX[2].y;
    walls[2][1] = sortedX[2].x - sortedX[3].x;
    walls[2][2] = walls[2][0]*sortedX[2].x + walls[2][1]*sortedX[2].y;

    // Calculate the top (fourth) wall with the two minimum y corners
    walls[3][0] = sortedY[1].y - sortedY[0].y;
    walls[3][1] = sortedY[0].x - sortedY[1].x;
    walls[3][2] = walls[3][0]*sortedY[0].x + walls[3][1]*sortedY[0].y;
}

const Table::Goals::goal_t Table::Goals::detect(const cv::Point_<int>& intersection, const int& xvelo) {
    if (xvelo > 0 && intersection.y >= goals[3].y && intersection.y <= goals[2].y) {
        return RIGHT_GOAL;
    }
    else if (xvelo < 0 && intersection.y >= goals[1].y && intersection.y <= goals[2].y) {
        return LEFT_GOAL;
    }
    else {
        return NO_GOAL;
    }
}
