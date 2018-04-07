//
// Created by mikel on 2/23/2018.
//

#ifndef TEAM555_CORNERS_H
#define TEAM555_CORNERS_H

#include "Camera.h"
#include "helpers.h"
#include "Puck.h"
#include "Table.h"

#include <opencv2/opencv.hpp>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>



#define CAM_PIX_WIDTH 1280
#define CAM_PIX_HEIGHT 720


class Corners {
    std::vector<cv::Point_<int>> corners;
    std::vector<cv::Point_<int>> tempCorners;
    std::vector<cv::Point_<int>> CalibratedCorners;
//    std::vector<cv::Point_<int>> offsets{{40, 40}, {-40, 40}, {40, -40}, {-40, -40}};
    std::vector<cv::Point_<int>> offsets{{40, 40}, {-40, 40}, {-40, -40}, {40, -40}};




    std::vector<cv::Point_<int>> tempXa;
    std::vector<cv::Point_<int>> tempYa;
    std::vector<cv::Point_<int>> fcorners;


public:
    std::vector<cv::Point_<int>> sortedX;
    std::vector<cv::Point_<int>> sortedY;
//    static std::vector<cv::Point_<int>> corners;
//    static std::vector<cv::Point_<int>> tempCorners;
//    static std::vector<cv::Point_<int>> CalibratedCorners;
//    static std::vector<cv::Point_<int>> offsets{ {40, 40}, {-40, 40}, {40, -40}, {-40, -40}};
    void drawSquareOld(cv::Mat previewSmall, std::vector<cv::Point_<int>> cornersVector, std::vector<cv::Point_<int>> offsetsVector);
    void drawSquareNew(cv::Mat previewSmall, std::vector<cv::Point_<int>> calibratedVector);
    void drawLabels(cv::Mat previewSmall, std::vector<cv::Point_<int>> cornersVector);
    void setCorners(std::vector<cv::Point_<int>> cornersVector);
    std::vector<cv::Point_<int>> getCorners();
    void setOffsets(std::vector<cv::Point_<int>>);
    std::vector<cv::Point_<int>> getOffsets();
    void calibrateCorners(cv::Mat frame, cv::Mat previewSmall, Table table, Puck puck);
    //std::vector<cv::Point_<int>> calibrateCorners(cv::Mat frame, cv::Mat previewSmall, Table table, Puck puck);
    void setCalibratedCorners(std::vector<cv::Point_<int>> calibratedVector);
    std::vector<cv::Point_<int>> getCalibratedCorners();
    std::vector<cv::Point_<int>> getSortedX(std::vector<cv::Point_<int>> calibrated);
    std::vector<cv::Point_<int>> getSortedY(std::vector<cv::Point_<int>> calibrated);
    Corners() {
        Corners(false);
    }
    explicit Corners(bool calibrate);
    ~Corners();
};

#endif //TEAM555_CORNERS_H
