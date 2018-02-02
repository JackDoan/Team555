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
    cv::Mat undistortedFrame;
    double src [3][3] = {{6.6679e+002, 0, 6.3720e+002}, {0, 6.6679e+002, 4.7674e+002}, {0, 0, 1}};
    double dst [5] = {-3.3227e-001, 1.3129e-001, 2.3326e-004, 2.0624e-004, -2.7547e-002};
    const cv::Mat& cameraMatrix = cv::Mat(3,3, CV_32F, src);
    const cv::Mat& distCoeffs = cv::Mat(1,5, CV_32F, dst);
    public:
    Camera(int nwidth, int nheight);
    std::vector<int> getCameraCenter();
    ~Camera();
    cv::Mat getFrame();
};


#endif //AIRHOCKEYROBOT_CAMERA_H
