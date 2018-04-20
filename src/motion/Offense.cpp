//
// Created by jad140230 on 3/24/2018.
//

#include "../../inc/motion/Offense.h"
#include "../../inc/MotorDriver.h"
#include "../../inc/motion/Motion.h"

bool Offense::run(Mallet& mallet, Puck& puck, cv::Mat& grabbed) {
    const cv::Scalar stageColor = cv::Scalar(100, 255, 255);
    const cv::Scalar attackColor = cv::Scalar(150, 255, 255);
    const cv::Scalar predictedLocationColor = cv::Scalar(180, 255, 255);
    MotorDriver& motorDriver = MotorDriver::getInstance();

    bool toReturn = false;


    switch(state) {
        case OFFENSEDONE:
            hitVector = Motion::findHitVector(mallet, puck, grabbed, 10);
            motorDriver.moveTo(hitVector.data.front());
            state = GOINGTOSTAGE;
            break;
        case GOINGTOSTAGE:
            if(Motion::notAt(mallet.location, hitVector.data.front(), 10)) {
                motorDriver.moveTo(hitVector.data.front());
            } else {
                state = STRIKING;
            }
            break;
        case STRIKING:
            if (Motion::notAt(mallet.location, hitVector.data.back(), 10)) {
                motorDriver.moveTo(hitVector.data.back());
            } else {
                state = OFFENSEDONE;
                motorDriver.moveTo(Table::home);
                toReturn = true;
            }
            break;
    }

    //cv::circle(grabbed, puck.predictLocation(table, 10), 15, predictedLocationColor, 6);
    cv::line(grabbed, hitVector.puckplus, hitVector.puckminus, cv::Scalar(55, 200, 200), 3);
    cv::circle(grabbed, hitVector.puckplus, 10, attackColor, 3);
    cv::circle(grabbed, hitVector.puckminus, 10, stageColor, 3);

    return toReturn;

}

bool Offense::run(GameState& gs) {
    const cv::Scalar stageColor = cv::Scalar(100, 255, 255);
    const cv::Scalar attackColor = cv::Scalar(150, 255, 255);
    const cv::Scalar predictedLocationColor = cv::Scalar(180, 255, 255);
    MotorDriver& motorDriver = MotorDriver::getInstance();

    bool toReturn = false;


    switch(state) {
        case OFFENSEDONE:
            hitVector = Motion::findHitVector(mallet, puck, gs.frame, 10);
            motorDriver.moveTo(hitVector.data.front());
            state = GOINGTOSTAGE;
            break;
        case GOINGTOSTAGE:
            if(Motion::notAt(gs.mallet.location, hitVector.data.front(), 10)) {
                motorDriver.moveTo(hitVector.data.front());
            }
            else {
                state = STRIKING;
            }
            break;
        case STRIKING:
            if (Motion::notAt(gs.mallet.location, hitVector.data.back(), 10)) {
                motorDriver.moveTo(hitVector.data.back());
            }
            else {
                state = OFFENSEDONE;
                motorDriver.moveTo(Table::home);
                toReturn = true;
            }
            break;
    }

    //cv::circle(grabbed, puck.predictLocation(table, 10), 15, predictedLocationColor, 6);
    cv::line(gs.frame, hitVector.puckplus, hitVector.puckminus, cv::Scalar(55, 200, 200), 3);
    cv::circle(gs.frame, hitVector.puckplus, 10, attackColor, 3);
    cv::circle(gs.frame, hitVector.puckminus, 10, stageColor, 3);

    return toReturn;

}

offenseState_t Offense::getState() const {
    return state;
}

void Offense::setDone() {
    Offense::state = OFFENSEDONE;
}

