//
// Created by mdl150330 on 3/19/2018.
//

#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/core/opengl.hpp>
#include <opencv2/highgui.hpp>
#include <opencv/cv.hpp>


#include "../../inc/motion/Motion.h"
#include "../../inc/MotorDriver.h"
#include "../../inc/Camera.h"
#include "../../inc/Table.h"
#include "../../inc/Mallet.h"
#include "../../inc/Puck.h"
#include "../../inc/Settings.h"

#pragma clang diagnostic ignored "-Wnarrowing"

Motion::Motion() {

}



void Motion::calibrateHome(Table table, Mallet mallet, Settings settings) {
    Camera& camera = Camera::getInstance();
    bool yHome = false; //set this to false
    bool xHome = false;
    int yHomeDecay = 0;
    int xHomeDecay = 0;
    cv::Point_<int> firstPosition = {0,0};
    bool first = false;
    cv::Mat calHomeGrabbed;
    cv::Mat calHomeSmall;
    cv::namedWindow("CalHome");
    MotorDriver& motorDriver = MotorDriver::getInstance();

    long initCount;
    while (!yHome) {
        if (!settings.undistort) {
            calHomeGrabbed = camera.getFrame();
        } else {
            calHomeGrabbed = camera.getUndistortedFrame(); // Query a new frame
        }
        if (calHomeGrabbed.empty()) {
            printf("No frames!\n");
            break;
        }
        mallet.findOne(calHomeGrabbed, table, true);
        if (table.preview == 1) {
            cv::resize(calHomeGrabbed, calHomeSmall, cv::Size(), 0.5, 0.5);
            imshow("CalHome", calHomeSmall);
        }
        if (!mallet.found) {
            printf("Mallet not found in calibrateHome y run!\n");
            if (cv::waitKey(100) >= 0)
                continue;
        }
        if (!first) {
            initCount = motorDriver.getSteps('y');
            // as the drivers where it is and store it as initial count
            firstPosition = mallet.location;
            first = true;
        }

        int stepsPerPixel = 1;
        if (abs(Table::home.y - mallet.location.y) <= 5) {
            if(yHomeDecay >= 5) {
                //printf("Close enough\n");
                yHome = true;
                long finalCount = motorDriver.getSteps('y');
                long countDif = finalCount - initCount;
                //ratio = abs((double)countDif / (double)abs(Table::home.y - firstPosition.y));
//            printf("ratio: %f\n", ratio);
                // ask the drivers where it is and store as final count
//            break;
            }
            else {
                yHomeDecay++;
            }

        } else {
            yHomeDecay = 0;
            int difference = Table::home.y - mallet.location.y;
            long toMove = (long) (abs(stepsPerPixel) * difference);
            if(abs(toMove) <= 3) {
                //printf("not moving %ld steps\n", toMove);
            }
            else {
//                printf("%ld steps\n", toMove);
                motorDriver.sendCMD(toMove, 'y');
            }
        }
        if (cv::waitKey(100) >= 0)
            break;
    }
    while (!xHome) {
        if (!settings.undistort) {
            calHomeGrabbed = camera.getFrame();
        } else {
            calHomeGrabbed = camera.getUndistortedFrame(); // Query a new frame
        }
        if (calHomeGrabbed.empty()) {
            printf("No frames!\n");
            break;
        }
        mallet.findOne(calHomeGrabbed, table, true);
        if (table.preview == 1) {
            cv::resize(calHomeGrabbed, calHomeSmall, cv::Size(), 0.5, 0.5);
            imshow("CalHome", calHomeSmall);
        }
        if (!mallet.found) {
            printf("Mallet not found in calibrateHome x run!\n");
            if (cv::waitKey(100) >= 0)
                continue;
        }
        if (!first) {
            initCount = motorDriver.getSteps('x');
            // as the drivers where it is and store it as initial count
            firstPosition = mallet.location;
            first = true;
        }

        int stepsPerPixel = 4;
        if (abs(Table::home.x - mallet.location.x) <= 5) {
            //printf("Close enough\n");
            if(xHomeDecay >=5) {
                xHome = true;
                long finalCount = motorDriver.getSteps('x');
                long countDif = finalCount - initCount;
            }
            else {
                xHomeDecay++;
            }
            //ratio = abs((double)countDif / (double)abs(Table::home.x - firstPosition.x));
//            printf("ratio: %f\n", ratio);
            // ask the drivers where it is and store as final count
//            break;
        }
        else {
            xHomeDecay = 0;
            int difference = mallet.location.x - Table::home.x;
            long toMove = (long) (abs(stepsPerPixel) * difference);
            if(abs(toMove) <= 3) {
                printf("not moving %ld steps\n", toMove);
            }
            else {
//                printf("%ld steps\n", toMove);
                motorDriver.sendCMD(toMove, 'x');
            }
        }
        if (cv::waitKey(100) >= 0)
            break;
    }
    // set the current position to the home position
    motorDriver.setHome();
    cvDestroyWindow("CalHome");
}

