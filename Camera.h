//
// Created by jad140230 on 1/21/2018.
//

#ifndef AIRHOCKEYROBOT_CAMERA_H
#define AIRHOCKEYROBOT_CAMERA_H

//todo fix camera distortion

class Camera {
    // Camera variables
    cv::VideoCapture capture;
    //float cam_pix_to_mm = CAM_PIX_TO_MM; //todo replace this with an expression
    float cam_rotation = 0.0;  //Camera rotation in radians
    // application parameters
    int cameraX = 1280;
    int cameraY = 960;
    int preview = 1;
    cv::Mat currentView;

public:
    Camera(int nwidth, int nheight);
    std::vector<int> getCameraCenter();
    ~Camera();
    cv::Mat getFrame();
};


#endif //AIRHOCKEYROBOT_CAMERA_H
