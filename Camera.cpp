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
            << 6.8876400960941680e+002, 0., 6.2643917219420166e+002, 0.,
            6.8876400960941680e+002, 3.5846724209181622e+002, 0., 0., 1.);
    distCoeffs = (cv::Mat_<double>(1,5)
            <<  -3.0194998279023200e-001, 1.0731515298292489e-001,
            1.8080799428778758e-004, -1.2116565979220450e-003,
            -1.7769273735972861e-002);

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

cv::Mat Camera::getUndistortedFrame() {
    capture.read(currentView);
    //undistortedFrame = currentView.clone();
    //cv::Mat undistortedFrame;// = currentView.clone();
    //bitwise_not(currentView, currentView);
    remap(currentView, undistortedFrame, map1, map2, cv::INTER_LINEAR);
    return undistortedFrame;
    //return currentView;
}

cv::Mat Camera::getFrame() {
    capture.read(currentView);
    return currentView;
    //return currentView;
}