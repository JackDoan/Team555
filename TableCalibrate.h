//
// Created by jes130330 on 2/25/2018.
//

#ifndef AIRHOCKEYROBOT_TABLECALIBRATE_H
#define AIRHOCKEYROBOT_TABLECALIBRATE_H

#include <opencv2/opencv.hpp>

struct CornerOffset {

    int minX = -100;
    int maxX =  100;
    int minY = -100;
    int maxY =  100;
};

class TableCalibrate {
    struct CornerOffset table_limits;
    cv::Mat result;
    //cv::Mat imgHSV;

public:
    explicit TableCalibrate(CornerOffset CornerLimits);
    ~TableCalibrate();
    cv::Mat get(cv::Mat&);
    void setLimits(struct CornerOffset);
    void setupTrackbars();
};


#endif //TEAM555_TABLECALIBRATE_H
