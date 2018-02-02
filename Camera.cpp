//
// Created by jad140230 on 1/21/2018.
//

#include <opencv2/videoio.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/imgproc.hpp>
#include <opencv/cv.hpp>
#include "Camera.h"

Camera::Camera(int nwidth, int nheight) {
    //double src [3][3] = {{1, 0, 1}, {0, 1, 1}, {0, 0, 1}};
    //double src [3][3] = {{6.6679e+002, 0, 6.3720e+002}, {0, 6.6679e+002, 4.7674e+002}, {0, 0, 1}};
    //double src [3][3] = {{6.6679e+002, 0, 6.3720e+002}, {0, 6.6679e+002, 4.7674e+002}, {0, 0, 1}};
    double dest [5] = {-3.3227e-001, 1.3129e-001, 2.3326e-004, 2.0624e-004, -2.7547e-002};
    cameraMatrix = (cv::Mat_<double>(3,3) << 6.6679e+002, 0, 6.3720e+002, 0, 6.6679e+002, 4.7674e+002, 0, 0, 1);//cv::Mat(3,3, CV_64F, src);
    distCoeffs = (cv::Mat_<double>(5,1) << -3.3227e-001, 1.3129e-001, 2.3326e-004, 2.0624e-004, -2.7547e-002);
    //distCoeffs = cv::Mat(5,1, CV_64F, dest);

    dimensions = Coordinate(nwidth,nheight);
    capture.open(0);
    if (!capture.isOpened()) {
        printf("OPENCV Capture failure!\n");
        exit(-123);
    }


    capture.set(CV_CAP_PROP_FRAME_WIDTH, nwidth);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, nheight);
    //capture.set(CV_CAP_PROP_FPS, table.fps);
    //capture.set(CV_CAP_PROP_EXPOSURE, 2);

}


Camera::~Camera() {
    capture.release();
}

Coordinate Camera::getCenter() {
    return dimensions;
}

cv::Mat Camera::getFrame() {
    capture.read(currentView);
    //undistortedFrame = currentView.clone();
    auto temp = currentView.clone();
    //bitwise_not(currentView, currentView);
   /* cv::Mat newCameraMatrix, map1, map2;
    cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(),
    newCameraMatrix, cv::Size(1280,720), CV_32FC1,map1, map2);
    remap(currentView, temp, map1, map2, cv::INTER_LINEAR);*/
    cv::undistort(currentView, temp, cameraMatrix, distCoeffs);
    //cv::undistort(currentView, undistortedFrame, cameraMatrix, distCoeffs);
    //cv::imshow("in", currentView);
    //cv::imshow("out", temp);
    return temp;
    //return undistortedFrame;
}