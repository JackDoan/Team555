#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/core/opengl.hpp>
#include <opencv2/highgui.hpp>
#include <opencv/cv.hpp>
#include <sysinfoapi.h>

#include "Table.h"
#include "Puck.h"
#include "Camera.h"

//BUILDING: do this, steps 1-9: https://stackoverflow.com/questions/35984678/use-opencv-with-clion-ide-on-windows
//For step 3, use C:\opencv\mingw instead of C:\opencv\mingw-build


void cameraProcessInit(Table& table) {
    // Default values
    table.cam_center_x = CAM_PIX_WIDTH;
    table.cam_center_y = CAM_PIX_HEIGHT;
    table.predict_x_old = -1;

    table.robot_table_center_y = table.robot_table_width / 2;
    table.robot_table_center_x = table.robot_table_length / 2;
    table.puckSize = table.robot_table_width / 20;  // puck size (radio) estimation
    table.defense_position = 60 + table.puckSize;  // Pusher defense position (for predictions)

    table.table_pix_maxx = 0.85*1280;//(int)((table.robot_table_center_y) / table.cam_pix_to_mm + table.cam_center_y) / 2;
    table.table_pix_maxy = 0.85*720;//(int)((table.robot_table_center_x) / table.cam_pix_to_mm + table.cam_center_x) / 2;
    table.table_pix_minx = 0.15*1280;//(int)((table.robot_table_center_y - table.robot_table_length) / table.cam_pix_to_mm + table.cam_center_y) / 2;
    table.table_pix_miny = 0.15*720;//(int)((table.robot_table_center_x - table.robot_table_width) / table.cam_pix_to_mm + table.cam_center_x) / 2;

    //printf("Table %d,%d %d,%d\n",table.table_pix_minx,table.table_pix_miny,table.table_pix_maxx,table.table_pix_maxy);
}

// Draw the table marks
void drawTable(cv::Mat& frameGrabbed, Table table) {
    // Draw marks on image and center

    cv::Scalar color = cv::Scalar(100, 255, 200);

    cv::line(frameGrabbed, cvPoint(CAM_PIX_WIDTH / 4 - 5, CAM_PIX_HEIGHT / 4), cvPoint(CAM_PIX_WIDTH / 4 + 5, CAM_PIX_HEIGHT / 4), color, 1);
    cv::line(frameGrabbed, cvPoint(CAM_PIX_WIDTH / 4, CAM_PIX_HEIGHT / 4 - 5), cvPoint(CAM_PIX_WIDTH / 4, CAM_PIX_HEIGHT / 4 + 5), color, 1);
    //Draw table limits
    //cv::line(frameGrabbed, cvPoint(table_pixx1,table_pixy1), cvPoint(table_pixx2,table_pixy2), cvScalar(100,255,200), 1);
    //cv::line(frameGrabbed, cvPoint(table_pixx2,table_pixy2), cvPoint(table_pixx3,table_pixy3), cvScalar(100,255,200), 1);
    //cv::line(frameGrabbed, cvPoint(table_pixx3,table_pixy3), cvPoint(table_pixx4,table_pixy4), cvScalar(100,255,200), 1);
    //cv::line(frameGrabbed, cvPoint(table_pixx4,table_pixy4), cvPoint(table_pixx1,table_pixy1), cvScalar(100,255,200), 1);

    cv::line(frameGrabbed, cvPoint(table.table_pix_minx, table.table_pix_miny), cvPoint(table.table_pix_minx + 20, table.table_pix_miny), color, 1);
    cv::line(frameGrabbed, cvPoint(table.table_pix_minx, table.table_pix_maxy), cvPoint(table.table_pix_minx + 20, table.table_pix_maxy), color, 1);
    cv::line(frameGrabbed, cvPoint(table.table_pix_maxx - 20, table.table_pix_miny), cvPoint(table.table_pix_maxx, table.table_pix_miny), color, 1);
    cv::line(frameGrabbed, cvPoint(table.table_pix_maxx - 20, table.table_pix_maxy), cvPoint(table.table_pix_maxx, table.table_pix_maxy), color, 1);
    cv::line(frameGrabbed, cvPoint(table.table_pix_minx, table.table_pix_miny), cvPoint(table.table_pix_minx, table.table_pix_miny + 20), color, 1);
    cv::line(frameGrabbed, cvPoint(table.table_pix_maxx, table.table_pix_miny), cvPoint(table.table_pix_maxx, table.table_pix_miny + 20), color, 1);
    cv::line(frameGrabbed, cvPoint(table.table_pix_minx, table.table_pix_maxy - 20), cvPoint(table.table_pix_minx, table.table_pix_maxy), color, 1);
    cv::line(frameGrabbed, cvPoint(table.table_pix_maxx, table.table_pix_maxy - 20), cvPoint(table.table_pix_maxx, table.table_pix_maxy), color, 1);
}

