//
// Created by jad140230 on 1/21/2018.
//

#include "Puck.h"
#include "ThresholdImage.h"
#include "TableCalibrate.h
#include "Table.h"
#include "helpers.h"
#include <vector>
#include <math.h>


void Puck::setupTrackbars() {
    char TrackbarName1[50];
    char TrackbarName2[50];
    char TrackbarName3[50];

    char name[] = "Puck Settings";
    int slidermax = 5000;
    sprintf(TrackbarName1, "MinArea %d", slidermax);
    sprintf(TrackbarName2, "MaxArea %d", slidermax);
    sprintf(TrackbarName3, "MinRound %d", slidermax);

    cv::namedWindow(name, 1);

    cv::createTrackbar(TrackbarName1, name, &minArea, slidermax, nullptr);
    cv::createTrackbar(TrackbarName2, name, &maxArea, slidermax, nullptr);
    cv::createTrackbar(TrackbarName3, name, &minRoundness, slidermax, nullptr);
    thresholdImage.setupTrackbars();
    //TableCalibrate.setupTrackbars();
}

double Puck::getMinArea()
{
    return (double)minArea;
}

double Puck::getMaxArea()
{
    return (double)maxArea;
}

double Puck::getMinRoundness()
{
    return (double)(minRoundness);
}

Puck::Puck() {
    struct threshold_s puckLimits;
    //for green

    puckLimits.minH = 43;
    puckLimits.maxH = 110;
    puckLimits.minS = 70;
    puckLimits.maxS = 155;
    puckLimits.minV = 20;
    puckLimits.maxV = 67;




    //for orange
    /*
    puckLimits.minH = 39;
    puckLimits.maxH = 103;
    puckLimits.minS = 90;
    puckLimits.maxS = 149;
    puckLimits.minV = 121;
    puckLimits.maxV = 212;
     */

    thresholdImage = ThresholdImage(puckLimits);
}

Puck::~Puck() = default;

