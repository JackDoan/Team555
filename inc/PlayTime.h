//
// Created by mike on 4/12/18.
//

#ifndef TEAM555_PLAYTIME_H
#define TEAM555_PLAYTIME_H
#include <time.h>

class PlayTime {
    //todo: if we keep PlayTime, add TimeLimit & ScoreLimit to Settings.
public:
    PlayTime();
    ~PlayTime() = default;
    //void StartGameClock();
    //void ScoreCount();
    void beginGame();
    void manage();
    void resetScore();
    void endGame();

    time_t startTime;
    time_t goalTime;
    double remaining;


    void incrementPlayerPoints();
    void incrementRobotPoint();


    // todo: read this in from config file
    const int timeLimit = 120;
    bool isPlaying;

    unsigned int playerPoints;
    unsigned int robotPoints;


//    bool RobotScore = false;    //set from arduino
//    bool PlayerScore = false;   //set from arduino digitalRead()
};


#endif //TEAM555_PLAYTIME_H
