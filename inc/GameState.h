//
// Created by mike on 4/19/18.
//

#ifndef TEAM555_FRAME_H
#define TEAM555_FRAME_H


#include <opencv2/core/mat.hpp>
#include "Table.h"

class GamePiece {
public:
//    cv::Point_<int> location = Table::home;
//    cv::Point_<int> lastLocation = Table::home;

    cv::Point_<int> location = {0,0};
    cv::Point_<int> lastLocation = {0,0};

    bool found = false;
    bool onTable = false;
    GamePiece() = default;

};

class GameState {
public:
    std::vector<std::vector<cv::Point_<int>>> puckTraj;
    Table::Goals::goal_t goalFlag = Table::Goals::NO_GOAL;
    cv::Mat frame = cv::Mat();
    GamePiece mallet, puck;
    GameState() = default;

};

class GameStateFactory {
public:
    static GameState build();
    static GameState build(cv::Mat& in);
    static void findPieceThread(GamePiece& out, cv::Mat& in, struct threshold_s& limits);
    static GamePiece findPiece(cv::Mat& in, struct threshold_s& limits);
    const static cv::Mat getThresholdImage(const cv::Mat& in, struct threshold_s& limits);
};

class GameStateManager {
private:
    static void setStateInfo(GamePiece& current, const GamePiece& past);
    static std::vector<GameState> history;
    static const unsigned long maxLen;
    static int lostCnt;
    static const unsigned int maxLostCnt;
    static void setStateInfo(GameState& gs);


public:
    GameStateManager() = default;

    static GameState get() {

        auto gs = GameStateFactory::build();  //build a GS from the frame

        setStateInfo(gs);                       //update it with what happened last time
        update(gs);                             //fill in the history


        return gs;                              //return the full object
    }

    static GameState get(cv::Mat& in) {
        auto gs = GameStateFactory::build(in);  //build a GS from the frame
        setStateInfo(gs);                       //update it with what happened last time
        update(gs);                             //fill in the history
        return gs;                              //return the full object
    }

    static void update(const GameState& in) {
        history.insert(history.begin(), in);
        history.resize(maxLen);
    }

    static int getLostCnt();

    static int getMagHistoryAvg() {
        double magHist = 0;
        for(auto item : history) {
            auto vectorXY = (item.puck.location - item.puck.lastLocation);
            magHist += sqrt((vectorXY.x * vectorXY.x) + (vectorXY.y * vectorXY.y));
        }
        return static_cast<int>(magHist/history.size());
    }

    static bool checkPastGoalFlags() {
        for(auto item : history) {
            if(item.goalFlag == Table::Goals::RIGHT_GOAL) {return true;}
        }
        return false;
    }



};

#endif //TEAM555_FRAME_H
