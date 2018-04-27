//
// Created by jad140230 on 1/21/2018.
//

#ifndef AIRHOCKEYROBOT_TABLE_H
#define AIRHOCKEYROBOT_TABLE_H


// All units in milimeters?

#include "Camera.h"
#include "helpers.h"
#include "Corners.h"
#include "Settings.h"


class Table {
public:

    class Corners {
        std::vector<cv::Point_<int>> corners;
        std::vector<cv::Point_<int>> offsets{{40, 40}, {-40, 40}, {-40, -40}, {40, -40}};

    public:
        std::vector<cv::Point_<int>> sortedX;
        std::vector<cv::Point_<int>> sortedY;

        void drawSquareNew(cv::Mat previewSmall, std::vector<cv::Point_<int>> calibratedVector);
        void setCorners(std::vector<cv::Point_<int>> cornersVector);
        std::vector<cv::Point_<int>> getCorners();
        std::vector<cv::Point_<int>> getSortedX(std::vector<cv::Point_<int>> calibrated);
        std::vector<cv::Point_<int>> getSortedY(std::vector<cv::Point_<int>> calibrated);
        Corners() : Corners(false) {}
        explicit Corners(bool calibrate);
        ~Corners();
    };


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

        int goalScale = 50;
        int goalPush = 0;

        Goals() = default;

        explicit Goals(const Corners& corners) {
            recalculate(corners);
        }

        void recalculate(const Corners& corners) {
            L_mid = cvPoint(corners.sortedX[1].x, 715/2);
            R_mid = cvPoint(corners.sortedX[2].x, 740/2);
            L_top = {L_mid.x, L_mid.y + 30 + goalScale};
            L_bottom = {L_mid.x, L_mid.y - 30 - goalScale};
            R_top = {R_mid.x+goalPush, R_mid.y + 40 + goalScale};
            R_bottom = {R_mid.x+goalPush, R_mid.y - 40 - goalScale};
            goals[0] = L_top;
            goals[1] = L_bottom;
            goals[2] = R_top;
            goals[3] = R_bottom;
        }

        const goal_t detect(const cv::Point_<int>& intersection, const int& xvelo);
    };



    typedef enum axes_e {X_AXIS, Y_AXIS} axis;

    static cv::Point_<int> max, min, motionLimitMin, motionLimitMax, strikeLimitMin, strikeLimitMax;
    static cv::Rect_<int> motionLimit;
    static const cv::Point_<int> home;
    static const cv::Point_<int> pixelsToSteps(const cv::Point_<int>& pixels);
    static const cv::Point_<int> stepsToPixels(const cv::Point_<int>& steps);
    static void setLimits();
    static void setWalls(const std::vector<cv::Point_<int>>& sortedX, const std::vector<cv::Point_<int>>& sortedY);
    static Corners corners;
    static Goals goals;
    static double walls[4][3];
    static void mouseHelper( int event, int x, int y, int, void* );
    static bool acceptMouseInput;
    static std::vector<cv::Point_<int>> newCorners;

    Table()  =default;
    ~Table();
};




#endif //AIRHOCKEYROBOT_TABLE_H
