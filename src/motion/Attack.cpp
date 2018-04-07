//
// Created by jad140230 on 3/24/2018.
//

#include "../../inc/motion/Attack.h"

bool Attack::checkLimits(const Puck& puck, const Mallet& mallet) {
    return ( //todo account for mallet size
            table.motionLimit.contains(puck.predictedLocation) &&
            table.motionLimit.contains(puck.location) &&
            table.motionLimit.contains(puck.predictedLocation + shotDistance) &&
            table.motionLimit.contains(puck.location + shotDistance)
    );
}

void Attack::run(const Puck& puck, const Mallet& mallet, cv::Mat &grabbed) {
    ///Don't call this unless you're /ready/
    MotorDriver& motorDriver = MotorDriver::getInstance();
    if ( !checkLimits(puck, mallet) ) {
        ready = false;
        return;
    }

    cv::Point_<int> shotSpot = puck.location+shotDistance;
    cv::Point_<int> db = {20,10};

    cv::Point_<int> loc;
    if(mallet.location.inside(cv::Rect_<int>(shotSpot+db, shotSpot-db))) {
        //take the shot
        loc = saturate(puck.location-overshotDistance,table.motionLimitMin,table.motionLimitMax);
        ready = false;
    }
    else {
        loc = shotSpot;
        ready = true;
    }
    cv::circle(grabbed, loc, 20, cv::Scalar(225, 255, 0), 6);
    motorDriver.moveTo(loc);
}
