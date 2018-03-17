//
// Created by mdl150330 on 3/15/2018.
//


#include <opencv2/core.hpp>
#include <opencv2/core/opengl.hpp>
#include <opencv2/highgui.hpp>
#include <opencv/cv.hpp>
#include <string>

#include "ImageProcess.h"
#include "Table.h"
#include "Mallet.h"
#include "Puck.h"
#include "Idle.h"
#include "Config.h"
#include "Corners.h"
#include "Camera.h"
#include "Settings.h"

ImageProcess::ImageProcess(Table table, Puck puck, Mallet mallet, Corners corners, Camera camera, Settings settings){

};

ImageProcess::~ImageProcess() = default;

void ImageProcess::process(Table table, Puck puck, Mallet mallet, Corners corners, Camera camera, Settings settings) {
    cv::VideoWriter video("output.avi", CV_FOURCC('M', 'J', 'P', 'G'),10, blahhhh);
    int FrameCounter = 0;
    long frameTimestamp = 0;
    time(&start);
    while (true) {
        if (!settings.undistort) {
            grabbed = camera.getFrame();
            time(&end);
            ++FrameCounter;
            sec = difftime (end, start);
            frameRate = FrameCounter / sec;
        } else {
            grabbed = camera.getUndistortedFrame(); // Query a new frame
            time(&end);
            ++FrameCounter;
            sec = difftime (end, start);
            frameRate = FrameCounter / sec;
        }
        if (grabbed.empty()) {
            printf("No frames!\n");
            break;
        }
        if (!settings.calibrateCorners) {
/*            puck.lastLocation = puck.location;
            puck.findPuck(grabbed, table);
            mallet.findMallet(grabbed, table);*/


            std::thread puckThread (&Puck::findPuck, std::ref(puck), grabbed, table);
            std::thread malletThread (&Mallet::findMallet, std::ref(mallet), grabbed, table);


            puckThread.join();
            malletThread.join();
            if (table.preview == 1) {
                sprintf(tempStr, "%f %ld %f %f %f\n", frameRate, frameTimestamp - firstTimestamp, puck.location.x,
                        puck.location.y, puck.speedY);
                cv::putText(grabbed, tempStr, cvPoint(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0));
                table.annotate(grabbed);
                cv::Mat previewSmall;
                cv::resize(grabbed, previewSmall, cv::Size(), 0.5, 0.5);

                if (!settings.calibrateCorners) {
                    // Draw Table borders
                    corners.drawSquareNew(previewSmall, corners.getCalibratedCorners());
                }


/*            if (corners.size() >= 3) {
                //printf("%d: \t %d,%d \t %d,%d \t %d,%d \t %d,%d\n", corners.size(), corners[0].x, corners[0].y, corners[1].x, corners[1].y, corners[2].x, corners[2].y, corners[3].x, corners[3].y);
            }*/
                if (settings.calibrateCorners) {
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
//              puck.setGoals(previewSmall, corners.sortedX);

                if (settings.video_output) {
                    video.write(previewSmall);
                }
                imshow("Video", previewSmall);
//            clock_t end = clock();
//            printf("elapsed: %f\n", double(end-begin)/CLOCKS_PER_SEC);
            }

            if (cv::waitKey(1) >= 0)
                break;



/*            printf("lastLocation: %d, %d\n", puck.lastLocation.x, puck.lastLocation.y);
            printf("location: %d, %d\n", puck.location.x, puck.location.y);


            stepsPerPixel = 35;
            if(puck.puckFound && mallet.puckFound) {
                if (abs(puck.location.y - mallet.location.y) <= 30) {
                    printf("Close enough\n");
                } else {
                    int difference = puck.location.y - mallet.location.y;
                    // NEGATIVE IS TOWARD THE MOTOR
                    // POSITIVE IS AWAY FROM THE MOTOR
                    long toMove = (long) (abs(stepsPerPixel) * difference) * -1;
                    if(abs(toMove) <= 1500) {
                        printf("not moving %ld steps\n", toMove);
                        delay = 100;
                    }
                    else {
                        printf("moving %ld steps\n", toMove);
                        motorDriver.moveSteps(toMove, 'x');
                        delay = 1000;
                    }

                }
            }
            corners = puck.findPucks(grabbed, table);
            puck.getCoords(table);
            puck.getVector(grabbed);
            puck.getVector(grabbed);
            printf("\nVectorXY: %f\n", VectorXY);*/
        }
    }


}