bool rollingCheck(bool in) {
    static int index = 0;
    static bool x[3];
    x[(++index %3 )] = in;
    return x[0] || x[1] || x[2];
}

// this is broken af cause trajectory doesnt fill if the puck isnt moving...
void Motion::trackPredictedY(Table table, Mallet mallet, Puck puck, cv::Mat grabbed) {
    MotorDriver& motorDriver = MotorDriver::getInstance();
    if(puck.lostCnt < 10 && mallet.found) {
        if (abs(puck.location.y - mallet.location.y) <= 30) {
            //printf("Close enough\n");
        } else if (puck.vectorXY.x != 0 || puck.vectorXY.y != 0){
            long toMove;
            //int difference = puck.predictedLocation.y - 720/2;//mallet.location.y;
            //todo why is ratio 4
            if(puck.trajectory.back()[1].y > table.max.y-100) {
                toMove = table.max.y-100;
            }
            else if(puck.trajectory.back()[1].y < table.min.y+100) {
                toMove = table.min.y+100;
            }
            else {
                toMove = puck.trajectory.back()[1].y;
            }
//            toMove = ((toMove-720/2) * 4); //get to the middle
            if(abs(toMove) <= 25) {
                //printf("not moving %ld steps\n", toMove);
            }
            else if (puck.onTable && puck.vectorXY.x > 0){
//                printf("%ld steps\n", toMove);
//                motorDriver.sendCMD(toMove, 'Y');
            }
            cv::circle(grabbed, cvPoint(mallet.location.x, (toMove)/4 + 360),
                       20, cv::Scalar(255, 0, 0), 4);

        }
    }
}

bool checkPastgoalFlags(std::vector<bool> goalFlagHistory) {
    for (int i = 0; i < goalFlagHistory.size(); i++) {
        if (goalFlagHistory[i])
            return true;
    }
}

