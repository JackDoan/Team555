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

    bool undistort = false;

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

    while (true) {
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
        corners = puck.findPucks(grabbed, table);
        //puck.getCoords(table);

        //puck.getVector(grabbed);

        VectorXY = puck.getVector(grabbed, location, lastLocation);

        printf("\nVectorXY: %f\n", VectorXY);

//        cameraProcess(grabbed, puck, 1000 / table.fps, table); // CAMERA PROCESS (puck coordinates, trajectory...)

        if (table.preview == 1) {
        //if (true) {
            // Put text over image
            sprintf(tempStr, "%f %ld %f %f %f\n", frameRate, frameTimestamp - firstTimestamp, puck.location.x, puck.location.y, puck.speedY);
            cv::putText(grabbed, tempStr, cvPoint(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0));
            //cv::putText(grabbed, table.puckDataString2, cvPoint(170, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0));
            // Draw Table borders
            if (corners.size() == 4) {
                cv::line(grabbed, corners[0], corners[1], cv::Scalar(0, 255, 0), 4);
                cv::line(grabbed, corners[1], corners[2], cv::Scalar(0, 255, 0), 4);
                cv::line(grabbed, corners[2], corners[3], cv::Scalar(0, 255, 0), 4);
                cv::line(grabbed, corners[3], corners[0], cv::Scalar(0, 255, 0), 4);
            }
            table.annotate(grabbed);


            cv::Mat previewSmall;
            cv::resize(grabbed, previewSmall, cv::Size(), 0.5, 0.5);
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