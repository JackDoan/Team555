//
// Created by mikel on 2/23/2018.
//
#include "Corners.h"
#include "Camera.h"
#include "Table.h"
#include "puck.h"

//std::vector<cv::Point_<int>> corners;





Corners::Corners() {

}

Corners::~Corners() = default;


// TODO: set the offsets from a config file, right now it defaults to 40px out from each corner in each dimension as defined in the header file
void Corners::calibrateCorners(cv::Mat in, cv::Mat previewSmall, Table table, Puck puck) {
    tempCorners = puck.findPucks(in, table);
    //printf("tempCorners size: %d\n", tempCorners.size());
    if (tempCorners.size() == 4) {
        setCorners(tempCorners);
        drawSquare(previewSmall, getCorners(), getOffsets());
        drawLabels(previewSmall, corners);
    } else {
        drawLabels(previewSmall, corners);
    }
}

void Corners::drawLabels(cv::Mat previewSmall, std::vector<cv::Point_<int>> cornersVector) {
    for (int i = 0; i < cornersVector.size(); i++) {
        switch(i) {
            case 1 : cv::putText(previewSmall, "1", corners[0]/2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                                 2.8, cv::Scalar(255, 0, 255), 2, cv::LINE_8, false);
                printf("Drawing labels: 1\n");
            case 2 : cv::putText(previewSmall, "1", corners[0]/2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                                 2.8, cv::Scalar(255, 0, 255), 2, cv::LINE_8, false);
                     cv::putText(previewSmall, "2", corners[1]/2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                                 2.8, cv::Scalar(255, 0, 0), 2, cv::LINE_8, false);
                printf("Drawing labels: 1 2\n");
            case 3 : cv::putText(previewSmall, "1", corners[0]/2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                                 2.8, cv::Scalar(255, 0, 255), 2, cv::LINE_8, false);
                     cv::putText(previewSmall, "2", corners[1]/2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                                 2.8, cv::Scalar(255, 0, 0), 2, cv::LINE_8, false);
                     cv::putText(previewSmall, "3", corners[2]/2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                                 2.8, cv::Scalar(0, 255, 0), 2, cv::LINE_8, false);
                printf("Drawing labels: 1 2 3\n");
            case 4 : cv::putText(previewSmall, "1", corners[0]/2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                                 2.8, cv::Scalar(255, 0, 255), 2, cv::LINE_8, false);
                     cv::putText(previewSmall, "2", corners[1]/2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                                 2.8, cv::Scalar(255, 0, 0), 2, cv::LINE_8, false);
                     cv::putText(previewSmall, "3", corners[2]/2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                                 2.8, cv::Scalar(0, 255, 0), 2, cv::LINE_8, false);
                     cv::putText(previewSmall, "4", corners[3]/2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                                 2.8, cv::Scalar(0, 0, 255), 2, cv::LINE_8, false);
                printf("Drawing labels: 1 2 3 4\n");
            default: cv::putText(previewSmall, "4", corners[3]/2, cv::FONT_HERSHEY_COMPLEX_SMALL,
                                 2.8, cv::Scalar(255, 255, 255), 2, cv::LINE_8, false);
                printf("defaulted\n");
        }
    }
}

void Corners::drawSquare(cv::Mat previewSmall, std::vector<cv::Point_<int>> cornersVector, std::vector<cv::Point_<int>> offsetsVector) {
    cv::Point_<int> zero; zero.x = cornersVector[0].x + offsetsVector[0].x; zero.y = cornersVector[0].y + offsetsVector[0].y;
    cv::Point_<int> one; one.x = cornersVector[1].x + offsetsVector[1].x; one.y = cornersVector[1].y + offsetsVector[1].y;
    cv::Point_<int> two; two.x = cornersVector[2].x + offsetsVector[2].x; two.y = cornersVector[2].y + offsetsVector[2].y;
    cv::Point_<int> three; three.x = cornersVector[3].x + offsetsVector[3].x; three.y = cornersVector[3].y +offsetsVector[3].y;

    cv::line(previewSmall, zero / 2, one / 2, cv::Scalar(255, 255, 255), 4);
    cv::line(previewSmall, one / 2, three / 2, cv::Scalar(255, 255, 255), 4);
    cv::line(previewSmall, three / 2, two / 2, cv::Scalar(255, 255, 255), 4);
    cv::line(previewSmall, two / 2, zero / 2, cv::Scalar(255, 255, 255), 4);

}

void Corners::setCorners(std::vector<cv::Point_<int>> cornersVector) {
    for (int i = 0; i <= 3; i++) {
        corners[i] = cornersVector[i];
    }
}

std::vector<cv::Point_<int>> Corners::getCorners() {
    return corners;
}

void Corners::setOffsets(std::vector<cv::Point_<int>> offsetsVector) {
    for (int i = 0; i <= 3; i++) {
        offsets[i] = offsetsVector[i];
    }
}

std::vector<cv::Point_<int>> Corners::getOffsets() {
    return offsets;
}