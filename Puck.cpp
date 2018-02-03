//
// Created by jad140230 on 1/21/2018.
//

#include "Puck.h"
#include "ThresholdImage.h"
#include "Table.h"

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

void Puck::find(cv::Mat in, Table table) {
    double area = 0;
    double perimeter = 0;
    double roundness = 0;
    int num;




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
                }

                // Draw contour
                if (table.preview == 1)
                    cv::drawContours(in, contours, i, cv::Scalar(255, 0, 0), 1, 8);

                getCoords(table);
                getVector(in);
                lastLocation = location;
                break;
            }
        }
    }

    //puck.speedX = vectorX * 100 / time;  // speed in dm/ms (
    //puck.speedY = vectorY * 100 / time;
}

void Puck::getCoords(Table table) {
    coordX = location.x/2 - table.cam_center_x;
    coordY = location.y/2 - table.cam_center_y;

    lastCoordX = lastLocation.x/2 - table.cam_center_x;
    lastCoordY = lastLocation.y/2 - table.cam_center_y;
}

void Puck::getVector(cv::Mat in) {
    // Calculate speed and angle
    vectorX = (coordX - lastCoordX);
    vectorY = (coordY - lastCoordY);

    if (vectorX != 0 && vectorY != 0) {
        printf("vectorX: %d\n", vectorX);
        printf("vectorY: %d\n", vectorY);
        cv::line(in, cvPoint((int)location.x/2, (int)location.y/2), cvPoint((int)location.x/2 + vectorX*10, (int)location.y/2 + vectorY*10), cvScalar(255, 0, 255), 4);
    } else {
        printf("coordX: %d\n", coordX);
        printf("lastCoordX %d\n", lastCoordX);
        printf("coordY: %d\n", coordY);
        printf("lastCoordY %d\n", lastCoordY);
    }
}

