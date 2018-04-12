//
// Created by mike on 4/12/18.
//

#include "PlayTime.h"

#include "time.h"



void PlayTime::StartGameClock(){
    time_t StartTime = time(NULL);
    time_t CurrentTime = time(NULL);

    while((CurrentTime - StartTime) <= (TimeLimit*60)){  //convert time limit to seconds
        //start play mode

        time_t CurrentTime = time(NULL);
    }
    //end game
}

void PlayTime::ScoreCount(){
    int PlayerPoints = 0;
    int RobotPoints = 0;

    //get signal from photogates/arduino

    //printf("Player: %d, Robot: %d\n", PlayerPoints, RobotPoints);

    if((PlayerPoints = ScoreLimit) || (RobotPoints = ScoreLimit)){
        //end game
    }
}
