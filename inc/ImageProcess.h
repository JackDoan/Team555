//
// Created by mdl150330 on 3/15/2018.
//

#ifndef TEAM555_IMAGEPROCESS_H
#define TEAM555_IMAGEPROCESS_H
#include <opencv2/core.hpp>
#include <opencv2/core/opengl.hpp>
#include <opencv2/highgui.hpp>
#include <opencv/cv.hpp>
#include <string>
#include <vector>

#include "Table.h"
#include "Puck.h"
#include "Mallet.h"
#include "Idle.h"
#include "Config.h"
#include "Corners.h"
#include "Camera.h"
#include "Settings.h"



class ImageProcess {
private:
    char tempStr[80];
    long firstTimestamp = 0;
    cv::Mat grabbed;
//    cv::Mat frame;
//    cv::Mat imgHSV;
//    cv::Mat imgThresh;
    time_t start = 0;
    time_t end = 0;
    double sec;
public:
    double frameRate = 0;

    ImageProcess() = default;
    ~ImageProcess() = default;

    void process(Table table, Puck puck, Mallet mallet, Corners corners, Camera& camera, Settings settings);
};


#endif //TEAM555_IMAGEPROCESS_H
