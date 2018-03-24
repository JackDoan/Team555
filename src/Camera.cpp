//
// Created by jad140230 on 1/21/2018.
//

#include <opencv2/videoio.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/imgproc.hpp>
#include <opencv/cv.hpp>
#include "../inc/Camera.h"

#include <time.h>

Camera::Camera(int nwidth, int nheight) {

    cameraMatrix = (cv::Mat_<double>(3,3)
            << 6.8876400960941680e+002, 0., 6.2643917219420166e+002, 0.,
            6.8876400960941680e+002, 3.5846724209181622e+002, 0., 0., 1.);
    distCoeffs = (cv::Mat_<double>(1,5)
            <<  -3.0194998279023200e-001, 1.0731515298292489e-001,
            1.8080799428778758e-004, -1.2116565979220450e-003,
            -1.7769273735972861e-002);

    dimensions = {nwidth,nheight};

    capture.open(0);
    printf("Exposure: %f\n", capture.get(CV_CAP_PROP_EXPOSURE));
    printf("Gain: %f\n", capture.get(CV_CAP_PROP_GAIN));

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

cv::Mat Camera::getUndistortedFrame() {
    // TODO: look at source code for capture.read and see if we can apply undistort to each pixel as its read from the camera to improve performance
    // VideoCapture::read calls retrieve
    auto result = capture.read(currentView);
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
cv::Mat Camera::getHomography(std::vector<cv::Point_<int>> corners,
                               std::vector<cv::Point_<int>> Calibrated_corners){
    std::vector<cv::Point_<int>> pts_src;
    pts_src.emplace_back(cv::Point(corners[3].x,corners[3].y));
    pts_src.emplace_back(cv::Point(corners[2].x,corners[2].y));
    pts_src.emplace_back(cv::Point(corners[0].x,corners[0].y));
    pts_src.emplace_back(cv::Point(corners[1].x,corners[1].y));

    std::vector<cv::Point_<int>> pts_dst;
    pts_dst.emplace_back(cv::Point(Calibrated_corners[3].x,Calibrated_corners[3].y));
    pts_dst.emplace_back(cv::Point(Calibrated_corners[2].x,Calibrated_corners[2].y));
    pts_dst.emplace_back(cv::Point(Calibrated_corners[0].x,Calibrated_corners[0].y));
    pts_dst.emplace_back(cv::Point(Calibrated_corners[1].x,Calibrated_corners[1].y));

    cv::Mat h_transform = cv::findHomography(pts_src, pts_dst);
    //use with warpPerspective(intput img, output img, h_transform, output.size());

    return h_transform;
}

void Camera::close() {
    capture.release();
}


// TODO: Create functions that adjust gain and exposure