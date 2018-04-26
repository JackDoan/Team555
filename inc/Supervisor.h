//
// Created by mdl150330 on 3/15/2018.
//

#ifndef TEAM555_IMAGEPROCESS_H
#define TEAM555_IMAGEPROCESS_H
#include <opencv2/core.hpp>
#include <opencv2/core/opengl.hpp>
#include <opencv2/highgui.hpp>
#include <opencv/cv.hpp>
#include <string>
#include <vector>


#include "Table.h"
#include "Config.h"
#include "Corners.h"
#include "Camera.h"
#include "Settings.h"
#include "MotorDriver.h"
#include "motion/Motion.h"
#include "DoubleBuffer.h"
#include "states.h"
#include "PlayTime.h"


class Supervisor {
private:
    typedef enum mode_e {IDLE, PLAY, CALIBRATE} mode_t;
    typedef enum playMode_e {DEFENSE, OFFENSE, FIX} playMode_t;
    typedef enum playState_e {DEFENDING, OFFENDING, FIXING} playState_t;
    typedef enum resetState_e {RESETDONE, CHECKING, DONECHECKING, RESETTING} resetState_t;
    typedef enum difficulty_e {EASY, MEDIUM, HARD} difficulty_t;
    typedef enum decisionMode_e {AUTOMATIC, MANUAL} decisionMode_t;
    mode_t mode = IDLE;
    playMode_t playMode = DEFENSE;
    playState_t playState = DEFENDING;
    resetState_t resetState = RESETDONE;
    decisionMode_t decisionMode = AUTOMATIC;
    bool resetDone = false;
    bool need2Defend = false;
    cv::Point_<int> intersectPoint;
    int lostCnt;

    DoubleBuffer frameBuf;

    long firstTimestamp = 0;

    time_t start = 0;
    time_t end = 0;
    time_t idleStart = 0;
    time_t idleEnd = 0;
    double sec = 0;
    double idleSec = 0;
    Motion motion;
    difficulty_t difficulty = HARD;
    long FrameCounter = 0;
    bool sendGetButtons;
    bool keepGoing;
    bool doneCheck;
    cv::Mat idleImage;
    void decorate(GameState gs, cv::Mat in, double frameRate, cv::Point_<int> movingTo);
    static cv::Mat& preview;
    static DoubleBuffer previewBuf;
    static void pushFrame();
    static bool timeToPushFrame;
    static bool pushNetworkFrame(const cv::Mat& send);



public:
    PlayTime playTime;

    double frameRate = 0;
    static cv::VideoWriter video;
    Supervisor();
    ~Supervisor() = default;
    void runCalibrateCorners();
    void run();

    void calcFPS();
    void checkKeyboard();
    void idle();
    void makeDecision(GameState& gs);
    void findPuck();
    bool resetPuck(const GameState& gs);
    bool stillMovingToCheck();
    void display(GameState gs, cv::Point_<int> movingTo);

};


#endif //TEAM555_IMAGEPROCESS_H
