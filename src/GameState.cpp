//
// Created by mike on 4/19/18.
//

#include <opencv/cv.hpp>
#include <thread>
#include "GameState.h"
#include "../inc/Settings.h"

std::vector<GameState> GameStateManager::history;
const unsigned long GameStateManager::maxLen = 30;
const unsigned int GameStateManager::maxLostCnt = 10;
int GameStateManager::lostCnt = 0;

///Takes in a frame, annotates it, and returns a class with data about
///what was in the frame.
GameState GameStateFactory::build(cv::Mat &in) {
    GameState toReturn;
    // if threadIt is set high we thread the contour searching process
    if(Settings::threadFindingThings) {
        std::thread puckThread(GameStateFactory::findPiece, toReturn.puck, in, Settings::puckLimits);
        std::thread malletThread(GameStateFactory::findPiece, toReturn.mallet, in, Settings::malletLimits);
        puckThread.join();
        malletThread.join();
    }
    else {
        toReturn.puck   = findPiece(in, Settings::puckLimits);
        toReturn.mallet = findPiece(in, Settings::malletLimits);
    }
    toReturn.frame = in;
    return toReturn;
}

const cv::Mat GameStateFactory::getThresholdImage(const cv::Mat& in, const struct threshold_s& limits) {
    cv::Mat blurred,result;
    cv::GaussianBlur(in, blurred, cv::Size(3, 3), 0, 0); //smooth the original image using Gaussian kernel

    cv::inRange(blurred,
                cv::Scalar(limits.minH, limits.minS, limits.minV),
                cv::Scalar(limits.maxH, limits.maxS, limits.maxV),
                result);

    return result;
}

void GameStateFactory::findPiece(GamePiece& out, cv::Mat& in, const struct threshold_s& limits) {
    ///this exists to make threading easier
    out = findPiece(in, limits);
}

GamePiece GameStateFactory::findPiece(cv::Mat& in, const struct threshold_s& limits) {
    GamePiece toReturn = GamePiece();
    cv::Mat imgThresh = getThresholdImage(in, limits);
    std::vector< std::vector<cv::Point> > contours;
    cv::Point_<int> center;
    cv::findContours(imgThresh, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    for (int i = 0; i < contours.size(); i++) {
        auto area = cv::contourArea(contours[i]);
        if ((area > limits.minArea) && (area < limits.maxArea)) {  // Min and Max size of object
            //Detecting roundness   roundness = perimeter^2 / (2*pi*area)
            auto perimeter = cv::arcLength(contours[i], true);
            auto roundness = (perimeter * perimeter) / (6.28 * area);
            if (roundness < limits.minRoundness) {
                cv::Moments moments = cv::moments(contours[i], true);
                double moment10 = moments.m10;
                double moment01 = moments.m01;
                area = moments.m00;
                // Calculate object center
                toReturn.location = {
                        static_cast<int>(moment10 * 2 / area)/2,
                        static_cast<int>(moment01 * 2 / area)/2
                };

                // limit the region of interest to the table
                if (!within(center, Table::min, Table::max)) {
                    toReturn.location = Table::home;
                    continue;  // continue with other contour... (this is outside the table)
                }
                else {
                    if (Settings::preview == 1) { // Draw contour
                        cv::drawContours(in, contours, i, limits.outlineColor, 5, 4);
                    }
                    toReturn.found = true;
                    toReturn.onTable = within(toReturn.location, Table::min, Table::max);
                    break; //we've found the piece, stop looking
                }
            }
        }
    }
    return toReturn;
}

static void GameStateManager::setStateInfo(GameState& gs) {
    auto lastGs;
    if(history.size() > 2) { //if there even was a previous state
        lastGs = history[1];
    }
    else {
        lastGs = GameState();
    }
    if(lastGs.found) {
        lostCnt= 0;
        gs.mallet.lastLocation = lastGs.mallet.location;
        gs.puck.lastLocation = lastGs.mallet.location;
    }
    else {
        if(++lostCnt > maxLostCnt) {
            //we've really lost the puck
            //todo?
        }
        else {
            //predict the puck's next location
            gs.mallet.lastLocation = lastGs.mallet.location;
            gs.puck.lastLocation = lastGs.puck.location;
            gs.mallet.location = lastGs.mallet.lastLocation + lastGs.mallet.location;
            gs.puck.location = lastGs.puck.lastLocation + lastGs.puck.location;
        }
    }


}

int GameStateManager::getLostCnt() {
    return lostCnt;
}

