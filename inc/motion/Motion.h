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
    void trackPredictedY(Table table, Mallet mallet, Puck puck, cv::Mat grabbed);
    cv::Point_<int> defend(const Table& table, Mallet& mallet, Puck& puck, cv::Mat& grabbed);
    bool attack(const Table& table, Mallet& mallet, Puck& puck, cv::Mat& grabbed);
    bool stillStaging (cv::Point_<int> malletloc, cv::Point_<int> staging);
    std::vector<cv::Point_<int>> hitVector;
    std::vector<cv::Point_<int>> findHitVector(const Table& table, Mallet& mallet, Puck& puck, cv::Mat& grabbed);
    std::vector<cv::Point_<int>> findImpulseVector(const Table& table, Mallet& mallet, Puck& puck, cv::Mat& grabbed);
    void calcPuckRange(cv::Point_<double> mallet2puck, cv::Point_<int>& puckPlus, cv::Point_<int>& puckMinus);
    typedef enum attackState_e {ATTACKDONE, STAGING, ATTACKING} attackState_t;
    attackState_t attackState = ATTACKDONE;
    std::vector<cv::Point_<int>> impulseVector;
    bool impulse(Table& table, Mallet& mallet, Puck& puck, cv::Mat& grabbed);
    typedef enum impulseState_e {IMPULSEDONE, STAGING1, STAGING2, HITTING} impulseState_t;
    impulseState_t impulseState = IMPULSEDONE;
    void resetOffense();
    Motion();
    ~Motion() = default;

};
#endif //TEAM555_MOTION_H
