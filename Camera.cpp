//
// Created by jad140230 on 1/21/2018.
//

#include <opencv2/videoio.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/imgproc.hpp>
#include <opencv/cv.hpp>
#include "Camera.h"

Camera::Camera(int nwidth, int nheight) {

    cameraMatrix = (cv::Mat_<double>(3,3)
            << 6.6679383028483210e+002, 0., 6.3719671275509222e+002,
            0., 6.6679383028483210e+002, 4.7673899365172701e+002,
            0., 0., 1. );
    distCoeffs = (cv::Mat_<double>(1,5)
            << -3.2269615280392588e-001, 1.3129405244706957e-001, 2.3325702456177653e-004,
            -2.0624376196568662e-004, -2.7547310468566403e-002);

    dimensions = Coordinate(nwidth,nheight);

    capture.open(0);

    if (!capture.isOpened()) {
        printf("OPENCV Capture failure!\n");
        exit(-123);
    }


    capture.set(CV_CAP_PROP_FRAME_WIDTH, nwidth);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, nheight);

    //capture.read(currentView);
    cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(),
                                cameraMatrix, cv::Size(nwidth,nheight), CV_32FC1,map1, map2);

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
    //cv::Mat undistortedFrame;// = currentView.clone();
    //bitwise_not(currentView, currentView);
    remap(currentView, undistortedFrame, map1, map2, cv::INTER_LINEAR);
    return undistortedFrame;
    //return currentView;
}