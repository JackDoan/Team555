//
// Created by jad140230 on 3/24/2018.
//

#include "../../inc/motion/Offense.h"
#include "../../inc/MotorDriver.h"
#include "../../inc/motion/Motion.h"
#include "../../inc/motion/Trajectory.h"

cv::Point_<int> Offense::run(GameState& gs, double frameRate) {
//    const cv::Scalar stageColor = cv::Scalar(100, 255, 255);
//    const cv::Scalar attackColor = cv::Scalar(150, 255, 255);
//    const cv::Scalar predictedLocationColor = cv::Scalar(180, 255, 255);

    static cv::Point_<int> desiredLocation = Table::home;
/*
    // how far is the puck in x and what is it's x velocity?
    // make a 'prediction' about how much time until it will be at the same xlevel as the mallet
    // if this time is less than 50ms set state to striking or if y velocity is really low and
    // is on the robots side of the table strike
    // else set state to tracking-y

    double xvelo = (int) round((gs.puck.location.x - gs.puck.lastLocation.x) / frameRate);
    int xdist2mallet = gs.mallet.location.x - gs.puck.location.x;
    double time2mallet = xdist2mallet/xvelo;
    auto frames2mallet = static_cast<int>(frameRate * time2mallet);

    if(within(gs.puck.location, Table::motionLimitMin, Table::motionLimitMax)) {
        state = STRIKING;
    }
    else {
        if (xvelo <= 0) {
            state = TRACKING;
        }
        else {
            if (frames2mallet < 5) {
                state = STRIKING;
            } else {
                state = TRACKING;
            }
        }
    }


    std::cout << "frames2Mallet: " << frames2mallet <<" xvelo: " << xvelo
              << " timeToMallet: " << time2mallet << " xdist2Mallet: " << xdist2mallet
              << std::endl;


    switch (state) {
        case STRIKING:
            // calculate a desired location which is the minimum x motion limit and the predicted y location of the puck
            desiredLocation = {Table::motionLimitMin.x, Trajectory::predictLocation(gs.puck, frames2mallet+3).y};
            break;
        case TRACKING:
            // calculate a desired location which is the home x position and the predicted y location of the puck
            if (xvelo <= 0) {
                desiredLocation = {Table::home.x, Trajectory::predictLocation(gs.puck, 3).y};
            } else {
                desiredLocation = {Table::home.x, Trajectory::predictLocation(gs.puck, frames2mallet+3).y};
            }

            break;
    }*/
    if (gs.puckTraj.empty()) {
        if (within(gs.puck.location, Table::motionLimitMin, Table::motionLimitMax)) {
            desiredLocation = {Table::motionLimitMin.x, gs.puck.location.y};
        } else {
            // do the last command
        }
    } else if (within(Trajectory::predictLocation(gs.puck, 2), Table::motionLimitMin, Table::motionLimitMax)){
        desiredLocation = {Table::motionLimitMin.x, gs.puckTraj.back()[0].y};
    } else {
        desiredLocation = {Table::home.x, gs.puckTraj.back()[0].y};
    }




    //desiredLocation = {commands.x.filter(desiredLocation.x), commands.y.filter(desiredLocation.y)};
    MotorDriver::getInstance().moveTo(desiredLocation);

    return desiredLocation;

}

offenseState_t Offense::getState() const {
    return state;
}