// TODO: come up with some way to ALWAYS prevent the robot from knocking the puck into its own goal
cv::Point_<int> Motion::defend(const Table& table, Mallet& mallet, Puck& puck, cv::Mat& grabbed) {
    const auto defenseColor = cv::Scalar(225, 255, 255);
    static cv::Point_<int> interceptSpot = Table::home;
    MotorDriver& motorDriver = MotorDriver::getInstance();
    cv::Point_<int> desiredLocation;
    if(puck.lostCnt < 10 && mallet.found) {
        // calc desired mallet location
        // i dont know if this intersect check here is needed, just did it cause
        if (puck.rightGoal) {
            // move to the midpoint of the last trajectory vector
            cv::putText(grabbed, "Goal!!", cvPoint(450, 320), cv::FONT_HERSHEY_SIMPLEX, 10, defenseColor, 7);
            desiredLocation = puck.trajectory.back()[0] + (puck.trajectory.back()[1] - puck.trajectory.back()[0]) * 0.7;
            interceptSpot = desiredLocation;
        }
        else if(checkPastgoalFlags(puck.rightGoalHistory)) {
            cv::putText(grabbed, "Goal!!", cvPoint(350, 320), cv::FONT_HERSHEY_SIMPLEX, 10, defenseColor, 7);
            desiredLocation =  interceptSpot;
        }
        else {
            desiredLocation = Table::home;
            interceptSpot = Table::home;
        }

        // limit desired location to x and y boundaries if they exceed those boundaries
        desiredLocation = saturate(desiredLocation, table.motionLimitMin, table.motionLimitMax);
//        if (puck.onTable && ((abs(desiredLocation.x - mallet.location.x) > motorDriver.deadband) || (abs(desiredLocation.y - mallet.location.y) > motorDriver.deadband)))
        if (puck.onTable) {
            cv::putText(grabbed, "moving", cvPoint(1100, 665), cv::FONT_HERSHEY_SIMPLEX, 1, defenseColor, 2);
            if(mallet.location == desiredLocation) {
                //printf("Motion::defend: mallet is where I want it, not moving\n");
            }
            else {
                motorDriver.moveTo(desiredLocation);
            }

        }

//        printf("Desired Location: (%d, %d)\n", desiredLocation.x, desiredLocation.y);

        cv::circle(grabbed, desiredLocation, 20, defenseColor, 6);
    }
    return desiredLocation;
}

void Motion::resetOffense() {
    attackState = ATTACKDONE;
}

// TODO: test this
bool Motion::attack(const Table& table, Mallet& mallet, Puck& puck, cv::Mat& grabbed) {
    MotorDriver& motorDriver = MotorDriver::getInstance();
    bool toReturn = false;
    const auto color = cv::Scalar(100, 255, 255);
    const auto color1 = cv::Scalar(150, 255, 255);
    cv::putText(grabbed, "attacking", cvPoint(1100, 665), cv::FONT_HERSHEY_SIMPLEX, 1, color, 2);
    switch(attackState) {
        case ATTACKDONE: {
            // this is the fist call to attack
            // calculate staging point and final point with findHitVector
            hitVector = findHitVector(table, mallet, puck, grabbed);
            // issue move command to staging point
            auto desiredLocation = hitVector.front();
            motorDriver.moveTo(desiredLocation);
            cv::circle(grabbed, desiredLocation, 20, color, 8);
            cv::circle(grabbed, hitVector.front(), 20, color, 6);
            cv::circle(grabbed, hitVector.back(), 20, color1, 6);
            // set attackState to staging
            attackState = STAGING;
            break;
        }
        case STAGING: {
            // if the mallet is at the staging point
            if (!stillStaging(mallet.location, hitVector.front())) {
                // issue move command to attack point
                auto desiredLocation = hitVector.back();
                motorDriver.moveTo(desiredLocation);
                cv::circle(grabbed, desiredLocation, 20, color, 8);
                cv::circle(grabbed, hitVector.front(), 20, color, 6);
                cv::circle(grabbed, hitVector.back(), 20, color1, 6);
                // set attackState to Attacking
                attackState = ATTACKING;
            }
            else {
                auto desiredLocation = hitVector.front();
                motorDriver.moveTo(desiredLocation);
                cv::circle(grabbed, hitVector.front(), 20, color, 6);
                cv::circle(grabbed, hitVector.back(), 20, color1, 6);
            }
            break;
        }
        case ATTACKING:
            // if the mallet is at the final attack point
            if (abs(mallet.location.x - hitVector.back().x) < 25 && abs(mallet.location.y - hitVector.back().y) < 25) {
                attackState = ATTACKDONE;
                toReturn = true;
            }
            else {
                auto desiredLocation = hitVector.back();
                motorDriver.moveTo(desiredLocation);
                cv::circle(grabbed, hitVector.front(), 20, color, 6);
                cv::circle(grabbed, hitVector.back(), 20, color1, 6);
            }

            break;
        default:
            // set attackState to done
            attackState = ATTACKDONE;
            toReturn = true;
            break;
    }

    /*if (within(puck.location, table.strikeLimitMin, table.strikeLimitMax)) {
        if (!hitVectorFound) {
            hitVector = findHitVector(table, mallet, puck, grabbed);
            // tell the motors to move to the staging position
            motorDriver.moveTo(hitVector.front());
        } else if (striking) {
            // check if done striking
            if (abs(mallet.location.x - hitVector.back().x) < 25 && abs(mallet.location.y - hitVector.back().y) < 25) {
                attackDone = true;
                hitVectorFound = false;
                striking = false;
                // tell motors to return to home position
                motorDriver.moveTo(table.home);
                // set motionMode to defense somewhere else if motion.attackDone is true;
            }
        } else if (!stillStaging(mallet.location, hitVector.front())){
            striking = true;
            // tell the motors to move to to the last point in hitVector
            motorDriver.moveTo(hitVector.back());

        }*/

/*     find a direct hit path from the current mallet location to a point some amount beyond the
     puck predicted location and some amount from the puck back to the mallet's current location the same amount
     use that location as the input lastLoc to calcAttackTraj and the location beyond as the location
     does this end in a leftGoal?
         yes, issue move command the the point beyond the puck, make sure to saturate it and return
         no, first calculate a y distance on the player's wall from the center of the goal to the end point found
         redo the above if the mallet's location is 40 px different in +/- y
         do these end in a goal? if one of them does take that shot and return
             if not which one resulted in less distance from center of player's goal to end point
             move 40px in that direction and redo again until either a goal or a max of 3 moves is done
             if reach 3 moves then just take that shot*/

    return toReturn;
}

