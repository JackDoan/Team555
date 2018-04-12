//
// Created by mdl150330 on 3/19/2018.
//

#ifndef TEAM555_MOTION_H
#define TEAM555_MOTION_H

#include "../MotorDriver.h"
#include "../Camera.h"
#include "../Table.h"
#include "../Mallet.h"
#include "../Puck.h"
#include "../Settings.h"

class Motion {

private:

public:
    bool hitVectorFound = false;
    bool staging = false;
    bool striking = false;
    bool attackDone = false;
    void calibrateHome(Table table, Mallet mallet, Settings settings);
    void trackY();
    void trackPredictedY(Table table, Mallet mallet, Puck puck, cv::Mat grabbed);
    void defend(Table table, Mallet mallet, Puck puck, cv::Mat& grabbed);
    void attack(Table table, Mallet mallet, Puck puck, cv::Mat & grabbed);
    bool stillStaging (cv::Point_<int> malletloc, cv::Point_<int> staging);
    std::vector<cv::Point_<int>> hitVector;
    std::vector<cv::Point_<int>> findHitVector(Table table, Mallet mallet, Puck puck, cv::Mat grabbed);
    Motion();
    ~Motion() = default;

};
#endif //TEAM555_MOTION_H
