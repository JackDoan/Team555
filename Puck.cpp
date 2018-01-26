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

    cv::createTrackbar(TrackbarName1, name, &minArea, slidermax, NULL);
    cv::createTrackbar(TrackbarName2, name, &maxArea, slidermax, NULL);
    cv::createTrackbar(TrackbarName3, name, &minRoundness, slidermax, NULL);
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
   /* for green
    puckLimits.minH = 26;
    puckLimits.maxH = 128;
    puckLimits.minS = 150;
    puckLimits.maxS = 236;
    puckLimits.minV = 59;
    puckLimits.maxV = 227; */

    //for orange
    puckLimits.minH = 39;
    puckLimits.maxH = 103;
    puckLimits.minS = 90;
    puckLimits.maxS = 149;
    puckLimits.minV = 121;
    puckLimits.maxV = 212;
    thresholdImage = ThresholdImage(puckLimits);
}

Puck::~Puck() {
}

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
    int posX = 0;
    int posY = 0;
    int localLastX = x;
    int localLastY = y;
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
                // We are using 320x240 pix but we are going to output the 640x480 equivalent (*2)
                posX = floor(moment10 * 2 / (double)area + 0.5); // round
                posY = floor(moment01 * 2 / (double)area + 0.5);

                // limit the region of interest to the table
                if ((posX > table.table_pix_maxx * 2) ||
                    (posX < table.table_pix_minx * 2) ||
                    (posY > table.table_pix_maxy * 2) ||
                    (posY < table.table_pix_miny * 2)) {
                    posX = 0;
                    posY = 0;
                    //contours = contours->h_next;
                    continue;  // continue with other contour... (this is outside the table)
                }
                else {
                    x = posX;
                    y = posY;
                    lastX = localLastX;
                    lastY = localLastY;
                }

                // Draw contour
                if (table.preview == 1)
                    cv::drawContours(in, contours, i, cv::Scalar(255, 0, 0), 1, 8);
                if (lastX >= 0 && lastY >= 0) {
                    if (table.preview == 1) { // Draw a line from the previous point to the current point
                        cv::line(in, cvPoint(posX / 2, posY / 2), cvPoint(lastX / 2, lastY / 2), cvScalar(255, 255, 0), 4);
                    }
                }
                lastX = posX;
                lastY = posY;
                break;
            }
        }
    }
}
