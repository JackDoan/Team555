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
#include "../states.h"
#include "Calibration.h"

class Motion {

private:
public:
    clock_t getTmpClk() const;

    clock_t getYHome2EdgeTime() const;

    clock_t getYEdge2EdgeTime() const;

    clock_t getXHome2EdgeTime() const;

    clock_t getXEdge2EdgeTime() const;

private:
    double calGoto(Mallet& mallet, const cv::Point_<int>& destination, time_t& beginTime, time_t& endTime);
    clock_t yHome2EdgeTime;
    clock_t yEdge2EdgeTime;
    clock_t xHome2EdgeTime;
    clock_t xEdge2EdgeTime;
public:

    Calibration calibrate;

    void calibrateSpeed(Mallet& mallet);
    bool hitVectorFound = false;
    bool staging = false;
    bool striking = false;
    bool attackDone = false;
    void trackPredictedY(Table table, Mallet mallet, Puck puck, cv::Mat grabbed);
//    typedef enum defenseState_e {ATHOME, GOINGHOME, INTERCEPTING, ATINTERCEPT} defenseState_t;
    defenseState_t defenseState = ATHOME;
    typedef enum defenseDecision_e {GOHOME, INTERCEPT} defenseDecision_t;

//    typedef enum offenseState_e {OFFENSEDONE, GOINGTOSTAGE, STRIKING} offenseState_t;
    offenseState_t offenseState = OFFENSEDONE;
    typedef enum offenseDecision_e {STAGE, STRIKE} offenseDecision_t;
    offenseDecision_t offenseDecision = STAGE;
    cv::Point_<int> defense(Mallet& mallet, Puck& puck, cv::Mat& grabbed);
    bool offense(const Table& table, Mallet& mallet, Puck& puck, cv::Mat& grabbed);
    cv::Point_<int> defend(const Table& table, Mallet& mallet, Puck& puck, cv::Mat& grabbed);
    bool attack(const Table& table, Mallet& mallet, Puck& puck, cv::Mat& grabbed);
    bool stillStaging (cv::Point_<int> malletloc, cv::Point_<int> staging);
    std::vector<cv::Point_<int>> hitVector;
    cv::Point_<int> puckplus;
    cv::Point_<int> puckminus;
    cv::Point_<double> mallet2puck;
    std::vector<cv::Point_<int>> findHitVector(const Table& table, Mallet& mallet, Puck& puck, cv::Mat& grabbed, int ff);
    std::vector<cv::Point_<int>> findImpulseVector(const Table& table, Mallet& mallet, Puck& puck, cv::Mat& grabbed);
    void calcPuckRange(cv::Point_<double> mallet2puck, cv::Point_<int>& puckPlus, cv::Point_<int>& puckMinus);
    typedef enum attackState_e {ATTACKDONE, STAGING, ATTACKING} attackState_t;
    attackState_t attackState = ATTACKDONE;
    std::vector<cv::Point_<int>> impulseVector;
    bool impulse(Table& table, Mallet& mallet, Puck& puck, cv::Mat& grabbed);
//    typedef enum impulseState_e {IMPULSEDONE, STAGING1, STAGING2, HITTING} impulseState_t;
    impulseState_t impulseState = IMPULSEDONE;
    void resetOffense();
    Motion();
    ~Motion() = default;


};
#endif //TEAM555_MOTION_H
