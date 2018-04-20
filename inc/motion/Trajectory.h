//
// Created by mike on 4/20/18.
//

#ifndef TEAM555_TRAJECTORY_H
#define TEAM555_TRAJECTORY_H


#include "../../src/GameState.h"

class Trajectory {
private:
    static const int vectorMult;
    static std::vector<bool> bounceDetect(const cv::Point_<int>& startPoint, const cv::Point_<int>& endPoint, const int& bnccnt);
public:

    static const cv::Point_<int> findIntersection(const std::vector<bool>& bounces, const cv::Point_<int>& startPoint, const cv::Point_<int>& endPoint);
    static std::vector<std::vector<cv::Point_<int>>> calculate(GameState& gs);
    static cv::Point_<int> predictLocation(GamePiece& gp, int frames);
};


#endif //TEAM555_TRAJECTORY_H
