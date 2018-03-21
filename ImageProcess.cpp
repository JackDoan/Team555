//
// Created by mdl150330 on 3/15/2018.
//


#include <opencv2/core.hpp>
#include <opencv2/core/opengl.hpp>
#include <opencv2/highgui.hpp>
#include <opencv/cv.hpp>
#include <string>
#include <vector>

#include "inc/ImageProcess.h"
#include "inc/Table.h"
#include "inc/Mallet.h"
#include "inc/Puck.h"
#include "inc/Idle.h"
#include "inc/Config.h"
#include "inc/Corners.h"
#include "inc/Camera.h"
#include "inc/Settings.h"
#include "inc/MotorDriver.h"
#include "inc/Motion.h"

clock_t begin;
clock_t end;

void ImageProcess::process(Table table, Puck puck, Mallet mallet, Corners corners, Camera& camera, Settings settings) {
    MotorDriver motorDriver = MotorDriver();
    Motion motion = Motion();
    bool yHome = false; //set this to false
    bool xHome = false;
    cv::Point_<int> malletHome = {1200, 360};
    cv::Point_<int> firstPosition = {0,0};
    bool first = false;


    puck.setGoals(corners.sortedX);
//    cv::Size blahhhh = {640, 360}; // removed this in the next line and replaced with cvSize(640, 360), didn't test it though
    cv::VideoWriter video("output.avi", CV_FOURCC('M', 'J', 'P', 'G'),10, cvSize(640, 360));
    int FrameCounter = 0;
    long frameTimestamp = 0;
    time(&start); //todo does this fps calc ignores processing time
    cv::namedWindow("Video");
    cv::Rect roi;
    cv::Mat previewSmall;

    // ********* the following commented code is not in Motion.cpp *********//
/*    long initCount;
    double ratio;
    while (!yHome) {
        if (!settings.undistort) {
            grabbed = camera.getFrame();
        } else {
            grabbed = camera.getUndistortedFrame(); // Query a new frame
        }
        if (grabbed.empty()) {
            printf("No frames!\n");
            break;
        }
        mallet.findOld(grabbed, table, true);
        if (!first) {
            initCount = motorDriver.getSteps('y');
            // as the drivers where it is and store it as initial count
            firstPosition = mallet.location;
            first = true;
        }

        cv::circle(grabbed, cvPoint(mallet.location.x, malletHome.y),
                   20, cv::Scalar(255, 0, 200), 4);
        if (table.preview == 1) {
            cv::resize(grabbed, moveSmall, cv::Size(), 0.5, 0.5);
            imshow("Video", moveSmall);
        }

        int stepsPerPixel = 1;
        if (abs(malletHome.y - mallet.location.y) <= 10) {
            //printf("Close enough\n");
            yHome = true;
            long finalCount = motorDriver.getSteps('y');
            long countDif = finalCount - initCount;
            ratio = abs((double)countDif / (double)abs(malletHome.y - firstPosition.y));
//            printf("ratio: %f\n", ratio);
            // ask the drivers where it is and store as final count
//            break;
        } else {
            int difference = malletHome.y - mallet.location.y;
            long toMove = (long) (abs(stepsPerPixel) * difference);
            if(abs(toMove) <= 3) {
                //printf("not moving %ld steps\n", toMove);
            }
            else {
                printf("%ld steps\n", toMove);
                motorDriver.moveSteps(toMove, 'y');
            }
            cv::circle(grabbed, cvPoint(mallet.location.x, difference),
                       25, cv::Scalar(255, 0, 0), 4);
        }

        if (cv::waitKey(100) >= 0)
            break;
    }

    while (!xHome) {
        if (!settings.undistort) {
            grabbed = camera.getFrame();
        } else {
            grabbed = camera.getUndistortedFrame(); // Query a new frame
        }
        if (grabbed.empty()) {
            printf("No frames!\n");
            break;
        }
        mallet.findOld(grabbed, table, true);
        if (!first) {
            initCount = motorDriver.getSteps('x');
            // as the drivers where it is and store it as initial count
            firstPosition = mallet.location;
            first = true;
        }

        cv::circle(grabbed, cvPoint(mallet.location.y, malletHome.x),
                   20, cv::Scalar(255, 0, 200), 4);
        if (table.preview == 1) {
            cv::resize(grabbed, moveSmall, cv::Size(), 0.5, 0.5);
            imshow("Video", moveSmall);
        }

        int stepsPerPixel = 1;
        if (abs(malletHome.x - mallet.location.x) <= 10) {
            //printf("Close enough\n");
            xHome = true;
            long finalCount = motorDriver.getSteps('x');
            long countDif = finalCount - initCount;
            ratio = abs((double)countDif / (double)abs(malletHome.x - firstPosition.x));
//            printf("ratio: %f\n", ratio);
            // ask the drivers where it is and store as final count
//            break;
        } else {
            int difference = mallet.location.x - malletHome.x;
            long toMove = (long) (abs(stepsPerPixel) * difference);
            if(abs(toMove) <= 3) {
                printf("not moving %ld steps\n", toMove);
            }
            else {
                printf("%ld steps\n", toMove);
                motorDriver.moveSteps(toMove, 'x');
            }
            cv::circle(grabbed, cvPoint(difference, mallet.location.y),
                       25, cv::Scalar(255, 0, 0), 4);
        }

        if (cv::waitKey(100) >= 0)
            break;
    }*/
    motion.calibrateHome(motorDriver, table, mallet, settings);



    // TODO: use these as our limits for X
    //motorDriver.moveSteps(1000, 'x');
    //_sleep(1000);
    //motorDriver.moveSteps(-1200, 'x');
    //_sleep(1000);
    //motorDriver.moveSteps(-700, 'x');
    //(1000);
    //motorDriver.moveSteps(200, 'x');

    std::vector<bool> output = {false, false, false, false};
    while (true) {
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
           puck.lastLocation = puck.location;
            //puck.findOld(grabbed, table);
            //mallet.findOld(grabbed, table);

            std::thread puckThread(&Puck::findOld, std::ref(puck), grabbed, table, false);
            std::thread malletThread(&Mallet::findOld, std::ref(mallet), grabbed, table, true);
            puckThread.join();
            malletThread.join();
            // the following code was used to test and debug the new trajectory calculation code
            //puck.calcTrajNew(table, grabbed);

            // the following code was used to test and debug bounce detection function
/*            output = puck.bounceDetect(table, puck.location, puck.location+(puck.location-puck.lastLocation)*20, grabbed);
            if (output[0]) {
                cv::putText(grabbed, "0", cvPoint(450,320), cv::FONT_HERSHEY_SIMPLEX, 10, cv::Scalar(225, 255, 0), 7);
            }
            if (output[1]) {
                cv::putText(grabbed, "1", cvPoint(450,320), cv::FONT_HERSHEY_SIMPLEX, 10, cv::Scalar(225, 255, 0), 7);
            }
            if (output[2]) {
                cv::putText(grabbed, "2", cvPoint(450,320), cv::FONT_HERSHEY_SIMPLEX, 10, cv::Scalar(225, 255, 0), 7);
            }
            if (output[3]) {
                cv::putText(grabbed, "3", cvPoint(450,320), cv::FONT_HERSHEY_SIMPLEX, 10, cv::Scalar(225, 255, 0), 7);
            }*/
            /*if (table.preview == 1) {
                sprintf(tempStr, "%f %ld %d %d %d\n", frameRate, frameTimestamp - firstTimestamp, puck.location.x, puck.location.y, puck.vectorXY.y);
                cv::putText(grabbed, tempStr, cvPoint(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0));
                table.annotate(grabbed);
                cv::resize(grabbed, previewSmall, cv::Size(), 0.5, 0.5);

                if (settings.calibrateCorners) {
                    corners.calibrateCorners(grabbed, previewSmall, table, puck);
                    writeConfigValues(corners);
                }
                else {
                    // Draw Table borders
                    corners.drawSquareNew(previewSmall, corners.getCalibratedCorners());
                }


*//*            if (corners.size() >= 3) {
                //printf("%d: \t %d,%d \t %d,%d \t %d,%d \t %d,%d\n", corners.size(), corners[0].x, corners[0].y, corners[1].x, corners[1].y, corners[2].x, corners[2].y, corners[3].x, corners[3].y);
            }*//*

                //GOAL check
                //find midpoints of Y lines being drawn with drawSquareNew
                //      manually add offset in (while drawing lines to check) to find f zone.


                //look into cvFitLine

                //j test end//////////////////////////////////////
                //(previewSmall);
//            corners.drawSquare(previewSmall, corners.getCorners(), corners.getOffsets());
//            puck.setGoals(previewSmall, corners.sortedX);
//              puck.setGoals(previewSmall, corners.sortedX);

                if (settings.video_output) {
                    video.write(previewSmall);
                }
                imshow("Video", previewSmall);
//            printf("elapsed: %f\n", double(end-begin)/CLOCKS_PER_SEC);
            }*/
            int key = cv::waitKey(1);
            ///NOTE: do not wait for any additional input
            ///you will cause the thread to hang
            switch(key) {
                case 27: //ESC
                    //do something to exit the program
                    break;
                case 'q':
                    motorDriver.setEnable(0,0);
                    break;
                case 'Q':
                    motorDriver.setEnable(1,1);
                    break;
                case 'p':
                    puck.toggleDebugInfo();
                    break;
                case 'm':
                    mallet.toggleDebugInfo();
                    break;
                case 'R':
                    //todo disable drivers
                    //roi = cv::selectROI("Video",previewSmall,true,false);
                    break;
                default:
                    if(key > 0)
                        printf("Key: %d\n", key);
                    break;
            }

            //printf("lastLocation: %d, %d\n", puck.lastLocation.x, puck.lastLocation.y);
            //printf("loc: %d, ploc:%d\n", puck.predictedLocation.y, puck.location.y);

            // this is now in Motion::trackPredictedY()
            /*if(puck.lostCnt < 10 && mallet.found) {
                if (abs(puck.location.y - mallet.location.y) <= 30) {
                    //printf("Close enough\n");
                } else {
                    long toMove;
                    //int difference = puck.predictedLocation.y - 720/2;//mallet.location.y;
                    //todo why is ratio 4
                    if(puck.predictedLocation.y > table.max.y-100) {
                        toMove = table.max.y-100;
                    }
                    else if(puck.predictedLocation.y < table.min.y+100) {
                        toMove = table.min.y+100;
                    }
                    else {
                        toMove = puck.predictedLocation.y;
                    }
                    toMove = ((toMove-720/2) * 4); //get to the middle
                    if(abs(toMove) <= 15) {
                       //printf("not moving %ld steps\n", toMove);
                    }
                    else if (puck.onTable){
                        printf("%ld steps\n", toMove);
                        motorDriver.moveSteps(toMove, 'Y');
                    }
                    cv::circle(grabbed, cvPoint(mallet.location.x, (toMove+720/2)/4),
                               20, cv::Scalar(255, 0, 0), 4);

                }
            }*/

            // **** Uncomment ONE of the following linex to actually play **** //
//            motion.trackPredictedY(motorDriver, table, mallet, puck, grabbed);
            motion.defend(motorDriver, table, mallet, puck, grabbed);


            if (table.preview == 1) {
                sprintf(tempStr, "%f %ld %d %d %d\n", frameRate, frameTimestamp - firstTimestamp, puck.location.x, puck.location.y, puck.vectorXY.y);
                cv::putText(grabbed, tempStr, cvPoint(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0));
                table.annotate(grabbed);
                cv::resize(grabbed, previewSmall, cv::Size(), 0.5, 0.5);

                if (settings.calibrateCorners) {
                    corners.calibrateCorners(grabbed, previewSmall, table, puck);
                    writeConfigValues(corners);
                }
                else {
                    // Draw Table borders
                    corners.drawSquareNew(previewSmall, corners.getCalibratedCorners());
                }


/*            if (corners.size() >= 3) {
                //printf("%d: \t %d,%d \t %d,%d \t %d,%d \t %d,%d\n", corners.size(), corners[0].x, corners[0].y, corners[1].x, corners[1].y, corners[2].x, corners[2].y, corners[3].x, corners[3].y);
            }*/

                //GOAL check
                //find midpoints of Y lines being drawn with drawSquareNew
                //      manually add offset in (while drawing lines to check) to find goal zone.


                //look into cvFitLine

                //j test end//////////////////////////////////////
                //(previewSmall);
//            corners.drawSquare(previewSmall, corners.getCorners(), corners.getOffsets());
//            puck.setGoals(previewSmall, corners.sortedX);
              //puck.setGoals(previewSmall, corners.sortedX);



                cv::line(previewSmall, puck.Goals[0]/2, puck.Goals[1]/2, cv::Scalar(255, 0, 0), 4);
                cv::line(previewSmall, puck.Goals[2]/2, puck.Goals[3]/2, cv::Scalar(255, 0, 0), 4);

                if (settings.video_output) {
                    video.write(previewSmall);
                }
                imshow("Video", previewSmall);

            }
//            corners = puck.findPucks(grabbed, table);
//            puck.getCoords(table);
//            puck.getVector(grabbed);
//            puck.getVector(grabbed);
//            printf("\nVectorXY: %f\n", VectorXY);
        }
//            printf("elapsed: %f\n", double(end-begin)/CLOCKS_PER_SEC);
    }
}
