//
// Created by mike on 4/12/18.
//

#include "../inc/PlayTime.h"

#include <time.h>
#include <stdio.h>



PlayTime::PlayTime() {
    isPlaying = false;
    playerPoints = 0;
    robotPoints = 0;
    goalTime = time(0);
}


void PlayTime::beginGame(){
    resetScore();
    remaining = timeLimit;
    isPlaying = true;
    startTime = time(0);
}

void PlayTime::manage() {
    remaining = timeLimit - difftime(time(0), startTime);
    if (remaining < 0) {
        endGame();
    }
}

void PlayTime::resetScore() {
    playerPoints = 0;
    robotPoints = 0;
}

void PlayTime::endGame() {
    isPlaying = false;
}

void PlayTime::incrementPlayerPoints() {
    if (difftime(time(0), goalTime) > 2) {
        playerPoints++;
        goalTime = time(0);
    }
}

void PlayTime::incrementRobotPoint() {
    if (difftime(time(0), goalTime) > 2) {
        robotPoints++;
        goalTime = time(0);
    }
}