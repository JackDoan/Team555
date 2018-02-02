//
// Created by jad140230 on 1/21/2018.
//

#ifndef AIRHOCKEYROBOT_CAMERA_H
#define AIRHOCKEYROBOT_CAMERA_H


#include <opencv2/videoio.hpp>
#include "helpers.h"

class Camera {
    // Camera variables
    cv::VideoCapture capture;
    //float cam_pix_to_mm = CAM_PIX_TO_MM; //todo replace this with an expression
    // application parameters

    Coordinate dimensions;

    int preview = 1;
    cv::Mat currentView;

public:
    Camera(int nwidth, int nheight);
    Coordinate getCenter();
    ~Camera();
    cv::Mat getFrame();
};


#endif //AIRHOCKEYROBOT_CAMERA_H
