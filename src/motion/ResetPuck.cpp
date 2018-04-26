//
// Created by mike on 4/25/18.
//

#include "../../inc/motion/ResetPuck.h"
#include "../../inc/states.h"
#include "../../inc/GameState.h"
#include "../../inc/Table.h"
#include "../../inc/motion/Trajectory.h"
#include "../../inc/MotorDriver.h"
#include <vector>



bool ResetPuck::run(GameState& gs, int lostCnt) {
    bool toReturn = false;
    switch(resetState) {
        case RESETDONE:
            // this is the first call to reset
            // if the puck has been lost for a while set the state to FINDINGPUCK
            if (lostCnt > 1500) {
                resetState = FINDINGPUCK;
                // else if the puck is found and is in the robots motion limits
            } else if (within(gs.puck.location, Table::motionLimitMin, Table::motionLimitMax)) {
                if (gs.puckTraj.empty()) {
                    //and not moving calculate call calculate stage and hit vector and set state to STAGING1
                    hitVector = calcStage(gs);
                    resetState = STAGING1;
                } else {
                    // else if the puck is moving then set the state to NOPUCK
                    resetState = NOPUCK;
                }
            } else {
                // set the state to NOPUCK
                resetState = NOPUCK;
            }
            break;

        case FINDINGPUCK:
            // this is called when the puck is not found for some time
            // issue move to table::motionlimitmin.x, table::home.y, the findingSpot
            MotorDriver::getInstance().moveTo(findingSpot);
            // check if the mallet is close to the finding spot
            if (within(gs.mallet.location, {findingSpot.x - 5, findingSpot.y - 5}, {findingSpot.x + 5, findingSpot.y + 5})) {
                // if the puck is found
                if (gs.puck.found) {
                    // if the puck is on the robot's side of the table
                    if (gs.puck.location.x > Table::motionLimitMin.x - 20) {
                        // if the puck is not moving
                        if (gs.puckTraj.empty()) {
                            // calculate stage spots and set state to STAGING1
                            hitVector = calcStage(gs);
                        } else {
                            // set state to RESETDONE, moveTo home and toReturn=true
                            resetState = RESETDONE;
                            MotorDriver::getInstance().moveTo(Table::home);
                            toReturn = true;
                        }
                    } else {
                        // set state to RESETDONE, moveTo home and toReturn=true
                        resetState = RESETDONE;
                        MotorDriver::getInstance().moveTo(Table::home);
                    }
                } else {
                    // set state to NOPUCK, moveTo home
                    resetState = NOPUCK;
                    MotorDriver::getInstance().moveTo(Table::home);
                }
            } else {
                // keep moving to the findingSpot
            }
            break;

        case NOPUCK:
            // this is called if no puck is found
            // play a sound telling the player the puck can't be located, please put a puck on the table
            // and set the state to RESETDONE
            resetState = RESETDONE;
            break;

        case STAGING1:
            // this is called to move the mallet to the first stage position
            // issue move to first item in the stage vector
            MotorDriver::getInstance().moveTo(hitVector[0]);
            // if the puck is near the first stage position set the state to STAGING2
            if (within(gs.mallet.location, {hitVector[0].x - 5, hitVector[0].y - 5}, {hitVector[0].x + 5, hitVector[0].y + 5})) {
                resetState = STAGING2;
            } else {
                // keep moving to the first staging spot
            }
            break;

        case STAGING2:
            // this is called to move the mallet to the second stage position
            // issue move to second item in the stage vector
            MotorDriver::getInstance().moveTo(hitVector[1]);
            // if the puck is near the second stage position set the state to HITTING
            if (within(gs.mallet.location, {hitVector[1].x - 5, hitVector[1].y - 5}, {hitVector[1].x + 5, hitVector[1].y + 5})) {
                resetState = HITTING;
            } else {
                // keep moving to the second staging spot
            }
            break;

        case HITTING:
            // this is called to hit the puck from its stuck position
            // issue the move to hit position
            MotorDriver::getInstance().moveTo(hitVector[2]);
            // if the puck is near the hit position set the state to RESETDONE and return true
            if (within(gs.mallet.location, {hitVector[2].x - 5, hitVector[2].y - 5}, {hitVector[2].x + 5, hitVector[2].y + 5})) {
                toReturn = true;
                resetState = RESETDONE;
            } else {
                // keep moving to the second staging spot
            }
            break;
    }
    return toReturn;
}

std::vector<cv::Point_<int>> ResetPuck::calcStage(GameState& gs) {
    std::vector<cv::Point_<int>> toReturn;
    toReturn.emplace_back(cvPoint(Table::motionLimitMax.x, gs.mallet.location.y));
    toReturn.emplace_back(cvPoint(Table::motionLimitMax.x, gs.puck.location.y));
    toReturn.emplace_back(cvPoint(Table::motionLimitMin.x, gs.puck.location.y));
    return toReturn;
}