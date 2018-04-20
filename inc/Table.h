//
// Created by jad140230 on 1/21/2018.
//

#ifndef AIRHOCKEYROBOT_TABLE_H
#define AIRHOCKEYROBOT_TABLE_H


// All units in milimeters?

#include "Camera.h"
#include "helpers.h"
#include "Corners.h"

// TODO: programatically determine the mid points from corners, why 720/2 and 700/2?
class Goals {
public:

    typedef enum goal_e {NO_GOAL, RIGHT_GOAL, LEFT_GOAL} goal_t;

    cv::Point_<int> goals[4];
    cv::Point_<int> L_mid;
    cv::Point_<int> R_mid;

    cv::Point_<int> L_top;
    cv::Point_<int> L_bottom;

    cv::Point_<int> R_top;
    cv::Point_<int> R_bottom;

    const int goalScale = 50;
    const int goalPush = 0;

    explicit Goals(const Corners& corners) {
        L_mid = {corners.sortedX[1].x, 715/2};
        R_mid = {corners.sortedX[2].x, 740/2};
        L_top = {L_mid.x, L_mid.y + 30 + goalScale};
        L_bottom = {L_mid.x, L_mid.y - 30 - goalScale};
        R_top = {R_mid.x+goalPush, R_mid.y + 40 + goalScale};
        R_bottom = {R_mid.x+goalPush, R_mid.y - 40 - goalScale};
        goals[0] = L_top;
        goals[1] = L_bottom;
        goals[2] = R_top;
        goals[3] = R_bottom;
    }

    const goal_t Goals::detect(const cv::Point_<int>& intersection, const int& xvelo) {
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
};

class Table {
public:

    typedef enum axes_e {X_AXIS, Y_AXIS} axis;

    static cv::Point_<int> max, min, motionLimitMin, motionLimitMax, strikeLimitMin, strikeLimitMax;
    static cv::Rect_<int> motionLimit;
    static const cv::Point_<int> home;
    static const cv::Point_<int> pixelsToSteps(const cv::Point_<int>& pixels);
    static const cv::Point_<int> stepsToPixels(const cv::Point_<int>& steps);
    static void setLimits(std::vector<cv::Point_<int>> sortedX, std::vector<cv::Point_<int>> sortedY);

    static Corners corners;
    static Goals goals;


    Table() =default;
    ~Table();
};




#endif //AIRHOCKEYROBOT_TABLE_H
