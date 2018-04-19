//
// Created by mike on 4/19/18.
//

#include "../../inc/motion/Impulse.h"
#include "../../inc/MotorDriver.h"
#include "../../inc/motion/Motion.h"

bool Impulse::run(Mallet& mallet, Puck& puck, cv::Mat &grabbed) {
    MotorDriver& motorDriver = MotorDriver::getInstance();
    bool toReturn = false;

    /////

    switch(impulseState) {
        case IMPULSEDONE:
            // calculate the impulse vector

            // issue move command to stage1
            impulseState = STAGING1;
            break;
        case STAGING1:
            if (!Motion::stillStaging(mallet.location, impulseVector[0])) {
                // issue move command to stage2
                motorDriver.moveTo(impulseVector[1]);
                // set attackState to Attacking
                impulseState = STAGING2;
            }
            break;
        case STAGING2:
            if (!Motion::stillStaging(mallet.location, impulseVector[1])) {
                // issue move command to hit
                motorDriver.moveTo(impulseVector[2]);
                impulseState = HITTING;
            }
            break;
        case HITTING:
            if (!Motion::stillStaging(mallet.location, impulseVector[2])) {
                // set done state and return true
                impulseState = IMPULSEDONE;
                toReturn = true;
            }
            break;
        default:
            impulseState = IMPULSEDONE;
            toReturn = true;
            break;
    }

}


std::vector<cv::Point_<int>> Impulse::findImpulseVector(Mallet& mallet, Puck& puck, cv::Mat& grabbed) {
    // determine if the puck is 'stuck' on an x-wall or the y-wall
    // if x-wall set a bool high that says its on an x-wall
    // if y-wall set a bool high that says its on a y-wall
    //
}