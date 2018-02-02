//
// Created by jad140230 on 1/21/2018.
//

#include <opencv2/videoio.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/imgproc.hpp>
#include "Camera.h"

Camera::Camera(int nwidth, int nheight) {



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
    cv::undistort(currentView, undistortedFrame, cameraMatrix, distCoeffs, cameraMatrix);
    return undistortedFrame;
}