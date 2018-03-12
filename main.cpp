#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/core/opengl.hpp>
#include <opencv2/highgui.hpp>
#include <opencv/cv.hpp>
#include <opencv2/imgproc.hpp>
#include <sysinfoapi.h>
#include <string>

#include "Table.h"
#include "Puck.h"
#include "Camera.h"
#include "motors/MotorComm.h"
#include "Corners.h"
#include "Serial.h"
#include "MotorDriver.h"
#include "Idle.h"
#include "Config.h"


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

// Robot process, convert robot position to coordinates
/* currently unused -- we're only tracking the puck
void robotProcess() {
	int coordX;
	int coordY;

	if ((RposX == 0) || (RposY == 0)) {
		robotCoordX = 0;
		robotCoordY = 0;
	}
	else {
		// Convert from Camera reference system to Robot reference system
		// Camera X axis correspond to robot Y axis
		coordY = (RposX - cam_center_x);   // First we convert image coordinates to center of image
		coordX = (RposY - cam_center_y);
		robotCoordY = robot_table_center_y - coordY*cam_pix_to_mm + robot_y_offset;
		robotCoordX = robot_table_center_x - coordX*cam_pix_to_mm;
	}
} */


int main(int argc, char* argv[]) {
    bool video_output = true;   //can be shown through Idle Process
    char tempStr[80];
    long frameTimestamp = 0;
    long firstTimestamp = 0;
    bool undistort = true;
    bool calibrateCorners;
    printf("%d\n", argc);
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (std::string(argv[i]) == "-calibrate") {
                calibrateCorners = true;
                printf("calibrateCorners set to true!\n");
            } else {
                calibrateCorners = false;
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


    // TODO: make calibrate an input argument to the whole program and read the offsets and corner values from a file in the Corners class

    cv::Mat grabbed;
    cv::Mat frame;
    cv::Mat imgHSV;
    cv::Mat imgThresh;
    cv::VideoWriter record;


    // path to video: C:\AirHockeyRobot\cmake-build-debug
    bool RunIdle = false;       //Idle Process to show video.
    if(RunIdle) {
        Idle::Idle_Process();
        std::exit(0);
    }

    time_t start = 0;
    time_t end = 0;
    double frameRate = 0;
    int execs = 0;

    cv::Point_<int> location;        //Added
    cv::Point_<int> lastLocation;    //Added

    cv::Point_<int> vectorXY;


    //printf("this is a test: %d, %d\n", fuck[1].x, fuck[1].y);
    MotorDriver motorDriver = MotorDriver();
    bool blahtest = motorDriver.initComPort('3', 'x');



    Camera camera = Camera(1280,720);
    Table table = Table(camera);
    firstTimestamp = (long)GetTickCount();
    Puck puck = Puck(table);
    Corners corners = Corners(calibrateCorners);
//    readValues(corners);
    std::vector<cv::Point_<int>> fuck;
    fuck = corners.getCalibratedCorners();
    table.setLimits(corners.sortedX, corners.sortedY);
    puck.setWalls(corners.sortedX, corners.sortedY);


    puck.setupTrackbars();

    //HANDLE xMotorCmd = createPipe(0);
    cv::Size blahhhh = {640, 360};


    cv::VideoWriter video("output.avi", CV_FOURCC('M', 'J', 'P', 'G'),10, blahhhh);

    while (true) {
//        testMessage(xMotorCmd);
//        if (execs >= 100) {
//            time(&end);
//            frameRate = 100 / difftime(end, start);
//            time(&start);
//            execs = 0;
//        }

        if (!undistort) {
            grabbed = camera.getFrame();
        } else {
            grabbed = camera.getUndistortedFrame(); // Query a new frame
        }
        if (grabbed.empty()) {
            printf("No frames!\n");
            break;
        }

        frameTimestamp = (long) GetTickCount(); // Get timestamp (not too much resolution)

        if (!calibrateCorners) {
            lastLocation = location;
            puck.findPuck(grabbed, table);

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


/*            if (corners.size() >= 3) {
                //printf("%d: \t %d,%d \t %d,%d \t %d,%d \t %d,%d\n", corners.size(), corners[0].x, corners[0].y, corners[1].x, corners[1].y, corners[2].x, corners[2].y, corners[3].x, corners[3].y);
            }*/
            if (calibrateCorners) {
                corners.calibrateCorners(grabbed, previewSmall, table, puck);
                writeConfigValues(corners);

            }
            //james test start//////////////////////////////
            //puck: pos1 -> pos2,      prediction: pre1 -> pre2
            //bool intersect variable
            //if intersect = true, check predicted points and cap values at edges of table.
            //      >> new predicted points (Capped)
            //
            //Check if X or Y got capped
            //      if X capped:
            //          pre1 = pos2(capped)
            //          pre2 = (pos1.x, - (+/- pos1.y));
            //          draw line (pre1, pre2)
            //              cap prediction line
            //
            //      if Y capped:
            //          pre1 = pos2(capped)
            //          pre2 = ( - (+/- pos1.x), pos1.y);
            //          draw line (pre1, pre2)
            //              cap prediction line


            /*
            //clipline
            //cv::Point pt1 = puck.location;
            //cv::Point pt2 = puck.getVector(grabbed) + puck.location;
            //cv::line(previewSmall, pt1, pt2, cvScalar(0,255,255),4);

            //bool clipped;

            //CvPoint pt2 = cvPoint((int)location.x + (location.x - lastLocation.x)*10, (int)location.y + (location.y - lastLocation.y)*10);
            //clippled = cvClipLine((50,50,300,300), CvPoint &pt1, Point &pt2);
            //take vector
/*
            double tempvx = (location.x - lastLocation.x);
            double tempvy = (location.y - lastLocation.y);
            double tempvx2 = 10*(location.x - lastLocation.x);
            double tempvy2 = 10*(location.y - lastLocation.y);

            if (tempvx != 0 || tempvy != 0) {
                double magv = sqrt(tempvx * tempvx + tempvy * tempvy);

                tempvx = tempvx / magv;
                tempvy = tempvy / magv;

                double tempholdv = tempvx;
                tempvx = -tempvy;
                tempvy = tempholdv;

                double def1x = location.x + tempvx * 10;
                double def1y = location.y + tempvy * 10;

                double def2x = location.x + tempvx * (-10);
                double def2y = location.y + tempvy * (-10);

                cv::line(previewSmall, cvPoint(location.x + tempvx2, location.y + tempvy2), cvPoint(def1x, def1y), cvScalar(0, 0, 255), 4);
                //cv::line(previewSmall, cvPoint(tempvx2, tempvy2), cvPoint(def1x, def1y), cvScalar(0, 0, 255), 4);

               // cv::line(previewSmall, cvPoint(location.x + tempvx2, location.y + tempvy2), cvPoint(def2x, def2y), cvScalar(0, 255, 0), 4);
                //cv::line(previewSmall, cvPoint(tempvx2, tempvy2), cvPoint(def2x, def2y), cvScalar(0, 255, 0), 4);

            }
             */

            //GOAL check
            //find midpoints of Y lines being drawn with drawSquareNew
            //      manually add offset in (while drawing lines to check) to find goal zone.


            //look into cvFitLine

            //j test end//////////////////////////////////////
            //(previewSmall);
//            corners.drawSquare(previewSmall, corners.getCorners(), corners.getOffsets());
            video.write(previewSmall);
            imshow("Video", previewSmall);
        }

        if (cv::waitKey(1) >= 0)
            break;

        execs++;
    }
    cvDestroyAllWindows();
    if (video_output) {
        record.release();
    }

    return 0;
}