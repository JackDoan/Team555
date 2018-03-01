#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/core/opengl.hpp>
#include <opencv2/highgui.hpp>
#include <opencv/cv.hpp>
#include <opencv2/imgproc.hpp>
#include <sysinfoapi.h>

#include "Table.h"
#include "Puck.h"
#include "Camera.h"
#include "motors/MotorComm.h"
#include "Corners.h"


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
    bool video_output = false;
    char tempStr[80];
    long frameTimestamp = 0;
    long firstTimestamp = 0;

    bool undistort = true;
    bool calibrateCorners = true;

    cv::Mat grabbed;
    cv::Mat frame;
    cv::Mat imgHSV;
    cv::Mat imgThresh;
    cv::VideoWriter record;

    std::vector<cv::Point_<int>> corners;

    time_t start = 0;
    time_t end = 0;
    double frameRate = 0;
    int execs = 0;

    cv::Point_<int> location;        //Added
    cv::Point_<int> lastLocation;    //Added

    Vector VectorXY;

    Puck puck = Puck();

    Camera camera = Camera(1280,720);
    Table table = Table(camera);
    firstTimestamp = (long)GetTickCount();



    puck.setupTrackbars();

    HANDLE xMotorCmd = createPipe(0);

    while (true) {
        testMessage(xMotorCmd);
        if (execs >= 100) {
            time(&end);
            frameRate = 100 / difftime(end, start);
            time(&start);
            execs = 0;
        }

        if (!undistort) {
            grabbed = camera.getFrame();
        } else {
            grabbed = camera.getUndistortedFrame(); // Query a new frame
        }
        if (grabbed.empty()) {
            printf("No frames!\n");
            break;
        }

        frameTimestamp = (long)GetTickCount(); // Get timestamp (not too much resolution)

        lastLocation = location;
        location = puck.find(grabbed, table);
        //corners = puck.findPucks(grabbed, table);
        //puck.getCoords(table);

        //puck.getVector(grabbed);

        VectorXY = puck.getVector(grabbed, location, lastLocation);

        //printf("\nVectorXY: %f\n", VectorXY);

//        cameraProcess(grabbed, puck, 1000 / table.fps, table); // CAMERA PROCESS (puck coordinates, trajectory...)

        if (table.preview == 1) {
        //if (true) {
            // Put text over image
            sprintf(tempStr, "%f %ld %f %f %f\n", frameRate, frameTimestamp - firstTimestamp, puck.location.x, puck.location.y, puck.speedY);
            cv::putText(grabbed, tempStr, cvPoint(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0));
            //cv::putText(grabbed, table.puckDataString2, cvPoint(170, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0));
            // Draw Table borders

            table.annotate(grabbed);


            cv::Mat previewSmall;
            cv::resize(grabbed, previewSmall, cv::Size(), 0.5, 0.5);
/*            if (corners.size() >= 3) {
                //printf("%d: \t %d,%d \t %d,%d \t %d,%d \t %d,%d\n", corners.size(), corners[0].x, corners[0].y, corners[1].x, corners[1].y, corners[2].x, corners[2].y, corners[3].x, corners[3].y);
            }*/
            if (calibrateCorners) {
                corners = Corners::calibrateCorners(grabbed, table);
               /* if (corners.size() == 4) {
                    corners[3].x -= 40;
                    corners[3].y -= 40;
                    corners[2].x += 40;
                    corners[2].y -= 40;
                    corners[1].x -= 40;
                    corners[1].y += 40;
                    corners[0].x += 40;
                    corners[0].y += 40;
                    cv::line(previewSmall, corners[0] / 2, corners[1] / 2, cv::Scalar(255, 255, 255), 4);
                    cv::line(previewSmall, corners[1] / 2, corners[3] / 2, cv::Scalar(255, 255, 255), 4);
                    cv::line(previewSmall, corners[3] / 2, corners[2] / 2, cv::Scalar(255, 255, 255), 4);
                    cv::line(previewSmall, corners[2] / 2, corners[0] / 2, cv::Scalar(255, 255, 255), 4);
                    cv::putText(previewSmall, "1", corners[0] / 2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                                2.8, cv::Scalar(255, 0, 255), 2, cv::LINE_8, false);
                    cv::putText(previewSmall, "2", corners[1] / 2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                                2.8, cv::Scalar(255, 0, 0), 2, cv::LINE_8, false);
                    cv::putText(previewSmall, "3", corners[2] / 2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                                2.8, cv::Scalar(0, 255, 0), 2, cv::LINE_8, false);
                    cv::putText(previewSmall, "4", corners[3] / 2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                                2.8, cv::Scalar(0, 0, 255), 2, cv::LINE_8, false);
                } else {
                    printf("See %d pucks! Need 4!", corners.size());
                    //cv::putText(previewSmall, "Need 4 pucks in corners", preview, )
                }*/
            }
            //james test start//////////////////////////////
            //rect
            //rectangle(previewSmall, cvPoint(50,50), cvPoint(300,300), cvScalar(255,0,0), 2, 8);
            //clipline
            //CvPoint pt1 = cvPoint((int)lastLocation.x,(int)lastLocation.y);
            //CvPoint pt2 = cvPoint((int)location.x + (location.x - lastLocation.x)*10, (int)location.y + (location.y - lastLocation.y)*10);
            //cvClipLine((50,50,300,300), &pt1, &pt2);
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
            //look into cvFitLine

            //j test end//////////////////////////////////////
*/
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