std::vector<cv::Point_<int>> Puck::findPucks(cv::Mat in, Table table) {
    double area = 0;
    double perimeter = 0;
    double roundness = 0;
    int num;
    std::vector<cv::Point_<int>> pointVec;
//    pointVec.reserve(4);


    cv::Mat imgThresh = thresholdImage.get(in);
    cv::Mat imgThreshSmall;
    cv::resize(imgThresh,imgThreshSmall, cv::Size(), 0.25, 0.25);
    imshow("Puck", imgThreshSmall);
    std::vector< std::vector<cv::Point> > contours;  //hold the pointer to a contour in the memory block
    CvSeq* result;   //hold sequence of points of a contour
    //CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours

    // Position initialization
    //double posX = 0;
    //double posY = 0;
    Coordinate pos = Coordinate(0.0,0);
    //double localLastX = x;
    //double localLastY = y;
    num = 0;

    cv::InputArray thisone = imgThresh;
    cv::findContours(imgThresh, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    for (int i = 0; i< contours.size(); i++) {
        //todo: shrink the image to the table area, THEN search for contours.
        num++;
        area = cv::contourArea(contours[i]);
        //printf("%s %.2f %.2f", logStr, area, roundness);
        //sprintf(logStr,"%s;%d %.2f", logStr,num, area);
        if ((area>getMinArea()) && (area<getMaxArea())) {  // Min and Max size of object
            //Detecting roundness   roundness = perimeter^2 / (2*pi*area)
            perimeter = cv::arcLength(contours[i], true);
            roundness = (perimeter*perimeter) / (6.28*area);
            if (roundness < getMinRoundness()) {
                cv::Moments moments = cv::moments(contours[i], true);
                double moment10 = moments.m10;
                double moment01 = moments.m01;
                area = moments.m00;
                // Calculate object center
                pos.x = moment10*2 / area;
                pos.y = moment01*2 / area;

                //pos.x = floor(moments.m10 * 2 / area + 0.5); // round
                //pos.y = floor(moments.m01 * 2 / area + 0.5);

                // limit the region of interest to the table
//                if ((pos.x > table.max.x * 2) ||
//                    (pos.x < table.min.x * 2) ||
//                    (pos.y > table.max.y * 2) ||
//                    (pos.y < table.min.y * 2)) {
//                    pos.x = 0;
//                    pos.y = 0;
//                    //contours = contours->h_next;
//                    continue;  // continue with other contour... (this is outside the table)
//                }
//                else {
                    //location = pos;

//                if (pointVec.size() > 0) {
//                    printf("%15d, %15d,\t", (int)round(pos.x/2), (int)round(pos.y/2));
//                }
                pointVec.emplace_back((int)round(pos.x/2), (int)round(pos.y/2));
/*                for (int j = 0; j < pointVec.size(); j++) {
                    if (pointVec.size() == 4)
                        printf("Number: %d: x: %15d,\ty: %15d\n",j , pointVec[j].x, pointVec[j].y);
                }*/
//                if (pointVec.size() > 0) {
//                    printf("%15d, %15d,\t %15d\n", pointVec[pointVec.size()].x, pointVec[pointVec.size()].y, i);
//                }
//                }

                // Draw contour
                if (table.preview == 1) {
                    cv::drawContours(in, contours, i, cv::Scalar(0, 255, 0), 5, 8);
                }
            }
        }
    }

    return pointVec;
}

//void Puck::find(cv::Mat in, Table table) {
cv::Point_<int> Puck::find(cv::Mat in, Table table) {
    double area = 0;
    double perimeter = 0;
    double roundness = 0;
    int num;

    cv::Point_<int> puckPoint;


    cv::Mat imgThresh = thresholdImage.get(in);
    cv::Mat imgThreshSmall;
    cv::resize(imgThresh,imgThreshSmall, cv::Size(), 0.25, 0.25);
    imshow("Puck", imgThreshSmall);
    std::vector< std::vector<cv::Point> > contours;  //hold the pointer to a contour in the memory block
    CvSeq* result;   //hold sequence of points of a contour
    //CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours

    //cv::Mat imgCorners = TableCalibrate.get(in);
    //mshow("Corners", imgCorners);

    // Position initialization
    //double posX = 0;
    //double posY = 0;
    Coordinate pos = Coordinate(0.0,0);
    Coordinate lastPos = Coordinate(0,0);
    //double localLastX = x;
    //double localLastY = y;
    num = 0;

    cv::InputArray thisone = imgThresh;
    cv::findContours(imgThresh, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    for (int i = 0; i< contours.size(); i++) {
        //todo: shrink the image to the table area, THEN search for contours.
        num++;
        area = cv::contourArea(contours[i]);
        //printf("%s %.2f %.2f", logStr, area, roundness);
        //sprintf(logStr,"%s;%d %.2f", logStr,num, area);
        if ((area>getMinArea()) && (area<getMaxArea())) {  // Min and Max size of object
            //Detecting roundness   roundness = perimeter^2 / (2*pi*area)
            perimeter = cv::arcLength(contours[i], true);
            roundness = (perimeter*perimeter) / (6.28*area);
            if (roundness < getMinRoundness()) {
                cv::Moments moments = cv::moments(contours[i], true);
                double moment10 = moments.m10;
                double moment01 = moments.m01;
                area = moments.m00;
                // Calculate object center
                pos.x = moment10*2 / area;
                pos.y = moment01*2 / area;

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
                }
                else {
                    location = pos;
                    puckPoint.x = (int)round(pos.x);
                    puckPoint.y = (int)round(pos.y);
                }



                // Draw contour
                if (table.preview == 1)
                    cv::drawContours(in, contours, i, cv::Scalar(0, 255, 0), 5, 8);

                //getCoords(table);
                //getVector(in);
                lastLocation = location;
                break;
            }
        }
    }

    //puck.speedX = vectorX * 100 / time;  // speed in dm/ms (
    //puck.speedY = vectorY * 100 / time;

    //CoordsDouble = Coordinate(location.x/2 - table.cam_center_x, location.y/2 - table.cam_center_y);
    //CoordsDouble = Coordinate(location.x/2, location.y/2);

return puckPoint;
}

Vector Puck::getVector(cv::Mat in, cv::Point_<int> location, cv::Point_<int> lastLocation) {
   // Calculate speed and angle
    Vector VectorXY;
    vectorX = (location.x - lastLocation.x);
    vectorY = (location.y - lastLocation.y);


//test
        if (location.x >= 1.001 * lastLocation.x || location.x <= 0.999 * lastLocation.x) {
            //printf("coordx,y: %f, %f\t\t lastcoordx,y: %f, %f\n", location.x, location.y, lastLocation.x, lastLocation.y);
            //printf("vectorx,y: %f, %f\n", vectorX, vectorY);
        }
//   if (vectorX != 0 && vectorY != 0)
   if (vectorX != 0 || vectorY != 0)
  {
       //printf("vectorX: %d\n", vectorX);
       //printf("vectorY: %d\n", vectorY);
        cv::line(in, cvPoint(lastLocation.x, lastLocation.y), cvPoint(location.x + vectorX*10, location.y + vectorY*10), cvScalar(255, 0, 255), 4);



  } else {
//        printf("coord: %f\n", location.x);
//        printf("lastCoord %f\n", lastLocation.x);
        //printf("coordY: %f\n", location.y);
        //printf("lastCoordY %f\n", lastLocation.y);
    }
    VectorXY = Vector(vectorX, vectorY);
    return VectorXY;
}


