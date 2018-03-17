#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/core/opengl.hpp>
#include <opencv2/highgui.hpp>
#include <opencv/cv.hpp>
#include <opencv2/imgproc.hpp>
#include <sysinfoapi.h>
#include <string>
#include <thread>
#include <ctime>

#include "inc/Table.h"
#include "inc/Puck.h"
#include "inc/Camera.h"
#include "motors/MotorComm.h"
#include "inc/Corners.h"
#include "inc/Serial.h"
#include "inc/MotorDriver.h"
#include "inc/Idle.h"
#include "inc/Config.h"
#include "inc/Mallet.h"
#include "inc/ImageProcess.h"

#include <time.h>


// Camera process, convert puck position to coordinates
// and generate trajectory prediction and visualization
// Xu = (Xd)/(1+param*dist2)  dist2 = distancia al cuadrado del pixel al centro
void cameraProcess(cv::Mat& frameGrabbed, Puck puck, int time, Table table) {
//    int coordX;
//    int coordY;
//    int lastCoordX;
//    int lastCoordY;
//    int vectorX;
//    int vectorY;
    double slope;

    int bounce_x;
    int bounce_y;
    int predict_pixX;
    int predict_pixY;
    int bounce_pixX;
    int bounce_pixY;
}


int main(int argc, char* argv[]) {
    Settings settings;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            // TODO: Calibrate mode is broken, threshold is not working properly, cant see the pucks
            if (std::string(argv[i]) == "-calibrate") {
                settings.calibrateCorners = true;
                printf("calibrateCorners set to true!\n");
            } else {
                settings.calibrateCorners = false;
                printf("calibrateCorners set to false!\n");
            }
        }
//        printf("%s\n", std::string(argv[2]));
    }
//    if (argv[1] == "calibrate") {
//        calibrateCorners = true;
//    } else {
//        calibrateCorners = false;
//    }

// TODO: need to create a debug mode that allows us to turn on and off every possible debugging feature we could use
// like trackbars for thresholding, thresholded images, writing and not writing the video, maybe even wrap all
// all of our printfs for debuggin in a debug version of printf?


    // TODO: need to make a process that identifies the goal areas on the left and right walls and creates 'ranges'
    // on the wall lines that are the goals

    // TODO: thread video writing to improve framerate


//    cv::Mat grabbed;
//    cv::Mat frame;
//    cv::Mat imgHSV;
//    cv::Mat imgThresh;
    cv::VideoWriter record;


    // path to video: C:\AirHockeyRobot\cmake-build-debug
//    bool RunIdle = false;       //Idle Process to show video.
    if (settings.RunIdle) {
        Idle::Idle_Process();
        std::exit(0);
    }

//    time_t start = 0;
//    time_t end = 0;
//    double frameRate = 0;
//    int FrameCounter = 0;
//    double sec;
//    int execs = 0;

//    cv::Point_<int> location;        //Added
//    cv::Point_<int> lastLocation;    //Added
//
//    cv::Point_<int> vectorXY;


    MotorDriver motorDriver = MotorDriver();
    Camera& camera = Camera::getInstance();
    Table table = Table();
    Puck puck = Puck();
    Mallet mallet = Mallet();
    Corners corners = Corners(settings.calibrateCorners);
    if (!settings.calibrateCorners) {
        table.setLimits(corners.sortedX, corners.sortedY);
        puck.setWalls(corners.sortedX, corners.sortedY);
    }


    ImageProcess imageProcess = ImageProcess();


    // TESTING STEPS TO PIXELS RATIO
    /*
    do{
    grabbed = camera.getUndistortedFrame(); // Query a new frame
    mallet.findMallet(grabbed, table);
}while (!mallet.found);
    imshow("Before", grabbed);
    motorDriver.moveSteps(5000, 'x');
    cv::waitKey(5000) >= 0;
do{
    grabbed = camera.getUndistortedFrame(); // Query a new frame
    mallet.findMallet(grabbed, table);

}while (!mallet.found);
    imshow("After", grabbed);
    cv::waitKey(1) >= 0;
*/

