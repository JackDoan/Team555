//
// Created by mdl150330 on 3/15/2018.
//


#include <opencv2/core.hpp>
#include <opencv2/core/opengl.hpp>
#include <opencv2/highgui.hpp>
#include <opencv/cv.hpp>
#include <string>
#include <vector>
#include <future>
#include "unistd.h"

#include "../inc/Supervisor.h"
#include "../inc/Table.h"
#include "../inc/Config.h"
#include "../inc/Corners.h"
#include "../inc/Camera.h"
#include "../inc/Settings.h"
#include "../inc/MotorDriver.h"
#include "../inc/motion/Motion.h"
#include "../inc/motion/Offense.h"
#include "../inc/GameState.h"
#include "../inc/motion/Trajectory.h"
#include "../inc/PlayTime.h"
#include "../lib/PracticalSocket.h"

cv::VideoWriter Supervisor::video = cv::VideoWriter("output.avi", CV_FOURCC('M', 'J', 'P', 'G'),10, cv::Size(Camera::getInstance().dimensions));


Supervisor::Supervisor() {
    motion = Motion();
}



void Supervisor::run() {
    // initializing class instances
    MotorDriver& motorDriver = MotorDriver::getInstance();
    Camera& camera = Camera::getInstance();

    // initialize counters, timers windows and frames etc.
    FrameCounter = 0;

    time(&start);
    time(&idleStart);

    cv::namedWindow("Video", CV_WINDOW_NORMAL);

    keepGoing = true;
    doneCheck = false;
    mode = CALIBRATE;

    // main while loop that runs the robot
    while (keepGoing) {
        calcFPS();
        auto gameState = GameStateManager::get();
        checkKeyboard();
        // check if keyboard input set keepGoing to false, if it did, jump to the top and quit
        if (!keepGoing) {
            continue;
        }

        cv::Point_<int> movingTo;
        if(mode == CALIBRATE) {
            motion.calibrate.run();
            mode = PLAY;
        }

        else if(mode == PLAY) {
           // call the decide function that will set the playMode enum and then play
            playTime.manage();
            lostCnt = GameStateManager::getLostCnt();
            if (!playTime.isPlaying) {
                mode = IDLE;
            }
            if (decisionMode == AUTOMATIC) {
                makeDecision(gameState);
            }
            switch(playMode) {
                case OFFENSE:
                    // call offense
                    playState = OFFENDING;
                    movingTo = motion.offense.run(gameState, frameRate);
                    break;
                case FIX:
                    // call impulse
                    playState = FIXING;
                    resetDone = motion.resetPuck.run(gameState, GameStateManager::getLostCnt());
                    ///doneCheck = resetPuck(motion, table, mallet, puck, frameBuf.active());
                    break;
                case DEFENSE:
                default:
                    // call defense
                    playState = DEFENDING;
                    movingTo = motion.defense.run(gameState, intersectPoint);
                    break;
            }
            display(gameState, movingTo);
        }
        else {
            idle();
        }
       // printf("lostCnt = %d\n", GameStateManager::getLostCnt());
    }
}

void Supervisor::calcFPS() {
    time(&end);
    sec = difftime (end, start);
    if (sec > 1) {
        frameRate = FrameCounter / sec;
        FrameCounter = 0;
        time(&start);
    }
    ++FrameCounter;
}


bool intersection(const cv::Point_<int>& o1, const cv::Point_<int>& p1, const cv::Point_<int>& o2, const cv::Point_<int>& p2,
                  cv::Point_<int> &r) {
    cv::Point_<int> x = o2 - o1;
    cv::Point_<int> d1 = p1 - o1;
    cv::Point_<int> d2 = p2 - o2;

    float cross = d1.x*d2.y - d1.y*d2.x;
    if (abs(cross) < 1e-6 || abs(cross) > 99999)
        return false;

    auto t1 = (x.x * d2.y - x.y * d2.x)/cross;
    r = o1 + d1 * t1;
    return true;
}

