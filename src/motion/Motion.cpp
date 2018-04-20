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
#include "../../inc/Settings.h"
#include "../../inc/motion/Trajectory.h"

const bool Motion::notAt(const cv::Point_<int>& at, const cv::Point_<int>& desired, const int& tolerance) {
    return !isAt(at, desired, tolerance);
}

const bool Motion::isAt(const cv::Point_<int>& at, const cv::Point_<int>& desired, const int& tolerance) {
    return abs(at.x - desired.x) < tolerance && abs(at.y - desired.y) < tolerance;
}

const bool Motion::stillStaging (const cv::Point_<int>& malletloc, const cv::Point_<int>& staging) {
    auto diff = malletloc-staging;
    return abs(diff.x) > 10 || abs(diff.y) > 10;
}

hitVector_t Motion::findHitVector(GameState& gs, int ff) { ///note: this is always called AFTER GameState is "finished"
    
    hitVector_t toReturn;
    bool useNewTraj = false;
    double sf = 80;
    int yoff = 10;
    int offdir = 0;
    int i = 2;
    cv::Point_<int> ffPuckLoc = (Trajectory::predictLocation(gs.puck,ff));

    cv::Point_<double> mallet2puck = ffPuckLoc - gs.mallet.location;
    double mag = sqrt(pow(mallet2puck.x, 2) + pow(mallet2puck.y, 2));
    cv::Point_<double> amountdbl = (mallet2puck/mag)*sf;
    cv::Point_<int> amountint = {static_cast<int>(round(amountdbl.x)), static_cast<int>(round(amountdbl.y))};
    toReturn.puckminus = ffPuckLoc - amountint;
    toReturn.puckplus = saturate(ffPuckLoc + amountint, Table::strikeLimitMin, Table::strikeLimitMax);

    auto estTraj = Trajectory::calculate(gs);
    auto newTraj = estTraj;



    if ((gs.goalFlag != Table::Goals::LEFT_GOAL)) {
        useNewTraj = true;
        // determine which y-direction gets me closer to the goal
        int dist2goal = abs(estTraj.back().back().y - Table::goals.L_mid.y);
        // calculate negative new puckplus and puckminus positions
        mallet2puck = ffPuckLoc - cv::Point(gs.mallet.location.x, gs.mallet.location.y-yoff);
        mag = sqrt(pow(mallet2puck.x, 2) + pow(mallet2puck.y, 2));
        amountdbl = (mallet2puck/mag)*sf;
        amountint = {static_cast<int>(round(amountdbl.x)), static_cast<int>(round(amountdbl.y))};
        toReturn.puckminus = ffPuckLoc - amountint;
        toReturn.puckplus = saturate(ffPuckLoc + amountint, Table::strikeLimitMin, Table::strikeLimitMax);
        ///newTraj = puck.calcTrajOffense(gs.frame, toReturn.puckminus, toReturn.puckplus);
        // if this new trajectory does not result in a goal or it takes us further away from the goal
        // immediately set the offset direction variable to 1 and start checking that direction else
        // if it results in a goal then take that shot but if not continue incrementing until a goal is found
        if((gs.goalFlag != Table::Goals::LEFT_GOAL) && abs(newTraj.back().back().y - Table::goals.L_mid.y) > dist2goal) {
            offdir = 1;
            for (i = 2; i < 10; i++) {
                mallet2puck = ffPuckLoc - cv::Point(gs.mallet.location.x, gs.mallet.location.y + yoff*i);
                mag = sqrt(pow(mallet2puck.x, 2) + pow(mallet2puck.y, 2));
                amountdbl = (mallet2puck/mag)*sf;
                amountint = {static_cast<int>(round(amountdbl.x)), static_cast<int>(round(amountdbl.y))};
                toReturn.puckminus = ffPuckLoc - amountint;
                toReturn.puckplus = saturate(ffPuckLoc + amountint, Table::strikeLimitMin, Table::strikeLimitMax);
                if ((gs.goalFlag == Table::Goals::LEFT_GOAL)) {
                    break;
                }
            }
        } else if ((gs.goalFlag == Table::Goals::LEFT_GOAL)) {
            // first move the mallet to the offset position and then tell it to move to strike spot
        } else {
            offdir = -1;
            for (i = 2; i < 10; i++) {
                mallet2puck = ffPuckLoc - cv::Point(gs.mallet.location.x, gs.mallet.location.y - yoff*i);
                mag = sqrt(pow(mallet2puck.x, 2) + pow(mallet2puck.y, 2));
                amountdbl = (mallet2puck/mag)*sf;
                amountint = {static_cast<int>(round(amountdbl.x)), static_cast<int>(round(amountdbl.y))};
                toReturn.puckminus = ffPuckLoc - amountint;
                toReturn.puckplus = saturate(ffPuckLoc + amountint, Table::strikeLimitMin, Table::strikeLimitMax);
                if ((gs.goalFlag == Table::Goals::LEFT_GOAL)) {
                    break;
                }
            }
        }
        ///newTraj = puck.calcTrajOffense(gs.frame, toReturn.puckminus, toReturn.puckplus);
    }
    cv::line(gs.frame, toReturn.puckplus, toReturn.puckminus, cvScalar(55, 200, 200), 3);
    cv::circle(gs.frame, toReturn.puckplus, 10, cv::Scalar(55, 200, 200), 3);
    cv::circle(gs.frame, toReturn.puckminus, 10, cv::Scalar(5, 200, 200), 3);
    if (useNewTraj) {
        //todo puck.drawTraj(gs.frame, newTraj);
    } else {
        //todo puck.drawTraj(gs.frame, estTraj);
    }
    // return the staging position and the moveTo position
    cv::Point_<int> stage = {gs.mallet.location.x, gs.mallet.location.y + yoff * i * offdir};
    stage = saturate(stage, Table::strikeLimitMin, Table::strikeLimitMax);
    cv::Point_<int> moveTo = saturate(toReturn.puckplus, Table::strikeLimitMin, Table::strikeLimitMax);
    toReturn.data = {stage, moveTo};
    return toReturn;
}


void Motion::calcPuckRange(cv::Point_<double> mallet2puck, cv::Point_<int>& puckPlus, cv::Point_<int>& puckMinus) {

}





