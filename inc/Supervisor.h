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
#include "Puck.h"
#include "Mallet.h"
#include "Config.h"
#include "Corners.h"
#include "Camera.h"
#include "Settings.h"
#include "MotorDriver.h"
#include "motion/Motion.h"
#include "DoubleBuffer.h"


class Supervisor {
private:
    typedef enum mode_e {IDLE, PLAY} mode_t;
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

    DoubleBuffer frameBuf;



    long firstTimestamp = 0;
    cv::Mat& grabbed = frameBuf.active();
//    cv::Mat frame;
//    cv::Mat imgHSV;
//    cv::Mat imgThresh;
    time_t start = 0;
    time_t end = 0;
    time_t idleStart = 0;
    time_t idleEnd = 0;
    double sec = 0;
    double idleSec = 0;
    Motion motion;
    difficulty_t difficulty = HARD;
    bool threadIt = true;
    Table table;
    Puck puck;
    Mallet mallet;
    Corners corners;
    static Settings settings;
    long FrameCounter = 0;
    bool sendGetButtons;
    bool keepGoing;
    bool doneCheck;
    cv::Mat idleImage;
    static void decorate(cv::Mat in, double frameRate, cv::Point_<int> puckLastLoc, cv::Point_<int> puckLoc, cv::Point_<int> tableMax, cv::Point_<int> tableMin, Corners corners, std::vector<cv::Point_<int>> goals, cv::Point_<int> movingTo);
    static cv::Mat& preview;
    static DoubleBuffer previewBuf;
    static bool pushFrame();
    static bool timeToPushFrame;



public:
    double frameRate = 0;
    static cv::VideoWriter video;
    Supervisor(Table& table, Puck& puck, Mallet& mallet, Corners& corners, Settings& settings);
    ~Supervisor() = default;
    void runCalibrateCorners();
    void run();

    void calcFPS();
    void checkKeyboard(const int& key, MotorDriver &motorDriver, Puck& puck, Mallet& mallet, Settings& settings);
    void idle();
    void decide();
    void play();
    void defense();
    void offense();
    void findPuck();
    bool resetPuck(Motion motion, Table table, Mallet mallet, Puck puck, cv::Mat &grabbed);
    bool stillMovingToCheck();
    void display(cv::Point_<int> movingTo);

};


#endif //TEAM555_IMAGEPROCESS_H
