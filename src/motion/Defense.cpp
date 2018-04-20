//
// Created by mike on 4/19/18.
//

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