//    cv::Size blahhhh = {640, 360};
//    cv::VideoWriter video("output.avi", CV_FOURCC('M', 'J', 'P', 'G'),10, blahhhh);
//    time(&start);
/*    while (true) {
//        clock_t begin = clock();

        if (!undistort) {
            grabbed = camera.getFrame();

            time(&end);
            ++FrameCounter;
            sec = difftime (end, start);
            frameRate = FrameCounter / sec;
            //printf("FPS = %.2f\n", frameRate);

        } else {
            grabbed = camera.getUndistortedFrame(); // Query a new frame
            time(&end);
            ++FrameCounter;
            sec = difftime (end, start);
            frameRate = FrameCounter / sec;
            //printf("FPS = %.2f\n", frameRate);
        }
        if (grabbed.empty()) {
            printf("No frames!\n");
            break;
        }
        if (!calibrateCorners) {
//            puck.lastLocation = puck.location;
//            puck.findPuck(grabbed, table);
//            mallet.findMallet(grabbed, table);


            std::thread puckThread (&Puck::findPuck, std::ref(puck), grabbed, table);
            std::thread malletThread (&Mallet::findMallet, std::ref(mallet), grabbed, table);


            puckThread.join();
            malletThread.join();



//            printf("lastLocation: %d, %d\n", puck.lastLocation.x, puck.lastLocation.y);
//            printf("location: %d, %d\n", puck.location.x, puck.location.y);


//            stepsPerPixel = 35;
//            if(puck.found && mallet.found) {
//                if (abs(puck.location.y - mallet.location.y) <= 30) {
//                    printf("Close enough\n");
//                } else {
//                    int difference = puck.location.y - mallet.location.y;
//                    // NEGATIVE IS TOWARD THE MOTOR
//                    // POSITIVE IS AWAY FROM THE MOTOR
//                    long toMove = (long) (abs(stepsPerPixel) * difference) * -1;
//                    if(abs(toMove) <= 1500) {
//                        printf("not moving %ld steps\n", toMove);
//                        delay = 100;
//                    }
//                    else {
//                        printf("moving %ld steps\n", toMove);
//                        motorDriver.moveSteps(toMove, 'x');
//                        delay = 1000;
//                    }
//
//                }
//            }
            //corners = puck.findPucks(grabbed, table);
            //puck.getCoords(table);
            //puck.getVector(grabbed);
            //puck.getVector(grabbed);
            //printf("\nVectorXY: %f\n", VectorXY);
        }
//        cameraProcess(grabbed, puck, 1000 / table.fps, table); // CAMERA PROCESS (puck coordinates, trajectory...)

        if (table.preview == 1) {
            sprintf(tempStr, "%f %ld %f %f %f\n", frameRate, frameTimestamp - firstTimestamp, puck.location.x,
                    puck.location.y, puck.speedY);
            cv::putText(grabbed, tempStr, cvPoint(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0));
            table.annotate(grabbed);
            cv::Mat previewSmall;
            cv::resize(grabbed, previewSmall, cv::Size(), 0.5, 0.5);

            if (!calibrateCorners) {
                // Draw Table borders
                corners.drawSquareNew(previewSmall, corners.getCalibratedCorners());
            }


*//*            if (corners.size() >= 3) {
                //printf("%d: \t %d,%d \t %d,%d \t %d,%d \t %d,%d\n", corners.size(), corners[0].x, corners[0].y, corners[1].x, corners[1].y, corners[2].x, corners[2].y, corners[3].x, corners[3].y);
            }*//*
            if (calibrateCorners) {
                corners.calibrateCorners(grabbed, previewSmall, table, puck);
                writeConfigValues(corners);
            }

            //GOAL check
            //find midpoints of Y lines being drawn with drawSquareNew
            //      manually add offset in (while drawing lines to check) to find goal zone.


            //look into cvFitLine

            //j test end//////////////////////////////////////
            //(previewSmall);
//            corners.drawSquare(previewSmall, corners.getCorners(), corners.getOffsets());
//            puck.setGoals(previewSmall, corners.sortedX);
            if (video_output) {
                video.write(previewSmall);
            }
            imshow("Video", previewSmall);
//            clock_t end = clock();
//            printf("elapsed: %f\n", double(end-begin)/CLOCKS_PER_SEC);
        }

        if (cv::waitKey(1) >= 0)
            break;

        execs++;

    }*/
    imageProcess.process(table, puck, mallet, corners, camera, settings);
    cvDestroyAllWindows();
    if (settings.video_output) {
        record.release();
    }

    return 0;
}