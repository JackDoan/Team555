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

};

class GameState {
public:
    cv::Mat& frame;
    GamePiece mallet, puck;

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
public:
    static int getLostCnt();

private:
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



};

#endif //TEAM555_FRAME_H
