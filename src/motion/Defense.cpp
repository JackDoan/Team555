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

cv::Point_<int> Defense::run(Mallet& mallet, Puck& puck, cv::Mat& grabbed) {
    const auto defenseColor = cv::Scalar(225, 255, 255);
    static cv::Point_<int> interceptSpot = Table::home;
    cv::Point_<int> desiredLocation = Table::home;
    MotorDriver& motorDriver = MotorDriver::getInstance();
    defenseDecision_t defenseDecision;

    // make defense decision based on current states and other inputs
    if (checkPastgoalFlags(puck.rightGoalHistory)) {
        defenseDecision = INTERCEPT;
        //calculate an intercept spot right off the bat if in THIS frame we think we are being scored on
        if (puck.rightGoal) { ///you need this or you segfault
            interceptSpot = puck.trajectory.back()[0] + (puck.trajectory.back()[1] - puck.trajectory.back()[0]) * 0.7;
        }
    }
    else {
        interceptSpot = Table::home; //just in case
        defenseDecision = GOHOME;
    }

    // determine what state defense is in
    if (defenseDecision == INTERCEPT) {
        if (Motion::notAt(mallet.location, interceptSpot, 10)) {
            state = INTERCEPTING;
        }
        else {
            state = ATINTERCEPT;
        }
    }
    else {
        if (Motion::notAt(mallet.location, Table::home, 10)) {
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
            cv::putText(grabbed, "moving", cvPoint(1100, 665), cv::FONT_HERSHEY_SIMPLEX, 1, defenseColor, 2);
            break;
        case ATINTERCEPT:
            desiredLocation = interceptSpot;
            break;

        case GOINGHOME:
            desiredLocation = Table::home;
            cv::putText(grabbed, "moving", cvPoint(1100, 665), cv::FONT_HERSHEY_SIMPLEX, 1, defenseColor, 2);
            break;
        default:
        case ATHOME:
            desiredLocation = Table::home;
            break;
    }

    desiredLocation = saturate(desiredLocation, Table::motionLimitMin, Table::motionLimitMax);
    if (mallet.onTable) {
        motorDriver.moveTo(desiredLocation);
    }
    // draw desired location on the frame
    cv::circle(grabbed, desiredLocation, 20, defenseColor, 6);
    // return moveTo position
    return desiredLocation;

}

defenseState_t Defense::getState() const {
    return state;
}
