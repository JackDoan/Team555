//
// Created by mike on 4/12/18.
//

#include "PlayTime.h"

#include "ctime"



void PlayTime::StartGameClock(){
    time_t StartTime = time(nullptr);
    time_t CurrentTime = time(nullptr);

    while((CurrentTime - StartTime) <= (TimeLimit*60)){  //convert time limit to seconds
        //start play mode

        time_t CurrentTime = time(nullptr);
    }
    //end game
}

void PlayTime::ScoreCount(){
    int PlayerPoints = 0;       //These need to be defined outside of ScoreCount so scores are not reset every pass of loop
    int RobotPoints = 0;

    //get signal from photogates/arduino
    /*
     *if(PlayerScore){
     *  PlayerPoints = PlayerPoints + 1;
     *  PlaySound();
     *  }
     *
     * if(RobotScore){
     *  RobotPoints = RobotPoints + 1;
     *  PlaySound();
     *  }
     *
     * */


    //printf("Player: %d, Robot: %d\n", PlayerPoints, RobotPoints);

    if((PlayerPoints = ScoreLimit) || (RobotPoints = ScoreLimit)){
        //end game
    }
}

/*
 * void PlayTime::BeginGame(){
 *  time_t StartTime = time(NULL);
 *  time_t CurrentTime = time(NULL);
 *
 *  int PlayerPoints = 0;
 *  int RobotPoints = 0;
 *
 *  while((CurrentTime - StartTime) <= (TimeLimit*60)){
 *      time_t CurrentTime = time(NULL);
 *
 *      if(PlayerScore){                                    //Change PlayerScore to bool flagged by arduino?
 *            = PlayerPoints + 1;
 *          PlaySound();
 *          }
 *      if(RobotScore){
 *          RobotPoints = RobotPoints + 1;
 *          PlaySound();
 *          }
 *
 *      printf("Player: %d, Robot: %d\n", PlayerPoints, RobotPoints);
 *
 *      if((PlayerPoints = ScoreLimit) || (RobotPoints = ScoreLimit)){
 *          break;
 *          }
 *      }
 *      printf("Game Over");
 *      //end
 * }
 * */