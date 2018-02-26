//
// Created by jes130330 on 2/25/2018.
//

#include "TableCalibrate.h"


TableCalibrate::TableCalibrate(struct CornerOffset CornerLimits){
    table_limits = CornerLimits;
};

TableCalibrate::~TableCalibrate() {
}

cv::Mat TableCalibrate::get(cv::Mat& in) {
   // cv::Mat blurred,imgHSV;
    //cv::GaussianBlur(in, blurred, cv::Size(3, 3), 0, 0); //smooth the original image using Gaussian kernel
    //cv::cvtColor(blurred, imgHSV, CV_BGR2HSV); //Change the color format from BGR to HSV
    cv::inRange(in,
                cv::Scalar(table_limits.minX, table_limits.minY),
                cv::Scalar(table_limits.maxX, table_limits.maxY),
                result);
    return result;
}

//void TableCalibrate::setLimits(struct CornerOffset newLimits) {
//    limits = newLimits;
//}


void TableCalibrate::setupTrackbars() {
    char Corners[] = "Puck Detect Settings";
    char TrackbarNameA[50];
    char TrackbarNameB[50];
    char TrackbarNameC[50];
    char TrackbarNameD[50];
    int slidermax = 255;
    sprintf(TrackbarNameA, "MinX %d", slidermax); //change to L,R,Up,Down
    sprintf(TrackbarNameB, "MaxX %d", slidermax);
    sprintf(TrackbarNameC, "MinY %d", slidermax);
    sprintf(TrackbarNameD, "MaxY %d", slidermax);
    cv::namedWindow(Corners, 1);

    cv::createTrackbar(TrackbarNameA, Corners, &(table_limits.minX), slidermax, NULL);
    cv::createTrackbar(TrackbarNameB, Corners, &(table_limits.maxX), slidermax, NULL);
    cv::createTrackbar(TrackbarNameC, Corners, &(table_limits.minY), slidermax, NULL);
    cv::createTrackbar(TrackbarNameD, Corners, &(table_limits.maxY), slidermax, NULL);
};
