//
// Created by mdl150330 on 3/15/2018.
//


#include <opencv2/core.hpp>
#include <opencv2/core/opengl.hpp>
#include <opencv2/highgui.hpp>
#include <opencv/cv.hpp>
#include <string>
#include <vector>

#include "../inc/ImageProcess.h"
#include "../inc/Table.h"
#include "../inc/Mallet.h"
#include "../inc/Puck.h"
#include "../inc/Idle.h"
#include "../inc/Config.h"
#include "../inc/Corners.h"
#include "../inc/Camera.h"
#include "../inc/Settings.h"
#include "../inc/MotorDriver.h"
#include "../inc/motion/Motion.h"
#include "../inc/motion/Attack.h"

clock_t begin;
clock_t end;

ImageProcess::ImageProcess(Table& table, Puck& puck, Mallet& mallet, Corners& corners, Settings& settings) {
    threadIt = true;
    this->table = table;
    this->puck = puck;
    this->mallet = mallet;
    this->corners = corners;
    this->settings = settings;
//    cv::VideoWriter video("output.avi", CV_FOURCC('M', 'J', 'P', 'G'),10, cvSize(640, 360));
}

void ImageProcess::process() {
    MotorDriver& motorDriver = MotorDriver::getInstance();
    cv::VideoWriter video("output.avi", CV_FOURCC('M', 'J', 'P', 'G'),10, cvSize(640, 360));
    Motion motion = Motion();
    Attack attack = Attack(table, motorDriver);
    Camera& camera = Camera::getInstance();

    puck.setGoals(corners.sortedX);

    int FrameCounter = 0;
    long frameTimestamp = 0;
    time(&start);
    cv::namedWindow("Video");
    cv::Mat previewSmall;

    motion.calibrateHome(table, mallet, settings);

    std::vector<bool> output = {false, false, false, false};
    bool keepGoing = true;
    bool sendGetButtons = true;
    while (keepGoing) {


        if (settings.checkButtons) {
            if(sendGetButtons) {
                motorDriver.getButtonsCmd();
                sendGetButtons = false;
            }
            else {
                long btns = motorDriver.getButtonsResult();
                if(btns & (1<<5)) {
                    printf("White button pressed!\n");
                }
                if(btns & (1<<4)) {
                    printf("Red button pressed!\n");
                }
                sendGetButtons = true;
            }
        }


        if (!settings.undistort) {
            grabbed = camera.getFrame();
        } else {
            grabbed = camera.getUndistortedFrame(); // Query a new frame
        }
        if (FrameCounter > 60) {
            time(&end);
            sec = difftime (end, start);
            frameRate = FrameCounter / sec;
            time(&start);
            FrameCounter = 1;

        }
        ++FrameCounter;


        if (grabbed.empty()) {
            printf("No frames!\n");
            break;
        }
        if (!settings.calibrateCorners) {
            if(threadIt) {
                std::thread puckThread(&Puck::findOne, std::ref(puck), grabbed, table, false);
                std::thread malletThread(&Mallet::findOne, std::ref(mallet), grabbed, table, true);
                puckThread.join();
                malletThread.join();
            }
            else {
                puck.findOne(grabbed,table,false);
                mallet.findOne(grabbed,table,false);
            }

            int key = cv::waitKey(1);
            ///NOTE: do not wait for any additional input
            ///you will cause the thread to hang
            switch(key) {
                case 27: //ESC
                    keepGoing = false;
                    motorDriver.stop();
                    continue; //this jumps to the top of the while loop and quits
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
                case 'm':
                    threadIt = false;
                    mallet.toggleDebugInfo();
                    break;
                case 'v':
                    settings.video_output = !settings.video_output;
                    printf("Video output: %d\n", settings.video_output);
                    break;
                case 'R':
                    //todo disable drivers
                    //roi = cv::selectROI("Video",previewSmall,true,false);
                    break;
                case 'd':
                    printf("Motion Mode = Defend\n");
                    motionMode = DEFEND;
                    break;
                case 'A':
                    printf("Motion Mode = Attack\n");
                    motionMode = ATTACK;
                    break;
                default:
                    //printf("Motion Mode = IDLE\n");
                    //motionMode = IDLE;
                    break;
            }

            switch(motionMode) {
                case IDLE:
                    //do nothing?  sit at home
                    break;
                case DEFEND:
                    motion.defend(table, mallet, puck, grabbed);
                    break;
                case ATTACK:
                    attack.run(puck, mallet, grabbed);
                    if(!attack.isReady()) {
                        printf("Attack done. Switching to IDLE\n");
                        motionMode = IDLE;
                    }
                    break;
            }

            if (table.preview == 1) {
                sprintf(tempStr, "%3.2f %ld %d %d %d\n", frameRate, frameTimestamp - firstTimestamp, puck.location.x, puck.location.y, puck.vectorXY.y);
                cv::putText(grabbed, tempStr, cvPoint(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 225, 0), 2);
                table.annotate(grabbed);
                cv::rectangle(grabbed,table.motionLimitMax,table.motionLimitMin,cv::Scalar(0,255,0),4);
                cv::resize(grabbed, previewSmall, cv::Size(), 0.5, 0.5);

                if (settings.calibrateCorners) {
                    corners.calibrateCorners(grabbed, previewSmall, table, puck);
                    writeConfigValues(corners);
                }
                else {
                    // Draw Table borders
                    corners.drawSquareNew(previewSmall, corners.getCalibratedCorners());
                }
                //cv::line(previewSmall,table.motionLimitMax,table.motionLimitMin,cv::Scalar(255,0,0),4);
                cv::line(previewSmall, puck.Goals[0]/2, puck.Goals[1]/2, cv::Scalar(255, 0, 0), 4);
                cv::line(previewSmall, puck.Goals[2]/2, puck.Goals[3]/2, cv::Scalar(255, 0, 0), 4);

                if (settings.video_output) {
                    video.write(previewSmall);
                }
                imshow("Video", previewSmall);
            }
        }
//            printf("elapsed: %f\n", double(end-begin)/CLOCKS_PER_SEC);
    }
}
