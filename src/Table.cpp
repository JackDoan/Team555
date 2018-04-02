//
// Created by jad140230 on 1/21/2018.
//

#include <opencv/cv.h>
#include <opencv/cv.hpp>
#include "../inc/Camera.h"
#include "../inc/Table.h"
#include <vector>


//using namespace cv;


void Table::annotate(cv::Mat frameGrabbed) {
    // Draw marks on image and center
    Camera& camera = Camera::getInstance();
    cv::Scalar color = cv::Scalar(100, 255, 200);

    cv::line(frameGrabbed,
             cvPoint(camera.dimensions.x / 4 - 5, camera.dimensions.y / 4),
             cvPoint(camera.dimensions.x / 4 + 5, camera.dimensions.y / 4),
             color, 1);
    cv::line(frameGrabbed,
             cvPoint(camera.dimensions.x / 4, camera.dimensions.y / 4 - 5),
             cvPoint(camera.dimensions.x / 4, camera.dimensions.y / 4 + 5),
             color, 1);

    //Draw table limits
    //cv::line(frameGrabbed, cvPoint(table_pixx1,table_pixy1), cvPoint(table_pixx2,table_pixy2), cvScalar(100,255,200), 1);
    //cv::line(frameGrabbed, cvPoint(table_pixx2,table_pixy2), cvPoint(table_pixx3,table_pixy3), cvScalar(100,255,200), 1);
    //cv::line(frameGrabbed, cvPoint(table_pixx3,table_pixy3), cvPoint(table_pixx4,table_pixy4), cvScalar(100,255,200), 1);
    //cv::line(frameGrabbed, cvPoint(table_pixx4,table_pixy4), cvPoint(table_pixx1,table_pixy1), cvScalar(100,255,200), 1);

//    cv::line(frameGrabbed, cvPoint(min.x, min.y), cvPoint(min.x + 20, min.y), color, 1);
//    cv::line(frameGrabbed, cvPoint(min.x, max.y), cvPoint(min.x + 20, max.y), color, 1);
//    cv::line(frameGrabbed, cvPoint(max.x - 20, min.y), cvPoint(max.x, min.y), color, 1);
//    cv::line(frameGrabbed, cvPoint(max.x - 20, max.y), cvPoint(max.x, max.y), color, 1);
//    cv::line(frameGrabbed, cvPoint(min.x, min.y), cvPoint(min.x, min.y + 20), color, 1);
//    cv::line(frameGrabbed, cvPoint(max.x, min.y), cvPoint(max.x, min.y + 20), color, 1);
//    cv::line(frameGrabbed, cvPoint(min.x, max.y - 20), cvPoint(min.x, max.y), color, 1);
//    cv::line(frameGrabbed, cvPoint(max.x, max.y - 20), cvPoint(max.x, max.y), color, 1);

}

Table::~Table() = default;
const cv::Point_<int> Table::home = cv::Point_<int>(1150,360);
void Table::setLimits(std::vector<cv::Point_<int>> sortedX, std::vector<cv::Point_<int>> sortedY) {
    min.x = sortedX[0].x; printf("Setting min X: %d\n", min.x);
    max.x = sortedX[3].x; printf("Setting max X: %d\n", max.x);
    min.y = sortedY[0].y; printf("Setting min Y: %d\n", min.y);
    max.y = sortedY[3].y; printf("Setting max Y: %d\n", max.y);

    //todo make these ratios
    motionLimitMin.y = min.y + 80;
    motionLimitMax.y = max.y - 80;
    motionLimitMin.x = min.x + 680;
    motionLimitMax.x = max.x - 55;
    motionLimit = cv::Rect_<int>(motionLimitMin,motionLimitMax);

}

cv::Point_<int> Table::pixelsToSteps(cv::Point_<int> pixels) {
    return 4*(pixels - home);
}

cv::Point_<int> Table::stepsToPixels(cv::Point_<int> steps) {
    return (steps/4) + home;
}