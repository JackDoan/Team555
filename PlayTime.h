//
// Created by mike on 4/12/18.
//

#ifndef TEAM555_PLAYTIME_H
#define TEAM555_PLAYTIME_H


class PlayTime {
    //todo: if we keep PlayTime, add TimeLimit & ScoreLimit to Settings.
    int TimeLimit = 2; //in minutes
    int ScoreLimit = 5;
    void StartGameClock();
    void ScoreCount();
};


#endif //TEAM555_PLAYTIME_H
