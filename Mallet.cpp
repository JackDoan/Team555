//
// Created by mdl150330 on 3/14/2018.
//

#include "Mallet.h"
#include "ThresholdImage.h"
#include "Table.h"
#include <vector>
#include <math.h>
#include <thread>


Mallet::Mallet(Table table) {
    struct threshold_s malletLimits;
    malletLimits.minH = 0;
    malletLimits.maxH = 23;
    malletLimits.minS = 60;
    malletLimits.maxS = 141;
    malletLimits.minV = 129;
    malletLimits.maxV = 255;

    thresholdImage = ThresholdImage(malletLimits);
    location = {0,0};
    lastLocation = {0,0};
    vectorXY = {0,0};
    predictedLocation = {0,0};
    bouncex = false;
    bouncey = false;
    malletFound = false;
    lostCnt = 0;
}
Mallet::~Mallet() = default;

void Mallet::setupTrackbars() {
    char TrackbarName1[50];
    char TrackbarName2[50];
    char TrackbarName3[50];

    char name[] = "Mallet Settings";
    int slidermax = 5000;
    sprintf(TrackbarName1, "MinArea %d", slidermax);
    sprintf(TrackbarName2, "MaxArea %d", slidermax);
    sprintf(TrackbarName3, "MinRound %d", slidermax);

    cv::namedWindow(name, 1);
    char name1[] = "Mallet Color Settings";
    cv::createTrackbar(TrackbarName1, name, &minArea, slidermax, nullptr);
    cv::createTrackbar(TrackbarName2, name, &maxArea, slidermax, nullptr);
    cv::createTrackbar(TrackbarName3, name, &minRoundness, slidermax, nullptr);
    thresholdImage.setupTrackbars(name1);
    //TableCalibrate.setupTrackbars();
}

// TODO: add logic to mallet.findPuck that uses functions from camera, to adjust exposure and gain when mallet isn't found for a long time
// If speed is measured 'high' and mallet is not found, turn up exposure and gain until it is found again
// then once it is found again return it to its default setting

double Mallet::getMinArea()
{
    return (double)minArea;
}

double Mallet::getMaxArea()
{
    return (double)maxArea;
}

double Mallet::getMinRoundness()
{
    return (double)(minRoundness);
}

void Mallet::findMallet(cv::Mat in, Table table) {
    double area = 0;
    double perimeter = 0;
    double roundness = 0;
    int num;

//    cv::Point_<int> malletPoint;


    cv::Mat imgThresh = thresholdImage.get(in);
    cv::Mat imgThreshSmall;
    cv::resize(imgThresh,imgThreshSmall, cv::Size(), 0.25, 0.25);
    imshow("Mallet", imgThreshSmall);
    std::vector< std::vector<cv::Point> > contours;  //hold the pointer to a contour in the memory block
    CvSeq* result;   //hold sequence of points of a contour
    //CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours

    //cv::Mat imgCorners = TableCalibrate.get(in);
    //mshow("Corners", imgCorners);

    // Position initialization
    //double posX = 0;
    //double posY = 0;
//    Coordinate pos = Coordinate(0.0,0);
    cv::Point_<int> pos = {0,0};
    Coordinate lastPos = Coordinate(0,0);
    //double localLastX = x;
    //double localLastY = y;
    num = 0;

    cv::InputArray thisone = imgThresh;
    cv::findContours(imgThresh, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    malletFound = false;
    lastLocation = location;
    for (int i = 0; i< contours.size(); i++) {
        //todo: shrink the image to the table area, THEN search for contours.
        num++;
        area = cv::contourArea(contours[i]);
        //printf("Mallet: A:%.2f R:%.2f\r\n", area, roundness);
        //sprintf(logStr,"%s;%d %.2f", logStr,num, area);
        if ((area > getMinArea()) && (area < getMaxArea())) {  // Min and Max size of object
            //Detecting roundness   roundness = perimeter^2 / (2*pi*area)
            perimeter = cv::arcLength(contours[i], true);
            roundness = (perimeter * perimeter) / (6.28 * area);
            if (roundness < getMinRoundness()) {
                cv::Moments moments = cv::moments(contours[i], true);
                double moment10 = moments.m10;
                double moment01 = moments.m01;
                area = moments.m00;
                // Calculate object center
                pos.x = moment10 * 2 / area;
                pos.y = moment01 * 2 / area;

                //pos.x = floor(moments.m10 * 2 / area + 0.5); // round
                //pos.y = floor(moments.m01 * 2 / area + 0.5);

                // limit the region of interest to the table
                if ((pos.x > table.max.x * 2) ||
                    (pos.x < table.min.x * 2) ||
                    (pos.y > table.max.y * 2) ||
                    (pos.y < table.min.y * 2)) {
                    pos.x = 0;
                    pos.y = 0;
                    //contours = contours->h_next;
                    continue;  // continue with other contour... (this is outside the table)
                } else {
                    location = pos/2;
//                    malletPoint.x = (int) round(pos.x);
//                    malletPoint.y = (int) round(pos.y);
//                }
//                    printf("malletPoint: %d, %d\n", (int) round(pos.x), (int) round(pos.y));
//                    printf("location: %d, %d\n", location.x, location.y);


                    // Draw contour
                    if (table.preview == 1)
                        cv::drawContours(in, contours, i, cv::Scalar(255, 0, 0), 5, 4);
                    //lastLocation = location;
                    //getCoords(table);
                    //getVector(in);
                    malletFound = true;
                    break;
                }

            }
        }
    }
    //mallet.speedX = vectorX * 100 / time;  // speed in dm/ms (
    //mallet.speedY = vectorY * 100 / time;

    //CoordsDouble = Coordinate(location.x/2 - table.cam_center_x, location.y/2 - table.cam_center_y);
    //CoordsDouble = Coordinate(location.x/2, location.y/2);

//    if (!malletFound) {
//        lostCnt++;
//        if (lostCnt < 5) {
//            lastLocation = location;
//            location = location + vectorXY;
//        }
//
//    }

    // TODO: need to make two separate paths in calcVector, calcTraj and drawVector
    // One for when the mallet is found and when the mallet is not found
    // when mallet is found use location and last location
    // when mallet is not found use estimated location and estimated last location
    // also need to make a counter for how many frames the mallet is not found
    // if it goes more than 5 frames without finding the mallet then stop calculating
    // and drawing the trajectories and vectors

//    if (!(malletFound && lostCnt > 0)) {
////        calcVector(in);
//        calcTraj(table);
//        drawVector(in);
//    } else {
//        lostCnt = 0;
//    }
}



