//
// Created by mdl150330 on 3/14/2018.
//

#include "../inc/Mallet.h"
#include "../inc/Table.h"
#include <vector>
#include <math.h>
#include <thread>


Mallet::Mallet() {
    limits.minH = 63;//0;
    limits.maxH = 109;//0;//23;
    limits.minS = 113;//58;//60;
    limits.maxS = 193;//203;//141;
    limits.minV = 0;//94;//129;
    limits.maxV = 255;//255;
    settingWindowName = const_cast<char *>("Mallet Settings 1");
    previewWindowName = const_cast<char *>("Mallet");
    outlineColor = cv::Scalar(255, 0, 0);

    minArea = 1548;
    maxArea = 4500;
    minRoundness = 800;
    //int minAreaCalib = 800;
    //int maxAreaCalib = 4000;
    //int minRoundnessCalib = 320;
}

// TODO: add logic to mallet.findPuck that uses functions from camera, to adjust exposure and gain when mallet isn't found for a long time
// If speed is measured 'high' and mallet is not found, turn up exposure and gain until it is found again
// then once it is found again return it to its default setting




