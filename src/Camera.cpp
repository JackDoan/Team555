//
// Created by jad140230 on 1/21/2018.
//

#include <opencv2/videoio.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/imgproc.hpp>
#include <opencv/cv.hpp>
#include "../inc/Camera.h"
#include "../inc/Settings.h"

#include <ctime>

#include <fcntl.h>


Camera::Camera(int nwidth, int nheight) {

    cameraMatrix = (cv::Mat_<double>(3,3)
            << 6.8876400960941680e+002, 0., 6.2643917219420166e+002, 0.,
            6.8876400960941680e+002, 3.5846724209181622e+002, 0., 0., 1.);
    distCoeffs = (cv::Mat_<double>(1,5)
            <<  -3.0194998279023200e-001, 1.0731515298292489e-001,
            1.8080799428778758e-004, -1.2116565979220450e-003,
            -1.7769273735972861e-002);

    dimensions = {nwidth,nheight};

#ifdef WINDOWS
    capture.open(0);
#else

//gst-launch-1.0 tcamsrc ! video/x-bayer,format=grbg,width=1280,height=720,fps=80/1 ! tcamwhitebalance ! bayer2rgb ! videobalance saturation=2.0 ! videobalance saturation=2.0 ! videoconvert ! xvimagesink

//    auto fd = open("/dev/bus/", O_WRONLY);
//    ioctl(fd, USBDEVFS_RESET, 0);
//    close(fd);
//
//

    system("tcam-ctrl -p -s \"Exposure=4000\" 42614274");
    system("tcam-ctrl -p -s \"Gain=96\" 42614274");
    system("tcam-ctrl -p -s \"Brightness=50\" 42614274"); //auto=false red=110 green=105 blue=200
    cv::waitKey(4000);
    capture.open("tcamsrc ! video/x-bayer,format=grbg,width=1280,height=720,fps=80/1 ! tcamwhitebalance auto=false red=90 green=90 blue=200 ! bayer2rgb ! video/x-raw,format=RGBx ! videoconvert ! appsink" );
    /*autovideosink*/
    //capture.open("tcambin ! video/x-raw,format=RGBx,width=1280,height=720,framerate=80/1 ! videobalance saturation=2.0 ! videobalance saturation=2.0 ! videoconvert ! appsink" );
                 /*autovideosink*/
#endif
    if (!capture.isOpened()) {
        printf("OPENCV Capture failure!\n");
        exit(-123);
    }

#ifdef WINDOWS
    capture.set(CV_CAP_PROP_FRAME_WIDTH, nwidth);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, nheight);
#endif

    cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(),
                                cameraMatrix, cv::Size(nwidth,nheight), CV_32FC1,map1, map2);


}


Camera::~Camera() {
    capture.release();
}




cv::Mat Camera::getFrame() {
    capture.read(currentView);
    remap(currentView, currentView, map1, map2, cv::INTER_LINEAR);
    cv::cvtColor(currentView, currentView, cv::COLOR_RGB2HSV);

    //printf("%3.3f, %3.3f, %3.3f\n", difftime(e, s)/CLOCKS_PER_SEC,difftime(b, e)/CLOCKS_PER_SEC, difftime(x, b)/CLOCKS_PER_SEC);
    return currentView;
    //return undistortedFrame;
}

//cv::ogl::Texture2D Camera::getGLFrame() {
//    capture.read(currentView);
//    cv::cvtColor(currentView, hsv, cv::COLOR_RGB2HSV);
//    if (Settings::undistort) {
//        remap(hsv, undistortedFrame, map1, map2, cv::INTER_LINEAR);
//    }
//    return undistortedFrame;
//}


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

double Camera::getFrameRate() {
    return capture.get(CV_CAP_PROP_FPS);
}

// TODO: Create functions that adjust gain and exposure