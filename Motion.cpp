//
// Created by mdl150330 on 3/19/2018.
//

#include <opencv2/core.hpp>
#include <opencv2/core/opengl.hpp>
#include <opencv2/highgui.hpp>
#include <opencv/cv.hpp>


#include "inc/Motion.h"
#include "inc/MotorDriver.h"
#include "inc/Camera.h"
#include "inc/Table.h"
#include "inc/Mallet.h"
#include "inc/Puck.h"
#include "inc/Settings.h"


Motion::Motion() {

}


void Motion::calibrateHome(MotorDriver motorDriver, Table table, Mallet mallet, Settings settings) {
    Camera& camera = Camera::getInstance();
    bool yHome = false; //set this to false
    bool xHome = false;
    cv::Point_<int> malletHome = {1100, 360};
    cv::Point_<int> firstPosition = {0,0};
    bool first = false;
    cv::Mat calHomeGrabbed;
    cv::Mat calHomeSmall;
    cv::namedWindow("CalHome");


    long initCount;
    double ratio;
    while (!yHome) {
        if (!settings.undistort) {
            calHomeGrabbed = camera.getFrame();
        } else {
            calHomeGrabbed = camera.getUndistortedFrame(); // Query a new frame
        }
        if (calHomeGrabbed.empty()) {
            printf("No frames!\n");
            break;
        }
        mallet.findOld(calHomeGrabbed, table, true);
        if (table.preview == 1) {
            cv::resize(calHomeGrabbed, calHomeSmall, cv::Size(), 0.5, 0.5);
            imshow("CalHome", calHomeSmall);
        }
        if (!mallet.found) {
            printf("Mallet not found in calibrateHome y process!\n");
            if (cv::waitKey(100) >= 0)
                continue;
        }
        if (!first) {
            initCount = motorDriver.getSteps('y');
            // as the drivers where it is and store it as initial count
            firstPosition = mallet.location;
            first = true;
        }

        int stepsPerPixel = 1;
        if (abs(malletHome.y - mallet.location.y) <= 5) {
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
//                printf("%ld steps\n", toMove);
                motorDriver.moveSteps(toMove, 'y');
            }
        }
        if (cv::waitKey(100) >= 0)
            break;
    }
    while (!xHome) {
        if (!settings.undistort) {
            calHomeGrabbed = camera.getFrame();
        } else {
            calHomeGrabbed = camera.getUndistortedFrame(); // Query a new frame
        }
        if (calHomeGrabbed.empty()) {
            printf("No frames!\n");
            break;
        }
        mallet.findOld(calHomeGrabbed, table, true);
        if (table.preview == 1) {
            cv::resize(calHomeGrabbed, calHomeSmall, cv::Size(), 0.5, 0.5);
            imshow("CalHome", calHomeSmall);
        }
        if (!mallet.found) {
            printf("Mallet not found in calibrateHome x process!\n");
            if (cv::waitKey(100) >= 0)
                continue;
        }
        if (!first) {
            initCount = motorDriver.getSteps('x');
            // as the drivers where it is and store it as initial count
            firstPosition = mallet.location;
            first = true;
        }

        int stepsPerPixel = 1;
        if (abs(malletHome.x - mallet.location.x) <= 5) {
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
//                printf("%ld steps\n", toMove);
                motorDriver.moveSteps(toMove, 'x');
            }
        }
        if (cv::waitKey(100) >= 0)
            break;
    }
    // set the current position to the home position
    motorDriver.setHome();
    cvDestroyWindow("CalHome");
}

void Motion::trackPredictedY(MotorDriver motorDriver, Table table, Mallet mallet, Puck puck, cv::Mat grabbed) {
    if(puck.lostCnt < 10 && mallet.found) {
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
            if(abs(toMove) <= 25) {
                //printf("not moving %ld steps\n", toMove);
            }
            else if (puck.onTable && puck.vectorXY.x > 0){
//                printf("%ld steps\n", toMove);
                motorDriver.moveSteps(toMove, 'Y');
            }
            cv::circle(grabbed, cvPoint(mallet.location.x, (toMove)/4 + 360),
                       20, cv::Scalar(255, 0, 0), 4);

        }
    }
}

void Motion::defend(MotorDriver motorDriver, Table table, Mallet mallet, Puck puck, cv::Mat grabbed) {
    long toMoveX;
    long toMoveY;
    cv::Point_<int> desiredLocation = {1100, 360};
    if(puck.lostCnt < 10 && mallet.found) {
        // calc desired mallet location
//        if (true) {
        // i dont know if this intersect check here is needed, just did it cause
        if (puck.goalFlag && puck.intersect.x != 0 && puck.intersect.y != 0) {
//            printf("Goal!!!!!\n");
            cv::putText(grabbed, "Goal!!", cvPoint(450,320), cv::FONT_HERSHEY_SIMPLEX, 10, cv::Scalar(225, 255, 0), 7);
            if (puck.bouncey) {
                desiredLocation.x = (int)puck.intersect.x + puck.vectorXY.x/2;
                desiredLocation.y = (int)puck.intersect.y + puck.vectorXY.y/2;
            } else {
                desiredLocation = puck.location + puck.vectorXY/2;
            }
        } else {
            desiredLocation.x = 1100;
            desiredLocation.y = puck.predictedLocation.y;
        }
        // limit desired location to x and y boundaries if they exceed those boundaries
        if(desiredLocation.y > table.max.y-100) {
            desiredLocation.y = table.max.y-100;
        }
        else if(desiredLocation.y < table.min.y+100) {
            desiredLocation.y = table.min.y+100;
        }

        if(desiredLocation.x > table.max.x-100) {
            desiredLocation.x = table.max.x-100;
        }
        else if(desiredLocation.x < table.min.x+740) {
            desiredLocation.x = table.min.x+200;
        }
        // move to that position

        toMoveX = (desiredLocation.x - 1100) * 4;
        toMoveY = (desiredLocation.y - 720/2) * 4;
        if (abs(toMoveX) <= 200) {
            //printf("not moving %ld steps\n", desiredLocation.x);
        } else if (puck.onTable && puck.vectorXY.x > 0) {
                printf("%ld X steps\n", toMoveX);
//            motorDriver.moveSteps(toMoveX, 'X');
        }
        if (abs(toMoveY) <= 200) {
            //printf("not moving %ld steps\n", desiredLocation.x);
        } else if (puck.onTable && puck.vectorXY.x > 0) {
                printf("%ld Y steps\n", toMoveY);
//            motorDriver.moveSteps(toMoveY, 'Y');

        }

//        printf("Desired Location: (%d, %d)\n", desiredLocation.x, desiredLocation.y);
        cv::circle(grabbed, cvPoint(desiredLocation.x, desiredLocation.y),
                   20, cv::Scalar(225, 255, 0), 6);
    }
}
