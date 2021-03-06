//
// Created by jad140230 on 1/21/2018.
//

#ifndef AIRHOCKEYROBOT_CAMERA_H
#define AIRHOCKEYROBOT_CAMERA_H


#include <opencv2/videoio.hpp>

#include "helpers.h"

//this is a Singleton
//https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
class Camera {
    private:
        // Camera variables
        cv::VideoCapture capture;
        //float cam_pix_to_mm = CAM_PIX_TO_MM; //todo replace this with an expression
        // application parameters
        Camera() : Camera(1280,720) {}
        Camera(int nwidth, int nheight);
        int preview = 1;
        cv::Mat currentView;
        cv::Mat undistortedFrame;
        cv::Mat newCameraMatrix, map1, map2;
        cv::Mat cameraMatrix;
        cv::Mat distCoeffs;


    cv::Mat hsv;
    public:
        static Camera& getInstance() {
            static Camera instance;
            return instance;
        }
        cv::Point_<int> dimensions;
        Camera(Camera const&)               = delete;
        void operator=(Camera const&)  = delete;

        ~Camera();
        void close();
        double getFrameRate();


        cv::Mat getFrame();
        //double calcFPS(int counter, bool active);
        cv::Mat getHomography(std::vector<cv::Point_<int>> corners,
                              std::vector<cv::Point_<int>> Calibrated_corners);
};


#endif //AIRHOCKEYROBOT_CAMERA_H
