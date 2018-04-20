//
// Created by mdl150330 on 3/19/2018.
//

#ifndef TEAM555_MOTION_H
#define TEAM555_MOTION_H

#include "../MotorDriver.h"
#include "../Camera.h"
#include "../Table.h"
#include "../Settings.h"
#include "../states.h"
#include "Calibration.h"
#include "Defense.h"
#include "Offense.h"
#include "Impulse.h"


class Motion {

private:

public:
    Offense offense;
    Calibration calibrate;
    Defense defense;
    Impulse impulse;

    static hitVector_t findHitVector(GameState& gs, int ff);
    void calcPuckRange(cv::Point_<double> mallet2puck, cv::Point_<int>& puckPlus, cv::Point_<int>& puckMinus);

    Motion() = default;
    ~Motion() = default;

    const static bool stillStaging(const cv::Point_<int>& malletloc, const cv::Point_<int>& staging);
    const static bool notAt(const cv::Point_<int>& at, const cv::Point_<int>& desired, const int& tolerance);
    const static bool isAt(const cv::Point_<int>& at, const cv::Point_<int>& desired, const int& tolerance);


};
#endif //TEAM555_MOTION_H