void Supervisor::checkKeyboard() {
    const int key = cv::waitKey(1);
    switch(key) {
        case 27: //ESC
            keepGoing = false;
            MotorDriver::getInstance().stop();
            break;
        case 'f':
            MotorDriver::getInstance().toggleFan();
            break;
        case 'j':
            mode = CALIBRATE;
            cv::displayOverlay("Video", "Robot is re-calibrating. Please wait.", 2000);
            break;
//        case 'p':
//            Settings::threadFindingThings = false;
//            puck.toggleDebugInfo();
//            break;
//        case 't':
//            Settings::threadFindingThings = true;
//            break;
//        case 'y':
//            Settings::threadFindingThings = false;
//            mallet.toggleDebugInfo();
//            break;
        case '3':
            // increment player goal count
            if (playTime.isPlaying) {
                playTime.incrementPlayerPoints();
            }
            break;
        case '5':
            // increment robot goal count
            if (playTime.isPlaying) {
                playTime.incrementRobotPoint();
            }
            break;
        case 'v': {
            char buf[80];
            Settings::video_output = !Settings::video_output;
            snprintf(buf, sizeof(buf), "Video output: %d\n", Settings::video_output);
            cv::displayOverlay("Video", buf, 2000);
        }
            break;
        case 'd':
            printf("Motion Mode = Defend\n");
            playMode = DEFENSE;
            break;
        case 'a':
            printf("Motion Mode = Attack\n");
            playMode = OFFENSE;
            break;
        case 'w':
            mode = PLAY;
            if (!playTime.isPlaying) {
                playTime.beginGame();
            }
            printf("Play Mode set to PLAY\n");
            break;
        case 'r':
            mode = IDLE;
            if (playTime.isPlaying) {
                playTime.endGame();
            }
            printf("Play Mode set to IDLE\n");
            break;
        case 'e':
            difficulty = EASY;
            cv::displayOverlay("Video", "Easy Mode!", 2000);
            break;
        case 'm':
            difficulty = MEDIUM;
            cv::displayOverlay("Video", "Medium Mode!", 2000);
            break;
        case 'h':
            difficulty = HARD;
            cv::displayOverlay("Video", "HARD MODE!", 2000);
            break;
        case 197: //F8
            if (decisionMode == AUTOMATIC) {
                decisionMode = MANUAL;
                printf("Decision Mode set to MANUAL\n");
            } else if (decisionMode == MANUAL) {
                decisionMode = AUTOMATIC;
                printf("Decision Mode set to AUTOMATIC\n");
            }
            break;
        case 200: //F11 = toggle fullscreen
            cv::setWindowProperty("Video", CV_WND_PROP_FULLSCREEN, !cv::getWindowProperty("Video", CV_WND_PROP_FULLSCREEN));
            break;
        case 191: {
            MotorDriver::getInstance().stop();
            Table::acceptMouseInput = true;
            cv::setMouseCallback("Video", Table::mouseHelper, nullptr); //todo draw the corners
            while (Table::acceptMouseInput) {
                cv::displayOverlay("Video", "Select the corners, then click middle click", 1000);
                cv::waitKey(1);
            }
            Table::corners.setCorners(Table::newCorners); //update things
            Table::goals.recalculate(Table::corners);
            Table::setLimits();
            Settings::writeConfigValues();
            }
            break;

        default:
            if(key != -1) {
                printf("You pressed %d\n", key);
            }
            break;
    }
}

void Supervisor::idle() {

    static int idleFrameCount = 0;

    time(&idleEnd);
    idleSec = difftime(idleEnd, idleStart);
    if (idleSec > 5) {
        // increment through images
        time(&idleStart);
        idleFrameCount++;

    }
    switch (idleFrameCount % 3) {
        case 0:
            idleImage = cv::imread("pic1.bmp", CV_LOAD_IMAGE_COLOR);
            if (!idleImage.data) {
                printf("Can't open idle image\n");
            } else {
                imshow("Video", idleImage);
            }
            break;
        case 1:
            idleImage = cv::imread("pic2.jpg", CV_LOAD_IMAGE_COLOR);
            if (!idleImage.data) {
                printf("Can't open idle image\n");
            } else {
                imshow("Video", idleImage);
            }
            break;
        case 2:
            idleImage = cv::imread("pic3.png", CV_LOAD_IMAGE_COLOR);
            if (!idleImage.data) {
                printf("Can't open idle image\n");
            } else {
                imshow("Video", idleImage);
            }
            break;
        default:
            break;
    }
}

