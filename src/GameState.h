//
// Created by mike on 4/19/18.
//

#ifndef TEAM555_FRAME_H
#define TEAM555_FRAME_H


#include <opencv2/core/mat.hpp>
#include "../inc/Table.h"

class GamePiece {
public:
    cv::Point_<int> location = Table::home;
    cv::Point_<int> lastLocation = Table::home;

    bool found = false;
    bool onTable = false;
    GamePiece() = default;

};

class GameState {
public:
    Table::Goals::goal_t goalFlag = Table::Goals::NO_GOAL;
    cv::Mat frame = cv::Mat();
    GamePiece mallet, puck;
    GameState() = default;

};

class GameStateFactory {
public:
    static GameState build(cv::Mat& in);
    static void findPiece(GamePiece& out, cv::Mat& in, const struct threshold_s& limits);
    static GamePiece findPiece(cv::Mat& in, const struct threshold_s& limits);
    const static cv::Mat getThresholdImage(const cv::Mat& in, const struct threshold_s& limits);
};

class GameStateManager {
private:
    static std::vector<GameState> history;
    static const unsigned long maxLen;
    static int lostCnt;
    static const unsigned int maxLostCnt;
    static void setStateInfo(GameState& gs);



public:
    GameStateManager() = default;

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



};

#endif //TEAM555_FRAME_H