bool Motion::stillStaging (cv::Point_<int> malletloc, cv::Point_<int> staging) {
    auto diff = malletloc-staging;
    return abs(diff.x) > 10 || abs(diff.y) > 10;
}

std::vector<cv::Point_<int>> Motion::findHitVector(const Table& table, Mallet& mallet, Puck& puck, cv::Mat& grabbed) {
    bool useNewTraj = false;
    double sf = 80;
    int yoff = 30;
    int offdir = 0;
    int i = 2;
    cv::Point_<double> mallet2puck = puck.location - mallet.location;
    double mag = sqrt(pow(mallet2puck.x, 2) + pow(mallet2puck.y, 2));
    cv::Point_<double> amountdbl = (mallet2puck/mag)*sf;
    cv::Point_<int> amountint = {round(amountdbl.x), round(amountdbl.y)};
    cv::Point_<int> puckplus = puck.location + amountint;
    cv::Point_<int> puckminus = puck.location - amountint;
    puckplus = saturate(puckplus, Table::strikeLimitMin, Table::strikeLimitMax);
    cv::line(grabbed, puckplus, puckminus, cvScalar(55, 200, 200), 3);
    cv::circle(grabbed, puckplus, 10, cv::Scalar(55, 200, 200), 3);
    cv::circle(grabbed, puckminus, 10, cv::Scalar(5, 200, 200), 3);
    auto estTraj = puck.calcTrajOffense(table, grabbed, puckminus, puckplus);
    auto newTraj = puck.calcTrajOffense(table, grabbed, puckminus, puckplus);
    if (!puck.leftGoalOffense) {
        useNewTraj = true;
        // determine which y-direction gets me closer to the goal
        int dist2goal = abs(estTraj.back().back().y - puck.L_mid.y);
        // calculate negative new puckplus and puckminus positions
        mallet2puck = puck.location - cv::Point(mallet.location.x, mallet.location.y-yoff);
        mag = sqrt(pow(mallet2puck.x, 2) + pow(mallet2puck.y, 2));
        amountdbl = (mallet2puck/mag)*sf;
        amountint = {round(amountdbl.x), round(amountdbl.y)};
        puckplus = puck.location + amountint;
        puckminus = puck.location - amountint;
        puckplus = saturate(puckplus, Table::strikeLimitMin, Table::strikeLimitMax);
        // if this new trajectory does not result in a goal or it takes us further away from the goal
        // immediately set the offset direction variable to 1 and start checking that direction else
        // if it results in a goal then take that shot but if not continue incrementing until a goal is found
        if(!puck.leftGoalOffense || abs(newTraj.back().back().y - puck.L_mid.y) > dist2goal) {
            offdir = 1;
            for (i = 2; i < 10; i++) {
                mallet2puck = puck.location - cv::Point(mallet.location.x, mallet.location.y + yoff*i);
                mag = sqrt(pow(mallet2puck.x, 2) + pow(mallet2puck.y, 2));
                amountdbl = (mallet2puck/mag)*sf;
                amountint = {static_cast<int>(round(amountdbl.x)), static_cast<int>(round(amountdbl.y))};
                puckplus = puck.location + amountint;
                puckminus = puck.location - amountint;
                puckplus = saturate(puckplus, Table::strikeLimitMin, Table::strikeLimitMax);
                if (puck.leftGoalOffense) {
                    break;
                }
            }
        } else if (puck.leftGoalOffense) {
            // first move the mallet to the offset position and then tell it to move to strike spot
        } else {
            offdir = -1;
            for (i = 2; i < 10; i++) {
                mallet2puck = puck.location - cv::Point(mallet.location.x, mallet.location.y - yoff*i);
                mag = sqrt(pow(mallet2puck.x, 2) + pow(mallet2puck.y, 2));
                amountdbl = (mallet2puck/mag)*sf;
                amountint = {round(amountdbl.x), round(amountdbl.y)};
                puckplus = puck.location + amountint;
                puckminus = puck.location - amountint;
                puckplus = saturate(puckplus, Table::strikeLimitMin, Table::strikeLimitMax);
                if (puck.leftGoalOffense) {
                    break;
                }
            }
        }
        newTraj = puck.calcTrajOffense(table, grabbed, puckminus, puckplus);
    }
    cv::line(grabbed, puckplus, puckminus, cvScalar(55, 200, 200), 3);
    cv::circle(grabbed, puckplus, 10, cv::Scalar(55, 200, 200), 3);
    cv::circle(grabbed, puckminus, 10, cv::Scalar(5, 200, 200), 3);
    if (useNewTraj) {
        puck.drawTraj(grabbed, newTraj);
    } else {
        puck.drawTraj(grabbed, estTraj);
    }
    // return the staging position and the moveTo position
    cv::Point_<int> stage = {mallet.location.x, mallet.location.y + yoff * i * offdir};
    stage = saturate(stage, Table::strikeLimitMin, Table::strikeLimitMax);
    auto moveTo = saturate(puckplus, Table::strikeLimitMin, Table::strikeLimitMax);
    hitVectorFound = true;
    return {stage, moveTo};

}

void Motion::calcPuckRange(cv::Point_<double> mallet2puck, cv::Point_<int>& puckPlus, cv::Point_<int>& puckMinus) {

}


bool Motion::impulse(Table& table, Mallet& mallet, Puck& puck, cv::Mat &grabbed) {
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
            if (!stillStaging(mallet.location, impulseVector[0])) {
                // issue move command to stage2
                motorDriver.moveTo(impulseVector[1]);
                // set attackState to Attacking
                impulseState = STAGING2;
            }
            break;
        case STAGING2:
            if (!stillStaging(mallet.location, impulseVector[1])) {
                // issue move command to hit
                motorDriver.moveTo(impulseVector[2]);
                impulseState = HITTING;
            }
            break;
        case HITTING:
            if (!stillStaging(mallet.location, impulseVector[2])) {
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

std::vector<cv::Point_<int>> Motion::findImpulseVector(const Table& table, Mallet& mallet, Puck& puck, cv::Mat& grabbed) {
    // determine if the puck is 'stuck' on an x-wall or the y-wall
        // if x-wall set a bool high that says its on an x-wall
        // if y-wall set a bool high that says its on a y-wall
    //
}