void Supervisor::makeDecision(GameState& gs) {
    //calculate trajectory:

    auto traj = Trajectory::calculate(gs);
    auto magHistoryAvg = GameStateManager::getMagHistoryAvg();
    need2Defend = false;
    if (gs.puck.found) {
        for(auto leg :gs.puckTraj) {
            if(intersection(leg[0], leg[1], {Table::home.x, Table::motionLimitMax.y}, {Table::home.x, Table::motionLimitMin.y}, intersectPoint)) {
                need2Defend = true;
                break;
            }
        }
        if (need2Defend) {
            playMode = DEFENSE;
        } else if (gs.puck.location.x < gs.mallet.location.x){
            playMode = OFFENSE;
        } else {
            MotorDriver::getInstance().moveTo(Table::home);
        }

        /*switch (playState) {
            case DEFENDING:
                if (gs.goalFlag == Table::Goals::RIGHT_GOAL) {
                    playMode = DEFENSE;
                    break; ///Return immediately, we need to block.
                } else {
                    playMode = OFFENSE;
                }
                break;
                case FIXING:
                    if (resetDone) {
                        playMode = DEFENSE;
                    } else {
                        playMode = FIX;
                    }
                    break;
            case OFFENDING:
                if (gs.goalFlag == Table::Goals::RIGHT_GOAL) {
                    playMode = DEFENSE;
                } else {
                    playMode = OFFENSE;
                }
                break;
            default:
                playMode = DEFENSE;
                break;
        }*/
    } else {
        // dont decide, use the last decision
    }

}


void Supervisor::display(const GameState gs, const cv::Point_<int> movingTo) {
    static int frameDelayer = 0;
    if (!gs.frame.empty()) {
        if((++frameDelayer % 4) == 0) {
//            cv::Mat toDisplay;
//            toDisplay = gs.frame.clone();
//            decorate(gs, toDisplay, frameRate, movingTo);
//            cv::imshow("Video", toDisplay);
            decorate(gs, gs.frame, frameRate, movingTo);
            cv::imshow("Video", gs.frame);
            if (Settings::video_output) {
                //Supervisor::video.write(toDisplay);
                Supervisor::video.write(gs.frame);
            }
        }
    }
}
const cv::QtFont font = cv::fontQt("Monospace", 30, cv::Scalar(255, 225, 0));
void Supervisor::decorate(GameState gs, cv::Mat in, double frameRate, cv::Point_<int> movingTo) {
    char tempStr[80] = {};
    const double ratio = 1.0;

    std::string modeStrings[] = {"Defending", "Attacking", "Fixing", "Bottom Text"};
    int modeStringIndex = 0;

    cv::cvtColor(in, in, cv::COLOR_HSV2RGB);
    sprintf(tempStr, "FPS: %3.0f Puck Loc: %d,%d   Mallet Dest: %d,%d", frameRate,
            gs.puck.location.x, gs.puck.location.y, movingTo.x, movingTo.y);

/*    std::vector<cv::Point_<int>> testTraj = Trajectory::newCalc(gs);
    for(int i = 0; i < testTraj.size(); i ++) {
        cv::circle(in, testTraj[i], 2, cv::Scalar(255, 255, 255), 4);
    }*/
    for (int i = 0; i < gs.puckTraj.size(); i ++) {
        cv::line(in, gs.puckTraj[i][0], gs.puckTraj[i][1], cv::Scalar(255, 255, 255), 4);
    }

    // drawing the motion limits
    cv::rectangle(in, Table::motionLimitMax * ratio, Table::motionLimitMin * ratio, cv::Scalar(210, 200, 0), 4);
    cv::circle(in, movingTo * ratio, 20, cv::Scalar(100, 200, 0), 4);
    // Draw Table borders
    Table::corners.drawSquareNew(in, Table::corners.getCorners());

    cv::line(in, Table::goals.goals[0] * ratio, Table::goals.goals[1] * ratio, cv::Scalar(180, 255, 255), 4);
    cv::line(in, Table::goals.goals[2] * ratio, Table::goals.goals[3] * ratio, cv::Scalar(180, 255, 255), 4);
    cv::addText(in, tempStr, cvPoint(10, 50), font);

    sprintf(tempStr, "Player: %d Robot: %d Time: %3.0f", playTime.playerPoints, playTime.robotPoints, playTime.remaining);
    cv::addText(in, tempStr, cvPoint(550, 345*2), font);
    switch(playMode) {
        case DEFENSE:
            modeStringIndex = 0;
            break;
        case OFFENSE:
            modeStringIndex = 1;
            break;
        case FIX:
            modeStringIndex = 2;
            break;
        default:
            modeStringIndex = 3;
            break;
    }
    cv::addText(in, modeStrings[modeStringIndex], cvPoint(35, 345*2), font);
}