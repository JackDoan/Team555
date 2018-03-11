//
// Created by jes130330 on 3/9/2018.
//

#include "Idle.h"
#include <opencv2/core.hpp>
#include <opencv2/core/opengl.hpp>
#include <opencv2/highgui.hpp>
#include <opencv/cv.hpp>
//#include <opencv2/imgproc.h>


Idle::Idle(){

}

void Idle::Idle_Process(){
    //Idle Process running.
        //cv::VideoCapture cap(0); // open the default camera
    cv::VideoCapture cap("C:/AirHockeyRobot/drop.avi"); // open the video file
        if(!cap.isOpened())  // check if we succeeded
            printf("Nope!");

        else cv::namedWindow("Video",1);
        for(;;)
        {
            cv::Mat frame;
            cap >> frame; // get a new frame from camera
            cv::imshow("Video", frame);
            if(cv::waitKey(30) >= 0) break;
        }
// the camera will be deinitialized automatically in VideoCapture destructor

};