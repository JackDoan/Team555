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

void ImageProcess::process(Table table, Puck puck, Mallet mallet, Corners corners, Camera& camera, Settings settings) {
    MotorDriver motorDriver = MotorDriver();
    Motion motion = Motion();
    Attack attack = Attack(table, motorDriver);
    bool threadIt = true;

    puck.setGoals(corners.sortedX);
    cv::VideoWriter video("output.avi", CV_FOURCC('M', 'J', 'P', 'G'),10, cvSize(640, 360));
    int FrameCounter = 0;
    long frameTimestamp = 0;
    time(&start); //todo does this fps calc ignores processing time
    cv::namedWindow("Video");
    cv::Mat previewSmall;

    motion.calibrateHome(motorDriver, table, mallet, settings);

    // TODO: use these as our limits for X
    //motorDriver.sendCMD(1000, 'x');
    //_sleep(1000);
    //motorDriver.sendCMD(-1200, 'x');
    //_sleep(1000);
    //motorDriver.sendCMD(-700, 'x');
    //(1000);
    //motorDriver.sendCMD(200, 'x');

    std::vector<bool> output = {false, false, false, false};
    bool keepGoing = true;
    while (keepGoing) {
        if (!settings.undistort) {
            grabbed = camera.getFrame();
        } else {
            grabbed = camera.getUndistortedFrame(); // Query a new frame
        }
        if (FrameCounter > 600) {
            time(&start);
            FrameCounter = 1;
        }
        time(&end);
        ++FrameCounter;
        sec = difftime (end, start);
        frameRate = FrameCounter / sec;
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
                    //do nothing?
                    break;
                case DEFEND:
                    motion.defend(motorDriver, table, mallet, puck, grabbed);
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
                sprintf(tempStr, "%f %ld %d %d %d\n", frameRate, frameTimestamp - firstTimestamp, puck.location.x, puck.location.y, puck.vectorXY.y);
                cv::putText(grabbed, tempStr, cvPoint(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0));
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
