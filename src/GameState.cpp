//
// Created by mike on 4/19/18.
//

#include <opencv/cv.hpp>
#include <thread>
#include "../inc/GameState.h"
#include "../inc/Settings.h"
#include "../inc/motion/Trajectory.h"

std::vector<GameState> GameStateManager::history;
const unsigned long GameStateManager::maxLen = 30;
const unsigned int GameStateManager::maxLostCnt = 10;
int GameStateManager::lostCnt = 0;

///Takes in a frame, annotates it, and returns a class with data about
///what was in the frame.
GameState GameStateFactory::build() {

    GameState toReturn;
    toReturn.frame = Camera::getInstance().getFrame();

    int cols = 400;
    int rows = Table::max.y - Table::min.y;
    auto malletROI = cv::Rect(Table::max.x-cols,Table::min.y,cols,rows);
    cv::Mat malletArea = toReturn.frame(malletROI);
    //cv::imshow("Test", malletArea);

    if(Settings::threadFindingThings) {
        std::thread puckThread(&GameStateFactory::findPieceThread, std::ref(toReturn.puck), std::ref(toReturn.frame), std::ref(Settings::puckLimits));
        std::thread malletThread(&GameStateFactory::findPieceThread, std::ref(toReturn.mallet), std::ref(malletArea), std::ref(Settings::malletLimits));
        puckThread.join();
        malletThread.join();
        //toReturn.mallet.location = toReturn.mallet.location+cv::Point_<int>(1280-cols, 720-rows);
        //cv::circle(toReturn.frame,toReturn.mallet.location, 20, Settings::malletLimits.outlineColor, 6);
    }
//    else {
//        toReturn.puck   = findPiece(toReturn.frame, Settings::puckLimits);
//        toReturn.mallet = findPiece(toReturn.frame, Settings::malletLimits);
//    }

    return toReturn;
}

GameState GameStateFactory::build(cv::Mat &in) {
    GameState toReturn;
    // if threadIt is set high we thread the contour searching process
    int cols = 400;
    int rows = Table::max.y - Table::min.y;
    auto malletROI = cv::Rect(Table::max.x-cols,Table::min.y,cols,rows);
    cv::Mat malletArea = in(malletROI);

    if(Settings::threadFindingThings) {
        std::thread puckThread(&GameStateFactory::findPieceThread, std::ref(toReturn.puck), std::ref(in), std::ref(Settings::puckLimits));
        std::thread malletThread(&GameStateFactory::findPieceThread, std::ref(toReturn.mallet), std::ref(malletArea), std::ref(Settings::malletLimits));
        puckThread.join();
        malletThread.join();
        cv::circle(toReturn.frame,toReturn.mallet.location, 5, Settings::malletLimits.outlineColor);
    }
    else {
        toReturn.puck   = findPiece(in, Settings::puckLimits);
        toReturn.mallet = findPiece(in, Settings::malletLimits);
    }
    toReturn.mallet.location = toReturn.mallet.location+cv::Point_<int>(1280-cols, 720-rows);
    cv::circle(toReturn.frame,toReturn.mallet.location, 20, Settings::malletLimits.outlineColor, 6);
    toReturn.frame = in;
    return toReturn;
}

const cv::Mat GameStateFactory::getThresholdImage(const cv::Mat& in, struct threshold_s& limits) {
    cv::Mat blurred,result;
    cv::GaussianBlur(in, blurred, cv::Size(3, 3), 0, 0); //smooth the original image using Gaussian kernel
    cv::inRange(blurred,
                cv::Scalar(limits.minH, limits.minS, limits.minV),
                cv::Scalar(limits.maxH, limits.maxS, limits.maxV),
                result);
    if(limits.debug) {
        if(limits.doBars) {
            limits.doBars = false;
                int slidermax = 255;
                int sslidermax = 5000;
                cv::namedWindow(limits.windowName, 1);
                cv::createTrackbar("minH", limits.windowName, &(limits.minH), slidermax);
                cv::createTrackbar("maxH", limits.windowName, &(limits.maxH), slidermax);
                cv::createTrackbar("minS", limits.windowName, &(limits.minS), slidermax);
                cv::createTrackbar("maxS", limits.windowName, &(limits.maxS), slidermax);
                //cv::createTrackbar(TrackbarName[4], limits.windowName, &(limits.minV), slidermax);
                //cv::createTrackbar(TrackbarName[5], limits.windowName, &(limits.maxV), slidermax);
                cv::createTrackbar("minArea", limits.windowName, &limits.minArea, sslidermax);
                cv::createTrackbar("maxArea", limits.windowName, &limits.maxArea, sslidermax);
                cv::createTrackbar("minRound", limits.windowName, &limits.minRoundness, sslidermax);
                //cv::resizeWindow(limits.windowName, 500, 800);
                //TableCalibrate.setupTrackbars();
        }
        imshow(limits.windowName, result);
        
    }

    return result;
}

void GameStateFactory::findPieceThread(GamePiece& out, cv::Mat& in, struct threshold_s& limits) {
    ///this exists to make threading easier
    out = findPiece(in, limits);
}

GamePiece GameStateFactory::findPiece(cv::Mat& in, struct threshold_s& limits) {
    GamePiece toReturn = GamePiece();
    cv::Mat imgThresh = getThresholdImage(in, limits);
    std::vector< std::vector<cv::Point> > contours;
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
                        (int)(moment10 * 2 / area)/2,
                        (int)(moment01 * 2 / area)/2
                };

                // limit the region of interest to the table
                if (!within(toReturn.location, Table::min, Table::max)) {
                    toReturn.location = Table::home;
                    continue;  // continue with other contour... (this is outside the table)
                }
                else {
                    cv::drawContours(in, contours, i, limits.outlineColor, 5, 4);
                    toReturn.found = true;
                    toReturn.onTable = within(toReturn.location, Table::min, Table::max);
                    break; //we've found the piece, stop looking
                }
            }
        }
    }
    return toReturn;
}

void GameStateManager::setStateInfo(GameState& gs) {
    GameState lastGs = GameState();
    const int db = 8;
    if(history.size() > 2) { //if there even was a previous state
        lastGs = history[1];
    }
    if(gs.puck.found && gs.mallet.found) {
        lostCnt = 0;
    }
    else {
        lostCnt++;
    }
    setStateInfo(gs.puck, lastGs.puck);
    setStateInfo(gs.mallet, lastGs.mallet);
    if(within(gs.puck.location,
            cvPoint(lastGs.puck.location.x-db, lastGs.puck.location.y-db),
            cvPoint(lastGs.puck.location.x+db, lastGs.puck.location.y+db))
            ) {
//            gs.puck.location = lastGs.puck.location;
    } else {
        gs.puckTraj = Trajectory::calculate(gs);
    }
}

void GameStateManager::setStateInfo(GamePiece& current, const GamePiece& past) {


    if(current.found) {
        /*if(within(
                current.location,
                cvPoint(past.location.x-db, past.location.y-db),
                cvPoint(past.location.x+db, past.location.y+db))
                ) {
            current.location = past.location;

        }*/

        current.lastLocation = past.location;
    }
    else {
        if(lostCnt > maxLostCnt) {
            //we've really lost our pieces
            current.location = Table::home;
            current.lastLocation = Table::home;
        }
        else {
            //predict the puck's next location
            current.lastLocation = past.location;
            current.location   = past.location + (past.location - past.lastLocation);
        }
    }
}


int GameStateManager::getLostCnt() {
    return lostCnt;
}