// Camera process, convert puck position to coordinates and generate trajectory prediction and visualization
// Simple lens distortion correction (one parameter) NOT USED NOW
// Xu = (Xd)/(1+param*dist2)  dist2 = distancia al cuadrado del pixel al centro
void cameraProcess(cv::Mat& frameGrabbed, Puck puck, int time, Table table) {
    int coordX;
    int coordY;
    int vectorX;
    int vectorY;
    double slope;

    int bounce_x;
    int bounce_y;
    int predict_pixX;
    int predict_pixY;
    int bounce_pixX;
    int bounce_pixY;


    // First we convert image coordinates to center of image
    //todo establish order of operations here
    coordX = table.robot_table_center_x - (puck.x - table.cam_center_x) * table.cam_pix_to_mm;
    coordY = table.robot_table_center_y - (puck.y - table.cam_center_y) * table.cam_pix_to_mm;

    // Calculate speed and angle
    vectorX = (coordX - puck.x);
    vectorY = (coordY - puck.y);

    puck.speedX = vectorX * 100 / time;  // speed in dm/ms (
    puck.speedY = vectorY * 100 / time;
    //puckSpeed = sqrt(vectorX*vectorX + vectorY*vectorY)*1000.0/time;
    //puckDirection = atan2(vectorY,vectorX);

    // Noise detection, big vector...
   /* if ((vectorY<-250) || (vectorY>250) || (vectorX>250) || (vectorX<-250)) {
        //sprintf(puckDataString2, "N %d", vectorY);
        return;
    } */

    // Its time to predict...
    // Based on actual position, speed and angle we need to know the future...
    // Possible impact?
    if ((puck.speedY<-35)) {
        // Puck is coming...
        // We need to predict the puck position when it reaches our goal Y=0
        // slope formula: m = (y2-y1)/(x2-x1)
        if (vectorX == 0)  // To avoid division by 0
            slope = 9999999;
        else
            slope = (float)vectorY / (float)vectorX;
        // x = (y2-y1)/m + x1
        table.predict_y = table.defense_position;
        table.predict_x = (table.predict_y - coordY) / slope + coordX;

        // puck has a bounce with side wall?
        if (( table.predict_y< table.puckSize) || ( table.predict_y > (table.robot_table_width - table.puckSize))) {
            // We start a new prediction
            // Which side?
            if (table.predict_y<table.puckSize) {
                bounce_x = table.puckSize; //Left side. We calculate the impact point
            }
            else {
                bounce_x = (table.robot_table_length - table.puckSize); //Right side. We calculate the impact point
            }
            bounce_y = (bounce_x - coordX)*slope + coordY;
            bounce_pixX = table.cam_center_x - (bounce_x - table.robot_table_center_x) / table.cam_pix_to_mm;
            bounce_pixY = table.cam_center_y - (bounce_y - table.robot_table_center_y) / table.cam_pix_to_mm;
            table.predict_time = (bounce_x - puck.x) * 100 / puck.speedX;  // time until bouce
            // bounce prediction
            // Slope change
            slope = -slope;
            table.predict_y = table.defense_position;
            table.predict_x = (table.predict_y - bounce_y) / slope + bounce_x;

            if ((table.predict_y<table.puckSize) || (table.predict_y>(table.robot_table_width - table.puckSize))) {
                // New bounce??
                // We do nothing then...
                //sprintf(puckDataString2, "2B %d %d", bounce_x, bounce_y);
                table.predict_x_old = -1;
            }
            else {
                // draw line
                if (table.preview == 1)
                    cv::line(frameGrabbed, cvPoint(coordX / 2, coordY / 2), cvPoint(bounce_pixX / 2, bounce_pixY / 2), cvScalar(255, 0, 0), 2);
                // result average
                if (table.predict_x_old != -1)
                    table.predict_x = (table.predict_x_old + table.predict_x) >> 1;
                table.predict_x_old = table.predict_x;
                table.predict_time = table.predict_time + (table.predict_y - puck.y) * 100 / puck.speedY;  // in ms
                //sprintf(puckDataString2, "%d t%d", table.predict_x, table.predict_time);
                predict_pixX = table.cam_center_x - (table.predict_x - table.robot_table_center_x) / table.cam_pix_to_mm;
                predict_pixY = table.cam_center_y - (table.predict_y - table.robot_table_center_y) / table.cam_pix_to_mm;
                // draw line
                if (table.preview == 1) {
                    cv::line(frameGrabbed, cvPoint(bounce_pixX / 2, bounce_pixY / 2), cvPoint(predict_pixX / 2, predict_pixY / 2), cvScalar(0, 255, 0), 2);
                }

            }
        }
        else  // No bounce, direct impact
        {
            // result average
            if (table.predict_x_old != -1)
                table.predict_x = (table.predict_x_old + table.predict_x) >> 1;
            table.predict_x_old = table.predict_x;

            table.predict_time = (table.predict_y - puck.y) * 100 / puck.speedY;  // in ms
            //sprintf(puckDataString2, "%d t%d", predict_x, predict_time);
            // Convert impact prediction position to pixels (to show on image)
            predict_pixX = table.cam_center_x - (table.predict_x - table.robot_table_center_x) / table.cam_pix_to_mm;
            predict_pixY = table.cam_center_y - (table.predict_y - table.robot_table_center_y) / table.cam_pix_to_mm;
            // draw line
            if (table.preview == 1)
                cv::line(frameGrabbed, cvPoint(coordX / 2, coordY / 2), cvPoint(predict_pixX / 2, predict_pixY / 2), cvScalar(0, 255, 0), 2);
        }
    }
    else // Puck is moving slowly or to the other side
    {
        //printf(puckDataString2, "", coordX, coordY, puckSpeedY);
        table.predict_x_old = -1;
    }

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


//todo convert all x/y vars into std::vectors

int main(int argc, char* argv[]) {
    bool video_output = 1;
    char tempStr[80];
    long frameTimestamp = 0;
    long firstTimestamp = 0;
    Table table;
    cv::Mat grabbed;
    cv::Mat frame;
    cv::Mat imgHSV;
    cv::Mat imgThresh;
    cv::VideoWriter record;
    FILE* logFile;
    int log_output = 1;
    time_t start = 0;
    time_t end = 0;
    double frameRate = 0;
    int execs = 0;

    Puck puck = Puck();

    Camera camera = Camera(1280,720);
    cameraProcessInit(table);
    firstTimestamp = (long)GetTickCount();


    puck.setupTrackbars();

    while (true) {
        if (execs >= 100) {
            time(&end);
            frameRate = 100 / difftime(end, start);
            time(&start);
            execs = 0;
        }

        grabbed = camera.getFrame(); // Query a new frame
        if (grabbed.empty()) {
            printf("No frames!\n");
            break;
        }

        frameTimestamp = (long)GetTickCount(); // Get timestamp (not too much resolution)

        puck.find(grabbed, table);
        cameraProcess(grabbed, puck, 1000 / table.fps, table); // CAMERA PROCESS (puck coordinates, trajectory...)

        if (table.preview == 1) {
            // Put text over image
            sprintf(tempStr, "%f %ld %d,%d %d\n", frameRate, frameTimestamp - firstTimestamp, puck.x, puck.y, puck.speedY);
            cv::putText(grabbed, tempStr, cvPoint(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0));
            //cv::putText(grabbed, table.puckDataString2, cvPoint(170, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0));
            // Draw Table for reference
            drawTable(grabbed, table);
        }

        if (table.preview > 0) {
            cv::Mat previewSmall;
            cv::resize(grabbed,previewSmall, cv::Size(), 0.5, 0.5);
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
    if (log_output) {
        fclose(logFile);
    }

    return 0;
}