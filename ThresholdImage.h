//
// Created by jad140230 on 1/21/2018.
//

#ifndef AIRHOCKEYROBOT_THRESHOLDIMAGE_H
#define AIRHOCKEYROBOT_THRESHOLDIMAGE_H

#include <opencv2/opencv.hpp>

struct threshold_s {
    int minH = 26;
    int maxH = 128;
    int minS = 69;
    int maxS = 138;
    int minV = 59;
    int maxV = 172;

//    int minH = 16;
//    int maxH = 121;
//    int minS = 23;
//    int maxS = 154;
//    int minV = 0;
//    int maxV = 0;
};

class ThresholdImage
{
    struct threshold_s limits;
    cv::Mat result;
    //cv::Mat imgHSV;
public:
    //ThresholdImage();
    explicit ThresholdImage(threshold_s myLimits);
    ~ThresholdImage();
    cv::Mat get(cv::Mat&);
    void setLimits(struct threshold_s);
    void setupTrackbars();
};


#endif //AIRHOCKEYROBOT_THRESHOLDIMAGE_H
