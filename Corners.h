//
// Created by mikel on 2/23/2018.
//

#ifndef TEAM555_CORNERS_H
#define TEAM555_CORNERS_H

#include "Camera.h"
#include "helpers.h"
#include "puck.h"
#include "Table.h"

#include <opencv2/opencv.hpp>
#include <vector>

#define CAM_PIX_WIDTH 1280
#define CAM_PIX_HEIGHT 720


class Corners {
    std::vector<cv::Point_<int>> corners;
    std::vector<cv::Point_<int>> tempCorners;
    std::vector<cv::Point_<int>> CalibratedCorners;
    std::vector<cv::Point_<int>> offsets{ {40, 40}, {-40, 40}, {40, -40}, {-40, -40}};
public:
    void drawSquare(cv::Mat previewSmall, std::vector<cv::Point_<int>> cornersVector, std::vector<cv::Point_<int>> offsetsVector);
    void drawLabels(cv::Mat previewSmall, std::vector<cv::Point_<int>> cornersVector);
    void setCorners(std::vector<cv::Point_<int>> cornersVector);
    std::vector<cv::Point_<int>> getCorners();
    void setOffsets(std::vector<cv::Point_<int>>);
    std::vector<cv::Point_<int>> getOffsets();
    void calibrateCorners(cv::Mat frame, cv::Mat previewSmall, Table table, Puck puck);
    //std::vector<cv::Point_<int>> calibrateCorners(cv::Mat frame, cv::Mat previewSmall, Table table, Puck puck);
    void setCalibratedCorners();
    std::vector<cv::Point_<int>> getCalibratedCorners();
    Corners();
    ~Corners();
};

#endif //TEAM555_CORNERS_H
