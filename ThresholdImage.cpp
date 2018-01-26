//
// Created by jad140230 on 1/21/2018.
//

#include "ThresholdImage.h"

ThresholdImage::ThresholdImage(struct threshold_s myLimits) {
    limits = myLimits;
    //imgThresh = cvCreateImage(cvGetSize(inFrame), IPL_DEPTH_8U, 1);
    //imgHSV = cvCreateImage(cvGetSize(inFrame), IPL_DEPTH_8U, 3);
}

ThresholdImage::~ThresholdImage() {
}

cv::Mat ThresholdImage::get(cv::Mat& in) {
    cv::Mat blurred,imgHSV;
    cv::GaussianBlur(in, blurred, cv::Size(3, 3), 0, 0); //smooth the original image using Gaussian kernel
    cv::cvtColor(blurred, imgHSV, CV_BGR2HSV); //Change the color format from BGR to HSV
    cv::inRange(in,
                cv::Scalar(limits.minH, limits.minS, limits.minV),
                cv::Scalar(limits.maxH, limits.maxS, limits.maxV),
                result);
    return result;
}

void ThresholdImage::setLimits(struct threshold_s newLimits) {
    limits = newLimits;
}

void ThresholdImage::setupTrackbars() {
    char name[] = "Puck Detect Settings";
    char TrackbarName1[50];
    char TrackbarName2[50];
    char TrackbarName3[50];
    char TrackbarName4[50];
    char TrackbarName5[50];
    char TrackbarName6[50];
    int slidermax = 255;
    sprintf(TrackbarName1, "MinH %d", slidermax);
    sprintf(TrackbarName2, "MaxH %d", slidermax);
    sprintf(TrackbarName3, "MinS %d", slidermax);
    sprintf(TrackbarName4, "MaxS %d", slidermax);
    sprintf(TrackbarName5, "MinV %d", slidermax);
    sprintf(TrackbarName6, "MaxV %d", slidermax);
    cv::namedWindow(name, 1);

    cv::createTrackbar(TrackbarName1, name, &(limits.minH), slidermax, NULL);
    cv::createTrackbar(TrackbarName2, name, &(limits.maxH), slidermax, NULL);
    cv::createTrackbar(TrackbarName3, name, &(limits.minS), slidermax, NULL);
    cv::createTrackbar(TrackbarName4, name, &(limits.maxS), slidermax, NULL);
    cv::createTrackbar(TrackbarName5, name, &(limits.minV), slidermax, NULL);
    cv::createTrackbar(TrackbarName6, name, &(limits.maxV), slidermax, NULL);
}
