//
// Created by jes130330 on 3/9/2018.
//

#include "inc/Idle.h"
#include <opencv2/core.hpp>
#include <opencv2/core/opengl.hpp>
#include <opencv2/highgui.hpp>
#include <opencv/cv.hpp>
#include "opencv2/imgproc.hpp"


Idle::Idle(){

}

void Idle::Idle_Process() {
    int key = 0;
    char str[200];
    CvFont font;
//    //Idle Process running.
//        //cv::VideoCapture cap(0); // open the default camera

    //open last recorded video

    //CvCapture *cap = cvCaptureFromAVI("C:/AirHockeyRobot/cmake-build-debug/output.avi");

    CvCapture *cap = cvCaptureFromAVI("C:/AirHockeyRobot/Moon.avi"); // open the video file
    IplImage *frame = cvQueryFrame(cap);
    if (!cap) {
        printf("Cannot open AVI!\n");
    }
    int fps = (int) cvGetCaptureProperty(cap, CV_CAP_PROP_FPS);
    cvSetCaptureProperty(cap, CV_CAP_PROP_POS_AVI_RATIO, 0);
      cvNamedWindow("Video", CV_WINDOW_NORMAL);                           //For Fullscreen
    //cvNamedWindow("Video", CV_WINDOW_AUTOSIZE);                         //For autosize window
    cvSetWindowProperty("Video", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN); //For Fullscreen

    while (key != 'x') {
        frame = cvQueryFrame(cap);
        if (!frame)
            cvSetCaptureProperty(cap, CV_CAP_PROP_POS_AVI_RATIO, 0);

        fps = (int) cvGetCaptureProperty(cap, CV_CAP_PROP_FPS);
        sprintf(str, "Average FPS = %d   -   Press 'x' to Exit   -   [This will be informative slideshow]",fps);

        cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, 0.5, 0.5, 0, 2);
        cvPutText(frame, str, cv::Point(30,30), &font, cv::Scalar(0, 0,255,255));
        cvShowImage("Video", frame);
        key = cvWaitKey(1000 / fps);
        printf("FPS: %d\n", fps);
    }
    cvDestroyAllWindows();
    cvReleaseCapture(&cap);
}

//
//    //open last recorded video
//    //cv::VideoCapture cap("C:/AirHockeyRobot/cmake-build-debug/output.avi");
//    int frame_counter = 0;
//    if(!cap.isOpened())  // check if we succeeded
//            printf("Nope!");
//
//        else {
//            //TODO: Currently crashes after a minute of running.
//            //cv::namedWindow("Video", CV_WINDOW_NORMAL);
//            //cv::setWindowProperty("Video", CV_WND_PROP_FULLSCREEN, 1);
//
//        //cv::setWindowProperty("Video", CV_CAP_PROP_POS_AVI_RATIO, 0);
//        //cv::setWindowProperty("Video", CV_CAP_PROP_POS_AVI_RATIO, 0);
//
//        //cv::setWindowProperty("Video", CV_CAP_PROP_FPS, 0);
//
//            cv::namedWindow("Video", 1);                  //NOT FULLSCREEN BUT NOT SHIT RES.
//            //cv::setWindowProperty("Video", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
//            //cap.set(CV_CAP_PROP_POS_AVI_RATIO, 0);
//
//        }
//        for(;;)
//        {
//            cv::Mat frame;
//
//            //frame_counter += 1;
//            //if (frame_counter == cap.get(CV_CAP_PROP_FRAME_COUNT))
//              //frame_counter = 0;
//            //cap.set(CV_CAP_PROP_POS_FRAMES, 0);
//
//            //cap.set(CV_CAP_PROP_POS_AVI_RATIO, 0);
//
//            cap >> frame; // get a new frame from camera
//            cv::imshow("Video", frame);
//            if(cv::waitKey(30) >= 0) break;
//        }
//    cap.release();
//    cv::destroyAllWindows();
// the camera will be deinitialized automatically in VideoCapture destructor

//};