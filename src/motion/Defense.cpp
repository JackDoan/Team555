////
//// Created by mike on 4/19/18.
////
//
//#include "../../inc/motion/Defense.h"
//#include "../../inc/MotorDriver.h"
//#include "../../inc/motion/Motion.h"
//
//
//bool Defense::checkPastgoalFlags(std::vector<bool> goalFlagHistory) {
//    for (int i = 0; i < goalFlagHistory.size(); i++) {
//        if (goalFlagHistory[i])
//            return true;
//    }
//}
//
////stolen from https://stackoverflow.com/questions/7446126/opencv-2d-line-intersection-helper-function?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa
//bool intersection(const cv::Point_<int>& o1, const cv::Point_<int>& p1, const cv::Point_<int>& o2, const cv::Point_<int>& p2,
//                  cv::Point_<int> &r)
//{
//    cv::Point_<int> x = o2 - o1;
//    cv::Point_<int> d1 = p1 - o1;
//    cv::Point_<int> d2 = p2 - o2;
//
//    float cross = d1.x*d2.y - d1.y*d2.x;
//    if (abs(cross) < 1e-8)
//        return false;
//
//    auto t1 = (x.x * d2.y - x.y * d2.x)/cross;
//    r = o1 + d1 * t1;
//    return true;
//}
//
//cv::Point_<int> Defense::run(GameState& gs, cv::Point_<int> intersection) {
//
//    //cv::Point_<int> desiredLocation = Table::home;
//    defenseDecision_t defenseDecision;
//    static cv::Point_<int> interceptSpot = Table::home;
//    cv::Point_<int> desiredLocation = Table::home;
//    // make defense decision based on current states and other inputs
//    if (GameStateManager::checkPastGoalFlags()) {
//        defenseDecision = INTERCEPT;
//        //calculate an intercept spot right off the bat if in THIS frame we think we are being scored on
//        if (gs.goalFlag == Table::Goals::RIGHT_GOAL) { ///you need this or you segfault
//            interceptSpot = gs.puckTraj.back()[0] + (gs.puckTraj.back()[1] - gs.puckTraj.back()[0]) * 0.7;
//            for(auto leg :gs.puckTraj) {
//                cv::Point_<int> intersectPoint;
//                if(intersection(leg[0], leg[1], {Table::home.x, Table::motionLimitMax.y}, {Table::home.x, Table::motionLimitMin.y}, intersectPoint)) {
//                    desiredLocation = intersectPoint;
//                    break;
//                }
//            }
//        }
//    }
//    else {
//        interceptSpot = Table::home; //just in case
//        defenseDecision = GOHOME;
//    }
//
//    // determine what state defense is in
//    if (defenseDecision == INTERCEPT) {
//        if (Motion::notAt(gs.mallet.location, interceptSpot, 10)) {
//            state = INTERCEPTING;
//        }
//        else {
//            state = ATINTERCEPT;
//        }
//    }
//    else {
//        if (Motion::notAt(gs.mallet.location, Table::home, 10)) {
//            state = GOINGHOME;
//        }
//        else {
//            state = ATHOME;
//        }
//    }
//
//    // based on decision state and current state issue move commands
//    switch (state) {
//        case INTERCEPTING:
//            desiredLocation = interceptSpot;
//            cv::putText(gs.frame, "moving", cvPoint(1100, 665), cv::FONT_HERSHEY_SIMPLEX, 1, defenseColor, 2);
//            break;
//        case ATINTERCEPT:
//            desiredLocation = interceptSpot;
//            break;
//
//        case GOINGHOME:
//            desiredLocation = Table::home;
//            cv::putText(gs.frame, "moving", cvPoint(1100, 665), cv::FONT_HERSHEY_SIMPLEX, 1, defenseColor, 2);
//            break;
//        default:
//        case ATHOME:
//            desiredLocation = Table::home;
//            break;
//    }
//
//    desiredLocation = saturate(intersection, Table::motionLimitMin, Table::motionLimitMax);
//    if (gs.mallet.onTable) {
//        MotorDriver::getInstance().moveTo(desiredLocation);
//    }
//    // draw desired location on the frame
//    cv::circle(gs.frame, desiredLocation, 20, defenseColor, 6);
//    // return moveTo position
//    return desiredLocation;
//
//}
//
//defenseState_t Defense::getState() const {
//    return state;
//}
#include "../../inc/motion/Defense.h"
#include "../../inc/MotorDriver.h"
#include "../../inc/motion/Motion.h"


bool Defense::checkPastgoalFlags(std::vector<bool> goalFlagHistory) {
    for (int i = 0; i < goalFlagHistory.size(); i++) {
        if (goalFlagHistory[i])
            return true;
    }
}

cv::Point_<int> Defense::run(GameState& gs) {

    static cv::Point_<int> interceptSpot = Table::home;
    cv::Point_<int> desiredLocation = Table::home;
    defenseDecision_t defenseDecision;

    // make defense decision based on current states and other inputs
    if (GameStateManager::checkPastGoalFlags()) {
        defenseDecision = INTERCEPT;
        //calculate an intercept spot right off the bat if in THIS frame we think we are being scored on
        if (gs.goalFlag == Table::Goals::RIGHT_GOAL) { ///you need this or you segfault
            interceptSpot = gs.puckTraj.back()[0] + (gs.puckTraj.back()[1] - gs.puckTraj.back()[0]) * 0.7;
        }
    }
    else {
        interceptSpot = Table::home; //just in case
        defenseDecision = GOHOME;
    }

    // determine what state defense is in
    if (defenseDecision == INTERCEPT) {
        if (Motion::notAt(gs.mallet.location, interceptSpot, 10)) {
            state = INTERCEPTING;
        }
        else {
            state = ATINTERCEPT;
        }
    }
    else {
        if (Motion::notAt(gs.mallet.location, Table::home, 10)) {
            state = GOINGHOME;
        }
        else {
            state = ATHOME;
        }
    }

    // based on decision state and current state issue move commands
    switch (state) {
        case INTERCEPTING:
            desiredLocation = interceptSpot;
            cv::putText(gs.frame, "moving", cvPoint(1100, 665), cv::FONT_HERSHEY_SIMPLEX, 1, defenseColor, 2);
            break;
        case ATINTERCEPT:
            desiredLocation = interceptSpot;
            break;

        case GOINGHOME:
            desiredLocation = Table::home;
            cv::putText(gs.frame, "moving", cvPoint(1100, 665), cv::FONT_HERSHEY_SIMPLEX, 1, defenseColor, 2);
            break;
        default:
        case ATHOME:
            desiredLocation = Table::home;
            break;
    }

    desiredLocation = saturate(desiredLocation, Table::motionLimitMin, Table::motionLimitMax);
    if (gs.mallet.onTable) {
        MotorDriver::getInstance().moveTo(desiredLocation);
    }
    // draw desired location on the frame
    cv::circle(gs.frame, desiredLocation, 20, defenseColor, 6);
    // return moveTo position
    return desiredLocation;

}

defenseState_t Defense::getState() const {
    return state;
}