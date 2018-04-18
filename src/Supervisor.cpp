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
#include "../inc/Mallet.h"
#include "../inc/Puck.h"
#include "../inc/Config.h"
#include "../inc/Corners.h"
#include "../inc/Camera.h"
#include "../inc/Settings.h"
#include "../inc/MotorDriver.h"
#include "../inc/motion/Motion.h"
#include "../inc/motion/Attack.h"

cv::VideoWriter Supervisor::video = cv::VideoWriter("output.avi", CV_FOURCC('M', 'J', 'P', 'G'),10, cvSize(640, 360));
DoubleBuffer Supervisor::previewBuf;
cv::Mat& Supervisor::preview = previewBuf.active();
Settings Supervisor::settings;
bool Supervisor::timeToPushFrame = true;

Supervisor::Supervisor(Table& table, Puck& puck, Mallet& mallet, Corners& corners, Settings& settings) {
    threadIt = true;
    this->table = table;
    this->puck = puck;
    this->mallet = mallet;
    this->corners = corners;
    this->settings = settings;
    //video = cv::VideoWriter("output.avi", CV_FOURCC('M', 'J', 'P', 'G'),10, cvSize(640, 360));
}

void Supervisor::runCalibrateCorners() {
    // this is kinda defunct now
}

void Supervisor::run() {
    // initializing class instances
    MotorDriver& motorDriver = MotorDriver::getInstance();
    motion = Motion();
    Attack attack = Attack(table, motorDriver);
    Camera& camera = Camera::getInstance();

    // set the puck goal locations
    puck.setGoals(corners.sortedX);

    // initialize counters, timers windows and frames etc.
    FrameCounter = 0;

    time(&start);
    time(&idleStart);
    //cv::namedWindow("Video", CV_WINDOW_AUTOSIZE);
    cv::namedWindow("Video", CV_WINDOW_NORMAL);
    // calibrate the steppers home positions
    motion.calibrateHome(table, mallet, settings);

    // calculate speed
    motion.calibrateSpeed(mallet);
    printf("yHome2EdgeTime: %d\n", motion.getYHome2EdgeTime());
    printf("yEdge2EdgeTime: %d\n", motion.getYEdge2EdgeTime());
    printf("xHome2EdgeTime: %d\n", motion.getXHome2EdgeTime());
    printf("xEdge2EdgeTime: %d\n", motion.getXEdge2EdgeTime());



    keepGoing = true;
    sendGetButtons = true;
    doneCheck = false;


    // main while loop that runs the robot
    while (keepGoing) {
        // calculating FPS
        calcFPS();

        // grabbing either an undistorted or distorted frame based on settings
        if (!settings.undistort) {
            frameBuf.active() = camera.getFrame();
        }
        else {
            frameBuf.active() = camera.getUndistortedFrame(); // Query a new frame
        }

        // checking if frames are there
        if (frameBuf.active().empty()) {
            printf("No frames!\n");
            break;
        }

        // if threadIt is set high we thread the contour searching process
        if(threadIt) {
            std::thread puckThread(&Puck::findOne, std::ref(puck), frameBuf.active(), table, false);
            std::thread malletThread(&Mallet::findOne, std::ref(mallet), frameBuf.active(), table, true);
            puckThread.join();
            malletThread.join();
        }
        else {
            puck.findOne(frameBuf.active(),table,false);
            mallet.findOne(frameBuf.active(),table,true);
        }

        int key = cv::waitKey(1);
        checkKeyboard(key, motorDriver, puck, mallet, settings);
        // check if keyboard input set keepGoing to false, if it did, jump to the top and quit
        if (!keepGoing) {
            continue;
        }
        cv::Point_<int> movingTo;
        if (mode == PLAY) {
           // call the decide function that will set the playMode enum and then play
            if (decisionMode == AUTOMATIC) {
                makeDecision();
            }
            switch(playMode) {
                case DEFENSE:
                    // call defense
                    playState = DEFENDING;
//                    movingTo = motion.defend(table, mallet, puck, frameBuf.active());
                    movingTo = motion.defense(mallet, puck, frameBuf.active());
                    break;
                case OFFENSE:
                    // call offense
                    playState = OFFENDING;
                    doneCheck = motion.offense(table, mallet, puck, frameBuf.active());
                    break;
                case FIX:
                    // call impulse
                    playState = FIXING;
                    doneCheck = resetPuck(motion, table, mallet, puck, frameBuf.active());
                    break;
                default:
                    // call defense
                    playState = DEFENDING;
                    movingTo = motion.defense(mallet, puck, frameBuf.active());
                    break;

            }
            if (table.preview == 1) {
                display(movingTo);
            }
        }
        else {
            idle();
        }
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

void Supervisor::checkKeyboard(const int& key, MotorDriver &motorDriver, Puck& puck, Mallet& mallet, Settings& settings) {
    switch(key) {
        case 27: //ESC
            keepGoing = false;
            motorDriver.stop();
        case 'j':
//            motion.calibrateHome(table, mallet, settings);
            break;
        case 'q':
            motorDriver.setEnable(false,false);
            break;
        case 'Q':
            motorDriver.setEnable(true,true);
            break;
        case 'p':
            threadIt = false;
            puck.toggleDebugInfo();
            break;
        case 't':
            threadIt = true;
            break;
        case 'y':
            threadIt = false;
            mallet.toggleDebugInfo();
            break;
        case 'v':
            settings.video_output = !settings.video_output;
            printf("Video output: %d\n", settings.video_output);
            break;
        case 'd':
            printf("Motion Mode = Defend\n");
            playMode = DEFENSE;
            break;
        case 'a':
            printf("Motion Mode = Attack\n");
            playMode = OFFENSE;
            motion.resetOffense();
            break;
        case 'w':
            mode = PLAY;
            printf("Play Mode set to PLAY\n");
            break;
        case 'r':
            mode = IDLE;
            printf("Play Mode set to IDLE\n");
            break;
        case 'e':
            difficulty = EASY;
            break;
        case 'm':
            difficulty = MEDIUM;
            break;
        case 'h':
            difficulty = HARD;
            break;
        case 197:
            if (decisionMode == AUTOMATIC) {
                decisionMode = MANUAL;
                printf("Decision Mode set to MANUAL\n");
            } else if (decisionMode = MANUAL) {
                decisionMode = AUTOMATIC;
                printf("Decision Mode set to AUTOMATIC\n");
            }
            break;
        case 200: //F11 = toggle fullscreen
            cv::setWindowProperty("Video", CV_WND_PROP_FULLSCREEN, !cv::getWindowProperty("Video", CV_WND_PROP_FULLSCREEN));
            break;

        case 'k':
            while(cvWaitKey(1) == -1) {
                cv::Point_<int> fuck = {0, 200};
                motorDriver.moveTo(Table::home + fuck);
                sleep(5);
                motorDriver.moveTo(Table::home - fuck);
                sleep(5);
            }

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

void Supervisor::makeDecision() {
    switch (playState) {
        case DEFENDING:
            if (puck.rightGoal) {
                playMode = DEFENSE;
                break;
            }
            if (within(puck.predictLocation(table, 10), Table::strikeLimitMin, Table::strikeLimitMax)
                && puck.magHistoryAvg < 500 && puck.location.x < mallet.location.x
                && motion.defenseState == ATHOME) {
                playMode = OFFENSE;
                doneCheck = false;
            } else {
                playMode = DEFENSE;
            }
            break;
        // literally does nothing right now
        /*case FIXING:
            if (doneCheck|| puck.rightGoal) {
                playMode = DEFENSE;
                doneCheck = false;
            } else {
                playMode = FIX;
            }
            break;*/
        case OFFENDING:
            if (motion.offenseState == OFFENSEDONE || puck.rightGoal
                || !within(puck.predictLocation(table, 10), Table::strikeLimitMin, Table::strikeLimitMax)
                || puck.magHistoryAvg >= 500) {
                motion.offenseState = OFFENSEDONE;
                playMode = DEFENSE;
                doneCheck = false;
            } else {
                playMode = OFFENSE;
            }
            break;
        default:
            playMode = DEFENSE;
            break;
    }
}

void Supervisor::decide() {
    // if puck.foundHistory is completely false and last predicted location and location was on our side of the table
        // start a timer

    // check the playState with a switch statement

    switch(playState) {
        // if playState is defending we need to decide if we should switch
        case DEFENDING:
            if (puck.rightGoal) {
                playMode = DEFENSE;
                break;
            }
            // check the lostTimer if its greater than 3 seconds
                // set playMode to impulse with argument lost set to true
                // and set playmode to impulse'
            if ((within(puck.location, Table::strikeLimitMin, Table::strikeLimitMax) || within(puck.predictLocation(table, 10), Table::strikeLimitMin, Table::strikeLimitMax))
                && puck.magHistoryAvg < 300 && puck.location.x < mallet.location.x) {
                playMode = OFFENSE;
                doneCheck = false;
            } else {
                playMode = DEFENSE;
            }
            break;
        case FIXING:
            if (doneCheck|| puck.rightGoal) {
                playMode = DEFENSE;
                doneCheck = false;
            } else {
                playMode = FIX;
            }
            break;
        case OFFENDING:
            if (doneCheck || puck.rightGoal) {
                playMode = DEFENSE;
                motion.resetOffense();
                doneCheck = false;
            } else {
                playMode = OFFENSE;
            }
            break;
        default:
            playMode = DEFENSE;
            break;
    }
}



void Supervisor::findPuck() {
    // did we come into this call with FINDINGPUCK high or low?
        // if high is the robot at the checking location?
            // no, wait for the robot to get there (return)
            // yes, is the puck found?
                // yes, is it stationary on our side of the table?
                // calculate an impulse stage position (might need two like draw a leg to get there)
    // TODO: need to store the last found and last predicted location somewhere
    // what was puck's last found location and last predicted location?
    // are either of these in some x-range with the current robot location?
        // if yes then set the playState to FINDINGPUCK
        // need to move the robot forward a little bit to some set location
        // once at this location
}

bool Supervisor::resetPuck(Motion motion, Table table, Mallet mallet, Puck puck, cv::Mat &grabbed) {
    bool impulseDone = false;
    bool toReturn = false;
    if(resetState == RESETDONE) {
        playState = FIXING;
        if(puck.onTable) {
            resetState = RESETTING;
        } else {
            resetState = CHECKING;
        }
    }

    if (resetState == CHECKING) {
        if (!stillMovingToCheck()) {
            resetState = DONECHECKING;
        }
    }

    if (resetState == DONECHECKING) {
        if(puck.onTable) {
            resetState = RESETTING;
        } else {
            resetState = RESETDONE;
        }
    }

    if (resetState == RESETTING) {
        impulseDone = motion.impulse(table, mallet, puck, frameBuf.active());
        if (impulseDone) {
            resetState = RESETDONE;
            toReturn = true;
        }
    }


    return toReturn;
}

bool Supervisor::stillMovingToCheck() {

}

void Supervisor::display(cv::Point_<int> movingTo) {
    if(timeToPushFrame) {
        pushFrame(); //todo threading
        timeToPushFrame = false;
        preview = previewBuf.active();
        frameBuf.toggle();
        //frameBuf.active() = frameBuf.active(); ///need to use both of these
        if(!frameBuf.inactive().empty()) {
            decorate(frameBuf.inactive(), frameRate, puck.lastLocation, puck.location, Table::motionLimitMax, Table::motionLimitMin, corners, puck.Goals, movingTo);
            //auto handle = std::async(std::launch::async, decorate, frameBuf.inactive(), frameRate, puck.lastLocation, puck.location, table.motionLimitMax, table.motionLimitMin, corners, puck.Goals);
        }
        else {
            timeToPushFrame = true;
        }
    }
}

void Supervisor::decorate(cv::Mat in, double frameRate, cv::Point_<int> puckLastLoc, cv::Point_<int> puckLoc, cv::Point_<int> tableMax, cv::Point_<int> tableMin, Corners corners, std::vector<cv::Point_<int>> goals, cv::Point_<int> movingTo) {
    char tempStr[80] = {};
    cv::resize(in, previewBuf.active(), cv::Size(), 0.5, 0.5);
    cv::cvtColor(previewBuf.active(), previewBuf.active(), cv::COLOR_HSV2RGB);
    sprintf(tempStr, "%3.0f %d,%d %d,%d   Mallet Dest: %d,%d", frameRate, puckLastLoc.x, puckLastLoc.y, puckLoc.x, puckLoc.y, movingTo.x, movingTo.y);

    // drawing the motion limits
    cv::rectangle(previewBuf.active(), tableMax, tableMin, cv::Scalar(210, 200, 0), 4);

    // Draw Table borders
    corners.drawSquareNew(previewBuf.active(), corners.getCalibratedCorners());

    //cv::line(previewBuf.active()Small,table.motionLimitMax,table.motionLimitMin,cv::Scalar(255,0,0),4);
    cv::line(previewBuf.active(), goals[0]/2, goals[1]/2, cv::Scalar(180, 255, 255), 4);
    cv::line(previewBuf.active(), goals[2]/2, goals[3]/2, cv::Scalar(180, 255, 255), 4);
    cv::putText(previewBuf.active(), tempStr, cvPoint(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 225, 0), 2);
    switch(playMode) {
        case DEFENSE:
            cv::putText(previewBuf.active(), "Defending", cvPoint(35, 335), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 225, 0), 2);
            break;
        case OFFENSE:
            cv::putText(previewBuf.active(), "Attacking", cvPoint(35, 335), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 225, 0), 2);
            break;
        case FIX:
            cv::putText(previewBuf.active(), "Resetting", cvPoint(35, 335), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 225, 0), 2);
            break;
        default:
            cv::putText(previewBuf.active(), "defending", cvPoint(35, 335), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 225, 0), 2);
            break;
    }
    timeToPushFrame = true;
    previewBuf.toggle();
}

bool Supervisor::pushFrame() {
    if(!previewBuf.inactive().empty()) {
        cv::imshow("Video", previewBuf.inactive());
        if (settings.video_output) {
            video.write(preview);
        }
        return true;
    }
    else {
        return false;
    }
}